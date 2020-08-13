#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "defines.h"
#include "scenediffuse.h"

#include <gtc/matrix_transform.hpp>
#include <gtx/transform2.hpp>

using std::cerr;
using std::endl;
using glm::vec3;

//This struct is setting the lighting intensity
struct S_Light
{
	//Diffusion
	vec3 Ld;
	
	//Ambient
	vec3 La;

	//Specular
	vec3 Ls;
} light_Bright;

namespace imat2908
{
//This is the contructor for the SceneDiffuse class
SceneDiffuse::SceneDiffuse()
{
}

//Intialising the scene
void SceneDiffuse::initScene(QuatCamera camera)
{
	//Compile and link the shader  
	compileAndLinkShader();

    gl::Enable(gl::DEPTH_TEST);

	light_Bright.La.x = 8.0; // set light brightness to 8 on the x
	light_Bright.La.y = 8.0; // set light brightness to 8 on the y
	light_Bright.La.z = 8.0; // set light brightness to 8 on the z

 	//Set up the lighting
	setLightParams(camera);


	//Create the plane to represent the ground
	plane = new VBOPlane(100.0,100.0,100,100);



	//A matrix to move the teapot lid upwards
	glm::mat4 lid = glm::mat4(1.0);
	//lid *= glm::translate(vec3(0.0,1.0,0.0));

	//Create the teapot with translated lid
	teapot = new VBOTeapot(16,lid);


}

void SceneDiffuse::update( float t )
{

}

void SceneDiffuse::setLightParams(QuatCamera camera)
{

	vec3 worldLight = vec3(10.0f,10.0f,10.0f);
   
	//Diffuse
	prog.setUniform("light.Ld", 0.9f, 0.9f, 0.9f);

	//Specular 
	prog.setUniform("light.La", light_Bright.La.x, light_Bright.La.y, light_Bright.La.z);

	//Ambient 
	prog.setUniform("light.Ls", 0.3f, 0.3f, 0.3f);
	
	prog.setUniform("LightPosition", worldLight );

	//Constant Parameter
	prog.setUniform("light.atten_constant", 0.5f);

	//Linear Parameter
	prog.setUniform("light.atten_linear", 0.08f);

	//Quadratic Parameter
	prog.setUniform("light.atten_quad", 0.09f);
}

void SceneDiffuse::lightChange(float Incre) // light change by "incre" ammount
{
	if (light_Bright.La.x <= 26.0 || light_Bright.La.x >= 0.0)
	{
		light_Bright.La = vec3(light_Bright.La.x + Incre, light_Bright.La.y + Incre, light_Bright.La.z + Incre); // Increase / decrease the the light by the incre at x,y and z
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Render the scene
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::render(QuatCamera camera)
{
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

	//First deal with the plane to represent the ground

	//Initialise the model matrix for the plane
	model = mat4(1.0f);
	//Set the matrices for the plane although it is only the model matrix that changes so could be made more efficient
    setMatrices(camera);
	//Set the plane's material properties in the shader and render
	prog.setUniform("matterial.Kd", 0.51f, 1.0f, 0.49f);
	prog.setUniform("matterial.Ka", 0.51f, 1.0f, 0.49f);
	prog.setUniform("matterial.Ks", 0.1f, 0.1f, 0.1f);
	plane->render();



	//Now set up the teapot 
	 model = mat4(1.0f);
	 setMatrices(camera);
	 //Set the Teapot material properties in the shader and render
	 prog.setUniform("matterial.Kd", 0.46f, 0.29f, 0.0f); 
	 prog.setUniform("matterial.Ka", 0.46f, 0.29f, 0.0f);
	 prog.setUniform("matterial.Ks", 0.29f, 0.29f, 0.29f);
	 teapot->render(); // render the teapot
	
}


/////////////////////////////////////////////////////////////////////////////////////////////
//Send the MVP matrices to the GPU
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::setMatrices(QuatCamera camera)
{

    mat4 mv = camera.view() * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", camera.projection() * mv);
	mat3 normMat = glm::transpose(glm::inverse(mat3(model)));// What does this line do?
	prog.setUniform("M", model);
	prog.setUniform("V", camera.view() );
	prog.setUniform("P", camera.projection() );
	prog.setUniform("viewPos", camera.position());
	
}

/////////////////////////////////////////////////////////////////////////////////////////////
// resize the viewport
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::resize(QuatCamera camera,int w, int h)
{
    gl::Viewport(0,0,w,h);
    width = w;
    height = h;
	camera.setAspectRatio((float)w/h);

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compile and link the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::compileAndLinkShader()
{
   
	try {
    	prog.compileShader("Shaders/phong.vert");
    	prog.compileShader("Shaders/phong.frag");
    	prog.link();
    	prog.validate();
    	prog.use();
    } catch(GLSLProgramException & e) {
 		cerr << e.what() << endl;
 		exit( EXIT_FAILURE );
    }
}

}
