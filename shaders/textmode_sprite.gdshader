shader_type canvas_item;

uniform sampler2D fore_colors;
uniform sampler2D back_colors;

void vertex() {}

void fragment() {
	COLOR = texture(TEXTURE, UV).r > 0.0 
		? texture(fore_colors, SCREEN_UV) 
		: texture(back_colors, SCREEN_UV);
}