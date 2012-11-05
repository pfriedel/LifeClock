/*
  USE_GITHUB_USERNAME=pfriedel
*/

#include <EEPROM.h>        // stock header
#include <avr/pgmspace.h>  //AVR library for writing to ROM
#include "Charliplexing.h"
#include "RealTimeClockDS1307.h"
#include "tinyfont.h" 
#include <stdio.h>

#define COLS 10 // usually x
#define ROWS 11 // usually y

// set to true to enable serial monitor - warning: uses ~2k of progmem and just
// shy of 256 bytes of ram.
#define _DEBUG_ true

uint8_t max_brightness=13;

byte world[COLS][ROWS][2]; // Create a double buffered world.

//--------------------------------------------------------------------------------
void setup() {

  if(_DEBUG_) { Serial.begin(115200); }

  // Initialize the screen  
  LedSign::Init(GRAYSCALE);
  LedSign::SetBrightness(31);

}

void loop() {
  Particles();
}

//--------------------------------------------------------------------------------
// functions


/** Writes an array to the display
 * @param str is the array to display
 * @param speed is the inter-frame speed
 */
//void Banner ( String str, int speed ) { // this works, hogs memory.
void Banner ( char* str, int speed, int y) {
  // length is length(array)
  // width is the width of the array in pixels.
  // these can be unsigned - int8 might be too small
  uint8_t width=0, length=0;
  
  if(_DEBUG_) { Serial.print("Banner text: "); Serial.println(str); }
  
  // figure out pixel width of str
  while(str[length]!='\0') { //read to the end of the string
    int charwidth=0;
    length++;
    // get the character's width by calling Font_Draw in the "Don't draw this, just tell me how wide it is" mode.
    // It would probably be faster to have a lookup table.
    charwidth = Font_Draw(str[length],0,0,0);
    // adds the width of ths current character to the pixel width.
    width=width+charwidth; 
  }

  // these need to be signed, otherwise I can't draw off the left side of the screen.
  int8_t x=0, x2=0; // x position buckets
  
  // j is the virtual display width from the actual rightmost column to a 
  // virtual column off the left hand side.  Decrements, so the scroll goes
  // to the left.
  for(int8_t j=5; j>=-(width+5); j--) {  // FIXME: this might need fixing for wider arrays 
    // x starts out at j (which is always moving to the left, remember)
    x=j; 
    // clear the sign
    LedSign::Clear(); 
    // walk through the array, drawing letters where they belong.
    for(int i=0; i<length; i++) { 
      x2 = Font_Draw(str[i],x,y,7);
      // sets the new xpos based off of the old xpos + the width of the 
      // current character.
      x+=x2;
    }
    delay(speed);
  }
}


/** Draws a figure (0-Z). You should call it with set=1, 
 * wait a little them call it again with set=0
 * @param figure is the figure [0-9]
 * @param x,y coordinates, 
 * @param set is 1 or 0 to draw or clear it
 */
uint8_t Font_Draw(unsigned char letter,int x,int y,int set) {
  uint16_t maxx=0;
  
  uint8_t charCol;
  uint8_t charRow;

  prog_uchar* character;
  if (letter==' ') return 2+2; // if I'm sent a space, just tell the called that the end column is 4 spaces away.
  if (letter<fontMin || letter>fontMax) { // return a blank if the sender tries to call something outside of my font table
    return 0;
  }
  character = font[letter-fontMin];

  int i=0;

  charCol = pgm_read_byte_near(character);
  charRow = pgm_read_byte_near(character + 1);

  while (charRow != 9) { // the terminal 9 ends the font element.
    if (charCol>maxx) maxx=charCol; // increment the maximum column count
    if ( // if the resulting pixel would be onscreen, send it to LedSign::Set.
	charCol + x < COLS && 
	charCol + x >= 0 && 
	charRow + y < ROWS && 
	charRow + y >= 0
	) {
      LedSign::Set(charCol + x, charRow + y, set);
    } 
    i+=2; // only get the even elements of the array.

    charCol = pgm_read_byte_near(character + i);
    charRow = pgm_read_byte_near(character + i + 1);
  }
  return maxx+2; // return the rightmost column + 2 for spacing.
}


void Particles() {
  const int numparticles = 2;

  long p_framecount = 0;

  int velx[numparticles];
  int vely[numparticles];
  int posx[numparticles];
  int posy[numparticles];

  // How big do you want the virtual particle space to be?  Higher resolution
  // means slower but more precise particles (without shortening the delay)
  int res = 7;

  // And how big is the display space? (actual pixels, not off-by-one)
  #define DISPLAY_X 10
  #define DISPLAY_Y 11

  int max_x = DISPLAY_X<<res;
  int max_y = DISPLAY_Y<<res;

  int maxspeed = 30;

  int row, col, i;

  for(i = 0; i<numparticles; i++) {
    velx[i] = random(11)+5;
    vely[i] = random(11)+5;
    posx[i] = (i%2)<<res;
    posy[i] = (i/2)<<res;
  }

  while(1) {
    p_framecount++;
    LedSign::Clear();

    // This only really works for particles % 2 = 0
    // Draw the Qix for each pair of particles
    for( i = 0; i < numparticles; i = i+2) {
      LedSign::drawLine(posx[i]>>res, posy[i]>>res, posx[i+1]>>res, posy[i+1]>>res,7);
    }

    // Update the location of the particles for the next frame.
    for( i = 0; i<numparticles; i++) {

      // The new positions are the old positions plus the velocity in the correct axis.
      posx[i]+=velx[i];
      posy[i]+=vely[i];

      // If the x position will go below 0
      if(posx[i] < 0) {
        posx[i] = 1; // the new x position is 1;
        velx[i] = -velx[i]; // The velocity on the x axis inverts (boing!)
        vely[i] = vely[i]+random(-1,1); // The velocity on the y axis gets a small random change.

        if(vely[i]>maxspeed) vely[i] = maxspeed; // if the new y velocity is too fast, pin it to the maximum speed.
        else if(vely[i]<-maxspeed) vely[i] = -maxspeed;
      }

      else if(posx[i]>=max_x) {
        posx[i] = max_x+(max_x-posx[i]);
        velx[i] = -velx[i];
        vely[i] = vely[i]+random(-1,1);

        if(vely[i]>maxspeed) vely[i] = maxspeed;
        else if(vely[i]<-maxspeed) vely[i] = -maxspeed;
      }

      if(posy[i]<0) {
        posy[i] = 1;
        vely[i] = -vely[i];
	velx[i] = velx[i]+random(-1,1);

        if(velx[i]>maxspeed) velx[i] = maxspeed;
        else if(velx[i]<-maxspeed) velx[i] = -maxspeed;
      }
      else if(posy[i]>=max_y) {
        posy[i] = max_y+(max_y-posy[i]);
        vely[i] = -vely[i];
        velx[i] = velx[i]+random(-1,1);

        if(velx[i]>maxspeed) velx[i] = maxspeed;
        else if(velx[i]<-maxspeed) velx[i] = -maxspeed;
      }

    }
    delay(5);
  }
}
