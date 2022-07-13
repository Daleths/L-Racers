#include "Main.h"

#include "TitleScreen.h"

MENUITEM gMI_1PlayerVsCPU = { "1 Player vs CPU", 144, 80, TRUE, MenuFunc_TitleScreen_1PVsCPU };

MENUITEM gMI_2PlayerLocal = { "2 Player Local", 144, 95, TRUE, MenuFunc_TitleScreen_2PLocal };

MENUITEM gMI_HostNetworkGame = { "Host Network Game", 144, 110, TRUE, MenuFunc_TitleScreen_HostLAN };

MENUITEM gMI_JoinNetworkGame = { "Join Network Game", 144, 125, TRUE, MenuFunc_TitleScreen_JoinLAN };

MENUITEM gMI_Options = { "Options", 144, 140, TRUE, MenuFunc_TitleScreen_Options };

MENUITEM gMI_Exit = { "Exit", 144, 155, TRUE, MenuFunc_TitleScreen_Exit };

MENUITEM* gMI_TitleScreenItems[] = { &gMI_1PlayerVsCPU, &gMI_2PlayerLocal, &gMI_HostNetworkGame, &gMI_JoinNetworkGame, &gMI_Options, &gMI_Exit };

MENU gMENU_TitleScreen = { "Title Screen", 0, _countof(gMI_TitleScreenItems), gMI_TitleScreenItems };

const char* gMarquee = "Welcome to L-RACERS! Player 1 may use either the WASD keys or the arrow keys \
to move. If playing a 2-Player Local game, Player 1 uses the WASD keys and Player 2 uses \
the arrow keys. Both players may also use the D-pads on XInput-compatible gamepads. You can change the \
color of your player in the Options menu. Only players on the Local Area Network \
will appear in the list of available hosts when attempting to join a multiplayer game. If you're \
trying to play with someone over the internet, you need to manually enter their IP address. \
Firewalls may prevent packets from reaching their destination. This game listens on UDP ports 9657 and 9658. \
In multiplayer games, the host will always be on the left side. \
So... thanks for playing! L-RACERS was created by Joseph Ryan Ries in 2022 as an homage to retro gaming.";

void Draw_TitleScreen(void)
{
	memset(gBackBuffer.Memory, 0, RENDER_AREA_MEM_SIZE);

	static uint64_t LocalFrameCounter;

	static uint64_t LastFrameSeen;

	static uint16_t MarqueeCursor;

	static uint16_t MarqueeCharsShown;

	static char MarqueeSubstring[65];

	if (gTotalFramesRendered > (LastFrameSeen + 1))
	{
		LocalFrameCounter = 0;

		gMENU_TitleScreen.SelectedItem = 0;

		MarqueeCursor = 0;

		MarqueeCharsShown = 0;

		memset(MarqueeSubstring, 0, sizeof(MarqueeSubstring));

		gInputDisabled = TRUE;
	}

	if (LocalFrameCounter == 10)
	{
		gInputDisabled = FALSE;
	}

	if (LocalFrameCounter > 120)
	{
		if (LocalFrameCounter % 8 == 0)
		{
			if (MarqueeCharsShown < 64)
			{
				MarqueeCharsShown++;
			}
			else
			{
				if (MarqueeCursor < strlen(gMarquee))
				{				
					MarqueeCursor++;
				}				
			}

			strncpy_s(MarqueeSubstring, sizeof(MarqueeSubstring), gMarquee + MarqueeCursor, MarqueeCharsShown);
		}

		BlitString(
			MarqueeSubstring,
			&g6x7Font,
			&DARK_GRAY_PIXEL,
			RENDER_WIDTH - (MarqueeCharsShown * 6),
			RENDER_HEIGHT - 8,
			NULL,
			0);
	}	

	Blit32BppBitmap(&gTopAccentPicture, 0, 0);

	BlitString(
		"L-RACERS",
		&g6x7Font,
		&FULL_GREEN_PIXEL,
		(RENDER_WIDTH / 2) - ((uint16_t)(strlen("L-RACERS") * 6) / 2),
		20,
		&DARK_GREEN_PIXEL,
		BS_SHADOW);

	DrawRectangle(
		0,
		16,
		128,
		16,
		&FULL_GREEN_PIXEL,
		&BLACK_PIXEL,
		&DARK_GREEN_PIXEL,
		DR_HORIZONTALLY_CENTERED | DR_SHADOW | DR_THICK | DR_ROUNDED_CORNERS | DR_BORDERED);

	DrawRectangle(
		0,
		0,
		128,
		96,
		&FULL_GREEN_PIXEL,
		&BLACK_PIXEL,
		&DARK_GREEN_PIXEL,
		DR_HORIZONTALLY_CENTERED | DR_VERTICALLY_CENTERED | DR_SHADOW | DR_THICK | DR_ROUNDED_CORNERS | DR_BORDERED);


	for (int i = 0; i < gMENU_TitleScreen.ItemCount; i++)
	{
		if (gMENU_TitleScreen.Items[i]->Enabled == TRUE)
		{
			BlitString(gMENU_TitleScreen.Items[i]->Name,
				&g6x7Font,
				&FULL_GREEN_PIXEL,
				gMENU_TitleScreen.Items[i]->x,
				gMENU_TitleScreen.Items[i]->y,
				&DARK_GREEN_PIXEL,
				BS_SHADOW);
		}
	}

	BlitString("\xBB",
		&g6x7Font,
		&FULL_GREEN_PIXEL,
		gMENU_TitleScreen.Items[gMENU_TitleScreen.SelectedItem]->x - 6,
		gMENU_TitleScreen.Items[gMENU_TitleScreen.SelectedItem]->y,
		&DARK_GREEN_PIXEL,
		BS_SHADOW);

	LocalFrameCounter++;

	LastFrameSeen = gTotalFramesRendered;
}

void PPI_TitleScreen(void)
{
	if (gPlayers[PLAYER_ONE].Input.DownKeyIsDown && !gPlayers[PLAYER_ONE].Input.DownKeyWasDown)
	{
		PlayGameSound(&gMenuSound);

		if (gMENU_TitleScreen.SelectedItem == gMENU_TitleScreen.ItemCount - 1)
		{
			gMENU_TitleScreen.SelectedItem = 0;
		}
		else
		{
			gMENU_TitleScreen.SelectedItem++;
		}
	}
	else if (gPlayers[PLAYER_ONE].Input.UpKeyIsDown && !gPlayers[PLAYER_ONE].Input.UpKeyWasDown)		
	{
		PlayGameSound(&gMenuSound);

		if (gMENU_TitleScreen.SelectedItem == 0)
		{
			gMENU_TitleScreen.SelectedItem = gMENU_TitleScreen.ItemCount - 1;
		}
		else
		{
			gMENU_TitleScreen.SelectedItem--;
		}
	}
	else if (gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown && !gPlayers[PLAYER_ONE].Input.ChooseKeyWasDown)
	{
		if (gMENU_TitleScreen.Items[gMENU_TitleScreen.SelectedItem]->Enabled)
		{
			PlayGameSound(&gMenuChooseSound);

			gMENU_TitleScreen.Items[gMENU_TitleScreen.SelectedItem]->Action();
		}
	}
}

void MenuFunc_TitleScreen_1PVsCPU(void)
{
	snprintf(
		gPlayers[PLAYER_ONE].Name,
		sizeof(gPlayers[PLAYER_ONE].Name),
		"Player 1");

	snprintf(
		gPlayers[PLAYER_TWO].Name,
		sizeof(gPlayers[PLAYER_TWO].Name),
		"CPU");

	gPlayers[PLAYER_TWO].AI = TRUE;

	ResetEverythingForNewGame();

	gGameState = GAMESTATE_ACTIONSCREEN;
}

void MenuFunc_TitleScreen_2PLocal(void)
{
	snprintf(
		gPlayers[PLAYER_ONE].Name,
		sizeof(gPlayers[PLAYER_ONE].Name),
		"Player 1");

	snprintf(
		gPlayers[PLAYER_TWO].Name,
		sizeof(gPlayers[PLAYER_TWO].Name),
		"Player 2");

	gPlayers[PLAYER_TWO].AI = FALSE;	

	ResetEverythingForNewGame();

	gGameState = GAMESTATE_ACTIONSCREEN;
}

void MenuFunc_TitleScreen_HostLAN(void)
{
	gPlayers[PLAYER_TWO].AI = FALSE;

	gNetworkData.IsHost = TRUE;

	memset(gPlayers[PLAYER_ONE].Name, 0, sizeof(gPlayers[PLAYER_ONE].Name));

	memset(gPlayers[PLAYER_TWO].Name, 0, sizeof(gPlayers[PLAYER_TWO].Name));

	gGameState = GAMESTATE_ENTERYOURNAME;
}

void MenuFunc_TitleScreen_JoinLAN(void)
{
	gPlayers[PLAYER_TWO].AI = FALSE;

	gNetworkData.IsHost = FALSE;

	memset(gPlayers[PLAYER_ONE].Name, 0, sizeof(gPlayers[PLAYER_ONE].Name));

	memset(gPlayers[PLAYER_TWO].Name, 0, sizeof(gPlayers[PLAYER_TWO].Name));

	gGameState = GAMESTATE_ENTERYOURNAME;
}

void MenuFunc_TitleScreen_Options(void)
{
	gGameState = GAMESTATE_OPTIONSSCREEN;
}

void MenuFunc_TitleScreen_Exit(void)
{
	gShouldExit = TRUE;
}