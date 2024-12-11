uniform float time;
uniform vec2 resolution;
uniform sampler2D texture;

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution;

    // Add slight distortion with sin and time
    float distortion = sin(uv.y * 8.0 + time * 2.0) * 0.0005;
    uv.x += distortion;

    // Create chromatic aberration by sampling the texture at slightly offset UVs
    vec3 col;
    col.r = texture2D(texture, uv + vec2(0.002, 0.0)).r; // Red channel
    col.g = texture2D(texture, uv).g;                   // Green channel
    col.b = texture2D(texture, uv - vec2(0.002, 0.0)).b; // Blue channel

    // Add scanlines
    float scanline = sin(uv.y * 1500.0) * 0.02; // Higher frequency for thinner lines
    col *= 1.0 - scanline;

    // Add noise
    float noise = (fract(sin(dot(uv * resolution.xy, vec2(12.9898, 78.233))) * 43758.5453));
    col.rgb += noise * 0.03;

    gl_FragColor = vec4(col, 1.0);
}
