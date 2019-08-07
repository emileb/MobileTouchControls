//
// Created by Emile on 03/05/2017.
//

#include "UI_Keyboard.h"
#include "TouchControlsConfig.h"
#include <math.h>
#include <iostream>
#include <cstring>

#include "SDL_keycode.h"

using namespace touchcontrols;

#define Y_START_POS  0.50
#define ROW_HEIGHT   0.1
#define ROW_GAP      0.001

#define EVEN_SPACE ( 1.f / MAX_ROW_KEYS )

UI_Keyboard::UI_Keyboard( std::string tag, RectF pos, std::string font_filename, int fontSet, uint32_t params, float textSize ):
    ControlSuper( TC_TYPE_UI_KEYBOARD, tag, pos ),
    font_filename(font_filename),
    fontSet(fontSet)
{
    std::memset(&layout,0,sizeof(layout));
    setKey( 0, 0, 'X',  0, 0.05f, 0, "key_tab"  );
    setKey( 0, 1, '1', '1', EVEN_SPACE, 0 );
    setKey( 0, 2, '2', '1', EVEN_SPACE, 0 );
    setKey( 0, 3, '3', '1', EVEN_SPACE, 0 );
    setKey( 0, 4, '4', '1', EVEN_SPACE, 0 );
    setKey( 0, 5, '5', '1', EVEN_SPACE, 0 );
    setKey( 0, 6, '6', '1', EVEN_SPACE, 0 );
    setKey( 0, 7, '7', '1', EVEN_SPACE, 0 );
    setKey( 0, 8, '8', '1', EVEN_SPACE, 0 );
    setKey( 0, 9, '9', '1', EVEN_SPACE, 0 );
    setKey( 0, 10, '0', '1', EVEN_SPACE, 0 );


    setKey( 1, 0, 'Q', '~', EVEN_SPACE, 0 );
    setKey( 1, 1, 'W', '!', EVEN_SPACE, 0 );
    setKey( 1, 2, 'E', '\\', EVEN_SPACE, 0 );
    setKey( 1, 3, 'R', '%', EVEN_SPACE, 0 );
    setKey( 1, 4, 'T', '^', EVEN_SPACE, 0 );
    setKey( 1, 5, 'Y', '?', EVEN_SPACE, 0 );
    setKey( 1, 6, 'U', '<', EVEN_SPACE, 0 );
    setKey( 1, 7, 'I', '>', EVEN_SPACE, 0 );
    setKey( 1, 8, 'O', '[', EVEN_SPACE, 0 );
    setKey( 1, 9, 'P', ']', EVEN_SPACE, 0 );
    setKey( 1, 10, 8,    0, EVEN_SPACE, 0, "key_backspace" );  //Backspace

    setKey( 2, 0, '\t', 0,     0.05f, 0,"key_tab" );  //blank
    setKey( 2, 1, 'A', '@', EVEN_SPACE, 0 );
    setKey( 2, 2, 'S', '#', EVEN_SPACE, 0 );
    setKey( 2, 3, 'D', '&', EVEN_SPACE, 0 );
    setKey( 2, 4, 'F', '*', EVEN_SPACE, 0 );
    setKey( 2, 5, 'G', '-', EVEN_SPACE, 0 );
    setKey( 2, 6, 'H', '+', EVEN_SPACE, 0 );
    setKey( 2, 7, 'J', '=', EVEN_SPACE, 0 );
    setKey( 2, 8, 'K', '(', EVEN_SPACE, 0 );
    setKey( 2, 9, 'L', ')', EVEN_SPACE, 0 );
    setKey( 2, 10, 13,   0,   0.145f, 0,"enter_key" );  //Enter

    setKey( 3, 0,  UI_KEYBOARD_SHIFT,  0, EVEN_SPACE, 0, "key_shift" );  //Shift key
    setKey( 3, 1, 'Z', '_', EVEN_SPACE, 0 );
    setKey( 3, 2, 'X', '$', EVEN_SPACE, 0 );
    setKey( 3, 3, 'C', '"', EVEN_SPACE, 0 );
    setKey( 3, 4, 'V', '\'', EVEN_SPACE, 0 );
    setKey( 3, 5, 'B', ':', EVEN_SPACE, 0 );
    setKey( 3, 6, 'N', ';', EVEN_SPACE, 0 );
    setKey( 3, 7, 'M', '/', EVEN_SPACE, 0 );
    setKey( 3, 8, '_',  0, EVEN_SPACE, 0 );
    setKey( 3, 9, SDLK_UP, 0, 0.07f, 0.016f, "key_arrow_up" );
    setKey( 3, 10, UI_KEYBOARD_SYMBOLS, 0, 0.07f, 0.016f, "key_symbol_shift" );

    setKey( 4, 0, UI_KEYBOARD_HIDE, 0, 0.10f, 0, "hide_key" ); //Hide keyboard
    setKey( 4, 1, ',', 0, 0.1f, 0 );
    setKey( 4, 2, ' ', 0, 0.48f, 0 );
    setKey( 4, 3, '.', 0, 0.084f, 0 );
    setKey( 4, 4, SDLK_LEFT, 0, 0.07f, 0, "key_arrow_left" );
    setKey( 4, 5, SDLK_DOWN, 0, 0.07f, 0, "key_arrow_down"  );
    setKey( 4, 6, SDLK_RIGHT, 0, 0.07f, 0, "key_arrow_right" );
    setKey( 4, 7, 0, 0, 0.1f, 0 );
    setKey( 4, 8, 0, 0, 0.1f, 0 );
    setKey( 4, 9, 0, 0, 0.1f, 0 );
    setKey( 4, 10, 0, 0, 0.1f, 0 );

    pressedKey = NULL;
    touchId = -1;
	shiftActive = false;
    symbolActive = false;

    // -1 is not yet used
    selectedX = -1;
    selectedRow = -1;
}

uint32_t UI_Keyboard::shiftKey( uint32_t key )
{
	if( shiftActive && key >= 'A' &&  key <= 'Z' )
	{
		return key + 32;
	}
	else
	{
		return key;
	}
}

void UI_Keyboard::setKey( uint32_t row, uint32_t key, uint32_t keyPrim, uint32_t keyAlt, float width, float padLeft, const char* primImg, const char * altImg )
{
    layout.rows[row].keys[key].keyPrim = keyPrim;
    layout.rows[row].keys[key].keyAlt  = keyAlt;
    layout.rows[row].keys[key].width   = width;
    layout.rows[row].keys[key].padLeft = padLeft;
    layout.rows[row].keys[key].primImg = primImg;
    layout.rows[row].keys[key].altImg  = altImg;
    layout.rows[row].keys[key].glPrim  = 0;
    layout.rows[row].keys[key].glAlt   = 0;
}

void UI_Keyboard::keyDown( KeyboardKey *key )
{
    LOGTOUCH("keyDown: %c",key->keyPrim);
    pressedKey = key;
    timeDown = getMS();
    useAltKey = false;
}

void UI_Keyboard::keyUp()
{
    LOGTOUCH("keyUp");
    if( pressedKey != NULL )
    {
		if( pressedKey->keyPrim == UI_KEYBOARD_SHIFT )
        {
			shiftActive = !shiftActive;
        }
        else if( pressedKey->keyPrim == UI_KEYBOARD_SYMBOLS )
        {
            symbolActive = !symbolActive;
        }
        else
        {
	        if( useAltKey  || (symbolActive && pressedKey->keyAlt ))
	            signal.emit( pressedKey->keyAlt );
	        else
	        {
                signal.emit( shiftKey(pressedKey->keyPrim) );
            }
		}

		signal_vibrate.emit(SHORT_VIBRATE);

        pressedKey = NULL;
    }
}

bool UI_Keyboard::processPointer(int action, int pid, float x, float y)
{
	if (action == P_DOWN)
	{
		if (touchId == -1) //Only process if not active
		{
			if( y > Y_START_POS )
			{
				touchId = pid;

				int row = (y - Y_START_POS) / (ROW_HEIGHT + ROW_GAP);
                if(row < NBR_ROWS)
                {
                    float xPos = 0;

                    for(int n = 0; n < MAX_ROW_KEYS; n++ )
                    {
                        KeyboardKey *key = &(layout.rows[row].keys[n]);
                        if( x < xPos + key->width + key->padLeft)
                        {
                            keyDown( key );
                            break;
                        }

                        xPos = xPos + key->width + key->padLeft;
                    }
                }

				return true;
			}
		}
		return false;
	}
	else if (action == P_UP)
	{
		if ( pid == touchId )
		{
		    keyUp();

		    touchId = -1;
			return true;
		}
		return false;
	}
	else if(action == P_MOVE)
	{
		return false;
	}

    return false;
}

// Scan keys to find the X centre of the key
float UI_Keyboard::findXCenter( uint32_t row, uint32_t key )
{
    float centre = 0;
    uint32_t n = 0;
    while(n != key)
    {
        centre += layout.rows[row].keys[n].width;
        n++;
    }

    centre += layout.rows[row].keys[key].width / 2;

    return centre;
}


bool UI_Keyboard::gamepadInput(bool down, GamePadKey gamepadKey)
{
    // Check gamepad has been used yet
    if( selectedX == -1 )
    {
        // Select G as default
        selectedX = 6;
        selectedRow = 1;
    }
    else
    {
        if( down )
        {
            switch( gamepadKey )
            {
                case LEFT:
                {
                    while (true)
                    {
                        selectedX--;
                        if(selectedX < 0)
                            selectedX = MAX_ROW_KEYS-1;

                        // Check key is valid
                        if( layout.rows[selectedRow].keys[selectedX].keyPrim != 0 )
                            break;
                    }
                }
                break;

                case RIGHT:
                {
                    while (true)
                    {
                        selectedX++;
                        if(selectedX == MAX_ROW_KEYS)
                            selectedX = 0;

                        // Check key is valid
                        if( layout.rows[selectedRow].keys[selectedX].keyPrim != 0 )
                            break;
                    }
                }
                break;

                case DOWN:
                case UP:
                {
                    // Find the closest key to the key on the current row
                    float currentXCentre = findXCenter(selectedRow,selectedX);

                    if( gamepadKey == UP )
                    {
                        selectedRow--;
                        if(selectedRow < 0)
                            selectedRow = NBR_ROWS - 1;
                    }
                    else // DOWN
                    {
                        selectedRow++;
                        if(selectedRow == NBR_ROWS)
                            selectedRow = 0;
                    }

                    uint32_t closestKey = 0;
                    float closestDistance = 0xFFFFFFF;
                    for( int n = 0; n < MAX_ROW_KEYS; n++ )
                    {
                        // Only valid keys
                        if( layout.rows[selectedRow].keys[n].keyPrim != 0 )
                        {
                            float dist = fabs(currentXCentre - findXCenter(selectedRow,n));
                            if( dist < closestDistance)
                            {
                                closestDistance = dist;
                                closestKey = n;
                            }
                        }
                    }

                    selectedX = closestKey;
                }
                break;

                case SELECT:
                case BACK:
                {
                }
                break;
            }
        }

        if( gamepadKey == SELECT )
        {
            if( down )
                keyDown( &layout.rows[selectedRow].keys[selectedX] );
            else
                keyUp();
        }
        else if( gamepadKey == BACK )
        {
            signal.emit( UI_KEYBOARD_HIDE );
        }
    }

    return true;
}

void UI_Keyboard::resetOutput()
{
    symbolActive = false;
}

bool UI_Keyboard::drawGL(bool forEditor)
{
    float rowXPos   = 0;
    float rowYPos   = Y_START_POS;
    float rowHeight = ROW_HEIGHT;
    float rowGap    = ROW_GAP;

    float alpha = 0.8;

    gl_setFixAspect( false );

    GLRect glRect;

    // Draw background
    glRect.resize( 1, 0.5 );
    gl_drawRect( 0, 0, 0, 0.8, 0, rowYPos, glRect );

    gl_color4f(1,1,1,alpha);

    for( int row = 0; row < NBR_ROWS; row++ )
    {
        rowXPos = 0;
        for( int n = 0; n < MAX_ROW_KEYS; n++ )
        {
            KeyboardKey *key = &(layout.rows[row].keys[n]);

            if( key->keyPrim != 0 )
            {
                // Add any padding
                rowXPos += key->padLeft;

                // Draw key background
                if( key == pressedKey ) //Make dark if pressed
                {
                    gl_color4f( 0.5, 0.5, 0.5, 1 );

                    // Check if button is held down
                    if( key->keyAlt && getMS() > timeDown + 500 )
                    {
                        useAltKey = true;
                    }
                }
                else if( (row == selectedRow) && (n == selectedX) ) // Show selected gamepad key
                {
                     gl_color4f( 0, 0, 1, 1 ); // Make very blue
                }
                else
                {
                    gl_color4f( 1, 1, 1, alpha );
                }

                glRect.resize( key->width, rowHeight );
                gl_drawRect( glKeyBg, rowXPos, rowYPos, glRect );

                //Top row is different color
                if( row == 0 )
                    gl_color4f( 0.7, 0.7, 1, alpha );
                else
                    gl_color4f( 1, 1, 1, alpha );

                // Check is symbol shift is active and if there is an alt key
                // Just show the alt key if so
                if( symbolActive && key->keyAlt )
                {
                    textDrawer.drawChar( key->keyAlt, fontSet, rowXPos +  (key->width * 0.5), rowYPos, 0.1, TEXT_DRAW_X_CENTER );
                }
                else
                {
                    // Check for custom image, otherwise use font
                    if( key->glPrim != 0 )
                    {
                        // Make images a bit greyer than letters
                        gl_color4f( 0.5, 0.5, 0.5, alpha );
                        gl_drawRect( key->glPrim , rowXPos, rowYPos, glRect );
                    }
                    else
                    {
                        textDrawer.drawChar( shiftKey(key->keyPrim), fontSet, rowXPos +  (key->width * 0.5), rowYPos, 0.1, TEXT_DRAW_X_CENTER );

                        if( key->keyAlt )
                           textDrawer.drawChar( key->keyAlt, fontSet, rowXPos +  (key->width * 0.8), rowYPos, 0.05, TEXT_DRAW_X_CENTER );
                    }
                }

                // Finger has been press for a while so show the altkey above the finger
                if( key == pressedKey && useAltKey )
                {
                    textDrawer.drawChar( key->keyAlt, fontSet, rowXPos +  (key->width * 0.5), rowYPos - rowHeight, 0.1, TEXT_DRAW_X_CENTER );
                }
            }

            rowXPos = rowXPos + key->width;
        }
        rowYPos = rowYPos + rowHeight + rowGap;
    }
    gl_color4f(1,1,1,1);

    gl_setFixAspect( true );

    return false;
}

bool UI_Keyboard::initGL()
{
    textDrawer.initGL(font_filename);
    int x, y;
    glKeyBg = loadTextureFromPNG( "key_background", x, y );

    // Load any custom image for the keys
    for( int row = 0; row < NBR_ROWS; row++ )
    {
        for(int n = 0; n < MAX_ROW_KEYS; n++ )
        {
            KeyboardKey *key = &(layout.rows[row].keys[n]);
            if(key->primImg != NULL)
                key->glPrim = loadTextureFromPNG( key->primImg, x, y );

            if(key->altImg != NULL)
                key->glAlt = loadTextureFromPNG( key->altImg, x, y );
        }
    }
    return false;
}

void UI_Keyboard::updateSize()
{

}

void UI_Keyboard::saveXML(TiXmlDocument &doc)
{

}

void UI_Keyboard::loadXML(TiXmlDocument &doc)
{

}