﻿/************************************************************************************
Filename    :   Win32_GLAppUtil.h
Content     :   OpenGL and Application/Window setup functionality for RoomTiny
Created     :   October 20th, 2014
Author      :   Tom Heath
Copyright   :   Copyright 2014 Oculus, LLC. All Rights reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*************************************************************************************/
#include <Windows.h>
#include <vector>
#include "util_pipeline.h"
#include <GL/CAPI_GLE.h>
#include <Extras/OVR_Math.h>
#include <Kernel/OVR_Log.h>
#include "OVR_CAPI_GL.h"

using namespace OVR;

//---------------------------------------------------------------------------------------
struct DepthBuffer
{
    GLuint        texId;

    DepthBuffer(Sizei size, int sampleCount)
    {
        OVR_ASSERT(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        GLenum internalFormat = GL_DEPTH_COMPONENT24;
        GLenum type = GL_UNSIGNED_INT;
        if (GLE_ARB_depth_buffer_float)
        {
            internalFormat = GL_DEPTH_COMPONENT32F;
            type = GL_FLOAT;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.w, size.h, 0, GL_DEPTH_COMPONENT, type, NULL);
    }
};

//--------------------------------------------------------------------------
struct TextureBuffer
{
    ovrSwapTextureSet* TextureSet;
    GLuint        texId;
    GLuint        fboId;
    Sizei          texSize;

    TextureBuffer(ovrHmd hmd, bool rendertarget, bool displayableOnHmd, OVR::Sizei size, int mipLevels, unsigned char * data, int sampleCount)
    {
        OVR_ASSERT(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

        texSize = size;

        if (displayableOnHmd) {
            // This texture isn't necessarily going to be a rendertarget, but it usually is.
            OVR_ASSERT ( hmd ); // No HMD? A little odd.
            OVR_ASSERT ( sampleCount == 1); // ovrHmd_CreateSwapTextureSetD3D11 doesn't support MSAA.

            ovrHmd_CreateSwapTextureSetGL(hmd, GL_RGBA, size.w, size.h, &TextureSet);
            for (int i = 0; i < TextureSet->TextureCount; ++i)
            {
                ovrGLTexture* tex = (ovrGLTexture*)&TextureSet->Textures[i];
                glBindTexture(GL_TEXTURE_2D, tex->OGL.TexId);

                if ( rendertarget )
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                }
                else
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                }
            }
        }
        else {
            glGenTextures(1, &texId);
            glBindTexture(GL_TEXTURE_2D, texId);

            if ( rendertarget )
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize.w, texSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }

        if (mipLevels > 1)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glGenFramebuffers(1, &fboId);
    }

    Sizei GetSize(void) const
    {
        return texSize;
    }

    void SetAndClearRenderSurface(DepthBuffer * dbuffer)
    {
        ovrGLTexture* tex = (ovrGLTexture*)&TextureSet->Textures[TextureSet->CurrentIndex];

        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->OGL.TexId, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dbuffer->texId, 0);

        glViewport(0, 0, texSize.w, texSize.h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void UnsetRenderSurface()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    }
};

//-------------------------------------------------------------------------------------------
struct OGL
{
    static const bool   UseDebugContext = false;

    HWND                Window;
    HDC                 hDC;
    HGLRC               WglContext;
    OVR::GLEContext     GLEContext;

    GLuint              fboId;

    bool                Running;
    bool                Key[256];

    static LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        OGL *p = (OGL *)GetWindowLongPtr(hWnd, 0);
        switch (Msg)
        {
        case WM_KEYDOWN:
            p->Key[wParam] = true;
            break;
        case WM_KEYUP:
            p->Key[wParam] = false;
            break;
        case WM_DESTROY:
            p->Running = false;
            break;
        default:
            return DefWindowProcW(hWnd, Msg, wParam, lParam); 
        }

        if ((p->Key['Q'] && p->Key[VK_CONTROL]) || p->Key[VK_ESCAPE])
            p->Running = false;

        return 0;
    }

    bool InitWindowAndDevice(HINSTANCE hInst, Recti vp, bool windowed, LPCWSTR title = nullptr)
    {
        Running = true;

        WglContext = 0;

        WNDCLASSW wc;
        memset(&wc, 0, sizeof(wc));
        wc.style = CS_CLASSDC;
        wc.lpfnWndProc = WindowProc;
        wc.cbWndExtra = sizeof(struct OGL *);
        wc.hInstance = GetModuleHandleW(NULL);
        wc.lpszClassName = L"ORT";
        RegisterClassW(&wc);

        const DWORD wsStyle = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME;
        RECT winSize = { 0, 0, vp.w, vp.h };
        AdjustWindowRect(&winSize, wsStyle, FALSE);
        Window = CreateWindowW(L"ORT", (title ? title : L"OculusRoomTiny (GL)"), wsStyle | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, winSize.right - winSize.left, winSize.bottom - winSize.top,
            NULL, NULL, hInst, NULL);
        if (!Window) return(false);
        SetWindowLongPtr(Window, 0, LONG_PTR(this));
        hDC = GetDC(Window);

        PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARBFunc = NULL;
        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARBFunc = NULL;
        {
            // First create a context for the purpose of getting access to wglChoosePixelFormatARB / wglCreateContextAttribsARB.
            PIXELFORMATDESCRIPTOR pfd;
            memset(&pfd, 0, sizeof(pfd));

            pfd.nSize = sizeof(pfd);
            pfd.nVersion = 1;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
            pfd.cColorBits = 32;
            pfd.cDepthBits = 16;

            int pf = ChoosePixelFormat(hDC, &pfd);
            if (!pf)
            {
                ReleaseDC(Window, hDC);
                return false;
            }

            if (!SetPixelFormat(hDC, pf, &pfd))
            {
                ReleaseDC(Window, hDC);
                return false;
            }

            HGLRC context = wglCreateContext(hDC);
            if (!wglMakeCurrent(hDC, context))
            {
                wglDeleteContext(context);
                ReleaseDC(Window, hDC);
                return false;
            }

            wglChoosePixelFormatARBFunc = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
            wglCreateContextAttribsARBFunc = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
            OVR_ASSERT(wglChoosePixelFormatARBFunc && wglCreateContextAttribsARBFunc);

            wglDeleteContext(context);
        }

        // Now create the real context that we will be using.
        int iAttributes[] = {
            // WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 16,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
            0, 0 };

        float fAttributes[] = { 0, 0 };
        int   pf = 0;
        UINT  numFormats = 0;

        if (!wglChoosePixelFormatARBFunc(hDC, iAttributes, fAttributes, 1, &pf, &numFormats))
        {
            ReleaseDC(Window, hDC);
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(pfd));

        if (!SetPixelFormat(hDC, pf, &pfd))
        {
            ReleaseDC(Window, hDC);
            return false;
        }

        GLint attribs[16];
        int   attribCount = 0;

        if (UseDebugContext)
        {
            attribs[attribCount++] = WGL_CONTEXT_FLAGS_ARB;
            attribs[attribCount++] = WGL_CONTEXT_DEBUG_BIT_ARB;
        }

        attribs[attribCount] = 0;

        WglContext = wglCreateContextAttribsARBFunc(hDC, 0, attribs);
        if (!wglMakeCurrent(hDC, WglContext))
        {
            wglDeleteContext(WglContext);
            ReleaseDC(Window, hDC);
            return false;
        }

        OVR::GLEContext::SetCurrentContext(&GLEContext);
        GLEContext.Init();

        ShowWindow(Window, SW_SHOWDEFAULT);

        glGenFramebuffers(1, &fboId);

        glEnable(GL_DEPTH_TEST);
        //glFrontFace(GL_CW);
        //glEnable(GL_CULL_FACE);

        if (UseDebugContext && GLE_ARB_debug_output)
        {
            glDebugMessageCallbackARB(DebugGLCallback, NULL);
            if (glGetError())
            {
                OVR_DEBUG_LOG(("glDebugMessageCallbackARB failed."));
            }

            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

            // Explicitly disable notification severity output.
            glDebugMessageControlARB(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
        }

        return true;
    }

    bool HandleMessages(void)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return Running;
    }

    void ReleaseWindow(HINSTANCE hInst)
    {
        glDeleteFramebuffers(1, &fboId);

        if (WglContext) {
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(WglContext);
        }

        UnregisterClassW( L"ORT", hInst );
    }

    static void GLAPIENTRY DebugGLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
    {
        OVR_DEBUG_LOG(("Message from OpenGL: %s\n", message));
    }

} Platform;


//------------------------------------------------------------------------------
struct ShaderFill
{
    GLuint            program;
    TextureBuffer*    texture;

    ShaderFill(GLuint vertex_shader, GLuint pixel_shader,
        TextureBuffer * arg_texture)
    {
        GLuint vShader = vertex_shader;
        GLuint fShader = pixel_shader;
        texture = arg_texture;

        program = glCreateProgram();
        glAttachShader(program, vShader);
        glAttachShader(program, fShader);

        glLinkProgram(program);
        GLint r;
        glGetProgramiv(program, GL_LINK_STATUS, &r);
        if (!r)
        {
            GLchar msg[1024];
            glGetProgramInfoLog(program, sizeof(msg), 0, msg);
            OVR_DEBUG_LOG(("Linking shaders failed: %s\n", msg));
        }

        glDetachShader(program, vShader);
        glDetachShader(program, fShader);
    }
};

//----------------------------------------------------------------
struct VertexBuffer 
{
    GLuint    buffer;

    VertexBuffer(void* vertices, size_t size) 
    {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }
};

//----------------------------------------------------------------
struct IndexBuffer 
{
    GLuint    buffer;

    IndexBuffer(void* indices, size_t size)
    {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    }
};

//---------------------------------------------------------------------------
struct OculusModel 
{
    struct Color
    { 
        unsigned char R,G,B,A;

        Color(unsigned char r = 0,unsigned char g=0,unsigned char b=0, unsigned char a = 0xff)
            : R(r), G(g), B(b), A(a) 
        { }
    };
    struct Vertex
    { 
        Vector3f  Pos;
        Color     C;
        float     U, V;
    };

    Vector3f     Pos;
    Quatf        Rot;
    Matrix4f     Mat;
    int          numVertices, numIndices;
    Vertex       Vertices[2000]; // Note fixed maximum
    uint16_t     Indices[2000];
    ShaderFill * Fill;
    VertexBuffer * vertexBuffer;
    IndexBuffer * indexBuffer;  

    OculusModel(Vector3f arg_pos, ShaderFill * arg_Fill ) { numVertices=0; numIndices=0; Pos = arg_pos; Fill = arg_Fill; }
   ~OculusModel()                                         { FreeBuffers(); }
    Matrix4f& GetMatrix()                           { Mat = Matrix4f(Rot); Mat = Matrix4f::Translation(Pos) * Mat; return Mat;   }
    void AddVertex(const Vertex& v)                 { Vertices[numVertices++] = v;  }
    void AddIndex(uint16_t a)                       { Indices[numIndices++] = a;   }

    void AllocateBuffers()
    {
        vertexBuffer = new VertexBuffer(&Vertices[0], numVertices * sizeof(Vertex));
        indexBuffer  = new IndexBuffer(&Indices[0], numIndices * 2);
    }

    void FreeBuffers()
    {
        delete[] vertexBuffer;
        delete[] indexBuffer;
    }

    void AddSolidColorBox(float x1, float y1, float z1, float x2, float y2, float z2, Color c)
    {
        Vector3f Vert[][2] =
        {
            Vector3f(x1, y2, z1), Vector3f(z1, x1),  Vector3f(x2, y2, z1), Vector3f(z1, x2),
            Vector3f(x2, y2, z2), Vector3f(z2, x2),  Vector3f(x1, y2, z2), Vector3f(z2, x1),
            Vector3f(x1, y1, z1), Vector3f(z1, x1),  Vector3f(x2, y1, z1), Vector3f(z1, x2),
            Vector3f(x2, y1, z2), Vector3f(z2, x2),  Vector3f(x1, y1, z2), Vector3f(z2, x1),
            Vector3f(x1, y1, z2), Vector3f(z2, y1),  Vector3f(x1, y1, z1), Vector3f(z1, y1),
            Vector3f(x1, y2, z1), Vector3f(z1, y2),  Vector3f(x1, y2, z2), Vector3f(z2, y2),
            Vector3f(x2, y1, z2), Vector3f(z2, y1),  Vector3f(x2, y1, z1), Vector3f(z1, y1),
            Vector3f(x2, y2, z1), Vector3f(z1, y2),  Vector3f(x2, y2, z2), Vector3f(z2, y2),
            Vector3f(x1, y1, z1), Vector3f(x1, y1),  Vector3f(x2, y1, z1), Vector3f(x2, y1),
            Vector3f(x2, y2, z1), Vector3f(x2, y2),  Vector3f(x1, y2, z1), Vector3f(x1, y2),
            Vector3f(x1, y1, z2), Vector3f(x1, y1),  Vector3f(x2, y1, z2), Vector3f(x2, y1),
            Vector3f(x2, y2, z2), Vector3f(x2, y2),  Vector3f(x1, y2, z2), Vector3f(x1, y2)
        };

        uint16_t CubeIndices[] = {0, 1, 3,     3, 1, 2,     5, 4, 6,     6, 4, 7,
            8, 9, 11,    11, 9, 10,   13, 12, 14,  14, 12, 15,
            16, 17, 19,  19, 17, 18,  21, 20, 22,  22, 20, 23 };

        for(int i = 0; i < 36; i++)
            AddIndex(CubeIndices[i] + (uint16_t) numVertices);

        for(int v = 0; v < 24; v++)
        {
            Vertex vvv; vvv.Pos = Vert[v][0];  vvv.U = Vert[v][1].x; vvv.V = Vert[v][1].y;
            float dist1 = (vvv.Pos - Vector3f(-2,4,-2)).Length();
            float dist2 = (vvv.Pos - Vector3f(3,4,-3)).Length();
            float dist3 = (vvv.Pos - Vector3f(-4,3,25)).Length();
            int   bri   = rand() % 160;
            float RRR   = c.R * (bri + 192.0f*(0.65f + 8/dist1 + 1/dist2 + 4/dist3)) / 255.0f;
            float GGG   = c.G * (bri + 192.0f*(0.65f + 8/dist1 + 1/dist2 + 4/dist3)) / 255.0f;
            float BBB   = c.B * (bri + 192.0f*(0.65f + 8/dist1 + 1/dist2 + 4/dist3)) / 255.0f;
            vvv.C.R = RRR > 255 ? 255: (unsigned char) RRR;
            vvv.C.G = GGG > 255 ? 255: (unsigned char) GGG;
            vvv.C.B = BBB > 255 ? 255: (unsigned char) BBB;
            AddVertex(vvv);
        }
    }


// ADDING START
	void AddSphereVertex(float x, float y, float z, float U, float V, Color c)
	{
		Vertex vvv;
		vvv.Pos = Vector3f(x,y,z), vvv.U = U; vvv.V = V;
		float dist1 = (vvv.Pos - Vector3f(-2, 4, -2)).Length();
		float dist2 = (vvv.Pos - Vector3f(3, 4, -3)).Length();
		float dist3 = (vvv.Pos - Vector3f(-4, 3, 25)).Length();
		int   bri = rand() % 160;
		float RRR = c.R * (bri + 192.0f*(0.65f + 8 / dist1 + 1 / dist2 + 4 / dist3)) / 255.0f;
		float GGG = c.G * (bri + 192.0f*(0.65f + 8 / dist1 + 1 / dist2 + 4 / dist3)) / 255.0f;
		float BBB = c.B * (bri + 192.0f*(0.65f + 8 / dist1 + 1 / dist2 + 4 / dist3)) / 255.0f;
		vvv.C.R = RRR > 255 ? 255 : (unsigned char)RRR;
		vvv.C.G = GGG > 255 ? 255 : (unsigned char)GGG;
		vvv.C.B = BBB > 255 ? 255 : (unsigned char)BBB;
		AddVertex(vvv);
	}

	void AddTriangle(uint16_t a, uint16_t b, uint16_t c, uint16_t before)
	{
		AddIndex(a + before);
		AddIndex(b + before);
		AddIndex(c + before);
	}

	void AddSphere(float radius, int sides, Color c)
	{
		uint16_t usides = (uint16_t)sides;
		uint16_t halfsides = usides / 2;
		uint16_t numVertices_before_sphere = numVertices;
		
		for (uint16_t k = 0; k < halfsides; k++) {
			
			float z = cosf(MATH_FLOAT_PI * k / float(halfsides));
			float z_r = sinf(MATH_FLOAT_PI * k / float(halfsides)); // the radius of the cross circle with coordinate z

			if (k == 0)
			{       // add north and south poles
				//AddIndex((uint16_t)numVertices);
				AddSphereVertex(0.0f, 0.0f, radius, 0.0f, 0.0f, c);
				//AddIndex((uint16_t)numVertices);
				AddSphereVertex(0.0f, 0.0f, -radius, 1.0f, 1.0f, c);
				//AddIndex((uint16_t)numVertices);
				//AddSphereVertex(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, c);
			}
			else
			{
				for (uint16_t i = 0; i < sides; i++)
				{
					float x = cosf(MATH_FLOAT_TWOPI * i / float(sides)) * z_r;
					float y = sinf(MATH_FLOAT_TWOPI * i / float(sides)) * z_r;

					uint16_t j = 0;
					if (i < sides - 1)
					{
						j = i + 1;
					}

					AddSphereVertex(radius * x, radius * y, radius * z, 0.0f, 1.0f, c);
					
					uint16_t indi = 2 + (k - 1)*usides + i;
					uint16_t indj = 2 + (k - 1)*usides + j;
					if (k == 1) // NorthPole
						AddTriangle(0, j + 2, i + 2, numVertices_before_sphere);
					else if (k == halfsides - 1)  //SouthPole
					{
						AddTriangle(1, indi, indj, numVertices_before_sphere);
						AddTriangle(indi, indi - usides, indj, numVertices_before_sphere);
						AddTriangle(indi - usides, indj - usides, indj, numVertices_before_sphere);
					}
					else
					{
						AddTriangle(indi, indi - usides, indj, numVertices_before_sphere);
						AddTriangle(indi - usides, indj - usides, indj, numVertices_before_sphere);
					}
				}
			} // end else
		}
	}

	void AddCylinder(float height, float radius, int sides, Color c)
	{
		uint16_t numVertices_before_cylinder = numVertices;
		float halfht = height * 0.5f;
		for (uint16_t i = 0; i < sides; i++)
		{
			float x = cosf(MATH_FLOAT_TWOPI * i / float(sides));
			float y = sinf(MATH_FLOAT_TWOPI * i / float(sides));

			//cyl->AddVertex(radius * x, radius * y, halfht, color, x + 1, y, 0, 0, 1);
			AddSphereVertex(radius * x, radius * y, halfht, x + 1, y, c);
			//cyl->AddVertex(radius * x, radius * y, -1.0f*halfht, color, x, y, 0, 0, -1);
			AddSphereVertex(radius * x, radius * y, -1.0f*halfht, x, y, c);

			uint16_t j = 0;
			if (i < sides - 1)
			{
				j = i + 1;
				AddTriangle(0, i * 4 + 4, i * 4, numVertices_before_cylinder);
				AddTriangle(1, i * 4 + 1, i * 4 + 5, numVertices_before_cylinder);
			}

			float nx = cosf(MATH_FLOAT_PI * (0.5f + 2.0f * i / float(sides)));
			float ny = sinf(MATH_FLOAT_PI * (0.5f + 2.0f * i / float(sides)));
			//cyl->AddVertex(radius * x, radius * y, halfht, color, x + 1, y, nx, ny, 0);
			AddSphereVertex(radius * x, radius * y, halfht, x + 1, y, c);
			//cyl->AddVertex(radius * x, radius * y, -1.0f*halfht, color, x, y, nx, ny, 0);
			AddSphereVertex(radius * x, radius * y, -1.0f*halfht, x, y, c);

			AddTriangle(i * 4 + 2, j * 4 + 2, i * 4 + 3, numVertices_before_cylinder);
			AddTriangle(i * 4 + 3, j * 4 + 2, j * 4 + 3, numVertices_before_cylinder);
		}
	}
// ADDING END


    void Render(Matrix4f view, Matrix4f proj)
    {
        Matrix4f combined = proj * view * GetMatrix();

        glUseProgram(Fill->program);
        glUniform1i(glGetUniformLocation(Fill->program, "Texture0"), 0);
        glUniformMatrix4fv(glGetUniformLocation(Fill->program, "matWVP"), 1, GL_TRUE, (FLOAT*)&combined);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Fill->texture->texId);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->buffer);

        GLuint posLoc   = glGetAttribLocation(Fill->program, "Position");
        GLuint colorLoc = glGetAttribLocation(Fill->program, "Color");
        GLuint uvLoc    = glGetAttribLocation(Fill->program, "TexCoord");

        glEnableVertexAttribArray(posLoc);
        glEnableVertexAttribArray(colorLoc);
        glEnableVertexAttribArray(uvLoc);

        glVertexAttribPointer(posLoc,   3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, Pos));
        glVertexAttribPointer(colorLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, C));
        glVertexAttribPointer(uvLoc,    2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, U));

        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, NULL);

        glDisableVertexAttribArray(posLoc);
        glDisableVertexAttribArray(colorLoc);
        glDisableVertexAttribArray(uvLoc);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glUseProgram(0);
    }
};

//------------------------------------------------------------------------- 
struct Scene  
{
    int     num_models;
    OculusModel * Models[20];

    void    Add(OculusModel * n)
    {   Models[num_models++] = n; }    

	Vector3f get_Normal(Vector3f p1, Vector3f p2, Vector3f p3)
	{
		float a = ((p2.y - p1.y)*(p3.z - p1.z) - (p2.z - p1.z)*(p3.y - p1.y));
		float b = ((p2.z - p1.z)*(p3.x - p1.x) - (p2.x - p1.x)*(p3.z - p1.z));
		float c = ((p2.x - p1.x)*(p3.y - p1.y) - (p2.y - p1.y)*(p3.x - p1.x));
		return Vector3f(a, b, c);
	}

    void Render(Matrix4f view, Matrix4f proj)
    {
        for (int i = 0; i < num_models; i++) {
            Models[i]->Render(view, proj);
        }
    }

    GLuint CreateShader(GLenum type, const GLchar* src)
    {
        GLuint shader = glCreateShader(type);

        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);

        GLint r;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
        if (!r)
        {
            GLchar msg[1024];
            glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
            if (msg[0]) {
                OVR_DEBUG_LOG(("Compiling shader failed: %s\n", msg));
            }
            return 0;
        }

        return shader;
    }

    Scene(int reducedVersion) : num_models(0) // Main world
    {
        static const GLchar* VertexShaderSrc =
            "#version 150\n"
            "uniform mat4 matWVP;\n"
            "in      vec4 Position;\n"
            "in      vec4 Color;\n"
            "in      vec2 TexCoord;\n"
            "out     vec2 oTexCoord;\n"
            "out     vec4 oColor;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = (matWVP * Position);\n"
            "   oTexCoord   = TexCoord;\n"
            "   oColor      = Color;\n"
            "}\n";

        static const char* FragmentShaderSrc =
            "#version 150\n"
            "uniform sampler2D Texture0;\n"
            "in      vec4      oColor;\n"
            "in      vec2      oTexCoord;\n"
            "out     vec4      FragColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = oColor * texture2D(Texture0, oTexCoord);\n"
            "}\n";

        GLuint    vshader = CreateShader(GL_VERTEX_SHADER, VertexShaderSrc);
        GLuint    fshader = CreateShader(GL_FRAGMENT_SHADER, FragmentShaderSrc);

        // Make textures
        ShaderFill * grid_material[4];
        for (int k=0;k<4;k++)
        {
             static DWORD tex_pixels[256*256];
             for (int j=0;j<256;j++)
             for (int i=0;i<256;i++)
                {
					//// 0000ff is red, 00ff00 is green, ff0000 is blue. The format may be BGR.
					//// 0xffb4b4b4 is light grey, and 0xff505050 is dark grey.
                    if (k==0) tex_pixels[j*256+i] = (((i >> 7) ^ (j >> 7)) & 1) ? 0xffb4b4b4 : 0xff505050;// floor
                    if (k==1) tex_pixels[j*256+i] = (((j/4 & 15) == 0) || (((i/4 & 15) == 0) && ((((i/4 & 31) == 0) ^ ((j/4 >> 4) & 1)) == 0)))
                        ? 0xff3c3c3c : 0xffb4b4b4;// wall
                    if (k==2) tex_pixels[j*256+i] = (i/4 == 0 || j/4 == 0)      ? 0xff505050 : 0xffb4b4b4;// ceiling
                    if (k==3) tex_pixels[j*256+i] = 0xff808080;// blank
             }
             TextureBuffer * generated_texture = new TextureBuffer(nullptr, false, false, Sizei(256,256),4,(unsigned char *)tex_pixels, 1);
             grid_material[k] = new ShaderFill(vshader,fshader,generated_texture);
        }

        glDeleteShader(vshader);
        glDeleteShader(fshader);

//0		// Construct geometry
		OculusModel * m = new OculusModel(Vector3f(0, 0, 0), grid_material[2]);  // Moving box
		m->AddSolidColorBox(0, 0, 0, +1.0f, +1.0f, 1.0f, OculusModel::Color(64, 64, 64));
		m->AllocateBuffers(); Add(m);

//1		//// test box
		m = new OculusModel(Vector3f(2.0f, 1.0f, 0.1f), grid_material[2]);  //// My box~
		m->AddSolidColorBox(0.0, 0.0, 0.0, +1.0f, +1.0f, 1.0f, OculusModel::Color(128, 0, 0));
		m->AllocateBuffers(); Add(m);

//2-47	//// We will draw a hand in the following.
		//// Each finger is composed by 6 spheres. The palm is composed by 4x4 spheres.
		Vector3f hand_pos = Vector3f(0, 1, 0.1f);
		//// thumb:
		Vector3f thumb_pos[6] =
		{ Vector3f(0, 0.0f, 0), Vector3f(-0.05f, 0.1f, 0), Vector3f(-0.1f, 0.2f, 0), Vector3f(-0.15f, 0.3f, 0), Vector3f(-0.2f, 0.4f, 0), Vector3f(-0.25f, 0.5f, 0) };
		float thumb_size[6] = { 0.8f, 0.8f, 0.9f, 0.7f, 0.8f, 0.7f };
		for (int sphere_num = 0; sphere_num < 6; sphere_num++)
		{
			m = new OculusModel(hand_pos + thumb_pos[sphere_num], grid_material[3]);  //// My Sphere~
			m->AddSphere(+0.1f*thumb_size[sphere_num], 10, OculusModel::Color(150, 80, 150));
			m->AllocateBuffers(); Add(m);
		}

		//// index:
		Vector3f index_pos[6] =
		{ Vector3f(0.2f, 0.5f, 0), Vector3f(0.2f, 0.6f, 0), Vector3f(0.2f, 0.7f, 0), Vector3f(0.2f, 0.8f, 0), Vector3f(0.2f, 0.9f, 0), Vector3f(0.2f, 1.0f, 0) };
		float index_size[6] = { 0.8f, 0.8f, 0.9f, 0.7f, 0.8f, 0.7f };
		for (int sphere_num = 0; sphere_num < 6; sphere_num++)
		{
			m = new OculusModel(hand_pos + index_pos[sphere_num], grid_material[3]);  //// My Sphere~
			m->AddSphere(+0.1f*index_size[sphere_num], 10, OculusModel::Color(128, 0, 0));
			m->AllocateBuffers(); Add(m);
		}

		//// middle:
		Vector3f middle_pos[6] =
		{ Vector3f(0.4f, 0.5f, 0), Vector3f(0.4f, 0.6f, 0), Vector3f(0.4f, 0.7f, 0), Vector3f(0.4f, 0.8f, 0), Vector3f(0.4f, 0.9f, 0), Vector3f(0.4f, 1.0f, 0) };
		float middle_size[6] = { 0.8f, 0.8f, 0.9f, 0.7f, 0.8f, 0.7f };
		for (int sphere_num = 0; sphere_num < 6; sphere_num++)
		{
			m = new OculusModel(hand_pos + middle_pos[sphere_num], grid_material[3]);  //// My Sphere~
			m->AddSphere(+0.1f*middle_size[sphere_num], 10, OculusModel::Color(0, 128, 0));
			m->AllocateBuffers(); Add(m);
		}

		//// ring:
		Vector3f ring_pos[6] =
		{ Vector3f(0.6f, 0.5f, 0), Vector3f(0.6f, 0.6f, 0), Vector3f(0.6f, 0.7f, 0), Vector3f(0.6f, 0.8f, 0), Vector3f(0.6f, 0.9f, 0), Vector3f(0.6f, 1.0f, 0) };
		float ring_size[6] = { 0.8f, 0.8f, 0.9f, 0.7f, 0.8f, 0.7f };
		for (int sphere_num = 0; sphere_num < 6; sphere_num++)
		{
			m = new OculusModel(hand_pos + ring_pos[sphere_num], grid_material[3]);  //// My Sphere~
			m->AddSphere(+0.1f*ring_size[sphere_num], 10, OculusModel::Color(0, 0, 128));
			m->AllocateBuffers(); Add(m);
		}

		//// little:
		Vector3f little_pos[6] =
		{ Vector3f(0.8f, 0.5f, 0), Vector3f(0.8f, 0.6f, 0), Vector3f(0.8f, 0.7f, 0), Vector3f(0.8f, 0.8f, 0), Vector3f(0.8f, 0.9f, 0), Vector3f(0.8f, 1.0f, 0) };
		float little_size[6] = { 0.8f, 0.8f, 0.9f, 0.7f, 0.8f, 0.7f };
		for (int sphere_num = 0; sphere_num < 6; sphere_num++)
		{
			m = new OculusModel(hand_pos + little_pos[sphere_num], grid_material[3]);  //// My Sphere~
			m->AddSphere(+0.1f*little_size[sphere_num], 10, OculusModel::Color(128, 128, 0));
			m->AllocateBuffers(); Add(m);
		}

		//// palm:
		Vector3f palm_pos[16] =
		{ Vector3f(0.2f, -0.1f, 0), Vector3f(0.2f, 0.05f, 0), Vector3f(0.2f, 0.2f, 0), Vector3f(0.2f, 0.35f, 0),
		Vector3f(0.4f, -0.1f, 0), Vector3f(0.4f, 0.05f, 0), Vector3f(0.4f, 0.2f, 0), Vector3f(0.4f, 0.35f, 0),
		Vector3f(0.6f, -0.1f, 0), Vector3f(0.6f, 0.05f, 0), Vector3f(0.6f, 0.2f, 0), Vector3f(0.6f, 0.35f, 0),
		Vector3f(0.8f, -0.1f, 0), Vector3f(0.8f, 0.05f, 0), Vector3f(0.8f, 0.2f, 0), Vector3f(0.8f, 0.35f, 0)
		};
		float palm_size[16] = { 1.2f, 0.8f, 0.9f, 0.9f, 1.2f, 1.1f, 1.0f, 1.0f,
			1.2f, 1.1f, 1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.7f };
		for (int sphere_num = 0; sphere_num < 16; sphere_num++)
		{
			m = new OculusModel(hand_pos + palm_pos[sphere_num], grid_material[3]);  //// My Sphere~
			m->AddSphere(+0.1f*palm_size[sphere_num], 10, OculusModel::Color(20, 20, 20));
			m->AllocateBuffers(); Add(m);
		}

		//Vector3f Cylinder_bottom = Vector3f(-3, 0, 0);
		//Vector3f Cylinder_top = Vector3f(-3, 1.5f, 1.1f);
		//float Cylinder_length = Cylinder_top.Distance(Cylinder_bottom);
		//m = new OculusModel((Cylinder_bottom + Cylinder_top) / 2, grid_material[3]);  //// My Cylinder~
		//m->AddCylinder(Cylinder_length, 0.1f, 10, OculusModel::Color(147, 74, 0));
		//m->Rot = Quatf(get_Normal(Cylinder_bottom, Cylinder_bottom + Vector3f(0, 0, 1.0f), Cylinder_top), (Cylinder_top - Cylinder_bottom).Angle(Vector3f(0, 0, Cylinder_length)));
		//m->AllocateBuffers(); Add(m);

//48-53	//// We will draw a new hand in the following. It is composed by 5 cylinders and a sphere.
		Vector3f cylinder_peak[6] =
		{ Vector3f(-3, 1, 0), Vector3f(-2.3f, 1.1f, 0.4f), Vector3f(-2.4f, 1.8f, 0.65f), Vector3f(-2.7f, 2.0f, 0.8f), Vector3f(-3, 1.9f, 0.7f), Vector3f(-3.3f, 1.6f, 0.6f)
		};
		//// Draw 5 cylinders
		for (int cylinder_num = 1; cylinder_num < 6; cylinder_num++)
		{
			float Cylinder_length = cylinder_peak[0].Distance(cylinder_peak[cylinder_num]);
			m = new OculusModel((cylinder_peak[0] + cylinder_peak[cylinder_num]) / 2, grid_material[3]);  //// My Cylinder~
			m->AddCylinder(Cylinder_length, 0.1f, 10, OculusModel::Color(147, 74, 0));
			m->Rot = Quatf(get_Normal(cylinder_peak[0], cylinder_peak[0] + Vector3f(0, 0, 1.0f), cylinder_peak[cylinder_num]), (cylinder_peak[cylinder_num] - cylinder_peak[0]).Angle(Vector3f(0, 0, Cylinder_length)));
			m->AllocateBuffers(); Add(m);
		}
		//// Draw a sphere
		m = new OculusModel(cylinder_peak[0], grid_material[3]);  //// My Sphere~
		m->AddSphere(+0.3f, 15, OculusModel::Color(70, 35, 0));
		m->AllocateBuffers(); Add(m);

//54	//// Create a button bottom
		m = new OculusModel(Vector3f(-1, 2, 2), grid_material[3]);  //// My Cylinder 1~
		m->AddCylinder(0.2f, 0.5f, 20, OculusModel::Color(0, 0, 128));
		m->AllocateBuffers(); Add(m);
//55	//// Create a button top
		m = new OculusModel(Vector3f(-1, 2, 1.8f), grid_material[3]);  //// My Cylinder 2~
		m->AddCylinder(0.2f, 0.25f, 15, OculusModel::Color(128, 0, 0));
		m->AllocateBuffers(); Add(m);
// ADDING END


        m = new OculusModel(Vector3f(0,0,0),grid_material[1]);  // Walls
        m->AddSolidColorBox( -10.1f,   0.0f,  -20.0f, -10.0f,  4.0f,  20.0f, OculusModel::Color(128,128,128)); // Left Wall
        m->AddSolidColorBox( -10.0f,  -0.1f,  -20.1f,  10.0f,  4.0f, -20.0f, OculusModel::Color(128,128,128)); // Back Wall
        m->AddSolidColorBox(  10.0f,  -0.1f,  -20.0f,  10.1f,  4.0f,  20.0f, OculusModel::Color(128,128,128));  // Right Wall
        m->AllocateBuffers(); Add(m);

        m = new OculusModel(Vector3f(0,0,0),grid_material[0]);  // Floors
        m->AddSolidColorBox( -10.0f,  -0.1f,  -20.0f,  10.0f,  0.0f, 20.1f,  OculusModel::Color(128,128,128)); // Main floor
        m->AddSolidColorBox( -15.0f,  -6.1f,   18.0f,  15.0f, -6.0f, 30.0f,  OculusModel::Color(128,128,128));// Bottom floor
        m->AllocateBuffers(); Add(m);

        if (reducedVersion) return;

        m = new OculusModel(Vector3f(0,0,0),grid_material[2]);  // Ceiling
        m->AddSolidColorBox( -10.0f,  4.0f,  -20.0f,  10.0f,  4.1f, 20.1f,  OculusModel::Color(128,128,128)); 
        m->AllocateBuffers(); Add(m);

        m = new OculusModel(Vector3f(0,0,0),grid_material[3]);  // Fixtures & furniture
        m->AddSolidColorBox(   9.5f,   0.75f,  3.0f,  10.1f,  2.5f,   3.1f,  OculusModel::Color(96,96,96) );   // Right side shelf// Verticals
        m->AddSolidColorBox(   9.5f,   0.95f,  3.7f,  10.1f,  2.75f,  3.8f,  OculusModel::Color(96,96,96) );   // Right side shelf
        m->AddSolidColorBox(   9.55f,  1.20f,  2.5f,  10.1f,  1.30f,  3.75f,  OculusModel::Color(96,96,96) ); // Right side shelf// Horizontals
        m->AddSolidColorBox(   9.55f,  2.00f,  3.05f,  10.1f,  2.10f,  4.2f,  OculusModel::Color(96,96,96) ); // Right side shelf
        m->AddSolidColorBox(   5.0f,   1.1f,   20.0f,  10.0f,  1.2f,  20.1f, OculusModel::Color(96,96,96) );   // Right railing   
        m->AddSolidColorBox(  -10.0f,  1.1f, 20.0f,   -5.0f,   1.2f, 20.1f, OculusModel::Color(96,96,96) );   // Left railing  
        for (float f=5.0f;f<=9.0f;f+=1.0f)
        {
            m->AddSolidColorBox(   f,   0.0f,   20.0f,   f+0.1f,  1.1f,  20.1f, OculusModel::Color(128,128,128) );// Left Bars
            m->AddSolidColorBox(  -f,   1.1f,   20.0f,  -f-0.1f,  0.0f,  20.1f, OculusModel::Color(128,128,128) );// Right Bars
        }
        m->AddSolidColorBox( -1.8f, 0.8f, 1.0f,   0.0f,  0.7f,  0.0f,   OculusModel::Color(128,128,0)); // Table
        m->AddSolidColorBox( -1.8f, 0.0f, 0.0f,  -1.7f,  0.7f,  0.1f,   OculusModel::Color(128,128,0)); // Table Leg 
        m->AddSolidColorBox( -1.8f, 0.7f, 1.0f,  -1.7f,  0.0f,  0.9f,   OculusModel::Color(128,128,0)); // Table Leg 
        m->AddSolidColorBox(  0.0f, 0.0f, 1.0f,  -0.1f,  0.7f,  0.9f,   OculusModel::Color(128,128,0)); // Table Leg 
        m->AddSolidColorBox(  0.0f, 0.7f, 0.0f,  -0.1f,  0.0f,  0.1f,   OculusModel::Color(128,128,0)); // Table Leg 
        m->AddSolidColorBox( -1.4f, 0.5f, -1.1f, -0.8f,  0.55f, -0.5f,  OculusModel::Color(44,44,128) ); // Chair Set
        m->AddSolidColorBox( -1.4f, 0.0f, -1.1f, -1.34f, 1.0f,  -1.04f, OculusModel::Color(44,44,128) ); // Chair Leg 1
        m->AddSolidColorBox( -1.4f, 0.5f, -0.5f, -1.34f, 0.0f,  -0.56f, OculusModel::Color(44,44,128) ); // Chair Leg 2
        m->AddSolidColorBox( -0.8f, 0.0f, -0.5f, -0.86f, 0.5f,  -0.56f, OculusModel::Color(44,44,128) ); // Chair Leg 2
        m->AddSolidColorBox( -0.8f, 1.0f, -1.1f, -0.86f, 0.0f,  -1.04f, OculusModel::Color(44,44,128) ); // Chair Leg 2
        m->AddSolidColorBox( -1.4f, 0.97f,-1.05f,-0.8f,  0.92f, -1.10f, OculusModel::Color(44,44,128) ); // Chair Back high bar

        for (float f=3.0f;f<=6.6f;f+=0.4f)
            m->AddSolidColorBox( -3,  0.0f, f,   -2.9f, 1.3f, f+0.1f, OculusModel::Color(64,64,64) );// Posts

        m->AllocateBuffers(); Add(m);
    }

   ~Scene()
    {
        for (int i = 0; i < num_models; i++)
            delete Models[i];
    }
};
