// --- Data structure for variables ---
// STATIC APPROACH !!!!!! VERY IMPORTANT ISSUE  !!!!

// ===============================================================================

// variable list. Check : 
// https://docs.google.com/spreadsheets/d/1-WK52-l29xzHfgZ8jEiWFrJqgO0YLU_k8t9KsvDa858/edit?gid=0#gid=0

#define  VAR_TEMP1      0 
#define  VAR_TEMP2      1 
#define  VAR_TEMP3      2 
#define  VAR_HUM1       3 
#define  VAR_HUM2       4 
#define  VAR_HUM3       5 
#define  VAR_LIFE       6    // variables index
#define  VAR_TCPCON     7   
#define  VAR_TCPCONMAX  8 
#define  VAR_FREEMEM    9 
#define  VAR_RESTARTS   10 
#define  VAR_TEMP_TH1   11   // thresholds
#define  VAR_TEMP_TH2   12  
#define  VAR_TEMP_TH3   13 
#define  VAR_CYCLEHOURS 14 
#define  VAR_AA1_DAYS   15 
#define  VAR_AA2_DAYS   16 
#define  VAR_MB_PORT    17 
#define  VAR_WEB_PORT   18 
#define  VAR_MQTT_PORT  19 
#define  VAR_NTP_PORT   20
#define  VAR_IP_ADD_0   21
#define  VAR_IP_ADD_1   22
#define  VAR_IP_ADD_2   23
#define  VAR_IP_ADD_3   24
#define  VAR_IP_MASK_0  25
#define  VAR_IP_MASK_1  26
#define  VAR_IP_MASK_2  27
#define  VAR_IP_MASK_3  28
#define  VAR_IP_GW_0    29
#define  VAR_IP_GW_1    30
#define  VAR_IP_GW_2    31
#define  VAR_IP_GW_3    32
#define  VAR_MAC_0      33
#define  VAR_MAC_1      34
#define  VAR_MAC_2      35
#define  VAR_MAC_3      36
#define  VAR_MAC_4      37
#define  VAR_MAC_5      38

#define  VAR_STATUSREC  39
#define  VAR_STATUSREC2 40
#define  VAR_YEAR       41
#define  VAR_MONTH      42
#define  VAR_DAY        43
#define  VAR_HOUR       44
#define  VAR_MINUTE     45

#define  VAR_t_UPDATE   46
#define  VAR_t_MQTT     47
#define  VAR_t_WEB      48
#define  VAR_t_MB       49
#define  VAR_t_SNMP     50
#define  VAR_t_NTP      51

#define  VAR_CycMinRem  52
#define  VAR_SPARE02    53
#define  VAR_SPARE03    54
#define  VAR_SPARE04    55
#define  VAR_SPARE05    56
#define  VAR_SPARE06    57
#define  VAR_SPARE07    58
#define  VAR_SPARE08    59
#define  VAR_SPARE09    60

#define  VAR_WIFI_SSID  61
#define  VAR_WIFI_PASS  62
#define  VAR_MQTT_URL   63
#define  VAR_NTP_URL    64

#define  VAR_AA1_STAT   65
#define  VAR_AA2_STAT   66
#define  VAR_TTHR1_EXCE 67
#define  VAR_TTHR2_EXCE 68
#define  VAR_TTHR3_EXCE 69
#define  VAR_TEMP_ALRM  70

#define  VAR_AA1_FORCED 71
#define  VAR_AA2_FORCED 72

#define  VAR_AA1_WARN   73
#define  VAR_AA2_WARN   74

//  running status 

#define  RUNNING_AA_1   1
#define  RUNNING_AA_2   2

// coils position in ModBus COIL array
#define  COIL_VAR_AA1_STAT   1
#define  COIL_VAR_AA2_STAT   2
#define  COIL_VAR_TTHR1_EXCE 3
#define  COIL_VAR_TTHR2_EXCE 4
#define  COIL_VAR_TTHR3_EXCE 5
#define  COIL_VAR_TEMP_ALRM  6
#define  COIL_VAR_AA1_FORCED 7
#define  COIL_VAR_AA2_FORCED 8

#define  COIL_VAR_AA1_WARN   9    // AA running and Temp. threhold exceeded
#define  COIL_VAR_AA2_WARN   10

// define fo status WORD

#define  STAT_AA1_STATUS         0
#define  STAT_AA2_STATUS         1
#define  STAT_T1_EXCEEDED        2 
#define  STAT_T2_EXCEEDED        3 
#define  STAT_T3_EXCEEDED        4 
#define  STAT_A1_A2_ON_T_HIGH    5 
#define  STAT_A1_FORCED          6 
#define  STAT_A2_FORCED          7 
#define  STAT_ALARM              8 
#define  STAT_NTP_SYNC_OK        9
#define  STAT_MQTT_CONNECTED     10
#define  STAT_BATTERY_LOW        11
#define  STAT_BATTERY_LOW_LOW    12
#define  STAT_INT_T_HIGH         13
#define  STAT_INT_T_HIGH_HIGH    14
#define  STAT_SPARE_BIT          15
 
#define  STAT_AA1_WARN           0
#define  STAT_AA2_WARN           1


// logic for AA ON / OFF
#define   ACOND_ON                1
#define   ACOND_OFF               0

//-----------------------------------------------------------------------------------------------

typedef struct  Variable {  //  basic structure to handle variables for SNMP / MQTT / MBTCP
    const char           *name;
    const char           *oid;
    int                  value;
    short int            oldValue;
    short int            hreg;
    short int            coil;
    short int            mqttEnable;
    unsigned long        lastMQTTPublishTime;
    unsigned short int   MQTTMinUpdateTime;
    unsigned short int   MQTTMaxUpdateTime;
    unsigned short int   MQTTUpdateONChange;
    ValueCallback        *snmpCallback;

    }Variable;

//-----------------------------------------------------------------------------------------------

Variable vars[] = {        // array of variables 
    {"temp1", ".1.3.6.1.4.1.99999.1.1.1",      0, 0, VAR_TEMP1, -1, 1 ,0, 20, 60, 1},            // 0
    {"temp2", ".1.3.6.1.4.1.99999.1.1.2",      0, 0, VAR_TEMP2, -1, 1, 0, 20, 60, 1},
    {"temp3", ".1.3.6.1.4.1.99999.1.1.3",      0, 0, VAR_TEMP3, -1, 1, 0, 20, 60, 1},
    {"hum1", ".1.3.6.1.4.1.99999.1.2.1",      0, 0, VAR_HUM1, -1, 1, 0, 20, 60, 1 },
    {"hum2", ".1.3.6.1.4.1.99999.1.2.2",      0, 0, VAR_HUM2, -1, 1, 0, 20, 60, 1},
    {"hum3", ".1.3.6.1.4.1.99999.1.2.3",      0, 0, VAR_HUM3, -1, 1, 0, 20, 60, 1},
    {"life", ".1.3.6.1.4.1.99999.1.3.1",       0, 0, VAR_LIFE, -1, 1, 0, 10, 300, 0},
    {"tcpcon", ".1.3.6.1.4.1.99999.1.4.1",     0, 0, VAR_TCPCON, -1, 1, 0, 60, 300, 1},
    {"tcpconmax", ".1.3.6.1.4.1.99999.1.4.2",  0, 0, VAR_TCPCONMAX, -1, 1, 0, 60, 300, 1},
    {"freemem", ".1.3.6.1.4.1.99999.1.4.3",    0, 0, VAR_FREEMEM, -1, 1, 0, 60, 300, 1},
    {"restarts", ".1.3.6.1.4.1.99999.1.5.1",    0, 0, VAR_RESTARTS, -1, 1, 0, 60, 300, 1},       // 10
    {"tempthre1", ".1.3.6.1.4.1.99999.1.6.1",    0, 0, VAR_TEMP_TH1, -1, 1, 0, 120, 300, 1},
    {"tempthre2", ".1.3.6.1.4.1.99999.1.6.2",    0, 0, VAR_TEMP_TH2, -1, 1, 0, 120, 300, 1},
    {"tempthre3", ".1.3.6.1.4.1.99999.1.6.3",    0, 0, VAR_TEMP_TH3, -1, 1, 0, 120, 300, 1},

    {"cyclehours", ".1.3.6.1.4.1.99999.1.7.1",    0, 0, VAR_CYCLEHOURS, -1, 1, 0, 120, 300, 1},
    {"AA1_Days", ".1.3.6.1.4.1.99999.1.8.1",    0, 0, VAR_AA1_DAYS, -1, 1, 0, 120, 300, 1},
    {"AA2_days", ".1.3.6.1.4.1.99999.1.8.2",    0, 0, VAR_AA2_DAYS, -1, 1, 0, 120, 300, 1},
    {"MB_PORT", ".1.3.6.1.4.1.99999.1.9.1",    0, 0, VAR_MB_PORT, -1, 1, 0, 120, 300, 1},
    {"WEB_PORT", ".1.3.6.1.4.1.99999.1.10.1",    0, 0, VAR_WEB_PORT, -1, 1, 0, 120, 300, 1},
    {"MQTT_PORT", ".1.3.6.1.4.1.99999.1.11.1",    0, 0, VAR_MQTT_PORT, -1, 1, 0, 120, 300, 1},
    {"NTP_PORT", ".1.3.6.1.4.1.99999.1.12.1",    0, 0, VAR_NTP_PORT, -1, 1, 0, 120, 300, 1},      // 20 

    {"IP_ADDR_0", ".1.3.6.1.4.1.99999.1.13.1",    0, 0, VAR_IP_ADD_0, -1, 1, 0, 120, 300, 1},
    {"IP_ADDR_1", ".1.3.6.1.4.1.99999.1.13.2",    0, 0, VAR_IP_ADD_1, -1, 1, 0, 120, 300, 1},
    {"IP_ADDR_2", ".1.3.6.1.4.1.99999.1.13.3",    0, 0, VAR_IP_ADD_2, -1, 1, 0, 120, 300, 1},
    {"IP_ADDR_3", ".1.3.6.1.4.1.99999.1.13.4",    0, 0, VAR_IP_ADD_3, -1, 1, 0, 120, 300, 1},

    {"IP_MASK_0", ".1.3.6.1.4.1.99999.1.14.1",    0, 0, VAR_IP_MASK_0, -1, 1, 0, 120, 300, 1},
    {"IP_MASK_1", ".1.3.6.1.4.1.99999.1.14.2",    0, 0, VAR_IP_MASK_1, -1, 1, 0, 120, 300, 1},
    {"IP_MASK_2", ".1.3.6.1.4.1.99999.1.14.3",    0, 0, VAR_IP_MASK_2, -1, 1, 0, 120, 300, 1},
    {"IP_MASK_3", ".1.3.6.1.4.1.99999.1.14.4",    0, 0, VAR_IP_MASK_3, -1, 1, 0, 120, 300, 1},

    {"IP_GW_0", ".1.3.6.1.4.1.99999.1.15.1",    0, 0, VAR_IP_GW_0, -1, 1, 0, 120, 300, 1},
    {"IP_GW_1", ".1.3.6.1.4.1.99999.1.15.2",    0, 0, VAR_IP_GW_1, -1, 1, 0, 120, 300, 1},       // 30
    {"IP_GW_2", ".1.3.6.1.4.1.99999.1.15.3",    0, 0, VAR_IP_GW_2, -1, 1, 0, 120, 300, 1},
    {"IP_GW_3", ".1.3.6.1.4.1.99999.1.15.4",    0, 0, VAR_IP_GW_3, -1, 1, 0, 120, 300, 1},

    {"VAR_MAC_0", ".1.3.6.1.4.1.99999.1.16.1",    0, 0, VAR_MAC_0, -1, 1, 0, 120, 300, 1},
    {"VAR_MAC_1", ".1.3.6.1.4.1.99999.1.16.2",    0, 0, VAR_MAC_1, -1, 1, 0, 120, 300, 1},
    {"VAR_MAC_2", ".1.3.6.1.4.1.99999.1.16.3",    0, 0, VAR_MAC_2, -1, 1, 0, 120, 300, 1},
    {"VAR_MAC_3", ".1.3.6.1.4.1.99999.1.16.4",    0, 0, VAR_MAC_3, -1, 1, 0, 120, 300, 1},
    {"VAR_MAC_4", ".1.3.6.1.4.1.99999.1.16.5",    0, 0, VAR_MAC_4, -1, 1, 0, 120, 300, 1},
    {"VAR_MAC_5", ".1.3.6.1.4.1.99999.1.16.6",    0, 0, VAR_MAC_5, -1, 1, 0, 120, 300, 1},
    
    {"VAR_STATUSREC", ".1.3.6.1.4.1.99999.1.30.1",    0, 0, VAR_STATUSREC, -1, 1, 0, 30, 300, 1},
    {"VAR_STATUSREC2", ".1.3.6.1.4.1.99999.1.30.2",    0, 0, VAR_STATUSREC2, -1, 1, 0, 30, 300, 1},    // 40

    {"VAR_YEAR", ".1.3.6.1.4.1.99999.1.40.1",    0, 0, VAR_YEAR, -1, 1, 0, 120, 300, 1},
    {"VAR_MONTH", ".1.3.6.1.4.1.99999.1.40.2",    0, 0, VAR_MONTH, -1, 1, 0, 120, 300, 1},
    {"VAR_DAY", ".1.3.6.1.4.1.99999.1.40.3",    0, 0, VAR_DAY, -1, 1, 0, 120, 300, 1},
    {"VAR_HOUR", ".1.3.6.1.4.1.99999.1.40.4",    0, 0, VAR_HOUR, -1, 1, 0, 120, 300, 1},
    {"VAR_MINUTE", ".1.3.6.1.4.1.99999.1.40.5",    0, 0, VAR_MINUTE, -1, 1, 0, 120, 300, 1},

    {"VAR_t_UPDATE", ".1.3.6.1.4.1.99999.1.41.1",    0, 0, VAR_t_UPDATE, -1, 1, 0, 30, 300, 1},
    {"VAR_t_MQTT", ".1.3.6.1.4.1.99999.1.41.2",    0, 0, VAR_t_MQTT, -1, 1, 0, 30, 300, 1},
    {"VAR_t_WEB", ".1.3.6.1.4.1.99999.1.41.3",    0, 0, VAR_t_WEB, -1, 1, 0, 30, 300, 1},
    {"VAR_t_MB", ".1.3.6.1.4.1.99999.1.41.4",    0, 0, VAR_t_MB, -1, 1, 0, 30, 300, 1},
    {"VAR_t_SNMP", ".1.3.6.1.4.1.99999.1.41.5",    0, 0, VAR_t_SNMP, -1, 1, 0, 30, 300, 1},           // 50
    {"VAR_t_NTP", ".1.3.6.1.4.1.99999.1.41.6",    0, 0, VAR_t_NTP, -1, 1, 0, 30, 300, 1},

    {"VAR_CycMinRem", ".1.3.6.1.4.1.99999.1.42.1",    0, 0, VAR_CycMinRem, -1, 1, 0, 120, 300, 0},      
    {"VAR_SPARE02", ".1.3.6.1.4.1.99999.1.42.2",    0, 0, VAR_SPARE02, -1, 1, 0, 120, 300, 0},
    {"VAR_SPARE03", ".1.3.6.1.4.1.99999.1.42.3",    0, 0, VAR_SPARE03, -1, 1, 0, 120, 300, 0},
    {"VAR_SPARE04", ".1.3.6.1.4.1.99999.1.42.4",    0, 0, VAR_SPARE04, -1, 1, 0, 120, 300, 0},
    {"VAR_SPARE05", ".1.3.6.1.4.1.99999.1.42.5",    0, 0, VAR_SPARE05, -1, 1, 0, 120, 300, 0},
    {"VAR_SPARE06", ".1.3.6.1.4.1.99999.1.42.6",    0, 0, VAR_SPARE06, -1, 1, 0, 120, 300, 0},
    {"VAR_SPARE07", ".1.3.6.1.4.1.99999.1.42.7",    0, 0, VAR_SPARE07, -1, 1, 0, 120, 300, 0},
    {"VAR_SPARE08", ".1.3.6.1.4.1.99999.1.42.8",    0, 0, VAR_SPARE08, -1, 1, 0, 120, 300, 0},
    {"VAR_SPARE09", ".1.3.6.1.4.1.99999.1.42.9",    0, 0, VAR_SPARE09, -1, 1, 0, 120, 300, 0},        // 60

    {"VAR_WIFI_SSID", ".1.3.6.1.4.1.99999.1.50.1",    0, 0, -1, -1, 1, 0, 60, 300, 1},
    {"VAR_WIFI_PASS", ".1.3.6.1.4.1.99999.1.51.1",    0, 0, -1, -1, 1, 0, 60, 300, 1},
    {"VAR_MQTT_URL", ".1.3.6.1.4.1.99999.1.52.1",    0, 0, -1, -1, 1, 0, 60, 300, 1},
    {"VAR_NTP_URL", ".1.3.6.1.4.1.99999.1.53.1",    0, 0, -1, -1, 1, 0, 60, 300, 1},

    {"VAR_AA1_STAT", ".1.3.6.1.4.1.99999.1.54.1",    0, 0, -1, COIL_VAR_AA1_STAT, 1, 0, 10, 20, 1},
    {"VAR_AA2_STAT", ".1.3.6.1.4.1.99999.1.55.1",    0, 0, -1, COIL_VAR_AA2_STAT, 1, 0, 10, 20, 1},
    {"VAR_TTHR1_EXCE", ".1.3.6.1.4.1.99999.1.56.1",    0, 0, -1, COIL_VAR_TTHR1_EXCE, 1, 0, 10, 300, 1},
    {"VAR_TTHR2_EXCE", ".1.3.6.1.4.1.99999.1.57.1",    0, 0, -1, COIL_VAR_TTHR2_EXCE, 1, 0, 10, 300, 1},
    {"VAR_TTHR3_EXCE", ".1.3.6.1.4.1.99999.1.58.1",    0, 0, -1, COIL_VAR_TTHR3_EXCE, 1, 0, 10, 300, 1},
    {"VAR_TEMP_ALRM", ".1.3.6.1.4.1.99999.1.59.1",    0, 0, -1, COIL_VAR_TEMP_ALRM, 1, 0, 10, 300, 1},       // 70 
    {"VAR_AA1_FORCED", ".1.3.6.1.4.1.99999.1.60.1",    0, 0, -1, COIL_VAR_AA1_FORCED, 1, 0, 10, 20, 1},       // 71 
    {"VAR_AA2_FORCED", ".1.3.6.1.4.1.99999.1.61.1",    0, 0, -1, COIL_VAR_AA2_FORCED, 1, 0, 10, 20, 1},       // 72 

    {"VAR_AA1_WARN", ".1.3.6.1.4.1.99999.1.62.1",    0, 0, -1, COIL_VAR_AA1_WARN, 1, 0, 10, 20, 1},       // 73 
    {"VAR_AA2_WARN", ".1.3.6.1.4.1.99999.1.63.1",    0, 0, -1, COIL_VAR_AA2_WARN, 1, 0, 10, 20, 1}       // 74 

};  


//typedef struct  Variable {  //  basic structure to handle variables for SNMP / MQTT / MBTCP
//    const char           *name;
//    const char           *oid;
//    int                  value;
//    short int            oldValue;
//    short int            hreg;
//    short int            coil;
//    short int            mqttEnable;
//    unsigned long        lastMQTTPublishTime;
//    unsigned short int   MQTTMinUpdateTime;
//    unsigned short int   MQTTMaxUpdateTime;
//    unsigned short int   MQTTUpdateONChange;
//    ValueCallback        *snmpCallback;
//    }Variable;


#define VAR_COUNT ( (sizeof(vars) / sizeof(vars[0])) )   // static array size

extern bool                 __coils[];
extern uint16_t             __holdingRegs[];
extern DallasTemperature    tempSensors;
extern PubSubClient         *mqttClient;

extern NTPClient            *timeClient; 
extern persistConfig        pconfig; 

extern unsigned long        _tupdate, _tmqtt, _tweb, _tmodbus, _tsnmp, _tNTP;

static int                  RunningAA = RUNNING_AA_1;

static unsigned long        epochTime;
static unsigned short       _ntpUpdateOK = 0;
unsigned short              _mqttUpdateOK = 0;

extern EthernetUDP          hbUDP;

int                         __counter = 0;

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

int usedSlotsCount();
int publishVar(Variable *v);

//-----------------------------------------------------------------------------------------------

int statusUpdate (uint16_t *status1, uint16_t *status2) {

  *status1 = 0;   // reset,  important!!!
  *status2 = 0;   // reset,  important!!!

  if (digitalRead(RELAY_PIN_1) == ACOND_ON)
    *status1 = *status1 | ( 1 << STAT_AA1_STATUS );

  if (digitalRead(RELAY_PIN_2) == ACOND_ON)
    *status1 = *status1 | ( 1 << STAT_AA2_STATUS );

  if ( vars[VAR_TEMP3].value >= ( (pconfig.t1 * 100) + 50) ) 
    *status1 = *status1 | ( 1 << STAT_T1_EXCEEDED );
  if ( vars[VAR_TEMP3].value >= ( (pconfig.t2 * 100) + 50) ) 
    *status1 = *status1 | ( 1 << STAT_T2_EXCEEDED );
  if ( vars[VAR_TEMP3].value >= ( (pconfig.t3 * 100) + 50) ) 
    *status1 = *status1 | ( 1 << STAT_T3_EXCEEDED );

  if (digitalRead(RELAY_PIN_1) == ACOND_ON && digitalRead(RELAY_PIN_2) == ACOND_ON && 
        vars[VAR_TEMP3].value >= ( (pconfig.t3 * 100) + 50) )
    *status1 = *status1 | ( 1 << STAT_A1_A2_ON_T_HIGH );

  if (_ntpUpdateOK > 0 ) 
    *status1 = *status1 | ( 1 << STAT_NTP_SYNC_OK );
    
  if (_mqttUpdateOK > 0 ) 
    *status1 = *status1 | ( 1 << STAT_MQTT_CONNECTED );
    
  // second status Word
  if ( vars[VAR_AA1_WARN].value > 0 ) 
    *status2 = *status2 |  ( 1 << STAT_AA1_WARN );
  if ( vars[VAR_AA2_WARN].value > 0 ) 
    *status2 = *status2 |  ( 1 << STAT_AA2_WARN );

  Serial.print("status1: ");
  Serial.print(*status1);
  Serial.print(" - status2: ");
  Serial.println(*status2);


  return(0);
  }

/*
#define  STAT_A1_FORCED          6 
#define  STAT_A2_FORCED          7 
#define  STAT_ALARM              8 
#define  STAT_BATTERY_LOW        11
#define  STAT_BATTERY_LOW_LOW    12
#define  STAT_INT_T_HIGH         13
#define  STAT_INT_T_HIGH_HIGH    14
#define  STAT_SPARE_BIT          15
*/

//-----------------------------------------------------------------------------------------------



int variableInit(Variable *v, SNMPAgent *snmp) {

    if (strlen(v->oid) > 2) {
      if ( strcmp(v->name, "VAR_AA1_FORCED") == 0 || strcmp(v->name, "VAR_AA2_FORCED") == 0 )  // W/R values 
        v->snmpCallback = snmp->addIntegerHandler(v->oid, (int *) & (v->value) , true );        
      else                                                                                     // RO values
        v->snmpCallback = snmp->addIntegerHandler(v->oid, (int *) & (v->value));        

      v->lastMQTTPublishTime = millis() - random (5, v->MQTTMinUpdateTime * 1000); 

      Serial.print("Adding OID: ");
      Serial.println(v->oid);
      }
    return(0);
}

//-----------------------------------------------------------------------------------------------

int variableSet(Variable *v, bool *coils, uint16_t *holdregs, int value) {

  if (v->coil >= 0)    // if this var is linked to a coil 
    coils[v->coil] = value;
  if (v->hreg >= 0)   // if this var is linked to a holding register
    holdregs[v->hreg] = value;
  v->oldValue = v->value;
  v->value = value;

  if ( v->oldValue != v->value && v->MQTTUpdateONChange > 0 && pconfig.mqttServerURL.length() > 5 && mqttClient->connected()) {
    publishVar( v );
    }    
  
  return(0);
}


//-----------------------------------------------------------------------------------------------

int updateAAbyTemp() {   // check condition (temp, time) for AA switchover

  static unsigned long  lastCheckTime = 0;
  static unsigned long  lastSwitchoverTime = 0;
  unsigned long now = millis();

  if ( (now - lastCheckTime) >= (MIN_TEMP_CHECK_TIME * 60UL * 1000UL)  ) {
  //if ( (now - lastCheckTime) >= (MIN_TEMP_CHECK_TIME * 5UL * 1000UL)  ) {

    Serial.printf(" Testing temp for AA. Now: %lu PrevCheck: %lu  PrevSwitch: %lu \n" ,now, lastCheckTime, lastSwitchoverTime); 
    Serial.printf(" Temp2: %d  T1:  %d T2:  %d T3:  %d  \n" , vars[VAR_TEMP3].value, (pconfig.t1 * 100), (pconfig.t2 * 100), (pconfig.t3 * 100)); 
    Serial.printf(" Status: R1: %d R2: %d  Running: %d  \n" , digitalRead(RELAY_PIN_1), digitalRead(RELAY_PIN_2), RunningAA); 
    lastCheckTime = now;

    // check switchover time 
    variableSet( &( vars[VAR_CycMinRem] ), __coils, __holdingRegs, 
    ( pconfig.cycleHours * 60UL ) - (now - lastSwitchoverTime) / ( 60UL * 1000UL ) );

    if ( (now - lastSwitchoverTime) >= ( pconfig.cycleHours * 60UL * 60UL * 1000UL)  ) {
      lastSwitchoverTime = now;
      RunningAA = (RunningAA == RUNNING_AA_1) ? RUNNING_AA_2 : RUNNING_AA_1;
      }

    // first check for FORCED condition 
    if ( vars[VAR_AA1_FORCED].value > 0 || vars[VAR_AA2_FORCED].value > 0 ) {
      if ( vars[VAR_AA1_FORCED].value > 0 ) {
        digitalWrite(RELAY_PIN_1, ACOND_ON); // Turn relay ON
        variableSet( &( vars[VAR_AA1_STAT] ), __coils, __holdingRegs, 1);
        }  
      if ( vars[VAR_AA2_FORCED].value > 0 ) {
        digitalWrite(RELAY_PIN_2, ACOND_ON); // Turn relay ON
        variableSet( &( vars[VAR_AA2_STAT] ), __coils, __holdingRegs, 1);
        }
      }  
    // now  we check for temperature,  then we apply time switchover  
    else if ( vars[VAR_TEMP3].value >= ( (pconfig.t2 * 100) + 50) ) {  // HIGH temp condition,  both ON
      Serial.println(" HIGH Temp:  both AA are ON ! ");
      
      // as temp. threshold was exceeded, we set AA running on 'warning' condition
      // only one AC should be ON
      if ( digitalRead(RELAY_PIN_1) == ACOND_ON && digitalRead(RELAY_PIN_2) == ACOND_OFF)
        variableSet( &( vars[VAR_AA1_WARN] ), __coils, __holdingRegs, 1);
      if ( digitalRead(RELAY_PIN_2) == ACOND_ON && digitalRead(RELAY_PIN_1) == ACOND_OFF)
        variableSet( &( vars[VAR_AA2_WARN] ), __coils, __holdingRegs, 1);

      digitalWrite(RELAY_PIN_2, ACOND_ON); // Turn relay ON
      variableSet( &( vars[VAR_AA1_STAT] ), __coils, __holdingRegs, 1);
      digitalWrite(RELAY_PIN_1, ACOND_ON); // Turn relay ON
      variableSet( &( vars[VAR_AA2_STAT] ), __coils, __holdingRegs, 1);

      } 
    else if ( vars[VAR_TEMP3].value >  ( (pconfig.t1 * 100) + 50) &&   // checl temp3 which is the 'best' T from 2 sensors
      vars[VAR_TEMP3].value < ( (pconfig.t2 * 100) - 50) ) {  // normal temp condition 

      // reset warning condition as temperatur is back in range   
      variableSet( &( vars[VAR_AA1_WARN] ), __coils, __holdingRegs, 0);
      variableSet( &( vars[VAR_AA2_WARN] ), __coils, __holdingRegs, 0);

      if (RunningAA == RUNNING_AA_1) {   
        digitalWrite(RELAY_PIN_1, ACOND_ON); // Turn relay 1 ON
        variableSet( &( vars[VAR_AA1_STAT] ), __coils, __holdingRegs, 1);
        digitalWrite(RELAY_PIN_2, ACOND_OFF); // Turn relay 2 OFF
        variableSet( &( vars[VAR_AA2_STAT] ), __coils, __holdingRegs, 0);
        }
      else if (RunningAA == RUNNING_AA_2) {   
        digitalWrite(RELAY_PIN_1, ACOND_OFF); // Turn relay 1 OFF
        variableSet( &( vars[VAR_AA1_STAT] ), __coils, __holdingRegs, 0);
        digitalWrite(RELAY_PIN_2, ACOND_ON); // Turn relay 2 ON
        variableSet( &( vars[VAR_AA2_STAT] ), __coils, __holdingRegs, 1);
        }
      }
    else if ( vars[VAR_TEMP3].value < ( (pconfig.t1 * 100) - 50) ) {  // LOW temp condition, both OFF

      // reset warning condition as temperatur is back in range   
      variableSet( &( vars[VAR_AA1_WARN] ), __coils, __holdingRegs, 0);
      variableSet( &( vars[VAR_AA2_WARN] ), __coils, __holdingRegs, 0);

      digitalWrite(RELAY_PIN_1, ACOND_OFF); // Turn relay 1 OFF
      variableSet( &( vars[VAR_AA1_STAT] ), __coils, __holdingRegs, 1);
      digitalWrite(RELAY_PIN_2, ACOND_OFF); // Turn relay 1 OFF
      variableSet( &( vars[VAR_AA2_STAT] ), __coils, __holdingRegs, 1);
      }
    }

  return(0);
  }

//-----------------------------------------------------------------------------------------------

int NTPUPdate()  {

  if ( (timeClient->update()) == true )   
    _ntpUpdateOK = 1;        // to update status record         
  else  
    _ntpUpdateOK = 0;

  epochTime = timeClient->getEpochTime();  // seconds since 1970
  struct tm *ptm = gmtime((time_t *)&epochTime);

  variableSet( &( vars[VAR_YEAR] ), __coils, __holdingRegs, ptm->tm_year + 1900); 
  variableSet( &( vars[VAR_MONTH] ), __coils, __holdingRegs, ptm->tm_mon + 1); 
  variableSet( &( vars[VAR_DAY] ), __coils, __holdingRegs, ptm->tm_mday); 
  variableSet( &( vars[VAR_HOUR] ), __coils, __holdingRegs, ptm->tm_hour); 
  variableSet( &( vars[VAR_MINUTE] ), __coils, __holdingRegs, ptm->tm_min); 

  return(0);
}

//-----------------------------------------------------------------------------------------------

int processNTP() {
  static unsigned long NTPLastTime = 0;
  static int counter = 0;

  if ( ++counter > 1000 ) {
    counter = 0;

    if ( NTPLastTime == 0 ) {   // NTP attempt at startup.. 
        NTPUPdate();
        NTPLastTime = millis();
        }

    else if (pconfig.NTPServerURL.length() > 5) {  // there is an NTP server defined 

      Serial.print("NTPLastTime: ");
      Serial.print(NTPLastTime); 
      Serial.print("  millis: ");
      Serial.print( timeClient->getEpochTime() );
      Serial.print("  --  ");
      Serial.println(  (timeClient->getEpochTime() - NTPLastTime ) / 1000 );
   
      if ( _tNTP > NTP_MAX_RESPONSE_T_ms ) {  // we will wait longer for next attempt
        if ( ( timeClient->getEpochTime() - NTPLastTime ) > NTP_CYCLE_IF_LAST_NOT_OK ) {
          NTPUPdate();
          NTPLastTime = timeClient->getEpochTime();
          }
        } 
      else {
        if ( ( timeClient->getEpochTime() - NTPLastTime ) > NTP_CYCLE_IF_LAST_OK ) {
          NTPUPdate();
          NTPLastTime = timeClient->getEpochTime();
          }
        }  
      }
    }
  return(0);
  }


//-----------------------------------------------------------------------------------------------

void sendHeartbeat() {
  byte mac[6];
  char buf[64];
  IPAddress gw = Ethernet.gatewayIP();

  if (gw == INADDR_NONE) return; // safety

  Ethernet.MACAddress(mac);
  
  hbUDP.beginPacket(gw, 55555); // arbitrary UDP port
  snprintf(buf, sizeof(buf), "%s %02X:%02X:%02X:%02X:%02X:%02X:", pconfig.stationName.c_str(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  hbUDP.print(buf);
  hbUDP.print(F("ESP8266-HB"));
  hbUDP.endPacket();
}

//-----------------------------------------------------------------------------------------------

int variablesUpdate() {
    static int tempAux1 = 0, tempAux2 = 0;
    uint16_t status1=0, status2=0;
    
    updateAAbyTemp();

    if ( ++__counter >= 1000 ) {    // slow calls 
      __counter = 0;

      sendHeartbeat();   //heartbeat with local IP info sent to default GW

      //tempSensors.requestTemperatures(); // first read will take some time
      tempSensors.setWaitForConversion(false);   // key line for async mode
      tempSensors.requestTemperatures(); 

      tempAux2 = tempSensors.getTempCByIndex(1) * 100;
      tempAux1 = tempSensors.getTempCByIndex(0) * 100;
      variableSet( &( vars[VAR_FREEMEM] ), __coils, __holdingRegs, ESP.getFreeHeap() / 100); // free mem / 100  

      variableSet( &( vars[VAR_RESTARTS] ), __coils, __holdingRegs, pconfig.bootCount); 
      variableSet( &( vars[VAR_TEMP_TH1] ), __coils, __holdingRegs, pconfig.t1 * 100); 
      variableSet( &( vars[VAR_TEMP_TH2] ), __coils, __holdingRegs, pconfig.t2 * 100); 
      variableSet( &( vars[VAR_TEMP_TH3] ), __coils, __holdingRegs, pconfig.t3 * 100); 

      variableSet( &( vars[VAR_CYCLEHOURS] ), __coils, __holdingRegs, pconfig.cycleHours ); 
      variableSet( &( vars[VAR_AA1_DAYS] ), __coils, __holdingRegs, 0); 
      variableSet( &( vars[VAR_AA2_DAYS] ), __coils, __holdingRegs, 0); 
      variableSet( &( vars[VAR_MB_PORT] ), __coils, __holdingRegs, pconfig.mbPort); 
      variableSet( &( vars[VAR_WEB_PORT] ), __coils, __holdingRegs, pconfig.webPort); 
      variableSet( &( vars[VAR_MQTT_PORT] ), __coils, __holdingRegs, pconfig.mqttServerPort); 
      variableSet( &( vars[VAR_NTP_PORT] ), __coils, __holdingRegs, pconfig.NTPServerPort); 

      variableSet( &( vars[VAR_IP_ADD_0] ), __coils, __holdingRegs, pconfig.ip[0]); 
      variableSet( &( vars[VAR_IP_ADD_1] ), __coils, __holdingRegs, pconfig.ip[1]); 
      variableSet( &( vars[VAR_IP_ADD_2] ), __coils, __holdingRegs, pconfig.ip[2]); 
      variableSet( &( vars[VAR_IP_ADD_3] ), __coils, __holdingRegs, pconfig.ip[3]); 

      variableSet( &( vars[VAR_IP_MASK_0] ), __coils, __holdingRegs, pconfig.mask[0]); 
      variableSet( &( vars[VAR_IP_MASK_1] ), __coils, __holdingRegs, pconfig.mask[1]); 
      variableSet( &( vars[VAR_IP_MASK_2] ), __coils, __holdingRegs, pconfig.mask[2]); 
      variableSet( &( vars[VAR_IP_MASK_3] ), __coils, __holdingRegs, pconfig.mask[3]); 

      variableSet( &( vars[VAR_IP_GW_0] ), __coils, __holdingRegs, pconfig.gateway[0]); 
      variableSet( &( vars[VAR_IP_GW_1] ), __coils, __holdingRegs, pconfig.gateway[1]); 
      variableSet( &( vars[VAR_IP_GW_2] ), __coils, __holdingRegs, pconfig.gateway[2]); 
      variableSet( &( vars[VAR_IP_GW_3] ), __coils, __holdingRegs, pconfig.gateway[3]); 
   
      variableSet( &( vars[VAR_MAC_0] ), __coils, __holdingRegs, pconfig.mac[0]); 
      variableSet( &( vars[VAR_MAC_1] ), __coils, __holdingRegs, pconfig.mac[1]); 
      variableSet( &( vars[VAR_MAC_2] ), __coils, __holdingRegs, pconfig.mac[2]); 
      variableSet( &( vars[VAR_MAC_3] ), __coils, __holdingRegs, pconfig.mac[3]); 
      variableSet( &( vars[VAR_MAC_4] ), __coils, __holdingRegs, pconfig.mac[4]); 
      variableSet( &( vars[VAR_MAC_5] ), __coils, __holdingRegs, pconfig.mac[5]); 

      statusUpdate (&status1, &status2);
      variableSet( &( vars[VAR_STATUSREC] ), __coils, __holdingRegs, status1); 
      variableSet( &( vars[VAR_STATUSREC2] ), __coils, __holdingRegs, status2); 

      variableSet( &( vars[VAR_t_UPDATE] ), __coils, __holdingRegs, _tupdate); 
      variableSet( &( vars[VAR_t_MQTT] ), __coils, __holdingRegs, _tmqtt); 
      variableSet( &( vars[VAR_t_WEB] ), __coils, __holdingRegs, _tweb); 
      variableSet( &( vars[VAR_t_MB] ), __coils, __holdingRegs, _tmodbus); 
      variableSet( &( vars[VAR_t_SNMP] ), __coils, __holdingRegs, _tsnmp); 
      variableSet( &( vars[VAR_t_NTP] ), __coils, __holdingRegs, _tNTP); 
   
    //{"VAR_WIFI_SSID", ".1.3.6.1.4.1.99999.1.17.1",    0, 0, VAR_WIFI_SSID, -1, 1},
    //{"VAR_WIFI_PASS", ".1.3.6.1.4.1.99999.1.18.1",    0, 0, VAR_WIFI_PASS, -1, 1},
    //{"VAR_MQTT_URL", ".1.3.6.1.4.1.99999.1.19.1",    0, 0, VAR_MQTT_URL, -1, 1},
    //{"VAR_NTP_URL", ".1.3.6.1.4.1.99999.1.20.1",    0, 0, VAR_NTP_URL, -1, 1},
    }

    variableSet( &( vars[VAR_LIFE] ), __coils, __holdingRegs, __counter);

    // temperature sensors 
    variableSet( &( vars[VAR_TEMP1] ), __coils, __holdingRegs, tempAux1);
    variableSet( &( vars[VAR_TEMP2] ), __coils, __holdingRegs, tempAux2);

    // set TEMP3 to the 'best' temperature in case of sensor error

    if ( tempAux1 > TEMP_THRESHOLD_LOW && tempAux1 < TEMP_THRESHOLD_HIGH )
      variableSet( &( vars[VAR_TEMP3] ), __coils, __holdingRegs, tempAux1);
    else if ( tempAux2 > TEMP_THRESHOLD_LOW && tempAux2 < TEMP_THRESHOLD_HIGH )
      variableSet( &( vars[VAR_TEMP3] ), __coils, __holdingRegs, tempAux2);
    else
      variableSet( &( vars[VAR_TEMP3] ), __coils, __holdingRegs, TEMP_ERROR);

    variableSet( &( vars[VAR_TCPCON] ), __coils, __holdingRegs, usedSlotsCount());
    variableSet( &( vars[VAR_TCPCONMAX] ), __coils, __holdingRegs, MAX_CLIENTS); 

    return(1);
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
