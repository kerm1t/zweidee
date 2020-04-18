
#include "stdafx.h"

#define FBUF2D_WIDTH  64 // HACK!! remove!!
#define FBUF2D_HEIGHT 64 // HACK!! remove!!
#include "engine.h"

zweidee::CEngine::CEngine()
{
//  fbuf2d.width  = FBUF2D_WIDTH; // 2do: this shall be from the input
//  fbuf2d.height = FBUF2D_HEIGHT;
//  fbuf2d.imagesize = m_game.fbuf2d.width * m_game.fbuf2d.height * 3;
  
//  fbuf2d = &m_game.fbuf2d;                     // fbuf part of game (e.g. galaga)
//  data = new unsigned char[fbuf2d->imagesize]; // data part of proj <-- 2do
}

zweidee::CEngine::~CEngine()
{
//  delete data;
}

int zweidee::CEngine::init(FrameBuf2D * fb2, unsigned char * dta)
{
//	m_render.Init(); // InitGL + Initshaders, kann auch spaeter aufgerufen werden...

//	m_render.FPS(); // <-- wenn ich das ins VAO fuelle, gibt's nen Fehler (erst mit dem neuen ShaderFPS)
				          //     beim LoadObjects(s.u.) call

//	GLuint texID = fbuf2d->framebuf2D();
//	m_render.vGLTexture.push_back(texID);

	// b) bind to VAO's
//	m_render.Bind_VBOs_to_VAOs(); // now hand over VBO's to VAO's
								  
	// ---------------
  // init your game!
  // ---------------
  fbuf2d = fb2;
  data = dta;
  m_game.fbuf2d = fb2;
  m_game.init();

  return 0;
}

int zweidee::CEngine::move()
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

int zweidee::CEngine::render()
{
//  wglMakeCurrent(m_render.hDC,m_render.hRC); // ;-) now Tab-switching in MTS possible

//  m_render.DrawVAOs_NEU();      // Draw The Scene

//  SwapBuffers(m_render.hDC);    // Swap Buffers (Double Buffering)

  return 0;
}

int zweidee::CEngine::fire()
{
  m_game.fire();
  return true;
}

int zweidee::CEngine::up()
{
  // not used ?????
  m_game.up();
  return true;
}

int zweidee::CEngine::down()
{
  // not used ??
  m_game.down();
  return true;
}

int zweidee::CEngine::left()
{
  m_game.left();
  return true;
}

int zweidee::CEngine::right()
{
  m_game.right();
  return true;
}
