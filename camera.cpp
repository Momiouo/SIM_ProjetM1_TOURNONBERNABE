#include "camera.h"

Camera::Camera(float radius,const glm::vec3 &center,int mode) 
  : _m(NONE),
    _w(0), 
    _h(0), 
    _p(0,0),
    _c(glmToVec3(center)),
    _r(radius),
    _t(),
    _f(45.0f),
    _d(mode),
    _up(0,0,0),
    _right(0,0,0),
    _view(0,0,0),
    _zmin(0),
    cameraPos(glm::vec3(-1.0f, 0.0f,  2.0f)),
    cameraFront(glm::vec3(1.0f, 0.0f,  -1.7f)),
    cameraUp(glm::vec3(0.0f, 0.0f,  1.0f)),
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
  _t = TrackBall(rad*2,Vec2f(wo2,ho2));
  _f = 45.0f;
  
  // projection transformations  
  if(_d==PERSP) {
    _matp = glmToMat4(glm::perspective(_f,(float)_w/(float)_h,_r/tmp1,_r*tmp1));
  } else {
    _matp = glmToMat4(glm::ortho((float)(-_w),(float)_w,(float)(-_h),(float)_h,0.0f,_r*100.0f));
  }
  
  if(!replace)
    return;

  //Computation of camera variables
  cameraPos = glm::vec3(-1.0f, 0.0f,  2.0f); //Camera position
  cameraFront = glm::vec3(0.5f, 0.0f, -0.8f); //Target of the camera
  cameraUp = glm::vec3(0.0f, 0.0f,  1.0f); //Up axis perpendicular to the target of the camera

   _matm = glmToMat4(glm::lookAt(cameraPos,cameraPos + cameraFront,cameraUp));

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

//Move To the left and the right of the map with left and right arrow
void Camera::moveSide(bool right){
  const float cameraSpeed = 5.0f;
  const float s = _r/300.0;
  if(right){
    _matm = _matm.translateEq(Vec3f((-cameraSpeed)*s,(0.0f)*s,0.0f));
  }else{
    _matm = _matm.translateEq(Vec3f((cameraSpeed)*s,(0.0f)*s,0.0f));
  }
  // update params 
  updateCamDists(_matm);
}

//Move To the front and the back of the map with up and down arrow
void Camera::moveHeight(bool up){
  const float cameraSpeed = 5.0f;
  const float s = _r/300.0;
  if(up){
    _matm = _matm.translateEq(Vec3f((0.0f)*s,(-cameraSpeed)*s,0.0f));
  }else{
    _matm = _matm.translateEq(Vec3f((0.0f)*s,(cameraSpeed)*s,0.0f));
  }
  // update params 
  updateCamDists(_matm);
}

//Zoom in the map with z and dezoom with s
void Camera::moveZ(bool forward){
  const float cameraSpeed = 2.0f;
  const float s = _r/100.0;
  if(forward){
    cameraPos += cameraSpeed * cameraFront;
    _matm = _matm.translateEq(Vec3f(0.0f,0.0f,(cameraSpeed)*s));
  }else{
    cameraPos -= cameraSpeed * cameraFront;
    _matm = _matm.translateEq(Vec3f(0.0f,0.0f,(-cameraSpeed)*s));
  }
  updateCamDists(_matm);
}
