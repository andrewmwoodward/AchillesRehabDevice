// not tested waiting for insole to be completed

const int frontInsolePin = A2, backInsolePin = A1;

void InsoleSetup(void){
  pinMode(frontInsolePin, INPUT_PULLUP);
  pinMode(backInsolePin, INPUT_PULLUP);
}


void calculateForceKg(int* back, int* front){
  int analogBack = analogRead(backInsolePin);
  int analogFront = analogRead(frontInsolePin); 
  
  *back = analogBack; // insert conversion equation here
  
  *front = analogFront; // insert conversion equation here
}



