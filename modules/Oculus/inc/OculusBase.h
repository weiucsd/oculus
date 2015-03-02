#ifndef VPL_OCULUS_OCULUSBASE_H
#define VPL_OCULUS_OCULUSBASE_H

#include <GL/glew.h>
#if defined(_WIN32)
   #pragma comment(lib,"Ws2_32.lib")
   #include <Windows.h>
   #define GLFW_EXPOSE_NATIVE_WIN32
   #define GLFW_EXPOSE_NATIVE_WGL
#elif defined(__linux__)
   #include <X11/X.h>
   #include <X11/extensions/Xrandr.h>
   #define GLFW_EXPOSE_NATIVE_X11
   #define GLFW_EXPOSE_NATIVE_GLX
#endif
#include <GLFW/glfw3.h>
#if !defined(__APPLE__)
   #include <GLFW/glfw3native.h>
#endif
#include <OVR.h>
#include <../Src/OVR_CAPI.h>
#include <../Src/OVR_CAPI_GL.h>

#include <iostream>

#include "OculusScene.h"
#include "OculusSampleScene.h"
#include "hands.h"

namespace VPL {

class OculusBase
{ 
  public:  
    OculusBase();  
    ~OculusBase();

    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void WindowSizeCallback(GLFWwindow* window, int width, int height);

	void InitRendering(OculusSampleScene &oculusscene);
	void RenderFrame(OculusSampleScene &oculusscene);
	void RenderTerminate();

	void Run(OculusSampleScene &oculusscene);

	void UpdateScene(OculusSampleScene &oculusscene, Hands &hands);

  private:
    void InitOculus();
    void InitGLFW();
    void InitGL();

    // Oculus variable
    ovrHmd hmd_;
    ovrGLConfig cfg_;
    ovrEyeRenderDesc eye_render_desc_[2];
    ovrTexture eye_textures_[2];
	ovrVector3f camera_position_;

	//Rendering variables
	OVR::Matrix4f projection_matrici_[2];
	ovrVector3f eye_offsets_[2];
	unsigned int frame_index_;
	ovrPosef eye_poses_[2];

    // glfw variable
    GLFWwindow* glfw_window_;

    // opengl variable
    GLuint fbo_id_;
    GLuint textureid_;
    GLuint depthbufferid_;

    bool multi_sampling_;
    int distortion_caps_; 
};

}  // namespace VPL

#endif