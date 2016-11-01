#version 150
in vec3 position;
in vec3 color;

out vec3 ourColor;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(position, 1.0f);
    //gl_Position = vec4(position, 1.0f);
     ourColor = color;
}