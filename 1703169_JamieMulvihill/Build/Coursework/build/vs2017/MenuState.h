#pragma once
#include "State.h"
#include "GameState.h"
#include "ControlsState.h"
#include "CreditsState.h"

class MenuState : public State
{
public:
	
	/// enum for selected button
	enum BUTTONS{ CONTROLS, PLAY, SCORE, DIFF };
	
	MenuState(gef::Platform& p);
	~MenuState();

	/// enum variable, enum inherited from State class
	DIFFICULTY difficult;
	
	/// enum variable of buttons
	BUTTONS button;

	///functions to initialize, render, update and delete 
	void Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager);
	void Render(gef::SpriteRenderer * sprite_renderer, float width, float height, gef::Font * font);
	void Update(float frame_time, gef::InputManager * input_manager, State ** nextState);
	void Release(gef::AudioManager* audio_manager);

private:

	void HandleInput(gef::InputManager* input_manager_);           
	void ButtonSelect();                                 /// function to change the color of the selected button, uses a switch statement to check current button slected
	void ButtonColorChange(gef::Sprite * sprite);        /// function to change the color of the selected button, increases the alpha value of sprite color
	void DifficultySelector();							/// function to select a difficulty, uses a switch statement check difficulty enum variable

	/// texture* variables
	gef::Texture* spashScreen;
	gef::Texture* playButton;
	gef::Texture* controlsButton;
	gef::Texture* creditsButton;
	gef::Texture* hardBtn;
	gef::Texture* easyBtn;
	gef::Texture* normalBtn;

	/// variables for sound effects
	Int32 buttonMove_sfx_id, buttonSelect_sfx_id;

	/// variables for colors
	UInt32 colorChange, defaultColor;

	bool inMenu, playSelected;
	float valueLeft, valueTop, valueRight, scale;
	gef::Sprite controlsBtn, playBtn, creditBtn, easySprite, normalSprite, hardSprite;
};

