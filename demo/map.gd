extends Node2D

@onready var base_layer: TileMapLayer = $BaseLayer
@onready var gen_layer: TileMapGenerator = $GenLayer

@export var MAP_SIZE : int;
@export var RADIUS : int;

const ATLAS_ID = 0;

const WATER = Vector2i(9,15);
const SAND = Vector2i(10,16);
const GRASS = Vector2i(10,15);
const FOREST = Vector2i(1,15);
const MOUNTAIN = Vector2i(5,11);

func _ready() -> void:
	set_process(false);
	for x in MAP_SIZE:
		for y in MAP_SIZE:
			base_layer.set_cell(Vector2i(x,y), ATLAS_ID, GRASS);
	
	var tileRules = {
		WATER : [WATER, SAND],
		SAND : [WATER, SAND, GRASS, FOREST, MOUNTAIN],
		GRASS : [SAND, GRASS, FOREST, MOUNTAIN],
		FOREST : [SAND, GRASS, FOREST, MOUNTAIN],
		MOUNTAIN : [SAND, GRASS, FOREST, MOUNTAIN],
	}
	
	gen_layer.build(tileRules, ATLAS_ID, MAP_SIZE, RADIUS)	
	gen_layer.seed(4, [SAND, FOREST], false)
	set_process(true);

func _process(delta: float) -> void:
	if (gen_layer.has_tiles_to_collapse()):
		gen_layer.collapse_tile(Vector2i(-1,-1), Vector2i(-1,-1))
	else :
		set_process(false);
