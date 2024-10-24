@tool
extends Window

# TODO Move enum to motley.gd
enum TileRotation { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 }
var selected_rotation := TileRotation.UP
var selected_tile := Vector2.ZERO

enum BrushType { SINGLE, LINE, PIXEL, RECT }
@onready var brush_group := ButtonGroup.new()

func get_brush() -> BrushType:
	match brush_group.get_pressed_button().name:
		"PaintLineButton": return BrushType.LINE
		"PaintPixelButton": return BrushType.PIXEL
		"PaintRectButton": return BrushType.RECT
		_: return BrushType.SINGLE

var fg_selected := 0
var bg_selected := 1

var tile_size: Vector2i

# TODO
var palette_count := 8

@onready var palette := [
	%PaletteColor01, %PaletteColor02, %PaletteColor03, %PaletteColor04, 
	%PaletteColor05, %PaletteColor06, %PaletteColor07, %PaletteColor08
]

func load_palette(path: String) -> void:
	pass

func get_foreground_color() -> Color:
	return ((palette[fg_selected] as Panel)
		.get_theme_stylebox("panel") as StyleBoxFlat).bg_color
	
func get_background_color() -> Color:
	return ((palette[bg_selected] as Panel)
		.get_theme_stylebox("panel") as StyleBoxFlat).bg_color

func is_paint_tile_on() -> bool:
	return %PaintTileButton.button_pressed

func is_paint_rotation_on() -> bool:
	return %PaintRotButton.button_pressed
	
func is_paint_foreground_on() -> bool:
	return %PaintForeButton.button_pressed
	
func is_paint_background_on() -> bool:
	return %PaintBackButton.button_pressed
	
func is_foreground_alpha() -> bool:
	return %ForeAlphaButton.button_pressed
	
func is_background_alpha() -> bool:
	return %BackAlphaButton.button_pressed

func _ready() -> void:
	unresizable = true
	%PaintSingleButton.button_group = brush_group
	%PaintLineButton.button_group = brush_group
	%PaintPixelButton.button_group = brush_group
	%PaintRectButton.button_group = brush_group
	%PaintSingleButton.button_pressed = true
	size = %MarginContainer.size
	
	for i in range(2, len(palette)):
		palette[i].get_theme_stylebox("panel").bg_color = Color(randf(), randf(), randf(), 1)

func _set_border(box: StyleBoxFlat, px: int, color: Color = Color.BLACK) -> void:
	box.border_width_left = px
	box.border_width_right = px
	box.border_width_top = px
	box.border_width_bottom = px
	if color != Color.BLACK:
		box.border_color = color

func _process(delta: float) -> void:
	size = %MarginContainer.size
	
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
		if Rect2(%Tileset.global_position, %Tileset.size).has_point(%Tileset.get_global_mouse_position()):
			var pos: Vector2 = %Tileset.get_global_mouse_position() - %Tileset.global_position
			pos = Vector2(
				floor(pos.x / tile_size.x) * tile_size.x,
				floor(pos.y / tile_size.y) * tile_size.y)
			selected_tile = Vector2(pos.x / tile_size.x, pos.y / tile_size.y)
		
		for i in range(len(palette)):
			var panel := palette[i] as Panel
			if Rect2(panel.global_position, panel.size).has_point(panel.get_global_mouse_position()):
				fg_selected = i
				break
	
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT):
		for i in range(len(palette)):
			var panel := palette[i] as Panel
			if Rect2(panel.global_position, panel.size).has_point(panel.get_global_mouse_position()):
				bg_selected = i
				break

	for i in range(len(palette)):
		_set_border(palette[i].get_theme_stylebox("panel"), 0)
	
	if bg_selected == fg_selected:
		_set_border(palette[bg_selected].get_theme_stylebox("panel"), 
			2, (Color.CRIMSON + Color.LIME_GREEN) * 0.75)
	else:
		_set_border(palette[bg_selected].get_theme_stylebox("panel"), 2, Color.CRIMSON)
		_set_border(palette[fg_selected].get_theme_stylebox("panel"), 2, Color.LIME_GREEN)
