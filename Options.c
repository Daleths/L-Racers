#include "Main.h"

#include "Options.h"

MENUITEM gMI_P1Color = { "Player 1 Color:", 110, 78, TRUE, MenuFunc_Options_P1Color };

MENUITEM gMI_P2Color = { "Player 2 Color:", 110, 93, TRUE, MenuFunc_Options_P2Color };

MENUITEM gMI_MusicVolume = { "Music Volume:", 110, 108, TRUE, MenuFunc_Options_MusicVolume };

MENUITEM gMI_SFXVolume = { "SFX Volume:", 110, 123, TRUE, MenuFunc_Options_SFXVolume };

MENUITEM gMI_Fullscreen = { "Fullscreen:", 110, 138, TRUE, MenuFunc_Options_Fullscreen };

MENUITEM gMI_Back = { "Back", 110, 153, TRUE, MenuFunc_Options_Back };

MENUITEM* gMI_OptionsItems[] = { &gMI_P1Color, &gMI_P2Color, &gMI_MusicVolume, &gMI_SFXVolume, &gMI_Fullscreen, &gMI_Back };

MENU gMENU_Options = { "Options", 0, _countof(gMI_OptionsItems), gMI_OptionsItems };

void Draw_Options(void)
{
	memset(gBackBuffer.Memory, 0, RENDER_AREA_MEM_SIZE);

	static uint64_t LocalFrameCounter;

	static uint64_t LastFrameSeen;

	if (gTotalFramesRendered > (LastFrameSeen + 1))
	{
		LocalFrameCounter = 0;

		gMENU_Options.SelectedItem = 0;
	}

	Blit32BppBitmap(&gTopAccentPicture, 0, 0);

	// Rectangle around the title
	DrawRectangle(
		0,
		16,
		128,
		16,
		&FULL_GREEN_PIXEL,
		&BLACK_PIXEL,
		&DARK_GREEN_PIXEL,
		DR_HORIZONTALLY_CENTERED | DR_SHADOW | DR_THICK | DR_ROUNDED_CORNERS | DR_BORDERED);

	BlitString(
		"Options",
		&g6x7Font,
		&FULL_GREEN_PIXEL,
		(RENDER_WIDTH / 2) - ((uint16_t)(strlen("Options") * 6) / 2),
		20,
		&DARK_GREEN_PIXEL,
		BS_SHADOW);

	// Big rectangle to encompass all the various options
	DrawRectangle(
		0,
		0,
		192,
		104,
		&FULL_GREEN_PIXEL,
		&BLACK_PIXEL,
		&DARK_GREEN_PIXEL,
		DR_HORIZONTALLY_CENTERED | DR_VERTICALLY_CENTERED | DR_SHADOW | DR_THICK | DR_ROUNDED_CORNERS | DR_BORDERED);

	// Draw each menu item, if it's enabled.
	for (int i = 0; i < gMENU_Options.ItemCount; i++)
	{
		if (gMENU_Options.Items[i]->Enabled == TRUE)
		{
			BlitString(gMENU_Options.Items[i]->Name,
				&g6x7Font,
				&FULL_GREEN_PIXEL,
				gMENU_Options.Items[i]->x,
				gMENU_Options.Items[i]->y,
				&DARK_GREEN_PIXEL,
				BS_SHADOW);
		}
	}

	// Draw sfx and music volume sliders.
	for (int8_t Volume = 0; Volume < 10; Volume++)
	{
		if (Volume >= gSFXVolume)
		{
			BlitString("\xf2", &g6x7Font, &DARK_GRAY_PIXEL, 200 + (Volume * 6), gMI_SFXVolume.y, NULL, 0);
		}
		else
		{
			BlitString("\xf2", &g6x7Font, &FULL_GREEN_PIXEL, 200 + (Volume * 6), gMI_SFXVolume.y, NULL, 0);
		}
	}

	for (int8_t Volume = 0; Volume < 10; Volume++)
	{
		if (Volume >= gMusicVolume)
		{
			BlitString("\xf2", &g6x7Font, &DARK_GRAY_PIXEL, 200 + (Volume * 6), gMI_MusicVolume.y, NULL, 0);
		}
		else
		{
			BlitString("\xf2", &g6x7Font, &FULL_GREEN_PIXEL, 200 + (Volume * 6), gMI_MusicVolume.y, NULL, 0);
		}
	}

	// Player 1 color
	DrawRectangle(
		200,
		78,
		8,
		8,
		NULL,
		gAvailableColors[gPlayers[PLAYER_ONE].ColorIndex],
		NULL,
		DR_OPAQUE);

	// Player 2 color
	DrawRectangle(
		200,
		93,
		8,
		8,
		NULL,
		gAvailableColors[gPlayers[PLAYER_TWO].ColorIndex],
		NULL,
		DR_OPAQUE);

	if (gFullscreen)
	{
		BlitString("Enabled",
			&g6x7Font,
			&FULL_GREEN_PIXEL,
			gMI_Fullscreen.x + 90,
			gMI_Fullscreen.y,
			&DARK_GREEN_PIXEL,
			BS_SHADOW);
	}
	else
	{
		BlitString("Disabled",
			&g6x7Font,
			&FULL_GREEN_PIXEL,
			gMI_Fullscreen.x + 90,
			gMI_Fullscreen.y,
			&DARK_GREEN_PIXEL,
			BS_SHADOW);
	}

	// The menu cursor that shows which menu item is selected.
	BlitString("\xBB",
		&g6x7Font,
		&FULL_GREEN_PIXEL,
		gMENU_Options.Items[gMENU_Options.SelectedItem]->x - 6,
		gMENU_Options.Items[gMENU_Options.SelectedItem]->y,
		&DARK_GREEN_PIXEL,
		BS_SHADOW);

	LocalFrameCounter++;

	LastFrameSeen = gTotalFramesRendered;
}

void PPI_Options(void)
{
	if (gPlayers[PLAYER_ONE].Input.DownKeyIsDown && !gPlayers[PLAYER_ONE].Input.DownKeyWasDown)
	{
		PlayGameSound(&gMenuSound);

		if (gMENU_Options.SelectedItem == gMENU_Options.ItemCount - 1)
		{
			gMENU_Options.SelectedItem = 0;
		}
		else
		{
			gMENU_Options.SelectedItem++;
		}
	}
	else if (gPlayers[PLAYER_ONE].Input.UpKeyIsDown && !gPlayers[PLAYER_ONE].Input.UpKeyWasDown)
	{
		PlayGameSound(&gMenuSound);

		if (gMENU_Options.SelectedItem == 0)
		{
			gMENU_Options.SelectedItem = gMENU_Options.ItemCount - 1;
		}
		else
		{
			gMENU_Options.SelectedItem--;
		}
	}
	else if (gPlayers[PLAYER_ONE].Input.LeftKeyIsDown && !gPlayers[PLAYER_ONE].Input.LeftKeyWasDown)
	{
		if (strcmp(gMENU_Options.Items[gMENU_Options.SelectedItem]->Name, "Back") != 0)
		{
			PlayGameSound(&gMenuChooseSound);

			gMENU_Options.Items[gMENU_Options.SelectedItem]->Action();
		}	
	}
	else if (gPlayers[PLAYER_ONE].Input.RightKeyIsDown && !gPlayers[PLAYER_ONE].Input.RightKeyWasDown)
	{
		if (strcmp(gMENU_Options.Items[gMENU_Options.SelectedItem]->Name, "Back") != 0)
		{
			PlayGameSound(&gMenuChooseSound);

			gMENU_Options.Items[gMENU_Options.SelectedItem]->Action();
		}
	}
	else if (gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown && !gPlayers[PLAYER_ONE].Input.ChooseKeyWasDown)
	{
		if (gMENU_Options.Items[gMENU_Options.SelectedItem]->Enabled)
		{
			PlayGameSound(&gMenuChooseSound);

			gMENU_Options.Items[gMENU_Options.SelectedItem]->Action();
		}
	}
}

void MenuFunc_Options_MusicVolume(void)
{
	if ((gPlayers[PLAYER_ONE].Input.RightKeyIsDown && !gPlayers[PLAYER_ONE].Input.RightKeyWasDown) ||
		(gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown && !gPlayers[PLAYER_ONE].Input.ChooseKeyWasDown))
	{
		gMusicVolume++;

		if (gMusicVolume > 10)
		{
			gMusicVolume = 0;
		}
	}
	else
	{
		gMusicVolume--;

		if (gMusicVolume < 0)
		{
			gMusicVolume = 10;
		}
	}

	gXAudioMusicSourceVoice->lpVtbl->SetVolume(gXAudioMusicSourceVoice, ((float)gMusicVolume / 10), XAUDIO2_COMMIT_NOW);
}

void MenuFunc_Options_SFXVolume(void)
{
	if ((gPlayers[PLAYER_ONE].Input.RightKeyIsDown && !gPlayers[PLAYER_ONE].Input.RightKeyWasDown) ||
		(gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown && !gPlayers[PLAYER_ONE].Input.ChooseKeyWasDown))
	{
		gSFXVolume++;		

		if (gSFXVolume > 10)
		{
			gSFXVolume = 0;
		}
	}
	else
	{
		gSFXVolume--;

		if (gSFXVolume < 0)
		{
			gSFXVolume = 10;
		}
	}

	for (uint8_t Counter = 0; Counter < NUMBER_OF_SFX_SOURCE_VOICES; Counter++)
	{
		gXAudioSFXSourceVoice[Counter]->lpVtbl->SetVolume(gXAudioSFXSourceVoice[Counter], ((float)gSFXVolume / 10), XAUDIO2_COMMIT_NOW);
	}
}

void MenuFunc_Options_P1Color(void)
{
	if ((gPlayers[PLAYER_ONE].Input.RightKeyIsDown && !gPlayers[PLAYER_ONE].Input.RightKeyWasDown) ||
		(gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown && !gPlayers[PLAYER_ONE].Input.ChooseKeyWasDown))
	{
		gPlayers[PLAYER_ONE].ColorIndex++;

		if (gAvailableColors[gPlayers[PLAYER_ONE].ColorIndex]->Bytes == DUMMY_PIXEL.Bytes)
		{
			gPlayers[PLAYER_ONE].ColorIndex = 0;
		}
	}
	else
	{
		if (gPlayers[PLAYER_ONE].ColorIndex == 0)
		{
			do
			{
				gPlayers[PLAYER_ONE].ColorIndex++;

			} while (gAvailableColors[gPlayers[PLAYER_ONE].ColorIndex + 1]->Bytes != DUMMY_PIXEL.Bytes);
		}
		else
		{
			gPlayers[PLAYER_ONE].ColorIndex--;
		}
	}
}

void MenuFunc_Options_P2Color(void)
{
	if ((gPlayers[PLAYER_ONE].Input.RightKeyIsDown && !gPlayers[PLAYER_ONE].Input.RightKeyWasDown) ||
		(gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown && !gPlayers[PLAYER_ONE].Input.ChooseKeyWasDown))
	{
		gPlayers[PLAYER_TWO].ColorIndex++;

		if (gAvailableColors[gPlayers[PLAYER_TWO].ColorIndex]->Bytes == DUMMY_PIXEL.Bytes)
		{
			gPlayers[PLAYER_TWO].ColorIndex = 0;
		}
	}
	else
	{
		if (gPlayers[PLAYER_TWO].ColorIndex == 0)
		{
			do
			{
				gPlayers[PLAYER_TWO].ColorIndex++;

			} while (gAvailableColors[gPlayers[PLAYER_TWO].ColorIndex + 1]->Bytes != DUMMY_PIXEL.Bytes);
		}
		else
		{
			gPlayers[PLAYER_TWO].ColorIndex--;
		}
	}
}

void MenuFunc_Options_Fullscreen(void)
{
	gFullscreen = !gFullscreen;

	CalculateWindowPlacement();
}

void MenuFunc_Options_Back(void)
{
	gGameState = GAMESTATE_TITLESCREEN;
}