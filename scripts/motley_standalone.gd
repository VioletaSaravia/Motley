extends Node

var last_tileset := "res://addons/motley/assets/microbe-2.png"
var known_tilesets: Dictionary = {
	"res://addons/motley/assets/microbe-2.png": Vector2i(16, 16)
}

var tileset_window := preload("res://addons/motley/scenes/tileset_window.tscn").instantiate()
func show_tileset_in_window() -> void:
	(tileset_window.get_child(0) as TextureRect).texture 


func cache_tileset(path: String) -> void:
	last_tileset = path

func new_sprite() -> void:
	var newsprite := preload("res://addons/motley/scenes/windowed_sprite.tscn").instantiate()
	add_child(newsprite)
	(newsprite.get_child(0) as TextModeSprite).window.get_node("%TilesetPath").text = last_tileset
	((newsprite.get_child(0) as TextModeSprite).window.get_node("%TilesetPath") 
		as LineEdit).text_changed.connect(cache_tileset)
		
	newsprite.focus_entered.connect(show_tileset_in_window)

func _ready() -> void:
	(%Toolbar.get_node("%NewButton") as Button).pressed.connect(new_sprite)

func _process(delta: float) -> void:
	pass
