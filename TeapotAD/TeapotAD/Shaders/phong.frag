#version 430

in vec3 vertPos;
in vec3 N;
in vec3 lightPos;


//Materials
struct S_Mat
{
	//Diffuse
	vec3 Kd;
	
	//Specular
	vec3 Ks;

	//Ambient
	vec3 Ka;
};

//Lighting
struct S_Light// light intesity
{
	//Diffuse
	vec3 Ld;  
	
	//Specular
	vec3 Ls;

	//Ambient
	vec3 La;

	float atten_constant;
	float atten_linear;
	float atten_quad;
};

uniform S_Mat matterial;
uniform S_Light light;
uniform vec3 viewPos;

//Full Phong Shading
layout( location = 0 ) out vec4 FragColour;

//Diffuse Lighting
vec3 cal_diff(vec3 L)
{   vec3 diff = light.Ld * matterial.Kd * max(dot(N,L), 0.0); 

   return diff; 
}
   
//Ambient Lighting
vec3 cal_ambi()
{
	//Light Properties * Material Properties
   vec3 ambient = matterial.Ka * light.La;

   return ambient;
}

//Specular Lighting
vec3 cal_spec(vec3 L)
{
	//Controls Intensity
	vec3 viewDir = normalize(viewPos - vertPos);
	vec3 reflectDir = reflect(-L, N) + 0.12;

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = matterial.Ks * spec * light.Ls;

   return specular;
}


//Main
void main() {
   //Light Vector
   vec3 L = normalize(lightPos - vertPos);

   //Diffuse Calc
   vec3 light_diff = cal_diff(L);

   //Ambient Calc
   vec3 light_ambi = cal_ambi();

   //Specular Calc
   vec3 light_spec = cal_spec(L);

   // attenuation
   float distance = length(lightPos - vertPos);
   float atten = 1.0 / (light.atten_constant + light.atten_linear * distance + light.atten_quad * pow(distance, 2));
   
   // render
   FragColour = vec4(light_ambi * atten + light_diff * atten + light_spec * atten, 1.0); // this has to be ran last 

}  
