#pragma once
#include "State.h"


class CreditsState : public State
{
public:
	CreditsState(gef::Platform& p);
	~CreditsState();

	///functions to initialize, render, update and delete 
	void Init(gef::InputManager* input_manager_, gef::AudioManager* audio_manager);
	void Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font);
	void Update(float frame_time, gef::InputManager * inputManager, State ** nextState);
	void Release(gef::AudioManager* audio_manager);
	
private:

	/// int used to increase the position of the sprites
	int positionChange;

	///	function to check the postion of the sprite and if within a range, return true
	bool RenderSprite(gef::Sprite sprite_);

	/// pivate functions and pointer for handeling input
	void HandleInput(gef::InputManager* input_manager_);

	UInt32 colorChange;

	/// textures used for rendering the credit sprites 
	gef::Texture * background;
	gef::Texture * creditProgram;
	gef::Texture * creditArt;
	gef::Texture * creditSound;
	gef::Texture * creditTextures;
	gef::Texture * creditGEF;
	gef::Texture * creditBox2d;
};


