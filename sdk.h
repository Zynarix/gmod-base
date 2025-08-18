#pragma once
#include <iostream>
#include <array>

struct Vector3 {
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}

    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 self() const {
        return *this;
    }

    Vector3 operator-(const Vector3& vec) const {
        return Vector3(x - vec.x, y - vec.y, z - vec.z);
    }
};

struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float xx, float yy) : x(xx), y(yy) {}

    Vec2 operator+(const Vector3& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 self() const {
        return *this;
    }

    Vec2 operator-(const Vector3& vec) const {
        return Vec2(x - vec.x, y - vec.y);
    }
};

struct Angle3 {
    float pitch, yaw, roll;

    Angle3() : pitch(0), yaw(0), roll(0) {}
    Angle3(float p, float y, float r) : pitch(p), yaw(y), roll(r) {}

    Angle3 operator+(const Angle3& other) const {
        return Angle3(pitch + other.pitch, yaw + other.yaw, roll + other.roll);
    }

    Angle3 self() const {
        return *this;
    }

    Angle3 operator-(const Angle3& other) const {
        return Angle3(pitch - other.pitch, yaw - other.yaw, roll - other.roll);
    }
};



struct ent {
    bool isply{};
    int hp{};
    int armor{};
    std::string clas{};
    Vector3 pos{};
    Vector3 ang{};
};

struct ply : ent {
    bool islocal{};
    std::string name{};
    Vector3 eyepos{};
    std::array<Vector3, 4> mins{};

    ply(const std::string& name_,
        bool islocal_,
        int hp_,
        int armor_,
        const std::string& clas_,
        const Vector3& pos_,
        const Vector3& ang_,
        const Vector3& eyepos_,
        const std::array<Vector3, 4>& mins_)
    {
        name = name_;
        islocal = islocal_;
        hp = hp_;
        armor = armor_;
        clas = clas_;
        pos = pos_;
        ang = ang_;
        eyepos = eyepos_;
        mins = mins_;
        isply = true;
    }
};