#version 330

in vec2 position;
in vec4 color;
in vec2 uv;

out vec2 f_position;
out vec4 f_color;
out vec2 f_uv;

uniform mat4 transformationMatrix;


void main() {
   
    gl_Position = transformationMatrix * vec4(position, 0.0, 1.0);
   
    f_position = position;
    f_color = color;
	f_uv = vec2(uv.x,1-uv.y);

}