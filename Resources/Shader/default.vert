#version 330 core
layout (location = 0) in vec2 aPos; 
layout (location = 1) in vec3 aColor;     
layout (location = 2) in vec2 aTexUV;       

// atlasRect enthält den Offset (x,y) und den Skalierungsfaktor (z,w)
// für den Bereich im Atlas
layout (location = 6) in vec4 atlasRect;

uniform mat4 projection;
uniform mat4 view;  
uniform mat3 model;     

out vec3 fragColor;     
out vec2 texCoord;  

void main() {
    vec3 pos = model * vec3(aPos, 1.0);
    gl_Position = projection * view * vec4(pos.xy, 0.0, 1.0);
    
    fragColor = aColor;
    // Berechne die angepassten UV-Koordinaten:
    // aTexUV im Bereich [0,1] wird zuerst skaliert (atlasRect.zw)
    // und dann um den Offset (atlasRect.xy) verschoben.
    texCoord = aTexUV * atlasRect.zw + atlasRect.xy;
}