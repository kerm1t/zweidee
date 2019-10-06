#include "stdafx.h"

#include "comanche.h"
#include "img_bitmap.hpp"

// https://github.com/s-macke/VoxelSpace
// s. http://www.flipcode.com/archives/Realtime_Voxel_Landscape_Engines-Part_1_Introduction.shtml

int comanche::CComanche::init()
{
  CBMPLoader bmp = CBMPLoader();
  bmp.loadBMP_custom("C:/Projekte/zweidee/data/C1W.bmp"); // 150 x 150
  for (int y = 0; y < 64; y++)
  {
    for (int x = 0; x < 64; x++)
    {
//      bmp[]
    }
  }
  return 0;
}

void comanche::CComanche::DrawVerticalLine(unsigned char * data, int x, int y1, int y2, zweidee::colrgb col) // <--
{
  for (int y = y1; y < y2; y++)
  {
    fbuf2d.setpixel(data, x, y, col.r, col.g, col.b);
  }
}

//  def Render(p, height, horizon, scale_height, distance, screen_width, screen_height) :
int comanche::CComanche::render(unsigned char * data, zweidee::point p, int height, int horizon, int scale_height, int distance, int screen_width, int screen_height)
{
  // Draw from back to the front(high z coordinate to low z coordinate)
/*  for z in range(distance, 1, -1) :
    # Find line on map.This calculation corresponds to a field of view of 90°
    pleft = Point(-z + p.x, -z + p.y)
    pright = Point(z + p.x, -z + p.y)
    # segment the line
    dx = (pright.x - pleft.x) / screen_width
    # Raster line and draw a vertical line for each segment
    for i in range(0, screen_width) :
      height_on_screen = (height - heightmap[pleft.x, pleft.y]) / z * scale_height. + horizon
      DrawVerticalLine(i, height_on_screen, screen_height, colormap[pleft.x, pleft.y])
      p1eft.x += dx
*/
  for (int z = distance; z > 1; z--)
  {
    zweidee::point pleft  = zweidee::point(-z + p.x, -z + p.y);
    zweidee::point pright = zweidee::point( z + p.x, -z + p.y);
    int dx = (pright.x - pleft.x) / screen_width;
    for (int i = 0; i < screen_width; i++)
    {
//      int h = *heightmap[pleft.x, pleft.y];
      int height_on_screen = (height - *heightmap[pleft.x, pleft.y]) / z * (float)scale_height + horizon;
      DrawVerticalLine(data, i, height_on_screen, screen_height, *colormap[pleft.x, pleft.y]);
      pleft.x += dx;
    }
  }
  return 0;
}

// ---------------
// main game cycle
// ---------------
int comanche::CComanche::doit(unsigned char * data)
{
  // Call the render function with the camera parameters :
  // position, height, horizon line position,
  // scaling factor for the height, the largest distance,
  // screen width and the screen height parameter
  render(data, zweidee::point(0, 0), 50, 32, 40, 20, 64, 64);
   
  int x = 32;
  int y = 32;
  char r = 255;
  char g = r;
  char b = r;
  fbuf2d.setpixel(data, x, y, r, g, b);

  return 0;
}

int comanche::CComanche::left()
{
  return 0;
}

int comanche::CComanche::right()
{
  return 0;
}

int comanche::CComanche::fire()
{
  return 0;
}
