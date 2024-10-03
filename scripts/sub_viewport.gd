@tool
extends SubViewport

@onready var sprite := $TextModeSprite
@onready var output := $"../Sprite2D"
var image: Image

func _set_size() -> void:
	size = Vector2(sprite.tile_size.x * sprite.size.x, sprite.tile_size.y * sprite.size.y)

func _ready() -> void:
	_set_size()
	output.texture = get_texture()

func _process(_delta: float) -> void:
	_set_size()
	
	if Input.is_key_pressed(KEY_SPACE):
		image = output.texture.get_image()
		image.save_png("res://assets/render.png")
