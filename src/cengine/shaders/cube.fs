"#version 330 core\n"

"out vec4 FragColor;\n"

"in float vBrightness;\n"
"in vec2 vTexCoord;\n"

"uniform sampler2D texture1;\n"

"void main(){\n"
"  FragColor = texture(texture1, vTexCoord) * vec4(vec3(vBrightness), 1.0);\n"
"}"
