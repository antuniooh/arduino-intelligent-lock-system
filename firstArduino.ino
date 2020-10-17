#include <Servo.h>
#include <Keypad.h>

Servo windowLeft;
Servo windowRight;
Servo door;

int light = 0;
float temperature = 0;
int sound = 220;
int gas = 0;
bool isOpen = false;
bool alarmOn = false;
bool doorOpen = false;

String password = "1234";
String insertPassword = "";
bool access = false;

const byte numRows= 4;
const byte numCols= 4; 

char keymap[numRows][numCols]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

byte rowPins[numRows] = {9,8,7,6}; 
byte colPins[numCols]= {5,4,3,2}; 

Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

void setup()
{
  windowLeft.attach(11);
  windowRight.attach(10);
  door.attach(12);
  
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  Serial.begin(9600);
    
  door.write(0);
}

void calcTemperature()
{
  temperature = analogRead(A0);
  temperature = map(temperature,20,350,-40,125);
}

void calcLight()
{
  light = analogRead(A1);
}

void onAlarm()
{
  analogWrite(A3, 1024);
  analogWrite(A4, sound);
  alarmOn = true;
}

void offAlarm()
{
  analogWrite(A3, 0);
  analogWrite(A4, 0);
  alarmOn = false;
}

void calcGas()
{
   gas = analogRead(A2); 
}


void getAllValues()
{
  calcLight();
  calcTemperature();
  calcGas();
}

void openWindow()
{
  windowLeft.write(90);
  windowRight.write(90);
  isOpen = true;
  Serial.write("JanelaAberta",12); //Write the serial data
}

void closeWindow()
{
  windowLeft.write(0);
  windowRight.write(0);
  isOpen = false;
  Serial.write("JanelaFechada",13); //Write the serial data
}

void checkWindow()
{
  getAllValues();
  
  if(digitalRead(13) == HIGH && access)
    changeDoor();

  if(isOpen)
  {
    //Serial.println("Está aberta");
    //situação de perigo
    if(gas > 125 && !alarmOn)
      onAlarm();
    //não está mais em perigo
    else if(gas < 125 && alarmOn)
      offAlarm();
      
    if(!alarmOn){
    //está de noite
    //se estiver frio
     if(temperature < 25.00 && light > 536 )
      closeWindow();
    }
  }
  else
  {
    //Serial.println("Está fechada");
    //situação de perigo
    if(gas > 125 && !alarmOn){
      onAlarm();
      openWindow();
    }
    //não está mais em perigo
    else if(gas < 125 && alarmOn)
      offAlarm();
            
    if(!alarmOn){
    //está de dia
    //se estiver calor
     if(temperature >= 25.0 || light <= 536)
      openWindow();
    }
  }
  delay(200);
}

void checkPassword()
{
  char keypressed = myKeypad.getKey();
  if (keypressed != NO_KEY)
	   insertPassword +=(keypressed);
  Serial.println(insertPassword);
  
  if(insertPassword.length() == 4)
  {
  	if(insertPassword == password)
  	{
    	doorOpen = true;
      	access = true;
        Serial.write("Correta",7); //Write the serial data
    	door.write(90);
  	}
    else
    {
      insertPassword = "";
      Serial.write("Incorreta", 9); //Write the serial data
  	  analogWrite(A4, sound);
      delay(50);
      analogWrite(A4, 0);
    }
  }
}

void changeDoor(){
  if(doorOpen){
     door.write(0);
     doorOpen = false;
  }
  else{
     door.write(90);
     doorOpen = true;
  }
}


void loop()
{
  if(!access)
  	checkPassword();
  
  checkWindow();
}