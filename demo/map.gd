extends Node2D

@onready var base_layer: TileMapLayer = $BaseLayer
@onready var gen_layer: TileMapGenerator = $GenLayer

@export var MAP_SIZE : int;
@export var RADIUS : int;
@export var DELAYMS : int;

const ATLAS_ID = 0;

const WATER = Vector2i(9,15);
const SAND = Vector2i(10,16);
const GRASS = Vector2i(10,15);
const FOREST = Vector2i(1,15);
const MOUNTAIN = Vector2i(5,11);

func _ready() -> void:
	for x in MAP_SIZE:
		for y in MAP_SIZE:
			base_layer.set_cell(Vector2i(x,y), ATLAS_ID, GRASS);
	
	var tileRules = {
		WATER : [WATER, SAND],
		SAND : [WATER, SAND, GRASS],
		GRASS : [SAND, GRASS, FOREST, MOUNTAIN],
		FOREST : [GRASS, FOREST, MOUNTAIN],
		MOUNTAIN : [GRASS, FOREST, MOUNTAIN]
	}
	
	gen_layer.generate(tileRules, ATLAS_ID, MAP_SIZE, RADIUS, DELAYMS)	
