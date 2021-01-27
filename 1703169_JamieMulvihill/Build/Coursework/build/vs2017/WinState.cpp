#include "WinState.h"
#include "scene_app.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <thread>

WinState::WinState(gef::Platform& p) :
	State(p),
	score(0),
	leveledUP(false),
	finalScore(0),
	oneStar(3000),
	twoStar(23000),
	threeStar(43000),
	currentLevel(0)
{
}


WinState::~WinState()
{
}

void WinState::Init(gef::InputManager* input_manager_, gef::AudioManager* audio_manager)
{
	
	winTex = CreateTextureFromPNG("Textures/WinBG.png", platform_);

	starLevel = 0;
	
	/// storing the players final score
	finalScore = SceneApp::getScore();

	audio_manager_ = audio_manager;
	audio_manager_->LoadMusic("GameSFX/WinMusic.wav", platform_);
	audio_manager_->PlayMusic();

	scoreUp_sfx_id = audio_manager_->LoadSample("GameSFX/ScoreIncrease.wav", platform_);
	audio_manager_->SetSampleVoiceVolumeInfo(scoreUp_sfx_id, volInfo);

	starLevel_sfx_id = audio_manager_->LoadSample("GameSFX/LevelUpSound.wav", platform_);
	audio_manager_->SetSampleVoiceVolumeInfo(starLevel_sfx_id, volInfo);

	/// create animations
	CreateSpriteAnimation(starsArray, 4, "Animations/StarAnim/star", gef::Vector4(480, 250, -0.99f), 160, 280, currentStar);

}

void WinState::Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font)
{
	spriteRenderer->Begin();

	gef::Sprite background;
	background.set_texture(winTex);
	background.set_position(gef::Vector4(width*0.50f, height*0.5f, -0.99f));
	background.set_height(height);
	background.set_width(456);
	spriteRenderer->DrawSprite(background);

	///render the sprite in the array at the index equal to StarLevel
	spriteRenderer->DrawSprite(starsArray[starLevel]);

	font->RenderText(
		spriteRenderer,
		gef::Vector4(width*0.55f, height - 180.f, -0.99f),
		2.0f,
		0xff000000,
		gef::TJ_CENTRE,
		"%i", (int)score);

	spriteRenderer->End();
}

void WinState::Update(float frame_time, gef::InputManager * input_manager, State ** nextState)
{
	/// lerp values of the current screo the final score
	score = Lerp(score, finalScore+1, 0.01);
	if (score <= finalScore) {
		LevelUp();
		audio_manager_->PlaySample(scoreUp_sfx_id);
	}

	/// update the starLevel 
	starLevel = CalculateStars();

	HandleInput(input_manager);
}

void WinState::Release(gef::AudioManager* audio_manager)
{
	delete winTex;
	winTex = NULL;

	audio_manager_->StopMusic();
	audio_manager_->UnloadMusic();
	audio_manager_->UnloadAllSamples();
}


void WinState::HandleInput(gef::InputManager* input_manager_)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	if (input_manager_) {
		input_manager_->Update();
		gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
		if (controller_input) {
			const gef::SonyController* controller = controller_input->GetController(0);
			if (controller) {

				///set the state to the splash screen
				if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE) {
					SceneApp::SetState(new SplashScreen(platform_));
				}
			}
		}
	}
}

/// function to clalculate how mant stars player has got by checking score
int WinState::CalculateStars()
{
	int stars = 0;

	if (score < oneStar) {
		stars = 0;
		return stars;
	}
	else if (score > oneStar && score < twoStar) {
		stars = 1;
		return stars;
	}
	else if (score > twoStar && score < threeStar) {
		stars = 2;
		return stars;
	}
	else if (score > threeStar) {
		stars = 3;
		return stars;
	}
	else {
		return 0;
	}
}

/// function to play a specif sound if the starLevel changes 
void WinState::LevelUp() {
	///check if the int currentLevel is less than StarLevel,
	///play sounds and set currentLevel to be the starlevel
	if (currentLevel < starLevel) {
		audio_manager_->StopPlayingSampleVoice(scoreUp_sfx_id);
		audio_manager_->PlaySample(starLevel_sfx_id);
		currentLevel = starLevel;
	}
	
}

/// function to create animantion of sprites, uses the size int parameter to loop though and populate an array of sprites, converts the int i value from an int to string, 
/// adds this string to the name char* parameter, adds ".png" to name string, intializes a texture with name string, assigns texture to sprite in array at i index
void WinState::CreateSpriteAnimation(gef::Sprite * array, int size, const char * name, gef::Vector4 position, int height, int width, gef::Texture * texture) {

	for (int i = 0; i < size; i++) {
		array[i].set_position(position);
		array[i].set_width(width);
		array[i].set_height(height);

		int value = i;
		std::string s;
		std::stringstream out;
		out << value;
		s = out.str();

		std::string num = s;
		std::string temp = name;
		std::string extendeer = ".png";
		temp += num;
		temp += extendeer;
		texture = CreateTextureFromPNG(temp.c_str(), platform_);
		array[i].set_texture(texture);
	}
}