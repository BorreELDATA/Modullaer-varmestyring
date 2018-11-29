/**
 *  This program is developed and written by Børre A. Opedal Lunde.
 *  Creation date: 26.11.2018
 *  Last updated:  29.11.2018
 */

/**
 *  Enumeration LedState is used to indicate whether
 *  a light emitting diode is turned on or off.
 */
enum LedState {
  ON, OFF
};

/**
 *  Memberfield of variables used in the program.
 */
const byte temperatureSensorPin = A0;                   // pin number of the temperature sensor connected to the arduino board.
const byte photoresistorPin = A1;                       // pin number of the photo resistor connected to the arduino board.
const byte greenLedPin = 12;                            // pin number of the green light emitting diode connected to the arduino board.
const byte redLedPin = 11;                              // pin number of the red light emitting diode connected to the arduino board.
const byte blueLedPin = 10;                             // pin number of the blue light emitting diode conncted to the arduino board.
const byte heatingElementTemperatureTriggerLevel = 15;  // temperature level needed for the heating element to activate
const byte airConditionTemperatureTriggerLevel = 30;    // temperature level needed for the air condition to activate
LedState greenLedState = OFF;                           // state of the green light emitting diode
LedState redLedState = OFF;                             // state of the red light emitting diode
LedState blueLedState = OFF;                            // state of the blue light emitting diode
unsigned long previousTime;                             // number used to hold the previous time of the repeater that
                                                        // limits how often information is outputed to the serial monitor.
unsigned long previousTimeTwo;                          // number used to hold teh previous time of the repeated that
                                                        // limits how often the sensor values are updated.
unsigned long currentTime;                              // number that holds the time since the program started in
                                                        // milliseconds. Used in the information output repeater.
float voltage;                // voltage level [0, 5] over the temperature sensor.
float temperature;            // temperature measured by the temperature sensor.
int lightLevel;               // analog signal [0, 1023] indicating the light level from the photoresistor.

/**
 *  Sets the LedState of the green LED.
 *  
 *  @param state  new led state
 */
void setGreenLedState(LedState state) {
  greenLedState = state;
}

/**
 *  Sets the LedState of the red LED.
 *  
 *  @param state  new led state
 */
void setRedLedState(LedState state) {
  redLedState = state;
}

/**
 *  Sets the LedState of the blue LED.
 *  
 *  @param state  new led state
 */
void setBlueLedState(LedState state) {
  blueLedState = state;
}

/**
 *  Returns the led state as an integer number that is
 *  either high or low. Used to set the digital value
 *  of the light emitting diodes.
 *  @param state  the led state to be converted to integer
 */
int convertLedStateToInteger(LedState state) {
  return state == ON ? HIGH : LOW;
}

/**
 *  Returns the led state as string. This is used for
 *  writing the led state to the serial monitor.
 *  
 *  @param state  the led state to be converted to text
 */
String convertLedStateToString(LedState state) {
  return state == ON ? "on" : "off";
}

/**
 *  Takes the sensor value given as argument in the parameter
 *  and returns the voltage converted from said value.
 *  As the sensor value is a number between 0 and 1023, dividing
 *  it by 1024 returns a number between 0 and 1. This number
 *  is multiplied by 5 to illustrate the voltage number. Thus
 *  the voltage number is between 0 and 5.
 *  F. e. (512 / 1024) * 5 = 0.5 * 5 = 2.5 V.
 *  
 *  @param sensorValue  the sensor value to be converted to voltage 
 */
float convertVoltageFromSensorValue(int sensorValue) {
  return (sensorValue / 1024.0) * 5.0;
}

/**
 *  Takes the voltage given as argument in the parameter
 *  and returns the temperature converted from said value.
 *  The temperature number is roughly 100 times as big
 *  as the voltage number.
 *
 * @param value  the voltage number to be converted to temperature 
 */
float convertTemperatureFromVoltage(float value) {
  return (value - 0.5) * 100;
}

/**
 *  Updates all the sensor values by reading from the sensors.
 */
void updateSensorValues() {
  const int temperatureSensorValue = analogRead(temperatureSensorPin);
  voltage = convertVoltageFromSensorValue(temperatureSensorValue);
  temperature = convertTemperatureFromVoltage(voltage);
  lightLevel = analogRead(photoresistorPin);
}

/**
   Decides the light emittion diode states depending on the temperature.
*/
void decideLedStates() {
  // keeps the green light emitting diode on at all times because measurement is always present.
  setGreenLedState(ON);
  // turns the red light emitting diode on if the temperature is colder than 15 degrees.
  if (temperature < heatingElementTemperatureTriggerLevel) setRedLedState(ON);
  else setRedLedState(OFF);
  // turns the blue light emitting diode on if the temperature is warmer than 30 degrees.
  if (temperature > airConditionTemperatureTriggerLevel) setBlueLedState(ON);
  else setBlueLedState(OFF);
}

/**
 *  Writes the light emitting diodes on or off depending on their state.
 */
void updateLeds() {
  digitalWrite(greenLedPin, convertLedStateToInteger(greenLedState));
  digitalWrite(redLedPin, convertLedStateToInteger(redLedState));
  digitalWrite(blueLedPin, convertLedStateToInteger(blueLedState));
}

/**
 *  Returns a boolean value of whether or not
 *  one second has pased since last time 'previousTime'
 *  was updated with the 'currentTime' number.
 */
boolean oneSecondHasPassed() {
  const int oneSecond = 1000; // 1 second.
  return currentTime > previousTimeTwo + oneSecond;
}

/**
 *  Returns a boolean value of whether or not
 *  one minute has passed since last time 'previousTime'
 *  was updated with the 'currentTime' number.
 */
boolean oneMinuteHasPassed() {
  const unsigned int oneMinute = 6000; // 60 seconds.
  return currentTime > previousTime + oneMinute;
}

/**
 *  Converts milliseconds to seconds.
 *  
 *  @param number  the number of milliseconds to be converted to seconds
 */
unsigned long convertMillisecondsToSeconds(unsigned long number) {
  return number * 0.001;
}

/**
 *  Prints information to the serial monitor of the condition
 *  of the sensors and status of the elements in the circuit.
 */
void outputInformation() {
  const String newLine = "\n";
  const String prefix = "Status as of " + (String) convertMillisecondsToSeconds(currentTime) + "s:";
  const String information = "Temperature: " + (String) temperature + newLine +
                             "Light level: " + (String) lightLevel + newLine +
                             "Heating element: " + convertLedStateToString(redLedState) + newLine +
                             "Air condition: " + convertLedStateToString(blueLedState);
  Serial.println(prefix + newLine + information + newLine);
}

/**
 *  Registers the pin modes of the light emitting diodes in the circuit.
 */
void registerLeds() {
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
}

/**
 *  Setup function. This code runs once the program starts and never again.
 */
void setup() {
  Serial.begin(9600); // Starts serial communication with the serial monitor on 9600 bps.
  registerLeds();     // Registers the pin modes of the light emitting diodes.
}

/**
 *  Loop runs over and over after the setup function has ended.
 */
void loop() {
  currentTime = millis();       // update the current time.
  if (oneSecondHasPassed()) {   // if a second has passed...
    previousTimeTwo = currentTime;// update previousTimeTwo to currentTime to allow the one second repeater to start over.
    updateSensorValues();         // reads from the sensors and updates the sensor variables.
  }
  if (oneMinuteHasPassed()) {     // if a minute has passed...
    previousTime = currentTime;   // updates previousTime to currentTime to allow the one minute repeater to start over.
    outputInformation();          // write information of the circuit to the serial monitor for the user to read.
  }
  decideLedStates();            // decides whether the LEDs should be on or off based on their led states.
  updateLeds();                 // turns the LEDs on or off.
}
