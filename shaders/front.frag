/*
The MIT License (MIT)

Copyright (C) 2015 Jam3
Copyright (C) 2019 Dmitry Kozichev

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#version 330 core

in vec2 UV;
out vec4 color;

uniform sampler2D tex0;
uniform vec2 resolution;
uniform vec2 dir;

// panel height from 0 to 1
const float panelH = 0.075;

vec4 blur5(vec2 direction) {

	float topY = 1.0 - panelH;

	// bottom panel
	if(UV.y <= panelH) {
		direction *= vec2(
				UV.y / panelH - 1.0) / 2;
	}

	// top panel
	else if(UV.y >= topY) {
		direction *= vec2(
				(topY - UV.y) / panelH) / 2;
	}

	else
		return texture(tex0, UV);


	/* these magic numbers are probably
		 calculated using http://dev.theomader.com/
		 gaussian-kernel-calculator
	*/
  vec2 off1 = vec2(1.3333333333333333)
		* direction;

	/* this code in not mine and i don't know
		 what does this this variable mean */
	vec2 division = off1 / resolution;

  vec4 col = texture(tex0, UV)
		* 0.29411764705882354;
	
  col += texture(tex0, UV + division)
		* 0.35294117647058826;

  col += texture(tex0, UV - division)
		* 0.35294117647058826;
	
  return col; 
}

void main() {
	color = blur5(dir);
}
