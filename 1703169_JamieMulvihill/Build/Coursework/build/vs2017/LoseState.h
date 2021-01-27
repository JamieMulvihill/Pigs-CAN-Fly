#pragma once
#include "State.h"


class LoseState : public State
{
public:
	LoseState(gef::Platform& p);
	~LoseState();

	///functions to initialize, render, update and delete 
	void Init(gef::InputManager* input_manager_, gef::AudioManager* audio_manager);
	void Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font);
	void Update(float frame_time, gef::InputManager * inputManager, State ** nextState);
	void Release(gef::AudioManager* audio_manager);
	void HandleInput(gef::InputManager* input_manager_);

	/// fucntion to set score
	void SetScore(int finalScore) { score = finalScore; }

private:
	gef::Texture* loseTex;
	int score;
};

