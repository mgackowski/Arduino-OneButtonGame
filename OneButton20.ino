//------------------------------[GLOBAL VARIABLES]------------------------------//

long previousMillis;
long time;    //for button press - hiscore mechanism

boolean halfWin = false; //a halfwin is getting the horizontal LEDs right

int fail = 0;
int points = 0;
int hiscore = 0;

int PWMpin = 10;
int PWMvalue = 4;
int PWMrate = 1;          //related to the red pulse-modulated LED

int ledPin[] = {
  A4,A5,2,3,A2,A3,4,5,A1,A0,7,6,12,13,9,8};           //add LEDs here
int buttonPin = 11;       //the pin used for the button

int buttonState;          //this will let us know if the button is pressed
int prevButtonState;      //so you can't cheat by always pushing the button

int led = 0;              //LED currently flashing)
int pled = 3;             //LED previously flashing
int target;               //the player has to "catch" this LED

//int yled;                 //LED currently flashing in the vertical part of the challenge
//int pyled;

int s = 200;              //speed of flashing in milliseconds
int mode = 0;             //direction/mode of flashing game
                //mode 0 is between challenges, shows the target LED
                //mode 1 flashes LEDs from left, to right
                //mode 2 flashes LEDs from right to left
                //mode 3 flashes from up to down
                //mode 4 flashes from down to up
                //mode 5 - hiscore display
                //mode 255 = fail


//------------------------------[PROGRAM SETUP]------------------------------//

void setup() {
  Serial.begin(9600);    //this is for making sure the button works
  for(int x=0; x<4; x++){    
    //pinMode(ledPin[x],OUTPUT);
  }
  //that was supposed to quickly assign all leds as outputs, but for some
    //reason it also makes them flash brighter (?!)
  pinMode(buttonPin,INPUT);
}




void loop() {
  
  long currentMillis = millis();
  



//----------------------------[RED LED & FAIL CONTROL]----------------------------//
  
  failCheck(); //COMMENT OUT IF YOU WANT FREE PLAY WITHOUT PUNISHMENT
  
  int sensorValue = digitalRead(buttonPin);
  Serial.println(sensorValue);        //these are only used for serial monitor


//------------------------------[WHITE LEDs CONTROL]------------------------------//

  
  //---[NEW CHALLENGE SECTION]---//
  
  
  
  if (mode==0){            //before a new challenge starts
    if (halfWin==false){
  
      target = random(0,15);  //pick a random led as a target
      digitalWrite(ledPin[target],HIGH);  //flash it to the player
      delay(3000);                        //for 3 seconds
      digitalWrite(ledPin[target],LOW);   //then hide it

      mode=1;
    }
    else{mode=3;}
  }
  
  //---[HISCORE DISPLAY]---//
  
  
  
  if (mode==5){ 
    time=0;
    for(int x=0; x<16; x++){
    digitalWrite(ledPin[x],HIGH);
    }
    spiral();
    for(int x=0; x<16; x++){
    digitalWrite(ledPin[x],LOW);
    }
    for(int x=0; x<hiscore; x++){
        digitalWrite(ledPin[x],HIGH);
        delay(500);
      }
    delay(5000);
    for(int x=0; x<16; x++){
    digitalWrite(ledPin[x],LOW);
    }
    s = 200;
    mode = 0;
    fail = 0;
    led=0;
    halfWin = false;
    points = 0;
     
    
  }
  
  
  //---[LED "BOUNCING" MECHANISM]---//
  
  if(mode==1 | mode==2){
    if (led==0){
      mode = 1; //if far left LED is reached, make flashing direction to the right
    }  
    if (led==3){
      mode = 2; //if far right LED is reached, make flashing direction to the left
    }
  }
  if(mode==3 | mode==4){
    if (led>=0 && led<=3){
      mode = 3;
    }  
    if (led>=12 && led<=15){
      mode = 4;      //same thing, but for vertical bouncing
    }
  }
  
  
  //---[FLASHING THE CORRECT LED AND PREPARING FOR THE NEXT CYCLE]---//
    
  if (currentMillis - previousMillis > s){
    
    if(mode==1 | mode==2){
      digitalWrite(ledPin[led],HIGH);
        digitalWrite(ledPin[led+4],HIGH);
        digitalWrite(ledPin[led+8],HIGH);
        digitalWrite(ledPin[led+12],HIGH);//flash the LED you're supposed to
      digitalWrite(ledPin[pled],LOW);
        digitalWrite(ledPin[pled+4],LOW);
        digitalWrite(ledPin[pled+8],LOW);
        digitalWrite(ledPin[pled+12],LOW);//and take out the previous one
    
      pled = led;    //in the end, the current LED becomes the previous LED
      previousMillis = currentMillis;
    
      if(mode==1){
        led++;      //and a new LED is assigned depending on mode(direction)
      }
      if(mode==2){
        led--;
      }
    }
    if(mode==3 | mode==4){
      digitalWrite(ledPin[led],HIGH);
      digitalWrite(ledPin[pled],LOW);
      
      pled = led;
      previousMillis = currentMillis;
      
      if(mode==3){
        led+=4;
      }
      if(mode==4){
        led-=4;
      }
    }
  }
  
  //------------------------------[BUTTON CONTROLS]------------------------------//
  
  buttonState = digitalRead(buttonPin);  //meanwhile, check for the button
  if(buttonState==1){
    time+=1;
  }
  if(time>1000){
    mode=5;
  }
  if(buttonState==0){
    time=0;
  }

  
  //---[PLAYER SUCCEEDING CONTROLS]---//
  
  if ((mode==1 | mode==2) && buttonState==prevButtonState+1 && (led==target | led+4==target | led+8==target | led+12==target)){
    halfWin = true;
    for(int x; x<16; x++){
      digitalWrite(ledPin[x],LOW);
    }
    mode = 0;    //if pressed at the right moment, start from mode 0 again

    digitalWrite(ledPin[led],LOW);    //take out LED
  }
  
  if ((mode==3 | mode==4) && buttonState==prevButtonState+1 && led==target){
    if(points<16){
      points+=1;
    }
    mode=0;
    s=0.9*s;
    halfWin = false;
    for(int x; x<16; x++){
      digitalWrite(ledPin[x],LOW);
    }
    for(int a = 1; a<6; a++){
      digitalWrite(ledPin[led],LOW);
      delay(100);
      digitalWrite(ledPin[led],HIGH);
      delay(100);      //a little flashing animation to indicate hit target
    }
    digitalWrite(ledPin[led],LOW);    //take out LED
    led = 0;
  }
  
  //---[PLAYER FAILING CONTROLS]---//
  
  if ((mode==1 | mode==2) && buttonState==prevButtonState+1 && (led!=target && led+4!=target && led+8!=target && led+12!=target)){
    fail=fail+1;
  }
  if ((mode==3 | mode==4) && buttonState==prevButtonState+1 && (led!=target)){
   fail=fail+1;
  }

  //---[PREPARATION FOR NEXT CYCLE---//
  
  prevButtonState = buttonState;  //prevents cheating, basically
                                  //see tutorial: ButtonStateChange
}


//-------------------------[FAIL CHECKING AND BEHAVIOUR]-------------------------//

void failCheck() {
  
  if (fail == 0){
    digitalWrite(PWMpin,LOW);
  }
  if (fail == 1){
    analogWrite(PWMpin,PWMvalue);
    PWMvalue+=PWMrate;
  }
  if (fail == 2){
    analogWrite(PWMpin,PWMvalue);
    PWMvalue+=3*PWMrate;
  }
  if (fail == 3){
    mode = 255;
  }
  
  //Serial.println(PWMvalue); // was used to slow down the PWM to reasonable value
  
  if (PWMvalue > 250 || PWMvalue < 3){
    PWMrate = -PWMrate;
  }

  
  if (mode==255){
    digitalWrite(PWMpin,HIGH);
    
  
      for(int x=0; x<16; x++){
        digitalWrite(ledPin[x],HIGH);
      }
      delay(300);
      spiral();
      delay(300);
      for(int x=0; x<16; x++){
      digitalWrite(ledPin[x],LOW);
      }
      delay(1000);
      for(int x=0; x<points; x++){
        digitalWrite(ledPin[x],HIGH);
        delay(500);
      }
      

    
    
    delay(5000);
    digitalWrite(PWMpin,LOW);
    for(int x=0; x<16; x++){
      digitalWrite(ledPin[x],LOW);
    }
    s = 200;
    mode = 0;
    fail = 0;
    led=0;
    halfWin = false;
    if (points>hiscore){
      hiscore = points;
    }
    points = 0;
  }
}

void spiral(){
  int anim[] = {4,7,A0,A3,A5,2,3,5,6,8,9,13,12,A1,A2,A4};
/*  int num1[] = {A5,2,4,7,9,13,8,1,1,1,1,1,1,1,1,1};
  int num2[] = {A5,2,5,7,13,9,8,1,1,1,1,1,1,1,1,1};
  int num3[] = {A5,2,3,5,7,6,8,9,13};
  int num4[] = {A5,A3,4,5,7,9};
  int num5[] = {3,2,A5,A3,4,6,9,13};*/
  
  
  
  for(int c; c<16; c++){
    digitalWrite(anim[c],LOW);
    delay(100);
  }
    for(int c; c<16; c++){
    digitalWrite(anim[c],HIGH);
    delay(100);
  }
}

//------------------------------[END OF CODE]------------------------------//
  
