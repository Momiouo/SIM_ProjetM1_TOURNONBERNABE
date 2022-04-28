#version 330

// input uniforms 
uniform vec3 light;
uniform vec3 motion;

// in variables 
in vec3  normalView;
in vec3  eyeView;
//textures
in vec2 uvcoord;
in float h;
in float hWater;
in float hLimit;

// out buffers 
layout(location = 0) out vec4 bufferColor;

//textures variables
uniform sampler2D testMap, snowMap, lakeMap, rockMap, specularmap, aomap;

//Shader représentant le terrain. On applique des textures en fonction de la hauteur du terrain.
void main() {

  //Textures
  if(h > (1-hLimit)) { //Draw snow
    vec4 texColor = texture(snowMap,uvcoord);
    vec4 texspec = texture(snowMap,uvcoord);
    vec4 texao = texture(snowMap,uvcoord);
    float et  =  (texspec.x)*100;

    vec3 n = normalize(normalView);
    vec3 e = normalize(eyeView);
    vec3 l = normalize(light);

    float diff = max(dot(l,n),0.);
    float spec = pow(max(dot(reflect(l,n),e),0.0),et);

    bufferColor = texColor*(diff + spec)*2.0;
    bufferColor = bufferColor * texao;

  } else if(h > (-0.05-hLimit) ) { //Draw rock
    vec4 texColor = texture(rockMap,uvcoord);
    vec4 texspec = texture(rockMap,uvcoord);
    vec4 texao = texture(rockMap,uvcoord);
    float et  =  (texspec.x)*100;

    vec3 n = normalize(normalView);
    vec3 e = normalize(eyeView);
    vec3 l = normalize(light);

    float diff = max(dot(l,n),0.);
    float spec = pow(max(dot(reflect(l,n),e),0.0),et);

    bufferColor = texColor*(diff + spec)*2.0;
    bufferColor = bufferColor * texao;
  }
  else if( h > (-0.195 - (hLimit/40))){ //Draw grass
    vec4 texColor = texture(testMap,uvcoord);
    vec4 texspec = texture(testMap,uvcoord);
    vec4 texao = texture(testMap,uvcoord);
    float et  =  (texspec.x)*100;

    vec3 n = normalize(normalView);
    vec3 e = normalize(eyeView);
    vec3 l = normalize(light);

    float diff = max(dot(l,n),0.);
    float spec = pow(max(dot(reflect(l,n),e),0.0),et);

    bufferColor = texColor*(diff + spec)*2.0;
    bufferColor = bufferColor * texao;
  }
}
