#version 330 core

layout(location = 0)in vec3 vertexIn;   
layout(location = 1)in vec2 textureIn;  
out vec2 textureOut;  

void main(void)  
{  
  gl_Position = vec4(vertexIn,1);   
  textureOut = textureIn;  
}  
