#include<Servo.h>

Servo VERT_R; // vertical move
Servo VERT_L; // vertical move
Servo SWNG_R; // swing move
Servo SWNG_L; // swing

int unoFS;
int unoRS;
int unoLS;
int unoBS;

void setup() {
  Serial.begin(115200);

  unoFS = 0;
  unoRS = 0;
  unoLS = 0;
  unoBS = 0;

  VERT_R.attach(12);
  VERT_L.attach(13);

  SWNG_R.attach(2);
  SWNG_L.attach(3);

  Serial.println("SETUP - DONE.");
  delay(500);
  Serial.println("INITIALISING...");
  SWNG_R.write(25);
  SWNG_L.write(25);
  VERT_R.write(30);
  VERT_L.write(150);
  delay(1000);
  Serial.println("STATUS: READY.");
  delay(1000);
  VERT_R.write(30);
  VERT_L.write(150);
  SWNG_R.write(110);
  SWNG_L.write(110);
  delay(500);
}

void tars(int nStatus) // fast
{
  switch(nStatus)
  {
    case 1: // down
      VERT_R.write(0);
      VERT_L.write(180);
      break;

    case 2: // middle
      VERT_R.write(30);
      VERT_L.write(150);
      break;
    
    case 3: // up
      VERT_R.write(60);
      VERT_L.write(120);
      break;



    case 4: // back
      SWNG_R.write(140);
      SWNG_L.write(80);
      break;

    case 5: // middle
      SWNG_R.write(110);
      SWNG_L.write(110);
      break;
    
    case 6: // front
      SWNG_R.write(80);
      SWNG_L.write(140);
      break;
  }
}

// up - swng back - down - swng mid

void frontSwing(int nRepeat)
{
  if(unoFS == 0)
  {
    for(int i = 0; i < nRepeat; i++)
    {
      tars(1);
      delay(300);
      tars(6);
      delay(300);
      tars(3);
      delay(300);
      tars(4);
      delay(500);
    }
    tars(5);
    unoFS = 1;
  }
}

void backSwing(int nRepeat)
{
  if(unoBS == 0)
  {
    for(int i = 0; i < nRepeat; i++)
    {
      tars(1);
      delay(300);
      tars(4);
      delay(300);
      tars(3);
      delay(300);
      tars(6);
      delay(500);
    }
    tars(5);
    unoBS = 0;
  }
}

void rightSwing(int nRepeat)
{
  if(unoRS == 0)
  {
    for(int i = 0; i < nRepeat; i++)
    {
      tars(1);
      delay(300);
      SWNG_L.write(140);
      SWNG_R.write(140);
      delay(300);
      tars(3);
      delay(300);
      SWNG_L.write(80);
      SWNG_R.write(80);
      delay(500);
    }
    tars(5);
    unoRS = 1;
  }
}

void leftSwing(int nRepeat)
{
  if(unoLS == 0)
  {
    for(int i = 0; i < nRepeat; i++)
    {
      tars(1);
      delay(300);
      SWNG_R.write(80);
      SWNG_L.write(80);
      delay(300);
      tars(3);
      delay(300);
      SWNG_R.write(140);
      SWNG_L.write(140);
      delay(500);
    }
    tars(5);
    unoLS = 1;
  }
}

void blueToothReciever()
{
  static word old_command = 0;
  word command = KitRS_RemoconRead();
  if(command != 0xFFFF) 
  {
    if(old_command != command)  
    {
      old_command = command;

      switch(command) 
      {
        case 0x1000: frontSwing(1);              break;
        case 0x4000: backSwing(1);               break;
        case 0x8000: leftSwing(1);               break;
        case 0x2000: rightSwing(1);              break;
        case 0x0010: frontSwing(1);              break; 
        case 0x0040: backSwing(1);               break; 
        case 0x0080: leftSwing(1);               break; 
        case 0x0020: rightSwing(1);              break; 
        case 0x0004: Serial.println(command);    break; 
        case 0x0008: Serial.println(command);    break; 
        case 0x0000: unoFS=0; unoBS=0; unoRS=0; unoLS=0;  break;  
      }
    }
  }
}


void loop() {
  blueToothReciever();
}


// ****************************** BLUETOOTH(controller) ******************************

  word KitRS_RemoconRead()  
  {
    static byte Check_Remocon_Index = 0;
    static byte Remocon_Buffer[3];
  
    while(Serial.available() > 0) 
    {
      byte ch = Serial.read();
      switch(Check_Remocon_Index) 
      {
        case 0: //header 1
          if(ch == 0xAA)  Check_Remocon_Index++;  break;
        case 1: //header 2
          if(ch == 0x55)  Check_Remocon_Index++;
          else            Check_Remocon_Index = 0;  break;
        case 5: //Checksum
          Check_Remocon_Index = 0;
          {
            byte checksum = Remocon_Buffer[0];
            checksum += Remocon_Buffer[1];
            checksum += Remocon_Buffer[2];
            if(checksum == ch)  {
              word com = Remocon_Buffer[0];
              com <<= 8;
              com |= Remocon_Buffer[1];
              //Remocon_Buffer[2];          //always 0x00
              return com;
            }
            else  return 0xFFFF;
          }
        default:  //2~4
          if(ch == 0xAA)  Check_Remocon_Index = 1;
          else  
          {
            Remocon_Buffer[Check_Remocon_Index -2] = ch;
            Check_Remocon_Index++;
          }
          break;
        }
     }
    return 0xFFFF;
  }

// ****************************** BLUETOOTH(controller) ******************************

