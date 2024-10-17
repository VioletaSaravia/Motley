shader_type canvas_item;

uniform vec3 foreground;
uniform vec3 background;

void vertex() {}

void fragment() {
	COLOR.rgb = texture(TEXTURE, UV).r > 0.0 ? foreground : background;
}