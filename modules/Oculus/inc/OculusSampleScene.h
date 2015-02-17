#ifndef VPL_OCULUS_OCULUSSAMPLESCENE_H
#define VPL_OCULUS_OCULUSSAMPLESCENE_H

#include <GL/glew.h>

#include <iostream>

#include "OculusScene.h"

namespace VPL {

class OculusSampleScene: public OculusScene
{ 
  public:  
    OculusSampleScene();  
    ~OculusSampleScene();

    void InitScene(float &cam_x,float &cam_y,float &cam_z);
    void DrawScene();

  private:
    void SetStaticLightPositions();
    void SetGLEnvironment();

    GLfloat *points_;
    GLfloat *normals_;
    GLuint  *indici_;
};

}  // namespace VPL

#endif