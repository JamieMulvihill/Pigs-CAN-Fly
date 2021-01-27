#include "CreditsState.h"
#include "scene_app.h"


/// initializer list for variable used in class
CreditsState::CreditsState(gef::Platform& p) :
	State(p),
	positionChange(0),
	colorChange(0xff00000)
{
}


CreditsState::~CreditsState()
{
}


void CreditsState::Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager)
{
	background = CreateTextureFromPNG("Textures/CreditsScreen.png", platform_);
	creditArt = CreateTextureFromPNG("Credits/3dCredit.png", platform_);
	creditProgram = CreateTextureFromPNG("Credits/ProgrammingCredit.png", platform_);
	creditSound = CreateTextureFromPNG("Credits/SoundCredit.png", platform_);
	creditTextures = CreateTextureFromPNG("Credits/TextureCredit.png", platform_);
	creditBox2d = CreateTextureFromPNG("Credits/Box2dCredit.png", platform_);
	creditGEF = CreateTextureFromPNG("Credits/GefCredit.png", platform_);

}

/// function to Render sprites and controls of the game
void CreditsState::Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font)
{
	spriteRenderer->Begin();

	gef::Sprite backgroundSprite;
	backgroundSprite.set_texture(background);
	backgroundSprite.set_position(gef::Vector4(width*0.50f, height*0.5f, -0.99f));
	backgroundSprite.set_height(height);
	backgroundSprite.set_width(width);
	spriteRenderer->DrawSprite(backgroundSprite);

	gef::Sprite artCreditSprite;
	artCreditSprite.set_texture(creditArt);
	artCreditSprite.set_position(gef::Vector4(width*0.50f, 460 - positionChange/3, -0.99f));
	artCreditSprite.set_height(128);
	artCreditSprite.set_width(512);
	if (RenderSprite(artCreditSprite)) {
		spriteRenderer->DrawSprite(artCreditSprite);
	}

	gef::Sprite texCreditSprite;
	texCreditSprite.set_texture(creditTextures);
	texCreditSprite.set_position(gef::Vector4(width*0.50f, 520 - positionChange/3, -0.99f));
	texCreditSprite.set_height(128);
	texCreditSprite.set_width(512);
	if (RenderSprite(texCreditSprite)) {
		spriteRenderer->DrawSprite(texCreditSprite);
	}

	gef::Sprite proCreditSprite;
	proCreditSprite.set_texture(creditProgram);
	proCreditSprite.set_position(gef::Vector4(width*0.50f, 580 - positionChange/3, -0.99f));
	proCreditSprite.set_height(128);
	proCreditSprite.set_width(512);
	if (RenderSprite(proCreditSprite)) {
		spriteRenderer->DrawSprite(proCreditSprite);
	}


	gef::Sprite soundCreditSprite;
	soundCreditSprite.set_texture(creditSound);
	soundCreditSprite.set_position(gef::Vector4(width*0.50f, 640 - positionChange/3, -0.99f));
	soundCreditSprite.set_height(128);
	soundCreditSprite.set_width(512);
	if (RenderSprite(soundCreditSprite)) {
		spriteRenderer->DrawSprite(soundCreditSprite);
	}

	gef::Sprite gefCreditSprite;
	gefCreditSprite.set_texture(creditGEF);
	gefCreditSprite.set_position(gef::Vector4(width*0.50f, 700 - positionChange / 3, -0.99f));
	gefCreditSprite.set_height(128);
	gefCreditSprite.set_width(512);
	if (RenderSprite(gefCreditSprite)) {
		spriteRenderer->DrawSprite(gefCreditSprite);
	}
	
	gef::Sprite b2dCreditSprite;
	b2dCreditSprite.set_texture(creditBox2d);
	b2dCreditSprite.set_position(gef::Vector4(width*0.50f, 760 - positionChange/3, -0.99f));
	b2dCreditSprite.set_height(128);
	b2dCreditSprite.set_width(512);
	if (RenderSprite(b2dCreditSprite)) {
		spriteRenderer->DrawSprite(b2dCreditSprite);
	}
	

	spriteRenderer->End();
}

void CreditsState::Update(float frame_time, gef::InputManager * inputManager, State ** nextState)
{
	HandleInput(inputManager);
	positionChange++;
}

void CreditsState::Release(gef::AudioManager* audio_manager)
{
	delete background;
	background = NULL;

	delete creditProgram;
	creditProgram = NULL;

	delete creditArt;
	creditArt = NULL;
	
	delete creditSound;
	creditSound = NULL;

	delete creditTextures;
	creditTextures = NULL;

	delete creditGEF;
	creditGEF = NULL;

	delete creditBox2d;
	creditBox2d = NULL;

}
bool CreditsState::RenderSprite(gef::Sprite sprite_)
{

	if (sprite_.position().y() > 155 && sprite_.position().y() < 450) {
		return true;
	}
	else
		return false;

	
}
/// function to handle the input in state
void CreditsState::HandleInput(gef::InputManager* input_manager_)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	if (input_manager_) {
		input_manager_->Update();
		gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
		if (controller_input) {
			const gef::SonyController* controller = controller_input->GetController(0);
			if (controller) {
				/// change state to splash screen, this is down by calling the static function SetState and giving it a new SplashScreen
				if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE) {
					SceneApp::SetState(new SplashScreen(platform_));
				}
			}
		}
	}
}
