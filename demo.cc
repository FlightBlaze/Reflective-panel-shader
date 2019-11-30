#include "demo.h"

// Important: include GLEW before other graphic libraries
#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <GL/gl.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include "third_party/stb_image/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opencv2/opencv.hpp>

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

GLfloat quadVertices[] = {
    // Position    // UV
   -1.0f,   1.0f,  0.0f, 1.0f,
   -1.0f,  -1.0f,  0.0f, 0.0f,
    1.0f,  -1.0f,  1.0f, 0.0f,

   -1.0f,   1.0f,  0.0f, 1.0f,
    1.0f,  -1.0f,  1.0f, 0.0f,
    1.0f,   1.0f,  1.0f, 1.0f
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

void Demo :: parseArguments() {
		for(int i = 0; i < m_argc; i++) {
			
		}
}

Demo :: Demo(int argc, char **argv):
    m_argc(), m_argv() {

		parseArguments();
    
		initGLFW(); 
		initGLEW();

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
		 
		// Page texture & details
		
    this->texPage = loadImageAndPutDetails(
			"./resources/page.png",
			&this->pageWidth,
			&this->pageHeight,
			&this->pageBytesPerPixel
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
		
		printShader = new Shader(
        "./shaders/print.vert",
        "./shaders/print.frag"
    );
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
		const int virtualWidth = this->defaultWidth * this->scale,
						virtualHeight = this->defaultHeight * this->scale;

		// Init OpenCV

		cv::VideoWriter outputVideo;
		outputVideo.open(
			"output.avi",
			0x34363248, // H264 (fourcc)
			this->framerate,
			cv::Size(
				virtualWidth,
				virtualHeight
			),
			true
		);

		// Main cycle

    int recentWidth,
				recentHeight,
				frame = 0;
    
    do {
        int width, height;
        
        // glfwGetFramebufferSize(m_window, &width, &height);
       
				width = virtualWidth;
				height = virtualHeight;

				// Create or resize textures
        
        if(height != recentHeight || width != recentWidth) {
            recentHeight = height;
            recentWidth = width;
            
            reallocFramebuffer(fbWithPanels, texWithPanels, width, height);
        }
       
				// Setup viewport

				glViewport(0, 0, width, height);
        glBindVertexArray(vertexArrayObject);

				GLuint program;
        GLfloat anim; //(sin(glfwGetTime() / 2) / 2) + 0.5;

        glfwSwapBuffers(m_window);
        glfwPollEvents();
        
				anim = (float)frame / this->framerate / 8.0;

				// Panel shader

				switchFramebuffer(fbWithPanels);
				program = this->panelShader->use();
        
        glBindTexture(GL_TEXTURE_2D, texPage);
        glUniform1i(glGetUniformLocation(program, "tex0"), 0);
				glUniform1f(glGetUniformLocation(program, "pageH"), pageHeight);
				glUniform1f(glGetUniformLocation(program, "height"), defaultHeight);
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
				program = this->printShader->use();
        
        glBindTexture(GL_TEXTURE_2D, texWithPanels);
        glUniform1i(glGetUniformLocation(program, "tex"), 0);
        glUniform1f(glGetUniformLocation(program, "scale"), scale);

        glDrawArrays(GL_TRIANGLES, 0, 6);

				// Write pixels
				
				cv::Mat pixels(height, width, CV_8UC3);
				
				glBindTexture(GL_TEXTURE_2D, texWithPanels);
				glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data);

				// opencv readable matrix
				cv::Mat cv_pixels(height, width, CV_8UC3);

				// convert
				
				
				for(int y=0; y<height; y++) for(int x=0; x<width; x++) {
						cv_pixels.at<cv::Vec3b>(y,x)[2] = pixels.at<cv::Vec3b>(height-y-1,x)[0];
						cv_pixels.at<cv::Vec3b>(y,x)[1] = pixels.at<cv::Vec3b>(height-y-1,x)[1];
						cv_pixels.at<cv::Vec3b>(y,x)[0] = pixels.at<cv::Vec3b>(height-y-1,x)[2];
				}

				// put frame
				outputVideo << cv_pixels;

				frame++;
    }
    while (!glfwWindowShouldClose(m_window));
   
		outputVideo.release();

    return 0;
}

Demo :: ~Demo() {
	delete panelShader;
	delete frostShader;
	delete printShader;
		
  glDeleteFramebuffers(1, &fbWithPanels);
  glDeleteTextures(1, &texWithPanels);
  glDeleteTextures(1, &texPage);

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

