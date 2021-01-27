#pragma once
#include "State.h"

class SplashScreen : public State
{
public:
	SplashScreen(gef::Platform& p);
	~SplashScreen();

	///functions to initialize, render, update and delete 
	void Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager);
	void Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font);
	void Update(float frame_time, gef::InputManager * inputManger, State ** nextState);
	void Release(gef::AudioManager* audio_manager);
	void HandleInput(gef::InputManager* input_manager_);

	/// textures used to draw splash screen
	gef::Texture* spashScreen;
	gef::Texture* flyingPiggy;

	/// soundefefect for button push
	Int32 buttonPush_sfx_id;

	/// uInts used for changing the colours of text on screen
	UInt32 textfade, textBlack;
	UInt8 alphaClear, alphaBlack;
	
	bool isWaiting;
	/// values used for lerping text positions 
	float valueLeft, valueTop, valueRight, scale;
};

