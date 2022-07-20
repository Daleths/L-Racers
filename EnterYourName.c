#include "Main.h"

#include "EnterYourName.h"

// The Enter Your Name screen looks like this:
// A B C D E F G H I J K L M 
// N O P Q R S T U V W X Y Z
// a b c d e f g h i j k l m
// n o p q r s t u v w x y z
// Back                   OK
								 // (12 * 8) + (13 * 6) 12 spaces at 8 pixels each, plus 13 letters at 6 pixels wide each
MENUITEM gMI_EnterYourName_A = { "A", 118, 110, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_B = { "B", 130, 110, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_C = { "C", 142, 110, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_D = { "D", 154, 110, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_E = { "E", 166, 110, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_F = { "F", 178, 110, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_G = { "G", 190, 110, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_H = { "H", 202, 110, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_I = { "I", 214, 110, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_J = { "J", 226, 110, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_K = { "K", 238, 110, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_L = { "L", 250, 110, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_M = { "M", 262, 110, TRUE, MenuFunc_EnterYourName_Add };



MENUITEM gMI_EnterYourName_N = { "N", 118, 120, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_O = { "O", 130, 120, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_P = { "P", 142, 120, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_Q = { "Q", 154, 120, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_R = { "R", 166, 120, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_S = { "S", 178, 120, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_T = { "T", 190, 120, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_U = { "U", 202, 120, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_V = { "V", 214, 120, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_W = { "W", 226, 120, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_X = { "X", 238, 120, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_Y = { "Y", 250, 120, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_Z = { "Z", 262, 120, TRUE, MenuFunc_EnterYourName_Add };


MENUITEM gMI_EnterYourName_a = { "a", 118, 130, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_b = { "b", 130, 130, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_c = { "c", 142, 130, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_d = { "d", 154, 130, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_e = { "e", 166, 130, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_f = { "f", 178, 130, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_g = { "g", 190, 130, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_h = { "h", 202, 130, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_i = { "i", 214, 130, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_j = { "j", 226, 130, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_k = { "k", 238, 130, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_l = { "l", 250, 130, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_m = { "m", 262, 130, TRUE, MenuFunc_EnterYourName_Add };


MENUITEM gMI_EnterYourName_n = { "n", 118, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_o = { "o", 130, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_p = { "p", 142, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_q = { "q", 154, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_r = { "r", 166, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_s = { "s", 178, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_t = { "t", 190, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_u = { "u", 202, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_v = { "v", 214, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_w = { "w", 226, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_x = { "x", 238, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_y = { "y", 250, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_z = { "z", 262, 140, TRUE, MenuFunc_EnterYourName_Add };

MENUITEM gMI_EnterYourName_Back = { "Back", 118, 150, TRUE, MenuFunc_EnterYourName_Back };

MENUITEM gMI_EnterYourName_OK = { "OK", 256, 150, FALSE, MenuFunc_EnterYourName_OK };


MENUITEM* gMI_EnterYourNameItems[] = {
	&gMI_EnterYourName_A, &gMI_EnterYourName_B, &gMI_EnterYourName_C, &gMI_EnterYourName_D, &gMI_EnterYourName_E, &gMI_EnterYourName_F,
	&gMI_EnterYourName_G, &gMI_EnterYourName_H, &gMI_EnterYourName_I, &gMI_EnterYourName_J, &gMI_EnterYourName_K, &gMI_EnterYourName_L,
	&gMI_EnterYourName_M, &gMI_EnterYourName_N, &gMI_EnterYourName_O, &gMI_EnterYourName_P, &gMI_EnterYourName_Q, &gMI_EnterYourName_R,
	&gMI_EnterYourName_S, &gMI_EnterYourName_T, &gMI_EnterYourName_U, &gMI_EnterYourName_V, &gMI_EnterYourName_W, &gMI_EnterYourName_X,
	&gMI_EnterYourName_Y, &gMI_EnterYourName_Z,
	&gMI_EnterYourName_a, &gMI_EnterYourName_b, &gMI_EnterYourName_c, &gMI_EnterYourName_d, &gMI_EnterYourName_e, &gMI_EnterYourName_f,
	&gMI_EnterYourName_g, &gMI_EnterYourName_h, &gMI_EnterYourName_i, &gMI_EnterYourName_j, &gMI_EnterYourName_k, &gMI_EnterYourName_l,
	&gMI_EnterYourName_m, &gMI_EnterYourName_n, &gMI_EnterYourName_o, &gMI_EnterYourName_p, &gMI_EnterYourName_q, &gMI_EnterYourName_r,
	&gMI_EnterYourName_s, &gMI_EnterYourName_t, &gMI_EnterYourName_u, &gMI_EnterYourName_v, &gMI_EnterYourName_w, &gMI_EnterYourName_x,
	&gMI_EnterYourName_y, &gMI_EnterYourName_z, &gMI_EnterYourName_Back, &gMI_EnterYourName_OK };

MENU gMENU_EnterYourName = { "Enter Your Name", 0, _countof(gMI_EnterYourNameItems), gMI_EnterYourNameItems };

void Draw_EnterYourName(void)
{
    memset(gBackBuffer.Memory, 0, RENDER_AREA_MEM_SIZE);
        
    static uint64_t LocalFrameCounter;

    static uint64_t LastFrameSeen = 0;
    
    if (gTotalFramesRendered > (LastFrameSeen + 1))
    {
        LocalFrameCounter = 0;

        gMENU_EnterYourName.SelectedItem = 0;

        LoadPlayerName();

        if (strlen(gPlayers[PLAYER_ONE].Name) > 0)
        {
            gMI_EnterYourName_OK.Enabled = TRUE;
        }
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
        108, 
        105, 
        166, 
        60, 
        &FULL_GREEN_PIXEL, 
        NULL, 
        &DARK_GREEN_PIXEL, 
        DR_BORDERED | DR_SHADOW | DR_ROUNDED_CORNERS | DR_THICK | DR_HORIZONTALLY_CENTERED);

    BlitString(
        gMENU_EnterYourName.Name, 
        &g6x7Font, 
        &FULL_GREEN_PIXEL, 
        (RENDER_WIDTH / 2) - (((uint16_t)strlen(gMENU_EnterYourName.Name) * 6) / 2), 
        20,
        &DARK_GREEN_PIXEL,
        BS_SHADOW);

    for (uint8_t Letter = 0; Letter < _countof(gPlayers[PLAYER_ONE].Name) - 1; Letter++)
    {
        if (gPlayers[PLAYER_ONE].Name[Letter] == '\0')
        {
            BlitString(
                "_", 
                &g6x7Font, 
                &FULL_GREEN_PIXEL, 
                156 + (Letter * 6), 
                90,
                &DARK_GREEN_PIXEL,
                BS_SHADOW);
        }
        else
        {
            BlitString(
                &gPlayers[PLAYER_ONE].Name[Letter], 
                &g6x7Font, 
                &FULL_GREEN_PIXEL, 
                156 + (Letter * 6), 
                90,
                &DARK_GREEN_PIXEL,
                BS_SHADOW);
        }
    }

    for (uint8_t Counter = 0; Counter < gMENU_EnterYourName.ItemCount; Counter++)
    {
        if (gMENU_EnterYourName.Items[Counter]->Enabled)
        {
            BlitString(
                gMENU_EnterYourName.Items[Counter]->Name,
                &g6x7Font,
                &FULL_GREEN_PIXEL,
                gMENU_EnterYourName.Items[Counter]->x,
                gMENU_EnterYourName.Items[Counter]->y,
                &DARK_GREEN_PIXEL,
                BS_SHADOW);
        }
        else
        {
            BlitString(
                gMENU_EnterYourName.Items[Counter]->Name,
                &g6x7Font,
                &DARK_GREEN_PIXEL,
                gMENU_EnterYourName.Items[Counter]->x,
                gMENU_EnterYourName.Items[Counter]->y,
                NULL,
                0);
        }
    }

    BlitString(
        "\xBB",
        &g6x7Font,
        &FULL_GREEN_PIXEL,
        gMENU_EnterYourName.Items[gMENU_EnterYourName.SelectedItem]->x - 6,
        gMENU_EnterYourName.Items[gMENU_EnterYourName.SelectedItem]->y,
        &DARK_GREEN_PIXEL,
        BS_SHADOW);

    LocalFrameCounter++;

    LastFrameSeen = gTotalFramesRendered;
}

void PPI_EnterYourName(void)
{
    // Character Naming Menu

    // A B C D E F G H I J K L M 
    // N O P Q R S T U V W X Y Z
    // a b c d e f g h i j k l m
    // n o p q r s t u v w x y z
    // Back                   OK

#define ROW_WIDTH   13

#define BACK_BUTTON 52

#define OK_BUTTON   53

#define SELECTED_TEXT(Text) strcmp(Text, gMENU_EnterYourName.Items[gMENU_EnterYourName.SelectedItem]->Name) == 0 ? TRUE : FALSE

    if (gPlayers[PLAYER_ONE].Input.UpKeyIsDown && !gPlayers[PLAYER_ONE].Input.UpKeyWasDown)
    {
        if (SELECTED_TEXT("Back"))
        {
            gMENU_EnterYourName.SelectedItem -= ROW_WIDTH;
        }
        else if (SELECTED_TEXT("OK"))
        {
            gMENU_EnterYourName.SelectedItem -= 2;
        }
        else
        {
            switch ((char)gMENU_EnterYourName.Items[gMENU_EnterYourName.SelectedItem]->Name[0])
            {
            case 'A':
            case 'B':
            case 'C':
            {
                gMENU_EnterYourName.SelectedItem = BACK_BUTTON;

                break;
            }
            case 'L':
            case 'M':
            case 'K':
            {
                gMENU_EnterYourName.SelectedItem = OK_BUTTON;

                break;
            }
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            {
                gMENU_EnterYourName.SelectedItem += (ROW_WIDTH * 3);

                break;
            }
            default:
            {
                gMENU_EnterYourName.SelectedItem -= ROW_WIDTH;

                break;
            }
            }
        }

        PlayGameSound(&gMenuSound);
    }

    if (gPlayers[PLAYER_ONE].Input.DownKeyIsDown && !gPlayers[PLAYER_ONE].Input.DownKeyWasDown)
    {
        if (SELECTED_TEXT("Back"))
        {
            gMENU_EnterYourName.SelectedItem = 0;
        }
        else if (SELECTED_TEXT("OK"))
        {
            gMENU_EnterYourName.SelectedItem = ROW_WIDTH - 1;
        }
        else
        {
            switch ((char)gMENU_EnterYourName.Items[gMENU_EnterYourName.SelectedItem]->Name[0])
            {
            case 'n':
            case 'o':
            case 'p':
            {
                gMENU_EnterYourName.SelectedItem = BACK_BUTTON;

                break;
            }
            case 'x':
            case 'y':
            case 'z':
            {
                gMENU_EnterYourName.SelectedItem = OK_BUTTON;

                break;
            }
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            {
                gMENU_EnterYourName.SelectedItem -= (ROW_WIDTH * 3);

                break;
            }
            default:
            {
                gMENU_EnterYourName.SelectedItem += ROW_WIDTH;

                break;
            }
            }
        }

        PlayGameSound(&gMenuSound);
    }

    if (gPlayers[PLAYER_ONE].Input.LeftKeyIsDown && !gPlayers[PLAYER_ONE].Input.LeftKeyWasDown)        
    {
        if (SELECTED_TEXT("Back"))
        {
            gMENU_EnterYourName.SelectedItem = OK_BUTTON;
        }
        else if (SELECTED_TEXT("OK"))
        {
            gMENU_EnterYourName.SelectedItem = BACK_BUTTON;
        }
        else
        {
            switch ((char)gMENU_EnterYourName.Items[gMENU_EnterYourName.SelectedItem]->Name[0])
            {
            case 'A':
            case 'N':
            case 'a':
            case 'n':
            {
                gMENU_EnterYourName.SelectedItem += ROW_WIDTH - 1;

                break;
            }
            default:
            {
                gMENU_EnterYourName.SelectedItem--;

                break;
            }
            }
        }

        PlayGameSound(&gMenuSound);
    }

    if (gPlayers[PLAYER_ONE].Input.RightKeyIsDown && !gPlayers[PLAYER_ONE].Input.RightKeyWasDown)        
    {
        if (SELECTED_TEXT("Back"))
        {
            gMENU_EnterYourName.SelectedItem = OK_BUTTON;
        }
        else if (SELECTED_TEXT("OK"))
        {
            gMENU_EnterYourName.SelectedItem = BACK_BUTTON;
        }
        else
        {
            switch ((char)gMENU_EnterYourName.Items[gMENU_EnterYourName.SelectedItem]->Name[0])
            {
                case 'M':
                case 'Z':
                case 'm':
                case 'z':
                {
                    gMENU_EnterYourName.SelectedItem -= ROW_WIDTH - 1;

                    break;
                }
                default:
                {
                    gMENU_EnterYourName.SelectedItem++;

                    break;
                }
            }
        }

        PlayGameSound(&gMenuSound);
    }

    if (gPlayers[PLAYER_ONE].Input.ChooseKeyIsDown && !gPlayers[PLAYER_ONE].Input.ChooseKeyWasDown)
    {
        if (gMENU_EnterYourName.Items[gMENU_EnterYourName.SelectedItem]->Enabled)
        {
            gMENU_EnterYourName.Items[gMENU_EnterYourName.SelectedItem]->Action();
        }
    }
}

void MenuFunc_EnterYourName_Add(void)
{
    if (strlen(gPlayers[PLAYER_ONE].Name) < MAX_PLAYER_NAME_LEN)
    {
        PlayGameSound(&gMenuChooseSound);

        gPlayers[PLAYER_ONE].Name[strlen(gPlayers[PLAYER_ONE].Name)] = gMENU_EnterYourName.Items[gMENU_EnterYourName.SelectedItem]->Name[0];
    }

    if (strlen(gPlayers[PLAYER_ONE].Name) > 0)
    {
        gMI_EnterYourName_OK.Enabled = TRUE;
    }
}

void MenuFunc_EnterYourName_Back(void)
{
    PlayGameSound(&gMenuChooseSound);

    if (strlen(gPlayers[PLAYER_ONE].Name) == 1)
    {
        gMI_EnterYourName_OK.Enabled = FALSE;
    }

    if (strlen(gPlayers[PLAYER_ONE].Name) < 1)
    {
        gGameState = GAMESTATE_TITLESCREEN;
    }
    else
    {
        gPlayers[PLAYER_ONE].Name[strlen(gPlayers[PLAYER_ONE].Name) - 1] = '\0';
    }    
}

void MenuFunc_EnterYourName_OK(void)
{
    if (strlen(gPlayers[PLAYER_ONE].Name) > 0)
    {
        PlayGameSound(&gMenuChooseSound); 

        if (InitializeNetwork() != 0)
        {
            gShouldExit = TRUE;

            return;
        }

        if (gNetworkData.IsHost)
        {
            gGameState = GAMESTATE_HOSTLANGAME;
        }
        else
        {
            gGameState = GAMESTATE_JOINLANGAME;
        } 

        // save the player's name for future use
        if (SavePlayerName() != ERROR_SUCCESS)
        {
            MessageBoxW(NULL, L"Failed to save the player's name!", L"WARNING", MB_ICONWARNING | MB_OK);
        }
    }
}