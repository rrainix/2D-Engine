// simple_instanced.vert
#version 330 core

layout(location = 0) in vec2 aPos;       // Quad-Position
layout(location = 1) in vec2 aUV;        // Quad-UV

layout(location = 3) in vec3 instanceRow0;
layout(location = 4) in vec3 instanceRow1;
layout(location = 5) in vec3 instanceRow2;

layout(location = 7) in vec4 instanceColor;    // Instanz-Farbe

uniform mat4 projection;
uniform mat4 view;

out vec2 vUV;
out vec4 vColor;

void main() {
    mat3 model = mat3(instanceRow0, instanceRow1, instanceRow2);
    vec3 pos = model * vec3(aPos, 1.0);
    gl_Position = projection * view * vec4(pos.xy, 0.0, 1.0);

    vUV    = aUV;
    vColor = instanceColor;
}
