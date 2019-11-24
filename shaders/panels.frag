#version 330 core

in vec2 UV;
out vec4 color;

uniform sampler2D tex0;
uniform float pageH;
uniform float height;
uniform float width;
uniform float anim;

const float panelH = 0.075; // from 0 to 1

float pages = pageH / height;

/*
	get Y inside viewport using screen Y,
	scroll position (from 0 to 1) and ratio
*/
float viewY(float y, float scro, float ratio) {
	return y * ratio + scro / pages
		- ratio - scro;
}

void main() {
	vec2 ruv = UV;
	float ratio = height / pageH;

	// scrolling animation
	ruv.y = viewY(UV.y, anim, ratio);

	//bottom panel
	if(UV.y <= panelH) {
		float distance = panelH - UV.y;

		vec2 mirror = vec2(
			UV.x,
			viewY(
				UV.y + distance * 2,
				anim,
				ratio
			)
		);

		color = texture(
			tex0,
			mirror
		) + vec4(0.2);

		return;
	}

	color = texture(tex0, ruv);
}
