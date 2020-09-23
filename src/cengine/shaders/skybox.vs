"#version 330 core\n"

"layout (location = 0) in vec3 position;\n"

"out vec3 vCoord;\n"

"uniform mat4 projection;\n"
"uniform mat4 view;\n"

"void main(){\n"
"  gl_Position = (projection * mat4(mat3(view))) * vec4(position, 1.0);\n"
"  vCoord = position;\n"
"}"
