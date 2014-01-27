/*
  RF Link Transmitter and TMP36 temperature sensor data reading using an Arduino Micro
  -  RF transmitter data in is connected to the TX pin of the Micro
  -  uses analog reference pin tied to 3.3V of Arduino Micro 
  -  TMP36 Vcc is using 3.3V from Arduino Micro
  
  For more information about this project and other projects visit:
  http://codergirljp.blogspot.com/
*/

#define aref_voltage 3.36   // tie 3.3V to ARef and measure it with a multimeter
 
int tempSensorPin = A0;     // the analog input pin for the temperature sensor voltage reading
int sensorValue = 0;        // variable to store the value coming from the sensor

void setup()
{
    analogReference(EXTERNAL);
    Serial1.begin(1200);
}

void loop()
{
   // read the TMP36 temperature sensor value
   sensorValue = analogRead(tempSensorPin);   
   float voltage = sensorValue * aref_voltage;
   voltage/= 1024.0;
 
   // Calculate the temperature in Celcius
   // converting from 10 mv per degree with 500 mV offset
   // to degrees ((voltage - 500mV) times 100)
   float temperatureC = (voltage - 0.5) * 100 ;  
 
   // Convert to Fahrenheit
   float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
   
   // The 434MHz RF Link has a lot of noise. The Receiver side is looking for the character sequence Temp  
   // before each temperature reading. The sequence is unlikely to occur in random noise data.
   // Send some nulls to get the RF receiver to listen at our signal to noise level (improves reliablilty of getting "T")
   // Then send our data marker character sequence (Temp in the case)
   // Then send the temperature reading
   Serial1.write(0);Serial1.write(0);Serial1.write(0);Serial1.write(0);
   Serial1.write("Temp");
   Serial1.print(temperatureF);

   delay(5000); // wait 5 seconds
 
}
