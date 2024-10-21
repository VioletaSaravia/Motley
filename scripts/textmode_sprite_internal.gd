@tool
extends Node2D

var s: TextModeSprite

func _draw() -> void:
	if not s: return
	
	material.set_shader_parameter("fore_colors", s.fore_tex)
	material.set_shader_parameter("back_colors", s.back_tex)
	
	for i in range(s.size.x):
		for j in range(s.size.y):
			var rot = s.rotations[i][j]
			draw_set_transform(Vector2.ZERO, (rot if rot else 0.0) * PI / 2)
			var tile = s.tiles[i][j]
			if tile != null:
				draw_texture_rect_region(
					s.tileset,
					Rect2(Vector2(i * s.tile_size.x, j * s.tile_size.y), s.tile_size),
					Rect2(Vector2(tile.x * s.tile_size.x, tile.y * s.tile_size.y), s.tile_size)
				)
