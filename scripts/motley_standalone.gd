extends Node

func new_sprite() -> void:
	var newsprite := preload("res://addons/motley/scripts/textmode_sprite.gd").new()
	add_child(newsprite)
	newsprite.position += Vector2(600, 250)

func _ready() -> void:
	(%Toolbar.get_node("%NewButton") as Button).pressed.connect(new_sprite)
	pass

func _process(delta: float) -> void:
	pass
