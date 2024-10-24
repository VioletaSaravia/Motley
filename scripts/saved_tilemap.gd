class_name SavedCharmap extends Resource

# Charset info
@export var size: Vector2i
@export var tileset: Texture
@export var tile_size: Vector2i

# Charmap info
@export var tiles: Array[Array] # of Vector2
@export var rotations: Array[Array] # of TileRotation
@export var fore_colors: Image
@export var back_colors: Image
@export var fore_tex: ImageTexture
@export var back_tex: ImageTexture
