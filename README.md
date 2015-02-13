VR-Hands-VPL
============

For LINUX

mkdir build

cd build

cmake ..

make

./walalaExample

If the build directory exists already, you must clear the directory


For WINDOWS

mkdir build

cd build

cmake ..

click on the newly generated sln

compile (F7) on visual studio

run (ctrl + F5) on visual studio

External libraries
============

OpenCV 
============

For Linux 

Download the latest OpenCV

Extract and go into the opencv-3.0.0 folder, create a folder called "build", move everything inside it.

cd build

ccmake ..

Add/Remove modules accordingly

make

sudo make install


For Windows

Download the latest OpenCV

Set "OpenCV_DIR" system variable into with "XXX/opencv/build"

Set system variable at control panel->systems->advanced system settings->environment variables->system variables

OculusSDK
============
For Linux 

Download the latest Oculus PC SDK

Extract and go into the ovr_sdk_linux folder

sh ConfigureDebian.sh

make run

gedit ~/.bashrc 

add "export OCULUS_SDK_ROOT_DIR=/home/jason/ovr_sdk_linux_x.x.x" at the very end


For Windows

Download the latest Oculus PC SDK

Set "OCULUS_SDK_ROOT_DIR" system variable into with "XXX/ovr_sdk_linux_x.x.x"

OvrvisionSDK
============
For Windows

Download the latest Oculus PC SDK

Set "OVRVISION_SDK_ROOT_DIR" system variable into with "XXX/OvrVision/ovrvisionsdk_xxx"