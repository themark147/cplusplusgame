#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;
in uint vertexColor;

// Out variables
out vec4 vertexColorOut;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * aPos;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	vertexColorOut = vec4((vertexColor & 0xAA0000u) >> 16, (vertexColor & 0x00AA00u) >> 8, vertexColor & 0x0000AAu, 0xFF);
}