#version 330 core
in vec3 fragColor;
in vec2 texCoord;
out vec4 color;

uniform sampler2D atlas;

void main() {
    // Sample aus dem Atlas mit den berechneten UV-Koordinaten
    color = texture(atlas, texCoord) * vec4(fragColor, 1.0);
}