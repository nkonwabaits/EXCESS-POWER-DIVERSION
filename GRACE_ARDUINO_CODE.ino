//#include <Boards.h>
//#include <Firmata.h>
//#include <FirmataConstants.h>
//#include <FirmataDefines.h>
//#include <FirmataMarshaller.h>
//#include <FirmataParser.h>

//#include <SPI.h>  //SPI serial communication with the SD card
//#include <SD.h>  //SD Card library
#include <DS3231.h>   //REAL TIME CLOCK LIBRARY

DS3231  rtc(SDA, SCL);  //'rtc' is the object for real time clock usage

//NAMING ARDUINO PINS
const int chipSelect = 4;
int excessEnergyLoad = 8; // Arduino digital pin for Excess energy load RELAY 
int chargeControllerConnect = 7; // Arduino digital pin for charge controller RELAY (connects panel to charge controller)***(formerly)****
int batteryLoad = 6; // Arduino digital pin for battery load
int spare_relay = 5; //New charge controller connect digital pin ************************

int i, j;
float batteryVoltage_ADC;
float panelPositiveVoltage_ADC;
float panelNegativeVoltage_ADC;
float panelCurrent_ADC;



void setup() {
  // SETTING ARDUINO DIGITAL PIN PURPOSES
  pinMode(excessEnergyLoad, OUTPUT);
  pinMode(chargeControllerConnect, OUTPUT);
  pinMode(batteryLoad, OUTPUT);
  pinMode(spare_relay, OUTPUT);

  //connect panel to charge controller by default =
  digitalWrite(spare_relay, HIGH);
  //digitalWrite(excessEnergyLoad, HIGH);
  
  Serial.begin(9600); // Begins serial communication to the serial monitor
  rtc.begin(); //Begins serial communication with the real time clock
  

}

void loop() {
  
  String battery_voltage_str, panel_voltage_str, panel_current_str;   //Variables store measurements in string form
  char bv[10], pv[10], pc[10]; // variables store measurements in character array form
  
  String current_time_str = rtc.getTimeStr();  // acquires & stores current time from the real time clock
  String current_date_str = rtc.getDateStr();  // acquires & stores current date from real time clock


  //Writes time and date to serial monitor
  Serial.println("***");
  Serial.print("Current Time: ");
  Serial.println(current_time_str);
  Serial.println("***");


  //Filters out hour of day
  String current_hrs_str = current_time_str.substring(0, 2);
  int current_hrs = current_hrs_str.toInt();
  
  
  // measurement of analog values
  int batteryVoltage_ADC_sum = 0;
  int panelPositiveVoltage_ADC_sum = 0;
  int panelNegativeVoltage_ADC_sum = 0;
  int panelCurrent_ADC_sum = 0;
  

  for(i=0; i<30; i++){
  batteryVoltage_ADC_sum = batteryVoltage_ADC_sum + analogRead(A0);
  delay(50);
  panelPositiveVoltage_ADC_sum = panelPositiveVoltage_ADC_sum + analogRead(A1);
  delay(50);
  panelNegativeVoltage_ADC_sum = panelNegativeVoltage_ADC_sum + analogRead(A3);
  delay(50);
  panelCurrent_ADC_sum = panelCurrent_ADC_sum + analogRead(A2);
  delay(50);
  }

  batteryVoltage_ADC = batteryVoltage_ADC_sum/30.0;
  panelPositiveVoltage_ADC = panelPositiveVoltage_ADC_sum/30.0;
  panelNegativeVoltage_ADC = panelNegativeVoltage_ADC_sum/30.0;
  panelCurrent_ADC = panelCurrent_ADC_sum/30.0;
  
  Serial.println("*****************************************************************************");
  //wites analog measurements to serial monitor
  Serial.print("Battery voltage ADC = ");
  Serial.println(batteryVoltage_ADC);
  Serial.print("Panel Positive voltage ADC = ");
  Serial.println(panelPositiveVoltage_ADC);
  Serial.print("Panel Negative voltage ADC = ");
  Serial.println(panelNegativeVoltage_ADC);
  Serial.print("Panel current ADC = ");
  Serial.println(panelCurrent_ADC);
  
  Serial.println("*****************************************************************************");
  //converts analog measurements to actual voltages
  double Vcc = 5.05;
  Serial.print("VCC = ");
  Serial.println(Vcc);
  
  double A0_voltage = (batteryVoltage_ADC / 1023.0) * Vcc;
  double A1_voltage = (panelPositiveVoltage_ADC / 1023.0) * Vcc;
  double A3_voltage = (panelNegativeVoltage_ADC / 1023.0) * Vcc;
  double A2_voltage = (panelCurrent_ADC / 1023.0) * Vcc;
  
  //writes actual voltages to serial monitor
  Serial.print("A0 voltage = ");
  Serial.println(A0_voltage, 4);
  Serial.print("A1 voltage = ");
  Serial.println(A1_voltage, 4);
  Serial.print("A3 voltage = ");
  Serial.println(A3_voltage, 4);
  Serial.print("A2 voltage = ");
  Serial.println(A2_voltage, 4);
  
  Serial.println("*****************************************************************************");

  //converts actual voltages to real voltages 
  double battery_voltage = ((12.98 / 3.8998) * A0_voltage);
  double panel_positive_voltage = ((12.76 / 2.7776) * A1_voltage);
  double panel_negative_voltage = ((12.76 / 1.6019) * A3_voltage);
  double panel_voltage = panel_positive_voltage - panel_negative_voltage;
  if(panel_voltage < 0.0){
     
     panel_voltage = 0.00;
   
   }
  //panel_current = ((((panel_current_ADC / 1023.0) * 5000) - 2500) / 10.0);
  double panel_current = (((A2_voltage * 1000) - 2506.6) / 10.0);
   if(panel_current < 0.0){
     
     panel_current = 0.00;
   
   }
  //writes real voltages to serial monitor
  Serial.print("Battery voltage = ");
  Serial.println(battery_voltage);
  Serial.print("Panel voltage = ");
  Serial.println(panel_voltage);
  Serial.print("Panel current = ");
  Serial.println(panel_current);

  //converts real voltages to string form
  dtostrf(battery_voltage, 4, 2, bv);
  dtostrf(panel_voltage, 4, 2, pv);
  dtostrf(panel_current, 4, 2, pc);
  
  battery_voltage_str = bv;
  panel_voltage_str = pv;
  panel_current_str = pc;

  //writes string form of real voltages to serial monitor
  Serial.println(battery_voltage_str);
  Serial.println(panel_voltage_str);
  Serial.println(panel_current_str);
  
  Serial.println("*****************************************************************************");
  Serial.println("*****************************************************************************");

  
  //LOGIC
  
  if(battery_voltage > 13.60 && panel_voltage > 12.0){ //checks if battery voltage is greater than 13.60 and if panel voltage is greater than 12.0

    if(current_hrs > 9 && current_hrs < 18){ //checks if current time (hours) is later than 9am and earlier than 6pm
    
      digitalWrite(spare_relay, LOW);  // disconnects charge controller from panel
      digitalWrite(excessEnergyLoad, HIGH); //  connects panel to excess load

    }
  
  }
  
  if(current_hrs > 17){ // checks if current time is later than 5pm

    digitalWrite(excessEnergyLoad, LOW); //disconnects excess energy load form panels

    if(battery_voltage > 12.0){ // checks if battery voltage is greater than 12.8
      
      digitalWrite(batteryLoad, HIGH); // turns on inverter

    }
  
  }


  if(battery_voltage < 11.5){  // checks if battery voltage is less than 12.4

    digitalWrite(batteryLoad, LOW); // turns off inverter 
    
  }

  if(current_hrs == 6 && battery_voltage < 13.5){  // checks if current time is arounf 6am and if battery voltage is less than 13.7

    digitalWrite(spare_relay, HIGH); //connects charge controller to panel
    digitalWrite(batteryLoad, LOW);  // turns off inverter
    
  }
  
  ////////////////////////////////////////////////////////SD CARD WRITE/////////////////////////////////////////////////////
  
  
 delay(0);

}
