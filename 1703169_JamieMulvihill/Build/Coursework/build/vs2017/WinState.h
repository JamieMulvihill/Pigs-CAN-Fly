#pragma once
#include "State.h"

class WinState : public State
{
public:
	WinState(gef::Platform& p);
	~WinState();

	///functions to initialize, render, update and delete 
	void Init(gef::InputManager* input_manager_, gef::AudioManager* audio_manager);
	void Render(gef::SpriteRenderer * s, float width, float height, gef::Font * font);
	void Update(float frame_time, gef::InputManager * i, State ** nextState);
	void Release(gef::AudioManager* audio_manager);

	void SetScore(int finalScore) { score = finalScore; }

	/// function to clalculate how many stars player has got by checking score
	int CalculateStars();
	/// function to create an animation from an arry of sprites
	void CreateSpriteAnimation(gef::Sprite * array, int size, const char * name, gef::Vector4 position, int height, int width, gef::Texture * texture);
	/// function to play a specif sound if the starLevel changes 
	void LevelUp();
	void HandleInput(gef::InputManager* input_manager_);

	/// declaring variables for creating animations
	gef::Sprite starsArray[4];
	gef::Texture* currentStar;
	gef::Texture* winTex;

	/// ints used for changing the rendered sprite
	int starLevel, increment, oneStar, twoStar, threeStar, currentLevel;
	bool hasIncreased, leveledUP;
	float score, finalScore;
	Int32 scoreUp_sfx_id, starLevel_sfx_id;
};

