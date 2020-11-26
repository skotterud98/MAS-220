
Cage::Cage()      // Defining Cage class constructor
{
    pinMode (Enable, OUTPUT);
    pinMode (Phase, OUTPUT);            // Sets motor pins as output
    pinMode (Decay, OUTPUT);

    pinMode (encoderPinA, INPUT_PULLUP);      // Sets encoder pins as input with pullup resistors activated
    pinMode (encoderPinB, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(encoderPinA), ISR_A, CHANGE);             // Attach interrupt to both encoderpins with a ISR function for each channel
    attachInterrupt(digitalPinToInterrupt(encoderPinB), ISR_B, CHANGE);             // Acting on any change in voltage (rising/falling)

    digitalWrite(Decay, HIGH);          // Sets decay on DC-motor to low
}


void Cage::motorRotate(float power)         // Defining motorRotate() function inside Cage class
{
    digitalWrite(Enable, HIGH);             // Depending on the input it drives the DC-motor with a PWM signal on phase pin

    analogWrite(Phase, power);
}



void Cage::motorStop()                      // Defining motorStop() function inside Cage class
{
    digitalWrite(Enable, LOW);              // Drives enable and decay pin low, resulting in a total stop of the DC-motor
    digitalWrite(Decay, LOW);
}



float Cage::senseDeltaPosition()            // Defining senseDeltaPosition() function inside Cage class
{
    float deltaCount = protectedCount - previousCount;      // Calculates the difference in current encoder count and a previous count, then intialize the previous count and return the difference
    
    previousCount = protectedCount;
    
    return deltaCount;
}


float Cage::sensePosition()                 // Defining sensePosition() function inside Cage class
{
    return protectedCount;                  // Return the current count (postion) of the encoder
}


float Cage::senseLoad()                     // Defining senseLoad() function inside Cage class
{
    float load;
    
    load = analogRead(loadSensor);          // Reads the analog input from the potentiometer and returns the value (0 to 1023)

    return load;
}
