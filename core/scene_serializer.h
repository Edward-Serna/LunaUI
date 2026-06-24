#pragma once

#include "scene_node.h"
#include <string>
#include <vector>
#include <memory>

namespace sim {
    /// A complete scene: root node + metadata.
    struct Scene {
        std::string              name = "default";
        SceneNodePtr             root = nullptr;
        std::vector<SceneNodePtr> cameras = {};   // convenience cache, populated on load
    };
}