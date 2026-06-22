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






//  Public API //

Scene SceneSerializer::load(const std::string& filename, const std::string& dir) {
    Scene scene;
    std::string path = dir + "/" + filename;

    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "[SceneSerializer] File not found: " << path << "\n";
        return scene;   // root == nullptr signals failure
    }

    // json j;
    // in >> j;

    // scene.name = j["name"].get<std::string>();
    // scene.root = deserializeNode(j["nodes"]);

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