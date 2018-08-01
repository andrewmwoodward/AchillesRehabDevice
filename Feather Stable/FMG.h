
const int FSRPin = A0;

// maximum voluntary contraction
float MVCmax = 150; //FOR NICK TESTING (MUST BE CALIBRATED WITH EACH DORN)
float MVCmin = 25; //FOR NICK TESTING  (MUST BE CALIBRATED WITH EACH DORN)
float MVC = MVCmax-MVCmin;

//FMG Scale 
int zeroFMG = 0;

//FMG Peak Detection
float maxVal = 0;
float minVal = 0;
float count = 0;
float fmgConst = 0;
float fmgDiff = 0;
float percFMG = 0;
float fmgPrev = 0;


void FMGsetup(void){
  pinMode(FSRPin, INPUT_PULLUP);
  
  //FMG Scale to ~Zero
  for(int i=0; i<5; i++)
    {  
      zeroFMG += analogRead(FSRPin);          
    }
    zeroFMG /= 7;    
}



float updateFMG(void){
  //Serial.println(zeroFMG);
  float FMG = analogRead(FSRPin);
  //Serial.println(FMG);
  FMG = FMG-zeroFMG;
  //Serial.println(FMG);
 
  if (count == 0)
  {
    for(int j=0; j<5; j++)
     {  
        fmgConst += analogRead(FSRPin);          
     }
    minVal = fmgConst/5;
    count++;
  }
  
  //Identify/Record Peak to Peak Amplitude
  if ((FMG - fmgPrev)>10)
  {
    maxVal=0;
    minVal=fmgConst/5;
  }
  if (FMG > maxVal)
  {
    maxVal = FMG;
  }
  
  if (FMG < minVal)
  {
    minVal = FMG;
  }
  fmgDiff = maxVal-minVal;

 
  //Serial.println(maxVal);
  //Serial.println(minVal);
  
  //Determine FMG as percentage of MVC
  percFMG = (fmgDiff / MVC)*100;
  Serial.print(FMG);
  //Serial.print("\tdiff ");
  //Serial.println(fmgDiff);
  //Serial.print("\tMuscle ");
  //Serial.print(percFMG);

  fmgPrev = FMG;

  return percFMG; 
}

