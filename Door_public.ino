
Door::Door()        // Defining of the Door class constructor
{
    pinMode (EnableA, OUTPUT);
    pinMode (EnableB, OUTPUT);      // Sets motorpins as output
    pinMode (PhaseA, OUTPUT);
    pinMode (PhaseB, OUTPUT);

    pinMode (SDI, OUTPUT);
    pinMode (SCK, OUTPUT);        // Sets Digital to Analog converter pins to output
    pinMode (CS, OUTPUT);
    pinMode (LDAC, OUTPUT);
}


void Door::opening()      // Defining opening() function inside Door class
{
    dac_ON(true);                       // Executes dac_ON() function with a true input to turn the DAC on to get current through the windings of the stepper
    int steps = 40;                     // Number of step iterations thought the loop where one iteration is 8 half steps
    int waitOpen = 1000;                // Timer in milliseconds to wait before opening doors when elevator has stopped at a floor
    float delayBetweenSteps = 2.0;      // Delay between each of the steps. This is how we control the speed of the stepper. Shorter the delay, faster the rotation

    if (checkTime(lastMillis, waitOpen) || wait == false)        // Checks if the wait to open timer has passed or that the boolean wait variable is false
    {
        doorState = OPENING;      // Changing the state of the door. This is kind of our sensor, since we dont have any other sensor on the doorsystem
        wait = false;             // Changing the boolean wait variable to false so that the program don't have to wait for the open timer delay each time iterating through steps
        
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 0)
        {
            lastMillis = millis();
            digitalWrite (EnableA, HIGH);     // Step 1                       // Here are the 8 halfsteps that are cariied out one by one and looped until it has reached wanted number of loops
            digitalWrite (EnableB, HIGH);                                     // Using the stepsequence variable to keep track of what step to execute when
            digitalWrite (PhaseA, LOW);
            digitalWrite (PhaseB, LOW);
            stepSequence = 1;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 1)
        {
            lastMillis = millis();
            digitalWrite (EnableA, LOW);      // Step 2
            digitalWrite (EnableB, HIGH);
            digitalWrite (PhaseA, LOW);
            digitalWrite (PhaseB, LOW);
            stepSequence = 2;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 2)
        {
            lastMillis = millis();
            digitalWrite (EnableA, HIGH);     // Step 3
            digitalWrite (EnableB, HIGH);
            digitalWrite (PhaseA, HIGH);
            digitalWrite (PhaseB, LOW);
            stepSequence = 3;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 3)
        {
            lastMillis = millis();
            digitalWrite (EnableA, HIGH);     // Step 4
            digitalWrite (EnableB, LOW);
            digitalWrite (PhaseA, HIGH);
            digitalWrite (PhaseB, LOW);
            stepSequence = 4;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 4)
        {
            lastMillis = millis();
            digitalWrite (EnableA, HIGH);     // Step 5
            digitalWrite (EnableB, HIGH);
            digitalWrite (PhaseA, HIGH);
            digitalWrite (PhaseB, HIGH);
            stepSequence = 5;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 5)
        {
            lastMillis = millis();
            digitalWrite (EnableA, LOW);      // Step 6
            digitalWrite (EnableB, HIGH);
            digitalWrite (PhaseA, LOW);
            digitalWrite (PhaseB, HIGH);
            stepSequence = 6;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 6)
        {
            lastMillis = millis();
            digitalWrite (EnableA, HIGH);     // Step 7
            digitalWrite (EnableB, HIGH);
            digitalWrite (PhaseA, LOW);
            digitalWrite (PhaseB, HIGH);
            stepSequence = 7;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 7)
        {
            lastMillis = millis();
            digitalWrite (EnableA, HIGH);     // Step 8
            digitalWrite (EnableB, LOW);
            digitalWrite (PhaseA, LOW);
            digitalWrite (PhaseB, LOW);
            stepSequence = 0;
            stepCount ++;
        }
    }

    if (stepCount == steps)
    {   
        dac_ON(false);     
        digitalWrite (EnableA, LOW);          // Turn of DAC and stepper
        digitalWrite (EnableB, LOW);
        digitalWrite (PhaseA, LOW);
        digitalWrite (PhaseB, LOW);

        stepCount = 0;                        // Resetting the stepcount
        doorState = OPEN;                     // CHanging the state of the door to open
        wait = true;                          // Resetting the boolean wait variable so it waits for the open delay timer next time door opening or closing
        lastMillis = millis();
        return;
    }
}



void Door::closing()          // Defining closing function inside the Door class
{
    dac_ON(true);                           
    int steps = 40;
    int waitClose = 4000;         // Closing function is pretty much the same as the opening function except that the 8 half steps is in the opposite direction, making the stepper go the other way
    float delayBetweenSteps = 2.0;

    if (checkTime(lastMillis, waitClose) || wait == false)
    {
        doorState = CLOSING;
        wait = false;
        
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 0)
        {
            lastMillis = millis();
            digitalWrite (EnableA, HIGH);     // Step 8
            digitalWrite (EnableB, LOW);
            digitalWrite (PhaseA, LOW);
            digitalWrite (PhaseB, LOW);
            stepSequence = 1;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 1)
        {
            lastMillis = millis();
            digitalWrite (EnableA, HIGH);     // Step 7
            digitalWrite (EnableB, HIGH);
            digitalWrite (PhaseA, LOW);
            digitalWrite (PhaseB, HIGH);
            stepSequence = 2;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 2)
        {
            lastMillis = millis();
            digitalWrite (EnableA, LOW);      // Sep 6
            digitalWrite (EnableB, HIGH);
            digitalWrite (PhaseA, LOW);
            digitalWrite (PhaseB, HIGH);
            stepSequence = 3;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 3)
        {
            lastMillis = millis();
            digitalWrite (EnableA, HIGH);     // Step 5
            digitalWrite (EnableB, HIGH);
            digitalWrite (PhaseA, HIGH);
            digitalWrite (PhaseB, HIGH);
            stepSequence = 4;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 4)
        {
            lastMillis = millis();
            digitalWrite (EnableA, HIGH);     // Step 4
            digitalWrite (EnableB, LOW);
            digitalWrite (PhaseA, HIGH);
            digitalWrite (PhaseB, LOW);
            stepSequence = 5;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 5)
        {
            lastMillis = millis();
            digitalWrite (EnableA, HIGH);     // Step 3
            digitalWrite (EnableB, HIGH);
            digitalWrite (PhaseA, HIGH);
            digitalWrite (PhaseB, LOW);
            stepSequence = 6;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 6)
        {
            lastMillis = millis();
            digitalWrite (EnableA, LOW);      // Step 2
            digitalWrite (EnableB, HIGH);
            digitalWrite (PhaseA, LOW);
            digitalWrite (PhaseB, LOW);
            stepSequence = 7;
        }
        if (checkTime(lastMillis, delayBetweenSteps) && stepSequence == 7)
        {
            lastMillis = millis();
            digitalWrite (EnableA, HIGH);     // Step 1
            digitalWrite (EnableB, HIGH);
            digitalWrite (PhaseA, LOW);
            digitalWrite (PhaseB, LOW);
            stepSequence = 0;
            stepCount ++;
        }
    }

    if (stepCount == steps)
    {   
        dac_ON(false);     
        digitalWrite (EnableA, LOW);          // Turn of DAC and stepper
        digitalWrite (EnableB, LOW);
        digitalWrite (PhaseA, LOW);
        digitalWrite (PhaseB, LOW);

        stepCount = 0;
        doorState = CLOSED;                   // Changine the state of the door to closed
        wait = true;
        lastMillis = millis();
        return;
    }
}




bool Door::senseClosed()            // Defining senseClosed() function inside Door class
{
    if (doorState == CLOSED)
    {
        return true;                // Checks if door is closed and returns true if so. This is our door "sensor" function
    }
    else
    {
        return false;
    }
}
