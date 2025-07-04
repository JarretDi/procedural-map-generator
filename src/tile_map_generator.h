#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/tile_map_layer.hpp>

using namespace godot;

class TileMapGenerator : public TileMapLayer {
    GDCLASS(Generator, Object)

    protected:
        static void _bind_methods();
        
};