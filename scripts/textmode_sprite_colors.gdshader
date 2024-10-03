shader_type canvas_item;

varying vec2 coords;

uniform vec3 colors_fore[10240];
uniform vec3 colors_back[10240];

uniform vec2 canvas_size;
uniform vec2 tile_size;

void vertex() {
	coords = vec2(
		VERTEX.x / (canvas_size.x * tile_size.x) + 0.04545, // ???
		VERTEX.y / (canvas_size.y * tile_size.y) + 0.05555
	);
}

void fragment() {
	float color_x = coords.x * canvas_size.x;
	float color_y = coords.y * canvas_size.y;
	modf(color_x, color_x);
	modf(color_y, color_y);
	
	COLOR = texture(TEXTURE, UV);
	
	int coord = int(color_y * canvas_size.x + color_x);
	vec3 final_color = COLOR.r > 0.0 ? colors_fore[coord] : colors_back[coord];
	COLOR = vec4(final_color, 1);
	
	//COLOR = texture(TEXTURE, UV);
}