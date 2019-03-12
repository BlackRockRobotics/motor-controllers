int inA1 = 8; // input 1 of the stepper
int inA2 = 9; // input 2 of the stepper
int inB1 = 7; // input 3 of the stepper
int inB2 = 6; // input 4 of the stepper

int ENA=11;
int ENB=12;

int stepDelay = 20; // Delay between steps in milliseconds

int posStep = 0;
int dirDelay = 500;

int goal = 1;
int dirStep = 1;
int stepID = 0;
int lastStepID = 0;

void setup() {
  Serial.begin(9600);
                
  pinMode(inA1, OUTPUT);     
  pinMode(inA2, OUTPUT);     
  pinMode(inB1, OUTPUT);     
  pinMode(inB2, OUTPUT);     
}

void step1() {
  digitalWrite(inA1, LOW);   
  digitalWrite(inA2, HIGH);   
  digitalWrite(inB1, HIGH);   
  digitalWrite(inB2, LOW);   
  delay(stepDelay);
}
void step2() {
  digitalWrite(inA1, LOW);   
  digitalWrite(inA2, HIGH);   
  digitalWrite(inB1, LOW);   
  digitalWrite(inB2, HIGH);   
  delay(stepDelay);
}
void step3() {
  digitalWrite(inA1, HIGH);   
  digitalWrite(inA2, LOW);   
  digitalWrite(inB1, LOW);   
  digitalWrite(inB2, HIGH);   
  delay(stepDelay);
}
void step4() {
  digitalWrite(inA1, HIGH);   
  digitalWrite(inA2, LOW);   
  digitalWrite(inB1, HIGH);   
  digitalWrite(inB2, LOW);   
  delay(stepDelay);
}
void stepTestA() {
  digitalWrite(inA1, HIGH);   
  digitalWrite(inA2, LOW);   
  digitalWrite(inB1, LOW);   
  digitalWrite(inB2, LOW);   
  delay(stepDelay);
}

void stepTestB() {
  digitalWrite(inA1, LOW);   
  digitalWrite(inA2, LOW);   
  digitalWrite(inB1, HIGH);   
  digitalWrite(inB2, LOW);   
  delay(stepDelay);
}

void stopMotor() {
  digitalWrite(inA1, LOW);   
  digitalWrite(inA2, LOW);   
  digitalWrite(inB1, LOW);   
  digitalWrite(inB2, LOW);   
}

void setGoal(int newGoal) {
  goal = newGoal;
  if(goal < posStep) {
    dirStep = 0;
  } else {
    dirStep = 1;
  }
}

void setToStep() {
  switch(stepID) {
    case 0:
      step1();
      break;
    case 1:
      step2();
      break;
    case 2:
      step3();
      break;
    case 3:
      step4();
      break;
    }
}

void stepBack() {
  lastStepID = stepID;
  stepID = (stepID + 4-1)%4;
}

void stepForward() {
  lastStepID = stepID;
  stepID = (stepID + 1)%4;
}
// the loop routine runs over and over again forever: 
void loop() {
  digitalWrite(ENA, HIGH);   
  digitalWrite(ENB, HIGH);
  
  if(posStep != goal) {
    setToStep();
    switch(dirStep) {
      case 0:
        posStep--;
        stepBack();
        break;
      case 1:
        posStep++;
        stepForward();
        break;
    }
//    Serial.print("Step: ");
    Serial.println(posStep);    
  } else {
//    stopMotor();
//    Serial.println("Switching direction");
//    if(goal == 40) {
//      Serial.println("To zero");
//      setGoal(0);
//    } else {
//      setGoal(40);
//      Serial.println("To 40");
//    }
  }
  if(Serial.available()) {
    String s = Serial.readStringUntil('\n');
    if(s.charAt(0) == 'd') {
//      Serial.println("Disable");
      stopMotor();
    } else if(s.charAt(0) == 'e') {
//      Serial.println("Enable");
      int oldStepID = stepID;
      setToStep();
      stepID = lastStepID;
    } else {
      long i = s.toInt();
//      Serial.print("Set position to:");
//      Serial.println(i);
      setGoal(i);
    }

  }
//  Serial.println("Repeat");
 //  stepTestA();
// Serial.println("Step forward");
//  for (int i=0; i<=11; i++){ 
//    step1(); 
//    step2();
//    step3();
//    step4();
//  }
//  //stopMotor();
//  delay(dirDelay);
//  
// Serial.println("Step back");
//  for (int i=0; i<=11; i++){
//    step3();
//    step2();
//    step1();
//    step4(); 
//  }
//Serial.println("stop");

  //stopMotor();
//  delay(dirDelay);
}
