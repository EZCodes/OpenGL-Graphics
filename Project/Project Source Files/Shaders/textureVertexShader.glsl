#version 330

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 tex_coords;
in vec3 tangents;
in vec3 bitangents;

out vec2 TexCoords;
out vec3 vertex_in_world;
out vec3 tnorm;
out mat3 TBN;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

uniform int hasNormalsV = 0;


void main(){

  if (hasNormalsV == 1) {
	vec3 T = normalize(vec3(model * vec4(tangents,   0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangents, 0.0)));
    vec3 N = normalize(vec3(model * vec4(vertex_normal,    0.0)));
    mat3 TBN = mat3(T, B, N);
  }
  
  vertex_in_world = vec3(model * vec4(vertex_position, 1.0));
  // mat4 ModelViewMatrix = view * model;
  // mat3 NormalMatrix =  mat3(ModelViewMatrix);
  // Convert normal and position to eye coords
  // Normal in view space
  // tnorm = normalize( NormalMatrix * vertex_normal);
  tnorm = mat3(transpose(inverse(model))) * vertex_normal;
  
  // Convert position to clip coordinates and pass along
  gl_Position = proj * view * model * vec4(vertex_position,1.0);

  //Textures
  TexCoords = tex_coords;

}


  
