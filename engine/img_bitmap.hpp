// -----------------------
//
// Read Bitmap (*.bmp) into Texture
//
// -----------------------

#include "stdafx.h"
#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>   // Header File For The OpenGL32 Library

#include <stdio.h>

class CBMPLoader
{
public:
  
  /*
    (0,0): rgb (1,0): rgb (2,0): rgb
  */
/*  void setPixel(unsigned char * data, int x, int y, char r, char g, char b)
  {
    data[3* ( y * 128 + x)]   = r;
    data[3* ( y * 128 + x)+1] = g;
    data[3* ( y * 128 + x)+2] = b;
  }
  */
  GLuint loadBMP_custom(const char * imagepath)
  {
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char * data;
    size_t result;

    // Open the file
    FILE * file = fopen(imagepath,"rb");
    assert(file);
    if (!file)
    {
      printf("Image could not be opened\n");
      return 0;
    }

    if (fread(header, 1, 54, file) != 54) // If not 54 bytes read : problem
    {
      printf("Not a correct BMP file\n");
      return false;
    }

    if (header[0] != 'B' || header[1] != 'M')
    {
      printf("Not a correct BMP file\n");
      return 0;
    }

    // Read ints from the byte array
    dataPos   = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width     = *(int*)&(header[0x12]);
    height    = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0) imageSize = width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0)   dataPos = 54; // The BMP header is done that way

    // Create a buffer
    data = new unsigned char[imageSize];

    // Read the actual data from the file into the buffer
    result = fread(data,1,imageSize,file);

    /*
    
      draw some stuff
    
    */
/*    for (int i = 0; i < 40; i++)
    {
      char r, g, b;
      r = ((i+1) % 3) * 255;
      g = ( i    % 3) * 255;
      b = ((i-1) % 3) * 255;
      setPixel(data, i, i, r, g, b);
      setPixel(data, (2*40-2)-i, i, r,g,b);
    }
*/
    // Everything is in memory now, the file can be closed
    fclose(file);

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
