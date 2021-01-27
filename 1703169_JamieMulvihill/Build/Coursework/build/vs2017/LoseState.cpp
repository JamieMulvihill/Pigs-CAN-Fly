#include "LoseState.h"
#include "scene_app.h"


/// initializer list for variable used in class
LoseState::LoseState(gef::Platform& p) :
	State(p)
{

}

LoseState::~LoseState()
{
}

/// function to initialize variables
void LoseState::Init(gef::InputManager* input_manager_, gef::AudioManager* audio_manager)
{
	loseTex = CreateTextureFromPNG("Textures/LoseScreen.png", platform_);
	audio_manager_ = audio_manager;
	audio_manager_->LoadMusic("GameSFX/LoseScreen.wav", platform_);
	audio_manager_->PlayMusic();
}

/// Render Function
void LoseState::Render(gef::SpriteRenderer * spriteRender, float width, float height, gef::Font * font)
{
	spriteRender->Begin();

	gef::Sprite background;
	background.set_texture(loseTex);
	background.set_position(gef::Vector4(width*0.50f, height*0.5f, -0.99f));
	background.set_height(height);
	background.set_width(456);
	spriteRender->DrawSprite(background);

	font->RenderText(
		spriteRender,
		gef::Vector4(width*0.52f, height - 150.f, -0.99f),
		2.0f,
		0xff000000,
		gef::TJ_CENTRE,
		" ");

	spriteRender->End();
}

void LoseState::Update(float frame_time, gef::InputManager * input_manager, State ** nextState)
{
	HandleInput(input_manager);
	score = SceneApp::getScore();
}

void LoseState::Release(gef::AudioManager* audio_manager)
{
	delete loseTex;
	loseTex = NULL;

	audio_manager->StopMusic();
	audio_manager->UnloadMusic();

}
void LoseState::HandleInput(gef::InputManager* input_manager_)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	if (input_manager_) {
		input_manager_->Update();
		gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
		if (controller_input) {
			const gef::SonyController* controller = controller_input->GetController(0);
			if (controller) {
				if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE) {
					SceneApp::SetState(new SplashScreen(platform_));
				}
			}
		}
	}
}
