#define METER_PIN     3 // This pin connects to the positive input of the gauge.
                        // Remember to add the right resistors inline!

#define MAX_POSITION 100
#define MIN_POSITION 0

# define MAX_TRANSITION_TIME 9999
# define MIN_TRANSITION_TIME 0

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

uint8_t previousVoltage = 0;

void setup () {
  pinMode(METER_PIN, OUTPUT); // define PWM meter pin as outputs
  Serial.begin(9600);
  inputString.reserve(9); // "\d\d\d,\d\d\d\d\n"
}

void loop () {
    if (stringComplete) {
      signed int commaPosition;
      signed int values[] = { 0, 0 };
      uint8_t valuePosition = 0;

      unsigned int transitionTimeElapsed = 0;

      while(commaPosition >=0) {
        commaPosition = inputString.indexOf(',');
        if(commaPosition != -1) {
          values[valuePosition] = inputString.substring(0,commaPosition).toInt();
          inputString = inputString.substring(commaPosition+1, inputString.length());
        } else {  // here after the last comma is found
          if(inputString.length() > 0)
            values[valuePosition] = inputString.toInt(); // if there is text after the last comma, print it.
        }
        valuePosition++;
      }

      signed int inputPosition = values[0];
      if (inputPosition > MAX_POSITION) { inputPosition = MAX_POSITION; }
      if (inputPosition < MIN_POSITION) { inputPosition = MIN_POSITION; }

      unsigned int transitionTime = values[1]; // milliseconds to complete change
      if (transitionTime > MAX_TRANSITION_TIME) { transitionTime = MAX_TRANSITION_TIME; }
      if (transitionTime < MIN_TRANSITION_TIME) { transitionTime = MIN_TRANSITION_TIME; }   
      
      uint8_t positionVoltage = map(inputPosition, MIN_POSITION, MAX_POSITION, 0, 255);
   
      if ((previousVoltage != positionVoltage) && (transitionTime) && (transitionTime > 0)) {
        signed int voltageDifference = previousVoltage - positionVoltage;
        if (voltageDifference != 0) {
          signed int change = 0;
          if (voltageDifference < 0) { // counting upward
            change = 1;
          } else { //downward
            change = -1;
          }

          voltageDifference = abs(voltageDifference);

          unsigned int transitionStepTime = transitionTime/voltageDifference;

          uint8_t currentVoltage = previousVoltage;
          while (voltageDifference > 0) {
            currentVoltage = currentVoltage + change;
            analogWrite(METER_PIN, currentVoltage);
            voltageDifference--;
            delay(transitionStepTime);
            transitionTimeElapsed = transitionTimeElapsed + transitionStepTime;
          }
        }
      }

      // final write, or only write if no transitions.
      analogWrite(METER_PIN, positionVoltage);

      previousVoltage = positionVoltage;
   
      // clear the string:
      inputString = "";
      stringComplete = false;

      // print ACK to client
      Serial.println("ACK. Position: " + String(inputPosition, DEC) + ", transitionTime: " + String(transitionTime) + ", PWM:" + String(positionVoltage, DEC));

      // adaptive delay
      // if previous transition took more than 100ms, don't delay here
      if (transitionTimeElapsed < 100) { delay(100 - transitionTimeElapsed); }
    } else {
      // normal polling delay
      delay(100);
      // Explicit calling of serialEvent() is only needed on Leonardo/Micro.
      // For other *duinos it should be called automatically. YMMV.
      //serialEvent();
    }
}

/*
 SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 
 This should be called automatically on most *duinos,
 EXCEPT the Leonardo and Micro (and derivities). For those,
 Please uncomment the serialEvent() call in loop().
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it. '\n' is
    // a unix newline, '\r' is part of '\r\n', the MS-DOS
    // newline.
    if ((inChar == '\n') || (inChar == '\r')) {
      inputString.trim(); // remove whitespace, including the newline characters.
      if (inputString.length()>0) {
        stringComplete = true;
      }
    }
  }
}

