#include "OculusBase.h"
#include "OculusSampleScene.h"

int main()
{
  VPL::OculusBase oculusbase;

  VPL::OculusSampleScene oculusscene;

  oculusbase.InitRendering(oculusscene);
  
  for (int ii = 0; true; ii++)
  {
	  oculusbase.RenderFrame(oculusscene);
  }
  
  oculusbase.RenderTerminate();
}