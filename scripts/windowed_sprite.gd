@tool
extends Window

@onready var sprite := %TextModeSprite

func close_window() -> void:
	self.queue_free()

func _ready() -> void:
	unresizable = true
	close_requested.connect(close_window)
	self.size = sprite.window.size
	title = "New tilemap"

func _process(delta: float) -> void:
	match sprite.state:
		TextModeSprite.STATE.Editing:
			self.size = sprite.viewport.size
			title = "*Unsaved"
		TextModeSprite.STATE.NotStarted:
			self.size = sprite.window.size
