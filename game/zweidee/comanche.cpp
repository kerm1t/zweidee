#include "stdafx.h"

#include "comanche.h"

// https://github.com/s-macke/VoxelSpace
// s. http://www.flipcode.com/archives/Realtime_Voxel_Landscape_Engines-Part_1_Introduction.shtml

int comanche::CComanche::init()
{
  return 0;
}

//  def Render(p, height, horizon, scale_height, distance, screen_width, screen_height) :
int render(zweidee::point p, int height, int horizon, int scale_height, int distance, int screen_width, int screen_height)
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
  render(zweidee::point(0, 0), 50, 120, 120, 300, 800, 600);
    
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
