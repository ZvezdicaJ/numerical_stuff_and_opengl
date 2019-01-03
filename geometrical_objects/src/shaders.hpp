namespace shaders {
// vertex shader
// it accepts a single vertex (3D, 4D,... coordinate) and transforms it into
// something or it just set the gl_Position

static const std::array<std::string, 3> uniform_vertex_shaders({
    R"(
#version 330 core

layout (location = 0) in vec2 aPos;

uniform mat4 transform;

void main()
{
gl_PointSize=30.0f;

gl_Position = transform*vec4(aPos,0.0f,1.0f);


}
)",

    R"(
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 transform;

void main()
{
gl_PointSize=30.0f;

gl_Position = transform*vec4(aPos, 1.0f);

}
)",

    R"(
#version 330 core

layout (location = 0) in vec4 aPos;

uniform mat4 transform;

void main()
{
gl_PointSize=10.0f;

gl_Position = transform*aPos;

}
)"

});

//  fragment shader
static const GLchar *uniform_fragment_shader = R"(
#version 330 core

out vec4 FragColor;

uniform vec4 color;

void main()
{
FragColor = color; //vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";

// namespace shaders

static const std::array<std::string, 3> custom_vertex_shaders({
    R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 CustomColor;

layout (location = 0) out vec4 Color;

uniform mat4 transform;

void main()
{
gl_PointSize=10.0f;

gl_Position = transform*vec4(aPos, 1.0f, 1.0f);

}
)",
    R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 CustomColor;

layout (location = 0) out vec4 Color;

uniform mat4 transform;

void main()
{
gl_PointSize=10.0f;

gl_Position = transform*vec4(aPos, 1.0f);

}
)",
    R"(
#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 CustomColor;

layout (location = 0) out vec4 Color;

uniform mat4 transform;

void main()
{
gl_PointSize=10.0f;

gl_Position = transform*aPos;

}
)"});

//  fragment shader
static const GLchar *custom_fragment_shader = R"(
#version 330 core

layout (location = 0) in  vec4 Color;

layout (location = 0) out vec4 FragColor;

void main()
{
FragColor = Color; //vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";

} // namespace shaders
