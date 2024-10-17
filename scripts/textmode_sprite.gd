@tool
class_name TextModeSprite extends Sprite2D

enum STATE { NotStarted, Editing, Baked }
var state := STATE.NotStarted

@export var size: Vector2i = Vector2i(10, 10):
	set(val):
		if state == STATE.NotStarted:
			size = val

@export var tileset: Texture
@export var tile_size: Vector2i

@export_category("Toolbar")
# TODO Move to top toolbar?
@export var selected_tile := Vector2.ZERO:
	set(val): if val.x >= 0 && val.y >= 0: selected_tile = val
@export var selected_rotation := TileRotation.UP
var mouse_tile: Vector2

@export var foreground_color := Color.WHITE
@export var transparent_foreground := false
@export var background_color := Color.BLACK
@export var transparent_background := false
@export var brush := BrushType.SINGLE
@export var paint_tile := true
@export var paint_foreground := true
@export var paint_background := true
@export var paint_rotation := true
@export var bake := false:
	set(val):
		if state == STATE.NotStarted: return
		state = STATE.Baked if not bake else STATE.Editing
		
		if bake:
			texture = viewport.get_texture()
		else:
			texture = ImageTexture.create_from_image(texture.get_image())
		
		bake = val

enum TileRotation { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 }
enum BrushType { SINGLE, LINE, RECTANGLE, PIXEL }

# Rendering data
var tiles: Array[Array] # of Vector2
var rotations: Array[Array] # of TileRotation
var fore_colors: Image
var back_colors: Image
var fore_tex: ImageTexture
var back_tex: ImageTexture

func _set_viewport_size() -> void:
	viewport.size = Vector2(tile_size.x * size.x, tile_size.y * size.y)

func save_png() -> void:
	texture.get_image().save_png("res://assets/sprite.png")

var viewport: SubViewport
var sprite: Node2D
var mouse: Node2D

var NewTilemapWindow := preload("res://addons/motley/scenes/new_tilemap_window.tscn")
var TilesetPicker := preload("res://addons/motley/scenes/tileset_picker.tscn")

var window: Control

func _ready() -> void:
	if state == STATE.NotStarted:
		window = NewTilemapWindow.instantiate()
		add_child(window)
		(window.get_node("%CreateButton") as Button).pressed.connect(_start_editing)
		return

var sprite_script := preload("res://addons/motley/scripts/textmode_sprite_internal.gd")
var shader_script := preload("res://addons/motley/shaders/textmode_sprite.gdshader")
var mouse_script := preload("res://addons/motley/scripts/mouse_sprite.gd")
var mouse_shader := preload("res://addons/motley/shaders/mouse_sprite.gdshader")

func _start_editing() -> void:
	size = Vector2(
		int(window.get_node("%TilemapWidth").text), 
		int(window.get_node("%TilemapHeight").text))
	
	viewport = SubViewport.new()
	viewport.transparent_bg = true
	viewport.snap_2d_transforms_to_pixel = true
	
	sprite = sprite_script.new()
	sprite.s = self
	sprite.material = ShaderMaterial.new()
	sprite.material.shader = shader_script
	
	mouse = mouse_script.new()
	mouse.s = self
	mouse.material = ShaderMaterial.new()
	mouse.material.shader = mouse_shader
	
	tileset = load(window.get_node("%TilesetPath").text)
	tile_size = Vector2(
		int(window.get_node("%TileSizeX").text), 
		int(window.get_node("%TileSizeY").text))
	
	fore_colors = Image.create(
		size.x * tile_size.x, size.y * tile_size.y, 
		false, Image.FORMAT_RGBA8)
	back_colors = Image.create(
		size.x * tile_size.x, size.y * tile_size.y, 
		false, Image.FORMAT_RGBA8)
	fore_tex = ImageTexture.create_from_image(fore_colors)
	back_tex = ImageTexture.create_from_image(back_colors)
	
	for i in range(size.x):
		tiles.append([])
		rotations.append([])
		tiles[i].resize(size.y)
		for j in range(size.y):
			rotations[i].append(TileRotation.UP)
	
	viewport.add_child(sprite)
	viewport.add_child(mouse)
	self.add_child(viewport)
	
	_set_viewport_size()
	
	texture = viewport.get_texture()
	texture_filter = TEXTURE_FILTER_NEAREST
	
	state = STATE.Editing
	window.queue_free()

func _place_tile() -> void:
	if paint_tile: 
		tiles[mouse_tile.x][mouse_tile.y] = selected_tile
	if paint_rotation: 
		rotations[mouse_tile.x][mouse_tile.y] = selected_rotation
	
	if not paint_foreground and not paint_background: return
	
	# TODO Inefficient
	for i in range(tile_size.x):
		for j in range(tile_size.y):
			if paint_foreground:
				fore_colors.set_pixel(
					int(mouse_tile.x * tile_size.x + i),
					int(mouse_tile.y * tile_size.y + j),
					Color(
						foreground_color.r, foreground_color.g, foreground_color.b, 
						0.0 if transparent_foreground else 1.0)
				)
			if paint_background:
				back_colors.set_pixel(
					int(mouse_tile.x * tile_size.x + i),
					int(mouse_tile.y * tile_size.y + j),
					Color(
						background_color.r, background_color.g, background_color.b, 
						0.0 if transparent_background else 1.0)
				)
	
	fore_tex.set_image(fore_colors)
	back_tex.set_image(back_colors)

func _set_mouse_tile() -> void:
	mouse_tile = get_local_mouse_position() + \
		Vector2(tile_size.x * size.x, tile_size.y * size.y) / 2 # Why?
	mouse_tile.x = floor(mouse_tile.x / tile_size.x)
	mouse_tile.y = floor(mouse_tile.y / tile_size.y)

func _update_editor() -> void:
	sprite.queue_redraw()
	mouse.queue_redraw()
	
	#if Input.is_key_pressed(KEY_SPACE): save_png()
	
	_set_mouse_tile()
	
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT) && \
		Rect2(0, 0, size.x, size.y).has_point(mouse_tile):
			_place_tile()

func _process(_delta: float) -> void:
	if state == STATE.Editing:
		_update_editor()
