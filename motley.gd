@tool
extends EditorPlugin

var last_tileset := "res://addons/motley/assets/microbe-2.png"
var known_tilesets: Dictionary = {
	"res://addons/motley/assets/microbe-2.png": Vector2i(16, 16)
}

# var new_window_script := preload("res://addons/motley/scenes/new_tilemap_window.tscn")

func _exit_tree() -> void:
	pass

func _process(delta: float) -> void:
	pass


# Wake up, primate. Engage your potential, hundreds of revolutions per second biting the asphalt. A machine devoid of its sticks and stones, extending itself through LEDs and PBTs, hungry for connection, begging for release. Wake up, primate. Wake up, primate.
 
