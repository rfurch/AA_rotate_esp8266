//-----------------------------------------------------------------------------------------------


// --- Helper: Encode credentials as Base64 (pre-encoded for efficiency)
// "admin:admin" →           "YWRtaW46YWRtaW4="
const char* AUTH_HEADER =    "YWRtaW46YWRtaW4=";
// use https://www.base64encode.org/

extern EthernetServer webServer;
extern persistConfig pconfig; 

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

// basic HTTP auth. 
bool checkAuth(EthernetClient &client) {
  String line;
  bool authorized = false;

  // Read HTTP headers
  while (client.available()) {
    line = client.readStringUntil('\n');
    line.trim();

    Serial.println(line);

    if (line.startsWith("Authorization: Basic ")) {
      String auth = line.substring(strlen("Authorization: Basic "));
      Serial.println(auth);
      if (auth == AUTH_HEADER) {
        authorized = true;
      }
    }

    if (line.length() == 0) break; // end of headers
  }

  if (!authorized) {
    // Request authentication
    client.println(F("HTTP/1.1 401 Unauthorized"));
    client.println(F("WWW-Authenticate: Basic realm=\"ESP8266 Access\""));
    client.println(F("Content-Type: text/html"));
    client.println(F("Connection: close"));
    client.println();
    client.println(F("<!DOCTYPE html><html><body><h3>401 Unauthorized</h3></body></html>"));
    client.stop();
  }

  return authorized;
}

//-----------------------------------------------------------------------------------------------

String getParam(String query, String name) {
  int pos = query.indexOf(name + "=");
  if (pos == -1) return "";
  int start = pos + name.length() + 1;
  int end = query.indexOf('&', start);
  if (end == -1) end = query.length();
  return query.substring(start, end);
}

//-----------------------------------------------------------------------------------------------

void parseQuery(String req, persistConfig *config) {
  int qPos = req.indexOf('?');
  int spPos = req.indexOf(' ', qPos);
  if (qPos == -1 || spPos == -1) return;
  
  String query = req.substring(qPos + 1, spPos);
  Serial.println("Query: " + query);

  // Split parameters
  int p1 = query.indexOf('&');
  int p2 = query.indexOf('&', p1 + 1);
  p2 = p2;

  String ipStr   = getParam(query, "ip");
  String maskStr = getParam(query, "mask");
  String gwStr   = getParam(query, "gateway");

  String modbusport   = getParam(query, "modbusport");
  String webserverport   = getParam(query, "webserverport");
  String ssid   = getParam(query, "ssid");
  String pass   = getParam(query, "pass");
  String mqttserverurl   = getParam(query, "mqttserverurl");
  String mqttserverport   = getParam(query, "mqttserverport");
  String ntpserverurl   = getParam(query, "ntpserverurl");
  String ntpserverport   = getParam(query, "ntpserverport");
  String logserverurl   = getParam(query, "logserverurl");
  String logserverport   = getParam(query, "logserverport");
  String stationName   = getParam(query, "stationName");

  String snmpROCommunity   = getParam(query, "snmpROCommunity");
  String snmpWRCommunity   = getParam(query, "snmpWRCommunity");

  String temp1   = getParam(query, "temp1");
  String temp2   = getParam(query, "temp2");
  String temp3   = getParam(query, "temp3");

  String cycleHours = getParam(query, "cyclehours");

  String mac0   = getParam(query, "mac0");
  String mac1   = getParam(query, "mac1");
  String mac2   = getParam(query, "mac2");
  String mac3   = getParam(query, "mac3");
  String mac4   = getParam(query, "mac4");
  String mac5   = getParam(query, "mac5");

  String snmpRestartMinutes   = getParam(query, "snmpRestartMinutes");
  String watchdogSeconds   = getParam(query, "watchdogSeconds");

  IPAddress newIP, newMask, newGW;
  if (ipStr.length() && newIP.fromString(ipStr))   config->ip = newIP;
  if (maskStr.length() && newMask.fromString(maskStr)) config->mask = newMask;
  if (gwStr.length() && newGW.fromString(gwStr))   config->gateway = newGW;

  config->mbPort = modbusport.toInt();
  config->webPort = webserverport.toInt();
  config->ssid = ssid;
  config->pass = pass;

  config->mqttServerURL = mqttserverurl;
  config->mqttServerPort = mqttserverport.toInt();

  config->NTPServerURL = ntpserverurl;
  config->NTPServerPort = ntpserverport.toInt();

  config->LOGServerURL = logserverurl;
  config->LOGServerPort = logserverport.toInt();

  config->t1 = temp1.toInt();
  config->t2 = temp2.toInt();
  config->t3 = temp3.toInt();

  config->cycleHours = cycleHours.toInt();

  config->cycleHours = (config->cycleHours < CYCLE_HOURS_MIN) ? CYCLE_HOURS_MIN : config->cycleHours;
  config->cycleHours = (config->cycleHours > CYCLE_HOURS_MAX) ? CYCLE_HOURS_MAX : config->cycleHours;
  
  config->stationName = stationName;

  config->snmpROCommunity = snmpROCommunity;
  config->snmpWRCommunity = snmpWRCommunity;

  config->mac[0] = strtol(mac0.c_str(), NULL, 16);
  config->mac[1] = strtol(mac1.c_str(), NULL, 16);
  config->mac[2] = strtol(mac2.c_str(), NULL, 16);
  config->mac[3] = strtol(mac3.c_str(), NULL, 16);
  config->mac[4] = strtol(mac4.c_str(), NULL, 16);
  config->mac[5] = strtol(mac5.c_str(), NULL, 16);

  config->snmpRestartMinutes = snmpRestartMinutes.toInt();    // time to reset and restart udp server  for SNMP.  zero ->  no restart
  config->watchdogSeconds = watchdogSeconds.toInt();          // time to reset board using watchdog zero -> no restart

  Serial.println("New IP: " + config->ip.toString());
  Serial.println("New Mask: " + config->mask.toString());
  Serial.println("New GW: " + config->gateway.toString());
  Serial.println("snmpRestartMinutes: " + config->snmpRestartMinutes);
  Serial.println("watchdogSeconds: " + config->watchdogSeconds);
}

//-----------------------------------------------------------------------------------------------

void sendHTML(EthernetClient &client, persistConfig *config) {
    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-Type: text/html"));
    client.println(F("Connection: close"));
    client.println();
    client.println(F("<!DOCTYPE html><html lang='en'><head>"));
    client.println(F("<meta name='viewport' content='width=device-width, initial-scale=1.0'>"));

    client.println(F("<style>"));
    client.println(F("body { font-family: Arial, sans-serif; background: linear-gradient(135deg,#74ABE2,#5563DE);"));
    client.println(F("color:#222; text-align:center; margin:0; padding:0;}"));
    client.println(F(".card { background:#fff; max-width:700px; margin:60px auto; padding:25px; "));
    client.println(F("border-radius:12px; box-shadow:0 4px 10px rgba(0,0,0,0.15);}"));
    client.println(F("h2 { color:#334; margin-bottom:20px;}"));
    client.println(F("form input[type='text'] { width:60%; padding:8px; margin:8px 0; border:1px solid #ccc; border-radius:6px;}"));
    client.println(F("form input[type='submit'] { background-color:#4CAF50; color:white; border:none; "));
    client.println(F("padding:10px 20px; margin-top:10px; border-radius:6px; cursor:pointer; font-weight:bold;}"));
    client.println(F("form input[type='submit']:hover { background-color:#45a049;}"));
    client.println(F(".info { text-align:left; margin-top:15px;} .info b { display:inline-block; width:90px;}"));
    client.println(F(".center-container { text-align: center; } " ));

    client.println(F("input, label { width: 280px; max-width: 300px; box-sizing: border-box;"));

    client.println(F("</style>"));

    client.println(F("</head><body>"));

    client.println(F("<div class='card'><h2>ESP8266 + W5500 - Configuration</h2><div class='info'>"));

    client.println(F(  "<style>"  "label {display:inline-block; width:300px; font-weight:bold;}"));
    client.println(F(  "input {width:100px; margin-bottom:8px;}" "</style>"  ));

    client.println(F("<form method='GET' >"));

    client.print(F("<label>IP:</label><input type='text' name='ip' value='"));
    client.print(config->ip);
    client.println(F("'>"));
    //client.println(F("'><br>"));

    client.print(F("<label>Mask:</label><input type='text' name='mask' value='"));
    client.print(config->mask);
    client.println(F("'>"));

    client.print(F("<label>Gateway:</label><input type='text' name='gateway' value='"));
    client.print(config->gateway);
    client.println(F("'><br>"));

    client.print(F("<label>MODBUS TCP PORT:</label><input type='text' name='modbusport' value='"));
    client.print(config->mbPort);
    client.println(F("'><br>"));

    client.print(F("<label>WEB SERVER TCP PORT:</label><input type='text' name='webserverport' value='"));
    client.print(config->webPort);
    client.println(F("'><br>"));    

    client.print(F("<label>WiFi SSID:</label><input type='text' name='ssid' value='"));
    client.print(config->ssid);
//    client.println(F("'><br>")); 
    client.println(F("'>")); 

    client.print(F("<label>WiFi pass:</label><input type='text' name='pass' value='"));
    client.print(config->pass);
    client.println(F("'><br>")); 

    client.print(F("<label>MQTT Server URL:</label><input type='text' name='mqttserverurl' value='"));
    client.print(config->mqttServerURL);
    //client.println(F("'><br>"));     
    client.println(F("'>"));     
    
    client.print(F("<label>MQTT Server Port:</label><input type='text' name='mqttserverport' value='"));
    client.print(config->mqttServerPort);
    client.println(F("'><br>")); 

    client.print(F("<label>NTP Server URL:</label><input type='text' name='ntpserverurl' value='"));
    client.print(config->NTPServerURL);
//    client.println(F("'><br>"));    
    client.println(F("'>"));    
        
    client.print(F("<label>NTP Server Port:</label><input type='text' name='ntpserverport' value='"));
    client.print(config->NTPServerPort);
    client.println(F("'><br>")); 

    client.print(F("<label>LOG Server URL:</label><input type='text' name='logserverurl' value='"));
    client.print(config->LOGServerURL);
    //client.println(F("'><br>"));     
    client.println(F("'>"));     
    
    client.print(F("<label>LOG Server Port:</label><input type='text' name='logserverport' value='"));
    client.print(config->LOGServerPort );
    client.println(F("'><br>")); 

    client.print(F("<label> Temp 1 (cycling):</label><input type='text' name='temp1' value='"));
    client.print(config->t1);
    client.println(F("'><br>")); 
    
    client.print(F("<label> Temp 2 (both on):</label><input type='text' name='temp2' value='"));
    client.print(config->t2);
    client.println(F("'><br>")); 
    
    client.print(F("<label> Temp 3 (alarm):</label><input type='text' name='temp3' value='"));
    client.print(config->t3);
    client.println(F("'><br>")); 

    client.print(F("<label>Cycle Hours (min: 6 max: 240):</label><input type='text' name='cyclehours' value='"));
    client.print(config->cycleHours);
    client.println(F("'><br>")); 

    client.print(F("<label>Station Name:</label><input type='text' name='stationName' value='"));
    client.print(config->stationName);
    client.println(F("'><br>")); 

    client.print(F("<label>SNMP RO Community:</label><input type='text' name='snmpROCommunity' value='"));
    client.print(config->snmpROCommunity);
    client.println(F("'>")); 

    client.print(F("<label>SNMP WR Community:</label><input type='text' name='snmpWRCommunity' value='"));
    client.print(config->snmpWRCommunity);
    client.println(F("'><br>")); 

    client.print(F("<label style='display:inline-block; width:200px;'>MAC (hex):</label>"));
    client.print(F("<input type='text' name='mac0' style='width:50px;' value='"));
    client.print(config->mac[0],HEX);
    client.println(F("'>")); 
    client.print(F("<input type='text' name='mac1' style='width:50px;' value='"));
    client.print(config->mac[1],HEX);
    client.println(F("'>")); 
    client.print(F("<input type='text' name='mac2' style='width:50px;' value='"));
    client.print(config->mac[2],HEX);
    client.println(F("'>")); 
    client.print(F("<input type='text' name='mac3' style='width:50px;' value='"));
    client.print(config->mac[3],HEX);
    client.println(F("'>")); 
    client.print(F("<input type='text' name='mac4' style='width:50px;' value='"));
    client.print(config->mac[4],HEX);
    client.println(F("'>")); 
    client.print(F("<input type='text' name='mac5' style='width:50px;' value='"));
    client.print(config->mac[5],HEX);
    client.println(F("'>")); 

    client.println(F("<br>")); 

    client.print(F("<label>SNMP restart minutes (or 0):</label><input type='text' name='snmpRestartMinutes' style='width:50px;' value='"));
    client.print(config->snmpRestartMinutes);
    client.println(F("'>")); 
    client.println(F("<br>")); 
    client.print(F("<label>watcdog restart seconds (or 0):</label><input type='text' name='watchdogSeconds' style='width:50px;' value='"));
    client.print(config->watchdogSeconds);
    client.println(F("'>")); 
    client.println(F("<br>")); 

    client.println(F("<hr>")); 
      
   // --- Confirmation button ---

    client.println(F( " <p class='center-container'> "));
    client.println(F( " <button type='submit' "));
    client.println(F( " onclick=\"return confirm('Are you sure you want to save these settings and restart?');\" >"));
    client.println(F( " Save Settings </button>" ));
    client.println(F( " </p> " ));

    client.println(F("<hr>")); 

    client.println(F("<div class='card'><h2>Real Time Data (refresh to update)</h2><div class='info'>"));

    client.print(F("<label> Temp sensor 1:  " ));
    client.print( (1.0 * vars[VAR_TEMP1].value) / 100.0 , 1);
    client.println(F(" &deg;C </label> <br>"));

    client.print(F("<label> Temp sensor 2:  " ));
    client.print( (1.0 * vars[VAR_TEMP2].value) / 100.0 , 1);
    client.println(F(" &deg;C </label> <br>"));

    client.print(F("<label> Selected TEMP (C):  " ));
    client.print( (1.0 * vars[VAR_TEMP3].value) / 100.0 , 1);
    client.println(F(" &deg;C </label> <br>"));

    client.print(F("<label> Minutes to cycle / rotate:  " ));
    client.print( vars[VAR_CycMinRem].value , 1);
    client.println(F("</label> <br>"));

    client.print(F("<label> Heartbeat:  " ));
    client.print( vars[VAR_LIFE].value , 1);
    client.println(F("</label> <br>"));

    client.println(F("<hr>")); 

client.println(F("</form></body></html>"));

}

//-----------------------------------------------------------------------------------------------


  int processWebServer() {

    EthernetClient client = webServer.available();
    if (client) {
      Serial.println("Requesting Auth !!!!");

    // --- Parse the first line of the HTTP request
    String requestLine = client.readStringUntil('\n');
    requestLine.trim();

    // Handle only GET/POST
    if (!requestLine.startsWith("GET") && !requestLine.startsWith("POST")) {
      client.println(F("HTTP/1.1 405 Method Not Allowed"));
      client.println(F("Connection: close"));
      client.println();
      client.stop();
      return(0);
      }

    // --- Authentication check
    if (checkAuth(client)) { 

      // --- Extract parameters (if any)
      if (requestLine.indexOf("GET /?") >= 0) {
        int ipIndex = requestLine.indexOf("ip=");
        if (ipIndex >= 0) {
          // You could parse the form values here to update IP/gateway/mask
          Serial.println("Received form submission:");
          Serial.println(requestLine);
          parseQuery(requestLine, &pconfig);
          Serial.print("New IP: "); Serial.print(pconfig.ip);
          Serial.print(" - "); Serial.print(pconfig.mask);
          Serial.print(" - "); Serial.println(pconfig.gateway);

          saveConfig(pconfig);    
          printConfig  (pconfig);  

          client.println(F("Connection: close"));
          client.println();
          client.stop();

          delay(200);
          ESP.restart();
        }
      }

      // --- Send main config page
      sendHTML(client, &pconfig);
      delay(200);
      client.flush();
      client.stop();
      Serial.println("Client disconnected");
      }
    }

  return(0);

  }

  //-----------------------------------------------------------------------------------------------
  //-----------------------------------------------------------------------------------------------