#version 330 core

uniform sampler2D tex;

in vec3 pos; // In world space
in vec3 normal;
in vec2 uv;

out vec4 out_color;

void main()
{
  const float ambient = 0.4;

  vec3 light_dir = vec3(0.7, -0.7, 0.7);
  float diffuse = clamp(dot(light_dir, normal), 0.0, 1.0);

  vec3 t = texture(tex, uv).rgb;

  out_color = vec4(t * (ambient + diffuse), 1);
}
