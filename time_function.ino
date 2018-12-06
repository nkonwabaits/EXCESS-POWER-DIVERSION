long int currenTimeSecs(){
  
  String current_time_str = rtc.getTimeStr();
  Serial.println("");
  Serial.println(current_time_str);
  
  String current_hrs_str = current_time_str.substring(0, 2);
  
  Serial.println(current_hrs_str);
  
  String current_mins_str = current_time_str.substring(3, 5);
  
  Serial.println(current_mins_str);
  
  String current_secs_str = current_time_str.substring(6, 8);
  
  Serial.println(current_secs_str);
  
  int current_hrs = current_hrs_str.toInt(); 
  int current_mins = current_mins_str.toInt();
  int current_secs = current_secs_str.toInt();
  
  //////////////////////////////// PRINT INTEGER TIMES //////////////////////////////////////////
  
  //Serial.println(current_hrs);
  //Serial.println(current_mins);
  //Serial.println(current_secs);
  
  return ((current_hrs*3600) + (current_mins*60) + current_secs);

}
