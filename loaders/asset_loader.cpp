#include "asset_loader.h"
#include "obj_loader.h"

#include <iostream>
#include <algorithm>
#include <cctype>

namespace sim {

void Mesh_Data::calculateBounds() {
    if (vertices.empty()) {
        minBounds = math::Vec3(0.0f);
        maxBounds = math::Vec3(0.0f);
        center = math::Vec3(0.0f);
        radius = 0.0f;
        return;
    }

    // Initialize bounds with first vertex
    minBounds = vertices[0].position;
    maxBounds = vertices[0].position;

    // Find min and max bounds
    for (const auto& vertex : vertices) {
        minBounds.x = std::min(minBounds.x, vertex.position.x);
        minBounds.y = std::min(minBounds.y, vertex.position.y);
        minBounds.z = std::min(minBounds.z, vertex.position.z);

        maxBounds.x = std::max(maxBounds.x, vertex.position.x);
        maxBounds.y = std::max(maxBounds.y, vertex.position.y);
        maxBounds.z = std::max(maxBounds.z, vertex.position.z);
    }

    // Calculate center and radius
    center = (minBounds + maxBounds) * 0.5f;

    float maxDistanceSquared = 0.0f;
    for (const auto& vertex : vertices) {
        math::Vec3 diff = vertex.position - center;
        float distSquared = glm::dot(diff, diff);
        maxDistanceSquared = std::max(maxDistanceSquared, distSquared);
    }

    radius = std::sqrt(maxDistanceSquared);
}

// AssetLoader implementation
AssetLoader& AssetLoader::getInstance() {
    static AssetLoader instance;
    return instance;
}

std::shared_ptr<Mesh_Data> AssetLoader::loadMesh(const std::string &filepath) {
    // Check cache first
    auto it = m_meshCache.find(filepath);
    if (it != m_meshCache.end()) {
        return it->second;
    }

    std::shared_ptr<Mesh_Data> meshData;

    // Check file extension
    std::string extension = filepath.substr(filepath.find_last_of(".") + 1);

    // Convert to lowercase
    std::ranges::transform(extension, extension.begin(),
                           [](unsigned char c) { return std::tolower(c); });

    try {
        if (extension == "obj") {
            // Try using Assimp first (more robust)
            meshData = OBJLoader::loadWithAssimp(filepath);

            // If Assimp fails, fall back to simple parser
            if (!meshData || meshData->vertices.empty()) {
                std::cerr << "[AssetLoader] Assimp failed for " << filepath
                          << ", falling back to simple parser\n";
                meshData = OBJLoader::loadSimpleOBJ(filepath);
            }
        } else {
            std::cerr << "[AssetLoader] Unsupported file format: " << extension << "\n";
            return nullptr;
        }

        if (meshData && !meshData->vertices.empty()) {
            meshData->calculateBounds();
            m_meshCache[filepath] = meshData;
            std::cout << "[AssetLoader] Loaded mesh: " << filepath
                      << " (" << meshData->vertices.size() << " vertices, "
                      << meshData->indices.size() << " indices)\n";
        } else {
            std::cerr << "[AssetLoader] Failed to load mesh: " << filepath << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "[AssetLoader] Error loading " << filepath << ": " << e.what() << "\n";
        return nullptr;
    }

    return meshData;
}

SceneNodePtr AssetLoader::loadMeshNode(const std::string& filepath, const std::string& nodeName) {
    auto meshData = loadMesh(filepath);
    if (!meshData) {
        return nullptr;
    }

    auto node = std::make_shared<SceneNode>();
    node->name = nodeName.empty() ? meshData->name : nodeName;
    node->type = NodeType::Mesh;

    // Create a custom MeshData structure for the scene node
    // (In a full implementation, you'd store the loaded mesh data differently)
    node->mesh = MeshData{ MeshType::Cube }; // Placeholder

    // Store the actual mesh data in a custom field
    // For now, we'll just create a basic cube node

    return node;
}

std::shared_ptr<Mesh_Data> AssetLoader::getMesh(const std::string &filepath) {
    auto it = m_meshCache.find(filepath);
    if (it != m_meshCache.end()) {
        return it->second;
    }
    return nullptr;
}

void AssetLoader::clearCache() {
    m_meshCache.clear();
}

bool AssetLoader::isLoaded(const std::string& filepath) const {
    return m_meshCache.find(filepath) != m_meshCache.end();
}

} // namespace sim