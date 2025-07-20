    #version 330 core
    layout (location = 0) in vec3 aPos;      // position variable has attribute position 0
    layout (location = 1) in vec2 aTexCoord; // texture variable has attribute position 1

out vec2 TexCoord;                      // output a texture coordinate to the fragment shader

    //uniform mat4 transform;                     // uniform variable for transform matrix
    
    uniform mat4 model;                        // uniform variable for model matrix
    uniform mat4 view;                         // uniform variable for view matrix
    uniform mat4 projection;                   // uniform variable for projection matrix


    void main(){
        gl_Position =  projection * view * model * vec4(aPos, 1.0);       // Directly give vec3 to vec4 creator
        TexCoord = vec2(aTexCoord.x, aTexCoord.y);               // Set our texture coordinates to the input texture coordinates we got from the vertex data
    }