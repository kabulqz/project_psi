uniform sampler2D texture;

void main() {
	vec2 uv = gl_TexCoord[0].xy;
	vec4 color = texture2D(texture, uv);

	float luminance = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;

	gl_FragColor = vec4(vec3(luminance), color.a);
}