#include "GUI.h"

void GUI::Init(gef::Platform &platform, int numPlayers)
{

	/// create GUI textures
	GuiBar = CreateTextureFromPNG("Textures/DownGui.png", platform);
	GuiPig = CreateTextureFromPNG("Textures/NewPiggy.png", platform);

	

	/// loop to populate and initialize vector with sprites
	for (int i = 0; i < numPlayers - 1; i++) {
		sprites.push_back(new gef::Sprite);
		sprites[i]->set_texture(GuiPig);
		sprites[i]->set_height(pigH);
		sprites[i]->set_width(pigW);
		sprites[i]->set_position(gef::Vector4(60 + (i * offset), pigY, -0.99f));
	}

	/// initialize Lower screen gui
	Bar.set_texture(GuiBar);
	Bar.set_height(512.f);
	Bar.set_width(962.f);
	Bar.set_position(gef::Vector4(platform.width()*0.5f, platform.height() - 40, -0.99f));

	/// initialize values for Pig Height, pig Width, pig X value, pig Y value 
	pigH = 32;
	pigW = 32;
	pigX = platform.height();
	pigY = 540;

	offset = 50;
}

/// function to Render sprites and display score
void GUI::Render(gef::SpriteRenderer * sprite_renderer, float width, float height, gef::Font * font, int score)
{
	sprite_renderer->DrawSprite(Bar);

	for (int i = 0; i < sprites.size(); i++) {
		sprite_renderer->DrawSprite(*sprites[i]);
	}

	font->RenderText(
		sprite_renderer,
		gef::Vector4(width - 250, height - 35, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Score: %i", score);
}

/// function that returns a float, lerps values from a start value to an end value
float GUI::Lerp(float start, float end, float time)
{
	float lerpValue = start + (end - start) * time;

	return lerpValue;
}


void GUI::Update(float frame_time)
{
	PigBounce();
}

void GUI::Release()
{
	delete GuiBar;
	GuiBar = NULL;

	delete GuiPig;
	GuiPig = NULL;
}

/// function to Change the values of the pig Gui sprites height(PigH), width(PigW), X position and Y position(pigY),
/// loops through vecor of sprites and initailizes values
void GUI::PigBounce()
{
	if (pigH == 40 && pigW == 26) {
		isSqueased = true;
	}

	else if (pigH == 26 && pigW == 40) {
		isSqueased = false;

	}

	if (!isSqueased) {
		pigH = Lerp(pigH, 40.f, .7);
		pigW = Lerp(pigW, 26.f, .7);
		pigY = Lerp(pigY, pigX - 40, .4);
	}

	else if (isSqueased) {
		pigH = Lerp(pigH, 26.f, .7);
		pigW = Lerp(pigW, 40.f, .7);
		pigY = Lerp(pigY, pigX - 20, .4);
	}

	/// loops through vecor of sprites and initailizes values
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i]->set_height(pigH);
		sprites[i]->set_width(pigW);
		sprites[i]->set_position(gef::Vector4(60+(i * offset), pigY, -0.99f));
	}
}
