#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform float scalex = 1.0f, scaley = 1.0f;
uniform float x, y;
uniform vec3 flatColor = vec3(1.0f, 1.0f, 1.0f);

void main()
{
   gl_Position = vec4(aPos.x * scalex + x, aPos.y * scaley + y, 0.0, 1.0);
   color = aColor * flatColor;
}