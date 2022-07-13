#include "Main.h"

#include "GamepadUnplugged.h"

void Draw_GamepadUnplugged(void)
{
	memset(gBackBuffer.Memory, 0, RENDER_AREA_MEM_SIZE);

	static uint64_t LocalFrameCounter;

	static uint64_t LastFrameSeen;

	if (gTotalFramesRendered > (LastFrameSeen + 1))
	{
		LocalFrameCounter = 0;
	}

	Blit32BppBitmap(&gTopAccentPicture, 0, 0);

	Blit32BppBitmap(&gXboxGamepadPicture, (RENDER_WIDTH / 2) - ((uint16_t)gXboxGamepadPicture.BitmapInfo.bmiHeader.biWidth / 2), 64);

	BlitString(
		"Gamepad Disconnected!",
		&g6x7Font,
		&FULL_GREEN_PIXEL,
		(RENDER_WIDTH / 2) - ((uint16_t)(strlen("Gamepad Disconnected!") * 6) / 2),
		20,
		&DARK_GREEN_PIXEL,
		BS_SHADOW);

	DrawRectangle(
		0,
		16,
		160,
		16,
		&FULL_GREEN_PIXEL,
		&BLACK_PIXEL,
		&DARK_GREEN_PIXEL,
		DR_HORIZONTALLY_CENTERED | DR_SHADOW | DR_THICK | DR_ROUNDED_CORNERS | DR_BORDERED);

	BlitString(
		"A gamepad was disconnected. Plug it back in to continue,",
		&g6x7Font,
		&FULL_GREEN_PIXEL,
		30,
		130,
		&DARK_GREEN_PIXEL,
		BS_SHADOW);

	BlitString(
		"or press Enter to continue with just the keyboard.",
		&g6x7Font,
		&FULL_GREEN_PIXEL,
		45,
		145,
		&DARK_GREEN_PIXEL,
		BS_SHADOW);

	LocalFrameCounter++;

	LastFrameSeen = gTotalFramesRendered;
}

void PPI_GamepadUnplugged(void)
{
	FindConnectedGamepads();

	if (gGamepadsPresent == gGamepadsExpected)
	{
		gGameState = gPreviousGameState;

		return;
	}

	if (gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown && !gPlayers[PLAYER_ONE].Input.ChooseKeyWasDown)
	{
		gGameState = gPreviousGameState;

		gGamepadsExpected = gGamepadsPresent;
	}
}