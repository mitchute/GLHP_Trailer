const int    Voltage_Read = A0;   // DC voltage from Transformer
const int    SCR_Out      = 9;    // Output for SCR
const int    Q_Set        = 3810; // Q constant variable
const double R_Static     = 16.1; // Resistance of base heater
const double R_SCR        = 12.4; // Resistance of variable heater
const int    SCR_Max      = 102;  // Max value of the SCR 

int start                 = 1;    // For initial value stuff

//For smoothing values of the inputs
const int numReadings = 30;

int SCR_Out_Set = 0;

// Raw to Voltage
double DC_Voltage, AC_Voltage, SCR_Out_Voltage;

//Average RAW Values
int DC_Avg = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  //Clear LCD Screen and back light on
  Serial.write(12);
  Serial.write(17);
}

void loop() {
  
  if (start == 1){
    //Set initial SCR Out Voltage to 0
    analogWrite(SCR_Out, SCR_Out_Set);
    start = 0;
  }
  //Wait_Delay();
  Read_Input(1000);
  Convert_Data();
  Calculate_Data();  
  Print_Stuff(); 
  Output_to_SCR();      
}

void Wait_Delay() {
  Serial.write(12);
  Serial.write(17);
  delay(5);
}

void Read_Input(int Read_Delay) {
  // read the analog in raw value and average them
  int DC_Voltage_Read[numReadings]; 
  int DC_Total = 0;

  // Clear all array contents to '0'
  for (int ClrArray = 0; ClrArray < numReadings; ClrArray++){
    DC_Voltage_Read[ClrArray] = 0;
  }
  // Start reading
  for(int index = 0; index < numReadings; index++){

    // read from the sensor:  
    DC_Voltage_Read[index] = analogRead(Voltage_Read);

    // add the reading to the total:
    DC_Total = DC_Total + DC_Voltage_Read[index];
    
    Serial.write(12);
    Serial.print("Reading......");

    delay(Read_Delay);
  }

  // calculate the average:
  DC_Avg = DC_Total / numReadings;
}

void Convert_Data(){

  //Convert Raw data to Voltage
  DC_Voltage = DC_Avg * (5.0 / 1023);
  SCR_Out_Voltage = (SCR_Out_Set * (5.0 / 255));

  //Convert DC To AC Voltage
  AC_Voltage = ((DC_Voltage * 208) / 4 );
}

void Calculate_Data(){

  double P_SCR = 0; // Percent of SCR
  
  P_SCR = ((R_SCR / pow(AC_Voltage,2)) * 
                  (Q_Set - ((pow(AC_Voltage,2)) / R_Static)));
  
  SCR_Out_Set = int(SCR_Max * P_SCR);
}

void Output_to_SCR(){

  // change the analog out value:
  analogWrite(SCR_Out, SCR_Out_Set);
}

void Print_Stuff() {

  // print the results to the serial monitor:
  Serial.write(12);
  Serial.write(17);
  delay(5);
  Serial.print("DC voltage: ");
  Serial.print(DC_Voltage);
  Serial.write(13);
  Serial.print("AC Voltage: ");
  Serial.print(AC_Voltage);
  Serial.write(13);
  Serial.print("SCR Out (V): ");
  Serial.print(SCR_Out_Voltage);
  Serial.write(13);
  Serial.print("SCR Out Set: ");
  Serial.print(SCR_Out_Set);
  delay(3000);
}