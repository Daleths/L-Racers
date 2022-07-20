#include "Main.h"

#include "HostLAN.h"

MENUITEM gMI_HostGame_Cancel = { "Cancel", (RENDER_WIDTH / 2) - ((6 * 6) / 2), 116, TRUE, MenuFunc_HostGame_Cancel};

MENUITEM* gMI_HostGameItems[] = { &gMI_HostGame_Cancel };

MENU gMENU_HostGame = { "Waiting for a player to connect...", 0, _countof(gMI_HostGameItems), gMI_HostGameItems };

void Draw_HostLANGame(void)
{
	memset(gBackBuffer.Memory, 0, RENDER_AREA_MEM_SIZE);

	static uint64_t LocalFrameCounter;

	static uint64_t LastFrameSeen;

	static BOOL Blink;

	if (gTotalFramesRendered > (LastFrameSeen + 1))
	{
		LocalFrameCounter = 0;

		gMENU_HostGame.SelectedItem = 0;
	}

	Blit32BppBitmap(&gTopAccentPicture, 0, 0);

	DrawRectangle(
		0,
		16,
		240,
		16,
		&FULL_GREEN_PIXEL,
		NULL,
		&DARK_GREEN_PIXEL,
		DR_BORDERED | DR_SHADOW | DR_ROUNDED_CORNERS | DR_THICK | DR_HORIZONTALLY_CENTERED);

	BlitString(
		gMENU_HostGame.Name,
		&g6x7Font,
		&FULL_GREEN_PIXEL,
		80,
		20,
		&DARK_GREEN_PIXEL,
		BS_SHADOW);

	DrawRectangle(
		0,
		0,
		64,
		16,
		&FULL_GREEN_PIXEL,
		NULL,
		&DARK_GREEN_PIXEL,
		DR_BORDERED | DR_SHADOW | DR_ROUNDED_CORNERS | DR_THICK | DR_HORIZONTALLY_CENTERED | DR_VERTICALLY_CENTERED);

	BlitString(
		gMENU_HostGame.Items[0]->Name,
		&g6x7Font,
		&FULL_GREEN_PIXEL,
		gMENU_HostGame.Items[0]->x,
		gMENU_HostGame.Items[0]->y,
		&DARK_GREEN_PIXEL,
		BS_SHADOW);

	BlitString("\xBB",
		&g6x7Font,
		&FULL_GREEN_PIXEL,
		gMENU_HostGame.Items[gMENU_HostGame.SelectedItem]->x - 6,
		gMENU_HostGame.Items[gMENU_HostGame.SelectedItem]->y,
		&DARK_GREEN_PIXEL,
		BS_SHADOW);

	if (LocalFrameCounter % 30 == 0)
	{
		Blink = !Blink;

		sendto(
			gNetworkData.BroadcastSocket,
			gNetworkData.BroadcastBuffer,
			(int)strlen(gNetworkData.BroadcastBuffer) + 1,
			0,
			(struct sockaddr*)&gNetworkData.BroadcastAddress,
			sizeof(struct sockaddr_in));
	}

	if (Blink)
	{
		BlitString(
			"\xf2",
			&g6x7Font,
			&FULL_GREEN_PIXEL,
			285,
			20,
			NULL,
			0);
	}

	// check for join requests from clients
	if (select(
		0,
		&(fd_set) {.fd_count = 1, .fd_array[0] = gNetworkData.UnicastSocket },
		NULL,
		NULL,
		&(TIMEVAL) {.tv_sec = 0, .tv_usec = 5000 }) == 1)
	{
		int AddressLength = sizeof(struct sockaddr);

		if (recvfrom(
			gNetworkData.UnicastSocket,
			gNetworkData.RecvBuffer,
			sizeof(gNetworkData.RecvBuffer),
			0,
			(struct sockaddr*)&gNetworkData.TempAddress,
			&AddressLength) == SOCKET_ERROR)
		{
			ShowFormattedErrorDialog(WSAGetLastError(), L"recvfrom failed!");

			gShouldExit = TRUE;
		}
		else
		{
			// We received a message. Example: "LR-JOIN:%02d:%s"
			if (strncmp(gNetworkData.RecvBuffer, "LR-JOIN:", 8) == 0)
			{
				char ColorAsString[3] = { 0 };				

				ColorAsString[0] = gNetworkData.RecvBuffer[8];

				ColorAsString[1] = gNetworkData.RecvBuffer[9];				

				_snprintf_s(
					gPlayers[PLAYER_TWO].Name,
					sizeof(gPlayers[PLAYER_TWO].Name),
					_TRUNCATE,
					"%s",
					gNetworkData.RecvBuffer + 11);

				gPlayers[PLAYER_TWO].ColorIndex = (uint8_t)atoi(ColorAsString);;

				// send acknowledgement back to client that we have accepted his join request
				// we are sending our color and name again in case the client did not receive our 
				// advertisement broadcast, yet knows our IP address and is trying to join manually.

				memcpy(&gNetworkData.RemoteAddress, &gNetworkData.TempAddress, sizeof(struct sockaddr_in));

				_snprintf_s(
					gNetworkData.SendBuffer,
					sizeof(gNetworkData.SendBuffer),
					_TRUNCATE,
					"LR-ACCEPTED:%02d:%02d:%s", gPlayers[PLAYER_ONE].ColorIndex, gMapStyle, gPlayers[PLAYER_ONE].Name);

				sendto(
					gNetworkData.UnicastSocket,
					gNetworkData.SendBuffer,
					(int)strlen(gNetworkData.SendBuffer) + 1,
					0,
					(struct sockaddr*)&gNetworkData.RemoteAddress,
					sizeof(struct sockaddr_in));

				ResetEverythingForNewGame();

				gGameState = GAMESTATE_ACTIONSCREEN;

				return;
			}
		}
	}

	LocalFrameCounter++;

	LastFrameSeen = gTotalFramesRendered;
}

void PPI_HostLANGame(void)
{
	if (gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown && !gPlayers[PLAYER_ONE].Input.ChooseKeyWasDown)
	{
		PlayGameSound(&gMenuChooseSound);

		gMENU_HostGame.Items[gMENU_HostGame.SelectedItem]->Action();		
	}
}

void MenuFunc_HostGame_Cancel(void)
{
	ASSERT(gNetworkData.NetworkInitialized == TRUE, "network is not initialized!");
	ASSERT(gNetworkData.IsHost == TRUE, "IsHost should be true here?")

	if (gNetworkData.NetworkInitialized)
	{
		WSACleanup();

		gNetworkData.NetworkInitialized = FALSE;
	}

	gGameState = GAMESTATE_ENTERYOURNAME;
}