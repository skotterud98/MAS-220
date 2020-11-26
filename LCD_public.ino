
LCD::LCD()
{
    lcd.begin (16, 2);                // Starting up the already declared Liquid Crystal object, telling it that the output screen has 16 colums and 2 rows
    
    lcd.createChar(0, upArrow);       
    lcd.createChar(1, downArrow);     // Creating the different arrows used on the display, bit values for the arrows lies under private in class
    lcd.createChar(2, leftArrow);
    lcd.createChar(3, rightArrow);
}


void LCD::printFloor(int floorStatus)                 // Printing to the upper row of the display what floor the elevator is in
{
    switch(floorStatus)
    {
        case 0:
            lcd.setCursor(0, 0);
            lcd.print("   1st Floor    ");
            break;
        case 1:
            lcd.setCursor(0, 0);
            lcd.print("   2nd Floor    ");
            break;
        case 2:
            lcd.setCursor(0, 0);
            lcd.print("   3rd Floor    ");
            break;

        default:
            break;
    }
}


void LCD::printDoor(int doorStatus)                   // Printing to the lower row of the display what state the door is in, both text and arrows
{
    switch (doorStatus)
    {
          case OPEN:
              lcd.setCursor(0, 1);
              lcd.print("   DOOR OPEN    ");
              break;
              
          case CLOSED:
              lcd.setCursor(0, 1);
              lcd.print("   DOOR CLOSED  ");
              break;
          
          case OPENING:
              lcd.setCursor(0, 1);
              lcd.write(byte(2));
              lcd.print(" Door Opening ");
              lcd.write(byte(3));
              break;

          case CLOSING:
              lcd.setCursor(0,1);
              lcd.write(byte(3));
              lcd.print(" Door Closing ");
              lcd.write(byte(2));
              break;

          default:
              break;
    }
}


void LCD::printDirection(int dirStatus)               // Printing to the lower row of the display what direction the elevator is going in while moving, both text and arrows
{
    switch(dirStatus)
    {
        case UP:
            lcd.setCursor(0, 1);
            lcd.write(byte(0));
            lcd.print("   Going Up   ");
            lcd.write(byte(0));
            break;

        case DOWN:
            lcd.setCursor(0, 1);
            lcd.write(byte(1));
            lcd.print("  Going Down  ");
            lcd.write(byte(1));
            break;

         default:
            break;
    }
}



void LCD::emergencyStatus()
{
    lcd.setCursor(0, 0);
    lcd.print(" EMERGENCY STOP ");              // Printing the emergency message on top of the screen together with a direction statement on lower part
    
    lcd.setCursor(0, 1);
    lcd.write(byte(1));
    lcd.print("  Going Down  ");
    lcd.write(byte(1));
}



void LCD::overloadMessage()
{
    int displayBlinkDelay = 80;
    
    lcd.setCursor(0, 0);
    lcd.print("    OVERLOAD    ");                                                      // When overload is detected it displays message on screen at the same time as blinking the backlight
    lcd.setCursor(0, 1);
    lcd.print("   MAX 800 KG   ");

    if (checkTime(lastMillis, displayBlinkDelay) && blinkSequence == 0)
    {
        lastMillis = millis();
        analogWrite(backlight, 100);
        blinkSequence = 1;
    }
    if (checkTime(lastMillis, displayBlinkDelay) && blinkSequence == 1)
    {
        lastMillis = millis();
        analogWrite(backlight, 200);
        blinkSequence = 0;
    }
}



void LCD::turnOnBacklight()
{
    digitalWrite(backlight, HIGH);          // Function to turn on the backlight
}
