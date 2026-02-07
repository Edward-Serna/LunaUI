#pragma once

#include "scene_node.h"

namespace sim {

/// Stateless updater for orbit cameras.
///
/// Call update() once per frame with dt (seconds).
/// It mutates the OrbitData on the node (azimuth, elevation, radius)
/// and then writes the resulting position into node->transform.position.
///
/// Mouse input is fed in via the delta helpers before update().
class OrbitCamera {
public:
    /// Apply a horizontal mouse drag delta (pixels → radians).
    /// Call this from your SDL event loop when left-button is held.
    static void applyMouseDeltaX(OrbitData& orbit, float deltaPixels, float sensitivity = 0.005f);

    /// Apply a vertical mouse drag delta.
    static void applyMouseDeltaY(OrbitData& orbit, float deltaPixels, float sensitivity = 0.005f);

    /// Apply a scroll-wheel zoom delta.
    static void applyZoom(OrbitData& orbit, float scrollDelta, float zoomSpeed = 0.5f);

    /// Advance the orbit by dt seconds.
    /// If autoOrbit is enabled, azimuth advances by orbit.speed * dt.
    /// Finally, writes the new world position into node->transform.position
    /// and sets the node's rotation to look at orbit.target.
    static void update(SceneNodePtr& node, float dt);

private:
    /// Clamp elevation to avoid gimbal lock at the poles.
    static constexpr float MIN_ELEVATION = -1.5f;  // just under -π/2
    static constexpr float MAX_ELEVATION =  1.5f;  // just under  π/2
    static constexpr float MIN_RADIUS    =  0.5f;
    static constexpr float MAX_RADIUS    = 500.0f;
};

} // namespace sim
