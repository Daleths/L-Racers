#pragma once

// 'x' bytes padding added after data member 'foo'
#pragma warning(disable: 4820)
// Compiler will insert Spectre mitigation for memory load
#pragma warning(disable: 5045)
// function 'foo' selected for automatic inline expansion
#pragma warning(disable: 4711)
// function 'foo' not inlined
#pragma warning(disable: 4710)

#define _CRT_RAND_S

#include <Windows.h>

#include <stdio.h>

#include <math.h>

#include <stdint.h>

#include <intrin.h>

// Xbox gamepad input
#include <Xinput.h>	

#pragma comment(lib, "XInput.lib")

// Audio library.
#include <xaudio2.h>

#pragma comment(lib, "XAudio2.lib")

// stb_vorbis by Sean T. Barrett - http://nothings.org/
#include "stb_vorbis.h"



#define GAME_NAME_W L"L-RACERS"

#define GAME_NAME_A "L-RACERS"

#define WINDOW_CLASS_NAME GAME_NAME_W L"_WndClass"

#define RENDER_WIDTH	384

#define RENDER_HEIGHT	240

#define RENDER_BPP		32

#define RENDER_AREA_MEM_SIZE	RENDER_WIDTH * RENDER_HEIGHT * (RENDER_BPP / 8)

#define ASSET_FILE_NAME_A "Assets.dat"

// 16.67 milliseconds is 60 frames per second.
#define TARGET_MICROSECONDS_PER_FRAME	16667ULL

// Every 120 frames/2 seconds we will calculate some statistics such as average FPS, CPU usage, etc.
#define CALCULATE_STATS_EVERY_X_FRAMES	120

#define ASSERT(Expression, Message) if (!(Expression)) { __ud2(); }

// Every font sheet we use in this game must have the same format,
// which currently is just all 98 ASCII characters in a single row.
#define FONT_SHEET_CHARACTERS_PER_ROW 98

#define WHITE_PIXEL			(PIXEL32) { .Bytes = 0xFFFFFFFF }

#define BLACK_PIXEL			(PIXEL32) { .Bytes = 0xFF000000 }

#define GRAY_PIXEL			(PIXEL32) { .Bytes = 0xFF444444 }

#define DARK_GRAY_PIXEL		(PIXEL32) { .Bytes = 0xFF202020 }

#define FULL_GREEN_PIXEL	(PIXEL32) { .Bytes = 0xFF00FF00 }

#define FULL_BLUE_PIXEL		(PIXEL32) { .Bytes = 0xFF0000FF }

#define FULL_RED_PIXEL		(PIXEL32) { .Bytes = 0xFFFF0000 }

#define PINK_PIXEL			(PIXEL32) { .Bytes = 0xFFF878F8 }

#define DARK_GREEN_PIXEL	(PIXEL32) { .Bytes = 0xFF004400 }

#define ORANGE_PIXEL		(PIXEL32) { .Bytes = 0xFFFF6A00 }

#define YELLOW_PIXEL		(PIXEL32) { .Bytes = 0xFFFFD800 }

#define LIGHT_BLUE_PIXEL	(PIXEL32) { .Bytes = 0xFF0094FF }

#define PURPLE_PIXEL		(PIXEL32) { .Bytes = 0xFF4800FF }

#define DUMMY_PIXEL			(PIXEL32) { .Bytes = 0xDEADBEEF }
 
#define PLAYER_ONE 0

#define PLAYER_TWO 1

#define MAX_PLAYER_NAME_LEN	12

#define NUMBER_OF_SFX_SOURCE_VOICES 4

#define MOUSE_TIMER_ID	4001

#define NETWORK_PORT_BROADCAST	9657

#define NETWORK_PORT_GAME		9658

typedef enum GAMESTATE
{
	GAMESTATE_TITLESCREEN,

	GAMESTATE_OPTIONSSCREEN,

	GAMESTATE_ACTIONSCREEN,

	GAMESTATE_POSTACTION,

	GAMESTATE_ENTERYOURNAME,

	GAMESTATE_HOSTLANGAME,

	GAMESTATE_JOINLANGAME,

	GAMESTATE_ENTERIP,

	GAMESTATE_GAMEPADUNPLUGGED

} GAMESTATE;

// For use in the DrawRectangle function.
typedef enum DR_FLAGS
{
	DR_BORDERED = 1,				// 1 << 0

	DR_OPAQUE = 2,					// 1 << 1, 0b00000010

	DR_HORIZONTALLY_CENTERED = 4,	// 1 << 2, 0b00000100

	DR_VERTICALLY_CENTERED = 8,		// 1 << 3, 0b00001000

	DR_SHADOW = 16,					// 1 << 4, 0b00010000	

	DR_SHAKE = 32,					// 1 << 5, 0b00100000

	DR_ROUNDED_CORNERS = 64,		// 1 << 6, 0b01000000

	DR_THICK = 128					// 1 << 7, 0b10000000

} DR_FLAGS;

typedef enum BS_FLAGS
{
	BS_SHADOW = 1

} BS_FLAGS;

typedef enum DIRECTION
{
	DIRECTION_LEFT,

	DIRECTION_RIGHT,

	DIRECTION_UP,

	DIRECTION_DOWN

} DIRECTION;

// GAMEBITMAP is any sort of bitmap, which might be a sprite, or a background, 
// or a font sheet, or even the back buffer itself.
typedef struct GAMEBITMAP
{
	BITMAPINFO BitmapInfo;

	void* Memory;

} GAMEBITMAP;

// Both sound effects and background music are GAMESOUNDs,
// we play them with the XAudio2 library.
typedef struct GAMESOUND
{
	WAVEFORMATEX WaveFormat;

	XAUDIO2_BUFFER Buffer;

} GAMESOUND;

// A 32-bit, 4-byte pixel. Each color is 0-255.
// This is a union so a PIXEL32 can either be referred to
// as an unsigned 32-bit whole, or as a struct with 4 separate 8-bit components.
typedef union PIXEL32 {

	struct Colors {

		uint8_t Blue;

		uint8_t Green;

		uint8_t Red;

		uint8_t Alpha;

	} Colors;

	DWORD Bytes;

} PIXEL32;

typedef struct GAMEINPUT
{
	SHORT LeftKeyIsDown;

	SHORT LeftKeyWasDown;

	SHORT RightKeyIsDown;

	SHORT RightKeyWasDown;

	SHORT UpKeyIsDown;

	SHORT UpKeyWasDown;

	SHORT DownKeyIsDown;

	SHORT DownKeyWasDown;

	SHORT ChooseKeyIsDown;

	SHORT ChooseKeyWasDown;

	XINPUT_STATE GamepadState;

	uint64_t VibrateTime;

} GAMEINPUT;

typedef struct PLAYER
{
	BOOL AI;

	char Name[MAX_PLAYER_NAME_LEN + 1];

	BOOL IsDead;

	POINT CurrentPosition;

	DIRECTION CurrentDirection;

	uint8_t ColorIndex;	

	POINT TurnPoints[1024];

	GAMEINPUT Input;	

} PLAYER;

// Every MENU is a collection of MENUITEMs.
typedef struct MENUITEM
{
	char Name[32];

	int16_t x;

	int16_t y;

	BOOL Enabled;

	void(*Action)(void);

} MENUITEM;

typedef struct MENU
{
	char* Name;

	uint8_t SelectedItem;

	const uint8_t ItemCount;

	MENUITEM** Items;

} MENU;

typedef struct NETWORKDATA
{
	BOOL NetworkInitialized;

	WSADATA WSAData;

	SOCKET BroadcastSocket;

	SOCKET UnicastSocket;

	struct sockaddr_in BroadcastAddress;

	struct sockaddr_in RemoteAddress;

	struct sockaddr_in LocalAddress;

	struct sockaddr_in TempAddress;

	char BroadcastBuffer[64];

	char SendBuffer[64];

	char RecvBuffer[64];

	BOOL IsHost;

} NETWORKDATA;

typedef struct GAMEHOST
{
	char Name[MAX_PLAYER_NAME_LEN + 1];

	uint8_t ColorIndex;

	struct sockaddr_in Address;

	uint64_t LastSeen;

} GAMEHOST;

/* BEGIN GLOBAL VARIABLE DECLARATIONS */

extern BOOL gShouldExit;

extern BOOL gFullscreen;

extern HWND gMainWindow;

extern uint8_t gDrawScale;

extern MONITORINFO gPrimaryMonitor;

extern GAMEBITMAP gBackBuffer;

extern GAMEBITMAP g6x7Font;

extern GAMEBITMAP gTopAccentPicture;

extern GAMEBITMAP gXboxGamepadPicture;

extern PLAYER gPlayers[2];

extern uint64_t gTotalFramesRendered;

extern GAMESTATE gGameState;

extern GAMESTATE gPreviousGameState;

extern BOOL gInputDisabled;

extern BOOL gWindowHasFocus;

extern PIXEL32* gAvailableColors[];

extern int8_t gSFXVolume;

extern int8_t gMusicVolume;

extern IXAudio2SourceVoice* gXAudioSFXSourceVoice[NUMBER_OF_SFX_SOURCE_VOICES];

IXAudio2SourceVoice* gXAudioMusicSourceVoice;

extern GAMESOUND gMenuSound;

extern GAMESOUND gMenuChooseSound;

extern GAMESOUND gExplosionSound;

extern NETWORKDATA gNetworkData;

extern GAMEHOST gGameHosts[5];

extern char gManualIP[16];

extern int8_t gGamepadsPresent;

extern int8_t gGamepadsExpected;

/* END GLOBAL VARIABLE DECLARATIONS */


LRESULT CALLBACK WindowProc(_In_ HWND WindowHandle, _In_ UINT Message, _In_ WPARAM WParam, _In_ LPARAM LParam);

void ShowFormattedErrorDialog(_In_ DWORD LastErrorCode, _In_ wchar_t* ActivityDescription);

void CollectPlayerInput(void);

void RenderFrameGraphics(void);

void DrawRectangle(
	_In_opt_ uint16_t x,
	_In_opt_ uint16_t y,
	_In_ int16_t Width,
	_In_ int16_t Height,
	_In_opt_ PIXEL32* BorderColor,
	_In_opt_ PIXEL32* BackgroundColor,
	_In_opt_ PIXEL32* ShadowColor,
	_In_ DWORD Flags);

void DrawLine(_In_ int x1, _In_ int y1, _In_ int x2, _In_ int y2, _In_ PIXEL32 Color);

void Blit32BppBitmap(_In_ GAMEBITMAP* GameBitmap, _In_ int16_t x, _In_ int16_t y);

void BlitString(_In_ char* String, _In_ GAMEBITMAP* FontSheet, _In_ PIXEL32* Color, _In_ uint16_t x, _In_ uint16_t y, _In_opt_ PIXEL32* ShadowColor, _In_ DWORD Flags);

HRESULT InitializeSoundEngine(void);

BOOL LoadAssets(void);

DWORD LoadAssetFromArchive(_In_ char* ArchiveName, _In_ char* AssetFileName, _Inout_ void* Resource);

DWORD Load32BppBitmapFromMemory(_In_ void* Buffer, _Inout_ GAMEBITMAP* GameBitmap);

DWORD LoadWavFromMemory(_In_ void* Buffer, _Inout_ GAMESOUND* GameSound);

DWORD LoadOggFromMemory(_In_ void* Buffer, _In_ uint32_t BufferSize, _Inout_ GAMESOUND* GameSound);

void PlayGameSound(_In_ GAMESOUND* GameSound);

void PlayGameMusic(_In_ GAMESOUND* Music);

void ResetEverythingForNewGame(void);

void CalculateAIMovement(void);

int InitializeNetwork(void);

void FindConnectedGamepads(void);

void CalculateWindowPlacement(void);