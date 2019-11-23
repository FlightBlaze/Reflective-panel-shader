#version 330 core

in vec2 UV;
out vec4 color;

uniform sampler2D texture0;
uniform int pageHeight;
uniform int height;
uniform int width;
uniform float anim;

const int panelHeight = 32; // in pixels

void main() {
	vec2 ruv = UV;
	float ratio = float(height) / float(pageHeight);

	// scrolling animation
	ruv.y = ruv.y * ratio - ratio - anim
		+ anim / (float(pageHeight) / float(height));

	color = texture(texture0, ruv);
}
