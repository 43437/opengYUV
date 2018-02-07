#include "WindowView.hpp"

int WindowView::createWindow()
{
  if( !glfwInit() )
  {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    getchar();
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
    getchar();
    glfwTerminate();
    return -1;
  }
  
//   glViewport(0.0, 0.0, 1024, 768);
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);
  
  glGenBuffers(1, &uvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertices), textureVertices, GL_STATIC_DRAW);

  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexVertices), vertexVertices, GL_STATIC_DRAW);
  
  return 0;
}

WindowView::WindowView()
{
  createWindow();
  InitShaders();
  
  if((infile=std::fopen("test426x240.yuv", "rb"))==NULL){  
    std::cerr<<"cannot open this file\n"<<std::endl;
  }  
  
  plane[0] = buf;  
  plane[1] = plane[0] + pixel_w*pixel_h;  
  plane[2] = plane[1] + pixel_w*pixel_h/4;  
  
  std::cout<<"width height "<<pixel_w<<" "<<pixel_h<<std::endl;
}

void WindowView::draw()
{
  //Clear  
  do
  {
    glClear(GL_COLOR_BUFFER_BIT);  
    
    if (std::fread(buf, 1, pixel_w*pixel_h*3/2, infile) != pixel_w*pixel_h*3/2){  
      // Loop  
      
      std::cerr<<"read byte not enough. "<<std::endl;
      std::fseek(infile, 0, 0);  
      std::fread(buf, 1, pixel_w*pixel_h*3/2, infile);  
    }  
    
    long pos = std::ftell(infile);
//     std::cout<<"cur pos "<<pos<<std::endl;

//     glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, vertexVertices); 
    
    glLinkProgram(p);  
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
    0,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    3,                                // size : U+V => 2
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
    );
    glEnableVertexAttribArray(0);
    

//     glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, textureVertices); 
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    2,                                // size : U+V => 2
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
    );
    glEnableVertexAttribArray(1);
	  
    //Y  
    glActiveTexture(GL_TEXTURE0);  
    glBindTexture(GL_TEXTURE_2D, id_y);  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w, pixel_h, 0, GL_RED, GL_UNSIGNED_BYTE, plane[0]);   
    //   glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniform1i(textureUniformY, 0);      

    //U  
    glActiveTexture(GL_TEXTURE1);  
    glBindTexture(GL_TEXTURE_2D, id_u);  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w/2, pixel_h/2, 0, GL_RED, GL_UNSIGNED_BYTE, plane[1]);         
    glUniform1i(textureUniformU, 1);  

    //V  
    glActiveTexture(GL_TEXTURE2);  
    glBindTexture(GL_TEXTURE_2D, id_v);  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w/2, pixel_h/2, 0, GL_RED, GL_UNSIGNED_BYTE, plane[2]);      
    glUniform1i(textureUniformV, 2);     

    // Draw  
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);  
    // Show  
    glfwSwapBuffers(window);
    glfwPollEvents();
    
//     std::this_thread::sleep_for(std::chrono::milliseconds(40));
//     std::cout<<"wake up"<<std::endl;
    
  }while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
      glfwWindowShouldClose(window) == 0 );
}

//Init Shader  
void WindowView::InitShaders()  
{  
    GLint vertCompiled, fragCompiled, linked;  
      
    GLint v, f;  
    const char *vs,*fs;  
    //Shader: step1  
    v = glCreateShader(GL_VERTEX_SHADER);  
    f = glCreateShader(GL_FRAGMENT_SHADER);  
    //Get source code  
//     vs = textFileRead("Shader.vsh");  
//     fs = textFileRead("Shader.fsh");  
    
    std::string strfv;
    std::ifstream fvStream("../Shader.vsh", std::ios::in);
    if (fvStream.is_open())
    {
      std::stringstream svStream;
      svStream << fvStream.rdbuf();
      strfv = svStream.str();
      fvStream.close();
//       std::cout<<"vetex shader "<<strfv<<std::endl;
    } 
    else 
    {
      std::cerr<<"vetex shader open failed."<<std::endl;
    }
    
    std::string strff;
    std::ifstream ffStream("../Shader.fsh", std::ios::in);
    if (ffStream.is_open())
    {
      std::stringstream sfStream;
      sfStream << ffStream.rdbuf();
      strff = sfStream.str();
      ffStream.close();
//       std::cout<<"fragment shader "<<strff<<std::endl;
    }
    else
    {
      std::cerr<<"fragment shader open failed. "<<std::endl;
    }
    
    vs = strfv.c_str();
    fs = strff.c_str();
    
    GLint Result = GL_FALSE;
    int InfoLogLength;
	
    //Shader: step2  
    glShaderSource(v, 1, &vs,NULL);  
    glShaderSource(f, 1, &fs,NULL);  
    //Shader: step3  
    glCompileShader(v);  
    //Debug  
    glGetShaderiv(v, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(v, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
      std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
      glGetShaderInfoLog(v, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
      std::printf("%s\n", &VertexShaderErrorMessage[0]);
    }
    
    glCompileShader(f);  
    glGetShaderiv(f, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(f, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
      std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
      glGetShaderInfoLog(f, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
      std::printf("%s\n", &VertexShaderErrorMessage[0]);
    }
  
    //Program: Step1  
    p = glCreateProgram();   
    //Program: Step2  
    glAttachShader(p,v);  
    glAttachShader(p,f);   
  
   /* glBindAttribLocation(p, ATTRIB_VERTEX, "vertexIn");  
    glBindAttribLocation(p, ATTRIB_TEXTURE, "textureIn"); */ 
    //Program: Step3  
    
    glLinkProgram(p);  
    //Debug  
    glGetShaderiv(p, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(p, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
      std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
      glGetShaderInfoLog(p, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
      std::printf("%s\n", &VertexShaderErrorMessage[0]);
    }   
    
    //Program: Step4  
    glUseProgram(p);  
  
  
    //Get Uniform Variables Location  
    textureUniformY = glGetUniformLocation(p, "tex_y");  
    textureUniformU = glGetUniformLocation(p, "tex_u");  
    textureUniformV = glGetUniformLocation(p, "tex_v");   
  
//     //Set Arrays  
//     glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 0, vertexVertices);  
//     //Enable it  
//     glEnableVertexAttribArray(ATTRIB_VERTEX);      
//     
	
    /*glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);  
    glEnableVertexAttribArray(ATTRIB_TEXTURE); */ 
  
  
    //Init Texture  
    glGenTextures(1, &id_y);   
    glBindTexture(GL_TEXTURE_2D, id_y);      
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
      
    glGenTextures(1, &id_u);  
    glBindTexture(GL_TEXTURE_2D, id_u);     
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
      
    glGenTextures(1, &id_v);   
    glBindTexture(GL_TEXTURE_2D, id_v);      
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  

}





