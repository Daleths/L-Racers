#include "Main.h"

#include "Action.h"

void Draw_Action(void)
{
	static uint64_t LocalFrameCounter;

	static uint64_t LastFrameSeen;

	// This counter is specifically for the rare case when both players are trying to occupy the same (X,Y)
	// simultaneously. This flag allows us to draw one more frame even after one or both players have died,
	// because it looks better that way. Otherwise it would look like the players never quite collided.
	static uint64_t PlayerDeathFrame;

	// The pixel that player one is attempting to move to
	PIXEL32 P1_DestinationPixel = { 0 };

	// The pixel that player two is attempting to move to
	PIXEL32 P2_DestinationPixel = { 0 };

	memset(gBackBuffer.Memory, 0, RENDER_AREA_MEM_SIZE);

	if (gTotalFramesRendered > (LastFrameSeen + 1))
	{
		LocalFrameCounter = 0;

		gInputDisabled = TRUE;

		PlayerDeathFrame = 0;
	}	

	if (LocalFrameCounter == 300)
	{
		gInputDisabled = FALSE;
	}

	if (gNetworkData.NetworkInitialized)
	{
		if (gNetworkData.IsHost)
		{
			BlitString(
				gPlayers[PLAYER_ONE].Name,
				&g6x7Font,
				gAvailableColors[gPlayers[PLAYER_ONE].ColorIndex],
				20,
				6,
				&DARK_GRAY_PIXEL,
				BS_SHADOW);

			BlitString(
				gPlayers[PLAYER_TWO].Name,
				&g6x7Font,
				gAvailableColors[gPlayers[PLAYER_TWO].ColorIndex],
				(RENDER_WIDTH - 20) - ((uint16_t)strlen(gPlayers[PLAYER_TWO].Name) * 6),
				6,
				&DARK_GRAY_PIXEL,
				BS_SHADOW);
		}
		else
		{
			BlitString(
				gPlayers[PLAYER_TWO].Name,
				&g6x7Font,
				gAvailableColors[gPlayers[PLAYER_TWO].ColorIndex],
				20,
				6,
				&DARK_GRAY_PIXEL,
				BS_SHADOW);

			BlitString(
				gPlayers[PLAYER_ONE].Name,
				&g6x7Font,
				gAvailableColors[gPlayers[PLAYER_ONE].ColorIndex],
				(RENDER_WIDTH - 20) - ((uint16_t)strlen(gPlayers[PLAYER_ONE].Name) * 6),
				6,
				&DARK_GRAY_PIXEL,
				BS_SHADOW);
		}
	}
	else
	{
		BlitString(
			gPlayers[PLAYER_ONE].Name,
			&g6x7Font,
			gAvailableColors[gPlayers[PLAYER_ONE].ColorIndex],
			20,
			6,
			&DARK_GRAY_PIXEL,
			BS_SHADOW);

		BlitString(
			gPlayers[PLAYER_TWO].Name,
			&g6x7Font,
			gAvailableColors[gPlayers[PLAYER_TWO].ColorIndex],
			(RENDER_WIDTH - 20) - ((uint16_t)strlen(gPlayers[PLAYER_TWO].Name) * 6),
			6,
			&DARK_GRAY_PIXEL,
			BS_SHADOW);
	}

	DrawRectangle(
		16,
		16,
		RENDER_WIDTH - 32,
		RENDER_HEIGHT - 32,
		&FULL_GREEN_PIXEL,
		NULL,
		&DARK_GREEN_PIXEL,
		DR_BORDERED | DR_THICK | DR_ROUNDED_CORNERS | DR_SHADOW);

	// Draw the dark grey dots in the background.
	for (int y = 19; y < 221; y += 3)
	{
		for (int x = 19; x < 365; x += 3)
		{
			memcpy((PIXEL32*)gBackBuffer.Memory + x + (RENDER_WIDTH * y), &DARK_GRAY_PIXEL, sizeof(PIXEL32));
		}
	}

	// What map are we drawing?

	switch (gMapStyle)
	{
		case 0:
		{
			// Do nothing, this is the default "blank" map.

			break;
		}
		case 1:
		{
			// L-shape upper left corner
			DrawRectangle(
				32,
				32,
				64,
				4,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				32,
				32,
				4,
				64,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			// L-shape upper-right corner
			DrawRectangle(
				RENDER_WIDTH - 96,
				32,
				64,
				4,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				RENDER_WIDTH - 36,
				32,
				4,
				64,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			// L-shape lower-right corner
			DrawRectangle(
				RENDER_WIDTH - 96,
				RENDER_HEIGHT - 36,
				64,
				4,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				RENDER_WIDTH - 36,
				RENDER_HEIGHT - 96,
				4,
				64,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			// L-shape lower-left corner
			DrawRectangle(
				32,
				RENDER_HEIGHT - 96,
				4,
				64,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				32,
				RENDER_HEIGHT - 36,
				64,
				4,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			// bar in the center
			DrawRectangle(
				(RENDER_WIDTH / 2) - (96 / 2),
				(RENDER_HEIGHT / 2) - (4 / 2),
				96,
				4,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			break;
		}
		case 2:
		{
			// uniform blocks

			DrawRectangle(
				56,
				32,
				16,
				16,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				120,
				32,
				16,
				16,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				184,
				32,
				16,
				16,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				248,
				32,
				16,
				16,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				312,
				32,
				16,
				16,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				56,
				192,
				16,
				16,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				120,
				192,
				16,
				16,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				184,
				192,
				16,
				16,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				248,
				192,
				16,
				16,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				312,
				192,
				16,
				16,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				(RENDER_WIDTH / 2) - (8 / 2),
				(RENDER_HEIGHT / 2) - (64 / 2),
				8,
				64,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);			

			break;
		}
		case 3:
		{
			DrawRectangle(
				32,
				64,
				128,
				4,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				RENDER_WIDTH - 160,
				64,
				128,
				4,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				32,
				172,
				128,
				4,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				RENDER_WIDTH - 160,
				172,
				128,
				4,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				161,
				64,
				4,
				112,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			DrawRectangle(
				219,
				64,
				4,
				112,
				NULL,
				&FULL_GREEN_PIXEL,
				NULL,
				DR_OPAQUE);

			break;
		}		
		default:
		{
			ASSERT(FALSE, "Unknown map!");
		}
	}

	// for each player...
	for (int p = 0; p < 2; p++)
	{
		// Draw the player's tail, i.e., draw a line between every turning point the player has made.
		// The 0th element of this array is the player's starting position.
		for (int i = 0; i < _countof(gPlayers[p].TurnPoints) - 1; i++)
		{
			if (gPlayers[p].TurnPoints[(i + 1)].x == 0)
			{
				// Draw one final line between the player's last turn point and their current position.			
				DrawLine(
					gPlayers[p].TurnPoints[i].x,
					gPlayers[p].TurnPoints[i].y,
					gPlayers[p].CurrentPosition.x,
					gPlayers[p].CurrentPosition.y,
					*gAvailableColors[gPlayers[p].ColorIndex]);

				break;
			}

			DrawLine(
				gPlayers[p].TurnPoints[i].x,
				gPlayers[p].TurnPoints[i].y,
				gPlayers[p].TurnPoints[(i + 1)].x,
				gPlayers[p].TurnPoints[(i + 1)].y,
				*gAvailableColors[gPlayers[p].ColorIndex]);
		}

		// Draw the player head as a white dot.
		memcpy((PIXEL32*)gBackBuffer.Memory + gPlayers[p].CurrentPosition.x + (RENDER_WIDTH * gPlayers[p].CurrentPosition.y),
			&WHITE_PIXEL,
			sizeof(PIXEL32));
	}

	if (LocalFrameCounter >= 300)
	{
		// Move player 1, but first copy the pixel that the player would be travelling to, 
		// and kill them if that pixel is already occupied.

		switch (gPlayers[PLAYER_ONE].CurrentDirection)
		{
			case DIRECTION_RIGHT:
			{
				memcpy(
					&P1_DestinationPixel,
					(PIXEL32*)gBackBuffer.Memory + (gPlayers[PLAYER_ONE].CurrentPosition.x + 1) + (RENDER_WIDTH * gPlayers[PLAYER_ONE].CurrentPosition.y),
					sizeof(PIXEL32));

				gPlayers[PLAYER_ONE].CurrentPosition.x++;

				break;
			}
			case DIRECTION_LEFT:
			{
				memcpy(
					&P1_DestinationPixel,
					(PIXEL32*)gBackBuffer.Memory + (gPlayers[PLAYER_ONE].CurrentPosition.x - 1) + (RENDER_WIDTH * gPlayers[PLAYER_ONE].CurrentPosition.y),
					sizeof(PIXEL32));

				gPlayers[PLAYER_ONE].CurrentPosition.x--;

				break;
			}
			case DIRECTION_UP:
			{
				memcpy(
					&P1_DestinationPixel,
					(PIXEL32*)gBackBuffer.Memory + gPlayers[PLAYER_ONE].CurrentPosition.x + (RENDER_WIDTH * (gPlayers[PLAYER_ONE].CurrentPosition.y + 1)),
					sizeof(PIXEL32));

				gPlayers[PLAYER_ONE].CurrentPosition.y++;

				break;
			}
			case DIRECTION_DOWN:
			{
				memcpy(
					&P1_DestinationPixel,
					(PIXEL32*)gBackBuffer.Memory + gPlayers[PLAYER_ONE].CurrentPosition.x + (RENDER_WIDTH * (gPlayers[PLAYER_ONE].CurrentPosition.y - 1)),
					sizeof(PIXEL32));

				gPlayers[PLAYER_ONE].CurrentPosition.y--;

				break;
			}
			default:
			{
				ASSERT(0, "UNKNOWN DIRECTION");
			}
		}

		// Move player 2, but first copy the pixel that the player would be travelling to, 
		// and kill them if that pixel is already occupied.

		switch (gPlayers[PLAYER_TWO].CurrentDirection)
		{
			case DIRECTION_RIGHT:
			{
				memcpy(
					&P2_DestinationPixel,
					(PIXEL32*)gBackBuffer.Memory + (gPlayers[PLAYER_TWO].CurrentPosition.x + 1) + (RENDER_WIDTH * gPlayers[PLAYER_TWO].CurrentPosition.y),
					sizeof(PIXEL32));

				gPlayers[PLAYER_TWO].CurrentPosition.x++;

				break;
			}
			case DIRECTION_LEFT:
			{
				memcpy(
					&P2_DestinationPixel,
					(PIXEL32*)gBackBuffer.Memory + (gPlayers[PLAYER_TWO].CurrentPosition.x - 1) + (RENDER_WIDTH * gPlayers[PLAYER_TWO].CurrentPosition.y),
					sizeof(PIXEL32));

				gPlayers[PLAYER_TWO].CurrentPosition.x--;

				break;
			}
			case DIRECTION_UP:
			{
				memcpy(
					&P2_DestinationPixel,
					(PIXEL32*)gBackBuffer.Memory + gPlayers[PLAYER_TWO].CurrentPosition.x + (RENDER_WIDTH * (gPlayers[PLAYER_TWO].CurrentPosition.y + 1)),
					sizeof(PIXEL32));

				gPlayers[PLAYER_TWO].CurrentPosition.y++;

				break;
			}
			case DIRECTION_DOWN:
			{
				memcpy(
					&P2_DestinationPixel,
					(PIXEL32*)gBackBuffer.Memory + gPlayers[PLAYER_TWO].CurrentPosition.x + (RENDER_WIDTH * (gPlayers[PLAYER_TWO].CurrentPosition.y - 1)),
					sizeof(PIXEL32));

				gPlayers[PLAYER_TWO].CurrentPosition.y--;

				break;
			}
			default:
			{
				ASSERT(0, "UNKNOWN DIRECTION");
			}
		}
	}

	if (P1_DestinationPixel.Bytes != 0 && P1_DestinationPixel.Bytes != (DARK_GRAY_PIXEL).Bytes)
	{
		gPlayers[PLAYER_ONE].IsDead = TRUE;		
	}

	if (P2_DestinationPixel.Bytes != 0 && P2_DestinationPixel.Bytes != (DARK_GRAY_PIXEL).Bytes)
	{
		gPlayers[PLAYER_TWO].IsDead = TRUE;
	}

	// There is a rare case here where if the players both try to move into the same point
	// at the same time from perpendicular angles, they may pass through one another because
	// one pixel forward in the direction that player 1 is traveling and one pixel forward
	// in the the direction player 2 is traveling both look empty at the time the measurement is made.
	// This seems the most obvious fix...
	if ((gPlayers[PLAYER_ONE].CurrentPosition.x == gPlayers[PLAYER_TWO].CurrentPosition.x) &&
		(gPlayers[PLAYER_ONE].CurrentPosition.y == gPlayers[PLAYER_TWO].CurrentPosition.y))
	{
		gPlayers[PLAYER_ONE].IsDead = TRUE;

		gPlayers[PLAYER_TWO].IsDead = TRUE;		
	}

	if ((PlayerDeathFrame == 0) && (gPlayers[PLAYER_ONE].IsDead || gPlayers[PLAYER_TWO].IsDead))
	{
		PlayerDeathFrame = LocalFrameCounter;
	}

	// This is to allow the game to draw one more frame even after one or both players have died
	if ((PlayerDeathFrame > 0) && (PlayerDeathFrame < LocalFrameCounter))
	{
		PlayGameSound(&gExplosionSound);

		gGameState = GAMESTATE_POSTACTION;

		gInputDisabled = TRUE;
	}

	if (LocalFrameCounter < 300)
	{
		static BOOL Blink;

		if (LocalFrameCounter % 25 == 0)
		{
			Blink = !Blink;
		}

		DrawRectangle(
			0,
			0,
			100,
			24,
			&FULL_GREEN_PIXEL,
			&BLACK_PIXEL,
			&DARK_GREEN_PIXEL,
			DR_SHADOW | DR_THICK | DR_ROUNDED_CORNERS | DR_BORDERED | DR_HORIZONTALLY_CENTERED | DR_VERTICALLY_CENTERED | DR_OPAQUE);

		if (Blink)
		{
			BlitString("GET READY...", &g6x7Font, &FULL_GREEN_PIXEL, 160, 116, &DARK_GREEN_PIXEL, BS_SHADOW);
		}
	}

	LocalFrameCounter++;

	LastFrameSeen = gTotalFramesRendered;
}

void PPI_Action(void)
{
	// Player can only turn 90 degrees at a time, so if the player is going right, 
	// they cannot just start going left. If they are going up, they cannot go down without making a left or right turn first.

	for (int i = 0; i < 2; i++)
	{
		// Skip this player if they are AI-controlled.
		if (gPlayers[i].AI)
		{
			CalculateAIMovement();

			continue;
		}

		DIRECTION PreviousDirection = gPlayers[i].CurrentDirection;

		if (gPlayers[i].Input.LeftKeyIsDown)
		{
			if (gPlayers[i].CurrentDirection != DIRECTION_RIGHT)
			{
				gPlayers[i].CurrentDirection = DIRECTION_LEFT;
			}
		}
		else if (gPlayers[i].Input.RightKeyIsDown)
		{
			if (gPlayers[i].CurrentDirection != DIRECTION_LEFT)
			{
				gPlayers[i].CurrentDirection = DIRECTION_RIGHT;
			}
		}
		else if (gPlayers[i].Input.UpKeyIsDown)
		{
			if (gPlayers[i].CurrentDirection != DIRECTION_DOWN)
			{
				gPlayers[i].CurrentDirection = DIRECTION_UP;
			}
		}
		else if (gPlayers[i].Input.DownKeyIsDown)
		{
			if (gPlayers[i].CurrentDirection != DIRECTION_UP)
			{
				gPlayers[i].CurrentDirection = DIRECTION_DOWN;
			}
		}

		if (PreviousDirection != gPlayers[i].CurrentDirection)
		{
			// Add the player's current position to the array of turn points
			for (int x = 0; x < _countof(gPlayers[i].TurnPoints); x++)
			{
				if (gPlayers[i].TurnPoints[x].x == 0)
				{
					gPlayers[i].TurnPoints[x].x = gPlayers[i].CurrentPosition.x;

					gPlayers[i].TurnPoints[x].y = gPlayers[i].CurrentPosition.y;

					break;
				}
			}
		}
	}
}