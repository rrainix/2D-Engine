// tilemap.frag
#version 330 core

in vec2 vUV;
flat in ivec2 vTileCoord; 
uniform usampler2D uTileMap;   // integer-Index-Textur
uniform sampler2D  uTileAtlas; // Sprite-Atlas

uniform vec2 uTileSize;        // Pixelgröße eines Tiles

out vec4 FragColor;

void main() {
    uint tileIndex = texelFetch(uTileMap, vTileCoord, 0).r;
    ivec2 atlasSize = textureSize(uTileAtlas, 0);
    int tilesPerRow = atlasSize.x / int(uTileSize.x);

    ivec2 tilePosInAtlas = ivec2(
        int(tileIndex) % tilesPerRow,
        int(tileIndex) / tilesPerRow
    );

    vec2 atlasOffset = vec2(tilePosInAtlas) * uTileSize;
    vec2 sampleUV   = (atlasOffset + vUV * uTileSize) / vec2(atlasSize);

    FragColor = texture(uTileAtlas, sampleUV);
}
