#include <Adafruit_NeoPixel.h>

#define STRIPLENGTH 36
#define BEATCHECK 4

#define STATELEDLOOP2

// FLEXSENSOR
int flex = 1;
int flexVal = 0;

// BUZZER
int buzzer = 8;
boolean isHighBeat = false;

// NEOPIXEL
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIPLENGTH,5,NEO_GRB + NEO_KHZ800);
int neoPixelVccPin = 12;
int beatStateCount = 0;
int nonBeatCount = 0;

//  VARIABLES
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 13;                // pin to blink led at each beat
int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin


// these variables are volatile because they are used during the interrupt service routine!
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.


void InterrupuSetup();

void setup(){
  pinMode(buzzer,OUTPUT);

  pinMode(neoPixelVccPin,OUTPUT);
  digitalWrite(neoPixelVccPin,HIGH);

  pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
  Serial.begin(115200);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
  // UN-COMMENT THE NEXT LINE IF YOU ARE POWERING The Pulse Sensor AT LOW VOLTAGE, 
  // AND APPLY THAT VOLTAGE TO THE A-REF PIN
  //analogReference(EXTERNAL);   

  strip.begin();
  strip.setBrightness(25);
  strip.show();
}



void loop(){
  flexVal = analogRead(flex);
  //Serial.println(flexVal);  
  for(int i = 0; i < STRIPLENGTH; i++)if(i < 10 || i > 25)strip.setPixelColor(i,strip.Color(0,255,0));
  
  if(beatStateCount >= BEATCHECK){
    for(int i = 10; i < 26; i++){
      strip.setPixelColor(i,strip.Color(255,0,0));
    }
    isHighBeat = true;
  }
  
  else if(beatStateCount <= -1* BEATCHECK){
    for(int i = 10; i < 26; i++){
      strip.setPixelColor(i,strip.Color(0,0,255));
    }
    //isHighBeat = false;
  }



  //sendDataToProcessing('S', Signal);     // send Processing the raw Pulse Sensor data

  if (QS == true){                       // Quantified Self flag is true when arduino finds a heartbeat
    fadeRate = 255;                  // Set 'fadeRate' Variable to 255 to fade LED with pulse
    //sendDataToProcessing('B',BPM);   // send heart rate with a 'B' prefix
    //sendDataToProcessing('Q',IBI);   // send time between beats with a 'Q' prefix


    if(BPM >= 95){
      for(int i = 0; i < STRIPLENGTH; i++){
        if(i < 10 || i > 25) strip.setPixelColor(i,strip.Color(255,0,0));
      }
      if(beatStateCount < 0)beatStateCount = 0;
      beatStateCount++;
    }
    else {
      for(int i = 0; i < STRIPLENGTH; i++){
        if(i < 10 || i > 25)strip.setPixelColor(i,strip.Color(0,0,255));
      }
      if(beatStateCount > 0)beatStateCount = 0;
      beatStateCount--;
    }
    nonBeatCount = 0;

    QS = false;                      // reset the Quantified Self flag for next time    
  }
  else{
    nonBeatCount++;
    if(beatStateCount < BEATCHECK && beatStateCount > -1 * BEATCHECK || nonBeatCount > 100){
      if(nonBeatCount > 50)beatStateCount = 0;
      for(int i = 10; i < 26; i++){
        strip.setPixelColor(i,strip.Color(0,255,0));
      }
      isHighBeat = false;
    }
  }


  strip.show();
  ledFadeToBeat();
  if(flexVal < 300 && isHighBeat){
    tone(buzzer,523);
    //digitalWrite(buzzer,HIGH);
  }
  else{
    noTone(buzzer);
    //digitalWrite(buzzer,LOW);
  }

  delay(20);                             //  take a break

}


void ledFadeToBeat(){
  fadeRate -= 15;                         //  set LED fade value
  fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
  analogWrite(fadePin,fadeRate);          //  fade LED
}


void sendDataToProcessing(char symbol, int data ){
  Serial.print(symbol);                // symbol prefix tells Processing what type of data is coming
  Serial.println(data);                // the data to send culminating in a carriage return
}












