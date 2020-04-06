#include "stdafx.h"

#include "proj_render.h"

//#include "Vec3f.hxx"
//#define GLM_FORCE_RADIANS // <-- strange!
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>

#include "stb_font_arial_10_usascii.inl"
static stb_fontchar fontdata[STB_SOMEFONT_NUM_CHARS];

// ---> GL_TRIANGLES
// Vertexarrays (Hauptspeicher)
// OpenGL1.1 - Displaylist (VRam d. Grafikkarte, statisch)
//      2003 - VBO (VRam + einfaches dynamisches Ändern d. Daten)

// VBO tutorial: http://www.ozone3d.net/tutorials/opengl_vbo.php
// using multiple vs. using 1 large VBO: https://www.opengl.org/discussion_boards/showthread.php/176365-Working-with-multiple-VBO-s

zweidee::Render::Render() // constructor
{
  hRC=NULL;                           // Permanent Rendering Context
  hDC=NULL;                           // Private GDI Device Context
//  hWnd=NULL;                          // Holds Our Window Handle

//  static unsigned char fontpixels[STB_SOMEFONT_BITMAP_HEIGHT][STB_SOMEFONT_BITMAP_WIDTH];
//  STB_SOMEFONT_CREATE(fontdata, fontpixels, STB_SOMEFONT_BITMAP_HEIGHT);

//  Init_Textures();
}

int zweidee::Render::Init()
{
  // only f. fixed pipeline --> glEnable(GL_TEXTURE_2D);
  glClearColor(0.3f,0.5f,1.0f,0.0f);

  glClearDepth(1.0f);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST); // <-- !
  // schneidet "zu viel" weg -->    glEnable(GL_CULL_FACE);

  InitShader2(); // Splash screen

  b_splash_screen = true;
    
  return 0;
}

GLvoid zweidee::Render::ReSizeGLScene(GLsizei width, GLsizei height) // Resize And Initialize The GL Window
{
  if (height==0)                     // Prevent A Divide By Zero By
  {
    height = 1;                    // Making Height Equal One
  }
  glViewport(0, 0, width, height);   // Reset The Current Viewport
}

HDC zweidee::Render::GL_attach_to_DC(HWND hWnd)
{ 
  GLuint PixelFormat;                     // Holds The Results After Searching For A Match

#define ERR_HDC           0
#define ERR_PIXELFORMAT   0
#define ERR_CONTEXT       0
#define ERR_GLMAKECURRENT 0

  static PIXELFORMATDESCRIPTOR pfd=       // pfd Tells Windows How We Want Things To Be
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
  if (!(hDC=GetDC(hWnd)))                 // Did We Get A Device Context?
  {
    MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return ERR_HDC;
  }
  if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd))) // Did Windows Find A Matching Pixel Format?
  {
    MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return ERR_PIXELFORMAT;
  }
  if(!SetPixelFormat(hDC,PixelFormat,&pfd)) // Are We Able To Set The Pixel Format?
  {
    char str[255];
    DWORD err = GetLastError();
    sprintf(str, "Can't Set The PixelFormat, err=%d", err);
    MessageBox(NULL,str,"ERROR",MB_OK|MB_ICONEXCLAMATION);

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

  if (!(hRC=wglCreateContext(hDC))) // Are We Able To Get A Rendering Context?
//  if (!(hRC=wglCreateContextAttribsARB(HDC hDC, HGLRC hshareContext, const int *attribList);
  {
    MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return ERR_CONTEXT;
  }
  if(!wglMakeCurrent(hDC,hRC)) // Try To Activate The Rendering Context
  {
    MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return ERR_GLMAKECURRENT;
  }

  ReSizeGLScene(width, height); // Set Up Our Perspective GL Screen
  return hDC;
}
/*
void proj::Render::Init_Textures()
{
  // s. proj.Init() ...
  //    aTextures[0].fXWorld = 5.0f; // [m]
  //    aTextures[0].fYWorld = 5.0f; // [m]
}
*/
/* Bind vertex buffers to VAO's
vertex buffers can hold any information: position, color, uv-coordinates
VAO's only exist from OpengL >=
*/
void zweidee::Render::Bind_VBOs_to_VAOs() // s. http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html
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
  glVertexAttribPointer(sh2_attr_pos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // wichtig, hier das richtige Attrib (nicht 0 oder 1) zu übergeben!
  glEnableVertexAttribArray(sh2_attr_pos);
  err = glGetError();

  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[iVAO]); // u,v-texture-coords
  err = glGetError();
  glVertexAttribPointer(sh2_attr_tex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // texAttrib
  err = glGetError(); // Fehler 1281
  glEnableVertexAttribArray(sh2_attr_tex);
  err = glGetError();

  glBindVertexArray(0); // Unbind
  err = glGetError();


  // ===== 3d-VBO's (regular shader, i.e. Scene + Objects) =====

  for (unsigned int iVAO = 1; iVAO < vVAOs.size(); iVAO++) // <-- start with 1, as 0 is for FPS-coords
  {
    glBindVertexArray(vVertexArray[iVAO]);  // select/bind array and
    // attach a)  position and
    //        b1) col or
    //        b2) texture/uv-buffers
//    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[iVAO]);
//    glVertexAttribPointer(sh1_attr_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // wichtig, hier das richtige Attrib (nicht 0 oder 1) zu übergeben!
//    glEnableVertexAttribArray(sh1_attr_pos);

    if (vVAOs[iVAO].t_Shade == SHADER_COLOR_FLAT) // flat (number of elements per Vertex = 3)
    {    
//      glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[iVAO]);
//      glVertexAttribPointer(sh1_attr_col, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // aeltere OpenGL-Version: glColorPointer
//      glEnableVertexAttribArray(sh1_attr_col); // s. http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_03
    }
    else if (vVAOs[iVAO].t_Shade == SHADER_TEXTURE) // texture (number of elements per Vertex = 2)
    {
      glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[iVAO]); // u,v-texture-coords
//      glVertexAttribPointer(sh1_attr_tex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
      err = glGetError();
//      glEnableVertexAttribArray(sh1_attr_tex);
    }
    glBindVertexArray(0); // Unbind
  }
}

void zweidee::Render::FPS()
{
  std::vector<GLfloat> coords;

  //         +-------+-------+ (1,1)
  //         |       |       |
  //         +-------+ (0,0) +
  //         |       |       |
  // (-1,-1) +---------------+

  coords.push_back(-1.0f); coords.push_back(-1.0f);
  coords.push_back( 1.0f); coords.push_back(-1.0f);
  coords.push_back(-1.0f); coords.push_back( 1.0f);
  coords.push_back(-1.0f); coords.push_back( 1.0f);
  coords.push_back( 1.0f); coords.push_back(-1.0f);
  coords.push_back( 1.0f); coords.push_back( 1.0f);

  //         +-------+ (1,1)
  //         |       |
  //   (0,0) +-------+
  std::vector<GLfloat> uvs;
  uvs.push_back( 0.0f); uvs.push_back(0.0f);
  uvs.push_back( 0.0f); uvs.push_back(1.0f);
  uvs.push_back( 1.0f); uvs.push_back(0.0f);
  uvs.push_back( 1.0f); uvs.push_back(0.0f);
  uvs.push_back( 1.0f); uvs.push_back(1.0f);
  uvs.push_back( 0.0f); uvs.push_back(1.0f);

  // ---------------------------
  // >>> now Push to OpenGL! >>>
  // ---------------------------
  unsigned int ui_idVBO = vVAOs.size();
  glGenBuffers(1, &positionBuffer[ui_idVBO]);
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[ui_idVBO]);
  glBufferData(GL_ARRAY_BUFFER, 6*2*sizeof(GLfloat), &coords[0], GL_STATIC_DRAW);

  glGenBuffers(1, &uvBuffer[ui_idVBO]);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[ui_idVBO]);
  glBufferData(GL_ARRAY_BUFFER, 6*2*sizeof(GLfloat), &uvs[0], GL_STATIC_DRAW);

  c_VAO fps;
  fps.t_Shade = SHADER_TEXTURE;
  fps.Name = "FPS";
  fps.ui_idTexture = TEX_ARIALFONT;
  fps.uiVertexCount = 6;
  vVAOs.push_back(fps);
}

// rotate: https://www.opengl.org/discussion_boards/showthread.php/179290-Rotation-w-Rectangular-Pixels-(2D-VAO)

void zweidee::Render::DrawVAOs_NEU()
{
  /*
  Achtung, hier springt das Programm nochmal 'rein nach Drücken des Close Buttons,
  dann sind aber die vVAO-Inhalte schon beliebig "dirty"!!
  z.B. ist dann vVAOs[ui].b_moving = TRUE und die if Abfrage (s.u.) wird angesprungen
  */

  //    char buf[512];
  //    GLenum glErr;
  glm::vec3 v3;

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glUseProgram(program_fps);

  GLenum err = GL_NO_ERROR;

  // draw Scene + Objects
  unsigned int ui_start = 1;
  if (b_splash_screen) ui_start = 0;
  for (unsigned int ui = ui_start; ui < vVAOs.size(); ui++) // start with 1 as 0 is fps-counter
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
      glBindTexture(GL_TEXTURE_2D, vGLTexture[vVAOs[ui].ui_idTexture-1]); // TEXTURE_ID shall be > 0 !     (-1!!)
      err = glGetError();
      
      if (ui == 0) // hack!!
        glUniform1i(sh2_unif_ID, 0); // hack!!
      else
//        glUniform1i(sh1_unif_sampler, 0)
      ;
    }
    else // vVAOs[ui].t_Shade == SHADER_COLOR_FLAT
    {
//      glUniform1i(sh1_unif_col_tex, 0); // shader into color-branch
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
