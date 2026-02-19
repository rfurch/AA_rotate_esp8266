// main file, setup, loop

#include <Arduino.h>
#include <SPI.h>
#include "Ethernet_Generic.h"
#include <PubSubClient.h>
#include <EthernetUdp.h>
#include <SNMP_Agent.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include <NTPClient.h>

#include "persistent.h"
#include "general.h"
#include "dataVariables.h"
#include "server_mb.h"
#include "mqtt_module.h"
#include "server_web.h"
#include "server_snmp.h"

//========================= MQTT ===================================

// persitent data record
persistConfig pconfig; 

// MQTT broker (e.g. your PC running Mosquitto)
int mqttPort = 1883;

// MQTT client setup
//EthernetClient ethClientMQTT;
EthernetClient *ethClientMQTT = nullptr;
//PubSubClient mqttClient(ethClient);
PubSubClient *mqttClient = nullptr; 

// NTP ethernet pointers
EthernetUDP *udpServerNTP = nullptr;;  // Servidor UDP for NTP
NTPClient *timeClient = nullptr;  // declare pointer, not instance 

// heartbeat sending local IP to GW
EthernetUDP hbUDP;

// Global Variables
//static unsigned long lastUptimeUpdateTime = 0;
uint32_t sysUptime = 0;
unsigned long _tupdate=0, _tmqtt=0, _tweb=0, _tmodbus=0, _tsnmp=0, _tNTP=0;

// ---------- SERVER ----------
EthernetServer modbusServer(MODBUS_PORT);
EthernetClient clients[MAX_CLIENTS];
unsigned long clientLastActivity[MAX_CLIENTS];

// ---------- WEB SERVER ----------
EthernetServer webServer(80);

// ---------- COILS & holding regs ----------
bool __coils[MAX_COILS];
uint16_t __holdingRegs[MAX_HREGS];

// --- DS18S20 setup ---
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensors(&oneWire);



// ================================================================================================
// ================================================================================================

// Function declarations            
int getUptime();
int readFakeSensor();

//-----------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);

  handlePersistentconfig(pconfig);   // load or create config and save again

  //loadDefaultConfig(pconfig);


  delay(200);
  initPins(pconfig);

  pinMode(W5500_CS_PIN, OUTPUT);
  digitalWrite(W5500_CS_PIN, HIGH);
  
  // Init SPI bus and Ethernet
  SPI.pins(SPI_CLK, SPI_MISO, SPI_MOSI, SPI_CS);
  SPI.begin();
  
  Serial.println("Inicializando W5500...");
  // Inicializa SPI/CS y Ethernet
  Ethernet.init(W5500_CS_PIN);
  delay(200);

  // Intentar inicializar con IP estática
  Serial.println("Configurando IP estatica...");
  Ethernet.begin(pconfig.mac, pconfig.ip);

  //Ethernet.begin(pconfig.mac);

  hbUDP.begin(0); // random ephemeral port for UDP heartbeat for IP info

  delay(200);  
  modbusServer.begin();

  Serial.print("Modbus TCP listening on ");
  Serial.print(Ethernet.localIP()); Serial.print(":"); Serial.println(MODBUS_PORT);
  Serial.println("\n*** Servidor Modbus TCP puerto 502 INICIADO ***");
  Serial.println("\nEsperando conexiones...\n");

  webServer.begin();
  Serial.println("Web server started. Listening on port 80...");

  tempSensors.begin(); 

  tempSensors.setWaitForConversion(true);
  tempSensors.setResolution(12);

  // NTP
  if (pconfig.NTPServerURL.length() > 5) // there is an NTP server defined 
    {  
    if (udpServerNTP == nullptr) {
      udpServerNTP = new EthernetUDP();
      }

    if (timeClient == nullptr) {
      timeClient = new NTPClient (*udpServerNTP, pconfig.NTPServerURL.c_str(), -3 * 3600, 600000);
      timeClient->begin();
      }
    }  

  // MQTT  
  if (pconfig.mqttServerURL.length() > 5) // there is an MQTT server defined 
    {
    Serial.println("Setting MQTT server...");
    if (ethClientMQTT ==  nullptr) {
      ethClientMQTT = new EthernetClient();
      }

    if (mqttClient == nullptr && ethClientMQTT != nullptr) {
      mqttClient = new PubSubClient(*ethClientMQTT);

      mqttClient->setServer(pconfig.mqttServerURL.c_str(), mqttPort);
      mqttClient->setCallback(mqttCallback);
      }
    }
    

  delay(200);  
  snmpInit();

  if (pconfig.watchdogSeconds > 0)  {   // set watchdog to deal with deadlock issues 
    // Initialize WDT with a timeout (e.g., 5 seconds)
    ESP.wdtDisable(); // disable
    ESP.wdtEnable(15000); // 15 seconds
    Serial.println("Setting Watchdog (15 seconds)");
    }

}

// ================================================================================================
// ================================================================================================

void loop() {

  unsigned long start_t=0, elapsed_t=0;
  static int counter1=0;
  
  start_t = millis();   variablesUpdate();       elapsed_t = millis() - start_t;
  _tupdate = (elapsed_t > _tupdate) ? elapsed_t : _tupdate;
 
  if (pconfig.mqttServerURL.length() > 5) { // there is an MQTT server defined 
    start_t = millis();   mqttHandle(mqttClient);  elapsed_t = millis() - start_t;
    _tmqtt = (elapsed_t > _tmqtt) ? elapsed_t : _tmqtt;
    }

  start_t = millis();   processWebServer();      elapsed_t = millis() - start_t;
  _tweb = (elapsed_t > _tweb) ? elapsed_t : _tweb;

  start_t = millis();   processModbusTCP();      elapsed_t = millis() - start_t;
  _tmodbus = (elapsed_t > _tmodbus) ? elapsed_t : _tmodbus;

  start_t = millis();   processSNMP ();          elapsed_t = millis() - start_t;
  _tsnmp = (elapsed_t > _tsnmp) ? elapsed_t : _tsnmp;

  if (pconfig.NTPServerURL.length() > 5) { // there is an NTP server defined 
    start_t = millis();   processNTP();            elapsed_t = millis() - start_t;
    _tNTP = (elapsed_t > _tNTP) ? elapsed_t : _tNTP;
    }

  counter1++;    
  if (counter1 > 300) {
      counter1 = 0;
      Serial.print("*** tupdate: ");       Serial.print(_tupdate);
      Serial.print(" tmqtt: ");      Serial.print(_tmqtt);
      Serial.print(" tweb: ");      Serial.print(_tweb);
      Serial.print(" tmodbus: ");      Serial.print(_tmodbus);
      Serial.print(" tsnmp: ");      Serial.print (_tsnmp);    
      Serial.print(" tNTP: ");      Serial.println (_tNTP); 
      }

  delay(5); 
  yield();     // reset Watchdog
  }

// ================================================================================================
// ================================================================================================
