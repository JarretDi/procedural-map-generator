#pragma once

#include <array>
#include <godot_cpp/variant/vector2i.hpp>

constexpr size_t TILE_TYPE_COUNT = 64;

const Vector2i NULLVEC = Vector2i(-1, -1);

const std::array<Vector2i, 4> CARDINAL_VECTORS = {
    Vector2i( 0, -1),
    Vector2i( 1,  0),
    Vector2i( 0,  1),
    Vector2i(-1,  0),
};