#include "demo.h"

// Important: include GLEW before other graphic libraries
#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <GL/gl.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include "third_party/stb_image/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <opencv2/opencv.hpp>

static
void key_callback(GLFWwindow*, int, int, int, int);
void error_callback(int, const char*);
static
int error(const char*);

void GLAPIENTRY
MessageCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(
        stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message
    );
}

namespace sandbox {

GLfloat vertices[] = {
     // Position          // Color            // UVs
     1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top right
     1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom right
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom left
    -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top left
};

GLfloat quadVertices[] = {
    // Position    // UV
   -1.0f,   1.0f,  0.0f, 1.0f,
   -1.0f,  -1.0f,  0.0f, 0.0f,
    1.0f,  -1.0f,  1.0f, 0.0f,

   -1.0f,   1.0f,  0.0f, 1.0f,
    1.0f,  -1.0f,  1.0f, 0.0f,
    1.0f,   1.0f,  1.0f, 1.0f
};

const GLuint indices[] = {
    0, 1, 3,
    1, 2, 3
};

unsigned Demo :: loadImageAndPutDetails(             
	const char* path, int *width,       
	int *height, int *channels) {
	
	stbi_set_flip_vertically_on_load(true);
	    
	unsigned char *image = stbi_load(
		path,
		width,
		height,
		channels,
		STBI_rgb_alpha
	);
	    
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	  
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);
	    
	return texture;
}

unsigned Demo :: loadImage(const char* path) {
	int width, height, channels;
    
	return loadImageAndPutDetails(path, &width, &height, &channels);
}

unsigned Demo :: createTexture(int width, int height) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return tex;
}

void Demo :: reallocFramebuffer(unsigned &framebuffer, unsigned &texture, int width, int height) {
    if(framebuffer != 0)
        glDeleteFramebuffers(1, &framebuffer);
    glGenFramebuffers(1, &framebuffer);
    
    if(texture != 0)
        glDeleteTextures(1, &texture);
    texture = createTexture(width, height);
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Demo :: Demo(int argc, char **argv):
    m_argc(), m_argv() {
    
    
}

void Demo :: initGLFW() {
    if (!glfwInit()) {
			std::cerr << "Failed to initialize GLFW3\n";
		}

    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
    m_window = glfwCreateWindow(defaultWidth, defaultHeight, "Demo", 0, 0);
    
    glfwSetKeyCallback(m_window, key_callback);
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
}

void Demo :: initGLEW() {
    glewExperimental = GL_TRUE;
    
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
				exit(1);
		}
}

/*
	Switch framebuffer and clear
*/
void switchFramebuffer(GLuint fb) {
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
}

int Demo :: run() {
		initGLFW(); 
		initGLEW();

		// Init OpenCV pixel buffer
		
		// cv::Mat pixels( height, width, CV_8UC3 );
    
    // Enable debug 
    
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
   
    // Arrays and buffers
    
    unsigned int vertexArrayBuffer, vertexBufferObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		 
    // Make textures
    
    GLuint texture0 = loadImage("./resources/photo1.png");
   
		// Page texture & detauls

    GLuint texPage;

    GLint pageWidth,
					pageHeight,
					pageBytesPerPixel;

    texPage = loadImageAndPutDetails(
			"./resources/page.png",
			&pageWidth,
			&pageHeight,
			&pageBytesPerPixel
    );
    
    // Load shaders
		
    panelShader = new Shader(
        "./shaders/panels.vert",
        "./shaders/panels.frag"
    );
		
		frostShader = new Shader(
        "./shaders/frost.vert",
        "./shaders/frost.frag"
    );
		
		normalShader = new Shader(
        "./shaders/blending/normal.vert",
        "./shaders/blending/normal.frag"
    );

		// Framebuffers and textures
    
    unsigned int fbWithPanels = 0,
                texWithPanels = 0;

    // Main cycle
    
    int recentWidth,
				recentHeight;
    
    do {
        int width, height;
        
        glfwGetFramebufferSize(m_window, &width, &height);
        
        // Create or resize textures
        
        if(height != recentHeight || width != recentWidth) {
            recentHeight = height;
            recentWidth = width;
            
            reallocFramebuffer(fbWithPanels, texWithPanels, width, height);
        }
       
				// Setup viewport

				glViewport(0, 0, width, height);
        glBindVertexArray(vertexArrayObject);

        GLfloat anim = (sin(glfwGetTime()) / 2) + 0.5;
				GLuint program;

        glfwSwapBuffers(m_window);
        glfwPollEvents();
        
				// Panel shader

				switchFramebuffer(fbWithPanels);
				program = this->panelShader->use();
        
        glBindTexture(GL_TEXTURE_2D, texPage);
        glUniform1i(glGetUniformLocation(program, "tex0"), 0);
				glUniform1f(glGetUniformLocation(program, "pageH"), pageHeight);
				glUniform1f(glGetUniformLocation(program, "height"), height);
        glUniform1f(glGetUniformLocation(program, "scroll"), anim);
        glDrawArrays(GL_TRIANGLES, 0, 6);
      
				// Frost shader

				program = this->frostShader->use();
        
        glBindTexture(GL_TEXTURE_2D, texWithPanels);
        glUniform1i(glGetUniformLocation(program, "tex0"), 0);
				glUniform2f(glGetUniformLocation(program, "resolution"), defaultWidth, defaultHeight);
        
				// frost (progressive blur)

				const int iterations = 17;

				for(int i = 0; i < iterations; i++) {
					float radius = iterations - i - 1;

					// dir = direction
					glUniform2f(glGetUniformLocation(program, "dir"),
							i % 2 == 0 ? radius : 0,
							i % 2 == 0 ? 0 : radius
					);
					glDrawArrays(GL_TRIANGLES, 0, 6);
				}
       
				// Print to screen
				
				switchFramebuffer(0);
				program = this->normalShader->use();
        
        glBindTexture(GL_TEXTURE_2D, texWithPanels);
        glUniform1i(glGetUniformLocation(program, "texSrc"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    while (!glfwWindowShouldClose(m_window));
    
    return 0;
}

Demo :: ~Demo() {
	delete panelShader;
	delete frostShader;
	delete normalShader;
/*		
  glDeleteFramebuffers(1, &fbWithPanels);
  glDeleteTextures(1, &texWithPanels);
*/
  glDeleteVertexArrays(1, &vertexArrayObject);
  glDeleteBuffers(1, &elementBufferObject);
  glDeleteBuffers(1, &vertexBufferObject);

	glfwDestroyWindow(m_window);
  glfwTerminate();
}

} // end namespace sandbox


void error_callback(int error, const char* description)
 {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static int error(const char *s) {
    printf("%s\n", s);
    return -1;
}

