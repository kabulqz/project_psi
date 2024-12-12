uniform float time;
uniform vec2 resolution;
uniform sampler2D texture;

void main()
{
    // Normalize coordinates and map to [-1, 1]
    vec2 uv = gl_FragCoord.xy / resolution;
    uv = uv * 2.0 - 1.0;

    // Apply barrel distortion for CRT corner warping
    float strength = 0.08; // Subtle distortion
    vec2 offset = uv * length(uv) * strength;
    uv += offset;

    // Map back to [0, 1] for texture sampling
    uv = (uv + 1.0) * 0.5;

    // Scale the texture slightly up
    float scale = 0.93; // Subtle scaling factor, closer to 1.0
    uv = (uv - 0.5) * scale + 0.5;

    // Ensure corners detach visibly by clamping UV outside bounds to black
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // Add slight distortion with sin and time
    float distortion = sin(uv.y * 8.0 + time * 2.0) * 0.0005;
    uv.x += distortion;

    // Chromatic aberration
    vec3 col;
    col.r = texture2D(texture, uv + vec2(0.0015, 0.0)).r; // Slight offset for red
    col.g = texture2D(texture, uv).g;                    // No offset for green
    col.b = texture2D(texture, uv - vec2(0.0015, 0.0)).b; // Slight offset for blue

    // Add scanlines (thin, subtle effect)
    float scanline = sin(uv.y * resolution.y * 1.2) * 0.05;
    col *= 1.0 - scanline;

    // Add noise
    float noise = (fract(sin(dot(uv * resolution.xy, vec2(12.9898, 78.233))) * 43758.5453));
    col.rgb += noise * 0.03;

    // Output the final color
    gl_FragColor = vec4(col, 1.0);
}
