#include "orbit_camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

namespace sim {

void OrbitCamera::applyMouseDeltaX(OrbitData& orbit, float deltaPixels, float sensitivity) {
    // FIXED: Negate deltaPixels so dragging right rotates camera right
    orbit.azimuth -= deltaPixels * sensitivity;
    // Wrap into [0, 2π) — not strictly required but keeps the saved value tidy.
    orbit.azimuth = std::fmod(orbit.azimuth, math::TAU);
    if (orbit.azimuth < 0.0f) orbit.azimuth += math::TAU;
}

void OrbitCamera::applyMouseDeltaY(OrbitData& orbit, float deltaPixels, float sensitivity) {
    orbit.elevation -= deltaPixels * sensitivity;   // minus: screen-Y is inverted
    orbit.elevation =  std::clamp(orbit.elevation, MIN_ELEVATION, MAX_ELEVATION);
}

void OrbitCamera::applyZoom(OrbitData& orbit, float scrollDelta, float zoomSpeed) {
    orbit.radius -= scrollDelta * zoomSpeed;
    orbit.radius = std::clamp(orbit.radius, MIN_RADIUS, MAX_RADIUS);
}

void OrbitCamera::update(SceneNodePtr& node, float dt) {
    if (!node || !node->orbit) return;

    OrbitData& orbit = *node->orbit;

    // ── Auto-orbit ────────────────────────────────────────
    if (orbit.autoOrbit) {
        orbit.azimuth += orbit.speed * dt;
        orbit.azimuth = std::fmod(orbit.azimuth, math::TAU);
        if (orbit.azimuth < 0.0f) orbit.azimuth += math::TAU;
    }

    // ── Spherical → Cartesian ─────────────────────────────
    //   x = r · cos(elev) · sin(azim)
    //   y = r · sin(elev)
    //   z = r · cos(elev) · cos(azim)
    float cosElev = std::cos(orbit.elevation);
    math::Vec3 offset {
        orbit.radius * cosElev * std::sin(orbit.azimuth),
        orbit.radius * std::sin(orbit.elevation),
        orbit.radius * cosElev * std::cos(orbit.azimuth)
    };

    node->transform.position = orbit.target + offset;

    // ── Rotation: point camera at target ──────────────────
    // We compute a lookAt matrix and extract the rotation quaternion from it.
    math::Vec3 up { 0.0f, 1.0f, 0.0f };
    math::Mat4 lookAt = glm::lookAt(node->transform.position, orbit.target, up);

    // The lookAt matrix is a view matrix (world→camera).
    // We want the inverse rotation (camera→world) as the node's orientation.
    // For an orthonormal rotation block the inverse == transpose.
    math::Mat3 rotBlock = math::Mat3(glm::inverse(lookAt));
    node->transform.rotation = glm::quat_cast(rotBlock);
}

} // namespace sim
