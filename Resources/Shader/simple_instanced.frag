// simple_instanced.frag
#version 330 core

in vec2 vUV;
in vec4 vColor;

uniform sampler2D uTexture;

out vec4 FragColor;

void main() {
    vec4 texCol = texture(uTexture, vUV);
    FragColor = texCol * vColor;

    // Optional: fully transparente Pixel verwerfen
    if (FragColor.a < 0.01) discard;
}
