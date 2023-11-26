#version 300 es

precision mediump float;

uniform vec4 color;
uniform mat4 viewMatrix;

//Light Properties
uniform vec3 lightPosition;
uniform vec4 Ia;
uniform vec4 Id;
uniform vec4 Is;
//Material properties
uniform vec4 Ka;
uniform vec4 Kd;
uniform vec4 Ks;

uniform samplerCube cubeTex;

in vec3 fragN;
in vec3 fragV;

out vec4 outColor;

vec4 BlinnPhong(vec3 N, vec3 V) {
  N = normalize(N);
  
  vec3 L = normalize((viewMatrix * vec4(lightPosition, 1.0)).xyz + V);

  // Compute lambertian term
  float lambertian = max(dot(N, L), 0.0);

  // Compute specular term
  float specular = 0.0;
  if (lambertian > 0.0) {
    V = normalize(V);
    vec3 H = normalize(L + V);
    float angle = max(dot(H, N), 0.0);
    specular = pow(angle, 25.0);
  }

  vec4 diffuseColor = Ka * Ia * lambertian;
  vec4 specularColor = Kd * Id * specular;
  vec4 ambientColor = Ks * Is; 

  return ambientColor + diffuseColor + specularColor;
}

void main() {
  if (gl_FrontFacing) {
    outColor = color * BlinnPhong(fragN, fragV);
  } else {
    //float i = (color.r + color.g + color.b) / 3.0;
    outColor = vec4(color.r * 0.2, color.g * 0.2, color.b *0.2, 1.0);
  }
}