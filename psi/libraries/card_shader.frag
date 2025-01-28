#version 130

in vec2 texCoords;  // Wspó³rzêdne tekstury przekazane z vertex shadera
uniform sampler2D texture; // Tekstura karty

out vec4 fragColor; // Koñcowy kolor piksela

void main()
{
    fragColor = texture(texture, texCoords); // Wczytanie koloru z tekstury
}
