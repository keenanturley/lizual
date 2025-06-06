#version 330 core
in vec3 ourColor;

out vec4 FragColor;

uniform float uTime;

const float PI = 3.1415926535897932384626433832795;

void main() {
  vec3 oscillation = vec3(sin(uTime) + 0.5f, sin(uTime - (PI / 2.0f)) + 0.5f, sin(uTime + (PI / 2.0f)) + 0.5f);
  FragColor = vec4(ourColor * oscillation, 1.0f);
}
