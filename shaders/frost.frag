#version 330 core

in vec2 UV;
out vec4 color;

uniform sampler2D tex0;
uniform vec2 resolution;
uniform vec2 ksize;

vec4 blur5(vec2 direction) {
  vec4 col = vec4(0.0);
  vec2 off1 = vec2(1.3333333333333333) * direction;
  col += texture2D(tex0, UV) * 0.29411764705882354;
  col += texture2D(tex0, UV + (off1 / resolution)) * 0.35294117647058826;
  col += texture2D(tex0, UV - (off1 / resolution)) * 0.35294117647058826;
  return col; 
}

void main() {
	color = blur5(ksize);
}
