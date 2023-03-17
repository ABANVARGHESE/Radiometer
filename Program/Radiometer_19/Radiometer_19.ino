#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSans18pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float sum = 0, avg, maxval, samplesize = 38;
float calval, rcalval;
float senval[38], filtered[38];
byte counter = 0, pushcounter = 0, timer1 = 0, cutoff = 0;

float Fanem[] = {0, 3, 6.1, 9, 12.1, 15.1, 18.1, 21, 24, 27, 30.1, 33.1, 36, 39.1, 42, 45, 48.1, 51.1, 54.1, 57, 60.1, 63.1, 66.1, 69, 72.6, 75.4, 78.6, 81, 84.1, 87.6, 90.3, 93.9, 97.2, 100.5, 104.2, 108.6, 111.6, 115, 118.8, 121.8, 124.8, 128.1, 131.4, 134.8, 138, 142, 145, 150};
float adc_reading[] = {20, 36.6, 60.6, 82.5, 105.6, 128.2, 150.9, 167.3, 181.6, 196.7, 211.9, 228.5, 244, 260.8, 277.5, 292.5, 311.3, 326.8, 342.7, 359.3, 375.3, 390.7, 421, 436.1, 454.2, 466.7, 480.5, 491.3, 505, 519.2, 529.8, 544.5, 555.5, 567.4, 581, 594.6, 605, 614.6, 626.2, 636.5, 645.8, 654.7, 663.1, 673.2, 680.6, 689.8, 700.9, 712.6};

volatile byte flag1 = LOW, flag4 = LOW;

const byte pushButton = 2, chrgdet = 3, sen_det = 4, state_pin = 5;

void setup(){

 pinMode(state_pin, OUTPUT);
 digitalWrite(state_pin, HIGH); 

 if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){ 
  for(;;);
 }

 pinMode(pushButton, INPUT);
 pinMode(chrgdet, INPUT);
 pinMode(sen_det, INPUT);
 pinMode(A0, INPUT);
 
 attachInterrupt(digitalPinToInterrupt(pushButton), hold, LOW);
 attachInterrupt(digitalPinToInterrupt(chrgdet), chrg_det, CHANGE);

 display.clearDisplay();
 display.setTextColor(WHITE);

 display.setFont(&FreeSerif9pt7b);
 display.setCursor(19,37);
 display.print("Ibis Medical");
 display.display();
 while(1){
  while(millis() - timer1 >= 2000UL){ 
   start();
  }
 }
    
}

void start(){

 while(1){
  
  rcalval = analogval();

   while(rcalval > 150){
    rcalval = analogval();
    display.clearDisplay();
    display.setFont(&FreeSerif9pt7b);
    display.setCursor(25,50);
    display.print("*********");
    display.display();
   }

   while(flag4 == HIGH){
    unsigned long timer2;
    timer2 = millis();
    display.clearDisplay();
    display.setFont(&FreeSerif9pt7b);
    display.setCursor(25,37);
    display.print("Low Battery");
    display.display();
    while(1){
     while(millis() - timer2 >= 3000UL){
      digitalWrite(state_pin, LOW);
     }
    } 
   }
      
   display.clearDisplay();
   display.setFont(&FreeSerif9pt7b);
   display.setCursor(27,13);             
   display.print("Irradiance");
   display.setFont(&FreeSans18pt7b);
   display.setCursor(0,50);
   display.print("Ir=");
   if(rcalval < 10){
    display.setCursor(60,50);
    display.print("0");
   }
   else if((rcalval < 100)){
    display.setCursor(60,50);
   }
   else{
    display.setCursor(41,50);
   }            
   display.print(rcalval,1);
   display.display();
   
   while(flag1 == HIGH){
    unsigned long timer5;
    timer5 = millis();
    bool flag2 = true;

    while(flag2){
     while((millis() - timer5 >= 500UL) && (flag2 == true)){
      rcalval = analogval();     
      display.clearDisplay();
      display.setFont(&FreeSerif9pt7b);
      display.setCursor(27,13);             
      display.print("Irradiance");
      display.setFont(&FreeSans18pt7b);
      display.setCursor(0,50);
      display.print("Ir=");
      if(rcalval < 10){
       display.setCursor(60,50);
       display.print("0");
      }
      else if((rcalval < 100)){
       display.setCursor(60,50);
      }
      else{
       display.setCursor(41,50);
      }            
      display.print(rcalval,1);
      display.display();
      while(1){ 
       display.setFont(&FreeSerif9pt7b);
       display.setCursor(5,13);
       display.print("H"); 
       display.display();
       while(millis() - cutoff >= 600000UL){
        unsigned long timer6;
        timer6 = millis();
        display.clearDisplay();
        display.setFont(&FreeSerif9pt7b);
        display.setCursor(15,37);
        display.print("Turning off...");
        display.display();
        while(1){
         while(millis() - timer6 >= 4000UL){
          digitalWrite(state_pin, LOW);
         }
        }
       }
       while(flag4 == HIGH){
        unsigned long timer4;
        timer4 = millis();
        display.clearDisplay();
        display.setFont(&FreeSerif9pt7b);
        display.setCursor(25,37);
        display.print("Low Battery");
        display.display();
        while(1){
         while(millis() - timer4 >= 3000UL){
          digitalWrite(state_pin, LOW);
         }
        } 
       } 
       if(flag1 == HIGH){
        counter++;  
       } 
           
       if(counter > 2){
        counter = 0;
        flag1 = LOW;
        pushcounter++; 
       }
        
       if(pushcounter == 2){
        pushcounter = 0;
        flag2 = false;
        break; 
       }
        } 
       }
      }
        
    } 

  }
   
}

void loop(){
    
}

float analogval(){

   for(byte k = 0; k < samplesize; k++){
        
    for(byte i = 0; i < samplesize; i++){
     senval[i] = analogRead(A0);
     sum = sum + senval[i];     
     while((i == 10) && (sum < 1)){
      display.clearDisplay();
      display.setFont(&FreeSerif9pt7b);
      display.setCursor(25,37);
      display.print("No Sensor");
      display.display();
      while(millis() - cutoff >= 600000UL){
       unsigned long timer7;
       timer7 = millis();
       display.clearDisplay();
       display.setFont(&FreeSerif9pt7b);
       display.setCursor(15,37);
       display.print("Turning off...");
       display.display();
       while(1){
        while(millis() - timer7 >= 4000UL){
         digitalWrite(state_pin, LOW);
        }
       }
      }
      while(flag4 == HIGH){
       unsigned long timer3;
       timer3 = millis();
       display.clearDisplay();
       display.setFont(&FreeSerif9pt7b);
       display.setCursor(25,37);
       display.print("Low Battery");
       display.display();
       while(1){
        while(millis() - timer3 >= 3000UL){
         digitalWrite(state_pin, LOW);
        }
       } 
      }
      byte sensor_value = digitalRead(sen_det);
      if(sensor_value == 0){
       start();
      }
     }
    } 

    avg = sum/samplesize;
    sum = 0;
    filtered[k] = avg;
     
   }
    
   for(byte j = 0; j < samplesize; j++){
    if(j == 0){
     float sub = filtered[j] - filtered[j+1];
     if(sub <= 0){
      maxval = filtered[j+1];   
     }
     else if(sub > 0){
      maxval = filtered[j];
     }
    }
    else{
     float newsub = maxval - filtered[j+1];  
     if(newsub <= 0){
      maxval = filtered[j+1];   
     }
     else if(newsub > 0){
      maxval = maxval;
     }
    }
    if(j == (samplesize - 1)){
     break;  
    }
   }

    byte ab = 0;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 1;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 2;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 3;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 4;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 5;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 6;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 7;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 8;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 9;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 10;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 11;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 12;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 13;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 14;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 15;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 16;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 17;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 18;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 19;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 20;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 21;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 22;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 23;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 24;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 25;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 26;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 27;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 28;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 29;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 30;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 31;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 32;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 33;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 34;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 35;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 36;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 37;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 38;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 39;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 40;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 41;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 42;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 43;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 44;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 45;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }
    ab = 46;    
    if((maxval >= adc_reading[ab]) && (maxval <= adc_reading[ab+1])){
     calval = Fanem[ab] + (((maxval - adc_reading[ab]) / ((adc_reading[ab + 1]) - (adc_reading[ab]))) * ((Fanem[ab + 1]) - (Fanem[ab]))); 
    }

    while(millis() - cutoff >= 600000UL){
     unsigned long timer8;
     timer8 = millis();
     display.clearDisplay();
     display.setFont(&FreeSerif9pt7b);
     display.setCursor(15,37);
     display.print("Turning off...");
     display.display();
     while(1){
      while(millis() - timer8 >= 4000UL){
       digitalWrite(state_pin, LOW);
      }
     }
    }

    if(calval < 1){
     calval = 0.0;
    }
    return calval;
    
}

void hold(){
 flag1 = HIGH;
}

void chrg_det(){
 flag4 = HIGH;
}
