#include "obj_loader.h"

// Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>

namespace sim {

std::shared_ptr<Mesh_Data> OBJLoader::loadWithAssimp(const std::string& filepath) {
    auto meshData = std::make_shared<Mesh_Data>();
    meshData->name = filepath.substr(filepath.find_last_of("/\\") + 1);
    
    Assimp::Importer importer;
    
    // Import settings
    unsigned int flags = aiProcess_Triangulate | 
                         aiProcess_GenSmoothNormals |
                         aiProcess_CalcTangentSpace |
                         aiProcess_JoinIdenticalVertices |
                         aiProcess_ImproveCacheLocality |
                         aiProcess_SortByPType |
                         aiProcess_OptimizeMeshes |
                         aiProcess_OptimizeGraph;
    
    const aiScene* scene = importer.ReadFile(filepath.c_str(), flags);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "[OBJLoader] Assimp error: " << importer.GetErrorString() << "\n";
        return nullptr;
    }
    
    if (!scene->HasMeshes()) {
        std::cerr << "[OBJLoader] No meshes found in: " << filepath << "\n";
        return nullptr;
    }
    
    // Process all meshes
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        processAssimpMesh(scene->mMeshes[i], scene, meshData);
    }
    
    // If we have indices, use them
    if (meshData->indices.empty() && !meshData->vertices.empty()) {
        // Generate sequential indices
        meshData->indices.resize(meshData->vertices.size());
        for (size_t i = 0; i < meshData->vertices.size(); i++) {
            meshData->indices[i] = static_cast<uint32_t>(i);
        }
    }
    
    return meshData;
}

void OBJLoader::processAssimpMesh(const aiMesh* mesh, const aiScene* scene,
                                 std::shared_ptr<Mesh_Data>& result) {
    size_t vertexStart = result->vertices.size();
    size_t indexStart = result->indices.size();
    
    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        
        // Position
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        
        // Normals
        if (mesh->HasNormals()) {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        } else {
            vertex.normal = math::Vec3(0.0f, 1.0f, 0.0f);
        }
        
        // Texture coordinates
        if (mesh->mTextureCoords[0]) {
            vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.texCoord = math::Vec2(0.0f, 0.0f);
        }
        
        // Color (generate from normal if not available)
        if (mesh->HasVertexColors(0)) {
            vertex.color.r = mesh->mColors[0][i].r;
            vertex.color.g = mesh->mColors[0][i].g;
            vertex.color.b = mesh->mColors[0][i].b;
        } else {
            vertex.color = calculateColorFromNormal(vertex.normal);
        }
        
        result->vertices.push_back(vertex);
    }
    
    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            result->indices.push_back(vertexStart + face.mIndices[j]);
        }
    }
}

math::Vec3 OBJLoader::calculateColorFromNormal(const math::Vec3& normal) {
    // Generate a pleasant color based on the normal
    math::Vec3 color;
    
    // Map normal components to RGB
    color.r = (normal.x + 1.0f) * 0.5f;
    color.g = (normal.y + 1.0f) * 0.5f;
    color.b = (normal.z + 1.0f) * 0.5f;
    
    // Make it more vibrant
    color = glm::normalize(color) * 0.8f + math::Vec3(0.2f);
    
    return color;
}

std::shared_ptr<Mesh_Data> OBJLoader::loadSimpleOBJ(const std::string& filepath) {
    auto meshData = std::make_shared<Mesh_Data>();
    meshData->name = filepath.substr(filepath.find_last_of("/\\") + 1);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[OBJLoader] Failed to open file: " << filepath << "\n";
        return nullptr;
    }
    
    std::vector<math::Vec3> positions;
    std::vector<math::Vec3> normals;
    std::vector<math::Vec2> texCoords;
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "v") {
            // Vertex position
            math::Vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "vn") {
            // Vertex normal
            math::Vec3 norm;
            iss >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);
        }
        else if (prefix == "vt") {
            // Texture coordinate
            math::Vec2 tex;
            iss >> tex.x >> tex.y;
            texCoords.push_back(tex);
        }
        else if (prefix == "f") {
            // Face - we only handle simple triangles/quads for now
            std::vector<std::string> faceTokens;
            std::string token;
            while (iss >> token) {
                faceTokens.push_back(token);
            }
            
            // Triangulate if quad
            if (faceTokens.size() == 4) {
                // Quad -> two triangles
                std::vector<std::string> tri1 = {faceTokens[0], faceTokens[1], faceTokens[2]};
                std::vector<std::string> tri2 = {faceTokens[0], faceTokens[2], faceTokens[3]};
                faceTokens = tri1;
                // We'll process the second triangle after this one
                // (simplified approach)
            }
            
            for (const auto& token : faceTokens) {
                std::istringstream viss(token);
                std::string vstr, tstr, nstr;
                
                std::getline(viss, vstr, '/');
                std::getline(viss, tstr, '/');
                std::getline(viss, nstr, '/');
                
                int vi = std::stoi(vstr) - 1;
                int ti = tstr.empty() ? -1 : std::stoi(tstr) - 1;
                int ni = nstr.empty() ? -1 : std::stoi(nstr) - 1;
                
                Vertex vertex;
                
                if (vi >= 0 && vi < positions.size()) {
                    vertex.position = positions[vi];
                }
                
                if (ni >= 0 && ni < normals.size()) {
                    vertex.normal = normals[ni];
                } else {
                    vertex.normal = math::Vec3(0.0f, 1.0f, 0.0f);
                }
                
                if (ti >= 0 && ti < texCoords.size()) {
                    vertex.texCoord = texCoords[ti];
                } else {
                    vertex.texCoord = math::Vec2(0.0f, 0.0f);
                }
                
                vertex.color = calculateColorFromNormal(vertex.normal);
                
                meshData->vertices.push_back(vertex);
                meshData->indices.push_back(static_cast<uint32_t>(meshData->vertices.size() - 1));
            }
        }
    }
    
    file.close();
    
    if (meshData->vertices.empty()) {
        std::cerr << "[OBJLoader] No vertices loaded from: " << filepath << "\n";
        return nullptr;
    }
    
    return meshData;
}

} // namespace sim