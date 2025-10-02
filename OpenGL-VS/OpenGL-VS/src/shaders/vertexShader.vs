#version 330 core
layout (location = 0) in vec3 aPos;      // 위치 데이터

    
uniform mat4 model;                        // uniform variable for model matrix
uniform mat4 view;                         // uniform variable for view matrix
uniform mat4 projection;                   // uniform variable for projection matrix


void main(){
    gl_Position =  projection * view * model * vec4(aPos, 1.0);       // Directly give vec3 to vec4 creator
}