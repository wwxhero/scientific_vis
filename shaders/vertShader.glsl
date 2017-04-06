#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition; // Vertex position in model space
layout(location = 1) in vec3 vertexNormal; //direction of normal



// Values that stay constant for the whole mesh.
uniform mat4 model2world;
uniform mat4 world2view;
uniform mat4 view2clip;
uniform mat3 model2view;

uniform vec4 lightPos;

out vec3 vecN;
out vec3 vecP;
out vec3 vecL;

void main()
{
  vec3 pos = (world2view * model2world * vec4(vertexPosition,1)).xyz;
  vecN = normalize(model2view * vertexNormal);
  vecL = -(world2view * lightPos).xyz;
  vecP = -pos;
  // Output position of the vertex, in clip space : MVP * position

  gl_Position =  view2clip * world2view * model2world * vec4(vertexPosition,1);

}

