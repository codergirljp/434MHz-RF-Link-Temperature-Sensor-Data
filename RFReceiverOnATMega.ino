/*
   This code is intended to run on the Arduino Mega as the recieving end of and 434MHz RF Link 
   where the transmitter only transmits temperature data with a start pattern of "Temp"
   A more detail discussion of this and other projects can be found: 
   http://codergirljp.blogspot.com/
   
*/
#define LEDPIN 13                 // to use the Arduino Mega 2560 onboard Led on pin 13
#define PATTERN_LEN 4             // Length of the start Pattern sequence

char StartPattern[] = "Temp";     // The sequence pattern of characters we expect to get before each temperature reading
int dataPatternIndex = 0;         // an index used to track that we received the start pattern
byte startDataPatternRcvd = 0;    // if the dataPattern is found we post data until data is no longer in the allowed data 

void setup()
{ 
  Serial.begin(1200);   // USB Serial monitor for debugging
  Serial3.begin(1200);  // Serial3 UART - RX/TX are pins 15/14 on the Mega (only Receiving so only using RX pin 15)
  
  // Set the Led pin as an output pin
  // wait 1/2 second, then blink the LED one time slowly
  pinMode(LEDPIN, OUTPUT);
  delay(500);
  blink();
}

void loop()
{
    if(Serial3.available()>0)
    {     
        // Read a data byte from Serial3
        byte in3Byte = Serial3.read();
        
       // Only process data we expected to get
        if(isAllowedData(in3Byte))
        {
          char charByte = static_cast<char>(in3Byte);
          if(dataPatternIndex < PATTERN_LEN && charByte == StartPattern[dataPatternIndex])
          {
            // receiving the Temp pattern sequence
            LedOn();
            
            if(dataPatternIndex==(PATTERN_LEN-1))
            {
              // hey we got the whole sequence pattern (i.e. "Temp") 
              // expecting the temperature reading next
              Serial.print("Temp");
              startDataPatternRcvd = 1;
              
            }
            
            dataPatternIndex++;
          }
          else if(startDataPatternRcvd)
          {
            // we are getting the temperature reading now
            // To Do - validate the temperature data and store it in a variable to be re-used
            Serial.print(charByte);
            LedOn();
          }
          
        }
        else
        {
          // The data we just got is not allowed data
          // Reset the data pattern sequence check and stop looking for temperature data
          dataPatternIndex = 0;
          startDataPatternRcvd = 0;
          
          // led is set high when we are checking pattern sequence or receiving temperature data
          // if it was high, set it low and end the line on the serial debug to make it easier to read
          if(digitalRead(LEDPIN) == HIGH)          
          {  
             Serial.println("");
             digitalWrite(LEDPIN, LOW);
          }
        }
     }      
}


// Blink the Led for 1 second
void blink()
{
  digitalWrite(LEDPIN, HIGH);   // turn the LED on (HIGH voltage level)
  delay(1000);               // wait for a second
  digitalWrite(LEDPIN, LOW);    // turn the LED off (LOW voltage level)
}

// Turn the LED On
void LedOn()
{
  if(digitalRead(LEDPIN)==LOW)  digitalWrite(LEDPIN, HIGH);
}


// The Primary data filter - the RF link is NOISY
// Check to filter out the majority of invalid data 
// only allow data that we expected to be sent from our temperature sensor
byte isAllowedData(byte data)
{
   // Filter out must data we don't expected to get
   // allow ascii char codes for - . and 0-9 (45 - 57)
   // or ascii char codes for T e m p sent before each temperature data report
   if((data>44 && data<58) || data==84 || data==101 || data==109 || data==112)
    {
      return 1;
    }
    else
    {
      return 0;
    }       
}
