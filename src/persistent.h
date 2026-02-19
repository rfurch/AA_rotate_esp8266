

//-----------------------------------------------------------------------------------------------

typedef struct persistConfig {
  IPAddress ip;
  IPAddress mask;
  IPAddress gateway;
  int       mbPort;
  int       webPort;
  String    ssid;
  String    pass;
  String    mqttServerURL;
  int       mqttServerPort;
  byte      mac[6];
  int       bootCount;
  String    NTPServerURL;
  int       NTPServerPort;  
  int       t1;
  int       t2;
  int       t3;
  String    LOGServerURL;
  int       LOGServerPort;  
  int       cycleHours;
  String    stationName;
  String    snmpROCommunity;
  String    snmpWRCommunity;
  int       snmpRestartMinutes;    // time to reset and restart udp server  for SNMP.  zero ->  no restart
  int       watchdogSeconds;       // time to reset board using watchdog zero -> no restart

}persistConfig;

#define FS_FILE_NAME "/config.json"

//-----------------------------------------------------------------------------------------------

bool loadConfig(persistConfig &config) {

  if (!LittleFS.begin()) {
    Serial.println("Error montando LittleFS");
    return false;
    }

  if (!LittleFS.exists(FS_FILE_NAME)) {
    Serial.println("Archivo de config no encontrado, se usará por defecto");
    return false;
    }

  File file = LittleFS.open(FS_FILE_NAME, "r");
  if (!file) {
    Serial.print("Error abriendo config: ");
    Serial.println(FS_FILE_NAME);
    return false;
    }

  JsonDocument doc; 
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println("Error leyendo JSON");
    return false;
  }

  // dump loaded values into config structure
  config.ip.fromString(doc["ip"].as<String>()); 
  config.mask.fromString(doc["subnet"].as<String>()); 
  config.gateway.fromString(doc["gw"].as<String>()); 
  
  config.ssid            = doc["ssid"].as<String>();
  config.pass            = doc["pass"].as<String>();
  config.mqttServerURL   = doc["mqttServerURL"].as<String>();
  config.mqttServerPort  = doc["mqttServerPort"] | 0;
  
  config.bootCount   = doc["bootCount"] | 0;
  config.mbPort   = doc["mbPort"] | 0;
  config.webPort   = doc["webPort"] | 0;
   
  config.mac[0] = doc["mac0"] | 0;
  config.mac[1] = doc["mac1"] | 0;
  config.mac[2] = doc["mac2"] | 0;
  config.mac[3] = doc["mac3"] | 0;
  config.mac[4] = doc["mac4"] | 0;
  config.mac[5] = doc["mac5"] | 0;

  config.NTPServerURL = doc["NTPServerURL"].as<String>();
  config.NTPServerPort = doc["NTPServerPort"] | 0;

  config.LOGServerURL = doc["LOGServerURL"].as<String>();
  config.LOGServerPort = doc["LOGServerPort"] | 0;

  config.t1   = doc["t1"] | 0;
  config.t2   = doc["t2"] | 0;
  config.t3   = doc["t3"] | 0;
  
  config.cycleHours = doc["cycleHours"] | 1;
  
  config.stationName = doc["stationName"].as<String>();
  config.snmpROCommunity = doc["snmpROCommunity"].as<String>();
  config.snmpWRCommunity = doc["snmpWRCommunity"].as<String>();

  config.snmpRestartMinutes = doc["snmpRestartMinutes"] | 0;
  config.watchdogSeconds = doc["watchdogSeconds"] | 0;

  return true;
}

//-----------------------------------------------------------------------------------------------

bool saveConfig(persistConfig &config) {
  JsonDocument doc;

  doc["ssid"]             = config.ssid;
  doc["pass"]             = config.pass;

  doc["ip"]               = config.ip.toString();
  doc["subnet"]           = config.mask.toString();
  doc["gw"]               = config.gateway.toString();

  doc["mqttServerURL"]    = config.mqttServerURL;
  doc["mqttServerPort"]   = config.mqttServerPort;
  
  doc["bootCount"]        = config.bootCount;
  doc["mbPort"]           = config.mbPort;
  doc["webPort"]          = config.webPort;

  doc["mac0"]             = config.mac[0];
  doc["mac1"]             = config.mac[1];
  doc["mac2"]             = config.mac[2];
  doc["mac3"]             = config.mac[3];
  doc["mac4"]             = config.mac[4];
  doc["mac5"]             = config.mac[5];
  
  doc["NTPServerURL"]    = config.NTPServerURL;
  doc["NTPServerPort"]   = config.NTPServerPort;  

  doc["LOGServerURL"]    = config.LOGServerURL;
  doc["LOGServerPort"]   = config.LOGServerPort;    

  doc["t1"]              = config.t1;
  doc["t2"]              = config.t2;
  doc["t3"]              = config.t3;
  doc["cycleHours"]      = config.cycleHours;

  doc["stationName"]     = config.stationName;
  doc["snmpROCommunity"] = config.snmpROCommunity;
  doc["snmpWRCommunity"] = config.snmpWRCommunity;

  doc["snmpRestartMinutes"] = config.snmpRestartMinutes;
  doc["watchdogSeconds"] = config.watchdogSeconds;

  File file = LittleFS.open(FS_FILE_NAME, "w");
  if (!file) {
    Serial.print("Error abriendo config: ");
    Serial.println(FS_FILE_NAME);
    return false;
    }
  else   
   Serial.println("!!!! Saving file to LittleFS");
 
  serializeJsonPretty(doc, file);
  file.close();
  return true;
}

//-----------------------------------------------------------------------------------------------

void  printConfig(persistConfig &config) {

    Serial.println("Persistent config:");
    Serial.print("IP: ");
    Serial.print( config.ip.toString() );
    Serial.print(" subnet: ");
    Serial.print( config.mask.toString() );
    Serial.print(" GW: ");
    Serial.print( config.gateway.toString() );

    Serial.println (" ");

    for (unsigned int i = 0; i < sizeof(config.mac); i++) {
        if (config.mac[i] < 0x10) { // Add a leading zero if the hex value is less than 0x10 (single digit)
        Serial.print("0");
        }
        Serial.print(config.mac[i], HEX); // Print the byte in hexadecimal format
        if (i < sizeof(config.mac) - 1) {
        Serial.print(":"); // Print a colon separator between bytes
        }
        }
    Serial.println(); // Print a newline at the end

    Serial.print("WiFi SSID: ");
    Serial.print(config.ssid);
    Serial.print(" pass: ");
    Serial.print(config.pass);

    Serial.println(); // Print a newline at the end

    Serial.print("MQTT Server: ");
    Serial.print(config.mqttServerURL);
    Serial.print(" MQTT Server PORT: ");
    Serial.print(config.mqttServerPort);
    
    Serial.println(); // Print a newline at the end

    Serial.print("BOOT Count : ");
    Serial.println(config.bootCount);

    Serial.println(); // Print a newline at the end
 
    Serial.print(" Modbus port: ");
    Serial.println(config.mbPort);
    Serial.print(" WEB port: ");
    Serial.println(config.webPort);
     
    Serial.print(" NTPServerURL : ");
    Serial.println(config.NTPServerURL);
    Serial.print(" NTPServerPort: ");
    Serial.println(config.NTPServerPort);
 
    Serial.print(" LOGServerURL : ");
    Serial.println(config.LOGServerURL);
    Serial.print(" LOGServerPort: ");
    Serial.println(config.LOGServerPort);
        
    Serial.print(" Cycle Days: ");
    Serial.println(config.cycleHours);
        
    Serial.print(" StationName: ");
    Serial.println(config.stationName);  
    
    Serial.print(" snmpROCommunity: ");
    Serial.println(config.snmpROCommunity);  

    Serial.print(" snmpWRCommunity: ");
    Serial.println(config.snmpWRCommunity);  

    Serial.print(" snmpRestartMinutes: ");
    Serial.println(config.snmpRestartMinutes);  

    Serial.print(" watchdogSeconds: ");
    Serial.println(config.watchdogSeconds);  
    
    Serial.println(); // Print a newline at the end

    }

//-----------------------------------------------------------------------------------------------

// default config in case there is no persistent data stored

void  loadDefaultConfig(persistConfig &defaultConfig) {

    defaultConfig.ip = IPAddress(10, 129, 6 ,230);
    defaultConfig.mask = IPAddress(255, 255, 255, 0);
    defaultConfig.gateway = IPAddress(10, 129, 6 ,1);

    // !!!!!!!!!!!!!!! ATTENTION:  modify this MAC for every UNIT!!!!!
    defaultConfig.mac[0] = 0xDE; defaultConfig.mac[1] = 0xAD; defaultConfig.mac[2] = 0xBE; 
    defaultConfig.mac[3] = 0x01; defaultConfig.mac[4] = 0x01; defaultConfig.mac[5] = 0x03;
    defaultConfig.mbPort = 502;
    defaultConfig.webPort = 80;
    defaultConfig.ssid = "none";
    defaultConfig.pass = "none";
    defaultConfig.mqttServerURL = "127.0.0.1";
    defaultConfig.mqttServerPort = 8888;
    defaultConfig.bootCount = 1;
    defaultConfig.NTPServerURL = "127.0.0.1";
    defaultConfig.NTPServerPort = 8888;
    defaultConfig.LOGServerURL = "none";
    defaultConfig.LOGServerPort = 8888;
    defaultConfig.t1 = 14;    
    defaultConfig.t2 = 22;    
    defaultConfig.t3 = 16;    
    defaultConfig.cycleHours = 12;
    defaultConfig.stationName = "Station00011";

    defaultConfig.snmpROCommunity = "public";
    defaultConfig.snmpWRCommunity = "private";

    defaultConfig.snmpRestartMinutes = 0;
    defaultConfig.watchdogSeconds = 0;

    }

//-----------------------------------------------------------------------------------------------


int   handlePersistentconfig(persistConfig &pconfig) {

  if (!loadConfig(pconfig)) {
    Serial.println("No persistent config!   Saving default values....");
    loadDefaultConfig(pconfig);
    saveConfig(pconfig);
    printConfig(pconfig);
    }
  else {
    Serial.println(" Persistent config loaded!");
    printConfig(pconfig);
    pconfig.bootCount++;
    saveConfig(pconfig);
    }
  return(0);  
  }  

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

