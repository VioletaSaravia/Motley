@tool
extends EditorPlugin

var bottom_control: Control = preload("res://addons/motley/scenes/editor/dock_menu.tscn").instantiate()

func _enter_tree() -> void:
	add_autoload_singleton("motley", "res://addons/motley/scripts/motley_singleton.gd")
	add_control_to_bottom_panel(bottom_control, "Textmode Editor")
	bottom_control.visible = false

func _exit_tree() -> void:
	remove_autoload_singleton("motley")
	remove_control_from_bottom_panel(bottom_control)

func _build() -> bool:
	return true

func _handles(object: Object) -> bool:
	return true
	
func _make_visible(visible: bool) -> void:
	bottom_control.visible = visible

func _process(delta: float) -> void:
	pass
