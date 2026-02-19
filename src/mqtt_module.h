
//= ======================== MQTT ===================================

// MQTT broker ( e.g. your PC running Mosquitto ) 
extern IPAddress mqttServer;
extern int mqttPort;

// MQTT client setup
extern PubSubClient *mqttClient;

extern persistConfig pconfig; 

extern Variable vars[];

//-----------------------------------------------------------------------------------------------
// MQTT message callback
void mqttCallback(char* topic, byte* payload, unsigned int length)  {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
//-----------------------------------------------------------------------------------------------
// Attempt to connect to MQTT broker
void mqttReconnect() {
  while (!mqttClient->connected()) {
    Serial.print("Attempting MQTT connection... ");
    if (mqttClient->connect("ESP8266_W5500_Client")) {
      Serial.println("connected!");
      mqttClient->subscribe("test/topic2");
      mqttClient->publish("test/topic", "ESP8266 + W5500 connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient->state());
      Serial.println(" — retrying in 5 seconds...");
      delay(5000);
    }
  }
}

//-----------------------------------------------------------------------------------------------

void tryReconnectMQTT() {
  Serial.print(F("Attempting MQTT connection... "));
  if (mqttClient->connect(pconfig.stationName.c_str())) {
    Serial.println(F("connected."));
    mqttClient->subscribe("test/topic");
    mqttClient->subscribe("test/topic2");

  } else {
    Serial.print(F("failed, rc="));
    Serial.println(mqttClient->state());
    // Do not block — just return, next retry will happen later
  }
}

//-----------------------------------------------------------------------------------------------

int publishVar(Variable *v) {
  char tagName[MAX_MQTT_TAG_LEN] , strValue[MAX_MQTT_TAG_LEN]; 
  unsigned long now = millis();

  strcpy( tagName, "metrics/" );
  strcat( tagName, pconfig.stationName.c_str() );
  strcat( tagName, "/" );
  strcat( tagName, v->name );
  sprintf(strValue, "%d", v->value );
  mqttClient->publish(tagName, strValue);
  Serial.print("Published: ");  
  Serial.print(tagName);  
  Serial.print(" -> ");  
  Serial.println(strValue);

  v->lastMQTTPublishTime = now;
  return(0);
  } 

//-----------------------------------------------------------------------------------------------

int   mqttHandle(PubSubClient *mqttClient) {

  static unsigned long lastMqttAttempt = 0;
  static unsigned long mqttRetryInterval = 10000; 
  static int counter = 0;
  static unsigned int  varNumber=0; 

  // Non-blocking MQTT connection management

  if (++counter >= 5) {
    counter = 0;
    if (pconfig.mqttServerURL.length() > 5)  {  // there is an MQTT server defined 

      if (!mqttClient->connected()) {
        unsigned long now = millis();
        if (now - lastMqttAttempt > mqttRetryInterval) {
          lastMqttAttempt = now;
          tryReconnectMQTT();
          }
        } 
      else {
        //char tagName[MAX_MQTT_TAG_LEN] , strValue[MAX_MQTT_TAG_LEN]; 
        unsigned long now = millis();
        
        mqttClient->loop(); // only runs if connected

        if ( vars[varNumber].mqttEnable ) {
          if ( ( ( (now - vars[varNumber].lastMQTTPublishTime) > ( vars[varNumber].MQTTMinUpdateTime * 1000) ) ) ) {
          //  && vars[varNumber].MQTTUpdateONChange > 0 && ( vars[varNumber].oldValue != vars[varNumber].value )  ) 
          //  || ( (now - vars[varNumber].lastMQTTPublishTime) > ( vars[varNumber].MQTTMaxUpdateTime * 1000) ) )  {

            publishVar( &( vars[varNumber] ));
            
            //strcpy( tagName, "metrics/" );
            //strcat( tagName, pconfig.stationName.c_str() );
            //strcat( tagName, "/" );
            //strcat( tagName, vars[varNumber].name );
            //sprintf(strValue, "%d", vars[varNumber].value );
            //mqttClient->publish(tagName, strValue);
            //Serial.print("Published: ");  
            //Serial.print(tagName);  
            //Serial.print(" -> ");  
            //Serial.println(strValue);

            //vars[varNumber].lastMQTTPublishTime = now;
            }  
          }
      // go through variable list, updating MQTT if required 
      varNumber = ( varNumber >= VAR_COUNT ) ? 0 : (varNumber+1);
      }
    }
  }  
  return (0);  
}  

//-----------------------------------------------------------------------------------------------