#pragma once

#include <godot-cpp/classes/object.hpp>

using namespace godot;

class Generator : public Object {
    GDCLASS(Generator, Object)

    protected:
        static void _bind_methods();
        
}