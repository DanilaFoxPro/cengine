"#version 330 core\n"

"layout (location = 0) in vec4 coord;\n"
"layout (location = 1) in int brightness;\n"
"layout (location = 2) in vec3 normal;\n"
"layout (location = 3) in vec2 texCoord;\n"

"out vec3 vPosition;\n"
"out float vBrightness;\n"
"out vec2 vTexCoord;\n"
"out vec3 vNormal;\n"
"out float vDiffuse;\n"

"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"uniform vec3 camera_position;\n"

"uniform vec3 light_direction = normalize(vec3(-0.9, 0.9, -0.1));\n"

"float calcLight(vec3 position, vec3 lightDir, vec3 normal){\n"
"  float diffuse = max(dot(normal, lightDir), 0.0);\n"
  /* specular */
"  vec3 viewDir = normalize(camera_position - position);\n"
"  vec3 reflectDir = reflect(-lightDir, normal);\n"
"  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);\n"

  "return diffuse + spec;\n"
"}\n"

"void main(){\n"
"  vPosition = vec3(model * vec4(coord.xyz, 1.0));\n"
"  vBrightness = float(brightness) * 0.1 + 1.0;\n"
"  vNormal = mat3(transpose(inverse(model))) * normal;\n"
"  vTexCoord = texCoord;\n"
// "  float sun = calcLight(vPosition, normalize(light_direction), normal);\n"
"  vDiffuse = max(0.0, dot(normal, light_direction));\n"

"  gl_Position = projection * view * vec4(vPosition, 1.0);\n"
"}"
