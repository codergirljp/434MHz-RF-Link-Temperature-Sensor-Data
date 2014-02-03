/**
* 434MHz Transmitter Temperature Sensor on the Trinket using Software Serial 
*/

#include <SoftwareSerial.h>

#define aref_voltage 3.3   // tie 3.3V to ARef and measure it with a multimeter
int tempSensorPin = 3;     // the analog input pin for the temperature sensor voltage reading
int sensorValue = 0;       // variable to store the value coming from the sensor

SoftwareSerial sSerial(0,1); // RX, TX Pins
void setup()
{
    sSerial.begin(1200);
}

void loop()
{
   // read the TMP36 temperature sensor value
   sensorValue = analogRead(tempSensorPin);   
   float voltage = sensorValue * aref_voltage;
   voltage/= 1024.0;
 
   // Calculate the temperature in Celcius
   // datasheet says 1ºC/10mV, and 750mV @ 25ºC
   // equation for temperature from:
   //    1/10mV  = 1/0.01 = 100 [ºC/Volt]                                 // convert to volts
   //    100 [ºC/Volt] = (temperatureC - 25º)/(voltage - 0.75) [ºC/Volt]  // use linear slope point
   //    temperatureC = 100 * (voltage -0.75) + 25                        // temperatureC in terms of voltage
   //    temperatureC = 100 * (voltage - 0.75 + 0.25) 
   float temperatureC = (voltage - 0.5) * 100;  
 
   // Convert to Fahrenheit
   float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
   
   // The 434MHz RF Link has a lot of noise. The Receiver side is looking for the character sequence Temp  
   // before each temperature reading. The sequence is unlikely to occur in random noise data.
   // Send some nulls to get the RF receiver to listen at our signal to noise level (improves reliablilty of getting "T")
   // Then send our data marker character sequence (Temp in the case)
   // Then send the temperature reading
   SendNulls();
   sSerial.write("Temp");
   sSerial.print(temperatureF);

   delay(5000); // wait 5 seconds
 
}

void SendNulls()
{
  for(int i=0; i<5; i++)
  {
    sSerial.write((uint8_t)0);
  }
}
