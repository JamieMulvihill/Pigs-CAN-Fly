#include "ControlsState.h"
#include "scene_app.h"
#include "GameState.h"

ControlsState::ControlsState(gef::Platform& p) :
	State(p)
{
	state_ = CONTROLS;
}


ControlsState::~ControlsState()
{
}

void ControlsState::Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager)
{
	background = CreateTextureFromPNG("Textures/NewControls.png", platform_);
}

/// function to Render sprites and controls of the game
void ControlsState::Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font)
{
	spriteRenderer->Begin();

	gef::Sprite backgroundSprite;
	backgroundSprite.set_texture(background);
	backgroundSprite.set_position(gef::Vector4(width*0.50f, height*0.5f, -0.99f));
	backgroundSprite.set_height(height);
	backgroundSprite.set_width(width);
	spriteRenderer->DrawSprite(backgroundSprite);

	spriteRenderer->End();
}

void ControlsState::Update(float frame_time, gef::InputManager * inputManager, State ** nextState)
{
	HandleInput(inputManager);
}

void ControlsState::Release(gef::AudioManager* audio_manager)
{
	delete background;
	background = NULL;
}
/// function to handle the input in state
void ControlsState::HandleInput(gef::InputManager* input_manager_)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	if (input_manager_) {
		input_manager_->Update();
		gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
		if (controller_input) {
			const gef::SonyController* controller = controller_input->GetController(0);
			if (controller) {
				/// change state to splash scrren, this is down by calling the static function SetState and giving it a new SplashScreen
				if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE){
					SceneApp::SetState(new SplashScreen(platform_));
				}
			}
		}
	}
}
