@tool
extends Control

enum BrushType { SINGLE, LINE, PIXEL, RECT }
@onready var brush_group := ButtonGroup.new()

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

func _ready() -> void:
	%PaintSingleButton.button_group = brush_group
	%PaintLineButton.button_group = brush_group
	%PaintPixelButton.button_group = brush_group
	%PaintRectButton.button_group = brush_group
	%PaintSingleButton.button_pressed = true

func _process(delta: float) -> void:
	pass
