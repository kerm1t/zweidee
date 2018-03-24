// --------------
//
// 2d framebuffer
//
// --------------

#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include <stdio.h>

//frogger
#define FBUF2D_WIDTH  200
#define FBUF2D_HEIGHT 320

//galaga
//#define FBUF2D_WIDTH  64
//#define FBUF2D_HEIGHT 64

class FrameBuf2D // which is actually an animated (OpenGL-)Texture
{
public:
  unsigned int width, height;
  unsigned int imagesize;   // = width*height*3
  unsigned int framecounter;

  /*
  (0,0): rgb (1,0): rgb (2,0): rgb
  */
  void setPixel(unsigned char * data, const unsigned int x, const unsigned int y, const unsigned char r, const unsigned char g, const unsigned char b)
  {
    unsigned int _y = height - 1 - y; // hack, texture upside down? 2do: check!!
//    unsigned int _y = y;
    unsigned int pos = 3 * (_y * width + x);
//    assert(pos < imageSize);
    if (pos >= imagesize) return;
    if (x > width) return; // test < 0 ?? <-- need signed then
    data[pos]     = b; // r ?
    data[pos + 1] = g;
    data[pos + 2] = r; // b ?
  }

  GLuint framebuf2D() // purpose: get texture-ID
  {
    // Actual RGB data
    unsigned char * data;

    width = FBUF2D_WIDTH;
    height = FBUF2D_HEIGHT;
    imagesize = width*height*3;

    // Create a buffer
    data = new unsigned char[imagesize];
    memset(data, 0, imagesize); // clear

    /*
      draw some stuff
    */
    for (unsigned int i = 1; i < (height-1); i++)
    {
      char r, g, b;
      r = ((i + 1) % 3) * 255;
      g = ( i      % 3) * 255;
      b = ((i - 1) % 3) * 255;
      setPixel(data,       i, i, r, g, b);
      setPixel(data, width-i, i, r, g, b);
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);   // hier gibt es Schwierigkeiten mit .bmp,

    delete data;

    return textureID;
  }
};
