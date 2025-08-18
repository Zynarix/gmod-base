#pragma once
#include "../sdk.h"
#include <cmath>

float M_PI = 3.14159265f;

struct aim {
    Angle3 calc(const Vector3& orig, const Vector3& tar, const Angle3& origang) const {
        Vector3 delta = tar - orig;
        float hypotenuse = std::sqrt(delta.x * delta.x + delta.z * delta.z);

        Angle3 targetAng;
        targetAng.pitch = std::atan2(-delta.y, hypotenuse) * (180.0f / M_PI);
        targetAng.yaw = std::atan2(delta.x, delta.z) * (180.0f / M_PI);
        targetAng.roll = 0.0f;

        Angle3 deltaAng;
        deltaAng.pitch = targetAng.pitch - origang.pitch;
        deltaAng.yaw = targetAng.yaw - origang.yaw;
        deltaAng.roll = 0.0f;

        auto normalizeAngle = [](float angle) {
            while (angle > 180.0f) angle -= 360.0f;
            while (angle < -180.0f) angle += 360.0f;
            return angle;
            };
        deltaAng.pitch = normalizeAngle(deltaAng.pitch);
        deltaAng.yaw = normalizeAngle(deltaAng.yaw);

        return deltaAng;
    }
    Vec2 ang3toscr(const Angle3& ang, int screenWidth, int screenHeight, float fovX) const {
        float aspectRatio = static_cast<float>(screenWidth) / screenHeight;
        float fovY = 2.0f * std::atan(std::tan(fovX * M_PI / 360.0f) / aspectRatio) * (180.0f / M_PI);

        float anglePerPixelX = fovX / screenWidth;
        float anglePerPixelY = fovY / screenHeight;

        Vec2 screenPos;
        screenPos.x = std::round(ang.yaw / anglePerPixelX);
        screenPos.y = std::round(ang.pitch / anglePerPixelY);

        return screenPos;
    }
};