#include <TinyGPSPlus.h>  // Library for GPS functionality
#include <Adafruit_NeoPixel.h>  // Library for NeoPixel LED control
#include <SoftwareSerial.h>  // Library for software serial communication
#include <HardwareSerial.h>  // Library for hardware serial communication
#include <Wire.h>  // Library for I2C communication
#include <DFRobot_DF1201S.h>  // Library for MP3 module control
#include <Stepper.h>  // Library for stepper motor control

#define PIN    16  // Data pin for NeoPixel
#define NUMPIXELS 1  // Number of NeoPixels


// Define the GPS Serial Port (Use Hardware Serial1)
#define RXPin 0  // Feather M4 RX1 (connect to GPS TX)
#define TXPin 1  // Feather M4 TX1 (connect to GPS RX)
#define GPSBaud 9600


// Stepper motor rotation control variables
int Voltas_Motor = 3;  // Number of motor rotations
const int stepsPerRevolution = 1700;  // Number of steps for one full revolution



// TinyGPSPlus object for GPS functionality
TinyGPSPlus gps;
Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);  // NeoPixel object
SoftwareSerial DF1201SSerial(25, 24);  // SoftwareSerial for MP3 module communication
DFRobot_DF1201S DF1201S;  // MP3 module object
Stepper stepper(stepsPerRevolution, 10, 11, 12, 13);  // Stepper motor 1 (4-pin connection)
Stepper stepper2(stepsPerRevolution, 3, 7, 8, 9);  // Stepper motor 2 (4-pin connection)



// Button Pin
#define BTN_PIN 6





int flag;  // Variable for flagging LED actions
const int intervalBlink = 200;  // Interval for LED blinking (in ms)
const int pauseTime = 10000;  // Pause time for LED after blinking (in ms)
unsigned long previousBlink = 0;  // Time tracking for blinking
unsigned long startBlinkTime = 0;  // Start time for pause after blinking
bool isBlinking = true;  // Flag to check if LED should be blinking
int blinkCount = 0;  // Counter for blink cycles
bool BlinkState = LOW;  // Current state of the LED (on/off)

int Brilho = 150;  // Brightness level for NeoPixel

// Structure to hold coordinates and species information
struct CoordinatesBirds {
  float longitude;  // Longitude of bird location
  float latitude;   // Latitude of bird location
  int species;      // Species ID
};

// Number of coordinates and pointer to coordinate array
int numCoordinates;
CoordinatesBirds *coordinates;

// Earth's radius in meters for Haversine distance calculation
const float R = 6371000;


// Function prototypes
CoordinatesBirds *loadCoordinatesFiles(int &numCoordinates);
float haversineDistance(float lat1, float lon1, float lat2, float lon2);
float toRadians(float degree);
void TesteBegin();


void setup() {

  
  Serial.begin(115200);  // Initialize USB Serial for debugging
  Serial1.begin(GPSBaud);  // Initialize GPS on Serial1
  strip.begin();  // Initialize NeoPixel strip
  strip.setBrightness(Brilho);  // Set NeoPixel brightness  
  DF1201SSerial.begin(115200);  // Initialize communication with MP3 module
  while(!DF1201S.begin(DF1201SSerial)){
    Serial.println("Init failed, please check the wire connection!");
    delay(1000);  // Retry if initialization fails
  }


  // MP3 Module Settings
  DF1201S.setVol(100);  // Set volume
  DF1201S.switchFunction(DF1201S.MUSIC);  // Set MP3 module mode to music
  DF1201S.setPlayMode(DF1201S.SINGLE);  // Set single track play mode
  DF1201S.setPrompt(false);  // Disable prompt sounds

  Serial.println(F("GPS Example for Adafruit Feather M4 Express"));
  Serial.print(F("Using TinyGPSPlus library v. "));
  Serial.println(TinyGPSPlus::libraryVersion());

  pinMode(BTN_PIN, INPUT_PULLUP); // Enable internal pull-up resistor

  stepper.setSpeed(40);  // Set speed for stepper motor 1
  stepper2.setSpeed(80);  // Set speed for stepper motor 2



  // Load predefined bird coordinates
  coordinates = loadCoordinatesFiles(numCoordinates);

  // Perform hardware test
  TesteBegin();
  
  DF1201S.pause();  // Pause the MP3 playback after the test
}

void loop() {
  // Read GPS data
  while (Serial1.available() > 0) {
    if (gps.encode(Serial1.read())) {
      displayInfo();
    }
  }

  // Check if GPS is connected
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    while (true); // Stop program if no GPS detected
  }
}

/*************************************
  Function: displayInfo
  Description: Displays GPS info, controls music, LED, and motor.
*************************************/
void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.println();
  Serial.println(numCoordinates);

  // Loop through predefined coordinates and calculate distance
  for (int i = 0; i < numCoordinates; i++) {
    float distancia = haversineDistance(coordinates[i].latitude, coordinates[i].longitude, gps.location.lat(), gps.location.lng());


    // If the distance is less than 40 meters
    if (distancia < 40) {
      Serial.println("Musica");
      if (digitalRead(BTN_PIN) == LOW) {   // Check if button is pressed
        Serial.println("Botão pressionado");


        // Control motor and play music based on species ID
        if (coordinates[i].species == 1) {
          while (Voltas_Motor != 0) {
            
            stepper.step(stepsPerRevolution);
            stepper.step(-stepsPerRevolution);
            Voltas_Motor = Voltas_Motor - 1;
          }
          DF1201S.playFileNum(2);
          delay(8000);
          DF1201S.pause();
          DF1201S.next();
          DF1201S.pause();
        }
        if (coordinates[i].species == 2) {
          while (Voltas_Motor != 0) {
            
            stepper2.step(stepsPerRevolution);
            stepper2.step(-stepsPerRevolution);
            Voltas_Motor = Voltas_Motor - 1;
          }
          DF1201S.playFileNum(3);
          delay(8000);
          DF1201S.pause();
          DF1201S.next();
          DF1201S.pause();
        }
        
      } else {
        Serial.println("Botão não pressionado");
        DF1201S.pause();
      }
      
      strip.setPixelColor(0, strip.Color(255, 255, 255));  // Red color
      strip.show();
      flag = i;

      
    }

    if (distancia > 40 && flag == i) {
      DF1201S.pause();



      unsigned long currentMILLIS = millis();
    
    if (isBlinking) {
        if (currentMILLIS - previousBlink >= intervalBlink) {
            previousBlink = currentMILLIS;
            BlinkState = !BlinkState; // Alterna estado do LED
            
            if (BlinkState) {
                strip.setPixelColor(0, strip.Color(255, 255, 255));
            } else {
                strip.setPixelColor(0, strip.Color(0, 0, 0));
                blinkCount++;
            }
            strip.show();
            
            if (blinkCount >= 3) { // Após 3 piscadas
                isBlinking = false;
                startBlinkTime = currentMILLIS;
            }
        }
    } else {
        if (currentMILLIS - startBlinkTime >= pauseTime) {
            isBlinking = true;
            blinkCount = 0;
        }
    }







      
      strip.show();
      Voltas_Motor = 3;
    }
  }
}







/*************************************
  Function: haversineDistance
  Description: Calculates distance between two coordinates.
*************************************/
float haversineDistance(float lat1, float lon1, float lat2, float lon2)
{
  // Convert latitude and longitude from degrees to radians
  lat1 = toRadians(lat1);
  lon1 = toRadians(lon1);
  lat2 = toRadians(lat2);
  lon2 = toRadians(lon2);

  // Difference in coordinates
  float dLat = lat2 - lat1;
  float dLon = lon2 - lon1;

  // Haversine formula
  float a = sin(dLat / 2) * sin(dLat / 2) +
            cos(lat1) * cos(lat2) *
                sin(dLon / 2) * sin(dLon / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));

  // Distance in meters
  float distance = R * c;

  return distance;
}

/*************************************
  Function: toRadians
  Description: Converts degrees to radians.
*************************************/
float toRadians(float degree) {
  return degree * (PI / 180);
}

/*************************************
  Function: loadCoordinatesFiles
  Description: Loads predefined coordinates.
*************************************/
CoordinatesBirds *loadCoordinatesFiles(int &numCoordinates) {
  static CoordinatesBirds coordinates[] = {
      {-9.140070, 38.720580, 2},
      {-9.139885, 38.722155, 2},
      {-9.141329, 38.720367, 1},
      {-16.896143, 32.662378, 2},
      {-17.029402, 32.787575, 2},
      {-17.104467, 32.67932, 2},
      {-16.899033, 32.666623, 2},
      {-16.899033, 32.666623, 2},
      {-16.899033, 32.666623, 2},
      {-16.899033, 32.666623, 2},
      {-16.899033, 32.666623, 2},
      {-16.899033, 32.666623, 2},
      {-16.899033, 32.666623, 2},
      {-16.899033, 32.666623, 2},
      {-16.920018, 32.648677, 2},
      {-17.1284791349, 32.7598684668, 2},
      {-16.9021080807, 32.6742936837, 2},
      {-16.746457, 32.736317, 2},
      {-17.07003012, 32.67410209, 2},
      {-16.8948631, 32.66244214, 2},
      {-16.78543568, 32.76008645, 2},
      {-17.1884119128, 32.8355579818, 2},
      {-16.7676446453, 32.7206853441, 2},
      {-16.9020799851, 32.6740540238, 2},
      {-16.8685332416, 32.6585412143, 2},
      {-16.8664774919, 32.6561218031, 2},
      {-17.2112313889, 32.7385113889, 2},
      {-16.902449, 32.674044, 2},
      {-16.8836836978, 32.6684373732, 2},
      {-16.9070393546, 32.6605783447, 2},
      {-16.94407741, 32.63431729, 2},
      {-16.8673070088, 32.6600975848, 2},
      {-17.1468113889, 32.8146430556, 2},
      {-16.791291, 32.7318412, 2},
      {-16.8975171212, 32.6779332804, 2},
      {-16.8960666667, 32.6625366667, 2},
      {-16.8946223706, 32.6635478041, 2},
      {-17.15469891, 32.85406549, 2},
      {-17.11473345, 32.79850817, 2},
      {-17.1260499722, 32.7539889722, 2},
      {-17.0349250361, 32.6902983407, 2},
      {-17.0411541313, 32.6810122357, 2},
      {-17.18980045, 32.84171656, 2},
      {-17.225842, 32.8278949722, 2},
      {-16.838792, 32.68665, 2},
      {-17.147469, 32.817188, 2},
      {-16.8051611111, 32.7033444444, 2},
      {-16.8956111111, 32.6622694444, 2},
      {-16.8658586964, 32.6580095336, 2},
      {-16.8947276825, 32.6625161896, 2},
      {-16.8685422, 32.6627915, 2},
      {-16.8846091491, 32.7175090756, 2},
      {-16.8846091491, 32.7175090756, 2},
      {-16.8846091491, 32.7175090756, 2},
      {-17.1701083414, 32.8654708239, 2},
      {-17.1322550124, 32.7938979331, 2},
      {-17.1723372303, 32.7921465314, 2},
      {-16.8962533333, 32.66268, 2},
      {-16.8989923596, 32.651567874, 2},
      {-16.8934022355, 32.8267593931, 2},
      {-16.7974074535, 32.7405915034, 2},
      {-16.7974074535, 32.7405915034, 2},
      {-16.9492034659, 32.6385882763, 2},
      {-16.86605, 32.6576039722, 2},
      {-16.901832819, 32.6746413777, 2},
      {-17.146725, 32.814525, 2},
      {-16.9116527215, 32.6478598985, 2},
      {-16.9516930729, 32.6856729043, 2},
      {-16.9736989722, 32.6466589722, 2},
      {-16.9243684396, 32.8171487374, 2},
      {-16.8947544, 32.6624061, 2},
      {-16.8947544, 32.6624061, 2},
      {-16.849763, 32.790943, 2},
      {-16.8273055841, 32.7731531121, 2},
      {-16.91424636, 32.65097144, 2},
      {-16.91424636, 32.65097144, 2},
      {-16.822165, 32.76765, 2},
      {-16.8869670108, 32.7348750076, 2},
      {-16.8674213923, 32.6612734688, 2},
      {-16.8872647285, 32.7370721352, 2},
      {-16.8870930672, 32.7371804312, 2},
      {-16.8815852635, 32.7212618236, 2},
      {-16.8956248462, 32.6710605161, 2},
      {-16.8279413134, 32.7717033757, 2},
      {-16.8245437804, 32.76748653, 2},
      {-16.8252304259, 32.7702732936, 2},
      {-16.7648540326, 32.7173745973, 2},
      {-16.8972797449, 32.6616669747, 2},
      {-16.9240554, 32.6669328, 2},
      {-16.8948260756, 32.6626649831, 2},
      {-16.8396725217, 32.6422481855, 2},
      {-17.0042461, 32.6554979, 2},
      {-16.867080338, 32.6576958031, 2},
      {-16.8947544694, 32.6624060808, 2},
      {-17.1552581786, 32.8539428708, 2},
      {-16.8685422465, 32.6627950305, 2},
      {-16.9030180556, 32.67333, 2},
      {-16.9071298465, 32.6581729684, 2},
      {-17.1469833333, 32.8146055556, 2},
      {-17.1469333333, 32.8146083333, 2},
      {-17.1489916138, 32.8135995787, 2},
      {-17.1489687082, 32.8137758385, 2},
      {-17.1493109264, 32.8137889338, 2},
      {-17.185518997, 32.7362925262, 2},
      {-17.185647743, 32.736495583, 2},
      {-17.1853902509, 32.7366554848, 2},
      {-17.1853902509, 32.7367998803, 2},
      {-17.1854331663, 32.7368720779, 2},
      {-17.1856048276, 32.7368359791, 2},
      {-17.1854760816, 32.7368720779, 2},
      {-16.9529071432, 32.8240472173, 2},
      {-17.1021707, 32.6953195, 2},
      {-17.2491045071, 32.8095199868, 2},
      {-17.2496016696, 32.8089583642, 2},
      {-16.9385188274, 32.6710511415, 2},
      {-17.1303893875, 32.6997588475, 2},
      {-17.1348087, 32.7617326, 2},
      {-16.77211067, 32.74243569, 2},
      {-16.782793, 32.7450751, 2},
      {-16.8961290074, 32.6623773185, 2},
      {-16.8657663041, 32.6580250513, 2},
      {-16.7905902773, 32.7516708148, 2},
      {-16.8951156721, 32.6625349834, 2},
      {-16.9604437798, 32.6655150905, 2},
      {-17.0658205344, 32.6706293719, 2},
      {-17.0205735415, 32.6640829555, 2},
      {-16.7719600639, 32.7423703517, 2},
      {-16.8950175488, 32.6629229674, 2},
      {-16.911358349, 32.6538335167, 2},
      {-17.1406611111, 32.7778444444, 2},
      {-17.0620736852, 32.7548832239, 2},
      {-16.8667177634, 32.6588097722, 2},
      {-16.8681638256, 32.6587532697, 2},
      {-16.9086305556, 32.6480083333, 2},
      {-17.1468053714, 32.8144641173, 2},
      {-16.765105, 32.717445, 2},
      {-16.9602781534, 32.6657674193, 2},
      {-17.2403616667, 32.82287, 2},
      {-16.9086388889, 32.8234083333, 2},
      {-17.1680045128, 32.7185203859, 2},
      {-16.9082357, 32.6506129, 2},
      {-16.89578, 32.662045, 2},
      {-17.1439803019, 32.8096538279, 2},
      {-17.1584174659, 32.8258865633, 2},
      {-16.8239967898, 32.7255957977, 2},
      {-16.914370805, 32.6506885317, 2},
      {-16.9027591869, 32.6733733604, 2},
      {-16.8668704768, 32.6601606957, 2},
      {-17.1491227672, 32.8137067399, 2},
      {-17.1491227672, 32.8137067399, 2},
      {-16.8961696, 32.6623424, 2},
      {-16.7882971997, 32.7543344997, 2},
      {-17.1469400844, 32.8144829613, 2},
      {-16.9594724849, 32.7564622131, 2},
      {-17.1469444444, 32.8144444444, 2},
      {-17.1466666667, 32.8144444444, 2},
      {-16.896342, 32.6625830001, 2},
      {-16.896342, 32.6625830001, 2},
      {-16.7914472222, 32.7379777778, 2},
      {-16.791555, 32.738546, 2},
      {-17.1468955278, 32.8145334741, 2},
      {-16.9594723, 32.7607074, 2},
      {-16.7646329667, 32.7175322972, 2},
      {-16.7645835972, 32.7174558528, 2},
      {-16.7620909219, 32.7379475259, 2},
      {-16.7620909219, 32.7379475259, 2},
      {-16.7620909219, 32.7379475259, 2},
      {-16.7620909219, 32.7379475259, 2},
      {-16.8942841669, 32.6635018928, 2},
      {-16.8942841669, 32.6635018928, 2},
      {-17.1470129583, 32.8146028667, 2},
      {-16.895512, 32.6622784, 2},
      {-17.0838583333, 32.719805, 2},
      {-16.9116333333, 32.6538388889, 2},
      {-16.85174, 32.793109, 2},
      {-16.9115472222, 32.6535972222, 2},
      {-16.8947544, 32.6624061, 2},
      {-16.823916994, 32.7257890069, 2},
      {-16.8876868124, 32.7742035795, 2},
      {-17.1490059, 32.8151819, 2},
      {-16.8954008821, 32.6621593879, 2},
      {-16.7705640525, 32.7231043958, 2},
      {-16.8685422, 32.6627915, 2},
      {-16.7497004295, 32.7422247315, 2},
      {-17.1222391513, 32.7599347397, 2},
      {-16.8963044659, 32.6626520629, 2},
      {-16.8942308769, 32.66308474, 2},
      {-16.8889538868, 32.665806833, 2},
      {-16.896171011, 32.6624455968, 2},
      {-16.8947544694, 32.6624060808, 2},
      {-16.9027385304, 32.6642595313, 2},
      {-16.86613, 32.65767, 2},
      {-16.8948423966, 32.6624787556, 2},
      {-16.907305, 32.6594083333, 2},
      {-16.7620084855, 32.7307391245, 2},
      {-16.8947544, 32.6624061, 2},
      {-16.9029072632, 32.673228474, 2},
      {-16.8947544, 32.6624061, 2},
      {-16.9441773182, 32.6344469688, 2},
      {-16.8279235438, 32.6675859159, 2},
      {-16.8961709027, 32.6624433461, 2},
      {-16.8658236169, 32.658049562, 2},
      {-17.1090155849, 32.7426748715, 2},
      {-16.8962329, 32.6624976, 2},
      {-17.1108258655, 32.7423521343, 2},
      {-17.1470423227, 32.8145201775, 2},
      {-16.8965002409, 32.6614612011, 2},
      {-16.8929009629, 32.6774184284, 2},
      {-17.1469197933, 32.8146219317, 2},
      {-16.8660775561, 32.6577211058, 2},
      {-16.9115353, 32.653739, 2},
      {-16.8666082046, 32.6577722117, 2},
      {-17.1466833333, 32.8144716667, 2},
      {-16.951475814, 32.7463787583, 2},
      {-17.146781534, 32.8144506319, 2},
      {-16.8960094342, 32.6620032186, 2},
      {-16.895928202, 32.6619708802, 2},
      {-16.99471075, 32.68048711, 2},
      {-17.1011056937, 32.6823137189, 2},
      {-16.9246999192, 32.6449358543, 2},
      {-16.8658366667, 32.6579883333, 2},
      {-17.1468071891, 32.8145265005, 2},
      {-16.7683888889, 32.7236361111, 2},
      {-16.7974703034, 32.7372762125, 2},
      {-16.9013366667, 32.6748083333, 2},
      {-16.9594723, 32.7607074, 2},
      {-16.9044871834, 32.6645244165, 2},
      {-16.9043237, 32.6646466, 2},
      {-16.8975629619, 32.8291557641, 2},
      {-16.881891, 32.805135, 2},
      {-16.897590925, 32.736189509, 2},
      {-16.8947544, 32.6624061, 2},
      {-17.0208353922, 32.733740119, 2},
      {-17.1143750846, 32.7527591575, 2},
      {-17.1070976, 32.8248109, 2},
      {-16.894795, 32.6626083333, 2},
      {-17.1467, 32.8143416667, 2},
      {-16.8482805043, 32.7377781583, 2},
      {-16.9093, 32.699136, 2},
      {-17.146915375, 32.8143528063, 2},
      {-16.895785, 32.6615116667, 2},
      {-16.8957111111, 32.6616277778, 2},
      {-16.8948064372, 32.6626352734, 2},
      {-17.1467838287, 32.8143386841, 2},
      {-17.147757858, 32.8134706092, 2},
      {-17.148428075, 32.8133618424, 2},
      {-16.8487304449, 32.7832322486, 2},
      {-16.9989197086, 32.6030881702, 2},
      {-17.1692225312, 32.7202714834, 2},
      {-17.144227536, 32.8109199087, 2},
      {-16.8687862439, 32.6628946162, 2},
      {-16.8687862439, 32.6628946162, 2},
      {-16.8686257989, 32.6627568853, 2},
      {-16.8684970529, 32.6627839819, 2},
      {-17.1368248388, 32.7367538595, 2},
      {-17.2164277732, 32.8111056049, 2},
      {-16.895725019, 32.6615468737, 2},
      {-16.910702339, 32.6890981253, 2},
      {-16.8201776774, 32.7661642712, 2},
      {-16.8957388406, 32.6616106756, 2},
      {-16.8970868795, 32.6628024021, 2},
      {-16.7646022886, 32.7175252024, 2},
      {-16.7647410929, 32.7176027752, 2},
      {-16.9115576159, 32.6540218234, 2},
      {-16.968740799999978, 32.70615040000007, 1},
      {-16.963998299999957, 32.69269020000007, 1},
      {-17.001930847999972, 32.72761107600007, 1},
      {-16.961548999999934, 32.724162300000046, 1},
      {-16.978629599999977, 32.702363200000036, 1},
      {-16.916793499999926, 32.74709750000005, 1},
      {-16.98374345899998, 32.776117156000055, 1},
      {-16.919448771999953, 32.74365898600007, 1},
      {-17.03813194999998, 32.79453077000005, 1},
      {-17.052444219999927, 32.682933700000035, 1},
      {-17.08972055499993, 32.70122551800006, 1},
      {-16.86590327099998, 32.659438581000074, 1},
      {-16.812051906999955, 32.66389977400007, 1},
      {-16.812872997999932, 32.66380550100007, 1},
      {-16.902271799999937, 32.78236330000004, 1},
      {-16.88903734099995, 32.68015858100006, 1},
      {-16.902497000999972, 32.78227527800004, 1},
      {-16.933099055999946, 32.69947672200004, 1},
      {-16.892232913999976, 32.74191319800008, 1},
      {-16.88623247199996, 32.74054897700006, 1},
      {-17.04547625999993, 32.78617910500003, 1},
      {-16.95633608299994, 32.70205019400004, 1},
      {-16.92019258099998, 32.783242302000076, 1},
      {-16.918490007999935, 32.67858078000006, 1},
      {-16.906005999999934, 32.78361200000006, 1},
      {-16.974878599999954, 32.75855270000005, 1},
      {-16.824119442999972, 32.72294658200008, 1},
      {-16.95955799999996, 32.66068670000004, 1},
      {-17.234056926999926, 32.768663471000025, 1},
      {-16.90380367499995, 32.67542865000007, 1},
      {-17.111901914999976, 32.81060010300007, 1},
      {-16.916724778999935, 32.69543338900007, 1},
      {-16.96027580699996, 32.67170484500008, 1},
      {-16.915431953999928, 32.68516327900005, 1},
      {-16.980102399999964, 32.80470720000005, 1},
      {-16.866146388999937, 32.65782305600004, 1},
      {-16.86690445199997, 32.66017712400003, 1},
      {-16.908638888999974, 32.82340833300003, 1},
      {-16.959472484999935, 32.75646221300008, 1},
      {-16.959472484999935, 32.75646221300008, 1},
      {-16.87257165099993, 32.737086212000065, 1},
      {-16.88793897599993, 32.747380951000025, 1},
      {-16.867157220999957, 32.659311923000075, 1},
      {-16.868542246999937, 32.662791361000075, 1},
      {-16.88568022399994, 32.73968400700005, 1},
      {-17.12269927699998, 32.759941629000025, 1},
      {-16.88635919999996, 32.734931000000074, 1},
      {-16.866641999999956, 32.65923900000007, 1},
      {-16.906167899999957, 32.78365110000004, 1},
      {-16.86863746499995, 32.66499293100003, 1},
      {-16.901179700999933, 32.67531418400006, 1},
      {-16.851707828999963, 32.786744707000025, 1},
      {-16.90262202699995, 32.67359014800007, 1},
      {-16.90262202699995, 32.67359014800007, 1},
      {-16.92158055599998, 32.779594444000054, 1},
      {-16.93819521299997, 32.77482528300004, 1},
      {-16.802427243999944, 32.69354086800007, 1},
      {-16.88755353399995, 32.72636720100007, 1},
      {-16.887457955999935, 32.736839031000045, 1},
      {-16.866788888999963, 32.65900555600007, 1},
      {-16.870549299999936, 32.73435140000004, 1},
      {-16.870311110999978, 32.73411666700002, 1},
      {-16.847953231999952, 32.749691519000066, 1},
      {-16.873550999999964, 32.747610000000066, 1},
      {-16.928561677999937, 32.814184039000054, 1},
      {-16.878813844999968, 32.74293977200006, 1},
      {-16.867319443999975, 32.65887500000002, 1},
      {-16.90177993499998, 32.67674466300008, 1},
      {-16.89301033299995, 32.743532609000056, 1},
      {-16.89176089199998, 32.744969437000066, 1},
      {-16.890469646999975, 32.74163539600005, 1},
      {-16.93825417599993, 32.77279912700004, 1},
      {-16.917159882999954, 32.78442457800003, 1},
      {-16.91667297899994, 32.78111469000004, 1},
      {-16.91180133399996, 32.78371168100006, 1},
      {-16.886976398999934, 32.698893645000055, 1},
      {-16.886656544999937, 32.737145869000074, 1},
      {-17.071405892999962, 32.799142525000036, 1},
      {-16.882742699999937, 32.74392540000002, 1},
      {-16.890112235999936, 32.74102554800004, 1},
      {-16.883332999999936, 32.73333300000007, 1},
      {-16.906849042999966, 32.74283998100003, 1},
      {-16.868406446999927, 32.662027598000066, 1},
      {-16.881328332999942, 32.75287000000003, 1},
      {-16.937254999999936, 32.776538333000076, 1},
      {-17.023124999999936, 32.75271297200004, 1},
      {-16.870199999999954, 32.64590833300008, 1},
      {-16.909219250999968, 32.78656291200008, 1},
      {-16.868542246999937, 32.66279503000004, 1},
      {-17.04701375199994, 32.80410828400005, 1},
      {-16.902437065999948, 32.674172948000034, 1},
      {-17.12346618899994, 32.760169475000055, 1},
      {-16.905813888999944, 32.785000000000025, 1},
      {-17.133838888999946, 32.76177222200005, 1},
      {-16.917878332999976, 32.78252833300007, 1},
      {-16.90550499999995, 32.78502000000003, 1},
      {-16.84926286299998, 32.783678174000045, 1},
      {-16.882490031999964, 32.70551385300007, 1},
      {-16.885049004999928, 32.73545244600007, 1},
      {-17.179012487999955, 32.83455382000005, 1},
      {-17.209730600999933, 32.85289540600007, 1},
      {-16.901046260999976, 32.675622082000075, 1},
      {-16.887911110999937, 32.73833055600005, 1},
      {-17.131252185999926, 32.83014619700003, 1},
      {-16.873192595999967, 32.720627394000076, 1},
      {-17.056233605999978, 32.77362948600006, 1},
      {-17.020930299999975, 32.75410530000005, 1},
      {-17.135900375999938, 32.764356217000056, 1},
      {-17.17414474499998, 32.73397827100007, 1},
      {-17.137146348999977, 32.769928562000075, 1},
      {-17.13413703799995, 32.76520235800007, 1},
      {-17.13042039399994, 32.75811984000006, 1},
      {-17.103854799999965, 32.820576800000026, 1},
      {-17.09530803399997, 32.81608960900007, 1},
      {-16.886799999999937, 32.733600000000024, 1},
      {-16.931906499999968, 32.63634300000007, 1},
      {-16.902419149999957, 32.67416384100005, 1},
      {-16.908638888999974, 32.82340833300003, 1},
      {-16.873130164999964, 32.646830333000025, 1}
  };
  numCoordinates = sizeof(coordinates) / sizeof(coordinates[0]);
  return coordinates;
}



/*************************************
  Function: TesteBegin
  Description: Performs hardware tests.
*************************************/
void TesteBegin() {
  Serial.println("Starting Test...");
  //for (int i = 0; i < 16; i++) stepper.step(stepsPerRevolution);
  //for (int j = 0; j < 16; j++) stepper.step(-stepsPerRevolution);
  for(int i = 0; i < 3; i++){
    stepper.step(stepsPerRevolution);
    stepper.step(-stepsPerRevolution);
  }
  for(int j = 0; j < 3; j++){
    stepper2.step(stepsPerRevolution);
    stepper2.step(-stepsPerRevolution);
  }
  DF1201S.playFileNum(3);
  delay(2000);
  DF1201S.pause();
  Serial.println("Test Completed!");
  DF1201S.next();
  DF1201S.pause();

  for (int i = 0; i < 3; i++) {
    strip.setPixelColor(0, strip.Color(255, 255, 255));  
    strip.show();
    delay(500);
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
    delay(500);
  }
}
