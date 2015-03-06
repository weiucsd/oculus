#include "OculusBase.h"

namespace VPL {

	OculusBase::OculusBase() :multi_sampling_(false), distortion_caps_(0 | ovrDistortionCap_Vignette | ovrDistortionCap_Chromatic | ovrDistortionCap_Overdrive)
	{
		std::cout << "[Esc] to quit, dismiss the warning with any key" << std::endl;
	}

	OculusBase::~OculusBase(){}

	void OculusBase::InitOculus()
	{
		// Initialize LibOVR
		ovr_Initialize();

		// Check for attached head mounted display
		hmd_ = ovrHmd_Create(0);
		if (!hmd_)
		{
			std::cout << "No Oculus Rift device attached, using virtual version" << std::endl;
			hmd_ = ovrHmd_CreateDebug(ovrHmd_DK2);
		}
	}

	static void glfwErrorCallback(int error, const char* description)
	{
		(void)error;

		std::cerr << description << std::endl;
		exit(EXIT_FAILURE);
	}

	void OculusBase::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		(void)scancode;
		(void)mods;

		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GL_TRUE);
				break;
			case GLFW_KEY_R:
				ovrHmd_RecenterPose(hmd_);
				break;
			case GLFW_KEY_UP:
				obj_position_.z += 0.1f;
				break;
			case GLFW_KEY_DOWN:
				obj_position_.z -= 0.1f;
				break;
			case GLFW_KEY_LEFT:
				obj_position_.x += 0.1f;
				break;
			case GLFW_KEY_RIGHT:
				obj_position_.x -= 0.1f;
				break;
			}

			// Remove HSW on every key
			ovrHSWDisplayState has_warning_state;
			ovrHmd_GetHSWDisplayState(hmd_, &has_warning_state);
			if (has_warning_state.Displayed) ovrHmd_DismissHSWDisplay(hmd_);
		}
	}

	static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		OculusBase *oculusbase = reinterpret_cast<OculusBase *>(glfwGetWindowUserPointer(window));
		oculusbase->KeyCallback(window, key, scancode, action, mods);
	}

	void OculusBase::WindowSizeCallback(GLFWwindow* window, int w, int h)
	{
		(void)window;

		if (w > 0 && h > 0)
		{
			cfg_.OGL.Header.BackBufferSize.w = w;
			cfg_.OGL.Header.BackBufferSize.h = h;

			if (!ovrHmd_ConfigureRendering(hmd_, &cfg_.Config, distortion_caps_, hmd_->MaxEyeFov, eye_render_desc_))
			{
				std::cout << "Configure failed" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	}

	static void glfwWindowSizeCallback(GLFWwindow* window, int w, int h)
	{
		OculusBase *oculusbase = reinterpret_cast<OculusBase *>(glfwGetWindowUserPointer(window));
		oculusbase->WindowSizeCallback(window, w, h);
	}

	void OculusBase::InitGLFW()
	{
		glfwSetErrorCallback(glfwErrorCallback);

		if (!glfwInit()) exit(EXIT_FAILURE);

		if (multi_sampling_) glfwWindowHint(GLFW_SAMPLES, 4); else glfwWindowHint(GLFW_SAMPLES, 0);

		// Check to see if we are running in "Direct" or "Extended Desktop" mode
		bool directmode = ((hmd_->HmdCaps & ovrHmdCap_ExtendDesktop) == 0);

		GLFWmonitor* monitor;
		ovrSizei clientsize;
		if (directmode)
		{
			std::cout << "Running in Direct mode" << std::endl;
			monitor = NULL;

			clientsize.w = hmd_->Resolution.w;
			clientsize.h = hmd_->Resolution.h;
		}
		else // Extended Desktop mode
		{
			std::cout << "Running in Extended Desktop mode" << std::endl;
			int monitor_count;
			GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
			switch (monitor_count)
			{
			case 0:
				std::cout << "No monitors found, exiting" << std::endl;
				exit(EXIT_FAILURE);
				break;
			case 1:
				std::cout << "Two monitors expected, found only one, using primary" << std::endl;
				monitor = glfwGetPrimaryMonitor();
				break;
			case 2:
				std::cout << "Two monitors found, using second monitor" << std::endl;
				monitor = monitors[1];
				break;
			default:
				std::cout << "More than two monitors found, using second monitor" << std::endl;
				monitor = monitors[1];
			}
			clientsize.w = hmd_->Resolution.w; // 1920 for DK2
			clientsize.h = hmd_->Resolution.h; // 1080 for DK2
		}

		// Create the window based on the parameters set above
		glfw_window_ = glfwCreateWindow(clientsize.w, clientsize.h, "GLFW Oculus Rift Test", monitor, NULL);

		// Check if window creation was succesfull
		if (!glfw_window_)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		// Attach the window in "Direct Mode"
#if defined(_WIN32)
		if (directmode)
		{
			if (!ovrHmd_AttachToWindow(hmd_, glfwGetWin32Window(glfw_window_), NULL, NULL))
			{
				std::cout << "Could not attach to window" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
#endif

		// Make the context current for this window
		glfwMakeContextCurrent(glfw_window_);

		// Don't forget to initialize Glew, turn glewExperimental on to avoid problem fetching function pointers
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cout << "glewInit() error" << std::endl;
			exit(EXIT_FAILURE);
		}

		// Oculus Rift eye configurations
		cfg_.OGL.Header.API = ovrRenderAPI_OpenGL;
		cfg_.OGL.Header.BackBufferSize.w = clientsize.w;
		cfg_.OGL.Header.BackBufferSize.h = clientsize.h;
		cfg_.OGL.Header.Multisample = (multi_sampling_ ? 1 : 0);
#if defined(_WIN32)
		cfg_.OGL.Window = glfwGetWin32Window(glfw_window_);
		cfg_.OGL.DC = GetDC(cfg_.OGL.Window);
#elif defined(__linux__)
		cfg_.OGL.Disp = glfwGetX11Display();
#endif

		glfwSetWindowUserPointer(glfw_window_, reinterpret_cast<void*>(this));

		glfwSetKeyCallback(glfw_window_, glfwKeyCallback);
		glfwSetWindowSizeCallback(glfw_window_, glfwWindowSizeCallback);

		// Start the sensor which provides the Rift’s pose and motion
		uint32_t supported_sensor_caps = ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position;
		uint32_t required_tracking_caps = 0;
		if (!ovrHmd_ConfigureTracking(hmd_, supported_sensor_caps, required_tracking_caps))
		{
			std::cout << "Could not start tracking" << std::endl;
			exit(EXIT_FAILURE);
		}

		// Print some info about the OpenGL context
		int opengl_major = glfwGetWindowAttrib(glfw_window_, GLFW_CONTEXT_VERSION_MAJOR);
		int opengl_minor = glfwGetWindowAttrib(glfw_window_, GLFW_CONTEXT_VERSION_MINOR);
		int opengl_profile = glfwGetWindowAttrib(glfw_window_, GLFW_OPENGL_PROFILE);
		std::cout << "OpenGL: " << opengl_major << "." << opengl_minor << std::endl;
		if (opengl_major >= 3) // Profiles introduced in OpenGL 3.0
		{
			if (opengl_profile == GLFW_OPENGL_COMPAT_PROFILE) std::cout << "GLFW_OPENGL_COMPAT_PROFILE" << std::endl; else std::cout << "GLFW_OPENGL_CORE_PROFILE" << std::endl;
		}
		std::cout << "Vendor: " << (char*)glGetString(GL_VENDOR) << std::endl;
		std::cout << "Renderer: " << (char*)glGetString(GL_RENDERER) << std::endl;
	}

	void OculusBase::InitGL()
	{
		if (multi_sampling_) glEnable(GL_MULTISAMPLE); else glDisable(GL_MULTISAMPLE);

		// Find out what the texture sizes should be for each eye separately first
		ovrSizei eye_texturesizes[2];

		eye_texturesizes[ovrEye_Left] = ovrHmd_GetFovTextureSize(hmd_, ovrEye_Left, hmd_->MaxEyeFov[ovrEye_Left], 1.0f);
		eye_texturesizes[ovrEye_Right] = ovrHmd_GetFovTextureSize(hmd_, ovrEye_Right, hmd_->MaxEyeFov[ovrEye_Right], 1.0f);

		// Combine for one texture for both eyes
		OVR::Sizei render_targetsize;
		render_targetsize.w = eye_texturesizes[ovrEye_Left].w + eye_texturesizes[ovrEye_Right].w;
		render_targetsize.h = (eye_texturesizes[ovrEye_Left].h > eye_texturesizes[ovrEye_Right].h ? eye_texturesizes[ovrEye_Left].h : eye_texturesizes[ovrEye_Right].h);

		// Create the FBO being a single one for both eyes (this is open for debate)
		glGenFramebuffers(1, &fbo_id_);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);

		// The texture we're going to render to
		glGenTextures(1, &textureid_);
		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, textureid_);
		// Give an empty image to OpenGL (the last "0")
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, render_targetsize.w, render_targetsize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		// Linear filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Create Depth Buffer
		glGenRenderbuffers(1, &depthbufferid_);
		glBindRenderbuffer(GL_RENDERBUFFER, depthbufferid_);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, render_targetsize.w, render_targetsize.h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbufferid_);

		// Set the texture as our color attachment #0
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureid_, 0);

		// Set the list of draw buffers
		GLenum gl_drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, gl_drawbuffers); // "1" is the size of DrawBuffers

		// Check if everything is OK
		if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "There is a problem with the FBO" << std::endl;
			exit(EXIT_FAILURE);
		}

		// Unbind
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Setup textures for each eye

		// Left eye
		eye_textures_[ovrEye_Left].Header.API = ovrRenderAPI_OpenGL;
		eye_textures_[ovrEye_Left].Header.TextureSize = render_targetsize;
		eye_textures_[ovrEye_Left].Header.RenderViewport.Pos.x = 0;
		eye_textures_[ovrEye_Left].Header.RenderViewport.Pos.y = 0;
		eye_textures_[ovrEye_Left].Header.RenderViewport.Size = eye_texturesizes[ovrEye_Left];
		((ovrGLTexture&)(eye_textures_[ovrEye_Left])).OGL.TexId = textureid_;

		// Right eye (mostly the same as left but with the viewport on the right side of the texture)
		eye_textures_[ovrEye_Right] = eye_textures_[ovrEye_Left];
		eye_textures_[ovrEye_Right].Header.RenderViewport.Pos.x = (render_targetsize.w + 1) / 2;
		eye_textures_[ovrEye_Right].Header.RenderViewport.Pos.y = 0;

		if (!ovrHmd_ConfigureRendering(hmd_, &cfg_.Config, distortion_caps_, hmd_->MaxEyeFov, eye_render_desc_))
		{
			std::cout << "Configure failed" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	void OculusBase::InitRendering(OculusSampleScene &oculusscene)
	{
		InitOculus();
		InitGLFW();
		InitGL();

		// Projection matrici for each eye will not change at runtime, we can set them here
		//OVR::Matrix4f projection_matrici[2];
		projection_matrici_[ovrEye_Left] = ovrMatrix4f_Projection(eye_render_desc_[ovrEye_Left].Fov, 0.3f, 100.0f, true);
		projection_matrici_[ovrEye_Right] = ovrMatrix4f_Projection(eye_render_desc_[ovrEye_Right].Fov, 0.3f, 100.0f, true);

		// IPD offset values will not change at runtime, we can set them here
		//ovrVector3f eye_offsets[2];
		eye_offsets_[ovrEye_Left] = eye_render_desc_[ovrEye_Left].HmdToEyeViewOffset;
		eye_offsets_[ovrEye_Right] = eye_render_desc_[ovrEye_Right].HmdToEyeViewOffset;

		// Do a single recenter to calibrate orientation to current state of the Rift
		ovrHmd_RecenterPose(hmd_);

		// Initialize the scene
		oculusscene.InitScene(obj_position_.x, obj_position_.y, obj_position_.z);

		// Main loop
		frame_index_ = 0;
	}

	void OculusBase::RenderTerminate()
	{
		// Clean up FBO
		glDeleteRenderbuffers(1, &depthbufferid_);
		glDeleteTextures(1, &textureid_);
		glDeleteFramebuffers(1, &fbo_id_);

		// Clean up Oculus
		ovrHmd_Destroy(hmd_);
		ovr_Shutdown();

		// Clean up window
		glfwDestroyWindow(glfw_window_);
		glfwTerminate();

		exit(EXIT_SUCCESS);
	}

	void OculusBase::RenderFrame(OculusSampleScene &oculusscene)
	{
		// Begin the frame
		ovrHmd_BeginFrame(hmd_, frame_index_);

		// Get eye poses for both the left and the right eye. eye_poses contains all Rift information: orientation, positional tracking and
		// the IPD in the form of the input variable eye_offsets.
		ovrHmd_GetEyePoses(hmd_, frame_index_, eye_offsets_, eye_poses_, NULL);

		// Bind the FBO
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);

		// Clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int eye_index = 0; eye_index < ovrEye_Count; eye_index++)
		{
			ovrEyeType eye_type = hmd_->EyeRenderOrder[eye_index];

			glViewport(
				eye_textures_[eye_type].Header.RenderViewport.Pos.x,
				eye_textures_[eye_type].Header.RenderViewport.Pos.y,
				eye_textures_[eye_type].Header.RenderViewport.Size.w,
				eye_textures_[eye_type].Header.RenderViewport.Size.h
				);

			// Pass projection matrix on to OpenGL
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glMultMatrixf(&(projection_matrici_[eye_type].Transposed().M[0][0]));

			// Create the model-view matrix and pass on to OpenGL
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			// Multiply with orientation retrieved from sensor
			//OVR::Quatf oculus_orientation = OVR::Quatf(eye_poses_[eye_type].Orientation);
			//OVR::Matrix4f modelview_matrix = OVR::Matrix4f(oculus_orientation.Inverted());
			//glMultMatrixf(&(modelview_matrix.Transposed().M[0][0]));

			// Translation due to positional tracking (DK2) and IPD
			//glTranslatef(-eye_poses_[eye_type].Position.x, -eye_poses_[eye_type].Position.y, -eye_poses_[eye_type].Position.z);

			// Move the world forward a bit to show the scene in front of us
			glTranslatef(obj_position_.x, obj_position_.y, obj_position_.z);

			// Draw Scene!
			oculusscene.DrawScene();
		}

		// Back to the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Do everything, distortion, front/back buffer swap
		ovrHmd_EndFrame(hmd_, eye_poses_, eye_textures_);

		++frame_index_;

		glfwPollEvents();
	}

	void OculusBase::UpdateScene(OculusSampleScene &oculusscene, Hands &hands)
	{
		ovrSizei oculus_size;

		oculusscene.hand_position_ = hands.hand_position_;

		//oculusscene.hand_position_.x = oculusscene.hand_position_.x;
		//oculusscene.hand_position_.y = oculusscene.hand_position_.y;
	}

}  // namespace VPL