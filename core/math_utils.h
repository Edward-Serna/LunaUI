#pragma once

// GLM's gtx/ extensions are flagged as experimental. This must be defined
// before any glm/ header is pulled in anywhere in the project.
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace sim::math {

    // ─── Constants ────────────────────────────────────────────────
    constexpr float PI     = 3.14159265358979323846f;
    constexpr float TAU    = 2.0f * PI;
    constexpr float DEG2RAD = PI / 180.0f;
    constexpr float RAD2DEG = 180.0f / PI;

    // ─── Aliases (keep code concise) ─────────────────────────────
    using Vec2  = glm::vec2;
    using Vec3  = glm::vec3;
    using Vec4  = glm::vec4;
    using Mat3  = glm::mat3;
    using Mat4  = glm::mat4;
    using Quat  = glm::quat;

    // ─── Helpers ──────────────────────────────────────────────────

    /// Linearly interpolate between two floats.
    inline float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    /// Wrap an angle (radians) into [0, TAU).
    inline float wrapAngle(float angle) {
        float wrapped = std::fmod(angle, TAU);
        return wrapped < 0.0f ? wrapped + TAU : wrapped;
    }

    /// Build a rotation matrix around an arbitrary axis.
    inline Mat4 rotateAround(const Vec3& axis, float radians) {
        return glm::rotate(Mat4(1.0f), radians, axis);
    }

} // namespace sim::math