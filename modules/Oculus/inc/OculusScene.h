#ifndef VPL_OCULUS_OCULUSSCENE_H
#define VPL_OCULUS_OCULUSSCENE_H

namespace VPL {

class OculusScene
{ 
  public:  
    OculusScene();  
    ~OculusScene();

    // Will be called one time
    virtual void InitScene(float &cam_x,float &cam_y,float &cam_z) = 0;

    // Will be called constantly
    virtual void DrawScene() = 0;

  private:
};

}  // namespace VPL

#endif