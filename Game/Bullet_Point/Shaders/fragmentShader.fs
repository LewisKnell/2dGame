#version 330


in vec2 f_position;
in vec4 f_color;
in vec2 f_uv;

out vec4 color;
uniform sampler2D mySampler;

void main() {

    vec4 textureSample = texture(mySampler, f_uv);

    color = f_color*textureSample;
}