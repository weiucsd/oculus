ABOUT THIS CODE
=======
This is a hand tracking demo in Oculus.  
This version is based on OculusSDK_0.6.0.1\Samples\OculusRoomTiny\OculusRoomTiny (GL), and Intel\PCSDK\sample\gesture_viewer.

To get this demo working, you need a Windows PC, since Oculus stop developing for Mac and Linux. Besides, your computer may need a dedicated graphics card Nvidia GTX 600 series or AMD Radeon HD 7000 series (or better), otherwise OpenGL cannot work. Then, you need to have Visual Studio 2013, since we provide you the lib for this platform and we have not used others.  
Next, you need to download ~~Oculus SDK for Windows V0.6.0.1-beta and~~ Oculus Runtime for Windows V0.6.0.1-beta from the link below.  
https://developer.oculus.com/downloads/  
Then, you need to install the runtime ~~and set "OCULUS_SDK_ROOT_DIR" system variable into "xxx\OculusSDK_0.6.0.1"~~.  
Besides, you need to download and install PCSDK from the link below. After restarting, it will automatically set "PCSDK_DIR" system variable into "C:\Program Files (x86)\Intel\PCSDK\"(default).  
https://software.intel.com/en-us/articles/intel-perceptual-computing-sdk-installer

Now you can use cmake to build this demo.  
Currently, you need to make some changes in VS2013(we may fix it later):  
  ~~1. In "Configuration Properties - C/C++ - Code Generation", change "Runtime Library" to "MTd".~~
  2. In "Configuration Properties - Linker - System", change "SubSystem" to "Windows".
  3. Set MovingCube as the Startup Project.
The demo should be able to run now.

About controlling in Oculus
-------
  * WASD - Move in the room  
  * RTYFGH - Control the hand with 5 cylinders (right hand)  
  * UIOJKL - Control the hand with 46 spheres (left hand)  
  * P - Detach the left hand from the red cube
  * Q - reset the red cube's position
  * Z - Grab the red cube, press again to release
  * X - Throw the red cube
  * C - Rotate the red cube to the right
  * V - Rotate the red cube to the left
  * B - Push the button
  * N - Pull the button
  * M - You need to press M every time before pushing or pulling the button, or it will not work :->

UPDATE
-------
### Aug. 4th, 2015
[Optimize] If the camera is not on, the program can still work.
### Aug. 5th, 2015
[Optimize] Revamp the cmake file. Add all included files and libraries to github, so you do not need to download the SDK or set the system variable. But you still need to install the runtime.  
[Modify] Change all the runtime library to Multi-threaded Debug DLL(MDd).
### Aug. 6th, 2015
[Add] Add iPhone to the demo.
