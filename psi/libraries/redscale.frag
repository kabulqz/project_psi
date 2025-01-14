uniform sampler2D texture;

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    vec4 color = texture2D(texture, uv);

    // Compute the luminance using the standard coefficients
    float luminance = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;

    // Apply a factor to enhance the red component
    float redEnhanceFactor = 0.8; // Adjust this to control redness intensity
    vec3 redTint = vec3(luminance * redEnhanceFactor, luminance * 0.2, luminance * 0.2);

    gl_FragColor = vec4(redTint, color.a);
}