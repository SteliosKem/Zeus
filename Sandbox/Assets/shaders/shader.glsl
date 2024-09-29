#type vertex
#version 450 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texture_coord;
layout(location = 3) in float a_texture_index;
layout(location = 4) in float a_tiling_factor;
layout(location = 5) in int a_entity_id;

uniform mat4 u_view_projection;

out vec4 v_color;
out vec2 v_texture_coord;
out flat float v_texture_index;
out float v_tiling_factor;
out flat int v_entity_id;

void main() {
	v_texture_coord = a_texture_coord;
	v_color = a_color;
	v_texture_index = a_texture_index;
	v_tiling_factor = a_tiling_factor;
	v_entity_id = a_entity_id;
	gl_Position = u_view_projection * vec4(a_position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 v_color;
in vec2 v_texture_coord;
in flat float v_texture_index;
in float v_tiling_factor;
in flat int v_entity_id;

uniform vec4 u_color;

uniform sampler2D u_textures[32];

void main() {
				
	//color = vec4(v_texture_coord, 0.0f, 1.0f);
	color = texture(u_textures[int(v_texture_index)], v_texture_coord * v_tiling_factor) * v_color;
	//color = int(v_texture_index) * v_color;

	color2 = v_entity_id;
}
