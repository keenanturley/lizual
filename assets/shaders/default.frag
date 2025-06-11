#version 330 core
in vec2 texCoord;

out vec4 FragColor;

uniform float uTime;
uniform sampler2D uTexture;
uniform sampler2D uTexture2;
uniform float uMix;

const float PI = 3.1415926535897932384626433832795;

void main() {
  // Oscillates between 0 and 1 across x, y, and z, each with a different phase.
  vec3 oscillation = vec3(
    (sin(uTime) / 2.0f) + 0.5f,
    (sin(uTime - (PI / 2.0f)) / 2.0f) + 0.5f,
    (sin(uTime + (PI / 2.0f)) / 2.0f) + 0.5f
  );

  // Texcoord's distance from the center (0.5, 0.5)
  float dist = length(texCoord - vec2(0.5f, 0.5f));
  float radius = (oscillation.y * sqrt(2.0f)) / 2.0f;
  float alpha = ceil(radius - dist);

  vec4 texColor = texture(uTexture, texCoord);
  vec4 texColor2 = texture(uTexture2, texCoord);
  texColor2 = vec4(texColor2.rgb, texColor2.a * alpha * uMix);
  FragColor = vec4(mix(texColor.rgb, texColor2.rgb, texColor2.a), 1.0f);
}
