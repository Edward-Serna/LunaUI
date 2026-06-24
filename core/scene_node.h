#pragma once

#include "utility/math.h"
#include <memory>

namespace sim {
    enum class NodeType {
        Empty, // pure transform node / grouping
        Camera,
        Mesh
    };

    struct Transform {
        math::Vec3 position{0.0f, 0.0f, 0.0f};
        math::Quat rotation{1.0f, 0.0f, 0.0f, 0.0f}; // identity
        math::Vec3 scale{1.0f, 1.0f, 1.0f};

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
        float fovDeg = 90.0f;
        float nearPlane = 0.0f;
        float farPlane = 1000.0f;
    };

    struct SceneNode; // Empty
    using SceneNodePtr = std::shared_ptr<SceneNode>;
    struct SceneNode {
        std::string name; // Identity
        NodeType type = NodeType::Empty;

        // Transform (local)
        Transform transform;

        // Type-specific payload
        std::optional<CameraData> camera;

        // Hierarchy
        SceneNodePtr parent;
        std::vector<SceneNodePtr> children;

        void addChild( SceneNodePtr child ) {
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
        SceneNodePtr findChild( const std::string& childName ) const {
            for (auto& c : children) {
                if (c->name == childName) return c;
            }
            return nullptr;
        }

    private:
        // We can't call shared_from_this() unless the node is already
        // owned by a shared_ptr. This helper is safe to call from addChild
        // because in practice we always construct nodes via make_shared.
        // For safety, we fall back to nullptr if something goes wrong.
        static SceneNodePtr shared_from_this_safe() {
            // We intentionally do NOT inherit from enable_shared_from_this
            // to keep things simple. The parent pointer is set externally.
            // This method exists only as a placeholder comment reminder.
            return nullptr; // parent is set by the caller of addChild
        }
    };

    // Factory helpers
    inline SceneNodePtr makeEmpty( const std::string& name ) {
        auto node = std::make_shared<SceneNode>();
        node->name = name;
        node->type = NodeType::Empty;
        return node;
    }

    inline SceneNodePtr makeMesh( const std::string& name ) {
        auto node = std::make_shared<SceneNode>();
        node->name = name;
        node->type = NodeType::Mesh;
        return node;
    }
}
