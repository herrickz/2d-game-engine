#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec4 color;
out vec2 TexCoord;

void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   color = vec4(gl_Position.x, gl_Position.y, gl_Position.z, 1.0);
   TexCoord = aTex;
};
