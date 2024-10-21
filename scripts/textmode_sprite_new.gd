@tool
class_name TextModeSpriteNEW extends Sprite2D

### STATE VARIABLES ###
@onready var toolbar := %Toolbar if Engine.is_editor_hint() else null
var foreground_color := Color.WHITE:
	get: return toolbar.get_foreground_color()
var transparent_foreground := false:
	get: return toolbar.is_foreground_alpha()
var background_color := Color.BLACK:
	get: return toolbar.get_background_color()
var transparent_background := false:
	get: return toolbar.is_background_alpha()
var brush:
	get: return toolbar.get_brush()
var paint_tile := true:
	get: return toolbar.is_paint_tile_on()
var paint_foreground := true:
	get: return toolbar.is_paint_foreground_on()
var paint_background := true:
	get: return toolbar.is_paint_background_on()
var paint_rotation := true:
	get: return toolbar.is_paint_rotation_on()
var mouse_tile: Vector2
var selected_tile := Vector2.ZERO:
	get: return toolbar.selected_tile if toolbar else selected_tile
	set(val): if val.x >= 0 && val.y >= 0: selected_tile = val
var selected_rotation:
	get: return toolbar.selected_rotation if toolbar else toolbar.TileRotation.UP

enum STATE { NotStarted, Editing, Baked }
var state := STATE.NotStarted
var size: Vector2i = Vector2i(10, 10):
	set(val): if state == STATE.NotStarted: size = val
var tileset: Texture
var tile_size: Vector2i

### RENDERING ###
var viewport_scene := preload("res://addons/motley/scenes/tilemap_viewport.tscn")
var viewport: SubViewport
var sprite: Node2D
var mouse_sprite: Node2D

var tiles: Array[Array] # of Vector2
var rotations: Array[Array] # of TileRotation
var fore_colors: Image
var back_colors: Image
var fore_tex: ImageTexture
var back_tex: ImageTexture

### MENU SCRIPTS ###
var new_msg_scene := preload("res://addons/motley/scenes/new_textmode_sprite_msg.tscn")
var new_msg: Label
var new_window_scene := preload("res://addons/motley/scenes/new_tilemap_window.tscn")
var tilset_picker := preload("res://addons/motley/scenes/tileset_picker.tscn")
var new_window: Control

### HELPER FUNCTIONS ###
func zoom_in() -> void:
	if state != STATE.Editing: return
	
	scale *= 2
	position += Vector2(
		size.x * tile_size.x / 4 * scale.x,
		size.y * tile_size.y / 4 * scale.y)
	
func zoom_out() -> void:
	if state != STATE.Editing: return
	
	scale /= 2
	position -= Vector2(
		size.x * tile_size.x / 2 * scale.x,
		size.y * tile_size.y / 2 * scale.y)

# TODO Save dialogs
var uid := str(ResourceUID.create_id())
var sprite_name := ""
func save_tilemap() -> void:
	if state != STATE.Editing: return
	
	var saved := PackedScene.new()
	#saved.pack(sprite)
	ResourceSaver.save(saved, "res://scenes/textmode_sprite_" + uid + ".tscn")

func save_png() -> void:
	texture.get_image().save_png("res://assets/sprite.png")

### MAIN FUNCTIONS ###

func _start_editing() -> void:
	size = Vector2(
		int(new_window.get_node("%TilemapWidth").text), 
		int(new_window.get_node("%TilemapHeight").text))
	tileset = load(new_window.get_node("%TilesetPath").text)
	tile_size = Vector2(
		int(new_window.get_node("%TileSizeX").text), 
		int(new_window.get_node("%TileSizeY").text))
	position += Vector2(size.x * tile_size.x / 2, size.y * tile_size.y / 2)
	#scale = Vector2(2, 2) # TODO
	
	viewport = viewport_scene.instantiate()
	sprite = viewport.get_node("%Sprite")
	sprite.s = self
	mouse_sprite = viewport.get_node("%MouseSprite")
	mouse_sprite.s = self
	add_child(viewport)
	viewport.size = Vector2(tile_size.x * size.x * scale.x, tile_size.y * size.y * scale.y)
	
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
			rotations[i].append(toolbar.TileRotation.UP)
	
	texture = viewport.get_texture()
	texture_filter = TEXTURE_FILTER_NEAREST
	
	state = STATE.Editing
	new_window.queue_free()
	new_msg.queue_free()

func _draw() -> void:
	pass

func _ready() -> void:
	if state != STATE.NotStarted: return
	
	if Engine.is_editor_hint():
		new_msg = new_msg_scene.instantiate()
		add_child(new_msg)
	else:
		new_window = new_window_scene.instantiate()
		add_child(new_window)
		(new_window.get_node("%CreateButton") as Button).pressed.connect(_start_editing)

### UPDATING ###

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
	mouse_sprite.queue_redraw()
	
	_set_mouse_tile()
	
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT) && \
		Rect2(0, 0, size.x, size.y).has_point(mouse_tile):
			_place_tile()

func _process(delta: float) -> void:
	if state == STATE.Editing: _update_editor()
