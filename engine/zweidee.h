#pragma once

//#include "resource.h"

#include <vector>
#include <string>

#include "../common/types.h"

//#include "Vec3f.hxx"
#include "math.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>    // Header File For The OpenGL32 Library
#include <gl\glu.h>   // Header File For The GLu32 Library


#include <stdio.h> // <-- sprintf
#include "inc_render.h"
//#define GLM_FORCE_RADIANS // 2do, replace all deg. by rad.!!
#include "glm.hpp"
//#include <gtc/matrix_transform.hpp>

//#include "proj.h"

namespace zweidee
{

/// from .h

	struct colrgb
	{
		char r;
		char g;
		char b;
	};

	struct point
	{
		unsigned int x;
		unsigned int y;
		point(const unsigned int _x, const unsigned int _y) : x(_x), y(_y) {}
	};

	class ShaderMan // Shader manager
	{
	public:
		// =============
		// GLSL / Shader
		// =============



		// ** Shader2 **
		GLuint program_fps; // = sh2
							// attrib + uniform
		GLuint sh2_attr_pos;
		GLuint sh2_attr_tex;
		GLuint sh2_unif_ID;



		void InitShader2()
		{
			GLenum err = glGetError();

			char buffer[512];

			/*
			4 coord.systems: Object, World, View, Clip
			==========================================
			Object --> Model Matrix --> World
			World --> View matrix --> View
			View --> Projection Matrix (w. implicit homogeneous divide) --> Clip
			*/
			/* vertex shader : output always to "homogeneous clip space", i.e. (-1:1, -1:1, -1:1, -1:1) */
			const GLchar * vshd_src_FPS[] = {
				"#version 330 core\n" // 410 not supported by SONY OpenGL driver
				"in vec2 vp_clipspace;\n"
				"out vec2 UV;\n"
				"void main()\n"
				"{\n"
				"  UV = (vp_clipspace+1.0) * 0.5;\n"
				"  gl_Position = vec4(vp_clipspace.x, vp_clipspace.y, 0.0, 1.0);\n"
				//      "  gl_Position.xy *= 0.5;\n"
				"  gl_Position.xy *= 1.0;\n"
				"}"
			};
			const GLchar * fshd_src_FPS[] = {
				"#version 330 core\n"
				"in vec2 UV;\n"
				"out vec3 color;\n"
				"uniform sampler2D myTexSampler;\n"
				"void main()\n"
				"{\n"
				"  color = texture(myTexSampler, UV).rgb;\n" // texture2D ist deprecated
				"}"
			};


			// Shader for FPS
			GLuint vshaderFPS = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vshaderFPS, 1, vshd_src_FPS, 0);
			glCompileShader(vshaderFPS);
			glGetShaderInfoLog(vshaderFPS, 512, NULL, buffer); // <-- debug, kann man sich schoen im debugger ansehen!!
			err = glGetError();
			// 2do: check that buffer = "No errors."
			GLuint fshaderFPS = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fshaderFPS, 1, fshd_src_FPS, 0); // set array of strings as source code
			glCompileShader(fshaderFPS); // compile
			glGetShaderInfoLog(fshaderFPS, 512, NULL, buffer); // <-- debug
			err = glGetError();
			// 2do: check that buffer = "No errors."
			program_fps = glCreateProgram(); // create empty program object
			glAttachShader(program_fps, vshaderFPS); // attach shader
			glAttachShader(program_fps, fshaderFPS); // attach shader
			glLinkProgram(program_fps); // link
			err = glGetError();
			glUseProgram(program_fps);
			err = glGetError();

			// attribs
			sh2_attr_pos = glGetAttribLocation(program_fps, "vp_clipspace");
			err = glGetError();
			sh2_attr_tex = glGetAttribLocation(program_fps, "vertexUV");     // UV geht auch
			err = glGetError();
			sh2_unif_ID = glGetUniformLocation(program_fps, "myTexSampler");
			err = glGetError();
		}
	};

/// from .h


	// ========
	// Textures
	// ========
#define TEX_ARIALFONT    1 // war vorher 0, da gab's einen Fehler!!
#define TEX_ROADSURFACE  2
	//#define TEX_TRAFFICSIGN  3
	//#define TEX_CAR          4 // <-- this is actually the body
	//#define TEX_CAR_WINDOWS  5
	//#define TEX_CAR_TIRE     6
	//#define TEX_BARRIER      7
	//  static const unsigned int TEXCOUNT = 7;

	enum tShading { SHADER_COLOR_FLAT, SHADER_TEXTURE };

	/*
	VAO, s. http://lwjgl.org/wiki/index.php?title=The_Quad_with_DrawArrays
	explained: http://stackoverflow.com/questions/5970087/understanding-vertex-array-objects-glgenvertexarrays
	VAO (>=OGL3.0): bundle multiple VBO's for easier handling, don't need to bind ... them each
	... really good tut: http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html
	*/
	class c_VAO //: public obj::CObject
	{
	public:
		std::string Name;
		tShading t_Shade;

		/*    uint16 idVBO_pos;		// OpenGL VBO
		uint16 idVBO_tex;
		uint16 idVBO_col;
		*/
		uint16 ui_idTexture;    // TextureID

		bool b_doDraw;
		bool b_Wireframe;
		//    Vec3f vPos;
		uint16 uiVertexCount;
		bool b_moving;

		c_VAO::c_VAO()
		{
			ui_idTexture = 0;
			b_doDraw = true;
			b_Wireframe = false;
			//      vPos         = Vec3f(0.0f,0.0f,0.0f);
			b_moving = false;
		}
	};

	/*
	class c_Texture
	{
	public:
	char c_filename[255];
	float fXWorld,fYWorld;
	};
	*/


/// from .h

	class Render : public zweidee::ShaderMan
	{
	public:
		HGLRC hRC;         // Permanent Rendering Context
		HDC   hDC;         // Private GDI Device Context
		HWND  hWnd;        // Holds Our Window Handle
		HWND  dummyHWND;   // for NVidia setpixelformat-Fix
		int width, height;

		//        bool b_PNG;

		// =================
		// Objects' pointers
		// =================
		// objects shall store VAO-id und Texture-id
		//		std::vector <obj::CObject> * p_vObjects;

		//        c_Texture aTextures[TEXCOUNT];
		//		std::vector<c_Texture> vTextures;

		//        GLuint Texture[TEXCOUNT]; // --> checken, dass nicht mit der FBO-Textur kollidiert!
		std::vector<GLuint> vGLTexture;
		// =====
		// VAO's ... VAO = {VBO1,VBO2,VB3,...}
		// =====
		/*        #define VBO_2TRIANGLES   0
		#define VBO_LEFT         1
		#define VBO_RIGHT        2
		#define VBO_ROAD         3
		#define VBO_TRAFFICSIGNS 4
		#define VBO_GUARDRAIL    5
		#define VBO_CURBSTONES   6
		#define VBO_MOVING1      7
		#define VBO_MOVING2      8
		#define VBO_CAR			 9 // <-- entfernt!!
		#define VBO_CAR_WINDOWS 10 // <-- with parts!!
		#define VBO_CAR_BODY    11
		#define VBO_CAR_TIRE1   12
		#define VBO_CAR_TIRE2   13
		*/        // nur statische integrale Datentypen können innerhalb einer Klasse initialisiert werden...
		static const unsigned int VBOCOUNT = 99;

		// pointers to all VAO buffer's
		GLuint positionBuffer[VBOCOUNT];
		GLuint    colorBuffer[VBOCOUNT]; // either color or ...
		GLuint       uvBuffer[VBOCOUNT]; // texture
										 //    GLuint ui_numVBOpos = 0;
										 //    GLuint ui_numVBOtex = 0;
										 //    GLuint ui_numVBOcol = 0;

		std::vector<c_VAO>  vVAOs;
		std::vector<GLuint> vVertexArray;    // stores VAO's: a) Position(x,y,z), b1) color OR b2) u,v-Texture

											 //    std::vector<GLuint> vPositionBuffer; // stores position

		bool b_splash_screen;

		// =============
		//    Camera * p_cam;

		//    proj::Scene * m_Scene;

		Render();
		int Init();

		HDC GL_attach_to_DC(HWND hWnd);

		GLvoid ReSizeGLScene(GLsizei width, GLsizei height); // Resize And Initialize The GL Window
															 //    void Init_Textures();

															 //    void Groundplane();
		void FPS();
		//    void Triangles_to_VBO(Vec3f v3pos); // most simple VBO/VAO, just in case errors happen...
		//    int Scene_to_VBO();//uint * p_idxVBO);
		//    int DestroyScene_VBO();

		//    void get_xyz_Hack(int iT, float &x, float &y, float &z, float &xto, float &yto, float &zto);

		void Bind_VBOs_to_VAOs();
		void DrawVAOs_NEU();

	private:
		static int const fbo_width = 512;
		static int const fbo_height = 512;
		GLuint fb, color, depth; // <--- nicer names!
	};

}
