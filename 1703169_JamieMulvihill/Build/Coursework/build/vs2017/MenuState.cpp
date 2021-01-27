#include "MenuState.h"
#include "scene_app.h"

/// initializer list for variable used in class
MenuState::MenuState(gef::Platform& p) :
	State(p),
	valueLeft(0),
	valueTop(0),
	valueRight(960),
	scale(1.f),
	buttonMove_sfx_id(-1),
	buttonSelect_sfx_id(-1),
	inMenu(true),
	playSelected(false),
	colorChange(0x66008800)
{
	state_ = MENU;
	difficult = EASY;

}
MenuState::~MenuState()
{
}
void MenuState::Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager) {

	/// create and initialize textures
	spashScreen = CreateTextureFromPNG("Textures/EmptyBackground.png", platform_);
	playButton = CreateTextureFromPNG("Textures/play.png", platform_);
	controlsButton = CreateTextureFromPNG("Textures/controls.png", platform_);
	creditsButton = CreateTextureFromPNG("Textures/credits.png", platform_);
	hardBtn = CreateTextureFromPNG("Textures/HardBtn.png", platform_);
	normalBtn = CreateTextureFromPNG("Textures/NormalBtn.png", platform_);
	easyBtn = CreateTextureFromPNG("Textures/EasyBtn.png", platform_);

	/// store uint32 value of default colour
	defaultColor = playBtn.colour(); 

	/// initialize seleted button
	button = CONTROLS;

	/// set diffuculty variable to static state:: getDiffuculty
	difficult = State::getDifficulty(); 

	///initialize sounds
	audio_manager_ = audio_manager;
	volInfo.volume = 1;

	buttonMove_sfx_id = audio_manager_->LoadSample("GameSFX/KeySound.wav", platform_);
	audio_manager_->SetSampleVoiceVolumeInfo(buttonMove_sfx_id, volInfo);

	buttonSelect_sfx_id = audio_manager_->LoadSample("GameSFX/PigButtonSelect.wav", platform_);
	audio_manager_->SetSampleVoiceVolumeInfo(buttonSelect_sfx_id, volInfo);

}

void MenuState::Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font)
{
	spriteRenderer->Begin();

	gef::Sprite background;
	background.set_texture(spashScreen);
	background.set_position(gef::Vector4(width*0.50f, height*0.5f, -0.99f));
	background.set_height(height);
	background.set_width(width);
	spriteRenderer->DrawSprite(background);

	font->RenderText(
		spriteRenderer,
		gef::Vector4(width*0.5f, valueTop, -0.99f),
		2.0f,
		0xff000000,
		gef::TJ_CENTRE,
		"Main Menu");

	/// Render button icon
	playBtn.set_texture(playButton);
	playBtn.set_position(gef::Vector4(width*0.5f, height*0.5f, -0.99f));
	playBtn.set_height(180.0f);
	playBtn.set_width(180.0f);
	spriteRenderer->DrawSprite(playBtn);



	font->RenderText(
		spriteRenderer,
		gef::Vector4(width*0.5f, height*0.7f, -0.99f),
		1.0f,
		0x88008800,
		gef::TJ_CENTRE,
		"Play Game");

	/// Render button icon
	controlsBtn.set_texture(controlsButton);
	controlsBtn.set_position(gef::Vector4(width*0.25f, height*0.5f, -0.99f));
	controlsBtn.set_height(180.0f);
	controlsBtn.set_width(180.0f);
	spriteRenderer->DrawSprite(controlsBtn);

	font->RenderText(
		spriteRenderer,
		gef::Vector4(width*0.25f, height*0.7f, -0.99f),
		1.0f,
		0x88008800,
		gef::TJ_CENTRE,
		"Controls");

	creditBtn.set_texture(creditsButton);
	creditBtn.set_position(gef::Vector4(width*0.75f, height*0.5f, -0.99f));
	creditBtn.set_height(180.0f);
	creditBtn.set_width(180.0f);
	spriteRenderer->DrawSprite(creditBtn);

	font->RenderText(
		spriteRenderer,
		gef::Vector4(width*0.75f, height*0.7f, -0.99f),
		1.0f,
		0x88008800,
		gef::TJ_CENTRE,
		"Credits");

	/// if bool plySelected is true, render the extras buttons to select diffuclty
	if (playSelected) {
		easySprite.set_texture(easyBtn);
		easySprite.set_position(gef::Vector4(width*0.4f, height*0.75f, -0.99f));
		easySprite.set_height(90.0f);
		easySprite.set_width(90.0f);
		spriteRenderer->DrawSprite(easySprite);

		normalSprite.set_texture(normalBtn);
		normalSprite.set_position(gef::Vector4(width*0.5f, height*0.75f, -0.99f));
		normalSprite.set_height(90.0f);
		normalSprite.set_width(90.0f);
		spriteRenderer->DrawSprite(normalSprite);

		hardSprite.set_texture(hardBtn);
		hardSprite.set_position(gef::Vector4(width*0.6f, height*0.75f, -0.99f));
		hardSprite.set_height(90.0f);
		hardSprite.set_width(90.0f);
		spriteRenderer->DrawSprite(hardSprite);
	}
	spriteRenderer->End();
}

void MenuState::Update(float frame_time, gef::InputManager * i, State ** nextState)
{

	HandleInput(i);

	if (inMenu) {
		valueTop = Lerp(valueTop, 80, 0.05); // lerp value to make text fall from top of screen
		ButtonSelect();
		DifficultySelector();
	}

}

void MenuState::Release(gef::AudioManager* audio_manager)
{
	delete spashScreen;
	spashScreen = NULL;

	delete playButton;
	playButton = NULL;

	delete controlsButton;
	controlsButton = NULL;

	delete creditsButton;
	creditsButton = NULL;

	delete hardBtn;
	hardBtn = NULL;

	delete normalBtn;
	normalBtn = NULL;

	delete easyBtn;
	easyBtn = NULL;

	audio_manager->StopMusic();

	audio_manager->UnloadAllSamples();

	audio_manager->UnloadMusic();

}

void MenuState::HandleInput(gef::InputManager* input_manager_)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	if (input_manager_) {
		input_manager_->Update();
		gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
		if (controller_input) {
			const gef::SonyController* controller = controller_input->GetController(0);
			if (controller) {
				if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS){
					audio_manager_->PlaySample(buttonSelect_sfx_id);
					/// switch to control what happens when the cross button is pressed
					/// button input sets the state based on what has been selected
					switch (button) {
					case CONTROLS:
						SceneApp::SetState(new ControlsState(platform_));
						break;
					case PLAY:
						playSelected = true;
						button = DIFF;
						break;
					case SCORE:
						SceneApp::SetState(new CreditsState(platform_));
						break;
					case DIFF:
						State::SetDifficulty(difficult);
						SceneApp::SetState(new GameState(platform_));

					}
				}
				/// switch to control what happens when the left button is pressed
				else if (controller->buttons_pressed() & gef_SONY_CTRL_LEFT) {
					audio_manager_->PlaySample(buttonMove_sfx_id);
					if (playSelected) {
						switch (difficult) {
						case EASY:
							difficult = HARD;
							easySprite.set_colour(defaultColor);
							break;
						case NORMAL:
							difficult = EASY;
							normalSprite.set_colour(defaultColor);
							break;
						case HARD:
							difficult = NORMAL;
							hardSprite.set_colour(defaultColor);
							break;
						}
					}
					else {
						switch (button) {
						case CONTROLS:
							button = SCORE;
							controlsBtn.set_colour(defaultColor);
							break;
						case PLAY:
							button = CONTROLS;
							playBtn.set_colour(defaultColor);
							break;
						case SCORE:
							button = PLAY;
							creditBtn.set_colour(defaultColor);
							break;
						}
					}
				}
				/// switch to control what happens when the right button is pressed
				else if (controller->buttons_pressed() & gef_SONY_CTRL_RIGHT) {
					audio_manager_->PlaySample(buttonMove_sfx_id);
					if (playSelected) {
						switch (difficult) {
						case EASY:
							difficult = NORMAL;
							easySprite.set_colour(defaultColor);
							break;
						case NORMAL:
							difficult = HARD;
							normalSprite.set_colour(defaultColor);
							break;
						case HARD:
							difficult = EASY;
							hardSprite.set_colour(defaultColor);
							break;
						}
					}
					else {
						switch (button) {
						case CONTROLS:
							button = PLAY;
							controlsBtn.set_colour(defaultColor);
							break;
						case PLAY:
							button = SCORE;
							playBtn.set_colour(defaultColor);
							break;
						case SCORE:
							button = CONTROLS;
							creditBtn.set_colour(defaultColor);
							break;
						}
					}
				}
			}
		}
	}
}

/// function to change the color of the selected button, increases the alpha value of sprite color
void MenuState::ButtonColorChange(gef::Sprite * sprite)
{
	sprite->set_colour(colorChange);
	colorChange += 0x05000000;
}

/// function to select difficulty
void MenuState::DifficultySelector()
{
	switch (difficult) {
	case EASY:
		difficult = EASY;
		ButtonColorChange(&easySprite);
		break;
	case NORMAL:
		difficult = NORMAL;
		ButtonColorChange(&normalSprite);
		break;
	case HARD:
		difficult = HARD;
		ButtonColorChange(&hardSprite);
		break;
	}
}

/// function to change the color of the selected button, uses a switch statment to check current button slected
void MenuState::ButtonSelect()
{
	switch (button) {
	case CONTROLS:
		button = CONTROLS;
		ButtonColorChange(&controlsBtn);
		break;
	case PLAY:
		button = PLAY;
		ButtonColorChange(&playBtn);
		break;
	case SCORE:
		button = SCORE;
		ButtonColorChange(&creditBtn);
		break;
	}
}


