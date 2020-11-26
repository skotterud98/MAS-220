
#include <LiquidCrystal.h>  // Built-in library to easily print to a Liquid Crystal Display (LCD)



// ***********************************************global variables, constants & functions********************************************************************** //

enum door {OPEN, CLOSED, OPENING, CLOSING};
enum request {IDLING, REQUEST, UP, DOWN, BOTH};
enum states {idle, elevator_down, elevator_up, door_opening, door_closing, overload_detected, emergency_protocol};


states nextState            = idle;                   // Variable of enumerator type "states" to assign the upcoming state in the state machine, initially in idle state
door doorState              = CLOSED;                 // Variable of enumerator type "door" to store the current state of the door, initially CLOSED
request buttonRequest       = IDLING;                 // Variable of enumerator type "request" to store a button request, initially IDLING
request prevDirection       = IDLING;                 // Variable of enumerator type "request" to store the previous direction of the elevator, initially IDLING
request currentDirection    = IDLING;                 // Variable of enumerator type "reauest" to store the current direction of the elevator, initially IDLING


int   floorRequest[3][2]   =   {    {buttonRequest, buttonRequest},           // Simple queue-array to handle requests
                                    {buttonRequest, buttonRequest},           // where the rows and columms is as followed:
                                    {buttonRequest, buttonRequest}    };      // [  floornumber(0, 1, 2)  ]     [   in elevator requests(0),    at floor up/down requests(1)   ]


unsigned long lastMillis        = 0;              // Variable to store last measured milliseconds
unsigned long currentMillis     = 0;              // Variable to stor currently measures milliseconds

bool checkTime(unsigned long int lastMillisInput, int wait);                        // Function to execute timed operations without the use of delay(), resulting in a more non-blocking code

const byte rs = 41, en = 40, d4 = 37, d5 = 36, d6 = 35, d7 = 34, backlight = 4;     // LCD screen outputs
LiquidCrystal   lcd(rs, en, d4, d5, d6, d7);                                        // Initializing the Liquid Crystal display with creating an object with the LCD pins as parameters



// *******************************************************class declarations*********************************************************************************** //

class Buttons
{
    public:
        Buttons();                              // Buttons Class Constructor, determine pinModes
        void turnOffLED(int floorNumber);       // Turns off all the request indicating LED's in a floor
        void checkInFloor();                    // Checks if a button is pressed in one of the floors, lights up their LED and store the request in the global request array
        void checkInCage();
        void emergBlinkLED(bool ON);
        
    private:
        static void ISR_EMERG();                // Interrupt Service Routine for the emergency button
        
        const byte upButton[2]            = {29, 27};             // Up in first floor, Up in second floor
        const byte downButton[2]          = {28, 26};             // Down in second floor, Down in third floor
        const byte upButtonLED[2]         = {42, 44};             // Up in first floor, Up in second floor (LED)
        const byte downButtonLED[2]       = {43, 45};             // Down in second floor, Down in third floor (LED)
        const byte cageButton[3]          = {25, 24, 23};         // First, Second, Third Floor
        const byte cageButtonLED[4]       = {46, 47, 48, 49};     // First, Second, Third Floor (LED) + LED number 0
        const byte emergencyButton        = 2;                    // Button start the emergency protocol and go slowly down to nearest floor

        int blinkSequence = 0;
};



class Door
{
    public:
        Door();                 // Door Class Constructor, determines pinModes
        
        void opening();         // Opens door
        void closing();         // Close door
        bool senseClosed();     // "sense" if the door is closed and returns <true> if closed, <false> if open  

    private:
        void dac_ON(bool on);
        
        const byte SDI = 31, SCK = 32, CS = 33, LDAC = 30;                    // Pins for the Digital to Analog Converter which has to be used to get current through the windings of the stepper
        const byte EnableA = 69, EnableB = 67, PhaseA = 68, PhaseB = 66;      // Pins for the stepper motor simulating the door motors

        int stepSequence    = 0;        // Logic variable to use when moving through the 8 steps with checkTime() function
        int stepCount       = 0;        // Counter that counts total number of steps where one iteration of the 8 different steps is counted as one step
        bool wait           = true;     // Logic variable used in both closing() and opening() function to have a timer before stepping starts, without having to wait for the timer for each step
};



class Cage
{
    public:
        Cage();                             // Cage Class Constructor, determines pinModes
        
        void motorRotate(float power);      // Rotate the DC-motor in main hoist with a PWM value as input
        void motorStop();                   // Stops the elevator

        float senseDeltaPosition();         // Sense the change in encoder positions
        float sensePosition();              // Sense the current encoder position
        float senseLoad();                  // Sense current load inside cage

    private:
        static void ISR_A();                // Interrupt Service Routine on encoder, channel A
        static void ISR_B();                // Interrupt Service Routine on encoder, channel B

        const byte Enable = 7, Phase = 6, Decay = 5;    // Pins for the DC-motor simulating the main hoist motor
        const byte loadSensor           = A0;           // Potensiometer on servolab-unit simulating a loadsensor
        static const byte encoderPinA   = 20;           // Encoder pin A on DC-motor
        static const byte encoderPinB   = 21;           // Encoder pin B on DC-motor

        static long protectedCount;                     // A protected counter to count the encoder ticks inside the ISR where no interrupts is possible at the same time
        static volatile long encCount;                  // The actual encoder counter that counts up or down depending on encoder ticks
        static long previousCount;                      // Previous count variable to be used to measure a difference in ticks over a certain amount of time (speed)
};



class LCD
{
    public:
        LCD();                                          // LCD class constructor, starts up the Liquid Crystal library and creates the arrows used on the display

        void printFloor(int floorStatus);               // Prints floornumber input to display
        void printDoor(int doorStatus);                 // Prints input state of the door (opening, closing, open, closed) to display
        void printDirection(int dirStatus);             // Prints input moving direction (up/down)
        void emergencyStatus();                         // Shows the emergency message on display
        void overloadMessage();                         // Shows the overload message on display
        void turnOnBacklight();                         // Turns on display backlight
    
    private:
        byte upArrow[8]       = {B0,     B0,     B100,   B1110,  B11111, B0,     B0,     B0};       // Variables including the bitvalues in binary creating different arrows to display on screen
        byte downArrow[8]     = {B0,     B0,     B11111, B1110,  B100,   B0,     B0,     B0};
        byte leftArrow[8]     = {B10,    B110,   B1010,  B10010, B1010,  B110,   B10,    B0};
        byte rightArrow[8]    = {B1000,  B1100,  B1010,  B1001,  B1010,  B1100,  B1000,  B0};

        int blinkSequence = 0;                          // Logic variable used to keep track on what LED that should blink when entering emergency state
};



class ElevatorControl
{
    public:
        void idling();                
        void elevatorMoving();
        void doorOpening();               // The six main state-functions
        void doorClosing();
        void overload();
        void emergency();

        long int getFloorPosition(int floorNumber);         // Function that returns the encoder postion number of a floornumber input
        
    private:
        void checkDirection();                              // Calculates what direction to go based on one or several requests
        bool anyRequests();                                 // Checks if there are any requests present, return true if there is
        void clearRequest(int floorNumber);                 // Clear all requests in a floor
        int updateDestination();                            // Calculates destination continously in the PID control loop in case requests are made while moving, updates destination if so
        int checkFloor();                                   // Checks what floor the cage is currently idling in
        bool emergStop();                                   // Stop function that returns false if program enters the emergency state, terminating PID loop
        bool overloadDetected();

        Cage        cage;
        Door        door;             // Create local objects to implement the other classes in main control class, instead of using inheritance. This is to be able to move a lot of the code 
        Buttons     buttons;          // from setup() to the different class constructors wich will be executed once these objects are created
        LCD         screen;

        long int floorPosition[3] = {0, 16640, 33280};      // Encoder positions to the three floors. These can be changed to whatever here and the rest of the code will still work
        byte currentFloor = 0;                              // Variable to store floornumber the elevator is currently idling/staying in    
        bool atFloor;                                       // Logic variable to change the precision of the condition inside PID loop.
};                                                          // Two full rotations between each floor. 64 ticks x 130 gear ratio = 8320 pulses per rotation



// ********************************************************************global objects************************************************************************* //

ElevatorControl StateMachine;     // Creating a main-object for the ElevatorControl class which includes our State Machine functions


// **********************************************************************setup & loop************************************************************************* //

void setup() 
{
    Serial.begin(115200);
    TCCR4B = TCCR4B & 0b11111000 | 0x01;                  // Changing the PWM frequence from 490Hz to 32kHz, resulting in smoother control of the DC-motor and less noise
}



void loop() 
{   
    switch (nextState)                                    // Switch case function to determine what state the Elevator Controller should be in
    {
          case idle:
              currentDirection = IDLING;                  // Sets the global variable CurrentDirection to be "IDLING"
              StateMachine.idling();                      // Executes idling() function on the global object StateMachine of ElevatorControl class
              break;
          
          case elevator_down:
              currentDirection = DOWN;                    // Sets the global variable CurrentDirection to be "DOWN"
              StateMachine.elevatorMoving();              // Executes elevatorMoving() function on the global object StateMachine of ElevatorControl class
              break;
          
          case elevator_up:
              currentDirection = UP;                      // Sets the global variable CurrentDirection to be "UP"
              StateMachine.elevatorMoving();              // Executes elevatorMoving() function on the global object StateMachine of ElevatorControl class
              break;

          case door_opening:
              StateMachine.doorOpening();                 // Executes doorOpening() function on the global object StateMachine of ElevatorControl class
              break;

          case door_closing:
              StateMachine.doorClosing();                 // Executes doorClosing() function on the global object StateMachine of ElevatorControl class
              break;

          case overload_detected:
              StateMachine.overload();                    // Executes overload() function on the global object StateMachine of ElevatorControl class
              break;

          case emergency_protocol:
              StateMachine.emergency();                   // Executes emergency() function on the global object StateMachine of ElevatorControl class
              break;

          default:
              break;
    }
}



// *****************************************************global function definitions***************************************************************************** //

bool checkTime(unsigned long int lastMillisInput, int wait)     
{  
  currentMillis = millis();
  
  if (currentMillis - lastMillisInput >= wait)                                // Timer function instead of using delay(), calculates the difference in current milliseconds
  {                                                                           // and previous milliseconds input, then goes "true" when it exceeds wait time input.  
      return true;                                                            // This makes it easy to use timer without blocking the code with a delay().
  }
  return false;
}
