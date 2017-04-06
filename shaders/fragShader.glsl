#version 330 core



in vec3 vecN;
in vec3 vecL;
in vec3 vecP;

// Ouput data
out vec4 color;

uniform vec4 mat_ambient;
uniform vec4 mat_diffuse;
uniform vec4 mat_specular;
uniform float mat_shininess;

uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;


void main()
{


  vec4 AmbientProduct = light_ambient * mat_ambient;
  vec4 DiffuseProduct = light_diffuse * mat_diffuse;
  vec4 SpecularProduct = light_specular * mat_specular;

  //vec4 AmbientProduct = vec4(0.2, 0.2, 0.2, 1.0);
  //vec4 DiffuseProduct = vec4(0.5, 0.5, 0.5, 1.0);
  //vec4 SpecularProduct = vec4(0.5, 0.5, 0.5, 1.0);
  vec3 dirL = normalize(vecL);
  vec3 dirP = normalize(vecP);
  vec3 dirN = normalize(vecN);
  vec3 dirH = normalize( dirL + dirP );
  vec4 ambient = AmbientProduct;
  color = ambient;
  float Kd = max(dot(dirL, dirN), 0.0);
  vec4 diffuse = Kd*DiffuseProduct;
  float Ks = pow(max(dot(dirN, dirH), 0.0), 6.0);
  vec4 specular = Ks*SpecularProduct;
  float coeficient_s = max(0, dot(dirL, dirN));
  coeficient_s = normalize(coeficient_s); //coeficent_s = [0, 1]
  specular = coeficient_s * specular;
  color +=diffuse + specular;

  color.a = 1.0f;


}
