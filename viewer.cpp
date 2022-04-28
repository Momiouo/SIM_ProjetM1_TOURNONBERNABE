#include "viewer.h"

#include <math.h>
#include <iostream>
#include <QTime>
#include "meshLoader.h"

using namespace std;

Viewer::Viewer(char *, const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _currentshader(0),
    _light(glm::vec3(0,0,1)),
    _motion(glm::vec3(0,0,0)),
    _automotion(glm::vec3(0,0,0)),
    _mode(false),
    lastX(0),
    lastY(0),
    _ndResol(512),
    _depthResol(1024) {

  setlocale(LC_ALL,"C");

  _grid = new Grid(_ndResol,-1.0f,1.0f);
  _cam  = new Camera(1.0f,glm::vec3(0.0f,0.0f,0.0f));
  _mesh = new Mesh(filename);

  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}

Viewer::~Viewer() {
  delete _timer;
  delete _grid;
  delete _cam;
  delete _mesh;

  // delete all GPU objects
  deleteShaders();
  deleteVAO(); 
  deleteTextures();
  deleteFBO();
}

//For the fog effect
void Viewer::createFBO() {
  glGenFramebuffers(1,&_fbo);
  //glGenTextures(1,&_rendNormalId);
  //glGenTextures(1,&_rendColorId);

  //Shadow Mapping
  glGenTextures(1,&_texDepth);

  // create the texture for rendering depth values
  glBindTexture(GL_TEXTURE_2D,_texDepth);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,_depthResol,_depthResol,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

  // attach textures to framebuffer object 
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo);
  glBindTexture(GL_TEXTURE_2D,_texDepth);
  glFramebufferTexture2D(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,_texDepth,0);

  //
  // create the texture for rendering colors
  /*
  glBindTexture(GL_TEXTURE_2D,_rendColorId);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F,width(),height(),0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // create the texture for rendering normals 
  glBindTexture(GL_TEXTURE_2D,_rendNormalId);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F,width(),height(),0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // create the texture for rendering depth values
  //ALERTE : en double le bind de texDepth : lequel supprimer ?
  glBindTexture(GL_TEXTURE_2D,_texDepth);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,width(),height(),0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // attach textures to framebuffer object 
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo);
  glBindTexture(GL_TEXTURE_2D,_rendColorId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_rendColorId,0);
  glBindTexture(GL_TEXTURE_2D,_rendNormalId);
  glBindTexture(GL_TEXTURE_2D,_texDepth);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,_texDepth,0);
  */
  //

  glBindFramebuffer(GL_FRAMEBUFFER,0);

  // test if everything is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    cout << "Warning: FBO not complete!" << endl;

  // disable FBO
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Viewer::deleteFBO() {
  // delete all FBO Ids
  glDeleteFramebuffers(1,&_fbo);
  //glDeleteTextures(1,&_rendNormalId);
  //glDeleteTextures(1,&_rendColorId);
  //Shadow Mapping
  glDeleteTextures(1,&_texDepth);
}

void Viewer::createTextures() {
  QImage image, texture_neige, texture_eau, texture_roche;

  // enable the use of 2D textures
  glEnable(GL_TEXTURE_2D);

  // create three textures (possible) on the GPU
  glGenTextures(4,_texIds);

  // load an image (CPU side)
  image = QGLWidget::convertToGLFormat(QImage("textures/grass_text.jpg"));

  // activate this texture (the current one)
  glBindTexture(GL_TEXTURE_2D,_texIds[0]);

  // set texture parameters
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

  // transfer data from CPU to GPU memory
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,image.width(),image.height(),0,
  	       GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)image.bits());

  // generate mipmaps
  glGenerateMipmap(GL_TEXTURE_2D);

  //Texture enneigée 
  texture_neige = QGLWidget::convertToGLFormat(QImage("textures/snow_text.jpg")); 
  //Activation de la texture
  glBindTexture(GL_TEXTURE_2D,_texIds[1]); 
  //Set texture parameters
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  //Transfer data from CPU to GPU memory
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,texture_neige.width(),texture_neige.height(),0,
  	       GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)texture_neige.bits());
  // generate mipmaps
  glGenerateMipmap(GL_TEXTURE_2D);

  //Texture eau 
  texture_eau = QGLWidget::convertToGLFormat(QImage("textures/water_text.jpg")); 
  //Activation de la texture
  glBindTexture(GL_TEXTURE_2D,_texIds[2]); 
  //Set texture parameters
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  //Transfer data from CPU to GPU memory
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,texture_eau.width(),texture_eau.height(),0,
  	       GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)texture_eau.bits());
  // generate mipmaps
  glGenerateMipmap(GL_TEXTURE_2D);

  //Texture roche 
  texture_roche = QGLWidget::convertToGLFormat(QImage("textures/rock_text.jpg")); 
  //Activation de la texture
  glBindTexture(GL_TEXTURE_2D,_texIds[3]); 
  //Set texture parameters
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  //Transfer data from CPU to GPU memory
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,texture_roche.width(),texture_roche.height(),0,
  	       GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)texture_roche.bits());
  // generate mipmaps
  glGenerateMipmap(GL_TEXTURE_2D);

  // create the texture for rendering normals "fog effect"
  /*
  glBindTexture(GL_TEXTURE_2D,_rendNormalId);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F,width(),height(),0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  */

  
}

void Viewer::deleteTextures() {
  glDeleteTextures(1,_texIds);
  glDeleteTextures(2,_texIds);
  glDeleteTextures(3,_texIds);
  glDeleteTextures(3,_texIds);
}

void Viewer::createVAO() {
  // cree les buffers associés au terrain 

  glGenBuffers(2,_terrain);
  glGenVertexArrays(1,&_vaoTerrain);

  // create the VBO associated with the grid (the terrain)
  glBindVertexArray(_vaoTerrain);
  glBindBuffer(GL_ARRAY_BUFFER,_terrain[0]); //Vertices 
  glBufferData(GL_ARRAY_BUFFER,_grid->nbVertices()*3*sizeof(float),_grid->vertices(),GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_terrain[1]); // indices 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_grid->nbFaces()*3*sizeof(int),_grid->faces(),GL_STATIC_DRAW); 
}

void Viewer::deleteVAO() {
  glDeleteBuffers(2,_terrain);
  glDeleteVertexArrays(1,&_vaoTerrain);
}

void Viewer::createShaders() {
  _terrainShader = new Shader();
  _waterShader = new Shader();
  _shadowMapShader = new Shader(); 

  _terrainShader->load("shaders/terrain.vert","shaders/terrain.frag");
  _waterShader->load("shaders/water.vert","shaders/water.frag");
  _shadowMapShader->load("shaders/shadow-map.vert","shaders/shadow-map.frag");
}

void Viewer::deleteShaders() {
  delete _shadowMapShader; 
  delete _terrainShader;
  delete _waterShader;

  _terrainShader = NULL;
  _waterShader = NULL;
  _shadowMapShader = NULL;
}

void Viewer::reloadShaders() {
  if(_terrainShader)
    _terrainShader->reload("shaders/terrain.vert","shaders/terrain.frag");
    _waterShader->reload("shaders/water.vert","shaders/water.frag");
}


void Viewer::drawScene(GLuint id) {

  // send uniform variables 
  glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(_cam->mdvMatrix()[0][0]));
  glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));
  glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));
  glUniform3fv(glGetUniformLocation(id,"light"),1,&(_light[0]));
  glUniform3fv(glGetUniformLocation(id,"motion"),1,&(_motion[0]));

  _automotion[0] += 0.0001f;
  glUniform3fv(glGetUniformLocation(id,"automotion"),1,&(_automotion[0]));

  // draw faces 
  glBindVertexArray(_vaoTerrain);
  glDrawElements(GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0);
  glBindVertexArray(0);

  //send textures
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texIds[0]);
  glUniform1i(glGetUniformLocation(id,"testMap"),0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,_texIds[1]);
  glUniform1i(glGetUniformLocation(id,"snowMap"),1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D,_texIds[2]);
  glUniform1i(glGetUniformLocation(id,"lakeMap"),2);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D,_texIds[3]);
  glUniform1i(glGetUniformLocation(id,"rockMap"),3);
/*
  //Shadow mapping 
  // send initial mvp depth matrix
  const float size = _mesh->radius*10;
  glm::vec3 l   = glm::transpose(_cam->normalMatrix())*_light;
  glm::mat4 p = glm::ortho<float>(-size,size,-size,size,-size,2*size);
  glm::mat4 v   = glm::lookAt(l, glm::vec3(0,0,0), glm::vec3(0,1,0));
  glm::mat4 m   = glm::mat4(1.0);
  glm::mat4 mv  = v*m;

  const glm::mat4 mvpDepth = p*mv;
  glUniformMatrix4fv(glGetUniformLocation(id,"mvpDepthMat"),1,GL_FALSE,&mvpDepth[0][0]);

  glActiveTexture(GL_TEXTURE0+2);
  glBindTexture(GL_TEXTURE_2D,_texDepth);
  glUniform1i(glGetUniformLocation(id,"shadowmap"),4);
  */

}

/*
void Viewer::enableShader(unsigned int shader) {
  // current shader ID
  GLuint id = _shaders[shader]->id();

  // activate the current shader
  glUseProgram(id);

  // send the model-view matrix
  glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(_cam->mdvMatrix()[0][0]));

  // send the projection matrix
  glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));

  // send the normal matrix (top-left 3x3 transpose(inverse(MDV)))
  glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));

  // send a light direction (defined in camera space)
  glUniform3fv(glGetUniformLocation(id,"light"),1,&(_light[0]));

  // send textures
  /*
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texIds[0]);
  glUniform1i(glGetUniformLocation(id,"testMap"),0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texIds[1]);
  glUniform1i(glGetUniformLocation(id,"snowMap"),0);

}
*/


void Viewer::disableShader() {
  // desactivate all shaders
  glUseProgram(0);
}

void Viewer::drawSceneFromLight(GLuint id) {
   const float size = _mesh->radius*10;

  // mdv matrix from the light point of view
  glm::vec3 l   = glm::transpose(_cam->normalMatrix())*_light;
  glm::mat4 p   = glm::ortho<float>(-size,size,-size,size,-size,2*size);
  glm::mat4 v   = glm::lookAt(l, glm::vec3(0,0,0), glm::vec3(0,1,0));
  glm::mat4 m   = glm::mat4(1.0);
  glm::mat4 mv  = v*m;

  //glBindVertexArray(_vaoObject);

  // draw several objects
  const float r = _mesh->radius*2;
  const int   n = 2;
  for(int i=-n;i<=n;++i) {
    
    // send the modelview projection matrix (changes for each object)
    const glm::vec3 pos = glm::vec3(i*r,0,i*r);
    const glm::mat4 mdv = glm::translate(mv,pos);
    const glm::mat4 mvp = p*mdv;
    glUniformMatrix4fv(glGetUniformLocation(id,"mvpMat"),1,GL_FALSE,&mvp[0][0]);
    
    // draw faces 
    glDrawElements(GL_TRIANGLES,3*_mesh->nb_faces,GL_UNSIGNED_INT,(void *)0);
  }
  
  // send initial mvp matrix 
  const glm::mat4 mvp = p*mv;
  glUniformMatrix4fv(glGetUniformLocation(id,"mvpMat"),1,GL_FALSE,&mvp[0][0]);
  
  // draw the floor
  //glBindVertexArray(_vaoFloor);
  glDrawArrays(GL_TRIANGLES,0,6);

  // disable VAO
  glBindVertexArray(0);
}

void Viewer::paintGL() {
   // activate the created framebuffer object
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo);
  
  // allow opengl depth test 
  glEnable(GL_DEPTH_TEST);
  
  // screen viewport
  glViewport(0,0,width(),height());

  // clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // activate the buffer shader 
  glUseProgram(_terrainShader->id());
    
  // generate the map
  drawScene(_terrainShader->id());

  // activate the buffer shader 
  glUseProgram(_waterShader->id());

  // tell the GPU to use this specified shader and send custom variables (matrices and others)
  //enableShader(_currentshader);

  // generate the map
  drawScene(_waterShader->id());

  // activate the shadow map shader
  glUseProgram(_shadowMapShader->id());

  // create the shadow map 
  drawSceneFromLight(_shadowMapShader->id());

  // desactivate fbo
  glBindFramebuffer(GL_FRAMEBUFFER,0);

  // clear everything
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // disable depth test 
  glDisable(GL_DEPTH_TEST);

  // disable shader 
  glUseProgram(0);
}

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);
  // re-init the FBO (textures need to be resized to the new viewport size)
  createFBO();
  updateGL();
}

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
    lastX = p.x;//init first value of click for rotation
    lastY = p.y;
    _mode = false;    
  } else if(me->button()==Qt::MidButton) {
    _cam->initMoveZ(p);
    _mode = false;
  } else if(me->button()==Qt::RightButton) {
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
    _mode = true;
  } 

  updateGL();
}

void Viewer::mouseMoveEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));
 
  if(_mode) {
    // light mode
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
  } else {
    // camera mode
    _cam->move(p);
    /*
    float xoffset = p.x - lastX;
    float yoffset = lastY - p.y; 
    float sensitivity = 0.01f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;*/
    //_cam->initRotation(p, xoffset, yoffset);
  }

  updateGL();
}

void Viewer::keyPressEvent(QKeyEvent *ke) {
  const float step = 0.05;
  /*
  if(ke->key()==Qt::Key_Z) {
    glm::vec2 v = glm::vec2(glm::transpose(_cam->normalMatrix())*glm::vec3(0,0,-1))*step;
    if(v[0]!=0.0 && v[1]!=0.0) v = glm::normalize(v)*step;
    else v = glm::vec2(0,1)*step;
    _motion[0] += v[0];
    _motion[1] += v[1];
  }

  if(ke->key()==Qt::Key_S) {
    glm::vec2 v = glm::vec2(glm::transpose(_cam->normalMatrix())*glm::vec3(0,0,-1))*step;
    if(v[0]!=0.0 && v[1]!=0.0) v = glm::normalize(v)*step;
    else v = glm::vec2(0,1)*step;
    _motion[0] -= v[0];
    _motion[1] -= v[1];
  }

  if(ke->key()==Qt::Key_Q) {
    _motion[2] += step;
  }

  if(ke->key()==Qt::Key_D) {
    _motion[2] -= step;
  }*/


  // key a: play/stop animation
  /*if(ke->key()==Qt::Key_A) {
    if(_timer->isActive()) 
      _timer->stop();
    else 
      _timer->start();
  }*/

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }
  
  //Camera mouvement
  if(ke->key()==Qt::Key_Z) {
    _cam->moveZ(true);
  }
  if(ke->key()==Qt::Key_S) {
    _cam->moveZ(false);
  }
  if(ke->key()==Qt::Key_Up) {
    _cam->moveHeight(true);
  }
  if(ke->key()==Qt::Key_Down) {
    _cam->moveHeight(false);
  }
  if(ke->key()==Qt::Key_Left) {
    _cam->moveSide(false);
  }
  if(ke->key()==Qt::Key_Right) {
    _cam->moveSide(true);
  }
  if(ke->key()==Qt::Key_A) {
    //_cam->rotateSide(false);
  }
  if(ke->key()==Qt::Key_E) {
    //_cam->rotateSide(true);
  }

   // key f: compute FPS
   if(ke->key()==Qt::Key_F) {
     int elapsed;
     QTime timer;
     timer.start();
     unsigned int nb = 500;
     for(unsigned int i=0;i<nb;++i) {
       paintGL();
     }
     elapsed = timer.elapsed();
     double t = (double)nb/((double)elapsed);
     cout << "FPS : " << t*1000.0 << endl;
   }

  // key r: reload shaders 
  if(ke->key()==Qt::Key_R) {
    reloadShaders();
  }

  updateGL();
}

void Viewer::initializeGL() {
  // make this window the current one
  makeCurrent();

  // init and chack glew
  if(glewInit()!=GLEW_OK) {
    cerr << "Warning: glewInit failed!" << endl;
  }

  // init OpenGL settings
  glClearColor(0.0,0.0,0.0,1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,width(),height());

  // initialize camera
  _cam->initialize(width(),height(),true);

  // init shaders 
  createShaders();

  // init and load all shader files
  //for(unsigned int i=0;i<_vertexFilenames.size();++i) {
  //  _shaders.push_back(new Shader());
  //  _shaders[i]->load(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
  //}

  // init VAO/VBO
  createVAO();
  
  //Create the textures
  createTextures();

  // create/init FBO
  createFBO();

  // starts the timer 
  _timer->start();
}

