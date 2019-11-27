#version 330 core

in vec2 UV;
out vec4 color;

uniform sampler2D tex0;
uniform float pageH;
uniform float height;
uniform float scroll;

// panel height from 0 to 1
const float panelH = 0.075;

float pages = pageH / height;
float ratio = height / pageH;

/*
	get Y inside viewport using screen Y,
	amount of pages, ratio and scroll
	position
*/
float viewY(float y) {
	return y * ratio + scroll / pages
		- ratio - scroll;
}

void main() {
	vec2 ruv = UV;

	// scrolling animation
	ruv.y = viewY(UV.y);

	//bottom panel
	if(UV.y <= panelH) {
		/*
			distance between panel top frame and
			screen Y
		*/
		float dist = panelH - UV.y;

		vec2 mirror = vec2(
			UV.x,
			viewY(
				panelH + dist
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
