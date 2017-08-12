
#include "stdafx.h"

#include "proj.h"

proj::Proj::Proj()
{
//  fbuf2d.width = 64;
//  fbuf2d.height = 64;
//  fbuf2d.imageSize = fbuf2d.width * fbuf2d.height * 3;
//  data = new unsigned char[fbuf2d.imageSize];

//  m_galaga.fbuf2D = fbuf2d;
  // redundant --> remove!
  m_galaga.fbuf2d.width = 64;
  m_galaga.fbuf2d.height = 64;
  m_galaga.fbuf2d.imageSize = 64 * 64 * 3;
  
  fbuf2d = &m_galaga.fbuf2d;
  data = new unsigned char[fbuf2d->imageSize];
}

proj::Proj::~Proj()
{
  delete data;
}

int proj::Proj::Init()
{
  m_render.Init(); // InitGL + Initshaders, kann auch später aufgerufen werden...

  m_render.FPS(); // <-- wenn ich das ins VAO fülle, gibt's nen Fehler (erst mit dem neuen ShaderFPS)
                  //     beim LoadObjects(s.u.) call

  GLuint texID = fbuf2d->framebuf2D();
  m_render.vGLTexture.push_back(texID);

  // b) bind to VAO's
  m_render.Bind_VBOs_to_VAOs(); // now hand over VBO's to VAO's

  // ---------------
  // init your game!
  // ---------------
  m_galaga.init();

  return 0;
}

int proj::Proj::DoIt()
{
  wglMakeCurrent(m_render.hDC,m_render.hRC); // ;-) now Tab-switching in MTS possible

  // objects' movement --> has to be done in an own loop / !?thread!?
  if ((!bPause) || // key [p] pressed
      (bStep))     // key [o] pressed
  {

    memset(data, 0, fbuf2d->imageSize); // clear

    // ---------------------
    // game cycle
    // -----------
    m_galaga.doit(data);

    // bindet sich an die letzte Textur :-)
    //  glBindTexture(GL_TEXTURE_2D, 1); // TEXTURE_ID shall be > 0 !     (-1!!)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbuf2d->width, fbuf2d->height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);   // hier gibt es Schwierigkeiten mit .bmp,

    bStep = false;
  }

  m_render.DrawVAOs_NEU();      // Draw The Scene

  SwapBuffers(m_render.hDC);    // Swap Buffers (Double Buffering)

  return 0;
}

int proj::Proj::fire()
{
  m_galaga.fire();
  return true;
}

int proj::Proj::up()
{
  // not used
  return true;
}

int proj::Proj::down()
{
  // not used
  return true;
}

int proj::Proj::left()
{
  m_galaga.left();
  return true;
}

int proj::Proj::right()
{
  m_galaga.right();
  return true;
}

