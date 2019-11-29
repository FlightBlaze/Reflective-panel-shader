#pragma once

#include "shader.h"

class GLFWwindow;

namespace sandbox {
    
class Demo {
    
    int m_argc;
    char **m_argv;
    GLFWwindow* m_window = 0;
    
		unsigned loadImageAndPutDetails(
			const char* path,
			int *width,
			int *height,
			int *channels
		);

    unsigned loadImage(const char*);
    unsigned createTexture(int, int);
    
    void reallocFramebuffer(unsigned&, unsigned&, int, int);

		void initGLFW();
		void initGLEW();

public:

		const int defaultWidth = 360,
						defaultHeight = 670;

		Shader *panelShader;
		Shader *frostShader;
		Shader *normalShader;

    unsigned vertexArrayObject,
						vertexBufferObject,
						elementBufferObject;

    Demo(int, char**);
    ~Demo();
    
    int run();
    
};
    
} // end namespace sandbox

