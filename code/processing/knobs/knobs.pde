import processing.serial.*; 
import controlP5.*; 

Serial Arduino;
ControlP5 cp5;

int myColorBackground = color(50);
int lf = 10;
int f1 = 1000;
int f2 = 5000;
int t1 = 100;
int t2 = 100;

Knob myKnobA;
Knob myKnobB;
Knob myKnobC;
Knob myKnobD;

void setup() {
  surface.setTitle("DUAL DDS 2020");
  size(700,480);
  smooth(); 
  noStroke();
  
  Arduino = new Serial (this,Serial.list()[1],9600);
  Arduino.bufferUntil(lf);
  
  cp5 = new ControlP5(this);
  
  myKnobA = cp5.addKnob("offset1")
               .setRange(0,255)
               .setValue(0)
               .setPosition(100,70)
               .setRadius(50)
               .setNumberOfTickMarks(10)
               .setTickMarkLength(4)
               .setColorForeground(color(255))
               .setColorActive(color(255,255,0))
               .setDragDirection(Knob.VERTICAL)
               ;
                     
  myKnobB = cp5.addKnob("amp1")
               .setRange(0,255)
               .setValue(0)
               .setPosition(100,210)
               .setRadius(50)
               .setNumberOfTickMarks(10)
               .setTickMarkLength(4)
               .setColorForeground(color(255))
               .setColorBackground(color(0, 160, 100))
               .setColorActive(color(255,255,0))
               .setDragDirection(Knob.VERTICAL)
               ;
               
  myKnobC = cp5.addKnob("offset2")
               .setRange(0,255)
               .setValue(0)
               .setPosition(500,70)
               .setRadius(50)
               .setNumberOfTickMarks(10)
               .setTickMarkLength(4)
               .setColorForeground(color(255))
               .setColorActive(color(255,255,0))
               .setDragDirection(Knob.VERTICAL)
               ;
                     
  myKnobD = cp5.addKnob("amp2")
               .setRange(0,255)
               .setValue(0)
               .setPosition(500,210)
               .setRadius(50)
               .setNumberOfTickMarks(10)
               .setTickMarkLength(4)
               .setColorForeground(color(255))
               .setColorBackground(color(0, 160, 100))
               .setColorActive(color(255,255,0))
               .setDragDirection(Knob.VERTICAL)
               ;

    cp5.addNumberbox("frecuencia1")
     .setPosition(100,330)
     .setSize(100,20)
     .setRange(0,10000)
     .setMultiplier(10) // set the sensitifity of the numberbox
     .setDirection(Controller.HORIZONTAL) // change the control direction to left/right
     //.setValue(0)
     ;
     cp5.addNumberbox("frecuencia2")
     .setPosition(500,330)
     .setSize(100,20)
     .setRange(0,10000)
     .setMultiplier(10) // set the sensitifity of the numberbox
     .setDirection(Controller.HORIZONTAL) // change the control direction to left/right
     //.setValue(0)
     ;
     cp5.addNumberbox("fase1")
     .setPosition(100,30)
     .setSize(100,20)
     .setRange(0,360)
     .setMultiplier(1) // set the sensitifity of the numberbox
     .setDirection(Controller.HORIZONTAL) // change the control direction to left/right
     //.setValue(0)
     ;
     cp5.addNumberbox("fase2")
     .setPosition(500,30)
     .setSize(100,20)
     .setRange(0,360)
     .setMultiplier(1) // set the sensitifity of the numberbox
     .setDirection(Controller.HORIZONTAL) // change the control direction to left/right
     //.setValue(0)
     ;

ButtonBar b = cp5.addButtonBar("bar")
     .setPosition(80, 0)
     .setSize(140, 20)
     .addItems(split("a b c d"," "))
     ;
  b.changeItem("a","text","Sin");
  b.changeItem("b","text","Tri");
  b.changeItem("c","text","Sqr");
  b.changeItem("d","text","OFF");

ButtonBar b2 = cp5.addButtonBar("bar2")
     .setPosition(480, 0)
     .setSize(140, 20)
     .addItems(split("a b c d"," "))
     ;
  b2.changeItem("a","text","Sin");
  b2.changeItem("b","text","Tri");
  b2.changeItem("c","text","Sqr");
  b2.changeItem("d","text","OFF");

ButtonBar b3 = cp5.addButtonBar("bar3")
     .setPosition(80, 380)
     .setSize(140, 20)
     .addItems(split("a b c d"," "))
     ;
  b3.changeItem("a","text","Start");
  b3.changeItem("b","text","Stop");
  b3.changeItem("c","text","Sweep");
  b3.changeItem("d","text","OFF");

ButtonBar b4 = cp5.addButtonBar("bar4")
     .setPosition(480, 380)
     .setSize(140, 20)
     .addItems(split("a b c d"," "))
     ;
  b4.changeItem("a","text","Start");
  b4.changeItem("b","text","Stop");
  b4.changeItem("c","text","Sweep");
  b4.changeItem("d","text","OFF");

      cp5.addNumberbox("tiempo1")
     .setPosition(100,415)
     .setSize(100,20)
     .setRange(0,500)
     .setMultiplier(1) // set the sensitifity of the numberbox
     .setDirection(Controller.HORIZONTAL) // change the control direction to left/right
     .setValue(5)
     ;

     cp5.addNumberbox("tiempo2")
     .setPosition(500,415)
     .setSize(100,20)
     .setRange(0,500)
     .setMultiplier(1) // set the sensitifity of the numberbox
     .setDirection(Controller.HORIZONTAL) // change the control direction to left/right
     .setValue(5)
     ;

     cp5.addButton("AD9833-CH1")
     .setValue(0)
     .setPosition(80,460)
     .setSize(140,20)
     ;

     cp5.addButton("AD9833-CH2")
     .setValue(0)
     .setPosition(480,460)
     .setSize(140,20)
     ;

  println(Arduino.list());
}

void draw() {
  background(myColorBackground);
  fill(0,100);
  rect(80,20,140,440);
  rect(480,20,140,440);
}

void offset1(int off) {
  //myColorBackground = color(theValue);
  println(":AWG1:OFFSET "+off);
  Arduino.write(":AWG1:OFFSET "+off);
  delay(100);
}
void offset2(int off) {
  //myColorBackground = color(theValue);
  println(":AWG2:OFFSET "+off);
  Arduino.write(":AWG2:OFFSET "+off);
  delay(100);
}

void amp1(int amp) {
  //myColorBackground = color(theValue);
  println(":AWG1:OFFSET "+amp);
  Arduino.write(":AWG1:AMP "+amp);
  delay(100);
}
void amp2(int amp) {
  //myColorBackground = color(theValue);
  println(":AWG2:OFFSET "+amp);
  Arduino.write(":AWG2:AMP "+amp);
  delay(100);
}

void frecuencia1(int freq) {
  println(":AWG1:FREQ "+freq);
  f1 = freq;
  Arduino.write(":AWG1:FREQ "+freq);
  delay(100);
}

void frecuencia2(int freq) {
  f2 = freq;
  println(":AWG2:FREQ "+freq);
  Arduino.write(":AWG2:FREQ "+freq);
  delay(100);
}

void fase1(int fase) {
  println(":AWG1:PHA "+fase*10);
  Arduino.write(":AWG1:PHA "+fase*10);
  delay(100);
}

void fase2(int fase) {
  println(":AWG2:PHA "+fase*10);
  Arduino.write(":AWG2:PHA "+fase*10);
  delay(100);
}

void tiempo1(int tiempo) {
  t1 = tiempo*10;
  delay(100);
}

void tiempo2(int tiempo) {
  t2 = tiempo*10;
  delay(100);
}

void keyPressed() {
  switch(key) {
    case('h'):myKnobA.setValue(0);break;
    case('j'):myKnobB.setValue(0);break;
    case('k'):myKnobC.setValue(0);break;
    case('l'):myKnobD.setValue(0);break;
  }
  
}

void bar(int n) {
  switch(n){
    case(0):Arduino.write(":AWG1:FUNCTION S");break;
    case(1):Arduino.write(":AWG1:FUNCTION T");break;
    case(2):Arduino.write(":AWG1:FUNCTION C");break;
    case(3):Arduino.write(":AWG1:FUNCTION O");break;
  }
}

void bar2(int n) {
  switch(n){
    case(0):Arduino.write(":AWG2:FUNCTION S");break;
    case(1):Arduino.write(":AWG2:FUNCTION T");break;
    case(2):Arduino.write(":AWG2:FUNCTION C");break;
    case(3):Arduino.write(":AWG2:FUNCTION O");break;
  }
}

void bar3(int n) {
  switch(n){
    case(0):Arduino.write(":AWG1:SWEEPSTART "+f1);break;
    case(1):Arduino.write(":AWG1:SWEEPSTOP "+f1);break;
    case(2):Arduino.write(":AWG1:SWEEPTIME "+t1);delay(100);Arduino.write(":AWG1:SWEEP");break;
    case(3):Arduino.write(":AWG1:SWEEPSTOP "+f1);break;
  }
}

void bar4(int n) {
  switch(n){
    case(0):Arduino.write(":AWG2:SWEEPSTART "+f1);break;
    case(1):Arduino.write(":AWG2:SWEEPSTOP "+f2);break;
    case(2):Arduino.write(":AWG2:SWEEPTIME "+t2);delay(100);Arduino.write(":AWG2:SWEEP");break;
    case(3):Arduino.write(":AWG2:SWEEPSTOP "+f1);break;
  }
}