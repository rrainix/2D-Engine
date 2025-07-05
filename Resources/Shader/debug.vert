// debug.vert
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;

uniform mat4 u_ViewProj;

out vec4 vColor;

void main() {
    gl_Position = u_ViewProj * vec4(aPos,0.0,1.0);
    vColor = aColor;
}