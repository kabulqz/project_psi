#version 130

in vec2 texCoords;  // Wsp�rz�dne tekstury przekazane z vertex shadera
uniform sampler2D texture; // Tekstura karty

out vec4 fragColor; // Ko�cowy kolor piksela

void main()
{
    fragColor = texture(texture, texCoords); // Wczytanie koloru z tekstury
}
