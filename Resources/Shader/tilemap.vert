#version 330 core

layout(location = 0) in vec2 aPos;  
layout(location = 1) in vec2 aUV;    

uniform mat4 projection;
uniform mat4 view;

uniform vec2  uMapSize;
uniform vec2  uTileSize;  
out vec2 vUV;
flat out ivec2 vTileCoord;

void main() {
    vec2 worldPos = aPos * uTileSize;
    vTileCoord = ivec2(floor(aPos));
    vUV = fract(aUV * uMapSize);
    gl_Position = projection * view * vec4(worldPos, 0.0, 1.0);
}