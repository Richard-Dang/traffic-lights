#include <Servo.h>
/*  Long = 2 way street (2 sets of lights). Intersect = 1 way lane that T-intersects (1 set of lights).
    13: long red
    12: long yellow
    11: long green
    10: intersect red
    9: intersect yellow
    8: intersect green
    7: pedestrian green
    6: pedestrian red
*/

Servo gate;
void setup ()
{
  Serial.begin (9600);
  gate.attach (3);
  for (int i = 13; i >= 5; i --)
    pinMode (i, OUTPUT);
  gate.write (0);
}

void allOff ()
{
  for (int i = 13; i >= 6; i --)
    digitalWrite (i, LOW);
}

int ledDelays[] = {8000, 2000, 8000, 2000};
int led[4][3] = {{13, 8, 6}, {13, 9, 6}, {11, 10, 7}, {12, 10, 6}};
long ledTimer = 0;
long resetTimer = 0;
boolean reset = false;
int stage = 1; 


int LDR;
int button;
int IR;

long gateTimer = 0;
long closeTimer = 0;
int pos = 0;
boolean opening = false;

void loop ()
{
  unsigned long currentTime = millis ();
  LDR = analogRead (A0);
  button = analogRead (A1);
  IR = analogRead (A2);
    
  if (LDR <= 75) //Street light control based on Light Dependant Resistor
    digitalWrite (5, HIGH);
  else
    digitalWrite (5, LOW);
    
  if (currentTime - ledTimer > ledDelays [stage-1]) // If the previous time 
  {
    stage += 1;
    ledTimer = currentTime;
  }
  if (button == 1023 && stage != 2 && stage != 4)
  {
    resetTimer = currentTime;
    reset = true;
  }
  
  if (reset == true && currentTime - resetTimer > 2000)
  {
    if (stage == 1 || stage == 2)
      stage = 2;
    if (stage == 3 || stage == 4)
      stage = 4;
    ledTimer = currentTime;
    reset = false;
  }
  if (stage > 4)
    stage = 1;
    
  switch (stage)
  {
    case 1: //Long: red, intersect: green, pedestrian: red
      allOff ();
      digitalWrite (13, HIGH);
      digitalWrite (8, HIGH);
      digitalWrite (6, HIGH);
      break;
    case 2: //Long: red, intersect: yellow, pedestrian: red
      allOff ();
      digitalWrite (13, HIGH);
      digitalWrite (9, HIGH);      
      digitalWrite (6, HIGH);
      break;    
    case 3: //Long: green, intersect: red, pedestrian: green
      allOff ();
      digitalWrite (11, HIGH);
      digitalWrite (10, HIGH);
      digitalWrite (7, HIGH);
      break;
    case 4: //Long: yellow, intersect: red, pedestrian: red
      allOff ();
      digitalWrite (12, HIGH);
      digitalWrite (10, HIGH);
      digitalWrite (6, HIGH);
      break;
  }
  
  if (pos >= 91)
  {
    opening = false;
    pos -= 1;
    closeTimer = currentTime;
  }
  
  if (IR == 0 && opening == false)
    opening = true;

  if (opening == true)
  {
    if (currentTime - gateTimer > 5 && pos <= 90)
    {
      pos +=1;
      gateTimer = currentTime;
    }
  }
  else
  {
    if (pos > 0 && currentTime - gateTimer > 20 && IR != 0 && currentTime - closeTimer > 2000)
    {
      pos -= 1;
      gateTimer = currentTime;
    }
  }
  gate.write (pos);

  Serial.print ("LDR: ");
  Serial.print (LDR);
  Serial.print (" Button: ");
  Serial.print (button);
  Serial.print (" IR: ");
  Serial.println (IR);
}
