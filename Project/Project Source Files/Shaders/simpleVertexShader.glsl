#version 330

in vec3 vertex_position;
in vec3 vertex_normal;

out vec3 vertex_in_world;
out vec3 tnorm;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main(){
  
  vertex_in_world = vec3(model * vec4(vertex_position, 1.0));
  // mat4 ModelViewMatrix = view * model;
  // mat3 NormalMatrix =  mat3(ModelViewMatrix);
  // Convert normal and position to eye coords
  // Normal in view space
  // tnorm = normalize( NormalMatrix * vertex_normal);
  tnorm = mat3(transpose(inverse(model))) * vertex_normal;
  
  // Convert position to clip coordinates and pass along
  gl_Position = proj * view * model * vec4(vertex_position,1.0);

}


  