extends Node2D

var s: TextModeSprite

func _draw() -> void:
	if not s or not visible: return
	
	if Rect2(0, 0, s.size.x, s.size.y).has_point(s.mouse_tile):
		material.set_shader_parameter("foreground", s.foreground_color)
		material.set_shader_parameter("background", s.background_color)
		
		draw_set_transform(Vector2.ZERO, s.selected_rotation * PI / 2)
		draw_texture_rect_region(
			s.tileset, 
			Rect2(
				Vector2(s.mouse_tile.x * s.tile_size.x, s.mouse_tile.y * s.tile_size.y), 
				s.tile_size
			),
			Rect2(
				Vector2(s.selected_tile.x * s.tile_size.x, s.selected_tile.y * s.tile_size.y), 
				s.tile_size
			),
			Color(1.0, 1.0, 1.0, 0.7)
		)
