extends Node

var last_tileset := "res://addons/motley/assets/8px.png"
var known_tilesets: Dictionary = {
	"res://addons/motley/assets/8px.png": Vector2i(8, 8)
}

func cache_tileset(path: String) -> void:
	last_tileset = path

var created_sprites := []

func new_sprite() -> void:
	var newsprite := preload("res://addons/motley/scenes/standalone/windowed_sprite.tscn").instantiate()
	add_child(newsprite)
	
	var window := (newsprite.get_node("%TextModeSprite") as TextModeSprite).window
	window.get_node("%TilesetPath").text = last_tileset
	window.get_node("%TileSizeX").text = str(known_tilesets[last_tileset].x)
	window.get_node("%TileSizeY").text = str(known_tilesets[last_tileset].y)
	(window.get_node("%TilesetPath") as LineEdit).text_changed.connect(cache_tileset)
	
	created_sprites.append(newsprite)

func load_sprite() -> void:
	var newsprite := preload("res://addons/motley/scenes/standalone/windowed_sprite.tscn").instantiate()
	add_child(newsprite)
	
	# TODO load and save dialogs
	(newsprite.get_node("%TextModeSprite") as TextModeSprite).load_tilemap("res://resources/charmap5969898312590232458.tres")
	created_sprites.append(newsprite)
	

var toolbar: Window
func _ready():
	var root = get_tree().root
	toolbar = root.get_child(root.get_child_count() - 1).get_node("%Toolbar")
	(toolbar.get_node("%NewButton") as Button).pressed.connect(new_sprite)
	(toolbar.get_node("%OpenButton") as Button).pressed.connect(load_sprite)

func _process(delta: float) -> void:
	for i in created_sprites:
		if is_instance_valid(i) && i is Window && (i as Window).has_focus():
			var texture := (i.get_node("%TextModeSprite") as TextModeSprite).tileset
			if texture != null:
				(toolbar.get_node("%Tileset") as TextureRect).texture = texture
				toolbar.tile_size = (i.get_node("%TextModeSprite") as TextModeSprite).tile_size
