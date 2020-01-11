"#version 330 core\n"

"out vec4 FragColor;\n"

"in vec2 vTexCoord;\n"
"in vec3 vPosition;\n"
"in vec3 vNormal;\n"

"struct Material{\n"
"  sampler2D diffuse_texture;\n"
"  sampler2D specular_texture;\n"
"  sampler2D normal_texture;\n"
"  float ambient;\n"
"  float diffuse;\n"
"  float specular;\n"
"  float shininess;\n"
"};\n"

"struct Light{\n"
"  vec4 position;\n"
"};\n"

"uniform vec3 cameraPosition;\n"
"uniform Material material;\n"
"uniform Light light;\n"

"void main(){\n"
"  vec3 tex = vec3(texture(material.diffuse_texture, vTexCoord));\n"
"  vec3 ambient = vec3(material.ambient) * tex;\n"
// "  vec3 normal = normalize(vNormal);\n"
"  vec3 normal = normalize(texture(material.normal_texture, vTexCoord).rgb * 2.0 - 1.0);\n"
"  vec3 lightDir;\n"
// "  vec3 lightDir = normalize(light.position - vPosition);\n"
// "  vec3 lightDir = normalize(-light.direction);\n"
"  if(light.position.w < 0.01){\n"
"    lightDir = normalize(-vec3(light.position));\n"
"  }else if(light.position.w > 0.99){\n"
"    lightDir = normalize(vec3(light.position) - vPosition);\n"
"  }\n"
  /* diffuse */
"  vec3 diffuse = max(dot(normal, lightDir), 0.0) * vec3(material.diffuse) * tex;\n"
  /* specular */
"  vec3 viewDir = normalize(cameraPosition - vPosition);\n"
"  vec3 reflectDir = reflect(-lightDir, normal);\n"
"  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
"  vec3 specular = material.specular * spec * vec3(texture(material.specular_texture, vTexCoord));\n"
  /* attenuation */
"  if(light.position.w > 0.99){\n"
"    float distance = length(vec3(light.position) - vPosition);\n"
"    float attenuation = 1.0 / (1.0f + 0.09f * distance + 0.032f * (distance * distance));\n"
"    ambient *= attenuation;\n"
"    diffuse *= attenuation;\n"
"    specular *= attenuation;\n"
"  }\n"

  /* calculate fog */
"  float f = clamp(gl_FragCoord.z / gl_FragCoord.w / 150.0, 0.0, 1.0);\n"

"  FragColor = mix(vec4(ambient + diffuse + specular, 1.0), vec4(0.2, 0.2, 0.25, 1.0), f);\n"
"}"
