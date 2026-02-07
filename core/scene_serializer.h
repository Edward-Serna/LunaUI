#pragma once

#include "scene_node.h"

#include <string>
#include <vector>
#include <memory>

namespace sim {

    /// A complete scene: root node + metadata.
    struct Scene {
        std::string              name = "default";
        SceneNodePtr             root;
        std::vector<SceneNodePtr> cameras;   // convenience cache, populated on load
    };

    /// Serialisation / deserialisation of a Scene to/from a JSON file.
    ///
    /// File layout (scenes/<name>.json):
    ///   {
    ///     "name"    : "default",
    ///     "nodes"   : [ … recursive tree … ]
    ///   }
    ///
    /// Each node object:
    ///   {
    ///     "name"      : "Cube",
    ///     "type"      : "Mesh",           // "Empty" | "Camera" | "Mesh"
    ///     "transform" : { "position": [x,y,z], "rotation": [x,y,z,w], "scale": [x,y,z] },
    ///     "camera"    : { … } or null,
    ///     "mesh"      : { … } or null,
    ///     "orbit"     : { … } or null,
    ///     "children"  : [ … ]
    ///   }

    class SceneSerializer {
    public:
        /// Save the scene to <dir>/<scene.name>.json.
        /// Returns true on success.
        static bool save(const Scene& scene, const std::string& dir = "scenes");

        /// Load a scene from <dir>/<filename>.
        /// Returns an empty Scene on failure (check scene.root == nullptr).
        static Scene load(const std::string& filename, const std::string& dir = "scenes");

        /// Return the full path that would be used for a given scene name.
        static std::string scenePath(const std::string& name, const std::string& dir = "scenes");

    private:
    };

} // namespace sim