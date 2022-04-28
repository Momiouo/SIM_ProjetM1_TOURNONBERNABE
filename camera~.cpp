#include "camera.h"


Camera::Camera(float radius,const glm::vec3 &center,int mode) 
  : _m(NONE),
    _w(0), 
    _h(0), 
    _p(0,0),
    _c(glmToVec3(center)),
    _r(radius),
    _f(45.0f),
    _d(mode),
    _up(0,0,0),
    _right(0,0,0),
    _view(0,0,0),
    _zmin(0),
    cameraPos(glm::vec3(-1.0f, 0.0f,  2.0f)),
    cameraFront(glm::vec3(1.0f, 0.0f,  -1.7f)),
    cameraUp(glm::vec3(0.0f, 0.0f,  1.0f)),
    yaw(0),
    pitch(0),
    _zmax(0) {

}

void Camera::initialize(int w,int h,bool replace) {
  
  const float tmp1 = 100.0f;
  const float tmp2 = 3.0f;

  int wo2 = w/2;
  int ho2 = h/2;
  int rad = wo2<ho2 ? wo2 : ho2;
  
  // data initialization 
  _m = NONE;
  _w = w;
  _h = h;
  _f = 45.0f;
  yaw=-90.0f;
  pitch=30.0f;
  
  // projection transformations  
  if(_d==PERSP) {
    _matp = glmToMat4(glm::perspective(_f, (float)_w/(float)_h, 0.1f, 5.0f));
  } else {
    _matp = glmToMat4(glm::ortho((float)(-_w),(float)_w,(float)(-_h),(float)_h,0.0f,_r*100.0f));
  }
  
  if(!replace)
    return;

  //Computation of camera variables
  cameraPos = glm::vec3(-1.0f, 0.0f,  2.0f);
  cameraUp = glm::vec3(0.0f, 0.0f,  1.0f);
  glm::vec3 direction;
  direction.y = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.x = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(direction);


  //We set the position vector/ the direction vection / and the up vector
  _matm = glmToMat4(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp));

  // update params 
  updateCamVectors(_matm);
  updateCamDists(_matm);
}

void Camera::moveZ(bool forward){
  const float cameraSpeed = 0.05f;
  if(forward){
    cameraPos += cameraSpeed * cameraFront;
  }else{
    cameraPos -= cameraSpeed * cameraFront;
  }
 //update lookat
 _matm = glmToMat4(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp));
  // update params 
  updateCamVectors(_matm);
  updateCamDists(_matm);
}

void Camera::moveSide(bool right){
  const float cameraSpeed = 0.05f;
  if(right){
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  }else{
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  }
 //update lookat
 _matm = glmToMat4(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp));
  // update params 
  updateCamVectors(_matm);
  updateCamDists(_matm);
}


void Camera::myRotation(const glm::vec2 &p, float xoffset, float yoffset){
    yaw   += xoffset;//Rotation around y(vertical) axis
    pitch += yoffset;//Rotation around x(horizontal) axis

    //Permit to see only above and not below the terrain
    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.y = cos(glm::radians(yaw))* cos(glm::radians(pitch));
    direction.x = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(-96.0f)) * cos(glm::radians(30.0f));
    cameraFront = glm::normalize(direction);

    //printf("YAW = %f , PITCH = %f \n",yaw,pitch);

    //update lookat
   _matm = glmToMat4(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp));
    // update params 
    updateCamVectors(_matm);
    updateCamDists(_matm);
}


void Camera::setFovy(float f) {
  _f = f;
  initialize(_w,_h,false);
}

void Camera::setMode(int m) {
  if(_d!=m) {
    _d = m;
    initialize(_w,_h,false);
  }
}
