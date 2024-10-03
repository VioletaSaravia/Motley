@tool
extends Node2D
class_name TextModeSprite

@export var size: Vector2i = Vector2i(20, 12)

@export_group("Tilemap")
@export var tilemap: Texture
@export var tile_size: Vector2i

@export_category("Toolbar")
# TODO Move to top toolbar?
@export var foreground_color: Color = Color.WHITE
@export var background_color: Color = Color.BLACK
@export var brush: BrushType = BrushType.SINGLE
@export var paint_tile: bool = true
@export var paint_color: bool = true
@export var paint_rotation: bool = true

enum TileRotation { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 }
enum BrushType { SINGLE, LINE, RECTANGLE }

# Mouse data
@export var selected_tile := Vector2.ZERO:
	set(val): if val.x >= 0 && val.y >= 0: selected_tile = val
var selected_rotation := TileRotation.UP
var mouse_tile: Vector2

# Data sent to shader
var tiles: PackedVector2Array
var rotations: PackedInt32Array
var colors_fore: PackedVector3Array
var colors_back: PackedVector3Array

func _ready() -> void:
	material = ShaderMaterial.new()
	material.shader = preload("res://scripts/textmode_sprite.gdshader")
	
	for i in range(size.x * size.y):
		tiles.append(Vector2.ZERO)
		colors_fore.append(Vector3.ONE)
		colors_back.append(Vector3.ZERO)
		rotations.append(TileRotation.UP)

func _draw() -> void:
	# draw_rect(Rect2(Vector2.ZERO, Vector2(size.x * tile_size.x, size.y * tile_size.y)), Color.BLACK, false)
	if mouse_tile.x >= 0 && \
		mouse_tile.y >= 0 && \
		mouse_tile.x < size.x && \
		mouse_tile.y < size.y:
		
		draw_texture_rect_region(
			tilemap, 
			Rect2(
				Vector2(mouse_tile.x * tile_size.x, mouse_tile.y * tile_size.y), 
				tile_size
			), 
			Rect2(
				Vector2(selected_tile.x * tile_size.x, selected_tile.y * tile_size.y), 
				tile_size
			),
			Color(1.0, 1.0, 1.0, 0.7)
		)

func _set_params():
	material.set_shader_parameter("colors_fore", colors_fore)
	material.set_shader_parameter("colors_back", colors_back)
	material.set_shader_parameter("canvas_size", size)
	material.set_shader_parameter("tile_size", tile_size)

func _process(_delta: float) -> void:
	mouse_tile = $"../../Sprite2D".get_local_mouse_position() + \
		Vector2(tile_size.x * size.x, tile_size.y * size.y) / 2 # Why?
	mouse_tile.x = floor(mouse_tile.x / tile_size.x)
	mouse_tile.y = floor(mouse_tile.y / tile_size.y)
	
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT) && \
		mouse_tile.x >= 0 && \
		mouse_tile.y >= 0 && \
		mouse_tile.x < size.x && \
		mouse_tile.y < size.y:
		
		tiles[mouse_tile.y * size.y + mouse_tile.x] = selected_tile
		colors_fore[mouse_tile.y * size.y + mouse_tile.x] = Vector3(
			foreground_color.r, foreground_color.g, foreground_color.b)
		colors_back[mouse_tile.y * size.y + mouse_tile.x] = Vector3(
			background_color.r, background_color.g, background_color.b)
		rotations[mouse_tile.y * size.y + mouse_tile.x] = selected_rotation
	
	queue_redraw()
