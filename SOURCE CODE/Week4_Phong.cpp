//#include <cstdio>
//#include <cstdlib>
#include <iostream>
#include <vector>

//include shape, shader header files
#include "GLerror.h"
#include "SDL_Start.h"
#include "Camera.h"
#include "Model.h"
#include "PlainModel.h"
#include "ModelLoaderClass.h"
#include "TextureClass.h"

#ifndef TEXTURECLASS_H
#define TEXTURECLASS_H
#ifndef SHADERCLASS_H
#define SHADERCLASS_H

// // GLEW - OpenGL Extension Wrangler - http://glew.sourceforge.net/
// // NOTE: include before SDL.h
#ifndef GLEW_H
#define GLEW_H
//#include <GL/glew.h>
#include "windows.h"

// SDL - Simple DirectMedia Layer - https://www.libsdl.org/
#ifndef SDL_H
#define SDL_H
#include "SDL.h"
#include "SDL_image.h"
//#include "SDL_mixer.h"
//#include "SDL_ttf.h"

// // - OpenGL Mathematics - https://glm.g-truc.net/
#define GLM_FORCE_RADIANS // force glm to use radians
// // NOTE: must do before including GLM headers
// // NOTE: GLSL uses radians, so will do the same, for consistency
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//***************
//variables
SDL_Event event;
SDL_Window *win;
bool windowOpen = true;
bool isFullScreen = false;
float bubbleSpeed = -0.001f;
float radius;

// Control of input variables
// degrees by which to adjust camera
float f = glm::radians(10.0f);

//screen boundaries for collision tests
float bX_r = 2.0f;
float bX_l = -2.0f;
float bY_t = 1.0f;
float bY_b = -1.0f;
//screen centre
float centreX = 0.0f;
float centreY = 0.0f;
//window aspect/width/height
int w = 1920;
int h = 1080;
float aspect;
int left;
int newwidth;
int newheight;

//transform matrices
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

glm::mat4 normalMatrix;

glm::mat4 translate;
glm::mat4 rotate;
glm::mat4 scale;
glm::vec3 b_scaleFactor;
glm::mat4 modelRotate;
glm::mat4 modelInitialRotate;
glm::mat4 modelScale;
glm::mat4 modelTranslate;

// second model
glm::mat4 modelBRotate;
glm::mat4 modelBInitialRotate;
glm::mat4 modelBScale;
glm::mat4 modelBTranslate;


glm::vec2 screenSize;

//create camera
Camera cam;
//loac camera variables
glm::vec3 camPos;
glm::vec3 camTarget;

bool flag = true;

glm::vec3 lightCol;
glm::vec3 lightPos;
float ambientIntensity;

//**************
//function prototypes
void handleInput();

int main(int argc, char *argv[]) {
	//start and initialise SDL
	SDL_Start sdl;
	SDL_GLContext context = sdl.Init();
	win = sdl.win;
	Uint32 isFullScreen = SDL_WINDOW_FULLSCREEN;
	//SDL_GetWindowSize(win, &w, &h);
	SDL_SetWindowSize(win, w, h);
	//SDL_SetWindowFullscreen(win, isFullScreen);
	glViewport(0, 0, w, h);
	aspect = (float)w / (float)h;

	//error class
	GLerror glerr;
	int errorLabel;

	//GLEW initialise
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	//register debug callback
	if (glDebugMessageCallback)
	{
		std::cout << "Registering OpenGL Debug callback function" << std::endl;
		glDebugMessageCallback(glerr.openglCallbackFunction, &errorLabel);
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			NULL,
			true);
	}

	//*****************************************************
	//OpenGL specific data

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glEnable(GL_CULL_FACE);

	//objects
	Model model;

	// B
	PlainModel modelB;

	//create model loader
	ModelImport modelLoader; 
	modelLoader.LoadOBJ2("Assets//Models//blenderSphere.obj", model.vertices, model.texCoords, model.normals, model.indices);

	modelLoader.LoadOBJ2("Assets//Models//blenderCube.obj", modelB.vertices, modelB.texCoords, modelB.normals, modelB.indices);


	errorLabel = 0;

	//*********************
	//create texture collection
	//create textures - space for 4, but only using 2
	Texture texArray[4];

	// unused tex
	texArray[0].load("Assets//Textures//bricks.png");
	texArray[0].setBuffers();
	// normal moon
	texArray[1].load("Assets//Textures//moon_day.jpg");
	texArray[1].setBuffers();
	// magma moon
	texArray[2].load("Assets//Textures//destroyed_moon.png");
	texArray[2].setBuffers();
	// perlin texture
	texArray[3].load("Assets//Textures//PerlinAlpha.png");
	texArray[3].setBuffers();

	errorLabel = 2;

	//OpenGL buffers
	model.setBuffers();
	modelB.setBuffers();

	errorLabel = 3;
	//*****************************************
	//set uniform variables
	int transformLocation;
	int modelLocation;
	// B
	int modelBLocation;

	int viewLocation;
	int projectionLocation;
	int importModelLocation;

	// B
	int importBModelLocation;
	int importViewLocation;
	int importProjectionLocation;
	int ambientIntensityLocation;
	int modelColourLocation;
	int modelAmbientLocation;

	// B
	int modelBColourLocation;
	int modelBAmbientLocation;


	int modelLightPosLocation;
	int normalMatrixLocation;
	int viewPositionLocation;
	int timeLocation;
	int srLocation;

	// texture uniforms
	int backTexLocation;

	int moonTexLocation;
	int lavaMoonTexLocation;
	int transitionTexLocation;

	float currentTime = 0.0f;

	float screen[2] = { w, h }; 
	std::cout << w << " " << h << std::endl;




	//light colour initial setting
	lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
	//light position
	lightPos = glm::vec3(0.0f, 10.0f, 10.0f);
	//light distance setting
	ambientIntensity = 1.0f;

	//initialise transform matrices 
	projectionMatrix = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 100.0f);
	//initialise view matrix to '1'
	viewMatrix = glm::mat4(1.0f);

	//backgroundScale = glm::mat4(1.0f);
	//backgroundTranslate = glm::mat4(1.0f);


	modelScale = glm::mat4(1.0f);
	modelRotate = glm::mat4(1.0f);
	modelTranslate = glm::mat4(1.0f);


	modelBScale = glm::mat4(1.0f);
	modelBRotate = glm::mat4(1.0f);
	modelBTranslate = glm::mat4(1.0f);



	//translate imported model?
	modelTranslate = glm::translate(modelTranslate, glm::vec3(0.0f, 0.0f, 0.0f));

	modelBTranslate = glm::translate(modelBTranslate, glm::vec3(1.0f, 0.0f, 0.0f));

	//once only scale to model
	modelScale = glm::scale(modelScale, glm::vec3(0.5f, 0.5f, 0.5f));

	modelBScale = glm::scale(modelScale, glm::vec3(0.1f, 0.1f, 0.1f));

	errorLabel = 4;

	//*****************************
	//'game' loop
	while (windowOpen)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//process inputs
		handleInput();
		cam.updateCamera();

		//time
		currentTime = SDL_GetTicks();


		errorLabel = 5;

		/* 
		//background
		glUseProgram(background.shaderProgram);
		backTexLocation = glGetUniformLocation(background.shaderProgram, "backTex");

		//set background lighting
		backgroundColourLocation = glGetUniformLocation(background.shaderProgram, "uLightColour");
		glProgramUniform3fv(background.shaderProgram, backgroundColourLocation, 1, glm::value_ptr(lightCol));
		//light distance
		ambientIntensityLocation = glGetUniformLocation(background.shaderProgram, "uAmbientIntensity");
		glProgramUniform1f(background.shaderProgram, ambientIntensityLocation, ambientIntensity);

		//set background image
		modelLocation = glGetUniformLocation(background.shaderProgram, "uModel");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(backgroundTranslate*backgroundScale));
		viewLocation = glGetUniformLocation(background.shaderProgram, "uView");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(cam.viewMatrix));
		projectionLocation = glGetUniformLocation(background.shaderProgram, "uProjection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		

		//screen resolution
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, texArray[0].texture);
		background.render();
		*/

		//tex model tex locations
		moonTexLocation = glGetUniformLocation(model.shaderProgram, "moonTex");
		lavaMoonTexLocation = glGetUniformLocation(model.shaderProgram, "lavaMoonTex");
		transitionTexLocation = glGetUniformLocation(model.shaderProgram, "transitionTex");

		//set .obj model
		glUseProgram(model.shaderProgram);

		//uniform bind for tex
		glUniform1i(moonTexLocation, 0);
		glUniform1i(lavaMoonTexLocation, 1);
		glUniform1i(transitionTexLocation, 2);

		//time
		timeLocation = glGetUniformLocation(model.shaderProgram, "uTime");
		glProgramUniform1f(model.shaderProgram, timeLocation, (float)currentTime);

		//set sphere lighting
		modelColourLocation = glGetUniformLocation(model.shaderProgram, "uLightColour");
		glProgramUniform3fv(model.shaderProgram, modelColourLocation, 1, glm::value_ptr(lightCol));
		//light position
		modelLightPosLocation = glGetUniformLocation(model.shaderProgram, "uLightPosition");
		glProgramUniform3fv(model.shaderProgram, modelLightPosLocation, 1, glm::value_ptr(lightPos));
		//light distance
		modelAmbientLocation = glGetUniformLocation(model.shaderProgram, "uAmbientIntensity");
		glProgramUniform1f(model.shaderProgram, modelAmbientLocation, ambientIntensity);




		modelRotate = glm::rotate(modelRotate, (float)0.4f / 1000, glm::vec3(0.1f, 0.9f, 0.0f));
		importModelLocation = glGetUniformLocation(model.shaderProgram, "uModel");
		glUniformMatrix4fv(importModelLocation, 1, GL_FALSE, glm::value_ptr(modelTranslate*modelRotate*modelScale));
		importViewLocation = glGetUniformLocation(model.shaderProgram, "uView");
		glUniformMatrix4fv(importViewLocation, 1, GL_FALSE, glm::value_ptr(cam.viewMatrix));
		importProjectionLocation = glGetUniformLocation(model.shaderProgram, "uProjection");
		glUniformMatrix4fv(importProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		normalMatrix = (glm::mat3)glm::transpose(glm::inverse(modelTranslate * modelRotate * modelScale));
		//set the normalMatrix in the shaders
		normalMatrixLocation = glGetUniformLocation(model.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		//set view position for specular component - use the camera position
		viewPositionLocation = glGetUniformLocation(model.shaderProgram, "uViewPosition");
		glProgramUniform3fv(model.shaderProgram, viewPositionLocation, 1, glm::value_ptr(cam.cameraPosition ));

		// bind moon, lava, and transition texture
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, texArray[1].texture);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, texArray[2].texture);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, texArray[3].texture);

		model.render();


		// MODEL B

		glUseProgram(modelB.shaderProgram);

		//time
		timeLocation = glGetUniformLocation(modelB.shaderProgram, "uTime");
		glProgramUniform1f(modelB.shaderProgram, timeLocation, (float)currentTime);

		//set sphere lighting
		modelBColourLocation = glGetUniformLocation(modelB.shaderProgram, "uLightColour");
		glProgramUniform3fv(modelB.shaderProgram, modelBColourLocation, 1, glm::value_ptr(lightCol));
		//light position
		modelLightPosLocation = glGetUniformLocation(modelB.shaderProgram, "uLightPosition");
		glProgramUniform3fv(modelB.shaderProgram, modelLightPosLocation, 1, glm::value_ptr(lightPos));
		//light distance
		modelBAmbientLocation = glGetUniformLocation(modelB.shaderProgram, "uAmbientIntensity");
		glProgramUniform1f(modelB.shaderProgram, modelBAmbientLocation, ambientIntensity);




		modelBRotate = glm::rotate(modelBRotate, (float)0.8f / 1000, glm::vec3(0.0f, 0.9f, 0.0f));
		importBModelLocation = glGetUniformLocation(modelB.shaderProgram, "uModel");
		glUniformMatrix4fv(importModelLocation, 1, GL_FALSE, glm::value_ptr(modelBTranslate * modelBRotate * modelBScale));
		importViewLocation = glGetUniformLocation(modelB.shaderProgram, "uView");
		glUniformMatrix4fv(importViewLocation, 1, GL_FALSE, glm::value_ptr(cam.viewMatrix));
		importProjectionLocation = glGetUniformLocation(modelB.shaderProgram, "uProjection");
		glUniformMatrix4fv(importProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		normalMatrix = (glm::mat3)glm::transpose(glm::inverse(modelBTranslate * modelBRotate * modelBScale));
		//set the normalMatrix in the shaders
		normalMatrixLocation = glGetUniformLocation(modelB.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		//set view position for specular component - use the camera position
		viewPositionLocation = glGetUniformLocation(modelB.shaderProgram, "uViewPosition");
		glProgramUniform3fv(modelB.shaderProgram, viewPositionLocation, 1, glm::value_ptr(cam.cameraPosition));

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, texArray[0].texture);

		modelB.render();





		SDL_GL_SwapWindow(sdl.win);
	}//end loop

	//****************************
	// Once finished with OpenGL functions, the SDL_GLContext can be deleted.
	SDL_GL_DeleteContext(context);

	SDL_Quit();
	return 0;
}

void handleInput()
{
	//*****************************
		//SDL handled input
		//Any input to the program is done here

	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			windowOpen = false;
		}
		if (event.type == SDL_WINDOWEVENT)
		{
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				std::cout << "Window resized w:" << w << " h:" << h << std::endl;
				SDL_GetWindowSize(win, &w, &h);
				newwidth = h * aspect;
				left = (w - newwidth) / 2;
				glViewport(left, 0, newwidth, h);
				break;
				

			default:
				break;
			}
		}
		
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			
			// -- LIGHT up/down movement
			case SDLK_UP:
				// rotate 'up' around centre 10 degrees
				lightPos = lightPos *
					glm::mat3(
						1.0, 0.0, 0.0,
						0.0, cos(f), -sin(f),
						0.0, sin(f), cos(f));
				break;

			case SDLK_DOWN:
				// rotate 'down' around centre 10 degrees
				lightPos = lightPos *
					glm::mat3(
						1.0, 0.0, 0.0,
						0.0, cos(-f), -sin(-f),
						0.0, sin(-f), cos(-f));
				break;
			
			case SDLK_a:
				// rotate cam around scene centre, left 10 degrees
				cam.cameraPosition = cam.cameraPosition *
					glm::mat3(
					cos(-f), 0, sin(-f),
					0, 1, 0,
					-sin(-f), 0, cos(-f));

				cam.camXPos = cam.cameraPosition.x;
				cam.camYPos = cam.cameraPosition.y;
				cam.camZPos = cam.cameraPosition.z;
				break;


			case SDLK_d:
				// rotate cam around scene centre, right 10 degrees
				cam.cameraPosition = cam.cameraPosition *
					glm::mat3(
						cos(f),  0.0,	sin(f),
						0.0,	  1.0,	0.0,
						-sin(f), 0.0,	cos(f));

				cam.camXPos = cam.cameraPosition.x;
				cam.camYPos = cam.cameraPosition.y;
				cam.camZPos = cam.cameraPosition.z;
				break;
			
			case SDLK_w:
				modelTranslate = glm::translate(modelTranslate, glm::vec3(0.0f, 0.1f, 0.0f));
				break;

			case SDLK_s:
				modelTranslate = glm::translate(modelTranslate, glm::vec3(0.0f, -0.1f, 0.0f));
				break;
			//case SDLK_p:
			//	//move camera 'forward' in the -ve z direction
			//	cam.camZPos -= cam.camSpeed;
			//	break;
			//case SDLK_l:
			//	//move camera 'backwards' in +ve z direction
			//	cam.camZPos += cam.camSpeed;
			//	break;



			case SDLK_1:
				lightCol = glm::vec3(0.0f, 0.0f, 1.0f);
				break;
			case SDLK_2:
				lightCol = glm::vec3(0.0f, 1.0f, 0.0f);
				break;
			case SDLK_0:
				lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
				break;
			
			}
		}
	}
}
#endif
#endif
#endif
#endif