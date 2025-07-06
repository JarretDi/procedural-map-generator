#include <godot_cpp/variant/vector2i.hpp>
#include <functional>

namespace std {
    template <>
    struct hash<godot::Vector2i> {
        size_t operator()(const godot::Vector2i& v) const {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };
}