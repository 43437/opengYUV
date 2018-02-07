#ifndef __WINDOW_VIEW__
#define __WINDOW_VIEW__

#include <cstdlib>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <vector>

extern "C"{
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>  
}

// Include GLM
#include <glm/glm.hpp>  


using namespace glm;

#define ATTRIB_VERTEX 3  
#define ATTRIB_TEXTURE 4 

class WindowView{
  
  GLFWwindow* window;
  
  GLuint id_y;
  GLuint id_u;
  GLuint id_v; // Texture id  
  GLuint textureUniformY, textureUniformU,textureUniformV; 
  GLuint vertexbuffer, uvbuffer;
  GLuint p;
  
  enum{
    pixel_w = 426,
    pixel_h = 240
  };
  
  uint8_t buf[pixel_w*pixel_h*3/2];
  uint8_t *plane[3]; 
  
  std::FILE *infile = NULL;  

  int createWindow();
  void InitShaders();
public:
  WindowView();
  void draw();
};


#if TEXTURE_ROTATE  
    static const GLfloat vertexVertices[] = {  
        -1.0f, -0.5f,  
         0.5f, -1.0f,  
        -0.5f,  1.0f,  
         1.0f,  0.5f,  
    };      
#else  
    static const GLfloat vertexVertices[] = {  
        -1.0f, -1.0f, 0.0f, 
        1.0f, -1.0f,  0.0f,
	1.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  0.0f
    };      
#endif  
  
#if TEXTURE_HALF  
    static const GLfloat textureVertices[] = {  
        0.0f,  1.0f,  
        0.5f,  1.0f,  
        0.0f,  0.0f,  
        0.5f,  0.0f,  
    };   
#else  
    static const GLfloat textureVertices[] = {  
        0.0f,  1.0f,  
	0.0f,  0.0f,  
	1.0f,  0.0f,  
        1.0f,  1.0f,  
    };   
#endif  

#endif