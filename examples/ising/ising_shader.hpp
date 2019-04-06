static const std::string ising_frame_vertex_shaders({
    R"(
#version 450 core
layout (location = 0) in vec2 aPos;

void main()
{
gl_PointSize=30.0f;

gl_Position = vec4(aPos, 0.0f, 1.0f);


}
)"});

static const std::string ising_frame_geometry_shader({
    R"(
#version 450 core
layout (points) in;
layout (line_strip, max_vertices = 5) out;

uniform float square_size;
uniform mat4 transform;

void main() {

gl_Position = gl_in[0].gl_Position + vec4(-square_size*0.5, -square_size*0.5, 0.0, 1.0f);
EmitVertex();

gl_Position = gl_in[0].gl_Position + vec4( -square_size*0.5, square_size*0.5, 0.0, 1.0f);
EmitVertex();

gl_Position = gl_in[0].gl_Position + vec4( square_size*0.5, square_size*0.5, 0.0, 1.0f);
EmitVertex();

gl_Position = gl_in[0].gl_Position + vec4( square_size*0.5, -square_size*0.5, 0.0, 1.0f);
EmitVertex();

gl_Position = gl_in[0].gl_Position + vec4(-square_size*0.5, -square_size*0.5, 0.0, 1.0f);
EmitVertex();

EndPrimitive();

}
)"});

//  fragment shader
// static const GLchar *
static const std::string ising_frame_fragment_shader({R"(
#version 450 core

out vec4 FragColor;

void main()
{
  FragColor = vec4(1, 1, 1, 1);
}
)"});

///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

static const std::string ising_vertex_shaders({
    R"(
#version 450 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in int spin_dir_in;

layout (location = 0) out int spin_dir_out;



void main()
{
gl_PointSize=30.0f;

gl_Position = vec4(aPos, 0.0f, 1.0f);

spin_dir_out=spin_dir_in;


}
)"});

static const std::string ising_geometry_shader({
    R"(
#version 450 core
layout (points) in;
layout (line_strip, max_vertices = 5) out;

uniform FL_FLOAT square_size
uniform mat4 transform;

void main() {

gl_Position = gl_in[0].gl_Position + vec4(-square_size/2, -square_size/2, 0.0, 1.0f);
EmitVertex();

gl_Position = gl_in[0].gl_Position + vec4( -square_size/2, square_size/2, 0.0, 1.0f);
EmitVertex();


gl_Position = gl_in[0].gl_Position + vec4( square_size/2, square_size/2, 0.0, 1.0f);
EmitVertex();

gl_Position = gl_in[0].gl_Position + vec4( square_size/2, -square_size/2, 0.0, 1.0f);
EmitVertex();

gl_Position = gl_in[0].gl_Position + vec4(-square_size/2, -square_size/2, 0.0, 1.0f);
EmitVertex();


EndPrimitive();
}
)"});

//  fragment shader
// static const GLchar *
static const std::string ising_fragment_shader({
    R"(
#version 450 core

layout (location = 0) in  int spin_dir_in;

out vec4 FragColor;

void main()
{

if(spin_dir_in == -1)
{
    FragColor = vec4(1, 1, 1, 1);
}
else
{
   FragColor = vec4(0, 0, 0, 0);
}

}
)"});
