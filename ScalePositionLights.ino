// OpenXsensor https://github.com/Peschi90/ScalePlaneLights/
// started by Marcel Peschka  http://www.peschi.info

// This is version : 1.0 (03.Aug.2018)

//******************************************************************************************************************************************************* //
//                                                                                                                                                        //
//                                             Scale Plane Position Lights with Sbus communication                                                        //
//                                                                                                                                                        //
//******************************************************************************************************************************************************* //


#include <sbus.h>
#define SBUS_PIN 13   // D13
SBUS sbus;

//SBUS CHANNEL NUMBER
#define SBUSchannel_1 5               //CHANGE IT TO YOUR RC CHANNEL
#define SBUSchannel_2 9               //CHANGE IT TO YOUR RC CHANNEL

//SBUS SWITCH MODES                   //WHAT TYPE OF SWITCHES DO YOU USE AT THE SBUS CHANNELS?
#define SBUSswitchMode 2              // 1 = BOTH 2 WAY SWITCHES (HIGH LOW)
                                      // 2 = BOTH 3 WAY SWITCHES (HIG MID LOW)
                                      // 3 = FIRST 2 WAY SECOND 3 WAY (HIGH MID & HIGH MID LOW)
                                      // 4 = ONE 2 WAY SWITCH (HIGH LOW)
                                      // 5 = ONE 3 WAY SWITCH (HIGH MID LOW)

//KIND OF MODES
#define MODE1 mode_1();
#define MODE2 mode_2();
#define MODE3 mode_3();
#define MODE4 mode_4();
#define MODE5 mode_5();
#define MODE6 mode_6();
#define MODE7 mode_7();
#define MODE8 mode_8();
#define MODE9 mode_9();


//HERE YOU CAN DEFINE YOU OWN END POINTS FOR THE SBUS SIGNALS
#define range_SBUS_high_top 2100
#define range_SBUS_high_low 1700
#define range_SBUS_mid_top 1699
#define range_SBUS_mid_low 1300
#define range_SBUS_low_top 1299
#define range_SBUS_low_low  800

//LED PINS                            I TAKE THE PWM PINS FOR THAT TO DIM OR FADE THE LEDS IN DIFFERENT MODES
#define low_light 3
#define top_light 5
#define left_light 6
#define right_light 9
#define rudder_light 10

//DELAY TIMES
#define blinking_Time_off_top 1000    //OFF TIME FOR THE TOP LIGHT IN BLINKING MODES IN MILLI SECONDS
#define blinking_Time_on_top 50       //ON TIME FOR THE TOP LIGHT IN BLINKING MODES IN MILLI SECONDS
#define blinking_Time_off_low 1100    //OFF TIME FOR THE LOW LIGHT IN BLINKING MODES IN MILLI SECONDS
#define blinking_Time_on_low 50       //ON TIME FOR THE LOW LIGHT IN BLINKING MODES IN MILLI SECONDS
#define fadeing_Time_on 40            //FADING SPEED FOR EVERY INCREASE STEP
#define fadeing_Time_off 30           //FADING SPEED FOR EVERY DECREASE STEP
#define next_light_Mode_2 100         //SPEED FOR NEXT LED IN MODE 2 IF YOU DECREASE YOU HAVE AN FASTER RUNNING LIGHT IN MODE 2
#define blinking_Time_off_error 1000  //OFF TIME FOR BLINKING WHILE ERROR MODE IN MILLI SECONDS
#define blinking_Time_on_error  1000  //ON TIME FOR BLINKING WHILE ERROR MODE IN MILLI SECONDS

//VARIABLES FOR TIME MESUREMENT      NO CHANGES NEEDED
unsigned long timeMillis;
unsigned long lastMillis_top;
unsigned long lastMillis_low;
unsigned long lastMillis_fade;
unsigned long lastMillis_Mode_2;

//MODE 2 VARIABLES
int LED_order[5]={low_light,left_light,top_light,rudder_light,right_light};   //YOU CAN CHANGE THE ORDER OF THE RUNNING LIGHT MODE BE EDITING THE ARRAY
int order_count_on = 0;
int order_count_off = 0;

//BLINKING OFF OR ON VARIABLE       NO CHANGES NEEDED
int blinking_off_top = 0;
int blinking_off_low = 0;

//VARIABLES FOR FADEING
int fadeing = 0;
int fadeValueOn = 0;
int fadeValueOff = 255;  //HERE YOU CAN CHANGE THE FADING BRIGHTNESS 255 IS MAXIMUM

//SBUS CHANNEL VARIABLE TEMPORARY   NO CHANGES NEEDED
uint16_t channel_1;     
uint16_t channel_2;

//SBUS CHANNEL RESULT
uint16_t channel_1_result;
uint16_t channel_2_result;
int channel_1_count = 0;
int channel_2_count = 0;


void setup() {
Serial.begin(115200);
sbus.begin(SBUS_PIN, sbusBlocking); 
pinMode (low_light, OUTPUT);
pinMode (top_light, OUTPUT);
pinMode (left_light, OUTPUT);
pinMode (right_light, OUTPUT);
pinMode (rudder_light, OUTPUT);
}

void loop() {
   timeMillis = millis();
   if (!sbus.waitFrame()) {               //IF YOU HAVE NO CONNECTION TO THE SBUS ALL LIGHTS WILL BE OFF
     analogWrite(top_light, 0);
     analogWrite(low_light, 0);
     analogWrite(left_light, 0);
     analogWrite(right_light, 0);
     analogWrite(rudder_light, 0);  
    } else {
      SBUSread_filter_Channels();       //I READ THE SBUS CHANNELS 16 TIMES AND FILTER THE SIGNAL BECOUSE SOMETIMES I GOT AN IVALID VALUE
      if(SBUSswitchMode == 1){
        Switch2x2way();
      }else if(SBUSswitchMode == 2){
        Switch2x3way();
      }else if(SBUSswitchMode == 3){
        Switch1x2way1x3way();
      }else if(SBUSswitchMode == 4){
        Switch1x2way();
      }else if(SBUSswitchMode == 5){
        Switch1x3way();
      }else{
        errorMode();                    //IF NO SBUSswitchMode IS DETECTET/DEFINED THE LEDS GO IN ERROR MODE 
      } 
    }
}
//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************
//*******************************************************MODE DEFINITION PART START*****************************************************************************
//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************
//IF YOU WANT YOU CAN RENAME THE MODE (FUNCTION NAME) HERE, BUT AFTER THAT YOU MUST CHANGE THE NAME TOO AT THE kind of mode SECTION AT THE BEGINNING FROM THIS FILE
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE1 START__________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
//SCALE LIGHT BEHAVIOUR
void mode_1(){
  digitalWrite(left_light,HIGH);
  digitalWrite(right_light,HIGH);
  //TOP BLINKER
  if(blinking_off_top==0 && (timeMillis - lastMillis_top)>= blinking_Time_on_top){
    digitalWrite(top_light, LOW);
    lastMillis_top = millis();
    blinking_off_top = 1;
    
  }else if(blinking_off_top==1 && (timeMillis - lastMillis_top) >= blinking_Time_off_top){
    digitalWrite(top_light, HIGH);
    lastMillis_top = millis();
    blinking_off_top = 0;
  }
  //LOW BLINKER
  if(blinking_off_low==0 && (timeMillis - lastMillis_low)>= blinking_Time_on_low){
    digitalWrite(low_light, LOW);
    lastMillis_low = millis();
    blinking_off_low = 1;
    
  }else if(blinking_off_low==1 && (timeMillis - lastMillis_low) >= blinking_Time_off_low){
    digitalWrite(low_light, HIGH);
    lastMillis_low = millis();
    blinking_off_low = 0;
  }
  //FADING RUDDER LIGHT
  if(fadeing==1 && (timeMillis - lastMillis_fade)>= fadeing_Time_on){
    fadeValueOn +=5;
    lastMillis_fade = millis();
    analogWrite(rudder_light,fadeValueOn);
    if(fadeValueOn >= 255){fadeing = 0;fadeValueOn = 0;}
  }
  if(fadeing == 0 && (timeMillis - lastMillis_fade) >= fadeing_Time_off){
    fadeValueOff -=5;
    lastMillis_fade = millis();
    analogWrite(rudder_light,fadeValueOff);
    if(fadeValueOff <= 0){fadeing = 1;fadeValueOff = 255;}
  }
}
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE1 END____________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE2 START__________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
//RUNNING LIGHT MODE
void mode_2(){
  if((timeMillis - lastMillis_Mode_2)>= next_light_Mode_2 && order_count_on <= 4){
    analogWrite(LED_order[order_count_on],255);
    order_count_on++;
    lastMillis_Mode_2 = millis();
  }else if((timeMillis - lastMillis_Mode_2) >= next_light_Mode_2 && order_count_on == 5 && order_count_off <= 4){
    analogWrite(LED_order[order_count_off],0);
    order_count_off++;
    lastMillis_Mode_2 = millis();
  }else if (order_count_on == 5 && order_count_off == 5){
    order_count_on = 0;
    order_count_off = 0;
  }
}
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE2 END____________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE3 START__________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
void mode_3(){
    if(blinking_off_top==0 && (timeMillis - lastMillis_top)>= blinking_Time_on_top){
    digitalWrite(top_light, LOW);
    digitalWrite(low_light, LOW);
    digitalWrite(left_light, LOW);
    digitalWrite(right_light, LOW);
    digitalWrite(rudder_light, LOW);
    lastMillis_top = millis();
    blinking_off_top = 1;
    
  }else if(blinking_off_top==1 && (timeMillis - lastMillis_top) >= blinking_Time_on_top){
    digitalWrite(top_light, HIGH);
    digitalWrite(low_light, HIGH);
    digitalWrite(left_light, HIGH);
    digitalWrite(right_light, HIGH);
    digitalWrite(rudder_light, HIGH);
    lastMillis_top = millis();
    blinking_off_top = 0;
  }
}
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE3 END____________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE4 START__________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
void mode_4(){
}
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE4 END____________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE5 START__________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
void mode_5(){
}
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE5 END____________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE6 START__________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
void mode_6(){
}
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE6 END____________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE7 START__________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
void mode_7(){  
}
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE7 END____________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE8 START__________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
void mode_8(){
}
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE8 END____________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE9 START__________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
void mode_9(){
}
//_______________________________________________________________________________________________________________________________________________________
//__________________________________________________________________MODE9 END____________________________________________________________________________
//_______________________________________________________________________________________________________________________________________________________
void errorMode(){
    if(blinking_off_top==0 && (timeMillis - lastMillis_top)>= blinking_Time_on_error){
    digitalWrite(top_light, LOW);
    digitalWrite(low_light, LOW);
    digitalWrite(left_light, LOW);
    digitalWrite(right_light, LOW);
    digitalWrite(rudder_light, LOW);
    lastMillis_top = millis();
    blinking_off_top = 1;
    
  }else if(blinking_off_top==1 && (timeMillis - lastMillis_top) >= blinking_Time_off_error){
    digitalWrite(top_light, HIGH);
    digitalWrite(low_light, HIGH);
    digitalWrite(left_light, HIGH);
    digitalWrite(right_light, HIGH);
    digitalWrite(rudder_light, HIGH);
    lastMillis_top = millis();
    blinking_off_top = 0;
  }
}
//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************
//*******************************************************MODE DEFINITION PART END*******************************************************************************
//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////NO CHANGES NEEDED////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////!!!!!!!!!!!!!!!!!////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//FILTERING THE SBUS CHANNELS OVER 16 READINGS BECOUSE OF SOME INTERRUPTS
void SBUSread_filter_Channels(){
    if(channel_1_count <= 15){
    channel_1 += sbus.getChannel(SBUSchannel_1);
    channel_1_count++;
    }else{
    channel_1_count = 0;
    channel_1_result = channel_1 / 16;
    channel_1 = 0;
    }
    if(channel_2_count <= 15){
    channel_2 += sbus.getChannel(SBUSchannel_2);
    channel_2_count++;
    }else{
    channel_2_count = 0;
    channel_2_result = channel_2 / 16;
    channel_2 = 0;
    }
}
void Switch2x2way(){
  if(channel_1_result <= range_SBUS_low_top && channel_1_result >= range_SBUS_low_low && channel_2_result <= range_SBUS_low_top && channel_2_result >= range_SBUS_low_low){
     MODE1
    }else if(channel_1_result <= range_SBUS_high_top && channel_1_result >= range_SBUS_high_low && channel_2_result <= range_SBUS_low_top && channel_2_result >= range_SBUS_low_low){
     MODE2
    }else if(channel_1_result <= range_SBUS_low_top && channel_1_result >= range_SBUS_low_low && channel_2_result <= range_SBUS_high_top && channel_2_result >= range_SBUS_high_low){
     MODE4
    }else if(channel_1_result <= range_SBUS_high_top && channel_1_result >= range_SBUS_high_low && channel_2_result <= range_SBUS_high_top && channel_2_result >= range_SBUS_high_low){
     MODE3
    }
}
void Switch2x3way(){
  if(channel_1_result <= range_SBUS_low_top && channel_1_result >= range_SBUS_low_low && channel_2_result <= range_SBUS_low_top && channel_2_result >= range_SBUS_low_low){
     MODE1
    }else if(channel_1_result <= range_SBUS_high_top && channel_1_result >= range_SBUS_high_low && channel_2_result <= range_SBUS_low_top && channel_2_result >= range_SBUS_low_low){
     MODE3 
    }else if(channel_1_result <= range_SBUS_low_top && channel_1_result >= range_SBUS_low_low && channel_2_result <= range_SBUS_high_top && channel_2_result >= range_SBUS_high_low){
     MODE7
    }else if(channel_1_result <= range_SBUS_high_top && channel_1_result >= range_SBUS_high_low && channel_2_result <= range_SBUS_high_top && channel_2_result >= range_SBUS_high_low){
     MODE5
    }else if(channel_1_result <= range_SBUS_mid_top && channel_1_result >= range_SBUS_mid_low && channel_2_result <= range_SBUS_low_top && channel_2_result >= range_SBUS_low_low){
     MODE2
    }else if(channel_1_result <= range_SBUS_low_top && channel_1_result >= range_SBUS_low_low && channel_2_result <= range_SBUS_mid_top && channel_2_result >= range_SBUS_mid_low){
     MODE8
    }else if(channel_1_result <= range_SBUS_mid_top && channel_1_result >= range_SBUS_mid_low && channel_2_result <= range_SBUS_mid_top && channel_2_result >= range_SBUS_mid_low){
     MODE9
    }else if(channel_1_result <= range_SBUS_high_top && channel_1_result >= range_SBUS_high_low && channel_2_result <= range_SBUS_mid_top && channel_2_result >= range_SBUS_mid_low){
     MODE4
    }else if(channel_1_result <= range_SBUS_mid_top && channel_1_result >= range_SBUS_mid_low && channel_2_result <= range_SBUS_high_top && channel_2_result >= range_SBUS_high_low){
     MODE6
    }
}
void Switch1x2way1x3way(){
  if(channel_1_result <= range_SBUS_low_top && channel_1_result >= range_SBUS_low_low && channel_2_result <= range_SBUS_low_top && channel_2_result >= range_SBUS_low_low){
     MODE1
    }else if(channel_1_result <= range_SBUS_high_top && channel_1_result >= range_SBUS_high_low && channel_2_result <= range_SBUS_low_top && channel_2_result >= range_SBUS_low_low){
     MODE2 
    }else if(channel_1_result <= range_SBUS_low_top && channel_1_result >= range_SBUS_low_low && channel_2_result <= range_SBUS_high_top && channel_2_result >= range_SBUS_high_low){
     MODE5
    }else if(channel_1_result <= range_SBUS_high_top && channel_1_result >= range_SBUS_high_low && channel_2_result <= range_SBUS_high_top && channel_2_result >= range_SBUS_high_low){
     MODE4
    }else if(channel_1_result <= range_SBUS_low_top && channel_1_result >= range_SBUS_low_low && channel_2_result <= range_SBUS_mid_top && channel_2_result >= range_SBUS_mid_low){
     MODE6
    }else if(channel_1_result <= range_SBUS_high_top && channel_1_result >= range_SBUS_high_low && channel_2_result <= range_SBUS_mid_top && channel_2_result >= range_SBUS_mid_low){
     MODE3
    }
}
void Switch1x2way(){
  if(channel_1_result <= range_SBUS_low_top && channel_1_result >= range_SBUS_low_low){
     MODE1
    }else if(channel_1_result <= range_SBUS_high_top && channel_1_result >= range_SBUS_high_low){
     MODE2 
    }
}
void Switch1x3way(){
  if(channel_1_result <= range_SBUS_low_top && channel_1_result >= range_SBUS_low_low){
     MODE1
    }else if(channel_1_result <= range_SBUS_mid_top && channel_1_result >= range_SBUS_mid_low){
     MODE2 
    }else if(channel_1_result <= range_SBUS_high_top && channel_1_result >= range_SBUS_high_low){
     MODE3 
    }
}
