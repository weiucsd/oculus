#include "OculusSampleScene.h"

namespace VPL {

OculusSampleScene::OculusSampleScene()
{
	
}

OculusSampleScene::~OculusSampleScene()
{
  
}

void OculusSampleScene::SetGLEnvironment()
{
  // Create some lights, materials, etc
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Material
  GLfloat material_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
  GLfloat material_shininess[] = { 10.0f };
  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

  // Some (stationary) lights, position will be set every frame separately
  GLfloat light0diffuse[] = { 1.0f, 0.8f, 0.6f, 1.0f };
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0diffuse);
  glEnable(GL_LIGHT0);

  GLfloat light1diffuse[] = { 0.6f, 0.8f, 1.0f, 1.0f };
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1diffuse);
  glEnable(GL_LIGHT1);
}

void OculusSampleScene::InitScene(float &cam_x,float &cam_y,float &cam_z)
{
  SetGLEnvironment();

  // Initial camera position
  cam_x =  0.0f;
  cam_y =  0.0f;
  cam_z = -2.0f;

  GLfloat points[] =
  {
     0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f, 0.5f, 0.5f,
     0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
     0.5f,-0.5f, 0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f,-0.5f
  };

  GLfloat normals[] =
  {
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f,-1.0f,
     0.0f, 0.0f,-1.0f,
     0.0f, 0.0f,-1.0f,
     0.0f, 0.0f,-1.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f,-1.0f, 0.0f,
     0.0f,-1.0f, 0.0f,
     0.0f,-1.0f, 0.0f,
     0.0f,-1.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f
  };

  GLuint indici[] =
  {
    0, 1, 2, 3,
    4, 5, 6, 7,
    8, 9, 10, 11,
    12, 13, 14, 15,
    16, 17, 18, 19,
    20, 21, 22, 23
  };

  points_ = new GLfloat[24*3];
  normals_= new GLfloat[24*3];
  indici_ = new GLuint [24];

  std::copy(points, points + 24*3, points_);
  std::copy(normals,normals+ 24*3, normals_);
  std::copy(indici, indici + 24,   indici_);
}

void OculusSampleScene::SetStaticLightPositions()
{
  // (Re)set the light positions so they don't move along with the cube...
  GLfloat light0position[] = { 3.0f, 4.0f, 2.0f, 0.0f };
  glLightfv(GL_LIGHT0, GL_POSITION, light0position);

  GLfloat light1position[] = { -3.0f, -4.0f, 2.0f, 0.0f };
  glLightfv(GL_LIGHT1, GL_POSITION, light1position);
}

void OculusSampleScene::DrawScene()
{
  SetStaticLightPositions();

  glTranslatef((GLfloat)x, (GLfloat) y, 0);

  // turn the cube a little bit so we can feel depth
  glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
  glRotatef(40.0f, 0.0f, 1.0f, 0.0f);

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, points_);

  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, 0, normals_);

  glDrawElements(GL_QUADS, 6*4, GL_UNSIGNED_INT, indici_);

  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);    
}


}  // namespace VPL