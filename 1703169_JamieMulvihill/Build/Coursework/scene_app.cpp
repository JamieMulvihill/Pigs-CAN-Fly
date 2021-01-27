#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <iostream>
#include "GameState.h"

/// initializing staic variables outside the class
State * SceneApp::currentState = NULL;
gef::InputManager * SceneApp::input_manager_ = NULL;
gef::AudioManager * SceneApp::audio_manager = NULL;
int SceneApp::score = 0;

/// intializer list for variable sused in class
SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	primitive_builder_(NULL),
	font_(NULL)
{
}
void SceneApp::Init(){

	///initilaize the sprite renderer
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);

	InitFont();

	/// initialise input manager
	input_manager_ = gef::InputManager::Create(platform_);

	///initilize audio manager
	audio_manager = gef::AudioManager::Create();

	/// set state to the splash screen
	SetState(new SplashScreen(platform_));
}
void SceneApp::CleanUp(){
	SetState(NULL);

	delete input_manager_;
	input_manager_ = NULL;

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool SceneApp::Update(float frame_time){
	///call the update function of the current state
	currentState->Update(frame_time, input_manager_, &currentState);

	return true;
}
void SceneApp::Render(){
	/// call the render function of the current state
	currentState->Render(sprite_renderer_, platform_.width(), platform_.height(), font_);
}
/// function to set the state
void SceneApp::SetState(State * newState){
	///if the current state is not null
	if (currentState)
	{
		/// call the current state s release function and delete state 
		currentState->Release(audio_manager);
		delete currentState;
	}
	/// set the current state to the state passed as a parameter
	currentState = newState;

	///if the current state is not null, call the current states init function
	if (currentState)
		currentState->Init(input_manager_, audio_manager);
}
void SceneApp::InitFont(){
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}
void SceneApp::CleanUpFont(){
	delete font_;
	font_ = NULL;
}
