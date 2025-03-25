    #version 330 core
    layout (location = 0) in vec3 aPos;      // position variable has attribute position 0
    layout (location = 1) in vec3 aColor;    // color variable has attribute position 1
    layout (location = 2) in vec2 aTexCoord; // texture variable has attribute position 2

    out vec3 ourColor;                      // output a color to the fragment shader
    out vec2 TexCoord;                      // output a texture coordinate to the fragment shader

    uniform mat4 transform;                     // uniform variable for transform matrix

    void main(){
        gl_Position =  transform * vec4(aPos, 1.0);       // Directly give vec3 to vec4 creator
        ourColor = aColor;                  // Set our color to the input color we got from the vertex data
        TexCoord = vec2(aTexCoord.x, aTexCoord.y);               // Set our texture coordinates to the input texture coordinates we got from the vertex data
    }