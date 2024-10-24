@tool
class_name TextModeSprite extends Sprite2D

### STATE VARIABLES ###

var sprite_name := ""
# TODO in editor, out of editor and standalone are three different states.
@onready var toolbar := $/root/Main/Toolbar if !Engine.is_editor_hint() else null
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
var selected_rotation := 0:
	get: return toolbar.selected_rotation if toolbar else selected_rotation

enum STATE { NotStarted, Editing, Baked }
var state := STATE.NotStarted
var size: Vector2i = Vector2i(1, 1):
	set(val): if state == STATE.NotStarted: size = val
var tileset: Texture
var tile_size: Vector2i

### RENDERING ###

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

var new_msg: Label
var window: Control

var save_dialog: FileDialog
var load_dialog: FileDialog
var export_dialog: FileDialog

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

func open_save_dialog() -> void:
	if state == STATE.NotStarted: return
	
	save_dialog = motley.save_dialog.instantiate()
	add_child(save_dialog)
	save_dialog.file_selected.connect(save_tilemap)
	
func open_load_dialog() -> void:
	load_dialog = motley.load_dialog.instantiate()
	add_child(load_dialog)
	load_dialog.file_selected.connect(load_tilemap)

func save_tilemap(path: String) -> void:
	if state != STATE.Editing: return
	
	var saved := SavedCharmap.new()
	saved.size = size
	saved.tileset = tileset
	saved.tile_size = tile_size
	saved.tiles = tiles
	saved.rotations = rotations
	saved.fore_colors = fore_colors
	saved.back_colors = back_colors
	saved.fore_tex = fore_tex
	saved.back_tex = back_tex
	
	ResourceSaver.save(saved, path)
	sprite_name = path
	if save_dialog: save_dialog.queue_free()
	
var loaded: SavedCharmap
func load_tilemap(path: String) -> void:
	loaded = load(path)
	
	size = loaded.size
	tileset = loaded.tileset
	tile_size = loaded.tile_size
	position += Vector2(size.x * tile_size.x / 2, size.y * tile_size.y / 2)
	#scale = Vector2(2, 2) # TODO
	
	viewport = motley.viewport.instantiate()
	sprite = viewport.get_node("%Sprite")
	sprite.s = self
	mouse_sprite = viewport.get_node("%MouseSprite")
	mouse_sprite.s = self
	add_child(viewport)
	viewport.size = Vector2(tile_size.x * size.x * scale.x, tile_size.y * size.y * scale.y)
	
	fore_colors = loaded.fore_colors
	back_colors = loaded.back_colors
	fore_tex = loaded.fore_tex
	back_tex = loaded.back_tex
	
	tiles = loaded.tiles
	rotations = loaded.rotations
	
	texture = viewport.get_texture()
	texture_filter = TEXTURE_FILTER_NEAREST
	
	state = STATE.Editing
	sprite_name = path
	if window: window.queue_free()
	if new_msg: new_msg.queue_free()
	if load_dialog: load_dialog.queue_free()

func open_export_dialog() -> void:
	if state == STATE.NotStarted: return
	
	export_dialog = motley.export_dialog.instantiate()
	add_child(export_dialog)
	export_dialog.file_selected.connect(export_charmap)

func export_charmap(path: String) -> void:
	(viewport.get_node("%MouseSprite") as Node2D).visible = false
	# Horrible delay to allow MouseSprite to become invisible
	await get_tree().create_timer(0.2).timeout
	var img := viewport.get_texture().get_image()

	match path.get_extension():
		"png": img.save_png(path)
		"jpg", "jpeg": img.save_jpg(path)
		"exr": img.save_exr(path)
		"webp": img.save_webp(path)
		"": push_warning("Please select a file type for exporting")
		_: push_warning("Unsupported file type: "+path.get_extension())
	
	(viewport.get_node("%MouseSprite") as Node2D).visible = true
	if export_dialog: export_dialog.queue_free()

### INIT ###

func _start_editing() -> void:
	size = Vector2(
		int(window.get_node("%TilemapWidth").text), 
		int(window.get_node("%TilemapHeight").text))
	tileset = load(window.get_node("%TilesetPath").text)
	tile_size = Vector2(
		int(window.get_node("%TileSizeX").text), 
		int(window.get_node("%TileSizeY").text))
	position += Vector2(size.x * tile_size.x / 2, size.y * tile_size.y / 2)
	#scale = Vector2(2, 2) # TODO
	
	viewport = motley.viewport.instantiate()
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
			rotations[i].append(0)
	
	texture = viewport.get_texture()
	texture_filter = TEXTURE_FILTER_NEAREST
	
	state = STATE.Editing
	if window != null: window.queue_free()
	if new_msg != null: new_msg.queue_free()
	if load_dialog: load_dialog.queue_free()

func _ready() -> void:
	if state != STATE.NotStarted: return
	
	if Engine.is_editor_hint():
		new_msg = motley.new_msg.instantiate()
		add_child(new_msg)
	else:
		window = motley.new_window.instantiate()
		add_child(window)
		(window.get_node("%CreateButton") as Button).pressed.connect(_start_editing)

### UPDATE ###

func _place_tile() -> void:
	if paint_tile: tiles[mouse_tile.x][mouse_tile.y] = selected_tile
	if paint_rotation: rotations[mouse_tile.x][mouse_tile.y] = selected_rotation
	
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
