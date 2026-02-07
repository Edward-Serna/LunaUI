#pragma once

#include "math_utils.h"

#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace sim {

struct SceneNode;
using SceneNodePtr = std::shared_ptr<SceneNode>;

enum class NodeType {
    Empty,      // pure transform node / grouping
    Camera,
    Mesh        // geometry (cube, sphere, …)
};

struct Transform {
    math::Vec3  position { 0.0f, 0.0f, 0.0f };
    math::Quat  rotation { 1.0f, 0.0f, 0.0f, 0.0f };  // identity
    math::Vec3  scale    { 1.0f, 1.0f, 1.0f };

    /// Compose into a 4×4 world-space matrix (relative to parent).
    math::Mat4 toMatrix() const {
        math::Mat4 m(1.0f);
        m = glm::translate(m, position);
        m = m * glm::mat4_cast(rotation);
        m = glm::scale(m, scale);
        return m;
    }
};

struct CameraData {
    float fovDeg    = 60.0f;
    float nearPlane = 0.1f;
    float farPlane  = 1000.0f;
};

enum class MeshType { Cube, Sphere };

struct MeshData {
    MeshType type = MeshType::Cube;
    // future: material index, UV options, etc.
};

struct OrbitData {
    float   radius      = 5.0f;   // distance from target
    float   azimuth     = 0.0f;   // horizontal angle (radians)
    float   elevation   = 0.4f;   // vertical angle (radians), clamped later
    math::Vec3 target   { 0.0f, 0.0f, 0.0f };  // point we orbit around
    float   speed       = 0.5f;   // radians / second (auto-orbit)
    bool    autoOrbit   = true;
};

struct SceneNode {
    // Identity
    std::string name;
    NodeType    type = NodeType::Empty;

    // Transform (local)
    Transform   transform;

    // Type-specific payload
    std::optional<CameraData>  camera;
    std::optional<MeshData>    mesh;
    std::optional<OrbitData>   orbit;   // only on Camera nodes

    // Hierarchy
    SceneNodePtr              parent;
    std::vector<SceneNodePtr> children;

    // ── helpers ─────────────────────────────────────────────
    void addChild(SceneNodePtr child) {
        // Parent back-pointer is set manually by the caller if needed
        // We don't use enable_shared_from_this to keep the class simple
        children.push_back(child);
    }

    /// Compute the world-space matrix by walking up the parent chain.
    math::Mat4 worldMatrix() const {
        math::Mat4 m = transform.toMatrix();
        if (parent) {
            m = parent->worldMatrix() * m;
        }
        return m;
    }

    /// Find the first child with the given name (non-recursive).
    SceneNodePtr findChild(const std::string& childName) const {
        for (auto& c : children) {
            if (c->name == childName) return c;
        }
        return nullptr;
    }

private:
    // We can't call shared_from_this() unless the node is already
    // owned by a shared_ptr. This helper is safe to call from addChild
    // because in practice we always construct nodes via make_shared.
    // For safety we fall back to nullptr if something goes wrong.
    static SceneNodePtr shared_from_this_safe() {
        // We intentionally do NOT inherit from enable_shared_from_this
        // to keep things simple. The parent pointer is set externally.
        // This method exists only as a placeholder comment reminder.
        return nullptr; // parent is set by the caller of addChild
    }
};

// ─── Factory helpers ──────────────────────────────────────────

inline SceneNodePtr makeEmpty(const std::string& name) {
    auto node        = std::make_shared<SceneNode>();
    node->name       = name;
    node->type       = NodeType::Empty;
    return node;
}

inline SceneNodePtr makeMesh(const std::string& name, MeshType meshType) {
    auto node        = std::make_shared<SceneNode>();
    node->name       = name;
    node->type       = NodeType::Mesh;
    node->mesh       = MeshData{ meshType };
    return node;
}

inline SceneNodePtr makeOrbitCamera(const std::string& name, const OrbitData& orbit, const CameraData& cam = {}) {
    auto node        = std::make_shared<SceneNode>();
    node->name       = name;
    node->type       = NodeType::Camera;
    node->camera     = cam;
    node->orbit      = orbit;
    return node;
}

} // namespace sim
