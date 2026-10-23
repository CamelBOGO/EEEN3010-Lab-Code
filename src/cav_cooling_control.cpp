
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "DHT.h"
#include "fan.h"
#include "humidity_sensor.h"
#include "led.h"
#include "pwm.h"
#include "pwm_servo.h"
#include "temperature_sensor.h"

// =====================================================================================================================
//                                       Global Variables and Objects
// =====================================================================================================================

// Built-in status LED on the controller board.
Led onboardLed;

// The LEDD indicator pins for the Peltier device and the humidifier.
const int peltierEnablePin = 19;
const int humidifierEnablePin = 12;
Led peltier(peltierEnablePin);
Led humidifier(humidifierEnablePin);

// LCD screen size and I2C address. The alternate address is kept here in case
const int lcdColumns = 20;
const int lcdRows = 4;
const byte lcdAddr = 0x27;     // PCF8574 chip from Texas Instruments
const byte lcdAltAddr = 0x3F;  // PCF8574 chip from NXP Semiconductors
LiquidCrystal_I2C lcd(lcdAddr, lcdColumns, lcdRows);

// Six DHT22 sensors measure both humidity and air temperature.
#define DHTTYPE DHT22                                 // DHT 22  (AM2302), AM2321
const int dhtNum = 6;                                 // Number of DHT sensors
const int dhtPin[dhtNum] = {32, 33, 25, 26, 27, 14};  // Digital pin connected to the DHT sensor
HumiditySensor humiditySensor[dhtNum] = {
    HumiditySensor(dhtPin[0], DHTTYPE),
    HumiditySensor(dhtPin[1], DHTTYPE),
    HumiditySensor(dhtPin[2], DHTTYPE),
    HumiditySensor(dhtPin[3], DHTTYPE),
    HumiditySensor(dhtPin[4], DHTTYPE),
    HumiditySensor(dhtPin[5], DHTTYPE)};

// Two DS18B20 sensors measure temperature.
const int ds18b20Num = 2;                     // Number of DS18B20 sensors
const int ds18b20Pin[ds18b20Num] = {13, 23};  // Digital pin connected to the DS18B20 sensor
TemperatureSensor temperatureSensor[ds18b20Num] = {
    TemperatureSensor(ds18b20Pin[0]),
    TemperatureSensor(ds18b20Pin[1])};

// Fan speed is controlled with PWM (rapid on/off pulses).
// Pin, channel, frequency, and resolution are set for each fan.
const int fanPwmFreq = 25000;  // driving frequency needs to be 25kHz to work, 50Hz does not work!
const int fanPwmRes = 10;      // 10 bit resolutions, 10 equals to 10bit, [0,1023]
const int fanInRpmPin = 36;
const int fanInPwmPin = 18;
const int fanInPwmCh = 0;
const int fanOutRpmPin = 39;
const int fanOutPwmPin = 5;
const int fanOutPwmCh = 1;
Fan fanIn(fanInRpmPin, &fanInRpmCount, fanInRpmInterruptRoutine, fanInPwmPin, fanInPwmCh, fanPwmFreq, fanPwmRes);
Fan fanOut(fanOutRpmPin, &fanOutRpmCount, fanOutRpmInterruptRoutine, fanOutPwmPin, fanOutPwmCh, fanPwmFreq, fanPwmRes);

// Heater PWM settings and controller.
const int heaterPwmFreq = 50;  // heater pwm frequency
const int heaterPwmRes = 10;   // 10 bit resolutions
const int heaterPwmPin = 17;   // GPIO17
const int heaterPwmCh = 2;     // pwm channels i.e. which timer channels
Pwm heater(heaterPwmPin, heaterPwmCh, heaterPwmFreq, heaterPwmRes);

// Three servo motors position the air-mixing, exhaust, and outdoor-air dampers.
// Pin, channel, frequency, resolution, and offset are set for each servo.
const int servoPwmFreq = 50;  // main frequency for all pwm channels
const int servoPwmRes = 10;   // main resolution of all pwm channels
const int servoAirMixPwmPin = 16;
const int servoAirMixPwmCh = 3;
const int servoAirMixOffsetCount = 4;
const int servoAirExhaustPwmPin = 15;
const int servoAirExhaustPwmCh = 4;
const int servoAirExhaustOffsetCount = 0;
const int servoAirOutdoorPwmPin = 4;
const int servoAirOutdoorPwmCh = 5;
const int servoAirOutdoorOffsetCount = 0;
PwmServo servoAirMix(servoAirMixPwmPin, servoAirMixPwmCh, servoPwmFreq, servoPwmRes, servoAirMixOffsetCount);
PwmServo servoAirExhaust(servoAirExhaustPwmPin, servoAirExhaustPwmCh, servoPwmFreq, servoPwmRes, servoAirExhaustOffsetCount);
PwmServo servoAirOutdoor(servoAirOutdoorPwmPin, servoAirOutdoorPwmCh, servoPwmFreq, servoPwmRes, servoAirOutdoorOffsetCount);

// Two knobs provide adjustable input values through the ESP32 analog pins.
const int knobNum = 2;
const int knobPin[knobNum] = {34, 35};
float knobValue[knobNum] = {0, 0};

// =====================================================================================================================
//                                            Function Prototypes
// =====================================================================================================================

void lcdInit();
void lcdWrite(int column, int row, String text);
float readKnobInput(int knobIndex);
void displaySerialPage();

// =====================================================================================================================
//                                            Custom Functions
// =====================================================================================================================

// Start the LCD and show a short welcome message.
void lcdInit() {
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("Hello, World!");
}

// Move the LCD cursor to a column and row, then print text there.
void lcdWrite(int cursorX, int cursorY, String str) {
    lcd.setCursor(cursorX, cursorY);
    lcd.print(str);
}

// Read one knob and return its value as a fraction from about 0.0 to 1.0.
float readKnobInput(int channel) {
    // Keep the knob number within the valid range: 0 or 1.
    channel = constrain(channel, 0, 1);

    // The ESP32 analog reading is divided by 4096 to convert it to a fraction from 0.0 to 1.0.
    return analogRead(knobPin[channel]) / 4096.0;
}

// Send sensor readings and device settings to the computer over Serial.
void displaySerialPage() {
    // Print humidity from each DHT22 sensor.
    for (int i = 0; i < dhtNum; i++) Serial.printf("%.1f,", humiditySensor[i].getHumidity());
    // Print air temperature from each DHT22 sensor.
    for (int i = 0; i < dhtNum; i++) Serial.printf("%.1f,", humiditySensor[i].getTemperature());
    // Print temperature from each DS18B20 sensor.
    for (int i = 0; i < ds18b20Num; i++) Serial.printf("%.2f,", temperatureSensor[i].getTempC());
    // Print device on/off states and PWM ratios in the same data row.
    Serial.printf("%d,%d,%.2f,%.2f,%.2f,\n",
                  peltier.getState(),
                  humidifier.getState(),
                  heater.getRatio(),
                  fanIn.pwm.getRatio(),
                  fanOut.pwm.getRatio());
}

// =====================================================================================================================
//                                            Arduino Main Functions
// =====================================================================================================================

void setup() {
    // Start the computer connection used for messages and sensor readings.
    Serial.begin(115200);
    Serial.println("Initializing...");

    // Initialize display early so users can see startup progress/errors
    lcdInit();
    lcdWrite(0, 0, "Initializing...");

    // Wait until the first DS18B20 reports a valid temperature.
    while (true) {
        // Request a temperature reading from each DS18B20 sensor and check the first sensor's reading.
        for (int i = 0; i < ds18b20Num; i++) temperatureSensor[i].requestTemperature();
        if (temperatureSensor[0].getTempC() > 0) break;

        // A repeated message helps identify a sensor or power connection problem.
        Serial.println("Invalid sensor measurement! Check if power supply is turned ON.");
        delay(2000);
    }

    // Start the display and show a short message to indicate the program is running.
    lcdWrite(0, 0, "Lab1:VAV Cooling");

    // Set the three servo motors to their initial positions.
    servoAirMix.writeRatio(0.5);
    servoAirExhaust.writeRatio(0.5);  // Suggest 0.5 for period 1, 0.2 for period 2
    servoAirOutdoor.writeRatio(0.5);  // Suggest 0.5 for period 1, 0.2 for period 2

    // Pre-cool the cooling sink with the Peltier device until it reaches a target temperature.
    Serial.println("Pre-cool the cooling sink...");
    float targetCoolSinkTemp = 8.0;
    do {
        // Request a temperature reading from each DS18B20 sensor.
        for (int i = 0; i < ds18b20Num; i++) temperatureSensor[i].requestTemperature();

        // Turn on the Peltier device to cool the cooling sink until it reaches the target temperature.
        peltier.on();

        // display temperature
        Serial.printf("targetCoolSinkTemp: %f, currentCoolSinkTemp: %f\n", targetCoolSinkTemp, temperatureSensor[0].getTempC());
        delay(2000);  // Max 0.5Hz update rate for DHT22
    } while (temperatureSensor[0].getTempC() > targetCoolSinkTemp);

    Serial.println("Pre-cool...finish!");
    delay(1000);

    // Turn off the heater and humidifier, as they are not used in this cooling control lab.
    heater.writeRatio(0.0);
    humidifier.off();
}

void loop() {
    // Request new readings before getting the values.
    for (int i = 0; i < dhtNum; i++) humiditySensor[i].readMeasurements();
    for (int i = 0; i < ds18b20Num; i++) temperatureSensor[i].requestTemperature();

    // Update the current fan speed from the two fans.
    fanIn.updateHz();
    fanOut.updateHz();

    // Read the two knobs and store their values in the knobValue array.
    knobValue[0] = readKnobInput(0);
    knobValue[1] = readKnobInput(1);

    // Fan speed control: set the same fan speed for both fans, using a value from 0.0 to 1.0.
    float fanControlInput = 0.5;  // change this value
    fanIn.pwm.writeRatio(fanControlInput);
    fanOut.pwm.writeRatio(fanControlInput);

    // Lab2 VAV cooling controls STARTS HERE ===========================================================================
    float targetAirTemperature = 22.0;                    // you can change this value so that this value is lower than the room environment.
    targetAirTemperature += knobValue[0] - knobValue[1];  // quick offset for update in realtime without re-program

    // Get the room temperature.
    float roomTemperature = humiditySensor[1].getTemperature();

    // Control logic for Peltier device: turn on the Peltier device if the room temperature is above the target.
    if (roomTemperature > targetAirTemperature) {
        peltier.on();
    } else {
        peltier.off();
    }
    // Lab2 VAV cooling controls ENDS HERE =============================================================================

    // Show current readings on the computer and the LCD.
    displaySerialPage();

    // Update current status on the LCD.
    lcdWrite(0, 1, "Fan:");
    lcdWrite(4, 1, String(fanControlInput));
    lcdWrite(10, 1, String(knobValue[0]));
    lcdWrite(15, 1, String(knobValue[1]));
    lcdWrite(0, 2, "targetTemp:");
    lcdWrite(12, 2, String(targetAirTemperature));
    lcdWrite(0, 3, "currentTemp:");
    lcdWrite(13, 3, String(roomTemperature));

    onboardLed.toggle();
    delay(2000);  // DHT22 sensors needs to be updated at 2Hz max
}
