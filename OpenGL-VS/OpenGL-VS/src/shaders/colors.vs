#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model; // Model matrix	
uniform mat4 view; // View matrix
uniform mat4 projection; // Projection matrix

void main()
{
	// Set the position of the fragment in clip space
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}