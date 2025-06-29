#include "stdafx.h"

#define FBUF2D_WIDTH  64 // HACK!! remove!!
#define FBUF2D_HEIGHT 64 // HACK!! remove!!
#include "zweidee_draw.h"
#include "glenz.h"


glm::vec3 tri[3] = { {0.,0.,0.},{.2,0.,0.},{.2,.2,0.} }; // pink
glm::vec3 tri2[3] = { {.2,.2,0.},{0.,0.,0.},{0.,.2,0.} };
//glm::vec3 tri2[3] = { {0.,0.,0.},{-.1,0.,0.},{-.1,0.,.1} };

class camera
{
  glm::vec3 pos;
  glm::vec3 dir;
  glm::vec3 focal_plane;

};

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
glm::vec3 rot3z(glm::vec3 v, float theta)
{
  glm::vec3 out;
  out.x = v.x * cos(theta) - v.y * sin(theta) + 0;
  out.y = v.x * sin(theta) + v.y * cos(theta) + 0;
  out.z = 0 + 0 + v.z;
  return out;
}
glm::vec3 rot3x(glm::vec3 v, float theta)
{
  glm::vec3 out;
//  theta = 0.0f;
//  theta = 3.145f/2.0f;
//  theta = 3.145f*3.0f / 2.0f;;
  out.x = v.x + 0. + 0.;
  out.y = 0. + v.y * cos(theta) - v.z * sin(theta);
  out.z = 0. + v.y * sin(theta) + v.z * cos(theta);
//  glm::mat3 r = { {1,0,0},
//                  {0,cos(theta),-sin(theta)},
//                  {0,sin(theta), cos(theta)} };
//  out =  r*v;
  return out;
}
glm::vec3 rot3y(glm::vec3 v, float theta)
{
  glm::vec3 out;
  out.x = v.x * cos(theta) + 0 + v.z * sin(theta);
  out.y = 0 + v.y + 0;
  out.z = -v.x * sin(theta) + 0 + v.z * cos(theta);
  return out;
}

// 2do: 
zweidee::vec2 persp(glm::vec3 v, glm::vec2 center, float focal)
{
  zweidee::vec2 out;
//#define ALMOST_ZERO 0.000001f // this is very slow
#define ALMOST_ZERO 0.01f // this is not so slow
  if (abs(v.z) < ALMOST_ZERO) v.z = ALMOST_ZERO;
  out.x = center.x + v.x / v.z * focal;
  out.y = center.y + v.y / v.z * focal;
  return out;
}

// ---------------
// main game cycle
// ---------------
// 2do:
// - camera
// - ImGUI
// - better rotation, maybe doing translation first
// + draw axes r,g,b
// 
int glenz::CGlenz::doit(unsigned char * data)
{
//  fbuf2d->setpixel(data, 10, 10, 50, 100, 100); // bgr

  glm::vec3 pink = { 212,22,212 };
  glm::vec3 white = { 255,255,255 };

// perspective projection
  glm::vec2 center = { 32, 32 };
  float f = 1.f;
  zweidee::vec2 t0[3];

// there's more problems with rot3y and rot3x
  zweidee::glenz = true;
  float rot = 0.03f;
  tri[0] = rot3x(tri[0], rot);
  tri[1] = rot3x(tri[1], rot);
  tri[2] = rot3x(tri[2], rot);
  t0[0] = persp(tri[0], center, f);
  t0[1] = persp(tri[1], center, f);
  t0[2] = persp(tri[2], center, f);
  Triangle_filled(fbuf2d, t0[0], t0[1], t0[2], pink, data);

  static float rot2;
  glm::vec3 tmp[3];
  rot2 += 0.01f;
  tmp[0] = rot3y(tri2[0], rot2);
  tmp[1] = rot3y(tri2[1], rot2);
  tmp[2] = rot3y(tri2[2], rot2);
//  tmp[0] = rot3y(tmp[0], 3.14f/2.0f);
//  tmp[1] = rot3y(tmp[1], 3.14f / 2.0f);
//  tmp[2] = rot3y(tmp[2], 3.14f / 2.0f);
//  tmp[0].z = tmp[0].z - .1f;
//  tmp[1].z = tmp[1].z - .1f;
//  tmp[2].z = tmp[2].z - .1f;
  t0[0] = persp(tmp[0], center, f);
  t0[1] = persp(tmp[1], center, f);
  t0[2] = persp(tmp[2], center, f);
  Triangle_filled(fbuf2d, t0[0], t0[1], t0[2], white, data);

  // draw axes
  zweidee::glenz = false;
  glm::vec3 x[2] = { {0.,0.,0.},{.1,0.,0.} };
  glm::vec3 y[2] = { {0.,0.,0.},{0.,.1f,0.} };
  glm::vec3 z[2] = { {0.,0.,0.},{0.,0.,.1f} };
  t0[0] = persp(x[0], center, f);
  t0[1] = persp(x[1], center, f);
  Bresenham(fbuf2d, t0[0].x, t0[0].y, t0[1].x, t0[1].y, glm::vec3{ 255,0,0 }, data);
  t0[0] = persp(y[0], center, f);
  t0[1] = persp(y[1], center, f);
  Bresenham(fbuf2d, t0[0].x, t0[0].y, t0[1].x, t0[1].y, glm::vec3{ 0,255,0 }, data);
  t0[0] = persp(z[0], center, f);
  t0[1] = persp(z[1], center, f);
  Bresenham(fbuf2d, t0[0].x, t0[0].y, t0[1].x, t0[1].y, glm::vec3{ 0,0,255 }, data);

  return TRUE;
}

// these implementations are needed, otherwise the class would be abstract
int glenz::CGlenz::left() { return TRUE; }
int glenz::CGlenz::right() { return TRUE; }
int glenz::CGlenz::fire() { return TRUE; }
int glenz::CGlenz::up() { return TRUE; }
int glenz::CGlenz::down() { return TRUE; }
