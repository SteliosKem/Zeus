// Renderer2D Circle Shader
#type vertex
#version 450 core

layout(location = 0) in vec3 a_world_position;
layout(location = 1) in vec3 a_local_position;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_thickness;
layout(location = 4) in float a_fade;
layout(location = 5) in int a_entity_id;

uniform mat4 u_view_projection;

out vec4 v_color;
out vec3 v_local_position;
out float v_fade;
out float v_thickness;
out flat int v_entity_id;

void main() {
	v_local_position = a_local_position;
	v_color = a_color;
	v_fade = a_fade;
	v_thickness = a_thickness;
	v_entity_id = a_entity_id;
	gl_Position = u_view_projection * vec4(a_world_position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 v_color;
in vec3 v_local_position;
in float v_fade;
in float v_thickness;
in flat int v_entity_id;

uniform vec4 u_color;

void main() {
	float distance = 1.0 - length(v_local_position);
	float alpha = smoothstep(0.0, v_fade, distance);
	alpha *= smoothstep(v_thickness + v_fade, v_thickness, distance);

	if(alpha == 0.0)
		discard;

	color = v_color;
	color.a *= alpha;

	color2 = v_entity_id;
}
