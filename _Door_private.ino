
void Door::dac_ON(bool on)
{
    bool dacBitsA[16] = {0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};     // Initializing values for the databits in channel A and B on Digital to Analog Converter 
    bool dacBitsB[16] = {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        
    if (on == true)
    {
        for (int i = 5; i <= 15; i++)     // Setting the data value in the last 12 bits on the two channels to 0b011111111111, which is similar to 2047 in decimalnumbers, giving us 50 % current 
        {
            dacBitsA [i] = 1;
            dacBitsB [i] = 1;
        }
    }
    else
    {
        for (int i = 4; i <= 15; i++)     // Setting the data value in the last 12 bits on the two channels to 0b000000000000
        {
            dacBitsA [i] = 0;
            dacBitsB [i] = 0;
        }
    }
    
    digitalWrite (CS, LOW);
  
    for (int i = 0; i <= 15; i++) 
    {
      digitalWrite (SCK, LOW);
      digitalWrite (SDI, dacBitsA [i]);         // For-loop for setting the 16 bit-signals to SDI pin, channel A. This could also be done with bitwise operators
      digitalWrite (SCK, HIGH);
    }
    digitalWrite (CS, HIGH);
    
  
    digitalWrite (CS, LOW);
    
    for (int i = 0; i <= 15; i++) 
    {
      digitalWrite (SCK, LOW);
      digitalWrite (SDI, dacBitsB [i]);         // For-loop for setting the 16 bit-signals to SDI pin, channel B
      digitalWrite (SCK, HIGH);
    }
  
    digitalWrite (CS, HIGH);
  
    digitalWrite (LDAC, LOW);
}
