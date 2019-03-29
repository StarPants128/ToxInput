#include "emu.h"
#include "controller.h"
#include "checkbox.h"
#include "ingame.h"

void init()
{
	window.clear(color::bg3);
	window.display();

	sf::Image icon;
	icon.loadFromFile("icon.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	char path [MAX_PATH];
	GetWindowsDirectory(path, MAX_PATH);
	main_font.loadFromFile(std::string(path) + std::string("\\Fonts\\seguisb.ttf"));

	Controller::init();
	Emu::init();
	CheckBox::init();

	for (int i = 0; i < 4; i++)
		Controller::checkIfConnected(i);
}

inline void text(float x, float y, long long int num)
{
	sf::Text text(std::to_string(num), main_font);
	text.setCharacterSize(15);
	text.setFillColor(sf::Color::Red);
	text.setPosition(x, y);
	window.draw(text);
}

int main()
{
	init();

	sf::Clock updateTimer;

	while (window.isOpen())
	{
		UpdateEvents();

		Emu::update();
		Controller::update();
		CheckBox::update();

		if (IsGameCompatible(EmuHandle))
		{
			player0 = GetPlayer0();

			UpdateJoystickInput();
			UpdateRunWithoutY();
			Controller::UpdateRumble();

			if (!jit_on) jit_on = IsJITModeCertainlyOn();

			if (jit_on)
			{
				input_checkbox.flags |= Button::dead;
				y_checkbox.flags |= Button::dead;
			}
			else
			{
				input_checkbox.flags &= ~Button::dead;

				if (input_checkbox.flags & Button::checked)
					y_checkbox.flags &= ~Button::dead;
			}

			rumble_checkbox.flags &= ~Button::dead;
			camera_checkbox.flags &= ~Button::dead;
		}
		else
		{
			rumble_checkbox.flags |= Button::dead;
			camera_checkbox.flags |= Button::dead;
			input_checkbox.flags |= Button::dead;
			y_checkbox.flags |= Button::dead;

			player0 = 0;
			prevFrameCounter = 0xffffffff;
			jit_on = false;

			if (EmuHandle != NULL)
			{
				CloseHandle(EmuHandle);
				EmuHandle = NULL;
				Emus[Emu::selectedEmu].flags |= Button::dead;
			}
		}

		if (GetFrameCounter() < 60)
		{
			jit_on = false;
			prevFrameCounter = 0xffffffff;
		}

		oldPlayer0 = player0;

		if (gfxChangeTimer > 0)
		{
			window.display();
			window.clear(color::bg3);

			gfxChangeTimer--;
		}

		sf::sleep(sf::seconds(1./30.) - updateTimer.getElapsedTime());
		updateTimer.restart();
	}

	CloseEmu();

	return 0;
}
