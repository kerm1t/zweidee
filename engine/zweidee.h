#pragma once
#ifndef ZWEIDEE_H
#define ZWEIDEE_H

#include "resource.h" // bit ugly, as ressource is associated to the project

#include <stdio.h>    // <-- sprintf
#include <vector>
#include <iostream>   // <-- fbo to pgm|ppm
#include <fstream>    // <-- fbo to pgm|ppm
#include <string>

#include "../common/types.h"


#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>    // Header File For The OpenGL32 Library
#include <gl\glu.h>   // Header File For The GLu32 Library

//#define GLM_FORCE_RADIANS // 2do, replace all deg. by rad.!!
#include "glm.hpp"

#include <process.h>    // _beginthread, _endthread


namespace zweidee
{
  struct colrgb
  {
    char r;
    char g;
    char b;
  };

  struct point
  {
  public:
    int x; // do not use unsigned int (except bitfields). this causes unsigned arithmetic, which is pretty bad
    int y;
    point() { } // default constructor
    point(const int _x, const int _y) : x(_x), y(_y) { } // constructor for change after declaration
  };

  static float fps;                        // This will store our fps
  static float lastTime;                   // This will hold the time from the last frame
 
  static void CalculateFrameRate()
  {
    float currentTime = GetTickCount() * 0.001f;
    ++fps;
    if (currentTime - lastTime > 1.0f)
    {
      lastTime = currentTime;
      fps = 0.0;
    }
  }

  static HGLRC            hRC;                           // Permanent Rendering Context
  static HDC              hDC;                           // Private GDI Device Context
  static HWND             hWnd;                          // Holds Our Window Handle


  class CShader // GLSL Shader manager
  {
  public:
    GLuint program_zweidee;
    GLuint sh_attr_pos;
    GLuint sh_attr_tex;
    GLuint sh_unif_ID;

    void InitShader()
    {
      GLenum err = glGetError();

      char buffer[512];

      const GLchar * vshd_src_FPS[] = {
        "#version 330 core\n" // 410 not supported by SONY OpenGL driver
        "in vec2 vp_clipspace;\n"
        "out vec2 UV;\n"
        "void main()\n"
        "{\n"
        "  UV = (vp_clipspace+1.0) * 0.5;\n"
        "  gl_Position = vec4(vp_clipspace.x, vp_clipspace.y, 0.0, 1.0);\n"
        "  gl_Position.xy *= 1.0;\n"
        "}"
      };
// https://stackoverflow.com/questions/2437977/fragment-shader-blur-how-does-this-work
      const GLchar * fshd_src_FPS[] = {
        "#version 330 core\n"
        "in vec2 UV;\n"
        "out vec3 color;\n"
        "uniform sampler2D myTexSampler;\n"
/*        "    uniform sampler2D myTexSampler;\n"
      "   uniform vec2 tc_offset[9];\n"
      "     void main()\n"
        "   {\n"
        "   vec4 sample[9];\n"
        "   for (int i = 0; i < 9; ++i)\n"
//          " sample[i] = texture2D(sampler0, gl_TexCoord[0].st + tc_offset[i]);\n"
        " sample[i] = texture2D(myTexSampler, UV.rgb + tc_offset[i]);\n"
        "\n"
        "  color = (sample[0] + (2.0 * sample[1]) + sample[2] +"
          "  (2.0 * sample[3]) + sample[4] + 2.0 * sample[5] +"
          "    sample[6] + 2.0 * sample[7] + sample[8]) / 13.0;\n"
      "}\n"
*/      "void main()\n"
        "{\n"
        "  color = texture(myTexSampler, UV).rgb;\n" // texture2D ist deprecated
        "}"
      };

      // Shader for FPS
      GLuint vshader_zweidee = glCreateShader(GL_VERTEX_SHADER);
      glShaderSource(vshader_zweidee, 1, vshd_src_FPS, 0);
      glCompileShader(vshader_zweidee);
      glGetShaderInfoLog(vshader_zweidee, 512, NULL, buffer); // <-- debug
      err = glGetError();
      // 2do: check that buffer = "No errors."
      GLuint fshader_zweidee = glCreateShader(GL_FRAGMENT_SHADER);
      glShaderSource(fshader_zweidee, 1, fshd_src_FPS, 0);    // set array of strings as source code
      glCompileShader(fshader_zweidee); // compile
      glGetShaderInfoLog(fshader_zweidee, 512, NULL, buffer); // <-- debug
      err = glGetError();
      // 2do: check that buffer = "No errors."
      program_zweidee = glCreateProgram(); // create empty program object
      glAttachShader(program_zweidee, vshader_zweidee); // attach shader
      glAttachShader(program_zweidee, fshader_zweidee); // attach shader
      glLinkProgram(program_zweidee); // link
      err = glGetError();
      glUseProgram(program_zweidee);
      err = glGetError();

      // attribs
      sh_attr_pos = glGetAttribLocation(program_zweidee, "vp_clipspace");
      err = glGetError();
      sh_attr_tex = glGetAttribLocation(program_zweidee, "vertexUV");     // UV geht auch
      err = glGetError();
      sh_unif_ID = glGetUniformLocation(program_zweidee, "myTexSampler");
      err = glGetError();
    }
  };


  // ========
  // Textures
  // ========
#define TEX_ARIALFONT    1 // war vorher 0, da gab's einen Fehler!!

  enum tShading { SHADER_COLOR_FLAT, SHADER_TEXTURE };

  /*
  VAO, s. http://lwjgl.org/wiki/index.php?title=The_Quad_with_DrawArrays
  explained: http://stackoverflow.com/questions/5970087/understanding-vertex-array-objects-glgenvertexarrays
  VAO (>=OGL3.0): bundle multiple VBO's for easier handling, don't need to bind ... them each
  ... really good tut: http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html
  */
  class c_VAO
  {
  public:
    std::string Name;
    tShading t_Shade;

    uint16 ui_idTexture;    // TextureID

    bool b_doDraw;
    bool b_Wireframe;
    uint16 uiVertexCount;
    bool b_moving;

    c_VAO::c_VAO()
    {
      ui_idTexture = 0;
      b_doDraw = true;
      b_Wireframe = false;
      b_moving = false;
    }
  };


  class CRender : public zweidee::CShader
  {
  public:
    HGLRC hRC;         // Permanent Rendering Context
    HDC   hDC;         // Private GDI Device Context
    HWND  hWnd;        // Holds Our Window Handle
    HWND  dummyHWND;   // for NVidia setpixelformat-Fix
    int width, height;

    std::vector<GLuint> vGLTexture;
    static const unsigned int VBOCOUNT = 99;

    // pointers to all VAO buffer's
    GLuint positionBuffer[VBOCOUNT];
    GLuint    colorBuffer[VBOCOUNT]; // either color or ...
    GLuint       uvBuffer[VBOCOUNT]; // texture

    std::vector<c_VAO>  vVAOs;
    std::vector<GLuint> vVertexArray;       // stores VAO's: a) Position(x,y,z), b1) color OR b2) u,v-Texture

    CRender() // constructor
    {
      hRC = NULL;                           // Permanent Rendering Context
      hDC = NULL;                           // Private GDI Device Context
    }

    int Init(int w, int h)///, GLuint texID)
    {
      width = w; // this will size the viewport
      height = h;
      zweidee::hDC = GL_attach_to_DC(zweidee::hWnd); // <== NeHe    

      glewExperimental = GL_TRUE; // <-- Nutzen?
      glewInit(); // <-- takes a little time

      glClearColor(0.3f, 0.5f, 1.0f, 0.0f);

      glClearDepth(1.0f);
      glDepthFunc(GL_LESS);
      glEnable(GL_DEPTH_TEST);

      InitShader(); // vertices to texture zweidee framebuffer

///      vGLTexture.push_back(texID);

      return 0;
    }

    HDC GL_attach_to_DC(HWND hWnd)
    {
      GLuint PixelFormat;                   // Holds The Results After Searching For A Match

#define ERR_HDC           0
#define ERR_PIXELFORMAT   0
#define ERR_CONTEXT       0
#define ERR_GLMAKECURRENT 0

      static PIXELFORMATDESCRIPTOR pfd =    // pfd Tells Windows How We Want Things To Be
      {
        sizeof(PIXELFORMATDESCRIPTOR),      // Size Of This Pixel Format Descriptor
        1,                                  // Version Number
        PFD_DRAW_TO_WINDOW |                // Format Must Support Window
        PFD_SUPPORT_OPENGL |                // Format Must Support OpenGL
        PFD_DOUBLEBUFFER,                   // Must Support Double Buffering
        PFD_TYPE_RGBA,                      // Request An RGBA Format
        8, /*32*/                                // framebuffer Color Depth
        0, 0, 0, 0, 0, 0,                   // Color Bits Ignored
        0,                                  // No Alpha Buffer
        0,                                  // Shift Bit Ignored
        0,                                  // No Accumulation Buffer
        0, 0, 0, 0,                         // Accumulation Bits Ignored
        16,                                 // 16Bit Z-Buffer (Depth Buffer)
        0,                                  // No (number of) bits for Stencil Buffer
        0,                                  // No bits for Auxiliary Buffer
        PFD_MAIN_PLANE,                     // Main Drawing Layer
        0,                                  // Reserved
        0, 0, 0                             // Layer Masks Ignored
      };
      if (!(hDC = GetDC(hWnd)))             // Did We Get A Device Context?
      {
        MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return ERR_HDC;
      }
      if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) // Did Windows Find A Matching Pixel Format?
      {
        MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return ERR_PIXELFORMAT;
      }
      if (!SetPixelFormat(hDC, PixelFormat, &pfd)) // Are We Able To Set The Pixel Format?
      {
        char str[255];
        DWORD err = GetLastError();
        sprintf(str, "Can't Set The PixelFormat, err=%d", err);
        MessageBox(NULL, str, "ERROR", MB_OK | MB_ICONEXCLAMATION);

        // --> on NVidia GTX 780 error : -1073283066 = 0xc0070006
        return ERR_PIXELFORMAT;
      }

      /*  ... when running in MTS-Thread --> s. http://www.graphicsgroups.com/6-opengl/0bafc0120a809ed8.htm

      According to the Windows OpenGL documentation:

      "Setting the pixel format of a window more than once can lead to significant
      complications for the Window Manager and for multithread applications, so it
      is not allowed. An application can only set the pixel format of a window one
      time. Once a window's pixel format is set, it cannot be changed."

      The card drivers that allow you do change the format are not following the
      rules; the ones (like GeForce3) that are giving you problems are giving you
      problems because you're violating the rules.
      As has been suggested elsewhere, create for yourself an invisible and/or
      off-screen dummy window, and use the rendering context you create for that
      window as your display-list share. You can either create it from a
      registered class with CS_OWNDC style, or you can just create a compatible DC
      and then not delete that DC until the program is done. Either way, this will
      be low-overhead solution, plus it should work on all compliant video
      drivers.
      */

      if (!(hRC = wglCreateContext(hDC))) // Are We Able To Get A Rendering Context?
      {
        MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return ERR_CONTEXT;
      }
      if (!wglMakeCurrent(hDC, hRC)) // Try To Activate The Rendering Context
      {
        MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return ERR_GLMAKECURRENT;
      }

      ReSizeGLScene(width, height); // Set Up Our Perspective GL Screen
      return hDC;
    }

    GLvoid ReSizeGLScene(GLsizei width, GLsizei height) // Resize And Initialize The GL Window
    {
      if (height == 0)                   // Prevent A Divide By Zero By
      {
        height = 1;                      // Making Height Equal One
      }
      glViewport(0, 0, width, height);   // Reset The Current Viewport
    }

    void Setup_Geometry(GLuint texID)
    {
      std::vector<GLfloat> coords;

      //         +-------+-------+ (1,1)
      //         |       |       |
      //         +-------+ (0,0) +
      //         |       |       |
      // (-1,-1) +---------------+

      coords.push_back(-1.0f); coords.push_back(-1.0f);
      coords.push_back(1.0f); coords.push_back(-1.0f);
      coords.push_back(-1.0f); coords.push_back(1.0f);
      coords.push_back(-1.0f); coords.push_back(1.0f);
      coords.push_back(1.0f); coords.push_back(-1.0f);
      coords.push_back(1.0f); coords.push_back(1.0f);

      //         +-------+ (1,1)
      //         |       |
      //   (0,0) +-------+
      std::vector<GLfloat> uvs;
      uvs.push_back(0.0f); uvs.push_back(0.0f);
      uvs.push_back(0.0f); uvs.push_back(1.0f);
      uvs.push_back(1.0f); uvs.push_back(0.0f);
      uvs.push_back(1.0f); uvs.push_back(0.0f);
      uvs.push_back(1.0f); uvs.push_back(1.0f);
      uvs.push_back(0.0f); uvs.push_back(1.0f);

      // ---------------------------
      // >>> now Push to OpenGL! >>>
      // ---------------------------
      unsigned int ui_idVBO = vVAOs.size();
      glGenBuffers(1, &positionBuffer[ui_idVBO]);
      glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[ui_idVBO]);
      glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(GLfloat), &coords[0], GL_STATIC_DRAW);

      glGenBuffers(1, &uvBuffer[ui_idVBO]);
      glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[ui_idVBO]);
      glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(GLfloat), &uvs[0], GL_STATIC_DRAW);

      c_VAO fps;
      fps.t_Shade = SHADER_TEXTURE;
      fps.Name = "FPS";
      fps.ui_idTexture = TEX_ARIALFONT;
      fps.uiVertexCount = 6;
      vVAOs.push_back(fps);

      vGLTexture.push_back(texID);
      Bind_VBOs_to_VAOs(); // now hand over VBO's to VAO's
    }

    /* Bind vertex buffers to VAO's
    vertex buffers can hold any information: position, color, uv-coordinates
    VAO's only exist from OpengL >=
    */
    void Bind_VBOs_to_VAOs() // s. http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html
    {
      GLuint gi;
      GLenum err = glGetError();
      for (unsigned int iVAO = 0; iVAO < vVAOs.size(); iVAO++)
      {
        vVertexArray.push_back(gi);
      }
      glGenVertexArrays(vVAOs.size(), &vVertexArray[0]);

      // ===== 2d-VBO's (FPS shader) =====

      unsigned int iVAO = 0;
      glBindVertexArray(vVertexArray[iVAO]);  // select/bind array and
                                              // attach a) position and
                                              //        b) texture/uv-buffers
      glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[iVAO]);
      glVertexAttribPointer(sh_attr_pos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // wichtig, hier das richtige Attrib (nicht 0 oder 1) zu übergeben!
      glEnableVertexAttribArray(sh_attr_pos);
      err = glGetError();

      glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[iVAO]); // u,v-texture-coords
      err = glGetError();
      glVertexAttribPointer(sh_attr_tex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // texAttrib
      err = glGetError(); // Fehler 1281
      glEnableVertexAttribArray(sh_attr_tex);
      err = glGetError();

      glBindVertexArray(0); // Unbind
      err = glGetError();


      // ===== 3d-VBO's (regular shader, i.e. Scene + Objects) =====

      for (unsigned int iVAO = 1; iVAO < vVAOs.size(); iVAO++) // <-- start with 1, as 0 is for FPS-coords
      {
        glBindVertexArray(vVertexArray[iVAO]);  // select/bind array and
        if (vVAOs[iVAO].t_Shade == SHADER_COLOR_FLAT) // flat (number of elements per Vertex = 3)
        {
          // not used
        }
        else if (vVAOs[iVAO].t_Shade == SHADER_TEXTURE) // texture (number of elements per Vertex = 2)
        {
          glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[iVAO]); // u,v-texture-coords
          err = glGetError();
        }
        glBindVertexArray(0); // Unbind
      }
    }

    void DrawVAOs_NEU()
    {
      /*
      Achtung, hier springt das Programm nochmal 'rein nach Drücken des Close Buttons,
      dann sind aber die vVAO-Inhalte schon beliebig "dirty"!!
      z.B. ist dann vVAOs[ui].b_moving = TRUE und die if Abfrage (s.u.) wird angesprungen
      */

      glm::vec3 v3;

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(program_zweidee);

      GLenum err = GL_NO_ERROR;

      // draw Scene + Objects
      for (unsigned int ui = 0; ui < vVAOs.size(); ui++) // start with 1 as 0 is fps-counter
      {

        if (vVAOs[ui].t_Shade == SHADER_TEXTURE)
        {
          //      glUniform1i(sh1_unif_col_tex, 1); // shader into texture-branch

          // http://ogldev.atspace.co.uk/www/tutorial16/tutorial16.html
          glActiveTexture(GL_TEXTURE0);
          err = glGetError();
          // ----------------------
          // Fehler 1282 auf NVidia
          // Ideen: rendern im Wireframe - hilft nicht --> konnte die Textur nicht geladen werden?
          // ----------------------
          glBindTexture(GL_TEXTURE_2D, vGLTexture[vVAOs[ui].ui_idTexture - 1]); // TEXTURE_ID shall be > 0 !     (-1!!)
          err = glGetError();

          if (ui == 0) // hack!!
            glUniform1i(sh_unif_ID, 0); // hack!!
          else
            //        glUniform1i(sh1_unif_sampler, 0)
            ;
        }
        else // vVAOs[ui].t_Shade == SHADER_COLOR_FLAT
        {
          // not used
        }
        err = glGetError();

        glBindVertexArray(vVertexArray[ui]); // <--- NVidia: hier Problem, wenn ui = 13 (beim ersten colorierten + texturierten Objekt!!)

                                             /*
                                             wenn's hier crasht, dann ist der Fehler vermutlich vorher beim buffern passiert und
                                             glGetError hätte etwas melden sollen!!
                                             */

        glDrawArrays(GL_TRIANGLES, 0, vVAOs[ui].uiVertexCount); // <-- if error is thrown here,
        err = glGetError();                                     //     it can be either positionbuffer, colorbuffer or uvbuffer
                                                                //     if t_Shade == TEXTURE,
                                                                //     then colorbuffer is NULL and vice versa!

      } // for ...

        //    if (b_PNG) FBO_to_PPM();
    }

    void Render()
    {
      wglMakeCurrent(hDC, hRC); // ;-) now Tab-switching in MTS possible
      DrawVAOs_NEU();      // Draw The Scene
      SwapBuffers(hDC);    // Swap Buffers (Double Buffering)
    }

  private:
    static int const fbo_width = 512;
    static int const fbo_height = 512;
    GLuint fb, color, depth; // <--- nicer names!
  };

  static unsigned char * data;

  class FrameBuf2D // which is actually an animated (OpenGL-)Texture
  {
  public:
    uint32 width, height;
    uint32 imagesize;      // = width*height*3
    uint32 framecounter;
/*
    ~FrameBuf2D()
    {
      delete data;
    }
*/
    // (0,0): rgb (1,0): rgb (2,0): rgb
    void setpixel(uint8 * data, const uint32 x, const uint32 y, const uint8 r, const uint8 g, const uint8 b)
    {
      uint32 _y = height - 1 - y; // hack, texture upside down? 2do: check!!
                                        //    unsigned int _y = y;
      uint32 pos = 3 * (_y * width + x);
      //    assert(pos < imageSize);
      if (pos >= imagesize) return;
      if (x > width) return; // test < 0 ?? <-- need signed then
      data[pos] = b; // r ?
      data[pos + 1] = g;
      data[pos + 2] = r; // b ?
    }
    void setpixel(uint8 * data, const uint32 x, const uint32 y, const float r, const float g, const float b) // overload
    {
      setpixel(data, x, y, (uint8)r, (uint8)g, (uint8)b);
    }
    void setpixel(uint8 * data, const uint32 x, const uint32 y, const int r, const int g, const int b) // overload
    {
      setpixel(data, x, y, (uint8)r, (uint8)g, (uint8)b);
    }

    GLuint Init(int w, int h) // purpose: get texture-ID
    {
      // Actual RGB data
///      unsigned char * data;

      width = w;
      height = h;
      imagesize = width*height * 3;

      // Create a buffer
///      data = new unsigned char[imagesize];
      memset(data, 0, imagesize); // clear

      // draw test pattern
      for (uint32 i = 1; i < (height - 1); i++)
      {
        uint8 r, g, b;
        r = ((i + 1) % 3) * 255;
        g = (i % 3) * 255;
        b = ((i - 1) % 3) * 255;
        setpixel(data, i, i, r, g, b);
        setpixel(data, width - i, i, r, g, b);
      }

      // Create one OpenGL texture
      GLuint textureID;
      glGenTextures(1, &textureID);

      // "Bind" the newly created texture : all future texture functions will modify this texture
      glBindTexture(GL_TEXTURE_2D, textureID);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      // Give the image to OpenGL
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);  // hier gibt es Schwierigkeiten mit .bmp,

///      delete data;

      return textureID;
    }

#if 0
    // ---
    // FBO
    // ---
    void CHECK_FRAMEBUFFER_STATUS()
    {
      GLenum status;
      status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
      switch (status)
      {
      case GL_FRAMEBUFFER_COMPLETE:
        break;

      case GL_FRAMEBUFFER_UNSUPPORTED: // choose different formats
        break;

      default: // programming error; will fail on all hardware
               //        fputs("Framebuffer Error\n", stderr);
        exit(-1);
      }
    }

    void proj::Render::init_FBO()
    {
      glGenFramebuffers(1, &fb);
      glGenTextures(1, &color);
      glGenRenderbuffers(1, &depth);

      glBindFramebuffer(GL_FRAMEBUFFER, fb);

      glBindTexture(GL_TEXTURE_2D, color);
      glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA,
        fbo_width, fbo_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

      glBindRenderbuffer(GL_RENDERBUFFER, depth);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fbo_width, fbo_height);
      glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

      CHECK_FRAMEBUFFER_STATUS();
    }

    //GLubyte pixels[fbo_height][fbo_width][3]; // too big for the stack
    GLubyte puxels[1000][600][3]; // too big for the stack
    void proj::Render::FBO_to_PPM() // write out as binary PPM (with lines in reverse order)
    {
      glBindTexture(GL_TEXTURE_2D, color);//textureId[0]);
      glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, puxels);
      glBindTexture(GL_TEXTURE_2D, 0); // <-?

      std::ofstream myfile;
      std::string filename;
      filename = "d:\\Zweidee_FBO.ppm";
      myfile.open(filename.c_str());
      myfile << "P6" << std::endl;
      myfile << std::to_string((_ULonglong)fbo_width) << " " << std::to_string((_ULonglong)fbo_height) << std::endl; // "w h"
                                                                                                                     //    myfile << "1000" << " " << "600" << std::endl; // "w h"
      myfile << "255" << std::endl;
      myfile.close();
      myfile.open(filename.c_str(), std::ofstream::app | std::ofstream::binary); // binary-"trick" from: http://www.gamedev.net/topic/510801-render-to-memory/

      for (int i = 0; i < fbo_height; i++)
      {
        for (int j = 0; j < fbo_width; j++)
        {
          for (int k = 0; k < 3; k++)
          {
            myfile << puxels[(fbo_height - 1) - i][j][k];
          }
        }
      }
      myfile.close();
    }
#endif
  }; // Framebuf2D

  static FrameBuf2D    fbuf2d;

  static zweidee::CRender m_render;

  class CWindow
  {
  public:
  };


#define FBUF2D_PIXELS FBUF2D_WIDTH * FBUF2D_HEIGHT
#define FBUF2D_SIZE   FBUF2D_PIXELS * 3 // r,g,b
  // windows dimension (the later may be changed with resize)
  static int win_w;
  static int win_h;

  static bool b_WM_resized;

  // --------- render thread ---------
  static void(*repeat)();      // a) function pointer

  static dword lasttickcount = 0;
  static dword accumulatedTimeSinceLastUpdate = 0;

  // Fix Timing
  // https://gafferongames.com/post/fix_your_timestep/

  // OpenGL calls moved to own thread
  // s. http://stackoverflow.com/questions/9833852/opengl-game-loop-multithreading
  static void RenderThread(void *args)
  {
    lasttickcount = GetTickCount();
    while (true)
    {
      accumulatedTimeSinceLastUpdate += (GetTickCount() - lasttickcount); // GetTickCount [ms] dword
      lasttickcount = GetTickCount();

      if (accumulatedTimeSinceLastUpdate > 12) // indep. from gfx-card -> update every 12 [ms]
      {
        accumulatedTimeSinceLastUpdate = 0;

        ////////////////
        // do stuff here
        ////////////////

        repeat(); // d) call function pointer
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zweidee::fbuf2d.width, zweidee::fbuf2d.height, 0, GL_BGR, GL_UNSIGNED_BYTE, zweidee::data);
      }

      if (b_WM_resized)
      {
        m_render.ReSizeGLScene(win_w, win_h);
        b_WM_resized = false;
      }

      m_render.Render(); // render update-rate independent from move() (s. above)
    }
    _endthread();
  }


#define MAX_LOADSTRING 100

  // global variables
  static HINSTANCE        hInst;                         // current instance
  static TCHAR            szTitle[MAX_LOADSTRING];       // Titelleistentext
  static TCHAR            szWindowClass[MAX_LOADSTRING]; // Klassenname des Hauptfensters
  

  // autogenerated: messagehandler for info dialogue
  static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
  {
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
      return (INT_PTR)TRUE;

    case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
      {
        EndDialog(hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;
      }
      break;
    }
    return (INT_PTR)FALSE;
  }

  // autogenerated: save instance handle and create main window
  static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, int w, int h)
  {
    hInst = hInstance; // store instance handle
                       // Nonsense:
                       // one is the viewport, i.e. window size
                       // the other is the internal texture size, i.e. the game playfield 

                       // center on screen
    RECT rect;
    GetClientRect(GetDesktopWindow(), &rect);
    int x = (rect.right - w) / 2;
    int y = (rect.bottom - h) / 2;
    // center on screen
    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, x, y, w, h, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
      return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
  }


  // autogenerated: process main window's messages
  static LRESULT CALLBACK WndProcZweidee(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
  {
    int wmId, wmEvent;

    switch (message)
    {
    case WM_COMMAND:
      wmId = LOWORD(wParam);
      wmEvent = HIWORD(wParam);
      // process menu choice
      switch (wmId)
      {
      case IDM_ABOUT:
        DialogBox(zweidee::hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, zweidee::About);
        break;
      case IDM_EXIT:
        DestroyWindow(hWnd);
        break;
      default:
        return DefWindowProc(hWnd, message, wParam, lParam);
      }
      break;
      //  case WM_PAINT:
      // ... painting by OpenGL
    case WM_SIZE:
      zweidee::win_w = LOWORD(lParam);
      zweidee::win_h = HIWORD(lParam);
      // resize --> in the thread now, as from here no access to OpenGL-context
      zweidee::b_WM_resized = true;
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
  }

  // autogenerated: register window class (mandatory)
  static ATOM MyRegisterClass(HINSTANCE hInstance, WNDPROC WndProc)
  {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZWEIDEE));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_ZWEIDEE);
    wcex.lpszClassName = zweidee::szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
  }

  static bool app_init(int w, int h, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow, WNDPROC WndProc = NULL)
  {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    win_w = w;
    win_h = h;
    zweidee::hRC = NULL;
    zweidee::hDC = NULL;
    zweidee::hWnd = NULL;
    zweidee::fps = 0.0f;
    zweidee::lastTime = 0.0f;
    zweidee::b_WM_resized = false;

    // init global strings
    LoadString(hInstance, IDS_APP_TITLE, zweidee::szTitle, MAX_LOADSTRING);  // window title etc. see .rc
    LoadString(hInstance, IDC_ZWEIDEE, zweidee::szWindowClass, MAX_LOADSTRING);
    if (WndProc == NULL)
      zweidee::MyRegisterClass(hInstance, WndProcZweidee);
    else
      zweidee::MyRegisterClass(hInstance, WndProc);

    if (!zweidee::InitInstance(hInstance, nCmdShow, zweidee::win_w, zweidee::win_h)) // init application
    {
      return FALSE;
    }

    zweidee::m_render.Init(zweidee::win_w, zweidee::win_h);

    zweidee::data = new unsigned char[FBUF2D_SIZE]; // size = pixels*3 (r,g,b)
    GLuint texID = zweidee::fbuf2d.Init(FBUF2D_WIDTH, FBUF2D_HEIGHT);

    zweidee::m_render.Setup_Geometry(texID);
    return true;
  }

  static int app_run(HINSTANCE hInstance)
  {
    if (repeat != NULL) _beginthread(zweidee::RenderThread, 0, 0);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZWEIDEE));
    
    // main message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
      wglMakeCurrent(NULL, NULL); // <-- no other access to OpenGL here!! --> only in RenderThread 

      if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      // --> rendering moved to RenderThread, otherwise no autonomous object-movement possible
    }
    
    delete zweidee::data;
    
    return (int)msg.wParam;
  }
}


#endif // ZWEIDEE_H
