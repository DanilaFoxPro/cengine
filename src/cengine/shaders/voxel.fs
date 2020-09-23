"#version 330 core\n"

"in vec3 vPosition;\n"
"in float vBrightness;\n"
"in vec2 vTexCoord;\n"
"in vec3 vNormal;\n"
"in float vDiffuse;\n"

"uniform sampler2D diffuse_texture;\n"
"uniform float daylight = 0.5;\n"

"void main(){\n"
"  vec3 color = texture(diffuse_texture, vTexCoord).rgb;\n"
"  if(color == vec3(1.0, 0.0, 1.0)){\n"
"    discard;\n"
"  }\n"

"  vec3 normal = normalize(vNormal);"

"  float ambient = daylight * 0.9;"
"  vec3 light = vec3(max(max(0.01, daylight * 0.08), ambient * vDiffuse) * vBrightness);"

"  float f = pow(clamp(gl_FragCoord.z / gl_FragCoord.w / 1000, 0, 0.8), 2);\n"

"  gl_FragColor = vec4(mix(color * light, vec3(0.53, 0.81, 0.92) * daylight, f), 1.0);\n"
"}"
