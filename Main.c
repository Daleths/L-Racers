// L-RACERS
// Written by Joseph Ryan Ries, 2022
// Everything is from scratch by me, except for miniz by Rich Geldreich and stb_vorbis by Sean Barrett.
// Music by nocturnalplant.
//
////////////////////////////////////////////////////////////////
// VERSION HISTORY
//
// v1.0.0: Initial release to Steam!
// 
// v1.0.1: Minor improvements to options menu controls.
//         Fixed minor title screen marquee glitch.
//         Fixed bug where volume sliders didn't update correctly.
//         Added green to the available colors for players.
//         Player two can now use the post-game menu after a 2-player local game.
//         Added GAMEPAD_UNPLUGGED gamestate. Game will now automatically pause if a gamepad is removed.
// 
// v1.0.2: Skipped
// 
// v1.0.3: Added fullscreen support.
//         Fixed a bug where if the two players crossed paths at the exact same moment
//         while travelling perpendicular to each other, they could pass through one another instead
//         of both players crashing as was intended.
// 
// 
////////////////////////////////////////////////////////////////

// Main.h includes various declarations that are common among multiple compilation units.
// There are separate header files for each gamestate which include declarations that are
// specific to that gamestate.

#include "Main.h"

#include "TitleScreen.h"

#include "Options.h"

#include "EnterYourName.h"

#include "JoinLAN.h"

#include "HostLAN.h"

#include "Action.h"

#include "PostAction.h"

#include "EnterIP.h"

#include "GamepadUnplugged.h"

#include "miniz.h"

#include "resource.h"

// for timeBeginPeriod()
#pragma comment(lib, "Winmm.lib")
// for network comms 
#pragma comment(lib, "Ws2_32.lib")

//////////////////////////////////////////////////
// Initialize extern globals declared in main.h
//////////////////////////////////////////////////

// If this is set to TRUE, the game will exit ASAP.
BOOL gShouldExit = FALSE;

// Is the window full screen or not
BOOL gFullscreen = FALSE;

// By what factor can we scale up the render resolution and still fit on the monitor
uint8_t gDrawScale = 0;

// Handle to the game's main window.
HWND gMainWindow = NULL;

// Holds information about the primary monitor such as its resolution.
MONITORINFO gPrimaryMonitor = { sizeof(MONITORINFO) };

// This is the drawing surface of the game. I have total control over every pixel
// that gets painted in every frame. 
GAMEBITMAP gBackBuffer = { 0 };

// Currently the only font that exists in this game.
GAMEBITMAP g6x7Font = { 0 };

// Green to black gradient that looks nice at the top of some of the gamestates.
GAMEBITMAP gTopAccentPicture = { 0 };

// Drawn in the GAMEPAD_UNPLUGGED gamestate.
GAMEBITMAP gXboxGamepadPicture = { 0 };

// gPlayers[0] is PLAYER_ONE. gPlayers[1] is PLAYER_TWO.
PLAYER gPlayers[2] = { 0 };

uint64_t gTotalFramesRendered = 0;

// The game is always in one of the possible gamestates.
GAMESTATE gGameState = GAMESTATE_TITLESCREEN;

// Used by the "gamepad unplugged" gamestate to restore us to wherever
// we were before the gamepad was surprise-removed.
GAMESTATE gPreviousGameState = GAMESTATE_TITLESCREEN;

// We disable input when the window is out of focus, but NOT if 
// we are currently in the "action" gamestate. The action will continue regardless
// of whether the window is out of focus or not. Otherwise, this would give player one
// the ability to "disable" player two's input by taking the window out of focus... 
// even in a multiplayer game!
BOOL gInputDisabled = FALSE;

BOOL gWindowHasFocus = FALSE;

// Players can change their racers to any of these colors.
// BE CAREFUL not to add BLACK_PIXEL or DARK_GRAY_PIXEL to this list, because
// we are doing collision detection based on those colors!
PIXEL32* gAvailableColors[] = {
	&FULL_BLUE_PIXEL,
	&FULL_RED_PIXEL,
	&FULL_GREEN_PIXEL,
	&WHITE_PIXEL,
	&GRAY_PIXEL,
	&PINK_PIXEL,
	&ORANGE_PIXEL,
	&YELLOW_PIXEL,
	&LIGHT_BLUE_PIXEL,
	&DARK_GREEN_PIXEL,
	&PURPLE_PIXEL,
	&DUMMY_PIXEL }; // DUMMY_PIXEL always needs to stay at the end.

// These will be converted to floats before sending them to SetVolume.
int8_t gSFXVolume = 5;

int8_t gMusicVolume = 3;

// The source voice used for playing sound effects. We can play 4 overlapping sound effects
// at once before the sounds start to queue up.
IXAudio2SourceVoice* gXAudioSFXSourceVoice[NUMBER_OF_SFX_SOURCE_VOICES] = { 0 };

IXAudio2SourceVoice* gXAudioMusicSourceVoice = NULL;

GAMESOUND gMenuSound = { 0 };

GAMESOUND gMenuChooseSound = { 0 };

GAMESOUND gExplosionSound = { 0 };

// Contains all the data we need for multiplayer network play.
NETWORKDATA gNetworkData = { 0 };

// Used for the "Join LAN Game" menu. These are hosts on the LAN that are broadcasting
// that they are waiting for someone to play with them.
GAMEHOST gGameHosts[5] = { 0 };

// For when the user wants to enter a manual IP address, to play with someone over the internet.
char gManualIP[16] = { 0 };

int8_t gGamepadsPresent = -1;

int8_t gGamepadsExpected = -1;

///////////////////////////////////
// End initialize extern globals 
///////////////////////////////////

// The following variables do not need to be extern'ed, 
// because they are only referenced here in main.c.
IXAudio2* gXAudio;

IXAudio2MasteringVoice* gXAudioMasteringVoice;

uint8_t gSFXSourceVoiceSelector;

GAMESOUND gMusic;


// The program's entry point.
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{	
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(pCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);			

	MSG WindowMessage = { 0 };

	WNDCLASSEXW WindowClass = { sizeof(WNDCLASSEXW) };
	
	int64_t FrameStart = 0;
	
	int64_t FrameEnd = 0;

	int64_t ElapsedMicroseconds = 0;

	int64_t ElapsedMicrosecondsAccumulatorRaw = 0;

	int64_t ElapsedMicrosecondsAccumulatorCooked = 0;

	int64_t PerformanceFrequency = 0;	

#ifdef _DEBUG
	float RawFPSAverage = 0;

	float CookedFPSAverage = 0;
#endif

	// We need to set the system timer to 1ms if we are going to have accurate frame timings.
	// There is only one system timer. This is a global setting. Windows will set this to lowest
	// value requested by any app on the system. So it may already be 1ms, depending on what other
	// apps are running.
	if (timeBeginPeriod(1) == TIMERR_NOCANDO)
	{
		ShowFormattedErrorDialog(ERROR_TIMER_RESOLUTION_NOT_SET, L"Failed to set global timer resolution!");
		
		return(0);
	}

	if (LoadAssets() == FALSE)
	{
		return(0);
	}

	if (InitializeSoundEngine() != S_OK)
	{
		return(0);
	}

	WindowClass.style = 0;

	WindowClass.lpfnWndProc = WindowProc;

	WindowClass.hbrBackground = GetStockObject(BLACK_BRUSH);

	WindowClass.cbClsExtra = 0;

	WindowClass.cbWndExtra = 0;

	WindowClass.hInstance = hInstance;

	WindowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);

	WindowClass.hIcon = LoadIconW(hInstance, L"MAINICON");

	WindowClass.lpszMenuName = NULL;

	WindowClass.lpszClassName = WINDOW_CLASS_NAME;

	if (RegisterClassExW(&WindowClass) == 0)
	{
		ShowFormattedErrorDialog(GetLastError(), L"RegisterClassExW failed while registering window class!");

		return(0);
	}

	// None of the styles or sizes matter right now because we are going to adjust
	// everything about the window in the CalculateWindowPlacement function.
	gMainWindow = CreateWindowExW(
		0,
		WINDOW_CLASS_NAME,
		GAME_NAME_W,
		(WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX)) | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		RENDER_WIDTH,
		RENDER_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	
	if (gMainWindow == NULL)
	{
		ShowFormattedErrorDialog(GetLastError(), L"CreateWindowExW failed while creating the window!");

		return(0);
	}

	CalculateWindowPlacement();

	QueryPerformanceFrequency((LARGE_INTEGER*)&PerformanceFrequency);

	gBackBuffer.BitmapInfo.bmiHeader.biSize = sizeof(gBackBuffer.BitmapInfo.bmiHeader);

	gBackBuffer.BitmapInfo.bmiHeader.biWidth = RENDER_WIDTH;

	gBackBuffer.BitmapInfo.bmiHeader.biHeight = RENDER_HEIGHT;

	gBackBuffer.BitmapInfo.bmiHeader.biBitCount = RENDER_BPP;

	gBackBuffer.BitmapInfo.bmiHeader.biCompression = BI_RGB;

	gBackBuffer.BitmapInfo.bmiHeader.biPlanes = 1;

	gBackBuffer.Memory = VirtualAlloc(NULL, RENDER_AREA_MEM_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (gBackBuffer.Memory == NULL)
	{
		ShowFormattedErrorDialog(GetLastError(), L"Failed to allocate memory for drawing surface!");		

		return(0);
	}

	ResetEverythingForNewGame();

	gPlayers[PLAYER_ONE].ColorIndex = 0;

	gPlayers[PLAYER_TWO].ColorIndex = 1;

	PlayGameMusic(&gMusic);

	FindConnectedGamepads();

	gGamepadsExpected = gGamepadsPresent;

	// Main application loop. This runs forever, or until gShouldExit is set to false.
	// It should never block or the entire application will hang.

	while (!gShouldExit)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&FrameStart);

		while (PeekMessageW(&WindowMessage, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessageW(&WindowMessage);
		}		

		CollectPlayerInput();

		RenderFrameGraphics();		

		QueryPerformanceCounter((LARGE_INTEGER*)&FrameEnd);

		ElapsedMicroseconds = FrameEnd - FrameStart;

		ElapsedMicroseconds *= 1000000;

		ElapsedMicroseconds /= PerformanceFrequency;

		gTotalFramesRendered++;

		ElapsedMicrosecondsAccumulatorRaw += ElapsedMicroseconds;

		while (ElapsedMicroseconds < TARGET_MICROSECONDS_PER_FRAME)
		{
			ElapsedMicroseconds = FrameEnd - FrameStart;

			ElapsedMicroseconds *= 1000000;

			ElapsedMicroseconds /= PerformanceFrequency;

			QueryPerformanceCounter((LARGE_INTEGER*)&FrameEnd);

			// If we are less than 75% of the way through the current frame, then rest.
			// Sleep(1) is only anywhere near 1 millisecond if we have previously set the global
			// system timer resolution to 1ms or below using timeBeginPeriod.

			if (ElapsedMicroseconds < (TARGET_MICROSECONDS_PER_FRAME * 0.75f))
			{
				Sleep(1);
			}
		}

		ElapsedMicrosecondsAccumulatorCooked += ElapsedMicroseconds;
		
#ifdef _DEBUG
		if ((gTotalFramesRendered % CALCULATE_STATS_EVERY_X_FRAMES) == 0)
		{
			wchar_t WindowText[128] = { 0 };

			RawFPSAverage = 1.0f / (((float)ElapsedMicrosecondsAccumulatorRaw / CALCULATE_STATS_EVERY_X_FRAMES) * 0.000001f);

			CookedFPSAverage = 1.0f / (((float)ElapsedMicrosecondsAccumulatorCooked / CALCULATE_STATS_EVERY_X_FRAMES) * 0.000001f);

			ElapsedMicrosecondsAccumulatorRaw = 0;

			ElapsedMicrosecondsAccumulatorCooked = 0;
			
			_snwprintf_s(WindowText, _countof(WindowText), _TRUNCATE, L"%s - FPS: %.02f / %.02f *DEBUG BUILD*", GAME_NAME_W, RawFPSAverage, CookedFPSAverage);

			SetWindowTextW(WindowHandle, WindowText);
		}
#endif
	}

	return(0);
}

LRESULT CALLBACK WindowProc(_In_ HWND WindowHandle, _In_ UINT Message, _In_ WPARAM WParam, _In_ LPARAM LParam)
{
	LRESULT Result = 0;

	static BOOL CursorIsHidden = FALSE;

	switch (Message)
	{
		case WM_CLOSE:
		{
			gShouldExit = TRUE;

			break;
		}
		case WM_ACTIVATE:
		{
			if (WParam == 0)
			{
				// Our window has lost focus

				gWindowHasFocus = FALSE;				
			}
			else
			{
				// Our window has gained focus
				
				gWindowHasFocus = TRUE;
			}

			break;
		}
		case WM_SETCURSOR:
		{
			if ((LOWORD(LParam) == HTCLIENT) && (CursorIsHidden == FALSE))
			{
				SetTimer(WindowHandle, MOUSE_TIMER_ID, 900, NULL);
			}
			else if ((LOWORD(LParam) != HTCLIENT) && (CursorIsHidden == TRUE))
			{
				ShowCursor(TRUE);

				CursorIsHidden = FALSE;

				KillTimer(WindowHandle, MOUSE_TIMER_ID);
			}
			
			return(FALSE);
		}
		case WM_TIMER:
		{
			if (WParam == MOUSE_TIMER_ID)
			{
				POINT MouseCursorPosition;

				RECT  ClientRect;

				GetClientRect(WindowHandle, &ClientRect);

				GetCursorPos(&MouseCursorPosition);

				ScreenToClient(WindowHandle, &MouseCursorPosition);

				if ((MouseCursorPosition.x >= 0) & (MouseCursorPosition.y >= 0) & (MouseCursorPosition.x < ClientRect.right) & (MouseCursorPosition.y < ClientRect.bottom))
				{
					ShowCursor(FALSE);

					CursorIsHidden = TRUE;
				}

				KillTimer(WindowHandle, MOUSE_TIMER_ID);
			}			

			break;
		}
		default:
		{
			Result = DefWindowProcW(WindowHandle, Message, WParam, LParam);
		}
	}

	return(Result);
}

BOOL LoadAssets(void)
{
	typedef struct ASSET 
	{
		char* Name;

		void* Destination;

	} ASSET;

	ASSET Assets[] = { 
		{ "6x7Font.bmpx", &g6x7Font },
		{ "TopAccent.bmpx", &gTopAccentPicture },
		{ "XBox360Controller.bmpx", &gXboxGamepadPicture },
		{ "Menu.wav", &gMenuSound },
		{ "Choose.wav", &gMenuChooseSound },
		{ "Explosion.wav", &gExplosionSound },
		{ "L-Racers-BG2.ogg", &gMusic }
	};

	DWORD AssetFileAttributes = 0;

	AssetFileAttributes = GetFileAttributesA(ASSET_FILE_NAME_A);

	if ((AssetFileAttributes == INVALID_FILE_ATTRIBUTES) || (AssetFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		ShowFormattedErrorDialog(ERROR_FILE_NOT_FOUND, L"The asset file was not found! It must reside in the same directory as the executable.");

		return(FALSE);
	}

	for (int i = 0; i < _countof(Assets); i++)
	{
		if (LoadAssetFromArchive(ASSET_FILE_NAME_A, Assets[i].Name, Assets[i].Destination) != ERROR_SUCCESS)
		{
			ShowFormattedErrorDialog(GetLastError(), L"Failed to load asset!");

			return(FALSE);
		}
	}

	return(TRUE);
}

void ShowFormattedErrorDialog(_In_ DWORD LastErrorCode, _In_ wchar_t* ActivityDescription)
{
	DWORD CharsReturned = 0;

	wchar_t CompleteMessage[256] = { 0 };

	wchar_t LastErrorString[256] = { 0 };

	CharsReturned = FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		LastErrorCode,
		0,
		LastErrorString,
		_countof(LastErrorString),
		NULL);

	_snwprintf_s(
		CompleteMessage,
		_countof(CompleteMessage),
		_TRUNCATE,
		L"%s\nError: 0x%08lx %s", ActivityDescription, LastErrorCode, (CharsReturned ? LastErrorString : L"unknown"));

	MessageBoxW(NULL, CompleteMessage, L"ERROR", MB_OK | MB_ICONERROR);
}

// It's important to collect player input on every frame, even when input is disabled.
void CollectPlayerInput(void)
{
	gPlayers[PLAYER_ONE].Input.LeftKeyIsDown   = GetAsyncKeyState('A');

	gPlayers[PLAYER_ONE].Input.RightKeyIsDown  = GetAsyncKeyState('D');

	gPlayers[PLAYER_ONE].Input.UpKeyIsDown     = GetAsyncKeyState('W');

	gPlayers[PLAYER_ONE].Input.DownKeyIsDown   = GetAsyncKeyState('S');

	gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown = GetAsyncKeyState(VK_SPACE);

	if (gGamepadsPresent >= PLAYER_ONE)
	{
		if (XInputGetState(PLAYER_ONE, &gPlayers[PLAYER_ONE].Input.GamepadState) == ERROR_SUCCESS)
		{
			gPlayers[PLAYER_ONE].Input.LeftKeyIsDown |= gPlayers[PLAYER_ONE].Input.GamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;

			gPlayers[PLAYER_ONE].Input.RightKeyIsDown |= gPlayers[PLAYER_ONE].Input.GamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;

			gPlayers[PLAYER_ONE].Input.UpKeyIsDown |= gPlayers[PLAYER_ONE].Input.GamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;

			gPlayers[PLAYER_ONE].Input.DownKeyIsDown |= gPlayers[PLAYER_ONE].Input.GamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;

			gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown |= gPlayers[PLAYER_ONE].Input.GamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_A;
		}
		else
		{
			gGamepadsPresent = -1;

			gPreviousGameState = gGameState;

			gGameState = GAMESTATE_GAMEPADUNPLUGGED;
		}
	}

	// Only if we are playing a 2-player Local game does player 2 need any local input.
	if ((gGameState == GAMESTATE_ACTIONSCREEN || gGameState == GAMESTATE_POSTACTION) && 
		(gPlayers[PLAYER_TWO].AI == FALSE) && 
		(gNetworkData.NetworkInitialized == FALSE))
	{
		gPlayers[PLAYER_TWO].Input.LeftKeyIsDown   = GetAsyncKeyState(VK_LEFT);

		gPlayers[PLAYER_TWO].Input.RightKeyIsDown  = GetAsyncKeyState(VK_RIGHT);

		gPlayers[PLAYER_TWO].Input.UpKeyIsDown     = GetAsyncKeyState(VK_UP);

		gPlayers[PLAYER_TWO].Input.DownKeyIsDown   = GetAsyncKeyState(VK_DOWN);

		gPlayers[PLAYER_TWO].Input.ChooseKeyIsDown = GetAsyncKeyState(VK_RETURN);

		if (gGamepadsPresent >= PLAYER_TWO)
		{
			if (XInputGetState(PLAYER_TWO, &gPlayers[PLAYER_TWO].Input.GamepadState) == ERROR_SUCCESS)
			{
				gPlayers[PLAYER_TWO].Input.LeftKeyIsDown |= gPlayers[PLAYER_TWO].Input.GamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;

				gPlayers[PLAYER_TWO].Input.RightKeyIsDown |= gPlayers[PLAYER_TWO].Input.GamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;

				gPlayers[PLAYER_TWO].Input.UpKeyIsDown |= gPlayers[PLAYER_TWO].Input.GamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;

				gPlayers[PLAYER_TWO].Input.DownKeyIsDown |= gPlayers[PLAYER_TWO].Input.GamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;

				gPlayers[PLAYER_TWO].Input.ChooseKeyIsDown |= gPlayers[PLAYER_TWO].Input.GamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_A;
			}
			else
			{
				gGamepadsPresent = -1;

				gPreviousGameState = gGameState;

				gGameState = GAMESTATE_GAMEPADUNPLUGGED;
			}
		}
	}
	else
	{
		// If not in a 2-player local game, give the arrow keys to player 1, so that they
		// have their choice of what keys they want to use.

		gPlayers[PLAYER_ONE].Input.LeftKeyIsDown   |= GetAsyncKeyState(VK_LEFT);

		gPlayers[PLAYER_ONE].Input.RightKeyIsDown  |= GetAsyncKeyState(VK_RIGHT);

		gPlayers[PLAYER_ONE].Input.UpKeyIsDown     |= GetAsyncKeyState(VK_UP);

		gPlayers[PLAYER_ONE].Input.DownKeyIsDown   |= GetAsyncKeyState(VK_DOWN);

		gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown |= GetAsyncKeyState(VK_RETURN);
	}

	// If player has died, make their gamepad vibrate for a second.
	if (gPlayers[PLAYER_ONE].IsDead && gPlayers[PLAYER_ONE].Input.VibrateTime == 0)
	{
		XINPUT_VIBRATION Vibration = { 0 };

		Vibration.wLeftMotorSpeed = 32000;

		Vibration.wRightMotorSpeed = 16000;

		XInputSetState(PLAYER_ONE, &Vibration);

		gPlayers[PLAYER_ONE].Input.VibrateTime = gTotalFramesRendered;
	}

	// If the gamepad has been vibrating for more than a second, stop it.
	if (gPlayers[PLAYER_ONE].Input.VibrateTime != 0 && gPlayers[PLAYER_ONE].Input.VibrateTime <= gTotalFramesRendered - 60)
	{
		XINPUT_VIBRATION Vibration = { 0 };

		Vibration.wLeftMotorSpeed = 0;

		Vibration.wRightMotorSpeed = 0;

		XInputSetState(PLAYER_ONE, &Vibration);
	} 
 
	if (gPlayers[PLAYER_TWO].IsDead && gPlayers[PLAYER_TWO].Input.VibrateTime == 0)
	{
		XINPUT_VIBRATION Vibration = { 0 };

		Vibration.wLeftMotorSpeed = 32000;

		Vibration.wRightMotorSpeed = 16000;

		XInputSetState(PLAYER_TWO, &Vibration);

		gPlayers[PLAYER_TWO].Input.VibrateTime = gTotalFramesRendered;
	}

	// If the gamepad has been vibrating for more than a second, stop it.
	if (gPlayers[PLAYER_TWO].Input.VibrateTime != 0 && gPlayers[PLAYER_TWO].Input.VibrateTime <= gTotalFramesRendered - 60)
	{
		XINPUT_VIBRATION Vibration = { 0 };

		Vibration.wLeftMotorSpeed = 0;

		Vibration.wRightMotorSpeed = 0;

		XInputSetState(PLAYER_TWO, &Vibration);
	}

	if (gInputDisabled || !gWindowHasFocus)
	{
		if (gGameState != GAMESTATE_ACTIONSCREEN)
		{
			goto InputDisabled;
		}
	}

	// PPI = "Process Player Input"

	switch (gGameState)
	{
		case GAMESTATE_TITLESCREEN:	
		{
			PPI_TitleScreen();

			break;
		}
		case GAMESTATE_OPTIONSSCREEN:
		{
			PPI_Options();

			break;
		}
		case GAMESTATE_ACTIONSCREEN:
		{
			if (gNetworkData.NetworkInitialized == TRUE)
			{
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

					// We received a message. Example: "LR-INPUT:-32768:000000:032768:000001"
					if (strncmp(gNetworkData.RecvBuffer, "LR-INPUT:", 9) == 0)
					{
						char parse[7] = { 0 };
						
						strncpy_s(parse, sizeof(parse), gNetworkData.RecvBuffer + 9, 6);						

						gPlayers[PLAYER_TWO].Input.LeftKeyIsDown = (SHORT)atoi(parse);

						strncpy_s(parse, sizeof(parse), gNetworkData.RecvBuffer + 16, 6);

						gPlayers[PLAYER_TWO].Input.RightKeyIsDown = (SHORT)atoi(parse);

						strncpy_s(parse, sizeof(parse), gNetworkData.RecvBuffer + 23, 6);

						gPlayers[PLAYER_TWO].Input.UpKeyIsDown = (SHORT)atoi(parse);

						strncpy_s(parse, sizeof(parse), gNetworkData.RecvBuffer + 30, 6);

						gPlayers[PLAYER_TWO].Input.DownKeyIsDown = (SHORT)atoi(parse);
					}
				}

				_snprintf_s(
					gNetworkData.SendBuffer,
					sizeof(gNetworkData.SendBuffer),
					_TRUNCATE,
					"LR-INPUT:%06hi:%06hi:%06hi:%06hi",
					gPlayers[PLAYER_ONE].Input.LeftKeyIsDown,
					gPlayers[PLAYER_ONE].Input.RightKeyIsDown,
					gPlayers[PLAYER_ONE].Input.UpKeyIsDown,
					gPlayers[PLAYER_ONE].Input.DownKeyIsDown);

				sendto(
					gNetworkData.UnicastSocket,
					gNetworkData.SendBuffer,
					(int)strlen(gNetworkData.SendBuffer) + 1,
					0,
					(struct sockaddr*)&gNetworkData.RemoteAddress,
					sizeof(struct sockaddr_in));
			}

			PPI_Action();

			break;
		}
		case GAMESTATE_POSTACTION:
		{
			PPI_PostAction();			

			break;
		}
		case GAMESTATE_ENTERYOURNAME:
		{
			PPI_EnterYourName();

			break;
		}
		case GAMESTATE_HOSTLANGAME:
		{			
			PPI_HostLANGame();

			break;
		}
		case GAMESTATE_JOINLANGAME:
		{
			PPI_JoinLANGame();

			break;
		}
		case GAMESTATE_ENTERIP:
		{
			PPI_EnterIP();

			break;
		}
		case GAMESTATE_GAMEPADUNPLUGGED:
		{
			PPI_GamepadUnplugged();

			break;
		}
		default:
		{
			ASSERT(0, "UNKNOWN GAMESTATE");
		}
	}

	// Even when input is disabled, we still want to collect player input on every frame.
	// Trust me it's better this way.

InputDisabled:

	for (int i = 0; i < 2; i++)
	{
		gPlayers[i].Input.LeftKeyWasDown   = gPlayers[i].Input.LeftKeyIsDown;

		gPlayers[i].Input.RightKeyWasDown  = gPlayers[i].Input.RightKeyIsDown;

		gPlayers[i].Input.UpKeyWasDown     = gPlayers[i].Input.UpKeyIsDown;

		gPlayers[i].Input.DownKeyWasDown   = gPlayers[i].Input.DownKeyIsDown;

		gPlayers[i].Input.ChooseKeyWasDown = gPlayers[i].Input.ChooseKeyIsDown;
	}
}

void RenderFrameGraphics(void)
{
	switch (gGameState)
	{
		case GAMESTATE_TITLESCREEN:
		{
			Draw_TitleScreen();

			break;
		}
		case GAMESTATE_OPTIONSSCREEN:
		{
			Draw_Options();

			break;
		}
		case GAMESTATE_ACTIONSCREEN:
		{
			Draw_Action();

			break;
		}
		case GAMESTATE_POSTACTION:
		{
			Draw_PostAction();

			break;
		}
		case GAMESTATE_ENTERYOURNAME:
		{
			Draw_EnterYourName();

			break;
		}
		case GAMESTATE_HOSTLANGAME:
		{			
			Draw_HostLANGame();

			break;
		}
		case GAMESTATE_JOINLANGAME:
		{
			Draw_JoinLANGame();

			break;
		}
		case GAMESTATE_ENTERIP:
		{
			Draw_EnterIP();

			break;
		}
		case GAMESTATE_GAMEPADUNPLUGGED:
		{
			Draw_GamepadUnplugged();

			break;
		}
		default:
		{
			ASSERT(0, "UNKNOWN GAMESTATE");
		}
	}

	HDC DeviceContext = GetDC(gMainWindow);

	RECT ClientRect = { 0 };

	GetClientRect(gMainWindow, &ClientRect);

	if (gFullscreen)
	{
		StretchDIBits(DeviceContext,
			((gPrimaryMonitor.rcMonitor.right - gPrimaryMonitor.rcMonitor.left) / 2) - ((RENDER_WIDTH * gDrawScale) / 2),
			((gPrimaryMonitor.rcMonitor.bottom - gPrimaryMonitor.rcMonitor.top) / 2) - ((RENDER_HEIGHT * gDrawScale) / 2),
			RENDER_WIDTH * gDrawScale,
			RENDER_HEIGHT * gDrawScale,
			0,
			0,
			RENDER_WIDTH,
			RENDER_HEIGHT,
			gBackBuffer.Memory,
			&gBackBuffer.BitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY);
	}
	else
	{
		StretchDIBits(DeviceContext,
			ClientRect.left,
			ClientRect.top,
			ClientRect.right - ClientRect.left,
			ClientRect.bottom - ClientRect.top,
			0,
			0,
			RENDER_WIDTH,
			RENDER_HEIGHT,
			gBackBuffer.Memory,
			&gBackBuffer.BitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY);
	}

	ReleaseDC(gMainWindow, DeviceContext);
}

// If WINDOW_FLAG_HORIZONTALLY_CENTERED is specified, the x coordinate is ignored and may be zero.
// If WINDOW_FLAG_VERTICALLY_CENTERED is specified, the y coordinate is ignored and may be zero.
// BackgroundColor is ignored and may be NULL if WINDOW_FLAG_OPAQUE is not set.
// BorderColor is ignored and may be NULL if WINDOW_FLAG_BORDERED is not set.
// Either the BORDERED or the OPAQUE flag needs to be set, or both, or else the window would just be
// transparent and invisible. 
// The window border will cut into the inside of the window area.

void DrawRectangle(
	_In_opt_ uint16_t x,
	_In_opt_ uint16_t y,
	_In_ int16_t Width,
	_In_ int16_t Height,
	_In_opt_ PIXEL32* BorderColor,
	_In_opt_ PIXEL32* BackgroundColor,
	_In_opt_ PIXEL32* ShadowColor,
	_In_ DWORD Flags)
{
	if (Flags & DR_HORIZONTALLY_CENTERED)
	{
		x = (RENDER_WIDTH / 2) - (Width / 2);
	}

	if (Flags & DR_VERTICALLY_CENTERED)
	{
		y = (RENDER_HEIGHT / 2) - (Height / 2);
	}

	ASSERT((x + Width <= RENDER_WIDTH) && (y + Height <= RENDER_HEIGHT), "Window is off the screen!");

	ASSERT((Flags & DR_BORDERED) || (Flags & DR_OPAQUE), "Window must have either the BORDERED or the OPAQUE flags (or both) set!");

	int32_t StartingScreenPixel = ((RENDER_WIDTH * RENDER_HEIGHT) - RENDER_WIDTH) - (RENDER_WIDTH * y) + x;

	if (Flags & DR_OPAQUE)
	{
		ASSERT(BackgroundColor != NULL, "WINDOW_FLAG_OPAQUE is set but BackgroundColor is NULL!");

		for (int Row = 0; Row < Height; Row++)
		{
			int MemoryOffset = StartingScreenPixel - (RENDER_WIDTH * Row);

			// If the user wants rounded corners, don't draw the first and last pixels on the first and last rows.
			// Get a load of this sweet ternary action:
			for (int Pixel = ((Flags & DR_ROUNDED_CORNERS) && (Row == 0 || Row == Height - 1)) ? 1 : 0;
				Pixel < Width - ((Flags & DR_ROUNDED_CORNERS) && (Row == 0 || Row == Height - 1)) ? 1 : 0;
				Pixel++)
			{
				memcpy((PIXEL32*)gBackBuffer.Memory + MemoryOffset + Pixel, BackgroundColor, sizeof(PIXEL32));
			}
		}
	}

	if (Flags & DR_BORDERED)
	{
		ASSERT(BorderColor != NULL, "WINDOW_FLAG_BORDERED is set but BorderColor is NULL!");

		// Draw the top of the border.
		int MemoryOffset = StartingScreenPixel;

		for (int Pixel = ((Flags & DR_ROUNDED_CORNERS) ? 1 : 0);
			Pixel < Width - ((Flags & DR_ROUNDED_CORNERS) ? 1 : 0);
			Pixel++)
		{
			memcpy((PIXEL32*)gBackBuffer.Memory + MemoryOffset + Pixel, BorderColor, sizeof(PIXEL32));
		}

		// Draw the bottom of the border.
		MemoryOffset = StartingScreenPixel - (RENDER_WIDTH * (Height - 1));

		for (int Pixel = ((Flags & DR_ROUNDED_CORNERS) ? 1 : 0);
			Pixel < Width - ((Flags & DR_ROUNDED_CORNERS) ? 1 : 0);
			Pixel++)
		{
			memcpy((PIXEL32*)gBackBuffer.Memory + MemoryOffset + Pixel, BorderColor, sizeof(PIXEL32));
		}

		// Draw one pixel on the left side and the right for each row of the border, from the top down.
		for (int Row = 1; Row < Height - 1; Row++)
		{
			MemoryOffset = StartingScreenPixel - (RENDER_WIDTH * Row);

			memcpy((PIXEL32*)gBackBuffer.Memory + MemoryOffset, BorderColor, sizeof(PIXEL32));

			MemoryOffset = StartingScreenPixel - (RENDER_WIDTH * Row) + (Width - 1);

			memcpy((PIXEL32*)gBackBuffer.Memory + MemoryOffset, BorderColor, sizeof(PIXEL32));
		}

		// Recursion ahead!
		// If the user wants a thick window, just draw a smaller concentric bordered window inside the existing window.
		if (Flags & DR_THICK)
		{
			DrawRectangle(x + 1, y + 1, Width - 2, Height - 2, BorderColor, NULL, NULL, DR_BORDERED);
		}
	}

	// TODO: If a window was placed at the edge of the screen, the shadow effect might attempt
	// to draw off-screen and crash! i.e. make sure there's room to draw the shadow before attempting!
	if (Flags & DR_SHADOW)
	{
		ASSERT(ShadowColor != NULL, "WINDOW_FLAG_SHADOW is set but ShadowColor is NULL!");

		// Draw the bottom of the shadow.
		int MemoryOffset = StartingScreenPixel - (RENDER_WIDTH * Height);

		for (int Pixel = 1;
			Pixel < Width + ((Flags & DR_ROUNDED_CORNERS) ? 0 : 1);
			Pixel++)
		{
			memcpy((PIXEL32*)gBackBuffer.Memory + MemoryOffset + Pixel, ShadowColor, sizeof(PIXEL32));
		}

		// Draw one pixel on the right side for each row of the border, from the top down.
		for (int Row = 1; Row < Height; Row++)
		{
			MemoryOffset = StartingScreenPixel - (RENDER_WIDTH * Row) + Width;

			memcpy((PIXEL32*)gBackBuffer.Memory + MemoryOffset, ShadowColor, sizeof(PIXEL32));
		}

		// Draw one shadow pixel in the bottom-right corner to compensate for rounded corner.
		if (Flags & DR_ROUNDED_CORNERS)
		{
			MemoryOffset = StartingScreenPixel - (RENDER_WIDTH * (Height - 1)) + (Width - 1);

			memcpy((PIXEL32*)gBackBuffer.Memory + MemoryOffset, ShadowColor, sizeof(PIXEL32));
		}
	}
}

// Loads any defined asset type such as a *.wav, *.ogg, *.tmx, or *.bmpx from an asset file into heap memory.
// The asset file is a compressed zip archive. The asset file is created with a customized version of miniz.
// The only difference is that some of the zip file metadata constants were changed so that tools such as 7-zip, WinRAR, etc., 
// will not be able to recognize the file. The asset file currently does not support any directory structure.
DWORD LoadAssetFromArchive(_In_ char* ArchiveName, _In_ char* AssetFileName, _Inout_ void* Resource)
{
	DWORD Error = ERROR_SUCCESS;

	mz_zip_archive Archive = { 0 };

	BYTE* DecompressedBuffer = NULL;

	size_t DecompressedSize = 0;

	BOOL FileFoundInArchive = FALSE;

	char* FileExtension = NULL;

	if ((mz_zip_reader_init_file(&Archive, ArchiveName, 0)) == FALSE)
	{
		Error = mz_zip_get_last_error(&Archive);		

		goto Exit;
	}	

	// Iterate through each file in the archive until we find the file we are looking for.

	for (uint32_t FileIndex = 0; FileIndex < mz_zip_reader_get_num_files(&Archive); FileIndex++)
	{
		mz_zip_archive_file_stat CompressedFileStatistics = { 0 };

		if (mz_zip_reader_file_stat(&Archive, FileIndex, &CompressedFileStatistics) == MZ_FALSE)
		{
			Error = mz_zip_get_last_error(&Archive);			

			goto Exit;
		}

		if (_stricmp(CompressedFileStatistics.m_filename, AssetFileName) == 0)
		{
			FileFoundInArchive = TRUE;

			if ((DecompressedBuffer = mz_zip_reader_extract_to_heap(&Archive, FileIndex, &DecompressedSize, 0)) == NULL)
			{
				Error = mz_zip_get_last_error(&Archive);				

				goto Exit;
			}			

			break;
		}
	}

	if (FileFoundInArchive == FALSE)
	{
		Error = ERROR_FILE_NOT_FOUND;		

		goto Exit;
	}	

	for (int i = (int)strlen(AssetFileName) - 1; i > 0; i--)
	{
		FileExtension = &AssetFileName[i];

		if (FileExtension[0] == '.')
		{
			break;
		}
	}

	if (FileExtension && _stricmp(FileExtension, ".bmpx") == 0)
	{
		Error = Load32BppBitmapFromMemory(DecompressedBuffer, Resource);
	}
	else if (FileExtension && _stricmp(FileExtension, ".wav") == 0)
	{
		Error = LoadWavFromMemory(DecompressedBuffer, Resource);
	}
	else if (FileExtension && _stricmp(FileExtension, ".ogg") == 0)
	{
		Error = LoadOggFromMemory(DecompressedBuffer, (uint32_t)DecompressedSize, Resource);
	}
	else
	{
		Error = ERROR_BAD_FILE_TYPE;

		goto Exit;
	}

Exit:

	mz_zip_reader_end(&Archive);

	SetLastError(Error);

	return(Error);
}

// This function should only be called from the LoadAssetFromArchive function.
// It takes raw bitmap data that has been extracted to the heap and turns it into
// a usable GAMEBITMAP structure.
DWORD Load32BppBitmapFromMemory(_In_ void* Buffer, _Inout_ GAMEBITMAP* GameBitmap)
{
	DWORD Error = ERROR_SUCCESS;

	WORD BitmapHeader = 0;

	DWORD PixelDataOffset = 0;

	memcpy(&BitmapHeader, Buffer, sizeof(WORD));

	if (BitmapHeader != 0x4d42) // "BM" backwards
	{
		Error = ERROR_INVALID_DATA;

		goto Exit;
	}

	memcpy(&PixelDataOffset, (BYTE*)Buffer + 0xA, sizeof(DWORD));

	memcpy(&GameBitmap->BitmapInfo.bmiHeader, (BYTE*)Buffer + 0xE, sizeof(BITMAPINFOHEADER));

	GameBitmap->Memory = (BYTE*)Buffer + PixelDataOffset;

Exit:

	return(Error);
}

// A compressed *.wav file has been extracted from the assets file and now resides
// in heap memory. This function parses that memory and populates a GAMESOUND data structure.
// This function should only be called from LoadAssetFromArchive.
DWORD LoadWavFromMemory(_In_ void* Buffer, _Inout_ GAMESOUND* GameSound)
{
	DWORD Error = ERROR_SUCCESS;

	DWORD RIFF = 0;

	uint16_t DataChunkOffset = 0;

	DWORD DataChunkSearcher = 0;

	BOOL DataChunkFound = FALSE;

	DWORD DataChunkSize = 0;

	memcpy(&RIFF, Buffer, sizeof(DWORD));

	if (RIFF != 0x46464952) // "RIFF" backwards
	{
		Error = ERROR_FILE_INVALID;

		//"[%s] First four bytes of memory buffer are not 'RIFF'!"

		goto Exit;
	}

	// 20 bytes into a wav file, there is a WAVEFORMATEX data structure.
	memcpy(&GameSound->WaveFormat, (BYTE*)Buffer + 20, sizeof(WAVEFORMATEX));

	if (GameSound->WaveFormat.nBlockAlign != ((GameSound->WaveFormat.nChannels * GameSound->WaveFormat.wBitsPerSample) / 8) ||
		(GameSound->WaveFormat.wFormatTag != WAVE_FORMAT_PCM) ||
		(GameSound->WaveFormat.wBitsPerSample != 16))
	{
		Error = ERROR_DATATYPE_MISMATCH;

		// This wav data in the memory buffer did not meet the format requirements! Only PCM format, 44.1KHz, 16 bits per sample wav files are supported. 0x%08lx!", 

		goto Exit;
	}

	// We search for the data chunk, which is an indeterminite number of bytes into the file/buffer.
	while (DataChunkFound == FALSE)
	{
		memcpy(&DataChunkSearcher, (BYTE*)Buffer + DataChunkOffset, sizeof(DWORD));

		if (DataChunkSearcher == 0x61746164) // 'data', backwards
		{
			DataChunkFound = TRUE;

			break;
		}
		else
		{
			DataChunkOffset += 4;
		}

		if (DataChunkOffset > 256)
		{
			Error = ERROR_DATATYPE_MISMATCH;

			//Data chunk not found within first 256 bytes of the memory buffer! 0x%08lx!"

			goto Exit;
		}
	}

	memcpy(&DataChunkSize, (BYTE*)Buffer + DataChunkOffset + 4, sizeof(DWORD));

	GameSound->Buffer.Flags = XAUDIO2_END_OF_STREAM;

	GameSound->Buffer.AudioBytes = DataChunkSize;

	GameSound->Buffer.pAudioData = (BYTE*)Buffer + DataChunkOffset + 8;

Exit:

	return(Error);
}

// This function should only be called from the LoadAssetFromArchive function.
// It takes raw OGG Vorbis data that has been extracted to the heap and turns it into
// a usable GAMESOUND structure.
DWORD LoadOggFromMemory(_In_ void* Buffer, _In_ uint32_t BufferSize, _Inout_ GAMESOUND* GameSound)
{
	DWORD Error = ERROR_SUCCESS;

	int SamplesDecoded = 0;

	int Channels = 0;

	int SampleRate = 0;

	short* DecodedAudio = NULL;

	SamplesDecoded = stb_vorbis_decode_memory(Buffer, (int)BufferSize, &Channels, &SampleRate, &DecodedAudio);

	if (SamplesDecoded < 1)
	{
		Error = ERROR_BAD_COMPRESSION_BUFFER;		

		goto Exit;
	}

	GameSound->WaveFormat.wFormatTag = WAVE_FORMAT_PCM;

	GameSound->WaveFormat.nChannels = (WORD)Channels;

	GameSound->WaveFormat.nSamplesPerSec = SampleRate;

	GameSound->WaveFormat.nAvgBytesPerSec = GameSound->WaveFormat.nSamplesPerSec * GameSound->WaveFormat.nChannels * 2;

	GameSound->WaveFormat.nBlockAlign = GameSound->WaveFormat.nChannels * 2;

	GameSound->WaveFormat.wBitsPerSample = 16;

	GameSound->Buffer.Flags = XAUDIO2_END_OF_STREAM;

	GameSound->Buffer.AudioBytes = SamplesDecoded * GameSound->WaveFormat.nChannels * 2;

	GameSound->Buffer.pAudioData = (const BYTE*)DecodedAudio;

Exit:

	return(Error);
}

void ResetEverythingForNewGame(void)
{	
	if (gNetworkData.NetworkInitialized)
	{
		if (gNetworkData.IsHost)
		{
			gPlayers[PLAYER_ONE].CurrentPosition = (POINT){ .x = 64, .y = (RENDER_HEIGHT / 2) };

			gPlayers[PLAYER_ONE].CurrentDirection = DIRECTION_RIGHT;

			gPlayers[PLAYER_TWO].CurrentPosition = (POINT){ .x = RENDER_WIDTH - 64, .y = (RENDER_HEIGHT / 2) };

			gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_LEFT;
		}
		else
		{
			gPlayers[PLAYER_ONE].CurrentPosition = (POINT){ .x = RENDER_WIDTH - 64, .y = (RENDER_HEIGHT / 2) };

			gPlayers[PLAYER_ONE].CurrentDirection = DIRECTION_LEFT;

			gPlayers[PLAYER_TWO].CurrentPosition = (POINT){ .x = 64, .y = (RENDER_HEIGHT / 2) };

			gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_RIGHT;
		}
	}
	else
	{
		gPlayers[PLAYER_ONE].CurrentPosition = (POINT){ .x = 64, .y = (RENDER_HEIGHT / 2) };

		gPlayers[PLAYER_ONE].CurrentDirection = DIRECTION_RIGHT;

		gPlayers[PLAYER_TWO].CurrentPosition = (POINT){ .x = RENDER_WIDTH - 64, .y = (RENDER_HEIGHT / 2) };

		gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_LEFT;
	}

	gPlayers[PLAYER_ONE].IsDead = FALSE;

	gPlayers[PLAYER_ONE].AI = FALSE;

	memset(gPlayers[PLAYER_ONE].TurnPoints, 0, sizeof(gPlayers[PLAYER_ONE].TurnPoints));
	
	gPlayers[PLAYER_ONE].TurnPoints[0].x = gPlayers[PLAYER_ONE].CurrentPosition.x;
	
	gPlayers[PLAYER_ONE].TurnPoints[0].y = gPlayers[PLAYER_ONE].CurrentPosition.y;	

	gPlayers[PLAYER_ONE].Input.VibrateTime = 0;


	gPlayers[PLAYER_TWO].IsDead = FALSE;	

	memset(gPlayers[PLAYER_TWO].TurnPoints, 0, sizeof(gPlayers[PLAYER_TWO].TurnPoints));

	gPlayers[PLAYER_TWO].TurnPoints[0].x = gPlayers[PLAYER_TWO].CurrentPosition.x;

	gPlayers[PLAYER_TWO].TurnPoints[0].y = gPlayers[PLAYER_TWO].CurrentPosition.y;	

	gPlayers[PLAYER_TWO].Input.VibrateTime = 0;
}




// This function draws any sized bitmap onto the global backbuffer. Sprites, text strings, etc.
// WARNING: Currently there is no safeguard preventing you from trying to draw pixels outside
// of the screen area, and attempting to do so will crash the game if the area to be
// drawn to falls outside of valid gBackBuffer memory!
void Blit32BppBitmap(_In_ GAMEBITMAP* GameBitmap, _In_ int16_t x, _In_ int16_t y)
{
	int32_t StartingScreenPixel = ((RENDER_WIDTH * RENDER_HEIGHT) - RENDER_WIDTH) - (RENDER_WIDTH * y) + x;

	int32_t StartingBitmapPixel = ((GameBitmap->BitmapInfo.bmiHeader.biWidth * GameBitmap->BitmapInfo.bmiHeader.biHeight) - \
		GameBitmap->BitmapInfo.bmiHeader.biWidth);

	int32_t MemoryOffset = 0;

	int32_t BitmapOffset = 0;

	PIXEL32 BitmapPixel = { 0 };

	PIXEL32 BackgroundPixel = { 0 };
	
	PIXEL32 BlendedPixel = { 0 };

	for (int16_t YPixel = 0; YPixel < GameBitmap->BitmapInfo.bmiHeader.biHeight; YPixel++)
	{
		for (int16_t XPixel = 0; XPixel < GameBitmap->BitmapInfo.bmiHeader.biWidth; XPixel++)
		{
			MemoryOffset = StartingScreenPixel + XPixel - (RENDER_WIDTH * YPixel);

			BitmapOffset = StartingBitmapPixel + XPixel - (GameBitmap->BitmapInfo.bmiHeader.biWidth * YPixel);

			memcpy(&BitmapPixel, (PIXEL32*)GameBitmap->Memory + BitmapOffset, sizeof(PIXEL32));

			memcpy(&BackgroundPixel, (PIXEL32*)gBackBuffer.Memory + MemoryOffset, sizeof(PIXEL32));

			// FUN FACT: Dividing by 255 is more mathematically accurate, but dividing by 256 is faster, because
			// the compiler understands that dividing by 256 is equivalent to shifting right 8 bits ( >> 8) which is faster than dividing.
			// But using 256/bitwise shift by 8, you'll get off-by-one colors that aren't perfect. You might not notice.				

			BlendedPixel.Colors.Blue = (uint8_t)(BitmapPixel.Colors.Blue * BitmapPixel.Colors.Alpha / 255 + BackgroundPixel.Colors.Blue * (255 - BitmapPixel.Colors.Alpha) / 255);

			BlendedPixel.Colors.Green = (uint8_t)(BitmapPixel.Colors.Green * BitmapPixel.Colors.Alpha / 255 + BackgroundPixel.Colors.Green * (255 - BitmapPixel.Colors.Alpha) / 255);

			BlendedPixel.Colors.Red = (uint8_t)(BitmapPixel.Colors.Red * BitmapPixel.Colors.Alpha / 255 + BackgroundPixel.Colors.Red * (255 - BitmapPixel.Colors.Alpha) / 255);

			memcpy((PIXEL32*)gBackBuffer.Memory + MemoryOffset, &BlendedPixel, sizeof(PIXEL32));
		}
	}
}

void BlitString(_In_ char* String, _In_ GAMEBITMAP* FontSheet, _In_ PIXEL32* Color, _In_ uint16_t x, _In_ uint16_t y, _In_opt_ PIXEL32* ShadowColor, _In_ DWORD Flags)
{
	// Map any char value to an offset dictated by the g6x7Font ordering.
	// 0xab and 0xbb are extended ASCII characters that look like double angle brackets.
	// We use them as a cursor in menus.
	static const int32_t FontCharacterPixelOffset[] = {
		//  .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. ..
		93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
		//     !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?
		94,64,87,66,67,68,70,85,72,73,71,77,88,74,91,92,52,53,54,55,56,57,58,59,60,61,86,84,89,75,90,93,
		//  @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _
		65,0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,80,78,81,69,76,
		//  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~  ..
		62,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,82,79,83,63,93,
		//  .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. ..
		93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
		//  .. .. .. .. .. .. .. .. .. .. .. bb .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. ab .. .. .. ..
		93,93,93,93,93,93,93,93,93,93,93,96,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,95,93,93,93,93,
		//  .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. ..
		93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
		//  .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. F2 .. .. .. .. .. .. .. .. .. .. .. .. ..
		93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,97,93,93,93,93,93,93,93,93,93,93,93,93,93 };

	uint16_t CharWidth = (uint16_t)FontSheet->BitmapInfo.bmiHeader.biWidth / FONT_SHEET_CHARACTERS_PER_ROW;

	uint16_t CharHeight = (uint16_t)FontSheet->BitmapInfo.bmiHeader.biHeight;

	uint16_t BytesPerCharacter = (CharWidth * CharHeight * (FontSheet->BitmapInfo.bmiHeader.biBitCount / 8));

	uint16_t StringLength = (uint16_t)strlen(String);

	GAMEBITMAP StringBitmap = { 0 };

	StringBitmap.BitmapInfo.bmiHeader.biBitCount = RENDER_BPP;

	StringBitmap.BitmapInfo.bmiHeader.biHeight = CharHeight;

	StringBitmap.BitmapInfo.bmiHeader.biWidth = CharWidth * StringLength;

	StringBitmap.BitmapInfo.bmiHeader.biPlanes = 1;

	StringBitmap.BitmapInfo.bmiHeader.biCompression = BI_RGB;

	StringBitmap.Memory = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ((size_t)BytesPerCharacter * (size_t)StringLength));

	for (int Character = 0; Character < StringLength; Character++)
	{
		int StartingFontSheetPixel = 0;

		int FontSheetOffset = 0;

		int StringBitmapOffset = 0;

		PIXEL32 FontSheetPixel = { 0 };

		StartingFontSheetPixel = (FontSheet->BitmapInfo.bmiHeader.biWidth * FontSheet->BitmapInfo.bmiHeader.biHeight) - \
			FontSheet->BitmapInfo.bmiHeader.biWidth + (CharWidth * FontCharacterPixelOffset[(uint8_t)String[Character]]);

		for (int YPixel = 0; YPixel < CharHeight; YPixel++)
		{
			for (int XPixel = 0; XPixel < CharWidth; XPixel++)
			{
				FontSheetOffset = StartingFontSheetPixel + XPixel - (FontSheet->BitmapInfo.bmiHeader.biWidth * YPixel);

				StringBitmapOffset = (Character * CharWidth) + ((StringBitmap.BitmapInfo.bmiHeader.biWidth * StringBitmap.BitmapInfo.bmiHeader.biHeight) - \
					StringBitmap.BitmapInfo.bmiHeader.biWidth) + XPixel - (StringBitmap.BitmapInfo.bmiHeader.biWidth) * YPixel;


				// NOTE: memcpy_s is safer but significantly slower.
				
				memcpy(&FontSheetPixel, (PIXEL32*)FontSheet->Memory + FontSheetOffset, sizeof(PIXEL32));				
				
				FontSheetPixel.Colors.Red   = Color->Colors.Red;

				FontSheetPixel.Colors.Green = Color->Colors.Green;

				FontSheetPixel.Colors.Blue  = Color->Colors.Blue;

				// NOTE: memcpy_s is safer but significantly slower.
				#pragma warning(suppress: 6387) // mem + offset could be 0
				memcpy((PIXEL32*)StringBitmap.Memory + StringBitmapOffset, &FontSheetPixel, sizeof(PIXEL32));				
			}
		}
	}

	// WARNING: RECURSION AHEAD
	if (Flags & BS_SHADOW)
	{
		ASSERT(ShadowColor != NULL, "shadow color is null, but caller wants a shadow");

		BlitString(String, &g6x7Font, ShadowColor, x, y + 1, NULL, 0);
	}

	Blit32BppBitmap(&StringBitmap, x, y);

	if (StringBitmap.Memory)
	{
		HeapFree(GetProcessHeap(), 0, StringBitmap.Memory);
	}
}

// All lines in this game will be either horizontal or vertical.
// Origin (0,0) is the bottom-left corner of the screen.
void DrawLine(_In_ int x1, _In_ int y1, _In_ int x2, _In_ int y2, _In_ PIXEL32 Color)
{	
	if (x1 == x2)
	{
		// we are drawing a vertical line
		if (y1 < y2)
		{
			// we are drawing upwards
			for (int ypixel = y1; ypixel < y2; ypixel++)
			{
				memcpy((PIXEL32*)gBackBuffer.Memory + x1 + (RENDER_WIDTH * ypixel),
					&Color,
					sizeof(PIXEL32));
			}
		}
		else if (y1 > y2)
		{
			// we are drawing downwards
			for (int ypixel = y1; ypixel > y2; ypixel--)
			{
				memcpy((PIXEL32*)gBackBuffer.Memory + x1 + (RENDER_WIDTH * ypixel),
					&Color,
					sizeof(PIXEL32));
			}
		}
	}
	else if (y1 == y2)
	{
		// we are drawing a horizontal line

		if (x1 < x2)
		{
			// we are drawing from left to right
			for (int xpixel = x1; xpixel < x2; xpixel++)
			{
				memcpy((PIXEL32*)gBackBuffer.Memory + xpixel + (RENDER_WIDTH * y1),
					&Color,
					sizeof(PIXEL32));
			}
		}
		else if (x1 > x2)
		{
			// we are drawing right to left
			for (int xpixel = x1; xpixel > x2; xpixel--)
			{
				memcpy((PIXEL32*)gBackBuffer.Memory + xpixel + (RENDER_WIDTH * y1),
					&Color,
					sizeof(PIXEL32));
			}
		}
	}
	else
	{
		ASSERT(0, "trying to draw a diagonal line?")
	}
}

// Initialize the Microsoft XAudio2 sound library once at the very beginning of the game.
HRESULT InitializeSoundEngine(void)
{
	HRESULT Result = S_OK;

	WAVEFORMATEX SfxWaveFormat = { 0 };

	WAVEFORMATEX MusicWaveFormat = { 0 };


	Result = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (Result != S_OK)
	{
		ShowFormattedErrorDialog(Result, L"CoInitializeEx failed during audio setup!");

		goto Exit;
	}

	Result = XAudio2Create(&gXAudio, 0, XAUDIO2_ANY_PROCESSOR);

	if (FAILED(Result))
	{
		ShowFormattedErrorDialog(Result, L"XAudio2Create failed during audio setup!");

		goto Exit;
	}

	Result = gXAudio->lpVtbl->CreateMasteringVoice(gXAudio, &gXAudioMasteringVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL, 0);

	if (FAILED(Result))
	{
		ShowFormattedErrorDialog(Result, L"CreateMasteringVoice failed during audio setup!");

		goto Exit;
	}

	SfxWaveFormat.wFormatTag = WAVE_FORMAT_PCM;

	SfxWaveFormat.nChannels = 1; // Mono

	SfxWaveFormat.nSamplesPerSec = 44100;

	SfxWaveFormat.nAvgBytesPerSec = SfxWaveFormat.nSamplesPerSec * SfxWaveFormat.nChannels * 2;

	SfxWaveFormat.nBlockAlign = SfxWaveFormat.nChannels * 2;

	SfxWaveFormat.wBitsPerSample = 16;

	SfxWaveFormat.cbSize = 0x6164;

	for (uint8_t Counter = 0; Counter < NUMBER_OF_SFX_SOURCE_VOICES; Counter++)
	{
		Result = gXAudio->lpVtbl->CreateSourceVoice(gXAudio, &gXAudioSFXSourceVoice[Counter], &SfxWaveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL);

		if (Result != S_OK)
		{
			ShowFormattedErrorDialog(Result, L"CreateSourceVoice failed during audio setup!");

			goto Exit;
		}

		gXAudioSFXSourceVoice[Counter]->lpVtbl->SetVolume(gXAudioSFXSourceVoice[Counter], ((float)gSFXVolume / 10), XAUDIO2_COMMIT_NOW);
	}

	MusicWaveFormat.wFormatTag = WAVE_FORMAT_PCM;

	MusicWaveFormat.nChannels = 2; // Stereo

	MusicWaveFormat.nSamplesPerSec = 44100;

	MusicWaveFormat.nAvgBytesPerSec = MusicWaveFormat.nSamplesPerSec * MusicWaveFormat.nChannels * 2;

	MusicWaveFormat.nBlockAlign = MusicWaveFormat.nChannels * 2;

	MusicWaveFormat.wBitsPerSample = 16;

	MusicWaveFormat.cbSize = 0;

	Result = gXAudio->lpVtbl->CreateSourceVoice(gXAudio, &gXAudioMusicSourceVoice, &MusicWaveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL);

	if (Result != S_OK)
	{
		ShowFormattedErrorDialog(Result, L"CreateSourceVoice failed during audio setup!");

		goto Exit;
	}

	gXAudioMusicSourceVoice->lpVtbl->SetVolume(gXAudioMusicSourceVoice, ((float)gMusicVolume / 10), XAUDIO2_COMMIT_NOW);


Exit:

	return(Result);
}

void PlayGameSound(_In_ GAMESOUND* GameSound)
{
	gXAudioSFXSourceVoice[gSFXSourceVoiceSelector]->lpVtbl->SubmitSourceBuffer(gXAudioSFXSourceVoice[gSFXSourceVoiceSelector], &GameSound->Buffer, NULL);

	gXAudioSFXSourceVoice[gSFXSourceVoiceSelector]->lpVtbl->Start(gXAudioSFXSourceVoice[gSFXSourceVoiceSelector], 0, XAUDIO2_COMMIT_NOW);

	gSFXSourceVoiceSelector++;

	if (gSFXSourceVoiceSelector > (NUMBER_OF_SFX_SOURCE_VOICES - 1))
	{
		gSFXSourceVoiceSelector = 0;
	}
}

void PlayGameMusic(_In_ GAMESOUND* Music)
{
	Music->Buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	gXAudioMusicSourceVoice->lpVtbl->SubmitSourceBuffer(gXAudioMusicSourceVoice, &Music->Buffer, NULL);

	gXAudioMusicSourceVoice->lpVtbl->Start(gXAudioMusicSourceVoice, 0, XAUDIO2_COMMIT_NOW);
}

void CalculateAIMovement(void)
{
	
	DIRECTION PreviousDirection = gPlayers[PLAYER_TWO].CurrentDirection;	

	PIXEL32 DestinationPixel = { 0 };

	PIXEL32 ChoiceA = { 0 };

	PIXEL32 ChoiceB = { 0 };

	BOOL ChoiceAIsSafe = FALSE;

	BOOL ChoiceBIsSafe = FALSE;

	unsigned int Random = 0;

	rand_s(&Random);

	if (gPlayers[PLAYER_TWO].CurrentDirection == DIRECTION_LEFT)
	{
		// Look to the left
		memcpy(
			&DestinationPixel,
			(PIXEL32*)gBackBuffer.Memory + (gPlayers[PLAYER_TWO].CurrentPosition.x - 1) + (RENDER_WIDTH * gPlayers[PLAYER_TWO].CurrentPosition.y),
			sizeof(PIXEL32));

		
		if ((DestinationPixel.Bytes != 0 && DestinationPixel.Bytes != (DARK_GRAY_PIXEL).Bytes) ||
			(Random % 199 == 0))
		{	
			// We have to turn up or down. We will die if we keep going left, and we cannot turn back on ourselves.

			// look up
			memcpy(
				&ChoiceA,
				(PIXEL32*)gBackBuffer.Memory + gPlayers[PLAYER_TWO].CurrentPosition.x + (RENDER_WIDTH * (gPlayers[PLAYER_TWO].CurrentPosition.y + 1)),
				sizeof(PIXEL32));

			// look down
			memcpy(
				&ChoiceB,
				(PIXEL32*)gBackBuffer.Memory + gPlayers[PLAYER_TWO].CurrentPosition.x + (RENDER_WIDTH * (gPlayers[PLAYER_TWO].CurrentPosition.y - 1)),
				sizeof(PIXEL32));			
				
			ChoiceAIsSafe = (ChoiceA.Bytes == 0 || ChoiceA.Bytes == (DARK_GRAY_PIXEL).Bytes);

			ChoiceBIsSafe = (ChoiceB.Bytes == 0 || ChoiceB.Bytes == (DARK_GRAY_PIXEL).Bytes);
			
			if (ChoiceAIsSafe & ChoiceBIsSafe)
			{
				// Both choices are safe, coin flip between them

				rand_s(&Random);

				if (Random % 2 == 0)
				{
					gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_UP;
				}
				else
				{
					gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_DOWN;
				}
			}
			else if (ChoiceAIsSafe)
			{
				gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_UP;
			}
			else if (ChoiceBIsSafe)
			{
				gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_DOWN;
			}
			else
			{
				// do nothing
			}
		}
	}
	else if (gPlayers[PLAYER_TWO].CurrentDirection == DIRECTION_RIGHT)
	{
		// Look to the right
		memcpy(
			&DestinationPixel,
			(PIXEL32*)gBackBuffer.Memory + (gPlayers[PLAYER_TWO].CurrentPosition.x + 1) + (RENDER_WIDTH * gPlayers[PLAYER_TWO].CurrentPosition.y),
			sizeof(PIXEL32));


		if ((DestinationPixel.Bytes != 0 && DestinationPixel.Bytes != (DARK_GRAY_PIXEL).Bytes) ||
			(Random % 211 == 0))
		{
			// We have to turn up or down. We will die if we keep going left, and we cannot turn back on ourselves.

			// look up
			memcpy(
				&ChoiceA,
				(PIXEL32*)gBackBuffer.Memory + gPlayers[PLAYER_TWO].CurrentPosition.x + (RENDER_WIDTH * (gPlayers[PLAYER_TWO].CurrentPosition.y + 1)),
				sizeof(PIXEL32));

			// look down
			memcpy(
				&ChoiceB,
				(PIXEL32*)gBackBuffer.Memory + gPlayers[PLAYER_TWO].CurrentPosition.x + (RENDER_WIDTH * (gPlayers[PLAYER_TWO].CurrentPosition.y - 1)),
				sizeof(PIXEL32));

			ChoiceAIsSafe = (ChoiceA.Bytes == 0 || ChoiceA.Bytes == (DARK_GRAY_PIXEL).Bytes);

			ChoiceBIsSafe = (ChoiceB.Bytes == 0 || ChoiceB.Bytes == (DARK_GRAY_PIXEL).Bytes);

			if (ChoiceAIsSafe & ChoiceBIsSafe)
			{
				// Both choices are safe, coin flip between them

				rand_s(&Random);

				if (Random % 2 == 0)
				{
					gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_UP;
				}
				else
				{
					gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_DOWN;
				}
			}
			else if (ChoiceAIsSafe)
			{
				gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_UP;
			}
			else if (ChoiceBIsSafe)
			{
				gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_DOWN;
			}
			else
			{
				// do nothing
			}
		}
	}
	else if (gPlayers[PLAYER_TWO].CurrentDirection == DIRECTION_UP)
	{
		// Look up
		memcpy(
			&DestinationPixel,
			(PIXEL32*)gBackBuffer.Memory + gPlayers[PLAYER_TWO].CurrentPosition.x + (RENDER_WIDTH * (gPlayers[PLAYER_TWO].CurrentPosition.y + 1)),
			sizeof(PIXEL32));

		if ((DestinationPixel.Bytes != 0 && DestinationPixel.Bytes != (DARK_GRAY_PIXEL).Bytes) ||
			(Random % 223 == 0))
		{
			// We have to turn right or left. We will die if we keep going up, and we cannot turn back on ourselves.

			// look right
			memcpy(
				&ChoiceA,
				(PIXEL32*)gBackBuffer.Memory + (gPlayers[PLAYER_TWO].CurrentPosition.x + 1) + (RENDER_WIDTH * gPlayers[PLAYER_TWO].CurrentPosition.y),
				sizeof(PIXEL32));

			// look left
			memcpy(
				&ChoiceB,
				(PIXEL32*)gBackBuffer.Memory + (gPlayers[PLAYER_TWO].CurrentPosition.x - 1) + (RENDER_WIDTH * gPlayers[PLAYER_TWO].CurrentPosition.y),
				sizeof(PIXEL32));

			ChoiceAIsSafe = (ChoiceA.Bytes == 0 || ChoiceA.Bytes == (DARK_GRAY_PIXEL).Bytes);

			ChoiceBIsSafe = (ChoiceB.Bytes == 0 || ChoiceB.Bytes == (DARK_GRAY_PIXEL).Bytes);

			if (ChoiceAIsSafe & ChoiceBIsSafe)
			{
				// Both choices are safe, coin flip between them

				rand_s(&Random);

				if (Random % 2 == 0)
				{
					gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_RIGHT;
				}
				else
				{
					gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_LEFT;
				}
			}
			else if (ChoiceAIsSafe)
			{
				gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_RIGHT;
			}
			else if (ChoiceBIsSafe)
			{
				gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_LEFT;
			}
			else
			{
				// do nothing
			}
		}
	}
	else if (gPlayers[PLAYER_TWO].CurrentDirection == DIRECTION_DOWN)
	{
		// Look down
		memcpy(
			&DestinationPixel,
			(PIXEL32*)gBackBuffer.Memory + gPlayers[PLAYER_TWO].CurrentPosition.x + (RENDER_WIDTH * (gPlayers[PLAYER_TWO].CurrentPosition.y - 1)),
			sizeof(PIXEL32));

		if ((DestinationPixel.Bytes != 0 && DestinationPixel.Bytes != (DARK_GRAY_PIXEL).Bytes) ||
			(Random % 227 == 0))
		{
			// We have to turn right or left. We will die if we keep going down, and we cannot turn back on ourselves.

			// look right
			memcpy(
				&ChoiceA,
				(PIXEL32*)gBackBuffer.Memory + (gPlayers[PLAYER_TWO].CurrentPosition.x + 1) + (RENDER_WIDTH * gPlayers[PLAYER_TWO].CurrentPosition.y),
				sizeof(PIXEL32));

			// look left
			memcpy(
				&ChoiceB,
				(PIXEL32*)gBackBuffer.Memory + (gPlayers[PLAYER_TWO].CurrentPosition.x - 1) + (RENDER_WIDTH * gPlayers[PLAYER_TWO].CurrentPosition.y),
				sizeof(PIXEL32));

			ChoiceAIsSafe = (ChoiceA.Bytes == 0 || ChoiceA.Bytes == (DARK_GRAY_PIXEL).Bytes);

			ChoiceBIsSafe = (ChoiceB.Bytes == 0 || ChoiceB.Bytes == (DARK_GRAY_PIXEL).Bytes);

			if (ChoiceAIsSafe & ChoiceBIsSafe)
			{
				// Both choices are safe, coin flip between them

				rand_s(&Random);

				if (Random % 2 == 0)
				{
					gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_RIGHT;
				}
				else
				{
					gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_LEFT;
				}
			}
			else if (ChoiceAIsSafe)
			{
				gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_RIGHT;
			}
			else if (ChoiceBIsSafe)
			{
				gPlayers[PLAYER_TWO].CurrentDirection = DIRECTION_LEFT;
			}
			else
			{
				// do nothing
			}
		}
	}	

	if (PreviousDirection != gPlayers[PLAYER_TWO].CurrentDirection)
	{
		// Add the player's current position to the array of turn points
		for (int x = 0; x < _countof(gPlayers[PLAYER_TWO].TurnPoints); x++)
		{
			if (gPlayers[PLAYER_TWO].TurnPoints[x].x == 0)
			{
				gPlayers[PLAYER_TWO].TurnPoints[x].x = gPlayers[PLAYER_TWO].CurrentPosition.x;

				gPlayers[PLAYER_TWO].TurnPoints[x].y = gPlayers[PLAYER_TWO].CurrentPosition.y;

				break;
			}
		}
	}
}

// If we are hosting, we are sending broadcasts to the subnet to advertise the fact that we are hosting.
// A client will receive broadcasts.
// A host will receive UDP unicast datagrams from the client.
// A client will send his movements to the host via udp unicast datagrams.
// Use the same socket to both send and receive unicast UDP packets.
int InitializeNetwork(void)
{
	ASSERT(gNetworkData.NetworkInitialized == FALSE, "Network is already initialized!");
		
	BOOL Broadcast = TRUE;

	if (WSAStartup(MAKEWORD(2, 2), &gNetworkData.WSAData) != 0)
	{
		WSASetLastError(WSASYSNOTREADY);

		ShowFormattedErrorDialog(WSAGetLastError(), L"WSAStartup failed!");

		goto Exit;
	}

	gNetworkData.BroadcastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (gNetworkData.BroadcastSocket == INVALID_SOCKET)
	{
		ShowFormattedErrorDialog(WSAGetLastError(), L"Failed to create broadcast socket!");

		goto Exit;
	}

	if (setsockopt(gNetworkData.BroadcastSocket, SOL_SOCKET, SO_BROADCAST, (char*)&Broadcast, sizeof(Broadcast)) != 0)
	{
		ShowFormattedErrorDialog(WSAGetLastError(), L"Failed to set send socket option!");

		goto Exit;
	}

	gNetworkData.BroadcastAddress.sin_family = AF_INET;

	gNetworkData.BroadcastAddress.sin_port = htons(NETWORK_PORT_BROADCAST);

	if (gNetworkData.IsHost == TRUE)
	{
		// If we are host, we are sending broadcast messages therefore we do not need to bind in order to send

		gNetworkData.BroadcastAddress.sin_addr.s_addr = INADDR_BROADCAST;

		snprintf(
			gNetworkData.BroadcastBuffer,
			_countof(gNetworkData.BroadcastBuffer),
			"LR-HOST:%02d:%s",
			gPlayers[PLAYER_ONE].ColorIndex, gPlayers[PLAYER_ONE].Name);
	}
	else
	{
		// If we are client we need to bind in order to receive broadcast messages

		gNetworkData.BroadcastAddress.sin_addr.s_addr = INADDR_ANY;

		if (bind(gNetworkData.BroadcastSocket, (struct sockaddr*)&gNetworkData.BroadcastAddress, sizeof(struct sockaddr_in)) != 0)
		{
			ShowFormattedErrorDialog(WSAGetLastError(), L"Failed to bind to broadcast socket!");

			goto Exit;
		}
	}

	// End broadcast socket stuff.
	// Begin unicast socket stuff.

	gNetworkData.UnicastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (gNetworkData.UnicastSocket == INVALID_SOCKET)
	{
		ShowFormattedErrorDialog(WSAGetLastError(), L"Failed to create unicast socket!");

		goto Exit;
	}

	gNetworkData.LocalAddress.sin_family = AF_INET;

	gNetworkData.LocalAddress.sin_port = htons(NETWORK_PORT_GAME);

	if (gNetworkData.IsHost == TRUE)
	{
		// The host needs to bind in order to receive datagrams from the client.

		gNetworkData.LocalAddress.sin_addr.s_addr = INADDR_ANY;

		if (bind(gNetworkData.UnicastSocket, (struct sockaddr*)&gNetworkData.LocalAddress, sizeof(struct sockaddr_in)) != 0)
		{
			ShowFormattedErrorDialog(WSAGetLastError(), L"Failed to bind unicast socket!");

			goto Exit;
		}
	}
	else
	{
		// Client does not yet know what address he needs to send to!
	}

	gNetworkData.NetworkInitialized = TRUE;

Exit:

	return(WSAGetLastError());
}

void FindConnectedGamepads(void)
{
	XINPUT_STATE State = { 0 };

	gGamepadsPresent = -1;

	if (XInputGetState(PLAYER_ONE, &State) == ERROR_SUCCESS)
	{
		gGamepadsPresent++;
	}

	if (XInputGetState(PLAYER_TWO, &State) == ERROR_SUCCESS)
	{
		gGamepadsPresent++;
	}
}

void CalculateWindowPlacement(void)
{
	DWORD WindowStyle = 0;

	// param 1 could be 0, but I don't think this could reasonably fail.
#pragma warning(suppress:6387)	
	GetMonitorInfoW(MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY), &gPrimaryMonitor);

	if (gFullscreen)
	{
		WindowStyle = WS_VISIBLE;

		for (uint8_t Counter = 1; Counter < 12; Counter++)
		{
			if (RENDER_WIDTH * Counter > (gPrimaryMonitor.rcMonitor.right - gPrimaryMonitor.rcMonitor.left) ||
				RENDER_HEIGHT * Counter > (gPrimaryMonitor.rcMonitor.bottom - gPrimaryMonitor.rcMonitor.top))
			{
				gDrawScale = Counter - 1;

				break;
			}
		}

		SetWindowLongPtrW(gMainWindow, GWL_STYLE, WindowStyle);

		SetWindowPos(
			gMainWindow,
			HWND_TOP,
			0,
			0,
			(gPrimaryMonitor.rcMonitor.right - gPrimaryMonitor.rcMonitor.left),
			(gPrimaryMonitor.rcMonitor.bottom - gPrimaryMonitor.rcMonitor.top),
			SWP_FRAMECHANGED);

		InvalidateRect(gMainWindow, NULL, TRUE);
	}
	else
	{
		// What is the largest we can make the window that maintains the 1.6 aspect ratio
		// and still fits on the user's work area? ("work area" means don't cover the task bar)

		// We remove thickframe and maximizebox from the window style because we want the window
		// to always have an aspect ratio of 1.6. If we allowed the user to stretch the window
		// to any size, the graphics would not look right. I'll size the window as appropriate to 
		// fit the user's monitor later on.

		WindowStyle = (WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX)) | WS_VISIBLE;

		SetWindowLongPtrW(gMainWindow, GWL_STYLE, WindowStyle);

		for (int Scale = 20; Scale > 0; Scale--)
		{
			int ScaledWidth = RENDER_WIDTH * Scale;

			int ScaledHeight = RENDER_HEIGHT * Scale;

			if ((gPrimaryMonitor.rcWork.right - gPrimaryMonitor.rcWork.left) > ScaledWidth &&
				(gPrimaryMonitor.rcWork.bottom - gPrimaryMonitor.rcWork.top) > ScaledHeight)
			{
				RECT WindowSize = { 0 };

				SetRect(&WindowSize, 0, 0, ScaledWidth, ScaledHeight);

				AdjustWindowRectEx(&WindowSize, WindowStyle, FALSE, 0);

				SetWindowPos(
					gMainWindow,
					HWND_TOP,
					((gPrimaryMonitor.rcWork.right - gPrimaryMonitor.rcWork.left) / 2) - ((WindowSize.right - WindowSize.left) / 2),
					0,
					(WindowSize.right - WindowSize.left),
					(WindowSize.bottom - WindowSize.top),
					SWP_FRAMECHANGED);

				gDrawScale = (uint8_t)Scale;				

				break;
			}
		}
	}
}