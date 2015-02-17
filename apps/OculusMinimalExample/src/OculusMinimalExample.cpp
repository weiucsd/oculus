#include "OculusBase.h"
#include "OculusSampleScene.h"

int main()
{
  VPL::OculusBase oculusbase;

  VPL::OculusSampleScene oculusscene;

  oculusbase.Run(oculusscene);
}