#ifndef ZWEIDEE_H
#define ZWEIDEE_H

#include <stdio.h> // <-- sprintf
#include <vector>
#include <iostream> // <-- fbo to pgm|ppm
#include <fstream> // <-- fbo to pgm|ppm
#include <string>

#include "../common/types.h"


#define GLEW_STATIC
#include <GL/glew.h>
#include <gl\gl.h>    // Header File For The OpenGL32 Library
#include <gl\glu.h>   // Header File For The GLu32 Library

//#include "inc_render.h"
//#define GLM_FORCE_RADIANS // 2do, replace all deg. by rad.!!
#include "glm.hpp"


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

		Render() // constructor
		{
			hRC = NULL;                           // Permanent Rendering Context
			hDC = NULL;                           // Private GDI Device Context
		}

		int Init()
		{
			// only f. fixed pipeline --> glEnable(GL_TEXTURE_2D);
			glClearColor(0.3f, 0.5f, 1.0f, 0.0f);

			glClearDepth(1.0f);
			glDepthFunc(GL_LESS);
			glEnable(GL_DEPTH_TEST); // <-- !
									 // schneidet "zu viel" weg -->    glEnable(GL_CULL_FACE);

			InitShader2(); // Splash screen

			b_splash_screen = true;

			return 0;
		}

		HDC GL_attach_to_DC(HWND hWnd)
		{
			GLuint PixelFormat;                     // Holds The Results After Searching For A Match

#define ERR_HDC           0
#define ERR_PIXELFORMAT   0
#define ERR_CONTEXT       0
#define ERR_GLMAKECURRENT 0

			static PIXELFORMATDESCRIPTOR pfd =       // pfd Tells Windows How We Want Things To Be
			{
				sizeof(PIXELFORMATDESCRIPTOR),      // Size Of This Pixel Format Descriptor
				1,                                  // Version Number
				PFD_DRAW_TO_WINDOW |                // Format Must Support Window
				PFD_SUPPORT_OPENGL |                // Format Must Support OpenGL
				PFD_DOUBLEBUFFER,                   // Must Support Double Buffering
				PFD_TYPE_RGBA,                      // Request An RGBA Format
				8, /*32*/                                // framebuffer Color Depth
				0, 0, 0, 0, 0, 0,                   // Color Bits Ignored
				0,                                  // No Alpha Buffer
				0,                                  // Shift Bit Ignored
				0,                                  // No Accumulation Buffer
				0, 0, 0, 0,                         // Accumulation Bits Ignored
				16,                                 // 16Bit Z-Buffer (Depth Buffer)
				0,                                  // No (number of) bits for Stencil Buffer
				0,                                  // No bits for Auxiliary Buffer
				PFD_MAIN_PLANE,                     // Main Drawing Layer
				0,                                  // Reserved
				0, 0, 0                             // Layer Masks Ignored
			};
			if (!(hDC = GetDC(hWnd)))                 // Did We Get A Device Context?
			{
				MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
				return ERR_HDC;
			}
			if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) // Did Windows Find A Matching Pixel Format?
			{
				MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
				return ERR_PIXELFORMAT;
			}
			if (!SetPixelFormat(hDC, PixelFormat, &pfd)) // Are We Able To Set The Pixel Format?
			{
				char str[255];
				DWORD err = GetLastError();
				sprintf(str, "Can't Set The PixelFormat, err=%d", err);
				MessageBox(NULL, str, "ERROR", MB_OK | MB_ICONEXCLAMATION);

				// --> on NVidia GTX 780 error : -1073283066 = 0xc0070006
				return ERR_PIXELFORMAT;
			}

			/*  ... when running in MTS-Thread --> s. http://www.graphicsgroups.com/6-opengl/0bafc0120a809ed8.htm

			According to the Windows OpenGL documentation:

			"Setting the pixel format of a window more than once can lead to significant
			complications for the Window Manager and for multithread applications, so it
			is not allowed. An application can only set the pixel format of a window one
			time. Once a window's pixel format is set, it cannot be changed."

			The card drivers that allow you do change the format are not following the
			rules; the ones (like GeForce3) that are giving you problems are giving you
			problems because you're violating the rules.
			As has been suggested elsewhere, create for yourself an invisible and/or
			off-screen dummy window, and use the rendering context you create for that
			window as your display-list share. You can either create it from a
			registered class with CS_OWNDC style, or you can just create a compatible DC
			and then not delete that DC until the program is done. Either way, this will
			be low-overhead solution, plus it should work on all compliant video
			drivers.
			*/

			if (!(hRC = wglCreateContext(hDC))) // Are We Able To Get A Rendering Context?
												//  if (!(hRC=wglCreateContextAttribsARB(HDC hDC, HGLRC hshareContext, const int *attribList);
			{
				MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
				return ERR_CONTEXT;
			}
			if (!wglMakeCurrent(hDC, hRC)) // Try To Activate The Rendering Context
			{
				MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
				return ERR_GLMAKECURRENT;
			}

			ReSizeGLScene(width, height); // Set Up Our Perspective GL Screen
			return hDC;
		}

		GLvoid ReSizeGLScene(GLsizei width, GLsizei height) // Resize And Initialize The GL Window
		{
			if (height == 0)                   // Prevent A Divide By Zero By
			{
				height = 1;                    // Making Height Equal One
			}
			glViewport(0, 0, width, height);   // Reset The Current Viewport
		}

		void FPS()
		{
			std::vector<GLfloat> coords;

			//         +-------+-------+ (1,1)
			//         |       |       |
			//         +-------+ (0,0) +
			//         |       |       |
			// (-1,-1) +---------------+

			coords.push_back(-1.0f); coords.push_back(-1.0f);
			coords.push_back(1.0f); coords.push_back(-1.0f);
			coords.push_back(-1.0f); coords.push_back(1.0f);
			coords.push_back(-1.0f); coords.push_back(1.0f);
			coords.push_back(1.0f); coords.push_back(-1.0f);
			coords.push_back(1.0f); coords.push_back(1.0f);

			//         +-------+ (1,1)
			//         |       |
			//   (0,0) +-------+
			std::vector<GLfloat> uvs;
			uvs.push_back(0.0f); uvs.push_back(0.0f);
			uvs.push_back(0.0f); uvs.push_back(1.0f);
			uvs.push_back(1.0f); uvs.push_back(0.0f);
			uvs.push_back(1.0f); uvs.push_back(0.0f);
			uvs.push_back(1.0f); uvs.push_back(1.0f);
			uvs.push_back(0.0f); uvs.push_back(1.0f);

			// ---------------------------
			// >>> now Push to OpenGL! >>>
			// ---------------------------
			unsigned int ui_idVBO = vVAOs.size();
			glGenBuffers(1, &positionBuffer[ui_idVBO]);
			glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[ui_idVBO]);
			glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(GLfloat), &coords[0], GL_STATIC_DRAW);

			glGenBuffers(1, &uvBuffer[ui_idVBO]);
			glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[ui_idVBO]);
			glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(GLfloat), &uvs[0], GL_STATIC_DRAW);

			c_VAO fps;
			fps.t_Shade = SHADER_TEXTURE;
			fps.Name = "FPS";
			fps.ui_idTexture = TEX_ARIALFONT;
			fps.uiVertexCount = 6;
			vVAOs.push_back(fps);
		}

		/* Bind vertex buffers to VAO's
		vertex buffers can hold any information: position, color, uv-coordinates
		VAO's only exist from OpengL >=
		*/
		void Bind_VBOs_to_VAOs() // s. http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html
		{
			GLuint gi;
			GLenum err = glGetError();
			for (unsigned int iVAO = 0; iVAO < vVAOs.size(); iVAO++)
			{
				vVertexArray.push_back(gi);
			}
			glGenVertexArrays(vVAOs.size(), &vVertexArray[0]);

			// ===== 2d-VBO's (FPS shader) =====

			unsigned int iVAO = 0;
			glBindVertexArray(vVertexArray[iVAO]);  // select/bind array and
													// attach a) position and
													//        b) texture/uv-buffers
			glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[iVAO]);
			glVertexAttribPointer(sh2_attr_pos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // wichtig, hier das richtige Attrib (nicht 0 oder 1) zu übergeben!
			glEnableVertexAttribArray(sh2_attr_pos);
			err = glGetError();

			glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[iVAO]); // u,v-texture-coords
			err = glGetError();
			glVertexAttribPointer(sh2_attr_tex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // texAttrib
			err = glGetError(); // Fehler 1281
			glEnableVertexAttribArray(sh2_attr_tex);
			err = glGetError();

			glBindVertexArray(0); // Unbind
			err = glGetError();


			// ===== 3d-VBO's (regular shader, i.e. Scene + Objects) =====

			for (unsigned int iVAO = 1; iVAO < vVAOs.size(); iVAO++) // <-- start with 1, as 0 is for FPS-coords
			{
				glBindVertexArray(vVertexArray[iVAO]);  // select/bind array and
														// attach a)  position and
														//        b1) col or
														//        b2) texture/uv-buffers
														//    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[iVAO]);
														//    glVertexAttribPointer(sh1_attr_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // wichtig, hier das richtige Attrib (nicht 0 oder 1) zu übergeben!
														//    glEnableVertexAttribArray(sh1_attr_pos);

				if (vVAOs[iVAO].t_Shade == SHADER_COLOR_FLAT) // flat (number of elements per Vertex = 3)
				{
					//      glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[iVAO]);
					//      glVertexAttribPointer(sh1_attr_col, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // aeltere OpenGL-Version: glColorPointer
					//      glEnableVertexAttribArray(sh1_attr_col); // s. http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_03
				}
				else if (vVAOs[iVAO].t_Shade == SHADER_TEXTURE) // texture (number of elements per Vertex = 2)
				{
					glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[iVAO]); // u,v-texture-coords
																   //      glVertexAttribPointer(sh1_attr_tex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
					err = glGetError();
					//      glEnableVertexAttribArray(sh1_attr_tex);
				}
				glBindVertexArray(0); // Unbind
			}
		}

		void DrawVAOs_NEU()
		{
			/*
			Achtung, hier springt das Programm nochmal 'rein nach Drücken des Close Buttons,
			dann sind aber die vVAO-Inhalte schon beliebig "dirty"!!
			z.B. ist dann vVAOs[ui].b_moving = TRUE und die if Abfrage (s.u.) wird angesprungen
			*/

			//    char buf[512];
			//    GLenum glErr;
			glm::vec3 v3;

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(program_fps);

			GLenum err = GL_NO_ERROR;

			// draw Scene + Objects
			unsigned int ui_start = 1;
			if (b_splash_screen) ui_start = 0;
			for (unsigned int ui = ui_start; ui < vVAOs.size(); ui++) // start with 1 as 0 is fps-counter
			{

				if (vVAOs[ui].t_Shade == SHADER_TEXTURE)
				{
					//      glUniform1i(sh1_unif_col_tex, 1); // shader into texture-branch

					// http://ogldev.atspace.co.uk/www/tutorial16/tutorial16.html
					glActiveTexture(GL_TEXTURE0);
					err = glGetError();
					// ----------------------
					// Fehler 1282 auf NVidia
					// Ideen: rendern im Wireframe - hilft nicht --> konnte die Textur nicht geladen werden?
					// ----------------------
					glBindTexture(GL_TEXTURE_2D, vGLTexture[vVAOs[ui].ui_idTexture - 1]); // TEXTURE_ID shall be > 0 !     (-1!!)
					err = glGetError();

					if (ui == 0) // hack!!
						glUniform1i(sh2_unif_ID, 0); // hack!!
					else
						//        glUniform1i(sh1_unif_sampler, 0)
						;
				}
				else // vVAOs[ui].t_Shade == SHADER_COLOR_FLAT
				{
					//      glUniform1i(sh1_unif_col_tex, 0); // shader into color-branch
				}
				err = glGetError();

				glBindVertexArray(vVertexArray[ui]); // <--- NVidia: hier Problem, wenn ui = 13 (beim ersten colorierten + texturierten Objekt!!)

													 /*
													 wenn's hier crasht, dann ist der Fehler vermutlich vorher beim buffern passiert und
													 glGetError hätte etwas melden sollen!!
													 */

				glDrawArrays(GL_TRIANGLES, 0, vVAOs[ui].uiVertexCount); // <-- if error is thrown here,
				err = glGetError();                                     //     it can be either positionbuffer, colorbuffer or uvbuffer
																		//     if t_Shade == TEXTURE,
																		//     then colorbuffer is NULL and vice versa!

			} // for ...

			  //    if (b_PNG) FBO_to_PPM();
		}
	private:
		static int const fbo_width = 512;
		static int const fbo_height = 512;
		GLuint fb, color, depth; // <--- nicer names!
	};


	//frogger
	//#define FBUF2D_WIDTH  200
	//#define FBUF2D_HEIGHT 320

	//galaga
#define FBUF2D_WIDTH  64
#define FBUF2D_HEIGHT 64


	class FrameBuf2D // which is actually an animated (OpenGL-)Texture
	{
	public:
		unsigned int width, height;
		unsigned int imagesize;   // = width*height*3
		unsigned int framecounter;

		/*
		(0,0): rgb (1,0): rgb (2,0): rgb
		*/
		void setpixel(unsigned char * data, const unsigned int x, const unsigned int y, const unsigned char r, const unsigned char g, const unsigned char b)
		{
			unsigned int _y = height - 1 - y; // hack, texture upside down? 2do: check!!
											  //    unsigned int _y = y;
			unsigned int pos = 3 * (_y * width + x);
			//    assert(pos < imageSize);
			if (pos >= imagesize) return;
			if (x > width) return; // test < 0 ?? <-- need signed then
			data[pos] = b; // r ?
			data[pos + 1] = g;
			data[pos + 2] = r; // b ?
		}

		GLuint framebuf2D() // purpose: get texture-ID
		{
			// Actual RGB data
			unsigned char * data;

			width = FBUF2D_WIDTH;
			height = FBUF2D_HEIGHT;
			imagesize = width*height * 3;

			// Create a buffer
			data = new unsigned char[imagesize];
			memset(data, 0, imagesize); // clear

										/*
										draw some stuff
										*/
			for (unsigned int i = 1; i < (height - 1); i++)
			{
				char r, g, b;
				r = ((i + 1) % 3) * 255;
				g = (i % 3) * 255;
				b = ((i - 1) % 3) * 255;
				setpixel(data, i, i, r, g, b);
				setpixel(data, width - i, i, r, g, b);
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


#if 0
		// ---
		// FBO
		// ---
		void CHECK_FRAMEBUFFER_STATUS()
		{
			GLenum status;
			status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
			switch (status)
			{
			case GL_FRAMEBUFFER_COMPLETE:
				break;

			case GL_FRAMEBUFFER_UNSUPPORTED: // choose different formats
				break;

			default: // programming error; will fail on all hardware
					 //        fputs("Framebuffer Error\n", stderr);
				exit(-1);
			}
		}

		void proj::Render::init_FBO()
		{
			glGenFramebuffers(1, &fb);
			glGenTextures(1, &color);
			glGenRenderbuffers(1, &depth);

			glBindFramebuffer(GL_FRAMEBUFFER, fb);

			glBindTexture(GL_TEXTURE_2D, color);
			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGBA,
				fbo_width, fbo_height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

			glBindRenderbuffer(GL_RENDERBUFFER, depth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fbo_width, fbo_height);
			glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

			CHECK_FRAMEBUFFER_STATUS();
		}

		//GLubyte pixels[fbo_height][fbo_width][3]; // too big for the stack
		GLubyte puxels[1000][600][3]; // too big for the stack
		void proj::Render::FBO_to_PPM() // write out as binary PPM (with lines in reverse order)
		{
			glBindTexture(GL_TEXTURE_2D, color);//textureId[0]);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, puxels);
			glBindTexture(GL_TEXTURE_2D, 0); // <-?

			std::ofstream myfile;
			std::string filename;
			filename = "d:\\glShoot_FBO.ppm";
			myfile.open(filename.c_str());
			myfile << "P6" << std::endl;
			myfile << std::to_string((_ULonglong)fbo_width) << " " << std::to_string((_ULonglong)fbo_height) << std::endl; // "w h"
																														   //    myfile << "1000" << " " << "600" << std::endl; // "w h"
			myfile << "255" << std::endl;
			myfile.close();
			myfile.open(filename.c_str(), std::ofstream::app | std::ofstream::binary); // binary-"trick" from: http://www.gamedev.net/topic/510801-render-to-memory/

			for (int i = 0; i < fbo_height; i++)
			{
				for (int j = 0; j < fbo_width; j++)
				{
					for (int k = 0; k < 3; k++)
					{
						myfile << puxels[(fbo_height - 1) - i][j][k];
					}
				}
			}
			myfile.close();
		}
#endif
	};
}

#endif // ZWEIDEE_H