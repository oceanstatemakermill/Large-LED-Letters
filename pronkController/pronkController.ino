// PRONK Controller
//
// Electron controller for Large LED Letters for PRONK
//
// Controller receives commands from web and send them via I2C to individual letter controllers
//
// Kelly Egan 2017
// Ocean State Maker Mill
// http://oceanstatemakermill.org
//

#define SWITCH_PIN D5

#define NUM_LETTERS 5

#define P_ADDRESS 10
#define R_ADDRESS 11
#define O_ADDRESS 12
#define N_ADDRESS 13
#define K_ADDRESS 14

#define NUM_ANIMATIONS 7

#define FADE 0
#define SLIDE 1
#define DANCE 3
#define SPARKLE 4
#define RAINBOW 5
#define JUGGLE 6

#define NUM_COLORS 8

#define WHITE 0xFFFFFF
#define RED 0xCC0000
#define ORANGE 0xFF8000
#define YELLOW 0xFFFF00
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define DARKBLUE 0x004080
#define LIGHTBLUE 0x66BBFF
#define PINK 0xFF6060
#define PURPLE 0x7000DD
#define OSMM_TEAL 0x00EE70
#define BROWN 0x663300

#define DEMO_MODE 0
#define TWITTER_MODE 1

const unsigned long colors[] = {RED, ORANGE, YELLOW, GREEN, BLUE, PINK, PURPLE, OSMM_TEAL};
const unsigned long rainbow[] = {RED, YELLOW, GREEN, BLUE, PURPLE};
const unsigned long brownu[] = {BROWN, RED};
const unsigned long johnsonwales[] = {BLUE, YELLOW};
const unsigned long uri[] = {DARKBLUE, LIGHTBLUE};
const unsigned long rhodeisland[] = {DARKBLUE, YELLOW, WHITE};
const unsigned long pizza[] = {RED, GREEN, WHITE};
const unsigned long brazil[] = {GREEN, YELLOW, BLUE};
const unsigned long unitystreetband[] = {GREEN, PURPLE};
const unsigned long whatcheer[] = {RED, WHITE};

int mode;
int commandCode, lastCommandCode;
bool executeCommand;
int animationCode;
int pause;
unsigned long lastAnimation;
bool connectToCellular = false;

// Requires manually turning on cellular connection
SYSTEM_MODE(SEMI_AUTOMATIC);
// Enable threading so network connection doesn't stop other functions
SYSTEM_THREAD(ENABLED);

void setup() {
  mode = DEMO_MODE;
	pause = 10000;
	executeCommand = false;
	commandCode = 0;
	lastCommandCode = 0;

	//Check SWITCH to see if we should run demo
	pinMode(SWITCH_PIN, INPUT);
	int buttonState = digitalRead(SWITCH_PIN);

	if( buttonState == HIGH ) {
		mode = TWITTER_MODE;
		Particle.connect();
		Particle.function("led",ledAction);
	}

	Wire.begin();
	lastAnimation = millis();
}

void loop() {

	//Wait between running animation
	if( millis() - lastAnimation > pause ) {
		if( mode == TWITTER_MODE && Particle.connected()) {
			twitterMode();
		} else {
		  demoMode();
			pause = 10000;
		}
		lastAnimation = millis();
	}
}

/**
 *  Accept commands from the Internet.
 */
int ledAction(String command) {
		commandCode = command.toInt();
		if( commandCode > 0 ) {
			executeCommand = true;
			return 1;
		} else {
			return 0;
		}
}

/**
 *  Twitter mode runs animations based on commands from the Internet
 */
void twitterMode() {
	if( commandCode != lastCommandCode ) {
		switch( commandCode ){
			case 1:   //OSMM
				animateOneColor( OSMM_TEAL, JUGGLE, 10);
				pause = 30000;
				break;
			case 2:   //PRIDE
				animateFiveColors( rainbow, RAINBOW, 10, 1000);
				pause = 30000;
				break;
			case 3:   //Providence College (black and white)
				break;
			case 4:   //Brown University  (brown 0x663300 and red 0xCC0000)
				animateTwoColors( brownu, FADE, 3, 500);
				pause = 30000;
				break;
			case 5:   //Johnson and Whales (blue and yellow)
				animateTwoColors( johnsonwales, FADE, 3, 500);
				pause = 30000;
				break;
			case 6:   //URI (light blue and dark blue)
				animateTwoColors( uri, FADE, 3, 500);
				pause = 30000;
				break;
			case 7:   //Rhode Island (dark blue, yellow and white)
				animateThreeColors( rhodeisland, SLIDE, 5, 500);
				pause = 30000;
				break;
			case 8:   //Pink
				animateOneColor( PINK, DANCE, 10);
				pause = 30000;
				break;
			case 9:   //Dream
				break;
			case 10:  //Pizza (red, white and green)
				animateThreeColors( pizza, SLIDE, 5, 500);
				pause = 30000;
				break;
			case 11:
			case 12: //Chaotic Noise Marching Corps
				animateOneColor( BLUE, DANCE, 10);
				pause = 30000;
				break;
			case 13: //Damas de Ferro (Rio, BRAZIL)
				animateThreeColors( brazil, SLIDE, 10, 1000);
				pause = 30000;
				break;
			case 14: //Dead Music Capital Band
				animateOneColor( RED, DANCE, 10);
				pause = 30000;
				break;
			case 15: //Drumadics Beat-N-Brass Band (Brooklyn, NY)
				animateOneColor( YELLOW, SLIDE, 10);
				pause = 30000;
				break;
			case 16:
				animateOneColor( PURPLE, DANCE, 10);
				pause = 30000;
				break;
			case 17:
				animateOneColor( GREEN, FADE, 10);
				pause = 30000;
				break;
			case 18: //School of Honk (Somerville, MA)
				animateOneColor( ORANGE, DANCE, 10);
				pause = 30000;
				break;
			case 19:
				animateTwoColors( unitystreetband, FADE, 10, 1000);
				pause = 30000;
				break;
			case 20:
				animateTwoColors( whatcheer, DANCE, 10, 1000);
				pause = 30000;
				break;
				break;
		}
	} else {
		//Add some sparkles to the current colors
		animateOneColor( WHITE, SPARKLE, 10);
		pause = 10000;
	}

	lastCommandCode = commandCode;
}

/**
 *  Demo mode rotates through colors and animations
 */
void demoMode() {
    int index = random(NUM_COLORS);
    unsigned long color = colors[index];
    animationCode = (animationCode + 1) % NUM_ANIMATIONS;

    animateOneColor( color, animationCode, 4 );
}

/**
 *  Sends sign one color animation
 *  sends all letters same, color, animation and duration, simultaneously
 */
void animateOneColor( const unsigned long color, byte animationCode, byte duration) {
	animateLetter( 0 , color, animationCode, duration );
}

/**
 *  Sends sign two color animation
 */
void animateTwoColors( const unsigned long colors[], byte animationCode, byte duration, int pause ) {
	animateLetter( P_ADDRESS, colors[0], animationCode, duration );
	animateLetter( O_ADDRESS, colors[0], animationCode, duration );
	animateLetter( K_ADDRESS, colors[0], animationCode, duration );
  if( pause > 0 ) { delay(pause); }
  animateLetter( R_ADDRESS, colors[1], animationCode, duration );
  animateLetter( N_ADDRESS, colors[1], animationCode, duration );
}

/**
 *  Sends sign three color animations
 */
void animateThreeColors( const unsigned long colors[], byte animationCode, byte duration, int pause ) {
	animateLetter( P_ADDRESS, colors[0], animationCode, duration );
  animateLetter( K_ADDRESS, colors[4], animationCode, duration );
  if( pause > 0 ) { delay(pause); }
  animateLetter( R_ADDRESS, colors[1], animationCode, duration );
  animateLetter( N_ADDRESS, colors[3], animationCode, duration );
	if( pause > 0 ) { delay(pause); }
  animateLetter( O_ADDRESS, colors[2], animationCode, duration );
}


/**
 *  Sends sign five color animation
 *  colors[] - array gives color for each letter.
 *  animationCode - a single animation for all letters.
 *  duration - how long the animation will last
 *  pause - sets delay between animating each letter.
 */
void animateFiveColors( const unsigned long colors[], byte animationCode, byte duration, int pause ) {
  animateLetter( P_ADDRESS, colors[0], animationCode, duration );
  if( pause > 0 ) { delay(pause); }
  animateLetter( R_ADDRESS, colors[1], animationCode, duration );
  if( pause > 0 ) { delay(pause); }
  animateLetter( O_ADDRESS, colors[2], animationCode, duration );
  if( pause > 0 ) { delay(pause); }
  animateLetter( N_ADDRESS, colors[3], animationCode, duration );
  if( pause > 0 ) { delay(pause); }
  animateLetter( K_ADDRESS, colors[4], animationCode, duration );
}

/**
 *  Send animation command to individual letter controller
 */
void animateLetter( byte address, unsigned long color, byte animationCode, byte duration ) {
    Wire.beginTransmission(address);

    byte r = (color >> 16) & 0xFF;
    byte g = (color >> 8) & 0xFF;
    byte b = color & 0xFF;

    Wire.write(animationCode);
    Wire.write(r);
    Wire.write(g);
    Wire.write(b);
		Wire.write(duration);

    Wire.endTransmission();
}
