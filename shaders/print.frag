#version 330 core

in vec2 UV;
out vec4 color;

uniform sampler2D tex;
uniform float scale;

void main() {
    color = texture2D(tex, UV * scale);
}

