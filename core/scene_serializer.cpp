#include "scene_serializer.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
using json   = nlohmann::json;

namespace sim {

// ═══════════════════════════════════════════════════════════════
//  Helpers: JSON ↔ GLM types
// ═══════════════════════════════════════════════════════════════

static json vec3ToJson(const math::Vec3& v) {
    return { v.x, v.y, v.z };
}

static math::Vec3 jsonToVec3(const json& j) {
    return { j[0].get<float>(), j[1].get<float>(), j[2].get<float>() };
}

static json quatToJson(const math::Quat& q) {
    // Store as [x, y, z, w] — matches GLM's internal layout.
    return { q.x, q.y, q.z, q.w };
}

static math::Quat jsonToQuat(const json& j) {
    // glm::quat constructor order: (w, x, y, z)
    return math::Quat(j[3].get<float>(), j[0].get<float>(),
                      j[1].get<float>(), j[2].get<float>());
}

// ═══════════════════════════════════════════════════════════════
//  Serialise a single node (recursive)
// ═══════════════════════════════════════════════════════════════

static json serializeNode(const SceneNodePtr& node) {
    json j;

    // ── Identity ─────────────────────────────────────────
    j["name"] = node->name;

    switch (node->type) {
        case NodeType::Empty:  j["type"] = "Empty";  break;
        case NodeType::Camera: j["type"] = "Camera"; break;
        case NodeType::Mesh:   j["type"] = "Mesh";   break;
    }

    // ── Transform ────────────────────────────────────────
    j["transform"] = {
        { "position", vec3ToJson(node->transform.position) },
        { "rotation", quatToJson(node->transform.rotation) },
        { "scale",    vec3ToJson(node->transform.scale)    }
    };

    // ── Camera payload ───────────────────────────────────
    if (node->camera) {
        j["camera"] = {
            { "fovDeg",    node->camera->fovDeg    },
            { "nearPlane", node->camera->nearPlane },
            { "farPlane",  node->camera->farPlane  }
        };
    } else {
        j["camera"] = nullptr;
    }

    // ── Mesh payload ─────────────────────────────────────
    if (node->mesh) {
        std::string meshTypeStr = (node->mesh->type == MeshType::Cube) ? "Cube" : "Sphere";
        j["mesh"] = {
            { "type", meshTypeStr }
        };
    } else {
        j["mesh"] = nullptr;
    }

    // ── Orbit payload ────────────────────────────────────
    if (node->orbit) {
        j["orbit"] = {
            { "radius",    node->orbit->radius    },
            { "azimuth",   node->orbit->azimuth   },
            { "elevation", node->orbit->elevation },
            { "target",    vec3ToJson(node->orbit->target) },
            { "speed",     node->orbit->speed     },
            { "autoOrbit", node->orbit->autoOrbit }
        };
    } else {
        j["orbit"] = nullptr;
    }

    // ── Children (recursive) ─────────────────────────────
    json childArr = json::array();
    for (auto& child : node->children) {
        childArr.push_back(serializeNode(child));
    }
    j["children"] = childArr;

    return j;
}

// ═══════════════════════════════════════════════════════════════
//  Deserialise a single node (recursive)
// ═══════════════════════════════════════════════════════════════

static SceneNodePtr deserializeNode(const json& j) {
    auto node = std::make_shared<SceneNode>();

    // ── Identity ─────────────────────────────────────────
    node->name = j["name"].get<std::string>();

    std::string typeStr = j["type"].get<std::string>();
    if      (typeStr == "Empty")  node->type = NodeType::Empty;
    else if (typeStr == "Camera") node->type = NodeType::Camera;
    else if (typeStr == "Mesh")   node->type = NodeType::Mesh;

    // ── Transform ────────────────────────────────────────
    const auto& t = j["transform"];
    node->transform.position = jsonToVec3(t["position"]);
    node->transform.rotation = jsonToQuat(t["rotation"]);
    node->transform.scale    = jsonToVec3(t["scale"]);

    // ── Camera ───────────────────────────────────────────
    if (!j["camera"].is_null()) {
        const auto& c = j["camera"];
        node->camera = CameraData {
            c["fovDeg"].get<float>(),
            c["nearPlane"].get<float>(),
            c["farPlane"].get<float>()
        };
    }

    // ── Mesh ─────────────────────────────────────────────
    if (!j["mesh"].is_null()) {
        std::string mt = j["mesh"]["type"].get<std::string>();
        node->mesh = MeshData {
            (mt == "Cube") ? MeshType::Cube : MeshType::Sphere
        };
    }

    // ── Orbit ────────────────────────────────────────────
    if (!j["orbit"].is_null()) {
        const auto& o = j["orbit"];
        node->orbit = OrbitData {
            o["radius"].get<float>(),
            o["azimuth"].get<float>(),
            o["elevation"].get<float>(),
            jsonToVec3(o["target"]),
            o["speed"].get<float>(),
            o["autoOrbit"].get<bool>()
        };
    }

    // ── Children (recursive) ─────────────────────────────
    for (const auto& childJson : j["children"]) {
        auto child   = deserializeNode(childJson);
        child->parent = node;
        node->children.push_back(child);
    }

    return node;
}

// ═══════════════════════════════════════════════════════════════
//  Public API
// ═══════════════════════════════════════════════════════════════

std::string SceneSerializer::scenePath(const std::string& name, const std::string& dir) {
    return dir + "/" + name + ".json";
}

bool SceneSerializer::save(const Scene& scene, const std::string& dir) {
    // Ensure the directory exists.
    fs::create_directories(dir);

    std::string path = scenePath(scene.name, dir);

    json j;
    j["name"]  = scene.name;
    j["nodes"] = serializeNode(scene.root);

    std::ofstream out(path);
    if (!out.is_open()) {
        std::cerr << "[SceneSerializer] Failed to open file for writing: " << path << "\n";
        return false;
    }

    out << j.dump(2);   // pretty-print with 2-space indent
    std::cerr << "[SceneSerializer] Saved scene to " << path << "\n";
    return true;
}

Scene SceneSerializer::load(const std::string& filename, const std::string& dir) {
    Scene scene;
    std::string path = dir + "/" + filename;

    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "[SceneSerializer] File not found: " << path << "\n";
        return scene;   // root == nullptr signals failure
    }

    json j;
    in >> j;

    scene.name = j["name"].get<std::string>();
    scene.root = deserializeNode(j["nodes"]);

    // ── Populate cameras cache ───────────────────────────
    // Simple recursive lambda to collect all Camera nodes.
    std::function<void(const SceneNodePtr&)> collectCameras = [&](const SceneNodePtr& node) {
        if (!node) return;
        if (node->type == NodeType::Camera) scene.cameras.push_back(node);
        for (auto& child : node->children) collectCameras(child);
    };
    collectCameras(scene.root);

    std::cerr << "[SceneSerializer] Loaded scene '" << scene.name
              << "' with " << scene.cameras.size() << " camera(s)\n";
    return scene;
}

} // namespace sim