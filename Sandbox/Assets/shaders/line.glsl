// Renderer2D Circle Shader
#type vertex
#version 450 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in int a_entity_id;

uniform mat4 u_view_projection;

out vec4 v_color;
out flat int v_entity_id;

void main() {
	v_color = a_color;
	v_entity_id = a_entity_id;
	gl_Position = u_view_projection * vec4(a_position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 v_color;
in flat int v_entity_id;

void main() {
	color = v_color;
	color2 = v_entity_id;
}
