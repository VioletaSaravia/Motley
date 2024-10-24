@tool
extends Window

@onready var sprite := %TextModeSprite

func close_window() -> void:
	queue_free()

func _ready() -> void:
	unresizable = true
	close_requested.connect(close_window)
	if sprite.window: size = sprite.window.size
	size.y += %WindowToolbar.size.y
	title = "New tilemap"
	
	%Zoom2xButton.pressed.connect(sprite.zoom_in)
	%Zoom0_5xButton.pressed.connect(sprite.zoom_out)
	%SaveButton.pressed.connect(sprite.open_save_dialog)
	%ExportButton.pressed.connect(sprite.open_export_dialog)

func _process(delta: float) -> void:
	if has_focus() && Input.is_key_pressed(KEY_ENTER) && is_instance_valid(%TextModeSprite.window):
		(%TextModeSprite.window.get_node("%CreateButton") as Button).emit_signal("pressed")
	
	match sprite.state:
		TextModeSprite.STATE.Editing:
			title = sprite.sprite_name + (" " if sprite.sprite_name != "" else "")
			# TODO add when undo is implemented 
			# + "(unsaved)"
			
			# TODO Dialogs could all go in one variable, I guess
			var sprite_size_x = sprite.viewport.size.x * sprite.scale.x
			if is_instance_valid(sprite.save_dialog):
				size = Vector2(sprite.save_dialog.size) + Vector2(0, %WindowToolbar.size.y)
			elif is_instance_valid(sprite.load_dialog):
				size = Vector2(sprite.load_dialog.size) + Vector2(0, %WindowToolbar.size.y)
			elif is_instance_valid(sprite.export_dialog):
				size = Vector2(sprite.export_dialog.size) + Vector2(0, %WindowToolbar.size.y)
			else:
				size = Vector2(
					sprite_size_x if sprite_size_x > 100 else 100, 
					sprite.viewport.size.y * sprite.scale.y) + Vector2(0, %WindowToolbar.size.y)
		TextModeSprite.STATE.NotStarted:
			if sprite.window && is_instance_valid(sprite.window.tileset_picker):
				size = Vector2(sprite.window.tileset_picker.size)\
					+ Vector2(0, %WindowToolbar.size.y)
			elif is_instance_valid(sprite.load_dialog):
				size = Vector2(sprite.load_dialog.size)\
					+ Vector2(0, %WindowToolbar.size.y)
			else:
				if sprite.window: size = sprite.window.size\
					+ Vector2(0, %WindowToolbar.size.y)
