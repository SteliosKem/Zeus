#version 330 core
uniform sampler2D u_texture;
uniform vec2 u_resolution;
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 a_tex_coords;

out vec2 o_tex_coords;
out sampler2D o_texture;
out vec2 o_resolution;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}

in vec2 o_tex_coords;
uniform sampler2D u_texture;
uniform vec2 u_resolution;

void main()
{
    vec2 uv = o_tex_coords.xy / o_tex_coords.xy;
    uv *= 1.0 - uv.yx;
    float vig = uv.x * uv.y * 15.0;
    vig = pow(vig, 0.2);
    gl_FragColor = vec4(vig, 1.0);
}