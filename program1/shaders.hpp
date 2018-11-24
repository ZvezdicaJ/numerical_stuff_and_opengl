namespace shaders {

// an example of vertex shaders
// it accepts a single vertex (3D, 4D,... coordinate) and transforms it into
// something or it just set the gl_Position
static const GLchar *test_vertex_shader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

// an example of fragment shaders
//
static const GLchar *test_fragment_shader = R"(
#version 330 core
out vec4 FragColor;
void main()
{
FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";
} // namespace shaders
