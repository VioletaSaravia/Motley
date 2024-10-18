@tool
extends Window

enum BrushType { SINGLE, LINE, PIXEL, RECT }
@onready var brush_group := ButtonGroup.new()
@onready var color_fore_group := ButtonGroup.new()
@onready var color_back_group := ButtonGroup.new()

func get_brush() -> BrushType:
	var name := brush_group.get_pressed_button().name
	
	if name == "PaintLineButton":
		return BrushType.LINE
	if name == "PaintPixelButton":
		return BrushType.PIXEL
	if name == "PaintRectButton":
		return BrushType.RECT
	
	return BrushType.SINGLE

func get_foreground_color() -> Color:
	return Color.WHITE
	
func get_background_color() -> Color:
	return Color.BLACK

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

var foreground_selected := 0
var background_selected := 0

@onready var palette_fore := [
	%PaletteColor01, %PaletteColor02, %PaletteColor03, %PaletteColor04, 
	%PaletteColor05, %PaletteColor06, %PaletteColor07, %PaletteColor08
]
	
@onready var palette_back := [
	%PaletteColor09, %PaletteColor10, %PaletteColor11, %PaletteColor12, 
	%PaletteColor13, %PaletteColor14, %PaletteColor15, %PaletteColor16
]

func _ready() -> void:
	unresizable = true
	%PaintSingleButton.button_group = brush_group
	%PaintLineButton.button_group = brush_group
	%PaintPixelButton.button_group = brush_group
	%PaintRectButton.button_group = brush_group
	%PaintSingleButton.button_pressed = true
	size = %MarginContainer.size
	
	for i in palette_fore:
		i = i as ColorPickerButton
		i.button_group = color_fore_group
	for i in palette_back:
		i = i as ColorPickerButton
		i.button_group = color_back_group
	
	var margins := get_child(0) as MarginContainer
	var margin_value = 5
	margins.add_theme_constant_override("margin_top", margin_value)
	margins.add_theme_constant_override("margin_left", margin_value)
	margins.add_theme_constant_override("margin_bottom", margin_value)
	margins.add_theme_constant_override("margin_right", margin_value)

func _process(delta: float) -> void:
	size = %MarginContainer.size
