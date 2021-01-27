#include "SplashScreen.h"
#include "MenuState.h"
#include "scene_app.h"

///intializer list to set values
SplashScreen::SplashScreen(gef::Platform& p) :
	State(p),
	valueLeft(0),
	valueTop(0),
	valueRight(960),
	scale(1.f),
	buttonPush_sfx_id(-1),
	isWaiting(true),
	textfade(0x01000000)
{
	state_ = MENU;
}
SplashScreen::~SplashScreen()
{
}
void SplashScreen::Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager) {

	spashScreen = CreateTextureFromPNG("Textures/EmptyBackground.png", platform_);
	flyingPiggy = CreateTextureFromPNG("Textures/NewSplash.png", platform_);

	audio_manager_ = audio_manager;
	audio_manager_->LoadMusic("GameSFX/MenuTheme.wav", platform_);

	volInfo.volume = 1;
	buttonPush_sfx_id = audio_manager_->LoadSample("GameSFX/PigButtonSelect.wav", platform_);
	audio_manager_->SetSampleVoiceVolumeInfo(buttonPush_sfx_id, volInfo);

	audio_manager_->PlayMusic();
}

/// function to render the prites and text to screen for the SplashScreen
void SplashScreen::Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font)
{
	spriteRenderer->Begin();

	gef::Sprite background;
	background.set_texture(spashScreen);
	background.set_position(gef::Vector4(width*0.50f, height*0.5f, -0.99f));
	background.set_height(height);
	background.set_width(width);
	spriteRenderer->DrawSprite(background);

	gef::Sprite splash;
	splash.set_texture(flyingPiggy);
	splash.set_position(gef::Vector4(width*0.50f, height*0.5f, -0.99f));
	splash.set_height((height / 4)* scale);
	splash.set_width((width / 4)* scale);
	spriteRenderer->DrawSprite(splash);

	font->RenderText(
		spriteRenderer,
		gef::Vector4(valueLeft, 100, -0.99f),
		2.0f,
		0xff000000,
		gef::TJ_CENTRE,
		"PIGS ");

	font->RenderText(
		spriteRenderer,
		gef::Vector4(width*0.5f, valueTop, -0.99f),
		2.0f,
		0xff000000,
		gef::TJ_CENTRE,
		"CAN");

	font->RenderText(
		spriteRenderer,
		gef::Vector4(valueRight, 100, -0.99f),
		2.0f,
		0xff000000,
		gef::TJ_CENTRE,
		"FLY!");

	font->RenderText(
		spriteRenderer,
		gef::Vector4(width*0.5f, height*0.75f + 32.0f, -0.99f),
		1.f,
		textfade,
		gef::TJ_CENTRE,
		"Press any Button to Continue");

	spriteRenderer->End();
}

void SplashScreen::Update(float frame_time, gef::InputManager * input_manager, State ** nextState)
{
	HandleInput(input_manager);

	textfade += 0x02000000; /// change the fade value for for the text colour

	/// if player has not selected to continue lerp values and adjust sprites accordingliy
	if (isWaiting) {
		valueLeft = Lerp(valueLeft, (960 * 0.5f) - 120, 0.05);
		valueTop = Lerp(valueTop, 100, 0.05);
		valueRight = Lerp(valueRight, (960 * 0.5f) + 120, 0.05);
		scale = Lerp(scale, 5.4, 0.0015);
	}
}

void SplashScreen::Release(gef::AudioManager* audio_manager)
{
	delete spashScreen;
	spashScreen = NULL;

	delete flyingPiggy;
	flyingPiggy = NULL;
}

void SplashScreen::HandleInput(gef::InputManager* input_manager_)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	if (input_manager_) {
		input_manager_->Update();
		gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
		if (controller_input) {
			const gef::SonyController* controller = controller_input->GetController(0);
			if (controller) {
				if (controller->buttons_pressed()) {
					/// set bool to false
					/// play sound effect
					///set state to MenuState
					isWaiting = false;
					audio_manager_->PlaySample(buttonPush_sfx_id);
					SceneApp::SetState(new MenuState(platform_));
				}
			}
		}
	}
}
