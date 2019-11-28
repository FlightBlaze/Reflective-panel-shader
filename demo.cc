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

#include "shader.h"

#include "third_party/stb_image/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

void Demo :: recreateSquareFramebuffer(unsigned &framebuffer, unsigned &texture, int width, int height) {
    int nwidth = width, nheight = height;
    
    if(width > height)
        nheight *= width / height;
    else
        nwidth *= height / width;
    
    recreateFramebuffer(framebuffer, texture, nwidth, nheight);
}

void Demo :: recreateFramebuffer(unsigned &framebuffer, unsigned &texture, int width, int height) {
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

Demo :: ~Demo() {
}

int Demo :: run() {
    // Init
    
    if (!glfwInit())
        return error("Failed to initialize GLFW3");
    
    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
		const int owidth = 360, oheight = 760;

    m_window = glfwCreateWindow(owidth, oheight, "Demo", 0, 0);
    
    glfwSetKeyCallback(m_window, key_callback);
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    
    // Init GLEW
    
    glewExperimental = GL_TRUE;
    
    if (glewInit() != GLEW_OK)
        return error("Failed to initialize GLEW");
    
    // Enable debug 
    
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
   
    // Create buffers and array
    
    GLuint vertexBufferObject, vertexArrayObject, elementBufferObject;
    
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &elementBufferObject);
    
    // Copy vertices
    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
    
    // Copy indices
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Set pointers to vertex attributes
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Position
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat))); // Color
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat))); // UVs
    glEnableVertexAttribArray(2);
    
    // Unbind buffers and array
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Make textures
    
    GLuint texture0 = loadImage("./resources/photo1.png");
    
    GLuint texPage;
    GLint pageWidth, pageHeight, pageBytesPerPixel;
    texPage = loadImageAndPutDetails(
		"./resources/page.png",
		&pageWidth,
		&pageHeight,
		&pageBytesPerPixel
    );
    
    // screen VAO
    
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
   
    // Load shaders
		
    Shader panelShader(
        "./shaders/panels.vert",
        "./shaders/panels.frag"
    );
		
		Shader frostShader(
        "./shaders/frost.vert",
        "./shaders/frost.frag"
    );
		
		Shader normalShader(
        "./shaders/blending/normal.vert",
        "./shaders/blending/normal.frag"
    );

		// Framebuffers and textures
    
    unsigned int fbWithPanels = 0,
                texWithPanels = 0;

    // Main cycle
    
    int rwidth = 0, rheight = 0;
    
    do {
        int width, height;
        
        glfwGetFramebufferSize(m_window, &width, &height);
        
        // Resize textures
        
        if(height != rheight || width != rwidth) {
         
            rheight = height;
            rwidth = width;
            
            recreateFramebuffer(fbWithPanels, texWithPanels, width, height);
        }
        
        glViewport(0, 0, width, height);
        
        GLfloat anim = (sin(glfwGetTime()) / 2) + 0.5;
       
        glfwSwapBuffers(m_window);

        glBindFramebuffer(GL_FRAMEBUFFER, fbWithPanels);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0, 0, 0, 0);
        
        panelShader.use();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texPage);
        glUniform1i(glGetUniformLocation(panelShader.program, "tex0"), 0);
				glUniform1f(glGetUniformLocation(panelShader.program, "pageH"), pageHeight);
				glUniform1f(glGetUniformLocation(panelShader.program, "height"), height);
        glUniform1f(glGetUniformLocation(panelShader.program, "scroll"), anim);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        frostShader.use();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texWithPanels);
        glUniform1i(glGetUniformLocation(frostShader.program, "tex0"), 0);
				glUniform2f(glGetUniformLocation(frostShader.program, "resolution"), owidth, oheight);
        
        glBindVertexArray(quadVAO);

				// blur

				const int iterations = 17;

				for(int i = 0; i < iterations; i++) {
					float radius = iterations - i - 1;

					// dir = direction
					glUniform2f(glGetUniformLocation(frostShader.program, "dir"),
							i % 2 == 0 ? radius : 0,
							i % 2 == 0 ? 0 : radius
					);
					glDrawArrays(GL_TRIANGLES, 0, 6);
				}
       
				// Print to screen
				
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0, 0, 0, 0);
        
        normalShader.use();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texWithPanels);
        glUniform1i(glGetUniformLocation(normalShader.program, "texDst"), 0);
        glUniform1i(glGetUniformLocation(normalShader.program, "texSrc"), 0);
        
        glBindVertexArray(quadVAO);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwPollEvents();
    }
    while (!glfwWindowShouldClose(m_window));
    
    // Ternimate
    
    glDeleteFramebuffers(1, &fbWithPanels);
    glDeleteTextures(1, &texWithPanels);
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &elementBufferObject);
    glDeleteBuffers(1, &vertexBufferObject);
    
    glfwDestroyWindow(m_window);
    glfwTerminate();
    
    return 0;
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

