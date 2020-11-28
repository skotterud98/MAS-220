# MAS-220
Elevator project 2020

Upload and run program:  

-   Open ElevatorMainProgram folder and run ElevatorMainProgram.ino file in Arduino IDE  
-   This will automatically open all the other files in the same folder as tabs in the sketch  
-   Press compile and upload with the servo-lab unit connected to bluetooth or a USB-port    
  
  
If encoder positions to the floors needs to be changed to match distance between floors, this can be done inside the floorPosition[] array in the private part of the class ElevatorControl in ElevatorMainProgram.ino file. Default positions is set to: 
-   first floor at 0
-   2nd floor at 16640
-   3rd floor at 33280  
  
  
The 4 leftmost buttons (7-4) on the servo-lab unit represents the 4 floor buttons where you got UP first floor, DOWN second floor, UP second floor and DOWN third floor.  
  
  
Next 3 buttons (3-1) represents the three floor buttons inside the elevator "cage", where button #3 is first floor, button #2 is second floor and button #1 is third floor.
  
  
There is also a "load sensor" which is simulated by the anlalog potmeter knob on the right. Turn it clockwise, when the door is open, and it will run in to overload state as the weight passes 800 kg. Turn the knob counter clockwise to go out of overload state and proceed.
  
  
Emergency state is activated by pressing down the left encoder knob when elevator is moving. This is connected with an interrupt entering the emergency state, which stops all movement, clears all requests and moves downwards to the nearest floor.
  
  A video of demonstration can be seen here: https://www.youtube.com/watch?v=OoV9yVkWrEY&feature=youtu.be
