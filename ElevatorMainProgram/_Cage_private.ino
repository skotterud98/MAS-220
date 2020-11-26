
volatile long Cage::encCount    = StateMachine.getFloorPosition(0);     // Initializing static member variables outside class
long Cage::protectedCount       = StateMachine.getFloorPosition(0);     // Initializing both encoder counts at predefined first floor positions
long Cage::previousCount        = 0;



void Cage::ISR_A()
{
    if (digitalRead(encoderPinB) == LOW)
    {
        if (digitalRead(encoderPinA) == HIGH)             // Interrupt Service Routine for encoder channel A. Counts up or down depending on what direction the motor is spinning
        {
            encCount++;
        }
        else
        {
            encCount--;
        }
    }
    else
    {
        if (digitalRead(encoderPinA) == HIGH)
        {
            encCount--;
        }
        else
        {
            encCount++;
        }
    }

    noInterrupts();
    protectedCount = encCount;        // Making absolute sure that no interruption happens when storing the count. This is to ensure that we don't miss a single pulse from hall sensor in encoder
    interrupts();
}




void Cage::ISR_B() 
{
    if (digitalRead(encoderPinA) == LOW)
    {
        if (digitalRead(encoderPinB) == HIGH)
        {
            encCount--;
        }
        else
        {
            encCount++;                          // Interrupt Service Routine for encoder channel B. Counts up or down depending on what direction the motor is spinning
        }
    }
    else
    {
        if (digitalRead(encoderPinB) == HIGH)
        {
            encCount++;
        }
        else
        {
            encCount--;
        }
    }

    noInterrupts();
    protectedCount = encCount;        // Making absolute sure that no interruption happens when storing the count. This is to ensure that we don't miss a single pulse from hall sensor in encoder
    interrupts();
}
