# TileMapGenerator (Godot GDExtension)

A procedural tile map generator with Godot 4 to explore the Wave Function Collapse algorithm (https://en.wikipedia.org/wiki/Model_synthesis). Implemented from scratch in C++ as a GDExtension module.

## Features

- Constraint-based tile placement via neighbor rules
- Radius-based propagation
- Visual step-by-step generation via `_process`
- Written in C++ for performance, using GDExtension
- Supports custom tiles and rules via Godot's `Dictionary` input

## Dependencies

- Godot Engine 4.4+
- C++ compiler (MSVC, GCC, or Clang)
- [godot-cpp](https://github.com/godotengine/godot-cpp) (included as a submodule)
- Python + SCons (for building)

## Setup

```bash
# Clone the repo
git clone --recurse-submodules https://github.com/JarretDi/procedural-map-generator.git
cd map-generator

# Build the extension
scons

# Alternatively:
python -m SCons
```

Then, open up demo/project.godot and run

## CREDITS:
	Tileset by Shade from https://merchant-shade.itch.io/16x16-mini-world-sprites under Creative Commons Zero v1.0 Universal
