
#include "stdafx.h"

#include "proj.h"

proj::Proj::Proj()
{
  m_game.fbuf2d.width = 64;
  m_game.fbuf2d.height = 64;
  m_game.fbuf2d.imageSize = 64 * 64 * 3;
  
  fbuf2d = &m_game.fbuf2d;                     // fbuf part of game (e.g. galaga)
  data = new unsigned char[fbuf2d->imageSize]; // data part of proj <-- 2do
}

proj::Proj::~Proj()
{
  delete data;
}

int proj::Proj::init()
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
  m_game.init();

  return 0;
}

int proj::Proj::move()
{
  if (
      ((!bPause) || // key [p] pressed
        (bStep))    // key [o] pressed
     )
  {
    memset(data, 0, fbuf2d->imageSize); // clear

    // -----------
    // game cycle
    // -----------
    m_game.doit(data);

    // bindet sich an die letzte Textur :-)
    //  glBindTexture(GL_TEXTURE_2D, 1); // TEXTURE_ID shall be > 0 !     (-1!!)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbuf2d->width, fbuf2d->height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);   // hier gibt es Schwierigkeiten mit .bmp,

    bStep = false;
  }
  return 0;
}

int proj::Proj::render()
{
  wglMakeCurrent(m_render.hDC,m_render.hRC); // ;-) now Tab-switching in MTS possible

  m_render.DrawVAOs_NEU();      // Draw The Scene

  SwapBuffers(m_render.hDC);    // Swap Buffers (Double Buffering)

  return 0;
}

int proj::Proj::fire()
{
  m_game.fire();
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
  m_game.left();
  return true;
}

int proj::Proj::right()
{
  m_game.right();
  return true;
}
