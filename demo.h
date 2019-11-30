#pragma once

#include "shader.h"

class GLFWwindow;

namespace sandbox {
    
class Demo {
    
    int m_argc;
    char **m_argv;
    GLFWwindow* m_window = 0;
		bool m_record = false;

    unsigned vertexArrayObject,
						vertexBufferObject,
						elementBufferObject;

		unsigned texWithPanels,
						 fbWithPanels;

		unsigned texPage;

    int pageWidth,
				pageHeight,
				pageBytesPerPixel;

		unsigned loadImageAndPutDetails(
			const char* path,
			int *width,
			int *height,
			int *channels
		);

    unsigned loadImage(const char*);
    unsigned createTexture(int, int);
    
    void reallocFramebuffer(unsigned&, unsigned&, int, int);

		void parseArguments();

		void initGLFW();
		void initGLEW();

public:

		const int defaultWidth = 360,
						defaultHeight = 670;

		Shader *panelShader;
		Shader *frostShader;
		Shader *printShader;

    Demo(int, char**);
    ~Demo();
    
		float framerate = 60.0;
		float scale = 1.0;
		
		int run();
    
};
    
} // end namespace sandbox

