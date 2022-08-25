#include "Main.h"

#include "SplashScreen.h"

void Draw_SplashScreen(void)
{
	memset(gBackBuffer.Memory, 0, RENDER_AREA_MEM_SIZE);

	static uint64_t LocalFrameCounter;

	static uint64_t LastFrameSeen;	

	if (gTotalFramesRendered > (LastFrameSeen + 1))
	{
		LocalFrameCounter = 0;
	}

    if (LocalFrameCounter >= 120 && LocalFrameCounter < 600)
    {
        // Play the opening splash screen sound exactly once.
        if (LocalFrameCounter == 120)
        {
            PlayGameSound(&gSplashScreenSound);
        }

        // Show the splash screen logo.
        Blit32BppBitmap(
            &gPolePig,
            (int16_t)((RENDER_WIDTH / 2) - (gPolePig.BitmapInfo.bmiHeader.biWidth / 2)),
            50);

        // Show the lightning bolts as part of the splash screen logo,
        // attempt to synchronize it with the lightning sound.
        if ((LocalFrameCounter >= 120 && LocalFrameCounter < 130) ||
            (LocalFrameCounter >= 180 && LocalFrameCounter < 190) ||
            (LocalFrameCounter >= 260 && LocalFrameCounter < 270))
        {
            Blit32BppBitmap(
                &gLightning01,
                150,
                55);
        }        

        BlitString(
            "POLE PIG PRODUCTIONS",
            &g6x7Font,
            &WHITE_PIXEL,
            (RENDER_WIDTH / 2) - ((20 * 6) / 2), 
            150,
            &DARK_GRAY_PIXEL,
            BS_SHADOW);
    }

    if (LocalFrameCounter == 630)
    {
        PlayGameMusic(&gMusic);

        gGameState = GAMESTATE_TITLESCREEN;
    }

	LocalFrameCounter++;

	LastFrameSeen = gTotalFramesRendered;	
}

void PPI_SplashScreen(void)
{

}