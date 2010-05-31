#include <OneWire.h>

#include <DallasTemperature.h>

#include <LiquidCrystal.h>

#include <PID_Beta6.h>

// Constants
const int oneWirePin = 6;
const int relayPin = 9;

// Variables
double temperature;
double setPoint = 225;
double pidOutput = 0;
int pValue = 50;
int iValue = 600;
int dValue = 10;
byte oneWireData[12];
byte oneWireAddr[8];
DeviceAddress insideThermometer;


// Temperature sensor
OneWire oneWire(oneWirePin);
DallasTemperature tempSensor(&oneWire);

// PID Controller
PID controller(&temperature, &pidOutput, &setPoint, pValue, iValue, dValue);

// LCD Display
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
  
void setup(void) {
  // Pins
  pinMode( 6, INPUT );
  pinMode( 9, OUTPUT );
  
  // Initialize sensors
  setup_sensors();
  
  // Initialize controller with a safe target value
  setup_controllers();
  
  // Initialize Display
  lcd.begin( 16, 2);
  lcd.print("initialized...");
}

void loop(void) {
  temperature = get_temp();
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
  
  Serial.print( "Target: " );
  Serial.print( setPoint );
  Serial.print( " Current: " );
  Serial.print( temperature );
  Serial.print( " Power: " );
  Serial.print( 100 * pidOutput / 255 );
  Serial.println( "%" );

  analogWrite( relayPin, pidOutput );
}

void setup_sensors(void) {
  // start serial port
  Serial.begin(9600);
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

void setup_controllers(void) {
  temperature = get_temp();
  controller.SetMode(AUTO);
  Serial.println('PID Controller initialized.');
}
  
float get_temp( void ) {
  //Serial.print('Requesting Temperature... ');
  tempSensor.requestTemperatures();
  //Serial.println("Done");

  float tempC = tempSensor.getTempC( insideThermometer );
  return DallasTemperature::toFahrenheit(tempC); 
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
