uniform sampler2D fromTexture;   // The previous texture (the "from" texture)
uniform sampler2D toTexture;     // The next texture (the "to" texture)
uniform float progress;          // Progress of the transition (0.0 to 1.0)
uniform vec2 resolution;         // Resolution of the screen/texture (width, height)

void main() {
    vec2 uv = gl_FragCoord.xy / resolution;  // Normalize coordinates based on resolution

    // Calculate the effect of VHS channel change (horizontal tearing and noise)
    float tearOffset = sin(uv.y * 100.0 + progress * 10.0) * 0.03;  // Horizontal noise based on progress
    uv.x += tearOffset;  // Apply horizontal distortion based on "tear" effect

    // Fetch the colors from both textures
    vec4 fromColor = texture2D(fromTexture, uv);
    vec4 toColor = texture2D(toTexture, uv);

    // Smooth transition with smoothstep to ease the progress
    float smoothProgress = smoothstep(0.0, 1.0, progress);

    // Blend between the two textures based on smoothed progress
    vec4 blendedColor = mix(fromColor, toColor, smoothProgress);

    // Add VHS-style static noise (color flicker and distortion)
    float noise = (sin(uv.x * 20.0 + smoothProgress * 50.0) + cos(uv.y * 10.0 + smoothProgress * 30.0)) * 0.1;
    blendedColor.rgb += noise;  // Apply the noise to the color channels

    // Apply a slight color shift to simulate the VHS glitchy effect
    vec3 colorShift = vec3(0.0, 0.0, 0.2) * sin(smoothProgress * 3.0);  // Color shift effect based on smooth progress
    blendedColor.rgb += colorShift;

    // Final output with VHS-like channel change effect
    gl_FragColor = blendedColor;
}
