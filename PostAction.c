#include "Main.h"

#include "PostAction.h"

MENUITEM gMI_PlayAgain = { "Play Again", (RENDER_WIDTH / 2) - ((10 * 6) / 2), 160, TRUE, MenuFunc_PostAction_PlayAgain };

MENUITEM gMI_MainMenu = { "Main Menu", (RENDER_WIDTH / 2) - ((9 * 6) / 2), 175, TRUE, MenuFunc_PostAction_MainMenu };

MENUITEM* gMI_PostActionItems[] = { &gMI_PlayAgain, &gMI_MainMenu };

MENU gMENU_PostAction = { "Post Action", 0, _countof(gMI_PostActionItems), gMI_PostActionItems };


void Draw_PostAction(void)
{
	static uint64_t LocalFrameCounter;

	static uint64_t LastFrameSeen;

	if (gTotalFramesRendered > (LastFrameSeen + 1))
	{
		LocalFrameCounter = 0;

		gMENU_PostAction.SelectedItem = 0;

		if (gNetworkData.NetworkInitialized)
		{
			if (gNetworkData.IsHost)
			{
				gMI_PlayAgain.Enabled = TRUE;
			}
			else
			{
				gMI_PlayAgain.Enabled = FALSE;
			}
		}
		else
		{
			gMI_PlayAgain.Enabled = TRUE;
		}
	}

	if (select(
		0,
		&(fd_set) {.fd_count = 1, .fd_array[0] = gNetworkData.UnicastSocket },
		NULL,
		NULL,
		& (TIMEVAL) {.tv_sec = 0, .tv_usec = 1000 }) == 1)
	{
		int AddressLength = sizeof(struct sockaddr);

		recvfrom(
			gNetworkData.UnicastSocket,
			gNetworkData.RecvBuffer,
			sizeof(gNetworkData.RecvBuffer),
			0,
			(struct sockaddr*)&gNetworkData.TempAddress,
			&AddressLength);

		// We received a message. Example: "LR-PLAYAGAIN"
		if (strcmp(gNetworkData.RecvBuffer, "LR-PLAYAGAIN") == 0)
		{
			ResetEverythingForNewGame();

			gGameState = GAMESTATE_ACTIONSCREEN;
		}

		// The other party has quit. We should return to the title screen also.
		if (strcmp(gNetworkData.RecvBuffer, "LR-QUIT") == 0)
		{
			MenuFunc_PostAction_MainMenu();
		}
	}

	if (LocalFrameCounter == 120)
	{
		gInputDisabled = FALSE;
	}

	if (LocalFrameCounter > 120)
	{
		DrawRectangle(
			0,
			0,
			150,
			24,
			&FULL_GREEN_PIXEL,
			&BLACK_PIXEL,
			&DARK_GREEN_PIXEL,
			DR_SHADOW | DR_THICK | DR_ROUNDED_CORNERS | DR_BORDERED | DR_HORIZONTALLY_CENTERED | DR_VERTICALLY_CENTERED | DR_OPAQUE);

		if (gPlayers[PLAYER_ONE].IsDead && gPlayers[PLAYER_TWO].IsDead)
		{
			BlitString("DRAW!", &g6x7Font, &WHITE_PIXEL, 175, 116, &GRAY_PIXEL, BS_SHADOW);
		}
		else if (gPlayers[PLAYER_ONE].IsDead)
		{
			ASSERT(!gPlayers[PLAYER_TWO].IsDead, "both players shouldn't be dead?");

			char WinnerString[32] = { 0 };

			snprintf(WinnerString, sizeof(WinnerString), "%s WINS!", gPlayers[PLAYER_TWO].Name);

			BlitString(
				WinnerString, 
				&g6x7Font, 
				gAvailableColors[gPlayers[PLAYER_TWO].ColorIndex], 
				(RENDER_WIDTH / 2) - (((uint16_t)strlen(WinnerString) * 6) / 2),
				116,
				&DARK_GRAY_PIXEL,
				BS_SHADOW);
		}
		else if (gPlayers[PLAYER_TWO].IsDead)
		{
			ASSERT(!gPlayers[PLAYER_ONE].IsDead, "both players shouldn't be dead?");
			
			char WinnerString[32] = { 0 };

			snprintf(WinnerString, sizeof(WinnerString), "%s WINS!", gPlayers[PLAYER_ONE].Name);

			BlitString(
				WinnerString, 
				&g6x7Font, 
				gAvailableColors[gPlayers[PLAYER_ONE].ColorIndex], 
				(RENDER_WIDTH / 2) - (((uint16_t)strlen(WinnerString) * 6) / 2),
				116,
				&DARK_GRAY_PIXEL,
				BS_SHADOW);
		}

		DrawRectangle(
			0,
			150,
			100,
			42,
			&FULL_GREEN_PIXEL,
			&BLACK_PIXEL,
			&DARK_GREEN_PIXEL,
			DR_SHADOW | DR_THICK | DR_ROUNDED_CORNERS | DR_BORDERED | DR_HORIZONTALLY_CENTERED | DR_OPAQUE);

		for (int i = 0; i < gMENU_PostAction.ItemCount; i++)
		{
			if (gMENU_PostAction.Items[i]->Enabled)
			{
				BlitString(
					gMENU_PostAction.Items[i]->Name, 
					&g6x7Font, 
					&FULL_GREEN_PIXEL, 
					gMENU_PostAction.Items[i]->x, 
					gMENU_PostAction.Items[i]->y, 
					&DARK_GREEN_PIXEL, 
					BS_SHADOW);
			}
			else
			{
				BlitString(
					gMENU_PostAction.Items[i]->Name, 
					&g6x7Font, &DARK_GREEN_PIXEL, 
					gMENU_PostAction.Items[i]->x, 
					gMENU_PostAction.Items[i]->y,
					NULL,
					0);
			}
		}

		BlitString("\xBB",
			&g6x7Font,
			&FULL_GREEN_PIXEL,
			gMENU_PostAction.Items[gMENU_PostAction.SelectedItem]->x - 6,
			gMENU_PostAction.Items[gMENU_PostAction.SelectedItem]->y,
			&DARK_GREEN_PIXEL,
			BS_SHADOW);
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			if (gPlayers[i].IsDead)
			{
				for (int y = 18; y < RENDER_HEIGHT - 18; y++)
				{
					for (int x = 18; x < RENDER_WIDTH - 18; x++)
					{
						if (memcmp(
							(PIXEL32*)gBackBuffer.Memory + x + (RENDER_WIDTH * y),
							gAvailableColors[gPlayers[i].ColorIndex],
							sizeof(PIXEL32)) == 0)
						{
							unsigned int Random = 0;

							rand_s(&Random);

							if (Random % 10 == 0)
							{
								memcpy(
									(PIXEL32*)gBackBuffer.Memory + x + (RENDER_WIDTH * y),
									&BLACK_PIXEL,
									sizeof(PIXEL32));
							}
						}
					}
				}
			}
		}
	}

	LocalFrameCounter++;

	LastFrameSeen = gTotalFramesRendered;
}

void PPI_PostAction(void)
{
	if ((gPlayers[PLAYER_ONE].Input.DownKeyIsDown && !gPlayers[PLAYER_ONE].Input.DownKeyWasDown) ||
		(gPlayers[PLAYER_TWO].Input.DownKeyIsDown && !gPlayers[PLAYER_TWO].Input.DownKeyWasDown))
	{
		PlayGameSound(&gMenuSound);

		if (gMENU_PostAction.SelectedItem == gMENU_PostAction.ItemCount - 1)
		{
			gMENU_PostAction.SelectedItem = 0;
		}
		else
		{
			gMENU_PostAction.SelectedItem++;
		}
	}
	else if ((gPlayers[PLAYER_ONE].Input.UpKeyIsDown && !gPlayers[PLAYER_ONE].Input.UpKeyWasDown) ||
			 (gPlayers[PLAYER_TWO].Input.UpKeyIsDown && !gPlayers[PLAYER_TWO].Input.UpKeyWasDown))
	{
		PlayGameSound(&gMenuSound);

		if (gMENU_PostAction.SelectedItem == 0)
		{
			gMENU_PostAction.SelectedItem = gMENU_PostAction.ItemCount - 1;
		}
		else
		{
			gMENU_PostAction.SelectedItem--;
		}
	}
	else if ((gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown && !gPlayers[PLAYER_ONE].Input.ChooseKeyWasDown) ||
		     (gPlayers[PLAYER_TWO].Input.ChooseKeyIsDown && !gPlayers[PLAYER_TWO].Input.ChooseKeyWasDown))
	{
		if (gMENU_PostAction.Items[gMENU_PostAction.SelectedItem]->Enabled)
		{
			PlayGameSound(&gMenuChooseSound);

			gMENU_PostAction.Items[gMENU_PostAction.SelectedItem]->Action();
		}
	}
}

// Only the host may choose to Play Again
// The client only has the option to return to the main menu.
void MenuFunc_PostAction_PlayAgain(void)
{
	ResetEverythingForNewGame();

	gGameState = GAMESTATE_ACTIONSCREEN;

	if (gNetworkData.IsHost)
	{
		_snprintf_s(
			gNetworkData.SendBuffer,
			sizeof(gNetworkData.SendBuffer),
			_TRUNCATE,
			"LR-PLAYAGAIN");

		sendto(
			gNetworkData.UnicastSocket,
			gNetworkData.SendBuffer,
			(int)strlen(gNetworkData.SendBuffer) + 1,
			0,
			(struct sockaddr*)&gNetworkData.RemoteAddress,
			sizeof(struct sockaddr_in));
	}
}

void MenuFunc_PostAction_MainMenu(void)
{
	// if we are the client in a network game, we need to send the signal to the host that we are quitting.
	// or else the host may try to just keep playing even though the client is no longer there.

	_snprintf_s(
		gNetworkData.SendBuffer,
		sizeof(gNetworkData.SendBuffer),
		_TRUNCATE,
		"LR-QUIT");

	sendto(
		gNetworkData.UnicastSocket,
		gNetworkData.SendBuffer,
		(int)strlen(gNetworkData.SendBuffer) + 1,
		0,
		(struct sockaddr*)&gNetworkData.RemoteAddress,
		sizeof(struct sockaddr_in));

	if (gNetworkData.NetworkInitialized)
	{
		WSACleanup();

		gNetworkData.NetworkInitialized = FALSE;
	}

	memset(gGameHosts, 0, (sizeof(GAMEHOST) * _countof(gGameHosts)));

	gGameState = GAMESTATE_TITLESCREEN;
}