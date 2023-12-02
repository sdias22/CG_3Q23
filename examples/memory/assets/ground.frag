#version 300 es

precision mediump float;

in vec3 fragN;
in vec3 fragL;
in vec3 fragV;

// Light properties
uniform vec4 Ia, Id, Is;

// Material properties
uniform vec4 Ka, Kd, Ks;

out vec4 outColor;

vec4 Phong(vec3 N, vec3 L, vec3 V) {
  N = normalize(N);
  L = normalize(L);

  // Compute lambertian term
  float lambertian = max(dot(N, L), 0.0);

  // Compute specular term
  float specular = 0.0;
  if (lambertian > 0.0) {
    vec3 R = reflect(-L, N);
    V = normalize(V);
    float angle = max(dot(R, V), 0.0);
    specular = pow(angle, 25.0f);
  }

  vec4 diffuseColor = Kd * Id * lambertian;
  vec4 specularColor = Ks * Is * specular;
  vec4 ambientColor = Ka * Ia;

  return ambientColor + diffuseColor + specularColor;
}

void main() {
  vec4 colorAt = Phong(fragN, fragL, fragV);
  if (gl_FrontFacing) {
    outColor = colorAt * 0.75;
  } else {
    outColor = vec4(colorAt.r*0.3, colorAt.g *0.3, colorAt.b *0.3, 1.0);
  }
}