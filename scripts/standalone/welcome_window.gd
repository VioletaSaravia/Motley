extends Window

func close_window() -> void:
	queue_free()

func _ready() -> void:
	close_requested.connect(close_window)

func _process(_delta: float) -> void:
	if Input.is_anything_pressed(): close_window()
