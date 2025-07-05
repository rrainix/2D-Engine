// instanced.frag
#version 330 core

in vec2 vUV;
in vec4 vColor;

uniform sampler2D texSampler;  // oder 'uTexture', je nachdem wie du's im C++ setzt

out vec4 FragColor;

void main() {
    vec4 texCol = texture(texSampler, vUV);
    FragColor   = texCol * vColor;
    if (FragColor.a < 0.01) discard;
}
