
Buttons::Buttons()        // Defining the Buttons class constructor
{
    for (int i = 0; i <= 1; i++)
    {
        pinMode(upButton[i], INPUT_PULLUP);
        pinMode(downButton[i], INPUT_PULLUP);               // Sets the mode of the pins to the pushbuttons and LED
        pinMode(upButtonLED[i], OUTPUT);
        pinMode(downButtonLED[i], OUTPUT);
    }
    for (int i = 0; i <= 2; i++)
    {
        pinMode(cageButton[i], INPUT_PULLUP);
        pinMode(cageButtonLED[i], OUTPUT);
    }
    pinMode(cageButtonLED[3], OUTPUT);
    pinMode(emergencyButton, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(emergencyButton), ISR_EMERG, RISING);       // Attach an interrupt to the emergency button pin, telling it what function to execute and
}                                                                                     // that it should respond to a rising signal


void Buttons::turnOffLED(int floorNumber)                 // Defining the turnOffLED() function inside Buttons class
{
    digitalWrite(cageButtonLED[floorNumber], LOW);
    
    if (floorNumber >= 1 && floorNumber <= 2)
    {
        digitalWrite(downButtonLED[floorNumber - 1], LOW);          // Sets the pin that supplys the LED's, of a floornumber input, to low
    }
    if (floorNumber >= 0 && floorNumber <= 1)
    {
        digitalWrite(upButtonLED[floorNumber], LOW);
    }
}


void Buttons::checkInFloor()                              // Defining the checkInFloor() function inside Buttons class
{
    for (int i = 0; i <= 1; i++)
    {
        if (digitalRead(upButton[i]) == HIGH)
        {
            if (floorRequest[i][1] == DOWN)           
            {
                floorRequest[i][1] = BOTH;                // In case both of the direction calling buttons in a floor is pressed, store BOTH as the request information
            }
            else if (floorRequest[i][1] != BOTH)
            {
                floorRequest[i][1] = UP;                  // Places a request saying you want to go up from desired floor. Only in 1st and 2nd floor.
            }               

            digitalWrite(upButtonLED[i], HIGH);           // Turns on <Up> LED above pressed button
        }


        if (digitalRead(downButton[i]) == HIGH)
        {
            if (floorRequest[i + 1][1] == UP)
            {
                floorRequest[i + 1][1] = BOTH;            // In case both of the direction calling buttons in a floor is pressed, store BOTH as the request information
            }
            else if (floorRequest[i + 1][1] != BOTH)
            {
                floorRequest[i + 1][1]   = DOWN;          // Places a request saying you want to go down from desired floor. Only in 2nd and 3rd floor.
            }

            digitalWrite(downButtonLED[i], HIGH);         // Turns on <Down> LED above pressed button
        }
    }
}


void Buttons::checkInCage()                               // Defining the checkInCage() function inside Buttons class
{
    for (int i = 0; i <= 2; i++)
    {
        if (digitalRead(cageButton[i]) == HIGH)
        {
            floorRequest[i][0]     = REQUEST;             // Places a floor request when a button is pressed inside cage

            digitalWrite(cageButtonLED[i], HIGH);         //Turns on floor LED above pressed button 
        }
    }
}



void Buttons::emergBlinkLED(bool ON)                      // Defining the emergBlinkLED() function inside Buttons class
{
    int delayBetweenBlink = 30;       // Delay in milliseconds between each LED blink

    if (ON)
    {
        if (checkTime(lastMillis, delayBetweenBlink) && blinkSequence == 0)
        {
            lastMillis = millis();
            digitalWrite(cageButtonLED[3], LOW);
            digitalWrite(upButtonLED[0], HIGH);
            blinkSequence = 1;
        }
        if (checkTime(lastMillis, delayBetweenBlink) && blinkSequence == 1)
        {
            lastMillis = millis();
            digitalWrite(upButtonLED[0], LOW);
            digitalWrite(downButtonLED[0], HIGH);                                   // This function blinks all LED's one at a time starting from left to right if input is true
            blinkSequence = 2;
        }
        if (checkTime(lastMillis, delayBetweenBlink) && blinkSequence == 2)
        {
            lastMillis = millis();
            digitalWrite(downButtonLED[0], LOW);
            digitalWrite(upButtonLED[1], HIGH);
            blinkSequence = 3;
        }
        if (checkTime(lastMillis, delayBetweenBlink) && blinkSequence == 3)
        {
            lastMillis = millis();
            digitalWrite(upButtonLED[1], LOW);
            digitalWrite(downButtonLED[1], HIGH);
            blinkSequence = 4;
        }
        if (checkTime(lastMillis, delayBetweenBlink) && blinkSequence == 4)
        {
            lastMillis = millis();
            digitalWrite(downButtonLED[1], LOW);
            digitalWrite(cageButtonLED[0], HIGH);
            blinkSequence = 5;
        }
        if (checkTime(lastMillis, delayBetweenBlink) && blinkSequence == 5)
        {
            lastMillis = millis();
            digitalWrite(cageButtonLED[0], LOW);
            digitalWrite(cageButtonLED[1], HIGH);
            blinkSequence = 6;
        }
        if (checkTime(lastMillis, delayBetweenBlink) && blinkSequence == 6)
        {
            lastMillis = millis();
            digitalWrite(cageButtonLED[1], LOW);
            digitalWrite(cageButtonLED[2], HIGH);
            blinkSequence = 7;
        }
        if (checkTime(lastMillis, delayBetweenBlink) && blinkSequence == 7)
        {
            lastMillis = millis();
            digitalWrite(cageButtonLED[2], LOW);
            digitalWrite(cageButtonLED[3], HIGH);
            blinkSequence = 0;
        } 
    }
    else
    {
        for (int i = 42; i <= 49; i++)              // Turn off all the LED's when input goes false
        {
            digitalWrite(i, LOW);
        }
    }
}


void Buttons::ISR_EMERG()
{
    nextState = emergency_protocol;                 // Interrupt service routine for emergency button
}
