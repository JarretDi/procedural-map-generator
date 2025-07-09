extends Node2D

@onready var base_layer: TileMapLayer = $BaseLayer
@onready var gen_layer: TileMapGenerator = $GenLayer

@export var MAP_SIZE : int;
@export var RADIUS : int;

const ATLAS_ID = 0;

const WATER = Vector2i(9,15);
const SAND = Vector2i(10,16);
const GRASS2 = Vector2i(10,15);
const GRASS = Vector2i(11,15);
const FOREST = Vector2i(1,15);
const MOUNTAIN = Vector2i(5,11);

const tileRules = {
		WATER : [WATER, SAND],
		SAND : [WATER, SAND, GRASS, GRASS2],
		GRASS : [SAND, GRASS, GRASS2, FOREST, MOUNTAIN],
		GRASS2 : [SAND, GRASS, GRASS2, FOREST, MOUNTAIN],
		FOREST : [GRASS, GRASS2, FOREST, MOUNTAIN],
		MOUNTAIN : [GRASS, GRASS2, FOREST, MOUNTAIN],
	}

func _ready() -> void:
	set_process(false);
	
	var rules = gen_layer.parse_rules(base_layer, 6)
	
	print(rules)
	
	gen_layer.build(rules, ATLAS_ID, MAP_SIZE, RADIUS)	
	#gen_layer.seed(4, [], false)
	set_process(true);

func _process(delta: float) -> void:
	if (gen_layer.has_tiles_to_collapse()):
		gen_layer.collapse_tile()
		gen_layer.collapse_tile()
		gen_layer.collapse_tile()
		gen_layer.collapse_tile()
	else :
		gen_layer.refine();
		set_process(false);
