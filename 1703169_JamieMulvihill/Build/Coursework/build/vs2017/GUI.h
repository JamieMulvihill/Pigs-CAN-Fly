#pragma once
#include "load_texture.h"
#include <graphics/font.h>
#include <graphics/sprite.h>
#include <system/application.h>
#include <maths/vector2.h>
#include <vector>
#include <graphics/sprite_renderer.h>



class GUI
{
public:
	///functions to initialize, render, update and delete 
	void Init(gef::Platform &platform, int numPlayers);
	void Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font, int score);
	void Update(float frame_time);
	void Release();

	/// function to make sprite bounce
	void PigBounce();

	/// function for lerping values
	float Lerp(float start, float end, float time);
	/// vector of Sprite* 
	std::vector<gef::Sprite*> sprites;
private:
	gef::Sprite Bar;
	gef::Sprite Pig;
	gef::Texture* GuiBar;
	gef::Texture* GuiPig;

	int numPlayers, offset, random;
	float pigH, pigW, pigX, pigY;

	/// bool to check wether a sprite is stretched or squeased
	bool isSqueased = false;
};

