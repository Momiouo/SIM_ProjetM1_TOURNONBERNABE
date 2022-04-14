#version 330

// input uniforms 
uniform vec3 light;
uniform vec3 motion;

// in variables 
in vec3  normalView;
in vec3  eyeView;
//textures
in vec2 uvcoord;

// out buffers 
layout(location = 0) out vec4 bufferColor;

//textures variables
//uniform vec3      light;
uniform sampler2D colormap;
uniform sampler2D specularmap;
uniform sampler2D aomap;

void main() {


  /* base version
  const vec3 ambient  = vec3(0.3,0.3,0.2);
  const vec3 diffuse  = vec3(0.3,0.5,0.8);
  const vec3 specular = vec3(0.8,0.2,0.2);
  const float et = 50.0;

  vec3 n = normalize(normalView);
  vec3 e = normalize(eyeView);
  vec3 l = normalize(light);

  float diff = dot(l,n);
  float spec = pow(max(dot(reflect(l,n),e),0.0),et);

  vec3 color = ambient + diff*diffuse + spec*specular;

  outColor = vec4(color,1.0);
  */

//Texture version
  vec4 texColor = texture(colormap,uvcoord);
  vec4 texspec = texture(specularmap,uvcoord);
  vec4 texao = texture(aomap,uvcoord);
  float et  =  (texspec.x)*100;

  vec3 n = normalize(normalView);
  vec3 e = normalize(eyeView);
  vec3 l = normalize(light);

  float diff = max(dot(l,n),0.);
  float spec = pow(max(dot(reflect(l,n),e),0.0),et);

  bufferColor = texColor*(diff + spec)*2.0;
  bufferColor = bufferColor * texao;
}
