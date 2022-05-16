#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 uvCoord;

void main()
{
   
   uvCoord = aTex;

   gl_Position = vec4(aPos,1);
   
};