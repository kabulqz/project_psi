uniform sampler2D fromTexture;   // The previous texture (the "from" texture)
uniform sampler2D toTexture;     // The next texture (the "to" texture)
uniform float progress;          // Progress of the transition (0.0 to 1.0)
uniform vec2 resolution;         // Resolution of the screen/texture (width, height)
uniform float duration;          // Duration of the transition

// Helper function to fetch color from the "from" texture
vec4 getFromColor(vec2 p) {
    return texture2D(fromTexture, p);
}

// Helper function to fetch color from the "to" texture
vec4 getToColor(vec2 p) {
    return texture2D(toTexture, p);
}

// The transition effect function
vec4 transition(vec2 p) {
    // Create a "block" for the noise pattern
    vec2 block = floor(p.xy / vec2(16));

    // Generate noise UV coordinates based on progress
    vec2 uv_noise = block / vec2(64);
    uv_noise += floor(vec2(progress) * vec2(1200.0, 3500.0)) / vec2(64);

    // Calculate the distortion effect based on progress
    vec2 dist = progress > 0.0 ? (fract(uv_noise) - 0.5) * 0.3 * (1.0 - progress) : vec2(0.0);

    // Apply distortion in different directions for red, green, and blue channels
    vec2 red = p + dist * 0.2;
    vec2 green = p + dist * 0.3;
    vec2 blue = p + dist * 0.5;

    // Mix colors from the "from" and "to" textures based on progress
    return vec4(
        mix(getFromColor(red), getToColor(red), progress).r,
        mix(getFromColor(green), getToColor(green), progress).g,
        mix(getFromColor(blue), getToColor(blue), progress).b,
        1.0
    );
}

void main() {
    // Normalize the fragment coordinates
    vec2 uv = gl_FragCoord.xy / resolution;

    // Apply the transition effect
    vec4 blendedColor = transition(uv);

    // Set the final color
    gl_FragColor = blendedColor;
}