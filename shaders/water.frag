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

// out buffers 
layout(location = 0) out vec4 bufferColor;

//texture var
uniform sampler2D lakeMap;


void main() {

    vec4 texColor = texture(lakeMap,uvcoord);
    vec4 texspec = texture(lakeMap,uvcoord);
    vec4 texao = texture(lakeMap,uvcoord);
    float et  =  (texspec.x)*100;

    vec3 n = normalize(normalView);
    vec3 e = normalize(eyeView);
    vec3 l = normalize(light);

    float diff = max(dot(l,n),0.);
    float spec = pow(max(dot(reflect(l,n),e),0.0),et);

    bufferColor = texColor*(diff + spec)*2.0;
    bufferColor = bufferColor * texao;
}
