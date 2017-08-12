#include "stdafx.h"
#pragma once

#include <stdio.h> // <-- sprintf
#include "inc_render.h"
//#define GLM_FORCE_RADIANS // 2do, replace all deg. by rad.!!
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>

class ShaderMan // Shader manager
{
public:
  // =============
  // GLSL / Shader
  // =============



  // ** Shader2 **
  GLuint program_fps; // = sh2
  // attrib + uniform
  GLuint sh2_attr_pos;
  GLuint sh2_attr_tex;
  GLuint sh2_unif_ID;



  void InitShader2()
  {
    GLenum err = glGetError();

    char buffer[512];

/*
  4 coord.systems: Object, World, View, Clip
  ==========================================
  Object --> Model Matrix --> World
  World --> View matrix --> View
  View --> Projection Matrix (w. implicit homogeneous divide) --> Clip
*/
    /* vertex shader : output always to "homogeneous clip space", i.e. (-1:1, -1:1, -1:1, -1:1) */
    const GLchar * vshd_src_FPS[] = {
      "#version 330 core\n" // 410 not supported by SONY OpenGL driver
      "in vec2 vp_clipspace;\n"
      "out vec2 UV;\n"
      "void main()\n"
      "{\n"
      "  UV = (vp_clipspace+1.0) * 0.5;\n"
      "  gl_Position = vec4(vp_clipspace.x, vp_clipspace.y, 0.0, 1.0);\n"
//      "  gl_Position.xy *= 0.5;\n"
      "  gl_Position.xy *= 1.0;\n"
      "}"
    };
    const GLchar * fshd_src_FPS[] = {
      "#version 330 core\n"
      "in vec2 UV;\n"
      "out vec3 color;\n"
      "uniform sampler2D myTexSampler;\n"
      "void main()\n"
      "{\n"
      "  color = texture(myTexSampler, UV).rgb;\n" // texture2D ist deprecated
      "}"
    };


    // Shader for FPS
    GLuint vshaderFPS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshaderFPS, 1, vshd_src_FPS, 0);
    glCompileShader(vshaderFPS);
    glGetShaderInfoLog(vshaderFPS, 512, NULL, buffer); // <-- debug, kann man sich schoen im debugger ansehen!!
    err = glGetError();
    // 2do: check that buffer = "No errors."
    GLuint fshaderFPS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshaderFPS, 1, fshd_src_FPS, 0); // set array of strings as source code
    glCompileShader(fshaderFPS); // compile
    glGetShaderInfoLog(fshaderFPS, 512, NULL, buffer); // <-- debug
    err = glGetError();
    // 2do: check that buffer = "No errors."
    program_fps = glCreateProgram(); // create empty program object
    glAttachShader(program_fps, vshaderFPS); // attach shader
    glAttachShader(program_fps, fshaderFPS); // attach shader
    glLinkProgram(program_fps); // link
    err = glGetError();
    glUseProgram(program_fps);
    err = glGetError();

    // attribs
    sh2_attr_pos = glGetAttribLocation(program_fps, "vp_clipspace");
    err = glGetError();
    sh2_attr_tex = glGetAttribLocation(program_fps, "vertexUV");     // UV geht auch
    err = glGetError();
    sh2_unif_ID = glGetUniformLocation(program_fps, "myTexSampler");
    err = glGetError();
  }

};
