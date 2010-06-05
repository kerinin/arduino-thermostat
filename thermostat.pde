#include <spline.h>
#include <buttons.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <PID_Beta6.h>
#include <EEPROM.h>

// Setup
//--------------------------------------------------------------------------

struct config_t
{
    int tMode;
    int hwProfile;
    double constantSetPoint;
} configuration;

void setup(void) {
  Serial.begin(9600);
  
  EEPROM_readAnything(0, configuration);
  
  // Setup Sensors
  setup_sensors();
  
  // Setup Control
  setup_controllers();
  
  // Setup UI
  setup_ui();
}

// Sensors Constants
#define oneWirePin 11

// Sensor Variables
//byte oneWireData[12];
//byte oneWireAddr[8];
DeviceAddress insideThermometer;
double temperature;
OneWire oneWire(oneWirePin);
DallasTemperature tempSensor(&oneWire);

void setup_sensors(void) {
  pinMode( oneWirePin, INPUT );
  
  // start serial port
  Serial.println("Dallas Temperature IC Control Library Demo");

  // locate devices on the bus
  Serial.print("Locating devices...");
  tempSensor.begin();
  Serial.print("Found ");
  Serial.print(tempSensor.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (tempSensor.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Method 1:
  // search for devices on the bus and assign based on an index.  ideally,
  // you would do this to initially discover addresses on the bus and then 
  // use those addresses and manually assign them (see above) once you know 
  // the devices on your bus (and assuming they don't change).
  if (!tempSensor.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  tempSensor.setResolution(insideThermometer, 12);
 
  Serial.print("Device 0 Resolution: ");
  Serial.print(tempSensor.getResolution(insideThermometer), DEC); 
  Serial.println();
}


// Control Constants
#define relayPin 10

// Control Variables
double time0 = 0;
Spline tempCurve;
double pidOutput = 0;
double setPoint = 70;
int pValue = 50;
int iValue = 600;
int dValue = 10;
PID controller(&temperature, &pidOutput, &setPoint, pValue, iValue, dValue);
double timeArrayRamp[2] = {0,5};
double tempArrayRamp[2] = {60, 225};
double slopesArrayRamp[2] = {0, 0};

void setup_controllers(void) {
  pinMode( relayPin, OUTPUT );

  temperature = get_temp();
  reset_spline();
  tempCurve.setDegree(Hermite);
  
  Serial.print("Target Temp ramping from ");
  Serial.print( tempArrayRamp[0] );
  Serial.print(" to ");
  Serial.print( tempArrayRamp[1] ); 
  Serial.print(" over ");
  Serial.print( timeArrayRamp[1]/60000 );
  Serial.println(" minutes");
 
  for( float i = 0; i <= 300000; i+= 30000 ) {
    float temp = tempCurve.value(i);
    for(float j=0; j<= temp; j += 5) {
      Serial.print( "*" );
    }
    Serial.print( "   " );
    Serial.print(temp);
    Serial.print( " @" );
    Serial.println( i/60000 );
  }
  
  controller.SetMode(AUTO);
  Serial.println("PID Controller initialized.");
}


// UI Constants
#define lcdRsPin 12
#define lcdEnablePin 13
#define lcdD4Pin 4
#define lcdD5Pin 5
#define lcdD6Pin 6
#define lcdD7Pin 7

#define modeButtonPin 2
#define selectButtonPin 3

#define modActive 0
#define modTargetMode 1
#define modSetTarget 2
#define modHardwareProfile 3
#define modTune 4
#define modCount 5

#define tModeConstant 0
#define tModeSmokedPork 1
#define tModeSmokedBrisket 2
#define tModeCount 3

#define hwSmoker 0
#define hwSousVide 1
#define hwCount 2

// UI variables
LiquidCrystal lcd(lcdRsPin, lcdEnablePin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin);
Button bMode;
Button bSelect;
int mode = modActive;
int setupMode = true;

void setup_ui(void) {
  bMode.assign( modeButtonPin );
  bMode.setMode( Memory );
  bSelect.assign( selectButtonPin );
  bSelect.setMode( MemoryTimer );
  bSelect.setTimer( 1500 );
  bSelect.setRefresh( 200 );

  lcd.begin( 16, 2);
  lcd.print("initialized...");
}
  

// Loop
//--------------------------------------------------------------------------


void loop(void) {
  switch( bMode.check() ) {
    case ON:
      lcd.clear();
      break;
    case Pressed:
      break;
    case Released:
      next_mode();
      break;
    case OFF:
      switch( mode ) {
        case modActive:
          active_loop(); break;
        case modTargetMode:
          target_mode_loop(); break;
        case modSetTarget:
          set_target_loop(); break;
        case modHardwareProfile:
          hardware_profile_loop(); break;
        case modTune:
          tune_loop(); break;
      }
  }
}

void active_loop(void) {
  if( setupMode == true ) {
    lcd.clear();
    lcd.print("Active");
    lcd.setCursor(0,1);
    lcd.print("Getting Temp...");  
    
    setupMode = false;
  }
  
  temperature = get_temp();
  setPoint = tempCurve.value( static_cast<double>(millis()-time0)/60000 );
  controller.Compute();
  
  lcd.clear();
  lcd.print("T ");
  lcd.print(setPoint);
  lcd.print(" C ");
  lcd.print(temperature);
  lcd.setCursor(0,1);
  lcd.print("Pow: ");
  lcd.print( 100 * pidOutput / 255 );
  lcd.print( "%" );
  
  analogWrite( relayPin, pidOutput );
}

double timeArrayPork[5] =   {0,  5,   10,  360, 540};
double tempArrayPork[5] =   {60, 220, 228, 225, 215};
double slopesArrayPork[5] = {0,  0,   0,   0,   0};

double timeArrayBrisket[6] =   {0,  5,   10,  360, 540, 720};
double tempArrayBrisket[6] =   {60, 220, 225, 220, 210, 200};
double slopesArrayBrisket[6] = {0,  0,   0,   0,   0,   0};

void reset_spline( void ) {
  time0 = millis();
  float delta;
  switch( configuration.tMode ) {
    case tModeConstant:
      delta = temperature-configuration.constantSetPoint;
      ramp_to( configuration.constantSetPoint, abs(delta) * .033 );       // 2sec/degree ramp
    case tModeSmokedPork:
      tempArrayPork[0] = temperature;
      tempCurve.setPoints( timeArrayPork, tempArrayPork, slopesArrayPork, 5 );
    case tModeSmokedBrisket:
      tempArrayBrisket[0] = temperature;
      tempCurve.setPoints( timeArrayBrisket, tempArrayBrisket, slopesArrayBrisket, 6 );
  }
}

void target_mode_loop( void ) {
  if( setupMode == true ) {
    lcd.clear();
    lcd.print( "Target Mode" );
    lcd.setCursor( 0, 1 );
    
    float delta;
    
    switch( configuration.tMode ) {
      case tModeConstant:
        lcd.print( "=> Constant Temp" ); break;
      case tModeSmokedPork:
        lcd.print( "=> Pulled Pork" ); break;
      case tModeSmokedBrisket:
        lcd.print( "=> Brisket"); break;
    }
    
    setupMode = false;
  }
  
  switch( bSelect.check() ) {
    case Pressed:
      break;
    case ON:
      break;
    case Hold:
      break;
    case Released:
      setupMode = true; 
      configuration.tMode = (configuration.tMode + 1) % tModeCount;
      EEPROM_writeAnything(0, configuration);
      
      reset_spline();
      break;   
    case OFF:
      break;
  }
}

void set_target_loop(void) {
  if( setupMode == true ) {
    lcd.clear();
    lcd.print("Set Target");
    lcd.setCursor( 0, 1 );
      
    if( configuration.tMode == tModeConstant ) {
      lcd.print( "=> " );
      lcd.print( configuration.constantSetPoint );
      lcd.print( "F" );
    } else {
      lcd.print( "(Cnst Mode Only)" );
    }
    
    setupMode = false;
  }
  
  if( configuration.tMode == tModeConstant ) {
    float delta;
    
    switch( bSelect.check() ) {
      case Pressed:
        break;
      case ON:
        break;
      case Hold:
        setupMode = true; 
        configuration.constantSetPoint = configuration.constantSetPoint <= 32 ? 250 : configuration.constantSetPoint - 1.0;
        delta = temperature-configuration.constantSetPoint;
        ramp_to( configuration.constantSetPoint, abs(delta) * .033 );   // 2sec/degree ramp
        break;
      case Released:
        setupMode = true; 
        
        configuration.constantSetPoint = configuration.constantSetPoint <= 32 ? 250 : configuration.constantSetPoint - 1.0;
        EEPROM_writeAnything(0, configuration);
        
        delta = temperature-configuration.constantSetPoint;
        ramp_to( configuration.constantSetPoint, abs(delta) * .033 );   // 2sec/degree ramp
        break;   
      case OFF:
        break;
    }
  }
}

void hardware_profile_loop(void) {
  if( setupMode == true ) {
    lcd.clear();
    lcd.print("Hardware Profile");
    lcd.setCursor( 0, 1 );
    switch( configuration.hwProfile ) {
      case hwSmoker:
        lcd.print( "=> Smoker" );
        controller.SetTunings( 50, 600, 10 );
        break;
      case hwSousVide:
        lcd.print( "=> Sous Vide" ); 
        controller.SetTunings( 50, 600, 10 );  // ???
        break;
    }
    
    setupMode = false;
  }
  
  switch( bSelect.check() ) {
    case Pressed:
      break;
    case ON:
      break;
    case Hold:
      break;
    case Released:
      setupMode = true; 
      configuration.hwProfile = (configuration.hwProfile + 1) % hwCount;
      EEPROM_writeAnything(0, configuration);
      break;   
    case OFF:
      break;
  }
}

void tune_loop(void) {
  if( setupMode == true ) {
    lcd.clear();
    lcd.print("Auto-Tune");
    lcd.setCursor( 0, 1 );
    lcd.print( "NOT IMPLEMENTED!" );
    
    setupMode = false;
  }
}

// Utility Functions
//--------------------------------------------------------------------------

void ramp_to( float target, int duration ) {
  time0 = millis();
  timeArrayRamp[0] = 0;
  timeArrayRamp[1] = duration;
  
  tempArrayRamp[0] = temperature;
  tempArrayRamp[1] = target;

  tempCurve.setPoints( timeArrayRamp, tempArrayRamp, slopesArrayRamp, 2 );
}

void next_mode( void ) {
  mode = (mode + 1) % 5;
  setupMode = true;
  
  // Until the PID loop can operate concurrently,
  // stop processing until out of menu  
  analogWrite( relayPin, 0 );
}

float get_temp( void ) {
  //Serial.print('Requesting Temperature... ');
  tempSensor.requestTemperatures();
  //Serial.println("Done");

  float tempC = tempSensor.getTempC( insideThermometer );
  return DallasTemperature::toFahrenheit(tempC); 
}

void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

int EEPROM_writeAnything(int ee, const struct config_t& value)
{
    const byte* p = (const byte*)(const void*)&value;
    int i;
    for (i = 0; i < sizeof(value); i++)
	  EEPROM.write(ee++, *p++);
    return i;
}

int EEPROM_readAnything(int ee, struct config_t& value)
{
    byte* p = (byte*)(void*)&value;
    int i;
    for (i = 0; i < sizeof(value); i++)
	  *p++ = EEPROM.read(ee++);
    return i;
}
