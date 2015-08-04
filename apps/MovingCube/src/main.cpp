/*****************************************************************************

Filename    :   main.cpp
Content     :   Simple minimal VR demo
Created     :   December 1, 2014
Author      :   Tom Heath
Copyright   :   Copyright 2012 Oculus, Inc. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

/*****************************************************************************/
/// This sample has not yet been fully assimiliated into the framework
/// and also the GL support is not quite fully there yet, hence the VR
/// is not that great!

#include "Win32_GLAppUtil.h"
#include <Kernel/OVR_System.h>

// Include the Oculus SDK
#include "OVR_CAPI_GL.h"

using namespace OVR;

std::vector<Vector3f> DisplayGeoNode(PXCGesture *gesture) {
	std::vector<Vector3f> pos_six_point;
	PXCGesture::GeoNode nodes[2][11] = { 0 };
	gesture->QueryNodeData(0, PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY, 10, nodes[0]);
	gesture->QueryNodeData(0, PXCGesture::GeoNode::LABEL_BODY_HAND_SECONDARY, 10, nodes[1]);
	gesture->QueryNodeData(0, PXCGesture::GeoNode::LABEL_BODY_ELBOW_PRIMARY, &nodes[0][10]);
	gesture->QueryNodeData(0, PXCGesture::GeoNode::LABEL_BODY_ELBOW_SECONDARY, &nodes[1][10]);

	for (int i = 0; i < 6; i++)
	{
		Vector3f pos_one_point;
		pos_one_point.x = 4 * nodes[0][i].positionWorld.x;
		pos_one_point.z = -3 * nodes[0][i].positionWorld.y;
		pos_one_point.y = 6 * nodes[0][i].positionWorld.z;
		pos_six_point.push_back(pos_one_point);
	}

	//cout << x0 << " " << y0 << " " << z0 << endl;
	return pos_six_point;
}

static void DisplayPicture(PXCImage *depth, PXCGesture *gesture) {
	PXCImage *image = depth;
	bool dispose = false;

	if (gesture->QueryBlobImage(PXCGesture::Blob::LABEL_SCENE, 0, &image)<PXC_STATUS_NO_ERROR) return;
	dispose = true;

	if (dispose) image->Release();
}

//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	////Loading camera
	UtilPipeline pp;
	//pp->QueryCapture()->SetFilter(PXCCapture::Device::PROPERTY_DEPTH_SMOOTHING, true);

	//pxcCHAR * char_name = L"Hand/Finger Tracking and Gesture Recognition";
	pp.EnableGesture(L"Hand/Finger Tracking and Gesture Recognition");

	pp.EnableImage(PXCImage::COLOR_FORMAT_DEPTH);
	pp.Init();


    OVR::System::Init();

     // Initialise rift
    if (ovr_Initialize(nullptr) != ovrSuccess) { MessageBoxA(NULL, "Unable to initialize libOVR.", "", MB_OK); return 0; }
    ovrHmd HMD;
	//// Detect whether the device is connected
    ovrResult result = ovrHmd_Create(0, &HMD);
    if (result != ovrSuccess)
    {
		//// If the device is not connected, using windows to show
        result = ovrHmd_CreateDebug(ovrHmd_DK2, &HMD);
    }
 
    if (result != ovrSuccess) {    MessageBoxA(NULL,"Oculus Rift not detected.","", MB_OK); ovr_Shutdown(); return 0; }
    if (HMD->ProductName[0] == '\0') MessageBoxA(NULL,"Rift detected, display not enabled.","", MB_OK);

    // Setup Window and Graphics
    // Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution
    ovrSizei windowSize = { HMD->Resolution.w / 2, HMD->Resolution.h / 2 };
    if (!Platform.InitWindowAndDevice(hinst, Recti(Vector2i(0), windowSize), true, L"Oculus Room Tiny (GL)"))
        return 0;

    // Make eye render buffers
    TextureBuffer * eyeRenderTexture[2];
    DepthBuffer   * eyeDepthBuffer[2];
    for (int i=0; i<2; i++)
    {
        ovrSizei idealTextureSize = ovrHmd_GetFovTextureSize(HMD, (ovrEyeType)i, HMD->DefaultEyeFov[i], 1);
        eyeRenderTexture[i] = new TextureBuffer(HMD, true, true, idealTextureSize, 1, NULL, 1);
        eyeDepthBuffer[i]   = new DepthBuffer(eyeRenderTexture[i]->GetSize(), 0);
    }

    // Create mirror texture and an FBO used to copy mirror texture to back buffer
    ovrGLTexture* mirrorTexture;
    ovrHmd_CreateMirrorTextureGL(HMD, GL_RGBA, windowSize.w, windowSize.h, (ovrTexture**)&mirrorTexture);
    // Configure the mirror read buffer
    GLuint mirrorFBO = 0;
    glGenFramebuffers(1, &mirrorFBO);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture->OGL.TexId, 0);
    glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    ovrEyeRenderDesc EyeRenderDesc[2]; 
    EyeRenderDesc[0] = ovrHmd_GetRenderDesc(HMD, ovrEye_Left, HMD->DefaultEyeFov[0]);
    EyeRenderDesc[1] = ovrHmd_GetRenderDesc(HMD, ovrEye_Right, HMD->DefaultEyeFov[1]);

    ovrHmd_SetEnabledCaps(HMD, ovrHmdCap_LowPersistence|ovrHmdCap_DynamicPrediction);

    // Start the sensor
    ovrHmd_ConfigureTracking(HMD, ovrTrackingCap_Orientation|ovrTrackingCap_MagYawCorrection|
                                  ovrTrackingCap_Position, 0);

    // Turn off vsync to let the compositor do its magic
    wglSwapIntervalEXT(0);

    // Make scene - can simplify further if needed
    Scene roomScene(false); 

    bool isVisible = true;

	//// Given parameters:
	Vector3f finger_pos[7] =
	{ Vector3f(0, 0, 0), Vector3f(0, 0.5f, 0), Vector3f(0.1f, 0.5f, 0), Vector3f(0.2f, 0.5f, 0), Vector3f(0, 0.6f, 0), Vector3f(0.1f, 0.6f, 0), Vector3f(0.2f, 0.6f, 0) };
	int finger_size[7] = { 3, 1, 1, 1, 1, 1, 1 };
	int flag_hand_to_cube = 0;//// flag = 0 when hand is far from cube, = 1 when near

    // Main loop
    while (Platform.HandleMessages())
    {
		std::vector<Vector3f> pos_six_point;
		// Get the hand position
		if (pp.AcquireFrame(true)) {
			PXCGesture *gesture = pp.QueryGesture();
			PXCImage *depth_image = pp.QueryImage(PXCImage::IMAGE_TYPE_DEPTH);

			////DisplayPicture
			DisplayPicture(depth_image, gesture);

			////DisplayGeoNode
			pos_six_point = DisplayGeoNode(gesture);

			pp.ReleaseFrame();
			//pp.Close();
			//pp.Release();
		}
        // Keyboard inputs to adjust player orientation
        static float Yaw(3.141592f);  
        if (Platform.Key[VK_LEFT])  Yaw += 0.02f;
        if (Platform.Key[VK_RIGHT]) Yaw -= 0.02f;

        // Keyboard inputs to adjust player position
        static Vector3f Pos2(0.0f,1.6f,-5.0f);
        if (Platform.Key['W']||Platform.Key[VK_UP])     Pos2+=Matrix4f::RotationY(Yaw).Transform(Vector3f(0,0,-0.05f));
        if (Platform.Key['S']||Platform.Key[VK_DOWN])   Pos2+=Matrix4f::RotationY(Yaw).Transform(Vector3f(0,0,+0.05f));
        if (Platform.Key['D'])                          Pos2+=Matrix4f::RotationY(Yaw).Transform(Vector3f(+0.05f,0,0));
        if (Platform.Key['A'])                          Pos2+=Matrix4f::RotationY(Yaw).Transform(Vector3f(-0.05f,0,0));
		Pos2.y = ovrHmd_GetFloat(HMD, OVR_KEY_EYE_HEIGHT, Pos2.y);


// ADDING START		
		//// If the hand is close to the cube, the cube moves with the hand.
		Vector3f pos_cube = roomScene.Models[1]->Pos;
		Vector3f pos_hand = roomScene.Models[2]->Pos;
		if (pos_cube.x - pos_hand.x < 0 && pos_cube.x - pos_hand.x > -0.7 && pos_cube.y - pos_hand.y < 0 && pos_cube.y - pos_hand.y > -0.3 && pos_cube.z - pos_hand.z < 0.15 && pos_cube.z - pos_hand.z > -1.05)
		{
			flag_hand_to_cube = 1;
			//roomScene.Models[1]->Pos += Vector3f(0.05f, 0, 0);
		}
		//roomScene.Models[2]->Pos += Vector3f(0.05f, 0, 0);

		//// Manipulate my hand
		if (Platform.Key['J']){ for (int finger_num = 0; finger_num < 46 + flag_hand_to_cube; finger_num++)	roomScene.Models[finger_num + 2 - flag_hand_to_cube]->Pos += Vector3f(0.05f, 0, 0); }
		if (Platform.Key['L']){ for (int finger_num = 0; finger_num < 46 + flag_hand_to_cube; finger_num++)	roomScene.Models[finger_num + 2 - flag_hand_to_cube]->Pos += Vector3f(-0.05f, 0, 0); }
		if (Platform.Key['U']){ for (int finger_num = 0; finger_num < 46 + flag_hand_to_cube; finger_num++)	roomScene.Models[finger_num + 2 - flag_hand_to_cube]->Pos += Vector3f(0, 0, 0.05f); }
		if (Platform.Key['O']){ for (int finger_num = 0; finger_num < 46 + flag_hand_to_cube; finger_num++)	roomScene.Models[finger_num + 2 - flag_hand_to_cube]->Pos += Vector3f(0, 0, -0.05f); }
		if (Platform.Key['I']){ for (int finger_num = 0; finger_num < 46 + flag_hand_to_cube; finger_num++)	roomScene.Models[finger_num + 2 - flag_hand_to_cube]->Pos += Vector3f(0, 0.05f, 0); }
		if (Platform.Key['K']){ for (int finger_num = 0; finger_num < 46 + flag_hand_to_cube; finger_num++)	roomScene.Models[finger_num + 2 - flag_hand_to_cube]->Pos += Vector3f(0, -0.05f, 0); }
		//// Press P to release the cube
		if (Platform.Key['P']){ for (int finger_num = 0; finger_num < 46; finger_num++)	roomScene.Models[finger_num + 2]->Pos += Vector3f(-0.2f, 0, 0); flag_hand_to_cube = 0; }

		//// Control the button
		static bool press_flag = true;
		static int move_time = 0;
		//// Press B to press the button
		if (Platform.Key['B'] && press_flag)
		{
			roomScene.Models[55]->Pos += Vector3f(0, 0, 0.03f);
			move_time++;
			if (move_time>4)
				press_flag = false;
		}
		//// Press N to lift the button up
		if (Platform.Key['N'] && press_flag)
		{
			roomScene.Models[55]->Pos += Vector3f(0, 0, -0.03f);
			move_time++;
			if (move_time>4)
				press_flag = false;
		}
		//// Press M to reset the status of the button, or it can only be moved once.
		if (Platform.Key['M']){ move_time = 0; press_flag = true; }

		//// Manipulate the new hand
		if (Platform.Key['F']){ for (int finger_num = 0; finger_num < 6; finger_num++)	roomScene.Models[finger_num + 48]->Pos += Vector3f(0.05f, 0, 0); }
		if (Platform.Key['H']){ for (int finger_num = 0; finger_num < 6; finger_num++)	roomScene.Models[finger_num + 48]->Pos += Vector3f(-0.05f, 0, 0); }
		if (Platform.Key['R']){ for (int finger_num = 0; finger_num < 6; finger_num++)	roomScene.Models[finger_num + 48]->Pos += Vector3f(0, 0, 0.05f); }
		if (Platform.Key['Y']){ for (int finger_num = 0; finger_num < 6; finger_num++)	roomScene.Models[finger_num + 48]->Pos += Vector3f(0, 0, -0.05f); }
		if (Platform.Key['T']){ for (int finger_num = 0; finger_num < 6; finger_num++)	roomScene.Models[finger_num + 48]->Pos += Vector3f(0, 0.05f, 0); }
		if (Platform.Key['G']){ for (int finger_num = 0; finger_num < 6; finger_num++)	roomScene.Models[finger_num + 48]->Pos += Vector3f(0, -0.05f, 0); }

		//// If the index finger is close to the button, it press the button.
		Vector3f pos_button = roomScene.Models[55]->Pos;
		Vector3f pos_new_index = roomScene.Models[49]->Pos * 2 - roomScene.Models[53]->Pos;
		if (pos_button.x - pos_new_index.x < 0.15 && pos_button.x - pos_new_index.x > -0.25 && pos_button.y - pos_new_index.y < 0.15 && pos_button.y - pos_new_index.y > -0.15 && pos_button.z - pos_new_index.z < 0.25 && pos_button.z - pos_new_index.z > 0.1 && press_flag)
		{
			roomScene.Models[55]->Pos += Vector3f(0, 0, 0.03f);
			move_time++;
			if (move_time>4)
				press_flag = false;
		}

		//// Add movements of hand
		//// grab
		static bool flag_grab = false;
		static Vector3f pos_current_hand;
		static Vector3f pos_current_cube;
		if (Platform.Key['Z']){
			flag_grab = !flag_grab;
			pos_current_hand = roomScene.Models[53]->Pos;
			pos_current_cube = roomScene.Models[1]->Pos;
		}
		if (flag_grab)
			roomScene.Models[1]->Pos = pos_current_cube + roomScene.Models[53]->Pos - pos_current_hand;
		//// throw
		static float throw_speed = 0;
		if (Platform.Key['X'] && throw_speed < 0.009f)
			throw_speed = 0.15f;
		if (throw_speed > 0)
		{
			roomScene.Models[1]->Pos += Vector3f(0, 0, throw_speed);
			throw_speed -= 0.01f;
		}
		//// turn right & left
		static float rotate_angle = 0;
		static int rotate_time = 0;
		if (Platform.Key['C'] && rotate_time == 0)
			rotate_time = 10;
		if (Platform.Key['V'] && rotate_time == 0)
			rotate_time = -10;
		if (rotate_time > 0)	{ rotate_angle += 0.157f; rotate_time--; }
		if (rotate_time < 0)	{ rotate_angle -= 0.157f; rotate_time++; }
		roomScene.Models[1]->Rot = Quatf(Vector3f(0, 0, 1), rotate_angle);
		//// reset the cube
		if (Platform.Key['Q'])
			roomScene.Models[1]->Pos = Vector3f(2.0f, 1.0f, 0.1f);


		std::vector<Vector3f> aver_pos_six_point = pos_six_point;
		//// Change the position of the hand according to the camera
		roomScene.Models[53]->Pos = aver_pos_six_point[0] + Vector3f(0, -0.1f, 0);
		for (int cylinder_num = 1; cylinder_num < 6; cylinder_num++)
			if (aver_pos_six_point[cylinder_num].x != 0 || aver_pos_six_point[cylinder_num].y != 0 || aver_pos_six_point[cylinder_num].z != 0)
			{
				float Cylinder_length = aver_pos_six_point[0].Distance(aver_pos_six_point[cylinder_num]) + 0.01f;
				roomScene.Models[47 + cylinder_num]->Pos = (aver_pos_six_point[0] + aver_pos_six_point[cylinder_num]) / 2;
				roomScene.Models[47 + cylinder_num]->Rot = Quatf(roomScene.get_Normal(aver_pos_six_point[0], aver_pos_six_point[0] + Vector3f(0, 0, 1.0f), aver_pos_six_point[cylinder_num]), (aver_pos_six_point[cylinder_num] - aver_pos_six_point[0]).Angle(Vector3f(0, 0, Cylinder_length)));
			}

		//// If the player moves, his hand also moves.
		for (int i = 48; i <= 53; i++)
		{
			roomScene.Models[i]->Pos = roomScene.Models[i]->Pos + Pos2 + Vector3f(0, 0, 3);
		}
// ADDING END
        

		// Animate the cube
        static float cubeClock = 0;
        roomScene.Models[0]->Pos = Vector3f(9 * sin(cubeClock), 3, 9 * cos(cubeClock += 0.015f));


        // Get eye poses, feeding in correct IPD offset
        ovrVector3f               ViewOffset[2] = { EyeRenderDesc[0].HmdToEyeViewOffset,
                                                    EyeRenderDesc[1].HmdToEyeViewOffset };
        ovrPosef                  EyeRenderPose[2];

        ovrFrameTiming   ftiming = ovrHmd_GetFrameTiming(HMD, 0);
        ovrTrackingState hmdState = ovrHmd_GetTrackingState(HMD, ftiming.DisplayMidpointSeconds);
        ovr_CalcEyePoses(hmdState.HeadPose.ThePose, ViewOffset, EyeRenderPose);

        if (isVisible)
        {
            for (int eye = 0; eye<2; eye++)
            {
                // Increment to use next texture, just before writing
                eyeRenderTexture[eye]->TextureSet->CurrentIndex = (eyeRenderTexture[eye]->TextureSet->CurrentIndex + 1) % eyeRenderTexture[eye]->TextureSet->TextureCount;

                // Switch to eye render target
                eyeRenderTexture[eye]->SetAndClearRenderSurface(eyeDepthBuffer[eye]);

                // Get view and projection matrices
                Matrix4f rollPitchYaw = Matrix4f::RotationY(Yaw);
                Matrix4f finalRollPitchYaw = rollPitchYaw * Matrix4f(EyeRenderPose[eye].Orientation);
                Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
                Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
                Vector3f shiftedEyePos = Pos2 + rollPitchYaw.Transform(EyeRenderPose[eye].Position);

                Matrix4f view = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
                Matrix4f proj = ovrMatrix4f_Projection(HMD->DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_RightHanded);

            	// Render world
            	roomScene.Render(view,proj);

            	// Avoids an error when calling SetAndClearRenderSurface during next iteration.
            	// Without this, during the next while loop iteration SetAndClearRenderSurface
            	// would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
            	// associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
            	eyeRenderTexture[eye]->UnsetRenderSurface();
            }
        }

        // Do distortion rendering, Present and flush/sync

        // Set up positional data.
        ovrViewScaleDesc viewScaleDesc;
        viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
        viewScaleDesc.HmdToEyeViewOffset[0] = ViewOffset[0];
        viewScaleDesc.HmdToEyeViewOffset[1] = ViewOffset[1];

        ovrLayerEyeFov ld;
        ld.Header.Type  = ovrLayerType_EyeFov;
        ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

        for (int eye = 0; eye < 2; eye++)
        {
            ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureSet;
            ld.Viewport[eye]     = Recti(eyeRenderTexture[eye]->GetSize());
            ld.Fov[eye]          = HMD->DefaultEyeFov[eye];
            ld.RenderPose[eye]   = EyeRenderPose[eye];
        }

        ovrLayerHeader* layers = &ld.Header;
        ovrResult result = ovrHmd_SubmitFrame(HMD, 0, &viewScaleDesc, &layers, 1);
        isVisible = result == ovrSuccess;

        // Blit mirror texture to back buffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        GLint w = mirrorTexture->OGL.Header.TextureSize.w;
        GLint h = mirrorTexture->OGL.Header.TextureSize.h;
        glBlitFramebuffer(0, h, w, 0,
                          0, 0, w, h,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        SwapBuffers(Platform.hDC);
    }

    glDeleteFramebuffers(1, &mirrorFBO);
    ovrHmd_DestroyMirrorTexture(HMD, (ovrTexture*)mirrorTexture);
    ovrHmd_DestroySwapTextureSet(HMD, eyeRenderTexture[0]->TextureSet);
    ovrHmd_DestroySwapTextureSet(HMD, eyeRenderTexture[1]->TextureSet);

    // Release
    ovrHmd_Destroy(HMD);
    ovr_Shutdown();
    Platform.ReleaseWindow(hinst);
    OVR::System::Destroy();

    return 0;
}



