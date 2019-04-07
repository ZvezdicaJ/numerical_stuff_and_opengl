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

vec4 factor = vec4(0.5,0.5,1.0,1.0);

gl_Position = transform*(gl_in[0].gl_Position + vec4(-square_size, -square_size, 0.0, 1.0f)*factor);
EmitVertex();

gl_Position = transform*(gl_in[0].gl_Position + vec4( -square_size, square_size, 0.0, 1.0f)*factor);
EmitVertex();

gl_Position = transform*(gl_in[0].gl_Position + vec4( square_size, square_size, 0.0, 1.0f)*factor);
EmitVertex();

gl_Position = transform*(gl_in[0].gl_Position + vec4( square_size, -square_size, 0.0, 1.0f)*factor);
EmitVertex();

gl_Position = transform*(gl_in[0].gl_Position + vec4(-square_size, -square_size, 0.0, 1.0f)*factor);
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
  FragColor = vec4(0.5, 0.5, 0.5, 0.5);
}
)"});

///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

static const std::string ising_triangle_vertex_shaders({
    R"(
#version 450 core
layout (location = 0) in vec2 aPos;

layout (location = 0) out int spin_dir_out;

layout(std430, binding = 0) buffer spin_directions
 {
     int spin_dirs[];
 };

void main()
{
gl_PointSize=30.0f;

gl_Position = vec4(aPos, 0.0f, 1.0f);

spin_dir_out = spin_dirs[gl_VertexID];

}
)"});

static const std::string ising_triangle_geometry_shader({
    R"(
#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

layout (location=0) in int spin_dir[]; // Output from vertex shader for each vertex

layout (location=0) out int spin_dir_out; // Output to fragment shader

uniform float square_size;
uniform mat4 transform;

void main() {

vec4 factor = vec4(0.5,0.5,1.0,1.0);


gl_Position = transform*(gl_in[0].gl_Position + vec4(-square_size, square_size, 0.0, 1.0f)*factor);
EmitVertex();

gl_Position = transform*(gl_in[0].gl_Position + vec4( -square_size, -square_size, 0.0, 1.0f)*factor);
EmitVertex();

gl_Position = transform*(gl_in[0].gl_Position + vec4( square_size, square_size, 0.0, 1.0f)*factor);
EmitVertex();

gl_Position = transform*(gl_in[0].gl_Position + vec4( square_size, -square_size, 0.0, 1.0f)*factor);
EmitVertex();

EndPrimitive();

spin_dir_out=spin_dir[0];

}
)"});

//  fragment shader
// static const GLchar *
static const std::string ising_triangle_fragment_shader({R"(
#version 450 core
layout(location=0) in flat int spin_dir;

out vec4 FragColor;

void main()
{
if(spin_dir==0){
  FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
else{
  FragColor = vec4(0.5, 0.0, 0.0, 0.5);
}

}
)"});
