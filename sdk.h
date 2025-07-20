#pragma once

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