extends TextureRect

@onready var toolbar := $"../../.."

func _draw() -> void:
	var size := toolbar.tile_size as Vector2i
	draw_rect(
		Rect2(
			size.x * toolbar.selected_tile.x, 
			size.y * toolbar.selected_tile.y, size.x, size.y), 
		Color.RED, false)

func _process(delta: float) -> void:
	queue_redraw()
