@tool
extends Sprite2D

func _process(delta: float) -> void:
	(texture as PlaceholderTexture2D).size = DisplayServer.screen_get_size()
