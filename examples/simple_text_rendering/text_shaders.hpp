namespace basic_text_shaders {
static const std::string vertex_shader({
    R"(
#version 450 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;
uniform mat4 projection;
void main()
{
gl_Position = projection * vec4(vertex.xy, 1.0, 1.0);
TexCoords = vertex.zw;
}
)"});

//  fragment shader
// static const GLchar *
static const std::string fragment_shader({R"(
#version 450 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
color = vec4(textColor, 1.0) * sampled;
}
)"});
} // namespace basic_text_shaders