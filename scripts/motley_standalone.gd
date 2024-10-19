extends Node2D

var last_tileset := "res://assets/microbe-2.png"
var known_tilesets: Dictionary = {
	"res://assets/microbe-2.png": Vector2i(6, 6)
}

@onready var tileset_window := preload("res://addons/motley/scenes/tileset_window.tscn").instantiate()

func cache_tileset(path: String) -> void:
	last_tileset = path

var created_sprites := []

func new_sprite() -> void:
	var newsprite := preload("res://addons/motley/scenes/windowed_sprite.tscn").instantiate()
	add_child(newsprite)
	
	var window := (newsprite.get_node("%TextModeSprite") as TextModeSprite).window
	window.get_node("%TilesetPath").text = last_tileset
	window.get_node("%TileSizeX").text = str(known_tilesets[last_tileset].x)
	window.get_node("%TileSizeY").text = str(known_tilesets[last_tileset].y)
	(window.get_node("%TilesetPath") as LineEdit).text_changed.connect(cache_tileset)
	
	created_sprites.append(newsprite)

func _ready() -> void:
	(%Toolbar.get_node("%NewButton") as Button).pressed.connect(new_sprite)

func _process(delta: float) -> void:
	for i in created_sprites:
		if is_instance_valid(i) && i is Window && (i as Window).has_focus():
			var texture := (i.get_node("%TextModeSprite") as TextModeSprite).tileset
			if texture != null:
				(%Toolbar.get_node("%Tileset") as TextureRect).texture = texture
				%Toolbar.tile_size = (i.get_node("%TextModeSprite") as TextModeSprite).tile_size
