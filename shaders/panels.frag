#version 330 core

in vec2 UV;
out vec4 color;

uniform sampler2D texture0;
uniform int pageHeight;
uniform int height;
uniform int width;
uniform float anim;

const float panelHeight = 0.075; // from 0 to 1

const int   filterSize    = 13;  // must be odd
const float textureSize = 1024.0;

const int   halfFilterSize = filterSize / 2;
const float pixelSize = (1.0 / textureSize);

float Gaussian (float x, float sigma)
{
    return exp(-(x*x) / (6.0 * sigma*sigma));
}

vec3 BlurredPixel (in vec2 uv)
{
    float sigma = 5.0;
    
    float total = 0.0;
    vec3 ret = vec3(0);
        
    for (int iy = 0; iy < filterSize; iy++)
    {
        float fy = Gaussian (float(iy) - float(halfFilterSize), sigma);
        float offsety = float(iy-halfFilterSize) * pixelSize;
        
        for (int ix = 0; ix < filterSize; ix++)
        {
            float fx = Gaussian (float(ix) - float(halfFilterSize), sigma);
            float offsetx = float(ix-halfFilterSize) * pixelSize;
            
            float f = fx*fy;
            total += f;
            ret += texture(texture0, uv + vec2(offsetx, offsety)).rgb * f;
        }
    }
    return ret / total;
}

float relativeY(float y, float an, float ratio) {
	return y * ratio - ratio - an
		+ an / (float(pageHeight) / float(height));
}

void main() {
	vec2 ruv = UV;
	float ratio = float(height) / float(pageHeight);

	// scrolling animation
	ruv.y = relativeY(UV.y, anim, ratio);

	//bottom panel
	if(UV.y <= panelHeight) {
		float distance = float(panelHeight) - UV.y;
		float mirror = relativeY(UV.y + distance * 2, anim, ratio);
		vec2 mirrorUV = vec2(UV.x, mirror);
		color = vec4(BlurredPixel(mirrorUV) + 0.2, 1.0);
		return;
	}

	color = texture(texture0, ruv);
}
