const int    DC_Voltage_Pin   = A0;   // DC voltage from Transformer
const int    Pump_Volts_Pin   = A1;   // Pump power voltage
const int    Pump_Amps_Pin    = A2;   // Pump power amps
const int    SCR_Out_Pin      = 9;    // Output for SCR
const int    Q_Set            = 3810; // Q constant variable
const double R_Static         = 16.1; // Resistance of base heater
const double R_SCR            = 12.4; // Resistance of variable heater
const int numReadings         = 30;   //For avg values of the inputs
int start                     = 1;    // For initial value stuff
int SCR_Out_Set               = 0;
int DC_Avg                    = 0;    //Average RAW Values
int Pump_Volts_Avg            = 0;
int Pump_Amps_Avg             = 0;
double DC_Voltage, AC_Voltage, SCR_Out_Voltage;  // Raw to Voltage
double Pump_DC_Voltage, Pump_DC_Amps, Pump_Volts, Pump_Amps;
double Q_Base, Q_Pump, Q_SCR, FFP;

//******Initializes the communication and clears the LCD screen.
void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  //Clear LCD Screen and back light on
  Serial.write(12);
  Serial.write(17);
}

//******Main Driver********************************************
void loop() {
  if (start == 1){
    //Set initial SCR Out Voltage to 0
    analogWrite(SCR_Out_Pin, SCR_Out_Set);
    start = 0;
  }
  Read_Input(1000);
  Convert_Data();
  Calculate_Data();  
  Print_Stuff(); 
  Output_to_SCR();      
}

//******Reads the analog DC voltage*****************************
void Read_Input(int Read_Delay) {
  
  // Setup the variables
  int DC_Voltage_Read[numReadings]; 
  int Pump_Volts_Read[numReadings];
  int Pump_Amps_Read[numReadings];
  int DC_Total = 0;
  int Pump_Volts_Total = 0;
  int Pump_Amps_Total = 0;

  // Clear all array contents to '0'
  for (int ClrArray = 0; ClrArray < numReadings; ClrArray++){
    DC_Voltage_Read[ClrArray] = 0;
    Pump_Volts_Read[ClrArray] = 0;
    Pump_Amps_Read[ClrArray]  = 0;
  }
  // Start reading
  for(int index = 0; index < numReadings; index++){

    // read from the sensor:  
    DC_Voltage_Read[index] = analogRead(DC_Voltage_Pin);
    Pump_Volts_Read[index] = analogRead(Pump_Volts_Pin);
    Pump_Amps_Read[index] = analogRead(Pump_Amps_Pin);

    // add the reading to the total:
    DC_Total += DC_Voltage_Read[index];
    Pump_Volts_Total += Pump_Volts_Read[index];
    Pump_Amps_Total += Pump_Amps_Read[index];
    
    delay(Read_Delay);
  }
  // calculate the average:
  DC_Avg = DC_Total / numReadings; 
  Pump_Volts_Avg = Pump_Volts_Total / numReadings;
  Pump_Amps_Avg = Pump_Amps_Total / numReadings;
}

//******Converts bits to AC voltage************************
void Convert_Data(){

  //Convert Raw data to Voltage
  DC_Voltage = DC_Avg * (5.0 / 1023);
  Pump_DC_Voltage = Pump_Volts_Avg * (5.0 / 1023);
  Pump_DC_Amps = Pump_Amps_Avg * (5.0 / 1023);

  //Convert DC To AC Voltage
  AC_Voltage = (DC_Voltage / 5.0) * 226.5;
  Pump_Volts = (Pump_DC_Voltage / 5.0) * 210;
  Pump_Amps = Pump_DC_Amps;
}

void Calculate_Data(){
  //Reset zeros
  Q_Pump = 0;
  Q_Base = 0;
  Q_SCR  = 0;
  FFP 	 = 0;

  //Pump heat
  Q_Pump = 0.9158 * (Pump_Volts * Pump_Amps);
  
  // Base heater
  Q_Base = (pow(AC_Voltage,2)/R_Static); 
  
  //SCR calculations
  Q_SCR = (Q_Set - Q_Base - Q_Pump);
  
  //Fraction of full power
  FFP = Q_SCR/(pow(AC_Voltage,2)/R_SCR);
  
  //SCR Outputs in bits
  SCR_Out_Set = int(-40.291 * pow(FFP,2) + 127.2 * FFP + 32.601);
}


//******Writes analog output D-A to SCR********
void Output_to_SCR(){
  analogWrite(SCR_Out_Pin, SCR_Out_Set);
}

//******Prints to the serial monitor***********
void Print_Stuff() {
  Serial.write(12);
  Serial.write(17);
  delay(5);
  Serial.print("AC Voltage: ");
  Serial.print(AC_Voltage);
  Serial.write(13);
  Serial.print("DC voltage: ");
  Serial.print(DC_Voltage);
  Serial.write(13);
  
  //Correlated SCR Output voltage
  SCR_Out_Voltage = (1.78e-4 * pow(SCR_Out_Set,2)) - (6.92e-3 * SCR_Out_Set) + 3.62e-1;
  
  Serial.print("SCR Out (V): ");
  Serial.print(SCR_Out_Voltage);
  Serial.write(13);
  Serial.print("SCR Out Set: ");
  Serial.print(SCR_Out_Set);
  
  delay(4000);
  
  Serial.write(12);
  Serial.write(17);
  Serial.print("Pump Power: ");
  Serial.print(Q_Pump);
  Serial.write(13);
  Serial.print("Base Heater: ");
  Serial.print(Q_Base);
  Serial.write(13);
  Serial.print("SCR Heater: ");
  Serial.print(Q_SCR);
  Serial.write(13);
  Serial.print("FFP: ");
  Serial.print(FFP);
}
