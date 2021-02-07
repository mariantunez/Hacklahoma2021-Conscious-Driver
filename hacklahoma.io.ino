

/*
  Conscious Driver Project

 This sketch calculates the Blood Alcohol Concentration of a person.
 Determines if a person is allowed or not to drive.
 This sketch includes the code of the motor since it's part of the device as well.
 
  The circuit:
 * sensor MQ-2 A0 -> analog pin A0
 * LCD RS pin -> digital pin 7
 * LCD E pin -> digital pin 6
 * LCD D4 pin -> digital pin 5
 * LCD D5 pin -> digital pin 4
 * LCD D6 pin -> digital pin 3
 * LCD D7 pin -> digital pin 2
 * LCD R/W pin -> ground
 * LCD VSS pin -> ground
 * LCD VCC pin -> 5V
 * LED1 pin -> digital pin 8
 * LED2 pin -> digital pin 13
 * Motor input1Pin -> digital pin 12
 * Motor input2Pin -> digital pin 11
 * Motor input3Pin -> digital pin 10 
 * Motor input4Pin -> digital pin 9
 * 
 * 
 * 
 */

// Include the lcd screen and stepper motor library
#include <LiquidCrystal.h>
#include <Stepper.h>


// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 2, 3, 4, 5);

//Initialize variables and pins
int motor = 0;                // Stepper motor
int sensor_input = A0;       // Pin digital para el sensor del alcohol
float BAC;                  // Blood Alcohol Concentration (BAC)
float sensor_MQ;           // Variable que guarda el valor del sensor (300-750)
float curve_value;        //Curve value of the sensor that maps parts per million of alcohol to a voltage
int inp1Pin = 12;        //Motor pin 4
int inp2Pin = 11;       //Motor pin 4
int inp3Pin = 10;      //Motor pin 4
int inp4Pin = 9;      //Motor pin 4
int potpin = 8;      // analog pin used to connect the potentiometer

 
// Define number of steps on the motor
// 2048 steps makes a a complete rotation
//Define the 512 steps for the motor to rotate 90 degress to resemble the unlock
Stepper motor(512, inp1Pin,inp2Pin, inp3Pin, inp4Pin);

//Setup
void setup() {
  Serial.begin(9600);                    // Begin serial communication
  motor.setSpeed(20);                   // Define the stepper motor speed
  lcd.begin(16, 2);                    // set up the LCD's number of columns and rows
  lcd.print("Blow in Sensor..");      // Print a the initial message to the LCD
  pinMode(inp1Pin, OUTPUT);          //Set pin to OUTPUT
  pinMode(inp2Pin, OUTPUT);         //Set pin to OUTPUT
  pinMode(inp3Pin, OUTPUT);        //Set pin to OUTPUT
  pinMode(inp4Pin, OUTPUT);       //Set pin to OUTPUT
  pinMode(13, OUTPUT);           //Set pin to OUTPUT
  pinMode(12,OUTPUT);           //Set pin to OUTPUT
  delay(5000);
}

//Loop
void loop() {

  // Read the Analog voltage of the sensor
  sensor_MQ = analogRead(sensor_input);       //Assign readding to variable sensor_MQ
  Serial.print("Sensor analog voltage: ");   //Print the sensor analog voltage
  Serial.println(sensor_MQ);
  
  // Determine the digital voltage of the sensor input
  // In analog the arduino receive signals from 0 - 1023
  // In digial the arduino receive signals from 0 -5 
  sensor_MQ = (sensor_MQ*5)/1023;
  Serial.print("Sensor digital voltage: ");    //Printt the calculated sensor digital voltage
  Serial.println(sensor_MQ); 

  // Determine the curve value of sensor using formula
  // This is evaluated with the previously determined digital voltage
  curve_value = (150.4351049*pow(sensor_MQ,5)); 
  curve_value -= (2244.75988*pow(sensor_MQ,4)); 
  curve_value += (13308.5139*pow(sensor_MQ,3));
  curve_value -= (39136.08594*pow(sensor_MQ,2)); 
  curve_value += (57082.6258*sensor_MQ);
  curve_value -= 32982.05333;
  Serial.print("Curve Value");        //Print Curve Value just for reference
  Serial.println(curve_value);       //Print Curve Value

  // Determine BAC Value
  // The voltages that gave negative curve values are set to zero
  if (curve_value<0){
    BAC = 0.00;        //Set BAC to zero
  }
  
  else {
    BAC = curve_value/2600.00; //Determine BAC for positive curve values
  }
  Serial.print("BAC : ");  //Print the BAC value just for reference
  Serial.println(BAC);

  //Print BAC value in lcd screen
  delay(3000);
  lcd.begin(16, 2);
  lcd.print("BAC: ");      //Print the BAC value in the lcd screen
  lcd.print(BAC);
  lcd.print(" %");       // Print t he porcentage symbol in the lcd screen
  lcd.setCursor(0, 1);  //Set the cursos of the screen

  // The limit allowed value for drunk driving is 0.08
  // Use this value to determine is the driver is in the limit to drive or not
  if (BAC <= 0.08){  //BAC under the limit
    lcd.print("Allow to Drive  ");  //Print in the lcd screen that the person is allow to drive
    
    //Leds to resemble the motor in case tat is not available
    digitalWrite(13, HIGH);        //Send a HIGH signal true pin 13 to turn led on      
    delay(500);              
    digitalWrite(13, LOW);        //Send a LOW signal true pin 13 to turn led off 
    delay(500);
    
    //We only tell the motor to move the steps we assgined previously
    if (Serial.available()){          //Gets the number of bytes available for reading from the serial port.
     int moves = Serial.parseInt();  //Looks for the next valid integer in the incoming serial
      motor.step(moves);            //Tell the motor to move the steps
      Serial.println(moves);       //Print the move just fore reference
    }
  }
  else {
    //Since the value wasn't in the above statemente, it means the driver is above the limit
    //This means th e person is under the effect on alcohol and is not safe to drive
    lcd.print("Cannot Drive");  //Print in lcd sreen that the person Cannot Drive
    digitalWrite(8, HIGH);     //Send a HIGH signal true pin 8 to turn led N
    delay(500);              
    digitalWrite(8, LOW);   //Send a LOW signal true pin 8 to turn led off
    delay(500);
    }
  
  delay(1000);
}
 
