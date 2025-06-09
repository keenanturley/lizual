#version 330 core
in vec3 ourColor;
in vec2 texCoord;

out vec4 FragColor;

uniform float uTime;
uniform sampler2D uTexture;
uniform sampler2D uTexture2;

const float PI = 3.1415926535897932384626433832795;

void main() {
  vec3 oscillation = vec3(sin(uTime) + 0.5f, sin(uTime - (PI / 2.0f)) + 0.5f, sin(uTime + (PI / 2.0f)) + 0.5f);
  vec4 textureColor = mix(texture(uTexture, texCoord), texture(uTexture2, texCoord), 0.2f);
  FragColor = textureColor * vec4(ourColor * oscillation, 1.0f);
}
