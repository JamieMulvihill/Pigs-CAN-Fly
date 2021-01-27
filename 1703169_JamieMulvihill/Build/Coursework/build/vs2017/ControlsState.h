#pragma once
#include "State.h"

class ControlsState : public State
{
public:
	ControlsState(gef::Platform& p);
	~ControlsState();

	///functions to initialize, render, update and delete 
	void Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager);
	void Update(float frame_time, gef::InputManager * inputManager, State ** nextState);
	void Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font);
	void Release(gef::AudioManager* audio_manager);

private:
	/// pivate functions and pointer for handeling input
	void HandleInput(gef::InputManager* input_manager_);
	gef::InputManager* input_manager_;

	gef::Texture * background;
};

