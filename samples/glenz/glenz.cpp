#include "stdafx.h"

#define FBUF2D_WIDTH  64 // HACK!! remove!!
#define FBUF2D_HEIGHT 64 // HACK!! remove!!
#include "zweidee_draw.h"
#include "glenz.h"
float z = 3.0;
glm::vec3 tri[3] = { {5.,5.,z},{5.,21.,z},{25.,4.,z} };

int glenz::CGlenz::init()
{
  return TRUE;
}
// https://www.weizmann.ac.il/sci-tea/benari/sites/sci-tea.benari/files/uploads/softwareAndLearningMaterials/quaternion-tutorial-2-0-1.pdf
glm::vec2 rot2(glm::vec2 v, float theta)
{
  glm::vec2 out;
  out.x = v.x * cos(theta) - v.y * sin(theta);
  out.y = v.x * sin(theta) + v.y * cos(theta);
  return out;
}
glm::vec3 rot3x(glm::vec3 v, float theta)
{
  glm::vec3 out;
  out.x = v.x * cos(theta) - v.y * sin(theta) + 0;
  out.y = v.x * sin(theta) + v.y * cos(theta) + 0;
  out.z = 0 + 0 + v.z;
  return out;
}
glm::vec3 rot3y(glm::vec3 v, float theta)
{
  glm::vec3 out;
  out.x = v.x + 0 + 0;
  out.y = 0 + v.y * cos(theta) - v.z * sin(theta);
  out.z = 0 + v.y * sin(theta) + v.z * cos(theta);
  return out;
}
glm::vec3 rot3z(glm::vec3 v, float theta)
{
  glm::vec3 out;
  out.x = v.x * cos(theta) + 0 + v.z * sin(theta);
  out.y = 0 + v.y + 0;
  out.z = -v.x * sin(theta) + 0 + v.z * cos(theta);
  return out;
}
void glenz::CGlenz::lesson2(unsigned char * data) // draw 3 filled triangles
{
  zweidee::vec2 t0[3] = { zweidee::vec2(10, 20), zweidee::vec2(50, 40), zweidee::vec2(20, 30) };
  zweidee::vec2 t1[3] = { zweidee::vec2(41, 12), zweidee::vec2(40, 1),  zweidee::vec2(20, 60) };
  zweidee::vec2 t2[3] = { zweidee::vec2(30, 55), zweidee::vec2(30, 50), zweidee::vec2(40, 60) };
  glm::vec3 red   = glm::vec3(255, 0, 0);
  glm::vec3 white = glm::vec3(255, 255, 255);
  glm::vec3 green = glm::vec3(0, 255, 0);
  Triangle_filled(fbuf2d, t0[0], t0[1], t0[2], red, data);
  Triangle_filled(fbuf2d, t1[0], t1[1], t1[2], white, data);
  Triangle_filled(fbuf2d, t2[0], t2[1], t2[2], green, data);
}

// ---------------
// main game cycle
// ---------------
int glenz::CGlenz::doit(unsigned char * data)
{
//  lesson2(data);
  fbuf2d->setpixel(data, 10, 10, 50, 100, 100); // bgr

  glm::vec3 col = {222,22,222};
  int xc = 32; int yc = 32;
  float f = 1.f;
/*  int x1 = tri[0].x / tri[0].z * f;
  int y1 = tri[0].y / tri[0].z * f;
  int x2 = tri[1].x / tri[1].z * f;
  int y2 = tri[1].y / tri[1].z * f;
  Bresenham(fbuf2d, xc+x1, yc + y1, xc + x2, yc + y2, col, data);

  int x3 = tri[2].x / tri[2].z * f;
  int y3 = tri[2].y / tri[2].z * f;
  Bresenham(fbuf2d, xc + x2, yc + y2, xc + x3, yc + y3, col, data);
  Bresenham(fbuf2d, xc + x3, yc + y3, xc + x1, yc + y1, col, data);
*/
//  tri[0].z -= 0.01;
//  tri[1].z -= 0.01;
//  tri[2].z -= 0.01;
  zweidee::vec2 t0[3];
// there's a problem with rot3y and rot3z --> not anymore :-)
  tri[0] = rot3x(tri[0], .1);
  tri[1] = rot3x(tri[1], .1);
  tri[2] = rot3x(tri[2], .1);
  t0[0].x = xc + tri[0].x / tri[0].z * f;
  t0[0].y = yc + tri[0].y / tri[0].z * f;
  t0[1].x = xc + tri[1].x / tri[1].z * f;
  t0[1].y = yc + tri[1].y / tri[1].z * f;
  t0[2].x = xc + tri[2].x / tri[2].z * f;
  t0[2].y = yc + tri[2].y / tri[2].z * f;
  Triangle_filled(fbuf2d, t0[0], t0[1], t0[2], col, data);

  return TRUE;
}

// these implementations are needed, otherwise the class would be abstract
int glenz::CGlenz::left() { return TRUE; }
int glenz::CGlenz::right() { return TRUE; }
int glenz::CGlenz::fire() { return TRUE; }
int glenz::CGlenz::up() { return TRUE; }
int glenz::CGlenz::down() { return TRUE; }
