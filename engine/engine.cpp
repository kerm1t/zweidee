
#include "stdafx.h"

#include "engine.h"

zweidee::Engine::Engine()
{
  m_game.fbuf2d.width  = FBUF2D_WIDTH;
  m_game.fbuf2d.height = FBUF2D_HEIGHT;
  m_game.fbuf2d.imagesize = m_game.fbuf2d.width * m_game.fbuf2d.height * 3;
  
  fbuf2d = &m_game.fbuf2d;                     // fbuf part of game (e.g. galaga)
  data = new unsigned char[fbuf2d->imagesize]; // data part of proj <-- 2do
}

zweidee::Engine::~Engine()
{
  delete data;
}

int zweidee::Engine::init()
{
	m_render.Init(); // InitGL + Initshaders, kann auch spaeter aufgerufen werden...

	m_render.FPS(); // <-- wenn ich das ins VAO fuelle, gibt's nen Fehler (erst mit dem neuen ShaderFPS)
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

int zweidee::Engine::move()
{
  if (
      ((!bPause) || // key [p] pressed
        (bStep))    // key [o] pressed
     )
  {
    memset(data, 0, fbuf2d->imagesize); // clear

    // -----------
    // game cycle
    // -----------
    m_game.doit(data);

    // bindet sich an die letzte Textur :-)
    //  glBindTexture(GL_TEXTURE_2D, 1); // TEXTURE_ID shall be > 0 !     (-1!!)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbuf2d->width, fbuf2d->height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);   // hier gibt es Schwierigkeiten mit .bmp,

    fbuf2d->framecounter++;
    bStep = false;
  }
  return 0;
}

int zweidee::Engine::render()
{
  wglMakeCurrent(m_render.hDC,m_render.hRC); // ;-) now Tab-switching in MTS possible

  m_render.DrawVAOs_NEU();      // Draw The Scene

  SwapBuffers(m_render.hDC);    // Swap Buffers (Double Buffering)

  return 0;
}

int zweidee::Engine::fire()
{
  m_game.fire();
  return true;
}

int zweidee::Engine::up()
{
  // not used
  return true;
}

int zweidee::Engine::down()
{
  // not used
  return true;
}

int zweidee::Engine::left()
{
  m_game.left();
  return true;
}

int zweidee::Engine::right()
{
  m_game.right();
  return true;
}
