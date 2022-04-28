#version 330

//Activated == 1, 0 otherwise
#define FOG_EFFECT 0

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
//Shadow Mapping
in  vec4  shadcoord;
in  vec3  tangentView;

// out buffers 
layout(location = 0) out vec4 bufferColor;
//out vec4 outBuffer;

//textures variables
uniform sampler2D testMap, snowMap, lakeMap, rockMap, specularmap, aomap;
//Shadow Mapping
uniform sampler2D colormap, normalmap;
uniform sampler2DShadow shadowmap;


void main() {

  vec4 texColor;
  vec4 texspec;
  vec4 texao;


  //Textures
  if(h > (1-hLimit)) { //Draw snow
    texColor = texture(snowMap,uvcoord);
    texspec = texture(snowMap,uvcoord);
    texao = texture(snowMap,uvcoord);
    float et  =  (texspec.x)*100;

    vec3 n = normalize(normalView);
    vec3 e = normalize(eyeView);
    vec3 l = normalize(light);

    float diff = max(dot(l,n),0.);
    float spec = pow(max(dot(reflect(l,n),e),0.0),et);

    bufferColor = texColor*(diff + spec)*2.0;
    bufferColor = bufferColor * texao;

  } else if(h > (-0.05-hLimit) ) { //Draw rock
    texColor = texture(rockMap,uvcoord);
    texspec = texture(rockMap,uvcoord);
    texao = texture(rockMap,uvcoord);
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
    texColor = texture(testMap,uvcoord);
    texspec = texture(testMap,uvcoord);
    texao = texture(testMap,uvcoord);
    float et  =  (texspec.x)*100;

    vec3 n = normalize(normalView);
    vec3 e = normalize(eyeView);
    vec3 l = normalize(light);

    float diff = max(dot(l,n),0.);
    float spec = pow(max(dot(reflect(l,n),e),0.0),et);

    bufferColor = texColor*(diff + spec)*2.0;
    bufferColor = bufferColor * texao;
  
  }
  /*
  //Shadow mapping
  vec3 n = getModifiedNormal();
  vec3 e = normalize(eyeView);
  vec3 l = normalize(light);
  vec4 c = texture2D(colormap,uvcoord);

  float diff = max(dot(l,n),0.0);
  float spec = pow(max(dot(reflect(l,n),e),0.0),20.0);

  vec4 color = c*(diff+spec)*1.0;

  float v = 1.0;
  float b = 0.005;

  //Version simple (avec texture classique, sans PCF)
  if ( texture( shadowmap,shadcoord.xy ).z  <  shadcoord.z){
       v= 0.5;
  }
  
  outBuffer  = color*v;
  */
  #if FOG_EFFECT
  // on recupere ce qui se trouve dans le canal alpha (i.e. la profondeur)
  // on peut le modifier eventuellement, puis on clampe les valeurs entre 0 et 1
  float d = clamp(texspec.w*1.5,0.0,0.5);
  // couleur du brouillard
  vec4 fogColor = vec4(0.8,0.75,0.76,1.0);
  // on modifie la couleur avec la couleur du brouillard
  // en fonction de la profondeur (simple interpolation lineaire ici)
  bufferColor = mix(bufferColor,fogColor,d);
  #endif // FOG_EFFECT

}
