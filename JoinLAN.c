#include "Main.h"

#include "JoinLAN.h"


MENUITEM gMI_JoinGame_Slot00 = { "...", 110, 80, TRUE, MenuFunc_JoinGame_Join };

MENUITEM gMI_JoinGame_Slot01 = { "...", 110, 95, TRUE, MenuFunc_JoinGame_Join };

MENUITEM gMI_JoinGame_Slot02 = { "...", 110, 110, TRUE, MenuFunc_JoinGame_Join };

MENUITEM gMI_JoinGame_Slot03 = { "...", 110, 125, TRUE, MenuFunc_JoinGame_Join };

MENUITEM gMI_JoinGame_Slot04 = { "...", 110, 140, TRUE, MenuFunc_JoinGame_Join };

MENUITEM gMI_JoinGame_ManualIP = { "Enter IP Address", 110, 155, TRUE, MenuFunc_JoinGame_ManualIP };

MENUITEM gMI_JoinGame_Cancel = { "Cancel", 110, 170, TRUE, MenuFunc_JoinGame_Cancel };

MENUITEM* gMI_JoinGameItems[] = { 
	&gMI_JoinGame_Slot00,
	&gMI_JoinGame_Slot01,
	&gMI_JoinGame_Slot02,
	&gMI_JoinGame_Slot03,
	&gMI_JoinGame_Slot04,
	&gMI_JoinGame_ManualIP,
	&gMI_JoinGame_Cancel };

MENU gMENU_JoinGame = { "Searching for players...", 0, _countof(gMI_JoinGameItems), gMI_JoinGameItems };

int gHostDidNotRespond;

void Draw_JoinLANGame(void)
{
	memset(gBackBuffer.Memory, 0, RENDER_AREA_MEM_SIZE);

	static uint64_t LocalFrameCounter;

	static uint64_t LastFrameSeen;

	static BOOL Blink;

	if (gTotalFramesRendered > (LastFrameSeen + 1))
	{
		LocalFrameCounter = 0;

		gMENU_JoinGame.SelectedItem = 0;

		gHostDidNotRespond = 0;

		memset(gGameHosts, 0, sizeof(gGameHosts));
	}

	Blit32BppBitmap(&gTopAccentPicture, 0, 0);	

	if (gHostDidNotRespond)
	{
		DrawRectangle(
			0,
			0,
			160,
			16,
			&FULL_GREEN_PIXEL,
			&BLACK_PIXEL,
			&DARK_GREEN_PIXEL,
			DR_HORIZONTALLY_CENTERED | DR_THICK | DR_BORDERED | DR_OPAQUE | DR_ROUNDED_CORNERS | DR_VERTICALLY_CENTERED | DR_SHADOW);
		
		BlitString(
			"Host did not respond!", 
			&g6x7Font, 
			&FULL_GREEN_PIXEL, 
			(RENDER_WIDTH / 2) - (((uint16_t)strlen("Host did not respond!") * 6) / 2), 
			(RENDER_HEIGHT / 2) - 4,
			&DARK_GREEN_PIXEL,
			BS_SHADOW);

		gHostDidNotRespond--;

		goto End;
	}

	DrawRectangle(
		0,
		16,
		192,
		16,
		&FULL_GREEN_PIXEL,
		NULL,
		&DARK_GREEN_PIXEL,
		DR_BORDERED | DR_SHADOW | DR_ROUNDED_CORNERS | DR_THICK | DR_HORIZONTALLY_CENTERED);

	BlitString(
		gMENU_JoinGame.Name,
		&g6x7Font,
		&FULL_GREEN_PIXEL,
		118,
		20,
		&DARK_GREEN_PIXEL,
		BS_SHADOW);

	DrawRectangle(
		0,
		0,
		224,
		128,
		&FULL_GREEN_PIXEL,
		NULL,
		&DARK_GREEN_PIXEL,
		DR_BORDERED | DR_SHADOW | DR_ROUNDED_CORNERS | DR_THICK | DR_HORIZONTALLY_CENTERED | DR_VERTICALLY_CENTERED);

	for (int i = 0; i < gMENU_JoinGame.ItemCount; i++)
	{
		if (i < 5)
		{
			if (gGameHosts[i].LastSeen > 0)
			{
				BlitString(
					gMENU_JoinGame.Items[i]->Name,
					&g6x7Font,
					gAvailableColors[gGameHosts[i].ColorIndex],
					gMENU_JoinGame.Items[i]->x,
					gMENU_JoinGame.Items[i]->y,
					&DARK_GRAY_PIXEL,
					BS_SHADOW);
			}
			else
			{
				// "..."
				BlitString(
					gMENU_JoinGame.Items[i]->Name,
					&g6x7Font,
					&DARK_GREEN_PIXEL,
					gMENU_JoinGame.Items[i]->x,
					gMENU_JoinGame.Items[i]->y,
					NULL,
					0);
			}
		}
		else
		{
			BlitString(
				gMENU_JoinGame.Items[i]->Name,
				&g6x7Font,
				&FULL_GREEN_PIXEL,
				gMENU_JoinGame.Items[i]->x,
				gMENU_JoinGame.Items[i]->y,
				&DARK_GREEN_PIXEL,
				BS_SHADOW);
		}
	}

	BlitString("\xBB",
		&g6x7Font,
		&FULL_GREEN_PIXEL,
		gMENU_JoinGame.Items[gMENU_JoinGame.SelectedItem]->x - 6,
		gMENU_JoinGame.Items[gMENU_JoinGame.SelectedItem]->y,
		&DARK_GREEN_PIXEL,
		BS_SHADOW);



	if (LocalFrameCounter % 30 == 0)
	{
		Blink = !Blink;

		ReceiveBroadcasts();

		if (strlen(gManualIP))
		{
			MenuFunc_JoinGame_Join();
		}
	}

	if (Blink)
	{
		BlitString(
			"\xf2",
			&g6x7Font,
			&FULL_GREEN_PIXEL,
			264,
			20,
			NULL,
			0);
	}


End:

	LocalFrameCounter++;

	LastFrameSeen = gTotalFramesRendered;
}

void PPI_JoinLANGame(void)
{
	if (gPlayers[PLAYER_ONE].Input.DownKeyIsDown && !gPlayers[PLAYER_ONE].Input.DownKeyWasDown)
	{
		PlayGameSound(&gMenuSound);

		if (gMENU_JoinGame.SelectedItem == gMENU_JoinGame.ItemCount - 1)
		{
			gMENU_JoinGame.SelectedItem = 0;
		}
		else
		{
			gMENU_JoinGame.SelectedItem++;
		}
	}
	else if (gPlayers[PLAYER_ONE].Input.UpKeyIsDown && !gPlayers[PLAYER_ONE].Input.UpKeyWasDown)
	{
		PlayGameSound(&gMenuSound);

		if (gMENU_JoinGame.SelectedItem == 0)
		{
			gMENU_JoinGame.SelectedItem = gMENU_JoinGame.ItemCount - 1;
		}
		else
		{
			gMENU_JoinGame.SelectedItem--;
		}
	}
	else if (gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown && !gPlayers[PLAYER_ONE].Input.ChooseKeyWasDown)
	{
		if (gMENU_JoinGame.Items[gMENU_JoinGame.SelectedItem]->Enabled)
		{
			PlayGameSound(&gMenuChooseSound);

			gMENU_JoinGame.Items[gMENU_JoinGame.SelectedItem]->Action();
		}
	}
}

void MenuFunc_JoinGame_Cancel(void)
{
	if (gNetworkData.NetworkInitialized)
	{
		WSACleanup();

		gNetworkData.NetworkInitialized = FALSE;
	}

	memset(gGameHosts, 0, (sizeof(GAMEHOST) * _countof(gGameHosts)));

	for (int i = 0; i < 5; i++)
	{
		_snprintf_s(
			gMENU_JoinGame.Items[i]->Name,
			sizeof(gMENU_JoinGame.Items[i]->Name),
			_TRUNCATE,
			"...");
	}

	gGameState = GAMESTATE_ENTERYOURNAME;
}

void MenuFunc_JoinGame_ManualIP(void)
{
	gGameState = GAMESTATE_ENTERIP;
}

void MenuFunc_JoinGame_Join(void)
{	
	if (strcmp(gMENU_JoinGame.Items[gMENU_JoinGame.SelectedItem]->Name, "...") == 0 &&
		strlen(gManualIP) == 0)
	{
		return;
	}

	// if gManualIP is populated, that means the user has entered a manual IP and wishes
	// to join a game that is not listed in the list of LAN games. Must be they want to 
	// play with someone over the internet. Problem now is, we don't yet know the name and
	// color of the remote party, unlike in the LAN scenario where the host has already 
	// broadcasted it to us.

	if (strlen(gManualIP) == 0)
	{
		_snprintf_s(
			gPlayers[PLAYER_TWO].Name,
			sizeof(gPlayers[PLAYER_TWO].Name),
			_TRUNCATE,
			"%s", gGameHosts[gMENU_JoinGame.SelectedItem].Name);

		gPlayers[PLAYER_TWO].ColorIndex = gGameHosts[gMENU_JoinGame.SelectedItem].ColorIndex;
	}

	_snprintf_s(
		gNetworkData.SendBuffer, 
		sizeof(gNetworkData.SendBuffer), 
		_TRUNCATE, 
		"LR-JOIN:%02d:%s", gPlayers[PLAYER_ONE].ColorIndex, gPlayers[PLAYER_ONE].Name);

	// send hello message to host and wait for acknowledgement from host	
			
	gNetworkData.RemoteAddress.sin_family = AF_INET;

	if (strlen(gManualIP) == 0)
	{
		gNetworkData.RemoteAddress.sin_addr = gGameHosts[gMENU_JoinGame.SelectedItem].Address.sin_addr;
	}
	else
	{
		gNetworkData.RemoteAddress.sin_addr.S_un.S_addr = inet_addr(gManualIP);

		// Once we are done with gManualIP we can clear it.
		memset(gManualIP, 0, sizeof(gManualIP));
	}
		
	gNetworkData.RemoteAddress.sin_port = htons(NETWORK_PORT_GAME);
		
	sendto(
		gNetworkData.UnicastSocket,
		gNetworkData.SendBuffer,
		(int)strlen(gNetworkData.SendBuffer) + 1,
		0,
		(struct sockaddr*)&gNetworkData.RemoteAddress,
		sizeof(struct sockaddr_in));

	if (select(
		0,
		&(fd_set) { .fd_count = 1, .fd_array[0] = gNetworkData.UnicastSocket },
		NULL,
		NULL,
		&(TIMEVAL) { .tv_sec = 1, .tv_usec = 0 }) == 1)
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
			// We received a message. Example: LR-ACCEPTED:01:02:Joe
			if (strncmp(gNetworkData.RecvBuffer, "LR-ACCEPTED:", 12) == 0)
			{
				char ColorAsString[3] = { 0 };

				char MapAsString[3] = { 0 };				

				ColorAsString[0] = gNetworkData.RecvBuffer[12];

				ColorAsString[1] = gNetworkData.RecvBuffer[13];

				gPlayers[PLAYER_TWO].ColorIndex = (uint8_t)atoi(ColorAsString);

				MapAsString[0] = gNetworkData.RecvBuffer[15];

				MapAsString[1] = gNetworkData.RecvBuffer[16];

				gMapStyle = (uint8_t)atoi(MapAsString);

				_snprintf_s(
					gPlayers[PLAYER_TWO].Name,
					sizeof(gPlayers[PLAYER_TWO].Name),
					_TRUNCATE,
					"%s",
					gNetworkData.RecvBuffer + 18);				

				ResetEverythingForNewGame();

				gGameState = GAMESTATE_ACTIONSCREEN;

				return;
			}
		}
	}	

	// If we made it this far that means the host did not acknowledge our join request.

	gHostDidNotRespond = 180;
}

void ReceiveBroadcasts(void)
{
	ASSERT(gNetworkData.NetworkInitialized, "network is not initialized!");
	ASSERT(gNetworkData.IsHost == FALSE, "IsHost should be false here?");

	// First clean out any entires who haven't sent a broadcast in over 3 seconds
	for (int i = 0; i < 5; i++)
	{
		if ((gGameHosts[i].LastSeen > 0) && (gGameHosts[i].LastSeen < (gTotalFramesRendered - 180)))
		{
			memset(&gGameHosts[i], 0, sizeof(GAMEHOST));

			_snprintf_s(
				gMENU_JoinGame.Items[i]->Name, 
				sizeof(gMENU_JoinGame.Items[i]->Name), 
				_TRUNCATE, 
				"...");
		}
	}

	if (select(
		0,
		&(fd_set) { .fd_count = 1, .fd_array[0] = gNetworkData.BroadcastSocket },
		NULL,
		NULL,
		&(TIMEVAL) { .tv_sec = 0, .tv_usec = 5000 }) == 1)
	{
		int AddressLength = sizeof(struct sockaddr);

		if (recvfrom(
			gNetworkData.BroadcastSocket,
			gNetworkData.BroadcastBuffer,
			sizeof(gNetworkData.BroadcastBuffer),
			0,
			(struct sockaddr*)&gNetworkData.TempAddress,
			&AddressLength) == SOCKET_ERROR)
		{
			ShowFormattedErrorDialog(WSAGetLastError(), L"recvfrom failed!");

			gShouldExit = TRUE;
		}
		else
		{
			// We received a message. Example: LR-HOST:01:02:Gabriel
			if (strncmp(gNetworkData.BroadcastBuffer, "LR-HOST:", 8) == 0)
			{
				BOOL Match = FALSE;

				char PlayerName[16] = { 0 };

				char ColorAsString[3] = { 0 };

				char MapAsString[3] = { 0 };

				int ColorIndex = 0;

				ColorAsString[0] = gNetworkData.BroadcastBuffer[8];

				ColorAsString[1] = gNetworkData.BroadcastBuffer[9];

				ColorIndex = atoi(ColorAsString);

				MapAsString[0] = gNetworkData.BroadcastBuffer[11];

				MapAsString[1] = gNetworkData.BroadcastBuffer[12];				

				_snprintf_s(
					PlayerName, 
					sizeof(PlayerName), 
					_TRUNCATE, 
					"%s",
					gNetworkData.BroadcastBuffer + 14);

				// if we find a match, update the last seen time
				// if no match found, add it				

				for (int i = 0; i < 5; i++)
				{
					if (strcmp(PlayerName, gGameHosts[i].Name) == 0 &&						
						(gNetworkData.TempAddress.sin_addr.S_un.S_addr == gGameHosts[i].Address.sin_addr.S_un.S_addr))
					{
						gGameHosts[i].LastSeen = gTotalFramesRendered;

						Match = TRUE;

						break;
					}
				}

				// Find the first empty slot and add it there
				if (!Match)
				{
					for (int i = 0; i < 5; i++)
					{
						if (gGameHosts[i].LastSeen == 0)
						{
							gGameHosts[i].Address = gNetworkData.TempAddress;

							gGameHosts[i].ColorIndex = (uint8_t)ColorIndex;

							gGameHosts[i].LastSeen = gTotalFramesRendered;

							gGameHosts[i].Map = (uint8_t)atoi(MapAsString);

							_snprintf_s(
								gGameHosts[i].Name,
								sizeof(gGameHosts[i].Name),
								_TRUNCATE,
								"%s", PlayerName);

							break;
						}
					}
				}

				// finally rebuild the on-screen menu to reflect current data

				for (int i = 0; i < 5; i++)
				{
					if (strlen(gGameHosts[i].Name) > 0)
					{
						_snprintf_s(
							gMENU_JoinGame.Items[i]->Name,
							sizeof(gMENU_JoinGame.Items[i]->Name),
							_TRUNCATE,
							"%s [%u.%u.%u.%u]", gGameHosts[i].Name, 
							gGameHosts[i].Address.sin_addr.S_un.S_un_b.s_b1, gGameHosts[i].Address.sin_addr.S_un.S_un_b.s_b2, gGameHosts[i].Address.sin_addr.S_un.S_un_b.s_b3, gGameHosts[i].Address.sin_addr.S_un.S_un_b.s_b4);
					}
				}
			}
		}
	}
}