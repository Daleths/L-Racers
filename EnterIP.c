#include "Main.h"

#include "EnterIP.h"

// The Enter Your Name screen looks like this:
// A B C D E F G H I J K L M 
// N O P Q R S T U V W X Y Z
// a b c d e f g h i j k l m
// n o p q r s t u v w x y z
// Back                   OK
// (12 * 8) + (13 * 6) 12 spaces at 8 pixels each, plus 13 letters at 6 pixels wide each
MENUITEM gMI_EnterIP_1 = { "1", 130, 100, TRUE, MenuFunc_EnterIP_Add };

MENUITEM gMI_EnterIP_2 = { "2", 142, 100, TRUE, MenuFunc_EnterIP_Add };

MENUITEM gMI_EnterIP_3 = { "3", 154, 100, TRUE, MenuFunc_EnterIP_Add };

MENUITEM gMI_EnterIP_4 = { "4", 166, 100, TRUE, MenuFunc_EnterIP_Add };

MENUITEM gMI_EnterIP_5 = { "5", 178, 100, TRUE, MenuFunc_EnterIP_Add };

MENUITEM gMI_EnterIP_6 = { "6", 190, 100, TRUE, MenuFunc_EnterIP_Add };

MENUITEM gMI_EnterIP_7 = { "7", 202, 100, TRUE, MenuFunc_EnterIP_Add };

MENUITEM gMI_EnterIP_8 = { "8", 214, 100, TRUE, MenuFunc_EnterIP_Add };

MENUITEM gMI_EnterIP_9 = { "9", 226, 100, TRUE, MenuFunc_EnterIP_Add };

MENUITEM gMI_EnterIP_0 = { "0", 238, 100, TRUE, MenuFunc_EnterIP_Add };

MENUITEM gMI_EnterIP_Period = { ".", 250, 100, TRUE, MenuFunc_EnterIP_Add };

MENUITEM gMI_EnterIP_Back = { "Back", 130, 115, TRUE, MenuFunc_EnterIP_Back };

MENUITEM gMI_EnterIP_OK = { "OK", 245, 115, FALSE, MenuFunc_EnterIP_OK };

MENUITEM* gMI_EnterIPItems[] = {
    &gMI_EnterIP_1, &gMI_EnterIP_2, &gMI_EnterIP_3, &gMI_EnterIP_4, &gMI_EnterIP_5,
    &gMI_EnterIP_6, &gMI_EnterIP_7, &gMI_EnterIP_8, &gMI_EnterIP_9, &gMI_EnterIP_0, &gMI_EnterIP_Period,
    &gMI_EnterIP_Back, &gMI_EnterIP_OK };

MENU gMENU_EnterIP = { "Enter IP Address", 0, _countof(gMI_EnterIPItems), gMI_EnterIPItems };



void Draw_EnterIP(void)
{
    memset(gBackBuffer.Memory, 0, RENDER_AREA_MEM_SIZE);

    static uint64_t LocalFrameCounter;

    static uint64_t LastFrameSeen = 0;

    if (gTotalFramesRendered > (LastFrameSeen + 1))
    {
        LocalFrameCounter = 0;

        gMENU_EnterIP.SelectedItem = 0;

        memset(gManualIP, 0, sizeof(gManualIP));

        gMI_EnterIP_OK.Enabled = FALSE;
    }

    Blit32BppBitmap(&gTopAccentPicture, 0, 0);

    DrawRectangle(
        0,
        16,
        128,
        16,
        &FULL_GREEN_PIXEL,
        NULL,
        &DARK_GREEN_PIXEL,
        DR_BORDERED | DR_SHADOW | DR_ROUNDED_CORNERS | DR_THICK | DR_HORIZONTALLY_CENTERED);

    DrawRectangle(
        0,
        96,
        160,
        32,
        &FULL_GREEN_PIXEL,
        NULL,
        &DARK_GREEN_PIXEL,
        DR_BORDERED | DR_SHADOW | DR_ROUNDED_CORNERS | DR_THICK | DR_HORIZONTALLY_CENTERED);

    BlitString(
        gMENU_EnterIP.Name,
        &g6x7Font,
        &FULL_GREEN_PIXEL,
        (RENDER_WIDTH / 2) - (((uint16_t)strlen(gMENU_EnterIP.Name) * 6) / 2),
        20,
        &DARK_GREEN_PIXEL,
        BS_SHADOW);

    for (uint8_t Letter = 0; Letter < _countof(gManualIP) - 1; Letter++)
    {
        if (gManualIP[Letter] == '\0')
        {
            BlitString(
                "_",
                &g6x7Font,
                &FULL_GREEN_PIXEL,
                144 + (Letter * 6),
                80,
                &DARK_GREEN_PIXEL,
                BS_SHADOW);
        }
        else
        {
            BlitString(
                &gManualIP[Letter],
                &g6x7Font,
                &FULL_GREEN_PIXEL,
                144 + (Letter * 6),
                80,
                &DARK_GREEN_PIXEL,
                BS_SHADOW);
        }
    }

    for (uint8_t Counter = 0; Counter < gMENU_EnterIP.ItemCount; Counter++)
    {
        if (gMENU_EnterIP.Items[Counter]->Enabled)
        {
            BlitString(
                gMENU_EnterIP.Items[Counter]->Name,
                &g6x7Font,
                &FULL_GREEN_PIXEL,
                gMENU_EnterIP.Items[Counter]->x,
                gMENU_EnterIP.Items[Counter]->y,
                &DARK_GREEN_PIXEL,
                BS_SHADOW);
        }
        else
        {
            BlitString(
                gMENU_EnterIP.Items[Counter]->Name,
                &g6x7Font,
                &DARK_GREEN_PIXEL,
                gMENU_EnterIP.Items[Counter]->x,
                gMENU_EnterIP.Items[Counter]->y,
                NULL,
                0);
        }
    }

    BlitString(
        "\xBB",
        &g6x7Font,
        &FULL_GREEN_PIXEL,
        gMENU_EnterIP.Items[gMENU_EnterIP.SelectedItem]->x - 6,
        gMENU_EnterIP.Items[gMENU_EnterIP.SelectedItem]->y,
        &DARK_GREEN_PIXEL,
        BS_SHADOW);


    LocalFrameCounter++;

    LastFrameSeen = gTotalFramesRendered;
}

void PPI_EnterIP(void)
{
#define ROW_WIDTH   11

#define BACK_BUTTON 11

#define OK_BUTTON   12

#define SELECTED_TEXT(Text) strcmp(Text, gMENU_EnterIP.Items[gMENU_EnterIP.SelectedItem]->Name) == 0 ? TRUE : FALSE

    if (gPlayers[PLAYER_ONE].Input.LeftKeyIsDown && !gPlayers[PLAYER_ONE].Input.LeftKeyWasDown)
    {
        PlayGameSound(&gMenuSound);

        if (SELECTED_TEXT("Back"))
        {
            gMENU_EnterIP.SelectedItem = OK_BUTTON;
        }
        else if (SELECTED_TEXT("OK"))
        {
            gMENU_EnterIP.SelectedItem = BACK_BUTTON;
        }
        else
        {
            if ((char)gMENU_EnterIP.Items[gMENU_EnterIP.SelectedItem]->Name[0] == '1')
            {
                gMENU_EnterIP.SelectedItem += ROW_WIDTH - 1;
            }
            else
            {           
                gMENU_EnterIP.SelectedItem--;            
            }
        }
    }

    if (gPlayers[PLAYER_ONE].Input.RightKeyIsDown && !gPlayers[PLAYER_ONE].Input.RightKeyWasDown)
    {
        PlayGameSound(&gMenuSound);

        if (SELECTED_TEXT("Back"))
        {
            gMENU_EnterIP.SelectedItem = OK_BUTTON;
        }
        else if (SELECTED_TEXT("OK"))
        {
            gMENU_EnterIP.SelectedItem = BACK_BUTTON;
        }
        else
        {
            if ((char)gMENU_EnterIP.Items[gMENU_EnterIP.SelectedItem]->Name[0] == '.')
            {
                gMENU_EnterIP.SelectedItem -= ROW_WIDTH - 1;
            }
            else
            {
                gMENU_EnterIP.SelectedItem++;
            }
        }
    }

    if (gPlayers[PLAYER_ONE].Input.DownKeyIsDown && !gPlayers[PLAYER_ONE].Input.DownKeyWasDown)
    {
        if (!SELECTED_TEXT("OK") && !SELECTED_TEXT("Back"))
        {
            PlayGameSound(&gMenuSound);

            gMENU_EnterIP.SelectedItem = BACK_BUTTON;
        }
    }

    if (gPlayers[PLAYER_ONE].Input.UpKeyIsDown && !gPlayers[PLAYER_ONE].Input.UpKeyWasDown)
    {
        if (SELECTED_TEXT("OK") || SELECTED_TEXT("Back"))
        {
            PlayGameSound(&gMenuSound);

            gMENU_EnterIP.SelectedItem = 0;
        }
    }

    if (gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown && !gPlayers[PLAYER_ONE].Input.ChooseKeyWasDown)
    {
        if (gMENU_EnterIP.Items[gMENU_EnterIP.SelectedItem]->Enabled)
        {
            PlayGameSound(&gMenuChooseSound);

            gMENU_EnterIP.Items[gMENU_EnterIP.SelectedItem]->Action();
        }
    }
}

void MenuFunc_EnterIP_Add(void)
{
    if (strlen(gManualIP) < 15)
    {
        PlayGameSound(&gMenuChooseSound);

        gManualIP[strlen(gManualIP)] = gMENU_EnterIP.Items[gMENU_EnterIP.SelectedItem]->Name[0];
    }

    if (IsValidIPv4Address(gManualIP))
    {
        gMI_EnterIP_OK.Enabled = TRUE;
    }
    else
    {
        gMI_EnterIP_OK.Enabled = FALSE;
    }
}

void MenuFunc_EnterIP_Back(void)
{
    PlayGameSound(&gMenuChooseSound);

    if (strlen(gManualIP) < 1)
    {
        memset(gManualIP, 0, sizeof(gManualIP));

        gGameState = GAMESTATE_JOINLANGAME;
    }
    else
    {
        gManualIP[strlen(gManualIP) - 1] = '\0';
    }

    if (IsValidIPv4Address(gManualIP))
    {
        gMI_EnterIP_OK.Enabled = TRUE;
    }
    else
    {
        gMI_EnterIP_OK.Enabled = FALSE;
    }
}

// Go back to the previous gamestate. There is a special case
// in the "Join LAN Game" gamestate that will execute when gManualIP is populated.
void MenuFunc_EnterIP_OK(void)
{
    gGameState = GAMESTATE_JOINLANGAME;
}

BOOL IsValidIPv4Address(_In_ char* IP)
{
    int NumberOfPeriods = 0;

    for (int c = 0; c < (int)strlen(IP); c++)
    {
        if (IP[c] == '.')
        {
            NumberOfPeriods++;
        }
    }

    if (strlen(IP) >= 7 && NumberOfPeriods == 3) // 1.1.1.1
    {
        char octet[4] = { 0 };

        int StartOfNextOctet = 0;

        // Validate the first octet

        for (int c = 0; c < 4; c++)
        {
            if (IP[c] == '.')
            {
                StartOfNextOctet = (c + 1);

                break;
            }

            octet[c] = IP[c];
        }

        if (strlen(octet) < 1 || strlen(octet) > 3 ||
            atoi(octet) <= 0 || atoi(octet) > 255)
        {
            return(FALSE);
        }

        // Validate the 2nd octet

        memset(octet, 0, sizeof(octet));

        for (int c = 0; c < 4; c++)
        {
            if (IP[c + StartOfNextOctet] == '.')
            {
                StartOfNextOctet += (c + 1);

                break;
            }

            octet[c] = IP[c + StartOfNextOctet];
        }

        if (strlen(octet) < 1 || strlen(octet) > 3 ||
            atoi(octet) < 0 || atoi(octet) > 255)
        {
            return(FALSE);
        }

        // Validate the 3rd octet

        memset(octet, 0, sizeof(octet));

        for (int c = 0; c < 4; c++)
        {
            if (IP[c + StartOfNextOctet] == '.')
            {
                StartOfNextOctet += (c + 1);

                break;
            }

            octet[c] = IP[c + StartOfNextOctet];
        }

        if (strlen(octet) < 1 || strlen(octet) > 3 ||
            atoi(octet) < 0 || atoi(octet) > 255)
        {
            return(FALSE);
        }

        // Validate the 4th octet

        memset(octet, 0, sizeof(octet));

        for (int c = 0; c < 4; c++)
        {
            if (IP[c + StartOfNextOctet] == '.')
            {
                StartOfNextOctet += (c + 1);

                break;
            }

            octet[c] = IP[c + StartOfNextOctet];
        }

        if (strlen(octet) < 1 || strlen(octet) > 3 ||
            atoi(octet) <= 0 || atoi(octet) > 255)
        {
            return(FALSE);
        }

        return(TRUE);
    }
    else
    {
        return(FALSE);
    }    
}