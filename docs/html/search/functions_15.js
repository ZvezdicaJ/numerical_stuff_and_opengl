var searchData=
[
  ['vertex_5fshader',['vertex_shader',['../namespacebasic__text__shaders.html#a3a1e289b8d23fe7bbc727f210d18a3d2',1,'basic_text_shaders::vertex_shader({ R&quot;(
#version 450 core
layout (location = 0) in vec4 vertex; // &lt;vec2 pos, vec2 tex&gt;
out vec2 TexCoords;
uniform mat4 projection;
void main()
{
gl_Position = projection * vec4(vertex.xy, 1.0, 1.0);
TexCoords = vertex.zw;
}
)&quot;})'],['../namespacebasic__text__shaders.html#a3a1e289b8d23fe7bbc727f210d18a3d2',1,'basic_text_shaders::vertex_shader({ R&quot;(
#version 450 core
layout (location = 0) in vec4 vertex; // &lt;vec2 pos, vec2 tex&gt;
out vec2 TexCoords;
uniform mat4 projection;
void main()
{
gl_Position = projection * vec4(vertex.xy, 1.0, 1.0);
TexCoords = vertex.zw;
}
)&quot;})']]]
];
