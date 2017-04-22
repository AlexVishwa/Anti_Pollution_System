#include <SPI.h>    
#include <Ethernet.h>                            
#include <PubSubClient.h>
// include the library code:
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins

// MAC Address of Arduino Ethernet (Sheild)
byte mac[] = { 0x98, 0x4f, 0xee, 0x01, 0xc4, 0xc8 };

#define API_KEY         "2ecc4f7f-2550-4173-9f62-83876e738ad1"
#define PROJECT_ID      "4743"
#define AN_SENSOR_NAME1 "motion_sensor"
#define AN_SENSOR_NAME2 "VERIFICATION"
#define DEVICE_UUID     "90463766-1170-43d4-8482-d66ddc8cfbf6"
#define sec             1000

char clientId[]       = "Arduino_Ethernet";
char sensorTopic1[]    = "/a/"API_KEY"/p/"PROJECT_ID"/d/"DEVICE_UUID"/sensor/"AN_SENSOR_NAME1"/data";
char sensorTopic2[]    = "/a/"API_KEY"/p/"PROJECT_ID"/d/"DEVICE_UUID"/sensor/"AN_SENSOR_NAME2"/data";
// server mqtt.devicehub.net ip
char server[]         = "104.155.7.31";
char message_buffer1[20];
char message_buffer2[20];

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int analogInPin2 = A1 ; 
unsigned int analog_sensor1 = 0;        // value read from the potg
unsigned int analog_sensor2 = 0;        // value output to the PWM (analog out)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
EthernetClient apiClient;

void callback(char* topic, byte* payload, unsigned int length)
{
  // handle message arrived
}

PubSubClient client(server, 1883, callback, apiClient);

boolean conn = false;
unsigned long time;

void setup(void)  
{
  // init serial for debugging
  Serial.begin(9600);
    lcd.begin(16, 2);

  if (Ethernet.begin(mac) == 0)  
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac);
  }

  Serial.println("\nStarting connection to server...");
  
  if(client.connect(clientId))  
  {
    conn = true;
    Serial.println("Successfuly connected and running!");
  }
  else  
  {
    Serial.println("Connection problem");
  }

}

void loop(void)  
{
  //if client it's not connected or disconnects here we try to reconnect
  if (!client.connected())
  { 
    Serial.println("reconnecting ...");
    client.connect(clientId);
    delay(3*sec); 
    sendData();  
  }

  sendData();
}

void sendData(void) 
{
  //simulation for analog sensors
  analog_sensor1 = analogRead(analogInPin);
  analog_sensor1 = analog_sensor1*4.88;

  
  analog_sensor2 = analogRead(analogInPin2);
  analog_sensor2 = analog_sensor2*4.88;
  lcd.setCursor(0,0); Serial.print("CO2(mv)");  lcd.print("CO2(mV)");
  Serial.print(analog_sensor1); lcd.print(analog_sensor1);
  lcd.setCursor(0,1); Serial.print("\tCO(mv)"); lcd.print("CO(mV)");
  Serial.println(analog_sensor2); lcd.print(analog_sensor2);
  delay(1000);
  lcd.clear();
   analog_sensor1= ((analog_sensor1*1996+10)*0.001);
  analog_sensor2=(analog_sensor2)*0.5;
  lcd.setCursor(0,0); Serial.print("CO2(ppm)");  lcd.print("CO2(ppm)");
  Serial.print(analog_sensor1); lcd.print(analog_sensor1);
  lcd.setCursor(0,1); Serial.print("\tCO(ppm)"); lcd.print("CO(ppm)");
  Serial.println(analog_sensor2); lcd.print(analog_sensor2);
  delay(1000);
  lcd.clear();
  if(conn) 
  {
    //publishing new value every 5 seconds
    if (millis() > (time + 5000)) 
    {
      time = millis();
      
      String pubString1 = "{\"value\": " + String(analog_sensor1) + "}";
      String pubString2 = "{\"value\": " + String(analog_sensor2) + "}";

      pubString1.toCharArray(message_buffer1, pubString1.length()+1);
      pubString2.toCharArray(message_buffer2, pubString2.length()+1);
    
      Serial.print("Publishing new value: ");
      Serial.println(analog_sensor1);
      Serial.println(analog_sensor2);   
      client.publish(sensorTopic1, message_buffer1);
      client.publish(sensorTopic2, message_buffer2);
    }
    // MQTT client loop processing
    client.loop();
  }
}
