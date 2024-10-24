@tool
extends Control

var tileset_picker_scene := preload("res://addons/motley/scenes/dialogs/tileset_picker.tscn")
var tileset_picker: FileDialog

func _pick_tileset() -> void:
	tileset_picker = tileset_picker_scene.instantiate()
	add_child(tileset_picker)
	tileset_picker.file_selected.connect(_set_tileset)
	tileset_picker.canceled.connect(_free_tileset_picker)

func _set_tileset(path: String) -> void:
	%TilesetPath.text = path
	tileset_picker.queue_free()

func _free_tileset_picker() -> void:
	tileset_picker.queue_free()

func _ready() -> void:
	%PickTileset.pressed.connect(_pick_tileset)

func _process(_delta: float) -> void:
	%CreateButton.disabled = !(
		%TilemapWidth.text != "" &&
		%TilemapHeight.text != "" &&
		%TileSizeX.text != "" &&
		%TileSizeY.text != "" &&
		%TilesetPath.text != ""
	)
