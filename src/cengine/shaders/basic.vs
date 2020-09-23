"#version 330 core\n"

"layout (location = 0) in vec2 position;\n"

"uniform mat4 model;\n"

"void main(){\n"
"  gl_Position = model * vec4(position, 0.0, 1.0);\n"
"}"
