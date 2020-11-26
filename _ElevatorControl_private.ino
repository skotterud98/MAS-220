
void ElevatorControl::checkDirection()
{   
    for (int i = 0; i <= 2; i++)
    {
        if ((floorRequest[i][0] == REQUEST || floorRequest[i][1] != IDLING) && currentFloor == i)
        {
            lastMillis = millis();
            nextState = door_opening;          // If there is a request in the same floor as the cage is currently idling, go to door_opening state
        }
    }
    
    switch (currentFloor)
    {
        case 0:
            
            for (int i = 1; i <= 2; i++)
            {
                if (floorRequest[i][0] == REQUEST || floorRequest[i][1] != IDLING)    // If elevator is in first floor, next state is upwards regardsless of where request is
                {
                    nextState = elevator_up;
                }
            }
            break;

        case 1:

            if ((floorRequest[0][0] == REQUEST || floorRequest[0][1] != IDLING) && floorRequest[2][0] == IDLING && floorRequest[2][1] == IDLING)
            {
                nextState = elevator_down;     // If there is a request in 1st floor and there's no requests in 3rd floor -- > go down
            }
            
            if ((floorRequest[2][0] == REQUEST || floorRequest[2][1] != IDLING) && floorRequest[0][0] == IDLING && floorRequest[0][1] == IDLING)
            {
                nextState = elevator_up;       // If there is a request in 3rd floor and there's no requests in 1st floor --> go up
            }
            
            if (prevDirection == DOWN && (floorRequest[0][0] == REQUEST || floorRequest[0][1] != IDLING))
            {
                nextState = elevator_down;     // If there is a request in 1st floor and previous direction was downwards --> go down
            }
            
            if (prevDirection == UP && (floorRequest[2][0] == REQUEST || floorRequest[2][1] != IDLING))
            {
                nextState = elevator_up;       // If there is a request in 3rd floor and previous direction was upwards --> go up
            }
            break;
            

        case 2:
            
            for (int i = 1; i >= 0; i--)
            {
                if (floorRequest[i][0] == REQUEST || floorRequest[i][1] != IDLING)    // If elevator is in third floor, next state is downwards regardsless of where request is
                {
                    nextState = elevator_down;
                }
            }
            break;
    }
}



bool ElevatorControl::anyRequests()
{
    for (int i = 0; i <= 2; i++)
    {
        for (int j = 0; j <=1; j++)
        {
            if (floorRequest[i][j] != IDLING)         // Two for-loops to go through all values in the two-dimensional array for request. Returning tru if there is a request somewhere
            {
                return true;
            }
        }
    }
    return false;
}



void ElevatorControl::clearRequest(int floorNumber)
{
    floorRequest[floorNumber][0]    = IDLING;               // Clears all requests in the floor that gets inputed as parameter
    floorRequest[floorNumber][1]    = IDLING;

    buttons.turnOffLED(floorNumber);                        // Turns off request-LED's in that floor too
    buttons.turnOffLED(floorNumber);
}



int ElevatorControl::updateDestination()
{  
    switch (currentFloor)
    {
        case 0:   // Elevator starts moving from first floor
            if (floorRequest[1][0] == REQUEST || floorRequest[1][1] == UP || floorRequest[1][1] == BOTH || (floorRequest[1][1] == DOWN && floorRequest[2][0] == IDLING && floorRequest[2][1] == IDLING))
            {
                return 1;       // motorStops in second floor if there is a request inside cabin or at floorlevel, 
                                // acting on floorlevel-request only if it is an up request or that there is no requests in third floor
            }
            else if ((floorRequest[2][0] == REQUEST || floorRequest[2][1] == DOWN) && floorRequest[1][0] == IDLING && (floorRequest[1][1] == DOWN || floorRequest[1][1] == IDLING))
            {
                return 2;       // Skipping second floor and goes to third floor, if it only is a down- or no request in second floor
            }
            break;

        case 1:   // In this case the elevator had a stop in second floor and starts moving from second
        
            if ((floorRequest[2][0] == REQUEST || floorRequest[2][1] != IDLING) && (prevDirection == UP || floorRequest[0][0] == IDLING && floorRequest[0][1] == IDLING))
            {
                return 2;       // Goes to third floor if previous direction was upwards or no requests in first floor
            }
            if ((floorRequest[0][0] == REQUEST || floorRequest[0][1] != IDLING) && (prevDirection == DOWN || floorRequest[2][0] == IDLING && floorRequest[2][1] == IDLING))
            {
                return 0;       // Goes to first floor if previous direction was downwards or no requests in third floor
            }
            break;

            
        case 2:   // Elevator starts moving from second floor

            if (floorRequest[1][0] == REQUEST || floorRequest[1][1] == DOWN || floorRequest[1][1] == BOTH || (floorRequest[1][1] == UP && floorRequest[0][0] == IDLING && floorRequest[0][1] == IDLING))
            {
                return 1;       // motorStops in second floor if there is a request inside cabin or at floorlevel, 
                                // acting on floorlevel-request only if it is a down request or that there is no requests in first floor
            }
            else if ((floorRequest[0][0] == REQUEST || floorRequest[0][1] == UP) && floorRequest[1][0] == IDLING && floorRequest[1][1] != DOWN)
            {
                return 0;       // Skipping second floor and goes to first floor, if it only is an up- or no request in second floor
            }
            break;

        default:
            break;
    }
}



int ElevatorControl::checkFloor()
{
    long currentEncoderCount = cage.sensePosition();
    
    if (currentEncoderCount > (floorPosition[0] - 50) && currentEncoderCount < (floorPosition[0] + 50))
    {
        return 0;       // First floor if encoder count is at pre-defined floor position (+- 50 ticks)
    }
    
    if (currentEncoderCount > (floorPosition[1] - 50) && currentEncoderCount < (floorPosition[1] + 50))
    {
        return 1;       // Second floor if encoder count is at pre-defined floor position (+- 50 ticks)
    }
    
    if (currentEncoderCount > (floorPosition[2] - 50) && currentEncoderCount < (floorPosition[2] + 50))
    {
        return 2;       // Third floor if encoder count is at pre-defined floor position (+- 50 ticks)
    }
}



bool ElevatorControl::emergStop()
{
    if (nextState == emergency_protocol)        // Just a boolean function returning true og false depending on wheather we are going into emergency state or not.
    {                                           // Used in PID loop for instance
        return false;
    }
    else
    {
        return true;
    }
}



bool ElevatorControl::overloadDetected()
{
    if (cage.senseLoad() > 800)                 // A condition function returning true if the sensing load inside cage is above 800 (kg)
    {
        return true;
    }
    else
    {
        return false;
    }
}
