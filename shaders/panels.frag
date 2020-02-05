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

/*
	blend two colors normally
	dst = destination
	src = source
*/
vec4 normal(vec4 src, vec4 dst) {
	// final alpha
  float alpha = src.a + dst.a *
		(1.0 - src.a);

	return vec4(
		(
			src.rgb * src.a + dst.rgb * dst.a *
			(1.0 - src.a)
		) / alpha,
		alpha
	);
}

vec3 saturation(vec3 rgb, float adjustment)
{
    const vec3 luminance = vec3(
				0.2125,
				0.7154,
				0.0721
		);
    vec3 intensity = vec3(dot(rgb, luminance));
    return mix(intensity, rgb, adjustment);
}

vec4 panel(vec2 uv) {
		vec4 col = texture(
			tex0,
			uv
		);
/*
		// foreground
		col = normal(vec4(0.5), col);

		// saturate background
		col.rgb = saturation(col.rgb, 1.75);
*/
		return col;
}

void main() {
	vec2 ruv = UV;

	// scrolling animation
	ruv.y = viewY(UV.y);

	color = texture(tex0, ruv);

	return;

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

		color = panel(mirror);
		return;
	}

	//top panel
	float topY = 1.0 - panelH;

	if(UV.y >= topY) {
		float dist = UV.y - topY;

		vec2 mirror = vec2(
			UV.x,
			viewY(
				topY - dist
			)
		);

		color = panel(mirror);
		return;
	}

	color = texture(tex0, ruv);
}
