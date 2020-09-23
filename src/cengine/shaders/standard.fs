"#version 330 core\n"
"#define POINT_LIGHTS 4\n"

"out vec4 FragColor;\n"

"in vec2 vTexCoord;\n"
"in vec3 vPosition;\n"
"in vec3 vNormal;\n"
"in mat3 vTBN;\n"

"struct Material{\n"
"  sampler2D diffuse_texture;\n"
"  sampler2D specular_texture;\n"
"  sampler2D normal_texture;\n"
"  float ambient;\n"
"  float diffuse;\n"
"  float specular;\n"
"  float shininess;\n"
"};\n"

"struct DirLight{\n"
"  vec3 direction;\n"
"};\n"

"struct PointLight{\n"
"  vec3 position;\n"
"  vec3 color;\n"
"};\n"

"uniform vec3 cameraPosition;\n"
"uniform Material material;\n"
"uniform DirLight dirLight;\n"
"uniform PointLight pointLights[POINT_LIGHTS];\n"

"vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){\n"
"  vec3 lightDir = normalize(-light.direction);\n"
"  float diff = max(dot(normal, lightDir), 0.0);\n"

"  vec3 reflectDir = reflect(-lightDir, normal);\n"
"  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"

"  vec3 ambient = vec3(material.ambient) * vec3(texture(material.diffuse_texture, vTexCoord));\n"
"  vec3 diffuse = vec3(material.diffuse) * diff * vec3(texture(material.diffuse_texture, vTexCoord));\n"
"  vec3 specular = vec3(material.specular) * spec * vec3(texture(material.diffuse_texture, vTexCoord));\n"

"  return ambient + diffuse + specular;\n"
"}\n"

"vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir){\n"
"  vec3 lightDir = normalize(light.position - vPosition);\n"
"  float diff = max(dot(normal, lightDir), 0.0);\n"

"  vec3 reflectDir = reflect(-lightDir, normal);\n"
"  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"

"  float distance = length(light.position - vPosition);\n"
"  float attenuation = 1.0 / (1.0f + 0.09f * distance + 0.032f * (distance * distance));\n"

"  vec3 ambient = light.color * vec3(material.ambient) * vec3(texture(material.diffuse_texture, vTexCoord));\n"
"  vec3 diffuse = light.color * vec3(material.diffuse) * diff * vec3(texture(material.diffuse_texture, vTexCoord));\n"
"  vec3 specular = light.color * vec3(material.specular) * spec * vec3(texture(material.diffuse_texture, vTexCoord));\n"

"  ambient *= attenuation;\n"
"  diffuse *= attenuation;\n"
"  specular *= attenuation;\n"

"  return ambient + diffuse + specular;\n"
"}\n"

"void main(){\n"
"  vec3 normal = normalize(texture(material.normal_texture, vTexCoord).rgb);\n"
"  normal = normalize(normal * 2.0 - 1.0);\n"
"  normal = normalize(vTBN * normal);\n"

"  vec3 viewDir = normalize(cameraPosition - vPosition);\n"

"  vec3 result = CalcDirLight(dirLight, normal, viewDir);\n"
"  for(int i = 0; i < POINT_LIGHTS; i++){\n"
"    result += CalcPointLight(pointLights[i], normal, viewDir);\n"
"  }\n"

  /* calculate fog */
"  float f = clamp(length(cameraPosition - vPosition) / 150.0 - 0.1, 0.0, 1.0);\n"

// "  FragColor = vec4(normal, 1.0);\n"
"  FragColor = mix(vec4(result, 1.0), vec4(0.2, 0.2, 0.25, 1.0), f);\n"
"}"
