/*Esplora Sound Sensor*/
#include <Esplora.h>
//Configure sound threshold and sound pin
int thunderThreshold = 150;
int sound;

//Configure FastLED and output pin
#include <FastLED.h> // see http://fastled.io; uses branch FastLED3.1
// Configure LED driver
#define DATA_PIN 11        // digital pin used for LED strip/ring data line
#define LED_COUNT 3      // how many LEDs in the strip/ring
#define DRIVER WS2812B   // which FastLED driver to use

// Configure behavior
#define DECAY_DELAY 20    // delay between each tick
#define BRIGHT_BASE 32   // minimum brightness for a fresh drop
#define BRIGHT_ADD 127    // add this to above for maximum brightness of a fresh drop
#define CHANCE_OF_DROP 10 // the percent chance a dark drop will become fresh
#define DIM_RATE 8        // max amount to dim on each tick

// global variables
CRGB led[LED_COUNT];     // Addresses the pixels via FastLED, must be RGB space
CHSV drops[LED_COUNT];   // Addresses the colors using HSV space; must map to led[] to display


/* seed_drop() - a rain drop "hits" a pixel
 * note that this function doesn't specify which pixel, that's on you
 */
CHSV seed_drop() {
    CHSV drop = CHSV(HUE_BLUE, 255, random8(BRIGHT_BASE)+BRIGHT_ADD);
    return(drop);
}

/* In the Arduino setup phase, configure the driver, seed the first drops, and
 * show a black strip
 */

void setup() {
    pinMode(DATA_PIN, OUTPUT); //Defining DATA_PIN as the output for LEDs
    delay(250);  // on some boards, this _really_ helps with catching the reset for re-programming

    // Add LEDs, set the color correction profile, and set all pixels black
    FastLED.addLeds<DRIVER, DATA_PIN, GRB>(led, LED_COUNT).setCorrection(TypicalLEDStrip);
    fill_solid(led, LED_COUNT, CRGB::Black);

    // Seed first rain drops
    uint8_t numdrops = random8(LED_COUNT / 2 * 3);
    for (uint8_t i = 0; i < numdrops; i++) {
        drops[i] = seed_drop();
    }
    FastLED.show(); // this will show all black, because led[] is still all black
}

void thunderburst() {
  // this thunder works by lighting two random lengths
  // of the strand from 10-20 pixels. 
  int rs1 = random(0,LED_COUNT/2);
  int rl1 = random(10,20);
  int rs2 = random(rs1+rl1,LED_COUNT);
  int rl2 = random(10,20);
  
  //repeat this chosen strands a few times, adds a bit of realism
  for(int r = 0;r<random(3,6);r++){
    
    for(int i=0;i< rl1; i++){
      led[i+rs1] = CHSV( 0, 0, 255);
    }
    
    if(rs2+rl2 < LED_COUNT){
      for(int i=0;i< rl2; i++){
        led[i+rs2] = CHSV( 0, 0, 255);
      }
    }
    
    FastLED.show();
    //stay illuminated for a set time
    delay(random(10,50));
    
    reset();
    delay(random(10,50));
  }
}

void reset(){
  for (int i=0;i<LED_COUNT;i++){
    led[i] = CHSV( 0, 0, 0);
  }
  FastLED.show();
   
}

/* In the Arduino loop phase, each tick dims bright drops and has a chance to
 * make fresh drops out of dim/dark pixels
 */
void loop() {
sound = Esplora.readMicrophone();//read the sensor into a variable
if(sound<=thunderThreshold){ //defining action under sound threshold
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        // if the drops are very dim or off, there's a chance to make them fresh drops on this tick
        if (drops[i].v <= 10) {
            if (random8(100) < CHANCE_OF_DROP) {
                drops[i] = seed_drop();
            }
        }

        // if they are lit *at all*, dim them by 0..DIM_RATE
        if (drops[i].v > 0) {
            drops[i].v = drops[i].v-random8(DIM_RATE);
        }

        led[i] = drops[i]; // Map to CRGB space for display
        /* TODO: use a FastLED fade function
         * the fade functions can work on RGB space, avoid mapping, and that
         * would improve performance and shrink the compiled sketch (proabably)
         */
    }

    // Show, then wait for next tick. Personal preference, you could reverse these
    FastLED.show();
    FastLED.delay(DECAY_DELAY); 
}
//defining the lightning effect levels
sound = Esplora.readMicrophone();//read the sensor into a variable
if(sound>=(thunderThreshold)){ //defining action at threshold

//Starting thunderburst definition
thunderburst();
reset();
  }
}
