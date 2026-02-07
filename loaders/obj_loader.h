#pragma once

#include "asset_loader.h"

// Assimp forward declarations
struct aiScene;
struct aiMesh;
struct aiNode;
struct aiMaterial;

namespace sim {

class OBJLoader {
public:
    /// Load mesh using Assimp library (supports multiple formats: OBJ, FBX, etc.)
    static std::shared_ptr<Mesh_Data> loadWithAssimp(const std::string& filepath);
    
    /// Simple OBJ parser (for basic OBJ files without dependencies)
    static std::shared_ptr<Mesh_Data> loadSimpleOBJ(const std::string& filepath);
    
private:
    // Process Assimp mesh
    static void processAssimpMesh(const aiMesh* mesh, const aiScene* scene, 
                                  std::shared_ptr<Mesh_Data>& result);
    
    // Process Assimp node recursively
    static void processAssimpNode(const aiNode* node, const aiScene* scene,
                                  std::shared_ptr<Mesh_Data>& result);
                                  
    // Calculate vertex color based on normal
    static math::Vec3 calculateColorFromNormal(const math::Vec3& normal);
};

} // namespace sim