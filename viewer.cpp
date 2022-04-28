#include "viewer.h"

#include <math.h>
#include <iostream>
#include <QTime>

using namespace std;

Viewer::Viewer(char *,const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _currentshader(0),
    _light(glm::vec3(0,0,1)),
    _motion(glm::vec3(0,0,0)),
    _automotion(glm::vec3(0,0,0)),
    _mode(false),
    _ndResol(512) {

  //_anim = 0; //A METTRE DANS .h
  setlocale(LC_ALL,"C");

  _grid = new Grid(_ndResol,-1.0f,1.0f);
  _cam  = new Camera(1.0f,glm::vec3(0.0f,0.0f,0.0f));

  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}

Viewer::~Viewer() {
  delete _timer;
  delete _grid;
  delete _cam;

  // delete all GPU objects
  deleteShaders();
  deleteVAO(); 
  deleteTextures();
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
  _terrainShader->load("shaders/terrain.vert","shaders/terrain.frag");
  _waterShader->load("shaders/water.vert","shaders/water.frag");

}

void Viewer::deleteShaders() {
  delete _terrainShader;
  delete _waterShader;

  _terrainShader = NULL;
  _waterShader = NULL;
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

  //draw textures
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
}


void Viewer::paintGL() {
  
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

  // generate the map
  drawScene(_waterShader->id());

  // disable depth test 
  glDisable(GL_DEPTH_TEST);

  // disable shader 
  glUseProgram(0);
}

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);
  updateGL();
}

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
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
  }

  updateGL();
}

void Viewer::keyPressEvent(QKeyEvent *ke) {

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }
  
  //New Camera mouvement
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
    //_cam->rotateSide(false); //doesn't work at all
  }
  if(ke->key()==Qt::Key_E) {
    //_cam->rotateSide(true); //doesn't work at all
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

  // init VAO/VBO
  createVAO();
  //Create the textures
  createTextures();

  // starts the timer 
  _timer->start();
}

