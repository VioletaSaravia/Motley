@tool 
extends TileMapLayer
class_name TextModeSpriteOLD

var colors_fore: PackedVector3Array
var colors_back: PackedVector3Array

@export var foreground_color := Color.WHITE;
@export var background_color := Color.BLACK;
@export var canvas_size := Vector2(11, 9)
@export var tile_size: Vector2: 
	get: return tile_set.tile_size
	set(val): tile_set.tile_size = val

func _set_params():
	material.set_shader_parameter("colors_fore", colors_fore)
	material.set_shader_parameter("colors_back", colors_back)
	material.set_shader_parameter("canvas_size", canvas_size)
	material.set_shader_parameter("tile_size", tile_size)

func _ready():
	material = ShaderMaterial.new()
	material.shader = preload("res://scripts/textmode_sprite_colors.gdshader")
	
	for i in range(canvas_size.x * canvas_size.y):
		colors_fore.append(Vector3.ONE)
		colors_back.append(Vector3.ZERO)
	_set_params()

func _process(delta: float) -> void:
	var mouse_pos := get_global_mouse_position()
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT) \
		&& mouse_pos.x > position.x \
		&& mouse_pos.x < (position.x + tile_size.x * canvas_size.x) \
		&& mouse_pos.y > position.y \
		&& mouse_pos.y < (position.y + tile_size.y * canvas_size.y):
		
		var tile_clicked := local_to_map(get_local_mouse_position())
		colors_fore[tile_clicked.x + tile_clicked.y * canvas_size.x] = \
			Vector3(foreground_color.r, foreground_color.g, foreground_color.b)
		colors_back[tile_clicked.x + tile_clicked.y * canvas_size.x] = \
			Vector3(background_color.r, background_color.g, background_color.b)
	_set_params()
