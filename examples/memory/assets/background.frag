#version 300 es

precision mediump float;

in vec3 fragTexCoord;

out vec4 outColor;

uniform samplerCube m_Tex;

void main() { outColor = texture(m_Tex, fragTexCoord); }