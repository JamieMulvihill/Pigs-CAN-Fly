#pragma once
#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/sprite_renderer.h>
#include <input/input_manager.h>
#include <box2d/Box2D.h>
#include <input/sony_controller_input_manager.h>
#include <graphics/renderer_3d.h>
#include <graphics/font.h>
#include <graphics/sprite.h>
#include "load_texture.h"
#include <audio/audio_manager.h>

class State
{


public:
	
	enum GAMESTATE { MENU, PLAYLEVEL, CONTROLS, WIN, LOSE };

	enum DIFFICULTY { EASY, NORMAL, HARD };
	
	GAMESTATE state_;
	State(gef::Platform &p);
	/// virtual functions to be overiddenby child classes
	virtual void Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager) {}
	virtual void Update(float frame_time, gef::InputManager * inputManager, State ** nextState) {}
	virtual void Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font) {}
	virtual void HandleInput(gef::InputManager* input_manager_) {}
	virtual void Release(gef::AudioManager* audio_manager) {}

	///getters and setters for settig difficulty
	static void SetDifficulty(DIFFICULTY currentDiff) { difficulty = currentDiff; }
	static DIFFICULTY getDifficulty() { return difficulty; }
	float Lerp(float start, float end, float time);

protected:
	gef::Platform  &platform_;
	static DIFFICULTY difficulty;
	gef::AudioManager* audio_manager_;
	gef::VolumeInfo volInfo;
};
