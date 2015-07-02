#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;

void main( void )
{
//     color = vec4(0.0f,1.0f,0.0,0.0);
     color = vec4(texture(texture_diffuse1, TexCoords));
//     color = color + vec4(texture(texture_diffuse2, TexCoords));
//     color = color + vec4(texture(texture_diffuse3, TexCoords));
//     color = color + vec4(texture(texture_specular1, TexCoords));
}
