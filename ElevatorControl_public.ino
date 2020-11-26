
void ElevatorControl::idling()        // Defining of the idling() function inside ElevatorControl class
{   
    lastMillis = millis();
    screen.turnOnBacklight();         
    screen.printFloor(currentFloor);    // Executes functions of the screen object of LCD class
    screen.printDoor(doorState);

    atFloor = true;             // Variable to use with PID accuracy paramteres. When start of elevator starts it has a +- 100 ticks tolerance on position, but when stopping it is exact position
    
    buttons.checkInFloor();           // Checks if any direction calling butting in one of the floors is pressed
    buttons.checkInCage();            // Checks if any buttons inside the cage is pressed
    
    if (anyRequests())                
    {
        checkDirection();             // If there is any requests, check and calculate what direction to go. Enters either elevator_up, elevator_down state or simply just stays inside idle
    }
}



void ElevatorControl::doorOpening()             // Defining of the doorOpening() function of the ElevatorControl class
{   
    if (cage.senseDeltaPosition() == 0)         // Checks that the cage is stationary at a floor and not moving before opening the door. Object function of class Cage
    {
        screen.printDoor(doorState);
    
        buttons.checkInFloor();                 // Checks if there are any requests inside cage and at the floors
        buttons.checkInCage();
        
        door.opening();                         // Open door
    
        if (doorState == OPEN)
        {
            screen.printDoor(doorState);
            clearRequest(currentFloor);         // When door is open, go to the next state, door_closing
            nextState = door_closing;
        }

    }
}



void ElevatorControl::doorClosing()             // Defining doorClosing() function inside the ElevatorControl class
{
    screen.printDoor(doorState);

    buttons.checkInFloor();
    buttons.checkInCage();

    if (overloadDetected() && doorState == OPEN)        // If overload is detected when the door is open it terminates the closing function and enters overload_detected state
    {
        nextState = overload_detected;
        return;
    }
    
    door.closing();                     

    if (doorState == CLOSED)
    {
        screen.printDoor(doorState);
        clearRequest(currentFloor);       // When door is closed, go to the next state, idle
        nextState = idle;
    }
}



void ElevatorControl::overload()        // Defining overload() function inside ElevatorControl class
{
    lastMillis = millis();

    while (overloadDetected())
    {
        screen.overloadMessage();       // As long as overloadDetected() is true it shows a overload message on screen, disabling all other functions until overloadDetected() goes false
    }

    nextState = door_closing;
    screen.turnOnBacklight();           // Returns to door_closing state when overload is not present anymore
    screen.printFloor(currentFloor);
}



void ElevatorControl::elevatorMoving()    // Defining elevatorMoving() function inside ElevatorControl class
{
    if (door.senseClosed())               // Only move if door is closed
    {
        if (currentDirection == UP)
        {
            screen.printDirection(UP);
        }
        else
        {
            screen.printDirection(DOWN);      // Print direction on display
        }
        
        bool slowStart = true;                // Logic variable to control on wheather the PID loop is in a start point starting the slowstart operations further down
        float slowstartSetpoint = 0;
        
        int sampleTime = 10;                  // Our sample time in the PID controller
        float motorPWM;
    
        long setpointPosition = floorPosition[updateDestination()];     // The destination setpoint gets calculated by the updateDestination() function
        float setpointVelocity = 0;
    
        float limit1 = floorPosition[1] - floorPosition[1] / 3;   // Position limiters. When elevator is heading from one end-floor to another, requests in second has to
        float limit2 = floorPosition[1] + floorPosition[1] / 3;   // be called within a certain position for the elevator to make a smooth stop in second floor befor continuing.
                                                                  // If request is too slow, it will go to and stop in second floor after it has serviced end floor (1st or 3rd).

                                                                                          
        //Position PID Variables:
        float feedbackPosition = cage.sensePosition();          // Telling the PID controller where the motor is right now
        
        float error_pos;                // Position error
        float kp_pos = 0.2;             // Proportional gain, position (outer loop)
        float ki_pos = 0.001;           // Integral gain, position (outer loop)
        float kd_pos = 2;               // Derivative gain, position (outer loop)
        
        float output_pos = 0.0;     // Output from the position controller (outer loop) that gets directly fed into the velocity controller
        
        float errorTotal_pos = 0;
        float lastError_pos;
        float proportional_pos;       // Variables for the outer loop PID calculation
        float integral_pos;
        float derivative_pos;

        float integralActiveZone_pos = 500;     // A predefined zone on where we want the integrator to "kick" in


        
        //Velocity PID Variables
        float feedbackVelocity = 0.0;     // Telling the PID controller what the current velocity is
        
        float error_vel;                  // Velocity error
        float kp_vel = 11;                // Proportional gain, velocity (inner loop)
        float ki_vel = 0.5;               // Integral gain, velocity (inner loop)
        float kd_vel = 0;                 // Derivative gain, velocity (inner loop) - not used in this controller, but not removed in case later application needs it
        
        float output_vel = 0.0;     // Output from the velocity controller (inner loop), gets mapped to a suitable PWM value and sent to DC-motor
        
        float errorTotal_vel = 0;
        float lastError_vel;
        float proportional_vel;       // Variables for the inner loop PID calculation
        float integral_vel;
        float derivative_vel;
     
        float integralActiveZone_vel = 10;      // Same predefined zone as the one above, only this is for the velocity controller



        /*******************************************PID LOOP***********************************************/
        
        // Keeps inside PID loop calculating as long as we're not in emergency state and until the actual position is equal to the setpoint +- about 0.1% of one full rotation
        while (emergStop() && (feedbackPosition < setpointPosition - 8 || feedbackPosition > setpointPosition + 8))
        {
            buttons.checkInFloor();                                 // Checks if any of the direction-calling buttons in one of the floorlevels is pressed while moving
            buttons.checkInCage();                                  // Checks if any of the buttons inside the elevator is pressed while moving
            
            if (feedbackPosition < limit1 && currentDirection == UP)
            {
                setpointPosition = floorPosition[updateDestination()];     // Updates setpoint if any of the call-buttons is pressed within the position limiter upwards
            }
            if (feedbackPosition > limit2 && currentDirection == DOWN)
            {
                setpointPosition = floorPosition[updateDestination()];     // Updates setpoint if any of the call-buttons is pressed within the position limiter downwards
            }
            
            Serial.print("Destination setpoint: ");
            Serial.print(setpointPosition);
            Serial.print("\t\t");
            Serial.print("Current position: ");
            Serial.print(feedbackPosition, 0);                // Turn on serial monitor to see position setpoint, actual position and error displayed live
            Serial.print("\t\t");
            Serial.print("Error: ");
            Serial.println(error_pos);
            
            if (checkTime(lastMillis, sampleTime))      // Sampling
            {    
                lastMillis = millis();

                feedbackPosition = cage.sensePosition();            // Angular position measured in encoder ticks (both needs to be inside loop to update continuously)
                feedbackVelocity = cage.senseDeltaPosition();       // Angular velocity measured in encoder ticks per 10 milliseconds as it is the current sample time         

                
                //Position Controller
                error_pos = setpointPosition - feedbackPosition;    // Calculating the current error between destination and actual position
        
                if (error_pos < 0)       // When we go downwards from an upper floor we will get a negative error, this results in complications to the integral zone if we don't make some changes
                {
                    if ((error_pos > -integralActiveZone_pos) && error_pos != 0)
                    {
                        errorTotal_pos  += error_pos;       // With negative error we want the error to be above the negative integral zone to be active
                    }
                    else
                    {
                        errorTotal_pos  = 0;                // If not total error, and thus the whole integrator term, turns to zero
                    }
                }
                else
                {
                    if ((error_pos < integralActiveZone_pos) && error_pos != 0)   // Positive error means other way around. Error should be less than active zone for the integral to kick in
                    {
                        errorTotal_pos  += error_pos;
                    }
                    else
                    {
                        errorTotal_pos  = 0;
                    }
                }
            
                if (errorTotal_pos > 50.0/ki_pos)         // The integrator term can be problematic as an integral quickly can become very big, this if-statement together with the active-zone
                {                                         // above keeps the integrator term on a leash.
                    errorTotal_pos = 50.0/ki_pos;
                }
            
                if (error_pos == 0)
                {
                    derivative_pos = 0;                   // If the error turns to zero we want to stop derivating
                }
            
            
                proportional_pos    =     error_pos                         *kp_pos;
                integral_pos        =     errorTotal_pos                    *ki_pos;    // Complete the calculations of each term
                derivative_pos      =     (error_pos - lastError_pos)       *kd_pos;
            
                lastError_pos = error_pos;    // Update this error as last error to use next time derivating
            
                output_pos = proportional_pos + integral_pos + derivative_pos;        // Adding them all together, creating a output signal
        
                if (output_pos < -1020)
                {
                    output_pos = -1020;
                }
                if (output_pos > 1020)    // Limiting output signal
                {
                    output_pos = 1020;
                }
        
        
                if (slowStart == true)      // As it is an elevator it should not start with full speed
                {
                    if (output_pos > 0)
                    {
                        slowstartSetpoint += 0.3;
                        if (slowstartSetpoint >= 50)        // We want a smooth ramp signal up to marching speed. This counter makes it go slowly from zero to desired speed 50 in a nice pace
                        {
                            slowStart = false;
                        }
                    }
                    else if (output_pos < 0)
                    {
                        slowstartSetpoint -= 0.3;
                        if (slowstartSetpoint <= -50)       // Exactly the same as the one above, except this is for slowstart with negative error, downwards movement
                        {
                            slowStart = false;
                        }
                    }
                    setpointVelocity = slowstartSetpoint;   // Here we set the slowstartSetpoint, which gets updated until desired speed, as the setpoint for velocity controller
                }
                else
                {
                    setpointVelocity = map(output_pos, -1020, 1020, -50, 50);   // Mapping output after slowstart to be maximum marching speed in either of the directions
                }


                
                
                //Velocity Controller
                error_vel = setpointVelocity - feedbackVelocity;
        
                if (error_vel < 0)
                {
                   if ((error_vel > integralActiveZone_vel) && error_vel != 0)                     // Exactly the same happens in this controller as the one above
                    {
                        errorTotal_vel  += error_vel;
                    }
                    else
                    {
                        errorTotal_vel  = 0;
                    } 
                }
                else
                {
                    if ((error_vel < integralActiveZone_vel) && error_vel != 0)
                    {
                        errorTotal_vel  += error_vel;
                    }
                    else
                    {
                        errorTotal_vel  = 0;
                    }
                }
                
                if (errorTotal_vel > 50.0/ki_vel)
                {
                    errorTotal_vel = 50.0/ki_vel;
                }
            
                if (error_vel == 0)
                {
                    derivative_vel = 0;
                }
            
                proportional_vel    =     error_vel                           * kp_vel;
                integral_vel        =     errorTotal_vel                      * ki_vel;
                derivative_vel      =     (error_vel - lastError_vel)         * kd_vel;
            
                lastError_vel = error_vel;
            
                output_vel = proportional_vel + integral_vel + derivative_vel;
        
                if (output_vel < -255)
                {
                    output_vel = -255;      // Limiting ouput values from velocity controller
                }
                if (output_vel > 255)
                {
                    output_vel = 255;
                }
        
                motorPWM = map(output_vel, -255, 255, 255, 0);    // Mapping ouput values to be suitable as pwm values and send them to the motor. 
                
                cage.motorRotate(motorPWM);
                
            }
        }
        cage.motorStop();                         // After PID loop is done, meaning we have stopped in a floor or emergency button is pressed, we turn off the motor
        
        if (currentDirection == DOWN)
        {
            prevDirection = DOWN;
        }
        if (currentDirection == UP)             // Updates the previous direction variable for next time calculating destination based on requests
        {
            prevDirection = UP;
        }

        currentFloor = checkFloor();
        screen.printFloor(currentFloor);        // Check what floor we've stopped in and print to display
        
        if (emergStop())
        {
            nextState = door_opening;           // If we're not going into the emergency state, open the doors.       
        }
        
        lastMillis = millis();
    }
}



void ElevatorControl::emergency()         // Defining emergency() function inside ElevatorControl class
{
    int emergDelay = 1500;                // A small delay after cage has stopped befor it starts moving downwards
    int motorPWM = 145;                   // PWM signal to drive the motor. 127 is not moving and >127 is downwards movement
    bool keepBlinking = true;
    bool goDown = true;
    
    clearRequest(0);
    clearRequest(1);            // Clearing all current requests
    clearRequest(2);

    screen.emergencyStatus();           // Display emergency status on the display

    float lastMillisEmerg = millis();

    while (keepBlinking)                      // This is the emergency loop. LED's will blink one after one from left to right at the same time as elevator goes slowly down to closest floor
    {
        buttons.emergBlinkLED(true);

        if (checkTime(lastMillisEmerg, emergDelay) && goDown)       // Elevator goes slowly downwards after delay
        {
            cage.motorRotate(motorPWM);
            goDown = false;
        }
        
        if (cage.sensePosition() >= floorPosition[0] - 50 && cage.sensePosition() <= floorPosition[0] + 50)             
        {
            cage.motorStop();
            buttons.emergBlinkLED(false);
            keepBlinking = false;
            currentFloor = 0;
            nextState = door_opening;
        }
  
        if (cage.sensePosition() >= floorPosition[1] - 50 && cage.sensePosition() <= floorPosition[1] + 50)           // Stops at the first of these three if-statements (floors)
        {
            cage.motorStop();
            buttons.emergBlinkLED(false);
            keepBlinking = false;
            currentFloor = 1;
            nextState = door_opening;
        }

        if (cage.sensePosition() >= floorPosition[2] - 50 && cage.sensePosition() <= floorPosition[2] + 50)
        {
            cage.motorStop();
            buttons.emergBlinkLED(false);
            keepBlinking = false;
            currentFloor = 2;
            nextState = door_opening;
        }
    }
    screen.printFloor(currentFloor);
    lastMillis = millis();
}



long int ElevatorControl::getFloorPosition(int floorNumber)           // Function to get the encoder positions of the floor inside ElevatorControl class
{
    return floorPosition[floorNumber];
}
