#pragma once

#include "math_utils.h"
#include "scene_node.h"

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace sim {

    struct Vertex {
        math::Vec3 position;
        math::Vec3 normal;
        math::Vec2 texCoord;
        math::Vec3 color;
    
        Vertex() = default;
        Vertex(const math::Vec3& pos, const math::Vec3& norm, const math::Vec2& uv, const math::Vec3& col)
            : position(pos), normal(norm), texCoord(uv), color(col) {}
    };

    struct Mesh_Data {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::string name;
    
        // Bounding box for culling
        math::Vec3 minBounds;
        math::Vec3 maxBounds;
    
        // Center and radius for sphere culling
        math::Vec3 center;
        float radius;
    
        // Calculate bounds after loading
        void calculateBounds();
    };

    class AssetLoader {
    public:
        /// Singleton instance
        static AssetLoader& getInstance();
    
        /// Load a mesh from OBJ file
        std::shared_ptr<Mesh_Data> loadMesh(const std::string& filepath);
    
        /// Load a mesh from OBJ and create a SceneNode with it
        SceneNodePtr loadMeshNode(const std::string& filepath, const std::string& nodeName);
    
        /// Get a previously loaded mesh (returns nullptr if not found)
        std::shared_ptr<Mesh_Data> getMesh(const std::string& filepath);
    
        /// Clear all cached meshes
        void clearCache();
    
        /// Check if mesh is already loaded
        bool isLoaded(const std::string& filepath) const;
    
    private:
        AssetLoader() = default;
        ~AssetLoader() = default;
    
        // Disable copying
        AssetLoader(const AssetLoader&) = delete;
        AssetLoader& operator=(const AssetLoader&) = delete;
    
        // Cache for loaded meshes
        std::unordered_map<std::string, std::shared_ptr<Mesh_Data>> m_meshCache;
    
        // Helper function to parse OBJ file
        std::shared_ptr<Mesh_Data> parseOBJ(const std::string& filepath);
    };

} // namespace sim