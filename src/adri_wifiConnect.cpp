/**
 * @file	C:\Users\adri\Documents\Arduino\libraries\adri_wifiConnect\src\adri_wifiConnect.cpp.
 *
 * @brief	Implements the adri WiFi connect class
 */

#include "adri_wifiConnect.h"

#include <arduino.h>

#include <adri_tools_v2.h>
#include <adri_timer.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// #define DEBUG

#if defined(ESP8266)
    #define FILESYSTEM LittleFS
#elif defined(ESP32)
    #define FILESYSTEM SPIFFS
#else
#endif

fs(awc_str_connect,     "connect startup");
fs(awc_str_connectSSID, "connect ssid");
fs(awc_str_station,     "station mod");
fs(awc_str_porgress,    "_porgress");
fs(awc_str_credential,  "cred");
fs(awc_str_ipmod,       "ip mod");
fs(awc_str_statu,       "wifi statu");

fs(awc_str_ip,          "ip");
fs(awc_str_ssid,        "ssid");
fs(awc_str_psk,         "psk");
fs(awc_str_subnet,      "subnet");
fs(awc_str_gateway,     "gateway");

char b_host_name[80];
void build_host_name(char * hostnamePrefix) {
   unsigned char mac[6];
   WiFi.macAddress(mac);
   strcpy(b_host_name, hostnamePrefix);
   for (int i=4; i<6; i++) {char b[2]; sprintf(b,"%02x",mac[i]); strcat(b_host_name, b); }
}

wifi_credential_ap * wifi_credentialAp_ptr;
wifi_credential_ap * wifi_credentialAp_ptr_get(){
    return wifi_credentialAp_ptr;
}
void wifi_credential_ap_register(wifi_credential_ap* ptr){
    wifi_credentialAp_ptr = ptr;
}
/**
 * @fn	wifi_credential_ap::wifi_credential_ap(const char * value)
 *
 * @brief	Constructor for an static ojet
 *
 * @author	Adri
 * @date	18/08/2020
 *
 * @param 	value	The value.
 */

wifi_credential_ap::wifi_credential_ap(const char * value){
    wifi_credentialAp_ptr = this;
     const char * bl = "";
    if (value!=bl) hostname_set(adri_toolsv2Ptr_get()->ch_toString(value));
}

/**
 * @fn	void wifi_credential_ap::hostname_set(String value)
 *
 * @brief	set ap HostName
 *
 * @author	Adri
 * @date	18/08/2020
 *
 * @param 	value	The HostName.
 */

void wifi_credential_ap::hostname_set(String value){
    char buffer[80];
    sprintf(buffer, "%s_", value.c_str());
    build_host_name(buffer);

    _ssid = String(b_host_name);
}

/**
 * @fn	void wifi_credential_ap::psk_set(String value)
 *
 * @brief	set the ap password
 *
 * @author	Adri
 * @date	18/08/2020
 *
 * @param 	value	The value.
 */
void wifi_credential_ap::psk_set(String value)      { _psk       = value;}

/**
 * @fn	void wifi_credential_ap::ip_set(String value)
 *
 * @brief	set the ap ip
 * @brief	string format "192.168.0.0"
 *
 * @author	Adri
 * @date	18/08/2020
 *
 * @param 	value	The value.
 */
void wifi_credential_ap::ip_set(String value)       {
    #ifdef DEBUG 
        fsprintf("\n[wifi_credential_ap::ip_set] current : %S - set to : %s\n", _ip.c_str(), value.c_str());
    #endif
    _ip = value;
}

/**
 * @fn	void wifi_credential_ap::subnet_set(String value)
 *
 * @brief	set the ap subnet mask adress
 * @brief	string format "255.255.255.0"
 *
 * @author	Adri
 * @date	18/08/2020
 *
 * @param 	value	The value.
 */
void wifi_credential_ap::subnet_set(String value)   { _subnet    = value;}

/**
 * @fn	String wifi_credential_ap::psk_get()
 *
 * @brief	get the ap password
 *
 * @author	Adri
 * @date	18/08/2020
 *
 * @returns	String.
 */
String wifi_credential_ap::psk_get()                { return _psk;    }

/**
 * @fn	String wifi_credential_ap::ip_get()
 *
 * @brief	get the ap ip
 * @brief	string format "192.168.0.0"
 *
 * @author	Adri
 * @date	18/08/2020
 *
 * @returns	String.
 */
String wifi_credential_ap::ip_get()                 { return _ip;     }

/**
 * @fn	String wifi_credential_ap::subnet_get()
 *
 * @brief	get the ap subnet massk
 * @brief	string format "255.255.255.0"
 *
 * @author	Adri
 * @date	18/08/2020
 *
 * @returns	String.
 */
String wifi_credential_ap::subnet_get()             { return _subnet; }

/**
 * @fn	String wifi_credential_ap::hostname_get()
 *
 * @brief	get the ap hostname
 *
 * @author	Adri
 * @date	18/08/2020
 *
 * @returns	String.
 */
String wifi_credential_ap::hostname_get()           { return _ssid;   }

/**
 * @fn	void wifi_credential_ap::print()
 *
 * @brief	Serial Prints ap identifiants .
 *
 * @author	Adri
 * @date	18/08/2020
 */
void wifi_credential_ap::print(){
    fsprintf("\n[wifi_credential_ap print]\n")
    fsprintf("%-15s : %s\n", fsget(awc_str_ssid).c_str(),   _ssid.c_str());
    fsprintf("%-15s : %s\n", fsget(awc_str_psk).c_str(),    _psk.c_str());
    fsprintf("%-15s : %s\n", fsget(awc_str_ip).c_str(),     _ip.c_str());
    fsprintf("%-15s : %s\n", fsget(awc_str_subnet).c_str(), _subnet.c_str());

}

/**
 * @fn	boolean wifi_credential_ap_fromSPIFF(String & result)
 *
 * @brief	load saved ap identifiants from the SPIFF MEMORY
 *
 * @author	Adri
 * @date	18/08/2020
 *
 * @param [in,out]	result ptr ver la string with the identifiants as xml format .
 *
 * @returns	True if the file can be open, false if the file is not created.
 */
boolean wifi_credential_ap_fromSPIFF(String & result) {

    String filename = "/" + CREDENTIALAP_FILENAME + ".txt";

    File f=FILESYSTEM.open(filename,"r");

    if (!f) {
        #ifdef DEBUG
            fsprintf("\n[wifi_credential_ap_fromSPIFF] Error reading : %S\n", filename.c_str())
        #endif  
        return false;
    }

    #ifdef DEBUG
        fsprintf("\n[wifi_credential_ap_fromSPIFF]\n")
    #endif

    String line = "";

    while (f.available()) {line += char(f.read());}

    result = line;

    #ifdef DEBUG
        fsprintf(" %s\n", result.c_str());
    #endif

    f.close();  

    return true;
}

/**
 *
 * @brief	Loads from spiif
 *
 * @author	Adri
 * @date	18/08/2020
 */

boolean wifi_credential_ap::load_fromSpiif(){

    String result = "";

    boolean load = wifi_credential_ap_fromSPIFF(result);

    if (!load) return false;

    String value = "";

    value = adri_toolsv2Ptr_get()->literal_value("psk",    result);
    _psk = value;

    value = adri_toolsv2Ptr_get()->literal_value("ip",     result);
    _ip = value;

    value = adri_toolsv2Ptr_get()->literal_value("subnet", result);
    _subnet = value;

    return true;
}
boolean wifi_credential_ap::setup_id(){
    String sSSID, sPsk;

    sSSID       = hostname_get();
    sPsk        = psk_get();

    if ((sSSID == "") || (sPsk == "")) {
        return false;
    }

    return true;
}
boolean wifi_credential_ap::setup_ip(){
    String sIp, sSubnet, sGateway;

    sIp         = ip_get();

    if (!isValidIp(sIp))                    return false;
    // if (!isValidIp(sIp, sSubnet, sGateway)) return false;

    return true;
}



/** @brief	The WiFi credential sta array[ credential max] */
wifi_credential_sta * wifi_credential_sta_array[CREDENTIAL_MAX];

wifi_credential_sta_best wifi_credential_sta_best;
wifi_credential_sta_best::wifi_credential_sta_best(){}

void _porgress_def(int v){}
wifiConnect * wifiConnect_ptr = nullptr;
wifiConnect * wifiConnect_instance(){
    return wifiConnect_ptr;
}
wifiConnect::wifiConnect(){
   wifiConnect_ptr = this; 
    _credential_sta = wifi_credential_sta_array[0];
   _porgress =  _porgress_def;
}

void wifiConnect::hostName_set(const char * mod)                    {
    _hostName = mod;

    // char buffer[80];
    // sprintf(buffer, "%s_", mod);

    // String escapedMac = WiFi.macAddress();
    // escapedMac.replace(":", "");
    // escapedMac.toLowerCase();
    // strcpy(_DNSname, buffer);
    // sprintf(_DNSname + 5, "%*s", 6, escapedMac.c_str() + 6);
    // fsprintf("\n[DNS]: %s\n", _DNSname);

    char buffer[80];
    sprintf(buffer, "%s", mod);

    unsigned char mac[6];
    WiFi.macAddress(mac);
    strcpy(_DNSname, buffer);
    // for (int i=4; i<6; i++) {char b[2]; sprintf(b,"%02x",mac[i]); strcat(_DNSname, b); }
    // fsprintf("\n[DNS]: %s\n", _DNSname);

}
void wifiConnect::MDSN_begin() {
    MDNS.begin(_DNSname);
    MDNS.addService("http", "tcp", 80);
}
void wifiConnect::MDSN_loop() {
    #if defined(ESP8266)
        MDNS.update();
    #elif defined(ESP32)
    #else
    #endif        
}
void wifiConnect::connect_set(WIFICONNECT_MOD mod)                  {_connect               = mod;}
void wifiConnect::connectSSID_set(WIFICONNECTSSID_MOD mod)          {_connectSSID           = mod;}
void wifiConnect::station_set(WiFiMode_t mod)                       {_station               = mod;}
void wifiConnect::progress_set(wifiConnect_progress mod)            {_porgress              = mod;}
void wifiConnect::credential_sta_set(wifi_credential_sta * mod)     {_credential_sta        = mod;}
void wifiConnect::credential_sta_pos_set(int mod)                   {_credential_sta_pos    = mod;}

void WIFI_STATU_toString(WIFI_STATU mod, String & result){
    switch(mod){
        case wifi_statu_none:               result = "none";                break;
        case wifi_statu_connect_ap_begin:   result = "connect_ap_begin";    break;
        case wifi_statu_connect_ap_succes:  result = "connect_ap_succes";   break;
        case wifi_statu_connect_ap_idFail:  result = "connect_ap_idFail";   break;
        case wifi_statu_connect_ap_coFail:  result = "connect_ap_coFail";   break;
        case wifi_statu_connect_sta_begin:  result = "connect_sta_begin";   break;
        case wifi_statu_connect_sta_succes: result = "connect_sta_succes";  break;
        case wifi_statu_connect_sta_coFail: result = "connect_sta_coFail";  break;
        case wifi_statu_connect_sta_idFail: result = "connect_sta_idFail";  break;
        case wifi_statu_sta_begin:          result = "sta_begin";           break;
        case wifi_statu_sta_search:         result = "sta_search";          break;
        case wifi_statu_sta_isconnected:    result = "sta_isconnected";     break;
        case wifi_statu_sta_fail:           result = "sta_fail";            break;
        default :                           result = "none";                break;
    }
}
void WIFICONNECT_MOD_toString(WIFICONNECT_MOD mod, String & result){
    switch(mod){
        case AWC_LOOP:  result = "loop";    break;
        case AWC_SETUP: result = "setup";   break;
        default :       result = "setup";   break;
    }
}
void WIFICONNECTSSID_MOD_toString(WIFICONNECTSSID_MOD mod, String & result){
    switch(mod){
        case AWCS_MULTI:    result = "multi";   break;
        case AWCS_AP:       result = "ap";      break;
        case AWCS_NORMAL:   result = "normal";  break;
        default :           result = "normal";  break;
    }
}
void WIFICONNECT_MOD_stringToMod(String mod, WIFICONNECT_MOD & result){
    if      (mod == "loop")     result = AWC_LOOP;
    else if (mod == "setup")    result = AWC_SETUP;
    else                        result = AWC_SETUP;
}
void WIFICONNECTSSID_MOD_stringToMod(String mod, WIFICONNECTSSID_MOD & result){
    if      (mod == "multi")    result = AWCS_MULTI;
    else if (mod == "ap")       result = AWCS_AP;    
    else if (mod == "normal")   result = AWCS_NORMAL;    
    else                        result = AWCS_NORMAL; 
}

void WIWIFICONFIGIP_MOD_toString(WIFICONFIGIP_MOD mod, String & result){
    switch(mod){
        case AWIP_IP:       result = "ip";      break;
        case AWIP_NOIP:     result = "noip";    break;
        default :           result = "noip";    break;
    }
}

void WiFiMode_t_toString(WiFiMode_t mod, String & result){
    switch(mod){
        case WIFI_OFF:      result = "WIFI_OFF";        break;
        case WIFI_STA:      result = "WIFI_STA";        break;
        case WIFI_AP:       result = "WIFI_AP";         break;
        case WIFI_AP_STA:   result = "WIFI_AP_STA";     break;
        // case WIFI_SHUTDOWN: result = "WIFI_SHUTDOWN";   break;
        // case WIFI_RESUME:   result = "WIFI_RESUME";     break;
        default :           result = "WIFI_STA";        break;
    }
}
void wl_status_t_toString(wl_status_t mod, String & result){
    switch(mod){
        case WL_NO_SHIELD:          result = "WL_NO_SHIELD";        break;
        case WL_IDLE_STATUS:        result = "WL_IDLE_STATUS";      break;
        case WL_NO_SSID_AVAIL:      result = "WL_NO_SSID_AVAIL";    break;
        case WL_CONNECTED:          result = "WL_CONNECTED";        break;
        case WL_CONNECT_FAILED:     result = "WL_CONNECT_FAILED";   break;
        case WL_CONNECTION_LOST:    result = "WL_CONNECTION_LOST";  break;
        case WL_DISCONNECTED:       result = "WL_DISCONNECTED";     break;
        default :                   result = "NULL";                break;
    }
}
#if defined(ESP8266)
    void station_status_t_toString(station_status_t mod, String & result){
        switch(mod){
            case STATION_IDLE:              result = "STATION_IDLE";        break;
            case STATION_CONNECTING:        result = "STATION_CONNECTING";      break;
            case STATION_WRONG_PASSWORD:    result = "STATION_WRONG_PASSWORD";    break;
            case STATION_NO_AP_FOUND:       result = "STATION_NO_AP_FOUND";        break;
            case STATION_CONNECT_FAIL:      result = "STATION_CONNECT_FAIL";   break;
            case STATION_GOT_IP:            result = "STATION_GOT_IP";  break;
            default :                       result = "NULL";                break;
        }
    }
#elif defined(ESP32)
#else
#endif        

void WiFiMode_t_stringToMod(String mod, WiFiMode_t & result){
    if      (mod == "WIFI_STA")     result = WIFI_STA;
    else if (mod == "WIFI_AP_STA")  result = WIFI_AP_STA;
    else                            result = WIFI_STA;
}

String wifiConnect::dnsName_get()               { return String(_DNSname);}
String wifiConnect::hostName_get()              { return adri_toolsv2Ptr_get()->ch_toString(_hostName);}
String wifiConnect::ip_get()                    { String result; WIWIFICONFIGIP_MOD_toString    (_cfgIp,        result); return result;}
String wifiConnect::connect_get()               { String result; WIFICONNECT_MOD_toString       (_connect,      result); return result;}
String wifiConnect::connectSSID_get()           { String result; WIFICONNECTSSID_MOD_toString   (_connectSSID,  result); return result;}
String wifiConnect::station_get()               { String result; WiFiMode_t_toString            (_station,      result); return result;}
String wifiConnect::credential_sta_pos_get()    { return String(_credential_sta_pos);}

String wifiConnect::statuConnect()      {String result; WIFICONNECT_MOD_toString(_statu_connect, result);           return result;}
String wifiConnect::statuConnectSSID()  {String result; WIFICONNECTSSID_MOD_toString(_statu_connectSSID, result);   return result;}
String wifiConnect::statSta()           {String result; WiFiMode_t_toString(_statu_sta, result);                    return result;}
String wifiConnect::statuIpMod()        {String result; WIWIFICONFIGIP_MOD_toString(_statu_ip, result);             return result;}
String wifiConnect::statuStatu()        {String result; WIFI_STATU_toString(_statu_current, result);                return result;}
String wifiConnect::espStationMod_get() {String result; WiFiMode_t_toString(WiFi.getMode(), result);                return result;}

String wifiConnect::currentSSID_get() {
    if (wifiConnect_ptr == nullptr) return "SSID";
   
    if (_statu_sta == WIFI_STA) { if (WiFi.status() != WL_CONNECTED) return "SSID"; else return WiFi.SSID();}
    if (_statu_sta == WIFI_AP)  return wifi_credentialAp_ptr->hostname_get();
    return "SSID";
}
String wifiConnect::currentIp_get() {
    if (wifiConnect_ptr == nullptr) return "IP";
    if (_statu_sta == WIFI_STA) return WiFi.localIP().toString();
    if (_statu_sta == WIFI_AP)  return WiFi.softAPIP().toString();
    return "IP";
}
IPAddress wifiConnect::_currentIp_get() {
    if (wifiConnect_ptr == nullptr) return WiFi.localIP();
    if (_statu_sta == WIFI_STA)     return WiFi.localIP();
    if (_statu_sta == WIFI_AP_STA)  return WiFi.localIP();
    if (_statu_sta == WIFI_AP)      return WiFi.softAPIP();
    return WiFi.localIP();
}
String wifiConnect::currentPsk_get() {
    if (wifiConnect_ptr == nullptr) return "";
    if (_statu_sta == WIFI_STA) return _credential_sta->psk_get();
    if (_statu_sta == WIFI_AP)  return wifi_credentialAp_ptr->psk_get();
    return "";
}
String wifiConnect::staPsk_get() {
    if (wifiConnect_ptr == nullptr) return "";
    return _credential_sta->psk_get();
}
String wifiConnect::staSsid_get() {
    if (wifiConnect_ptr == nullptr) return "";
    return _credential_sta->ssid_get();
}

String wifiConnect_get_ip(wifiConnect * obj)             {return obj->ip_get();}
String wifiConnect_get_connect(wifiConnect * obj)        {return obj->connect_get();}
String wifiConnect_get_connectSSID(wifiConnect * obj)    {return obj->connectSSID_get();}
String wifiConnect_get_station(wifiConnect * obj)        {return obj->station_get();}
String wifiConnect_get_credential_sta(wifiConnect * obj) {return obj->credential_sta_pos_get();}

String wifiConnect_get_statu_ip(wifiConnect * obj)              {return obj->statuIpMod();}
String wifiConnect_get_statu_connect(wifiConnect * obj)         {return obj->statuConnect();}
String wifiConnect_get_statu_connectSSID(wifiConnect * obj)     {return obj->statuConnectSSID();}
String wifiConnect_get_statu_station(wifiConnect * obj)         {return obj->statSta();}

String wifiConnect_get_hostname(wifiConnect * obj)              {return obj->hostName_get();}
String wifiConnect_get_dnsname(wifiConnect * obj)               {return obj->dnsName_get();}
String wifiConnect_get_ssid(wifiConnect * obj)                  {return obj->currentSSID_get();}

PROGMEM wifiConnect_getValue wifiConnect_getValues [] = {           
    {(char *)"ssid",                wifiConnect_get_ssid},
    {(char *)"dnsname",             wifiConnect_get_dnsname},
    {(char *)"hostname",            wifiConnect_get_hostname},

    {(char *)"ip",                  wifiConnect_get_ip},
    {(char *)"statu_ip",            wifiConnect_get_statu_ip},

    {(char *)"connect",             wifiConnect_get_connect},
    {(char *)"statu_connect",       wifiConnect_get_statu_connect},

    {(char *)"connectSSID",         wifiConnect_get_connectSSID},
    {(char *)"statu_connectSSID",   wifiConnect_get_statu_connectSSID},

    {(char *)"station",             wifiConnect_get_station},
    {(char *)"statu_station",       wifiConnect_get_statu_station},

    {(char *)"credential_sta",      wifiConnect_get_credential_sta},

};
int wifiConnect_getValues_count = ARRAY_SIZE(wifiConnect_getValues);

boolean wifiConnect::savToSpiff(){
    String filename = "/" + WIFICONNECT_FILENAME + ".txt";

    File f=FILESYSTEM.open(filename,"w");

    if (!f) {
        #ifdef DEBUG
            fsprintf("\n[wifiConnect_toSpiff] Error opening : %S\n", filename.c_str())
        #endif  
        return false;
    }

    // char buffer[120];
    String line = "";

    for (int j = 0; j < wifiConnect_getValues_count; ++j) {
        line += adri_toolsv2Ptr_get()->literal_item(
                wifiConnect_getValues[j].name, 
                wifiConnect_getValues[j].getValue(this)); 
    }

    f.print(line);

    f.close();  

    return true;        
}
boolean wifiConnect_load_fromSPIFF(String & result) {

    String filename = "/" + WIFICONNECT_FILENAME + ".txt";

    File f=FILESYSTEM.open(filename,"r");

    if (!f) {
        #ifdef DEBUG
            fsprintf("\n[wifiConnect_load_fromSPIFF] Error reading : %S\n", filename.c_str())
        #endif  
        return false;
    }

    #ifdef DEBUG
        fsprintf("\n[wifiConnect_load_fromSPIFF] DONE\n")
    #endif

    String line = "";

    while (f.available()) {line += char(f.read());}

    result = line;

    #ifdef DEBUG
        // fsprintf(" %s\n", result.c_str());
    #endif

    f.close();  

    return true;
}
boolean wifiConnect::load_fromSpiif(){

    String result = "";

    boolean load = wifiConnect_load_fromSPIFF(result);

    if (!load)
    {
        
        return false;
    }

    String value = "";

    // value = adri_toolsv2Ptr_get()->literal_value("connect",     result);
    // WIFICONNECT_MOD_stringToMod(value, _connect);

    // value = adri_toolsv2Ptr_get()->literal_value("connectSSID", result);
    // WIFICONNECTSSID_MOD_stringToMod(value, _connectSSID);

    // value = adri_toolsv2Ptr_get()->literal_value("credential_sta", result);
    // if (value != "") credential_sta_pos_set(value.toInt());

    // value = adri_toolsv2Ptr_get()->literal_value("station", result);
    // WiFiMode_t_stringToMod(value, _station);  

    return true;

}

void wifiConnect::print_cfg(){

    fsprintf("\n[wifiConnect print_cfg]\n");

    // String result;

    // WIFICONNECT_MOD_toString(_connect, result);
    // fsprintf("%-15s : %s\n", fsget(awc_str_connect).c_str(),        result.c_str());

    // WIFICONNECTSSID_MOD_toString(_connectSSID, result);
    // fsprintf("%-15s : %s\n", fsget(awc_str_connectSSID).c_str(),    result.c_str());

    // WiFiMode_t_toString(_station, result);
    // fsprintf("%-15s : %s\n", fsget(awc_str_station).c_str(),        result.c_str());

    // fsprintf("%-15s : %d\n", fsget(awc_str_porgress).c_str(),       _porgress);

    for (int j = 0; j < wifiConnect_getValues_count; ++j) {
        fsprintf("%-15s: %-20s\n", 
            wifiConnect_getValues[j].name, 
            wifiConnect_getValues[j].getValue(this).c_str()
        );
    }
}
void wifiConnect::print_sta(){
    _credential_sta->print();
}

boolean wifiConnect::setup_ap(){

    if (_statu_connectSSID != AWCS_AP) return false;

    #ifdef DEBUG
        fsprintf("\n\t[setup_ap]\n");
    #endif

     _statu_sta          = WIFI_AP;

    if (!wifi_credentialAp_ptr->setup_id()) _statu_current = wifi_statu_connect_ap_idFail;
    else {
        if (!wifi_credentialAp_ptr->setup_ip()) _statu_ip = AWIP_NOIP;
        _statu_current      = wifi_statu_connect_ap_begin;                
        connect_ap();
    }    
    return true;
}

boolean wifiConnect::WIFImulti_setup_id(){
    String sSSID, sPsk;

    _credential_sta = wifi_credential_sta_array[_credential_sta_pos];

    sSSID       = _credential_sta->ssid_get();
    sPsk        = _credential_sta->psk_get();

    if ((sSSID == "") || (sPsk == "")) {
        return false;
    }

    // print_sta();

    return true;
}
boolean wifiConnect::setup_id(){
    String sSSID, sPsk;

    _credential_sta = wifi_credential_sta_array[_credential_sta_pos];

    sSSID       = _credential_sta->ssid_get();
    sPsk        = _credential_sta->psk_get();

    if ((sSSID == "") || (sPsk == "")) {
        return false;
    }

    // print_sta();

    return true;
}
boolean wifiConnect::setup_ip(){
    String sIp, sSubnet, sGateway;

    sIp         = _credential_sta->ip_get();
    sSubnet     = _credential_sta->subnet_get();
    sGateway    = _credential_sta->gateway_get();

    if (!isValidIp(sIp))                    return false;
    // if (!isValidIp(sIp, sSubnet, sGateway)) return false;

    return true;
}
boolean wifiConnect::setup_sta_normal(){

    if (_statu_connectSSID != AWCS_NORMAL) return false;

    #ifdef DEBUG
        fsprintf("\n\t[setup_sta_normal]\n");
    #endif

    if (!setup_id()) {
        _statu_current = wifi_statu_connect_sta_idFail;
        return false;
    }
    
    const char * bl = "";
    switch(_statu_connect) {
        case AWC_SETUP:

            _statu_current = wifi_statu_connect_sta_begin;

            // ###############################################
            WiFi.disconnect(true);
            WiFi.softAPdisconnect(true);
            #if defined(ESP8266)
                WiFi.setPhyMode(WIFI_PHY_MODE_11B);
            #elif defined(ESP32)
            #else
            #endif                    

            delay(1000);
            WiFi.mode(WIFI_STA);   

            if (_hostName != bl) WiFi.hostname(_hostName);

            #if defined(ESP8266)
                wifi_set_sleep_type(NONE_SLEEP_T);
            #elif defined(ESP32)
            #else
            #endif                    
                        
            // ###############################################

            connect_sta_normal();

            return true;
        break;

         case AWC_LOOP:

            _statu_current = wifi_statu_connect_sta_begin;

            if (_hostName != bl) WiFi.hostname(_hostName);

            return true;
        break; 
        default: break;      
    }


    return false;
}

String  ESP8266WiFiMulti_tested[CREDENTIAL_MAX];
int     ESP8266WiFiMulti_testedCnt = 0;
boolean ESP8266WiFiMulti_testedSearch(String search){
   boolean result = false;
   for (int i = 0; i < CREDENTIAL_MAX; ++i)
    {
        if (search == ESP8266WiFiMulti_tested[i]) {result = true; break;}
    }
    return result;
}
boolean wifiConnect::setup_sta_multi(){

    if (_statu_connectSSID != AWCS_MULTI) return false;

    #ifdef DEBUG
        fsprintf("\n\t[setup_sta_multi]\n");
    #endif

    ESP8266WiFiMulti_testedCnt = 0;
    for (int i = 0; i < CREDENTIAL_MAX; ++i)
    {
       ESP8266WiFiMulti_tested[i]="";
    }

    const char * bl = "";
    adri_timer * timer;
    switch(_statu_connect) {
        case AWC_SETUP:

            _statu_current = wifi_statu_connect_sta_begin;

            // ###############################################
            WiFi.disconnect(true);
            WiFi.softAPdisconnect(true);
            #if defined(ESP8266)
                WiFi.setPhyMode(WIFI_PHY_MODE_11B);
            #elif defined(ESP32)
            #else
            #endif   

            delay(1000);
            WiFi.mode(WIFI_STA);   
          
            if (_hostName != bl) WiFi.hostname(_hostName);

            #if defined(ESP8266)
                wifi_set_sleep_type(NONE_SLEEP_T);
            #elif defined(ESP32)
            #else
            #endif           
            // ###############################################

            timer = new adri_timer(60000, "", true);
            while (true) {
                wl_status_t statu = ESP8266WiFiMulti_run(this, _statu_sta, &_porgress);
                if (statu == WL_CONNECTED) break;

                if (timer->loop()) {

                     _statu_sta = WIFI_AP;
                    if (!wifi_credentialAp_ptr->setup_id()) _statu_current = wifi_statu_connect_ap_idFail;
                    else {
                        if (!wifi_credentialAp_ptr->setup_ip()) _statu_ip = AWIP_NOIP;
                        _statu_current      = wifi_statu_connect_ap_begin;    
                        _statu_connectSSID  = AWCS_AP;              
                        setup_ap();
                    }   
                    break;
                }

                yield();
            }

            delete timer;

            #ifdef DEBUG
                fsprintf("\n\t[ESP8266WiFiMulti_run DONE]\n");
            #endif

            return true;
        break;
        default: 
            break;
    }

    return false;
}

// boolean wifiConnect::loop_sta_multi(){
//     if (ESP8266WiFiMulti_run(this, _statu_sta, &_porgress) == WL_CONNECTED) return false;
//     return true;

// }


void wifiConnect::statuPrint(String header){
    String result;

    fsprintf("%s", header.c_str());

    WIFICONNECT_MOD_toString(_statu_connect, result);
    fsprintf("%-15s : %s\n", fsget(awc_str_connect).c_str(),        result.c_str());

    WIFICONNECTSSID_MOD_toString(_statu_connectSSID, result);
    fsprintf("%-15s : %s\n", fsget(awc_str_connectSSID).c_str(),    result.c_str());

    WiFiMode_t_toString(_statu_sta, result);
    fsprintf("%-15s : %s\n", fsget(awc_str_station).c_str(),        result.c_str());

    WIWIFICONFIGIP_MOD_toString(_statu_ip, result);
    fsprintf("%-15s : %s\n", fsget(awc_str_ipmod).c_str(),          result.c_str()); 

    WIFI_STATU_toString(_statu_current, result);
    fsprintf("%-15s : %s\n", fsget(awc_str_statu).c_str(),          result.c_str()); 

    WiFiMode_t w_mod = WiFi.getMode(); 
    WiFiMode_t_toString(w_mod, result);
    fsprintf("W%-15s : %s\n", fsget(awc_str_station).c_str(), result.c_str()); 

}
void wifiConnect::stationIsSTA(boolean & result) {
    boolean ret = false;
    switch(_statu_current){
        case wifi_statu_connect_sta_succes:
            ret = true; 
        break;
        default: break;
    }
    result = ret;
}



WIFI_STATU wifiLoop_statu = wifi_statu_none;
unsigned long wifi_statu_sta_search_last;
unsigned long wifi_statu_sta_search_max = 15000;

void wifiConnect::setup(){
    wifiLoop_statu = wifi_statu_none;

    boolean resultConnect;

    _statu_ip           = _cfgIp;
    _statu_sta          = _station;
    _statu_connect      = _connect;
    _statu_connectSSID  = _connectSSID;
    _statu_current      = wifi_statu_none;

    #ifdef DEBUG
        statuPrint("\n[wifiConnect::setup] statuPrint START\n");
    #endif

    setup_ap();
    setup_sta_normal();
    setup_sta_multi();

    if (_statu_connectSSID == AWCS_NORMAL) {
        wl_status_t statu = WiFi.status(); 
        if (statu != WL_CONNECTED) {
             _statu_sta = WIFI_AP;
            if (!wifi_credentialAp_ptr->setup_id()) _statu_current = wifi_statu_connect_ap_idFail;
            else {
                _statu_current      = wifi_statu_connect_ap_begin;    
                _statu_connectSSID  = AWCS_AP;              
                setup_ap();
            }   
        }
    }

    WiFiMode_t w_mod = WiFi.getMode(); 
    switch(_statu_current){
        case wifi_statu_connect_sta_begin:
            resultConnect = wifi_connect_result("[wifiConnect::setup]");
            if (resultConnect)  {
                _statu_current = wifi_statu_connect_sta_succes;
                if (!setup_ip()) _statu_ip = AWIP_NOIP; 
                if (_statu_ip == AWIP_IP) configIp();                
            }
            else                _statu_current = wifi_statu_connect_sta_coFail;
        break;   
        case wifi_statu_connect_ap_begin:
            if (w_mod != WIFI_AP)   _statu_current = wifi_statu_connect_ap_coFail;
            else                    _statu_current = wifi_statu_connect_ap_succes;
        break;
        default: break;
    }
    #ifdef DEBUG
        statuPrint("\n[wifiConnect::setup] statuPrint END\n");
        if (_statu_sta == WIFI_STA){
             Serial.printf("\n[WIFICLASS WIFI_STA]\n\t%s - %s - %s\n\n", 
                WiFi.localIP().toString().c_str(), 
                WiFi.gatewayIP().toString().c_str(), 
                WiFi.subnetMask().toString().c_str()
            );       
        }
        if (_statu_sta == WIFI_AP){
             Serial.printf("\n[WIFICLASS WIFI_AP]\n\t%s \n\n", 
                WiFi.softAPIP().toString().c_str()
                // WiFi.apGatewayIP().toString().c_str(), 
                // WiFi.apSubnetMask().toString().c_str()
            );       
        }
    #endif
}

void wifiConnect::setupLoop(){

    // boolean resultConnect;

    _statu_ip           = _cfgIp;
    _statu_sta          = _station;
    _statu_connect      = _connect;
    _statu_connectSSID  = _connectSSID;
    _statu_current      = wifi_statu_none;

    #ifdef DEBUG
        statuPrint("\n[wifiConnect::setupLoop] statuPrint START\n");
    #endif

    if (_statu_connectSSID != AWCS_NORMAL) return ;


    if (!setup_id()) {
        _statu_current = wifi_statu_connect_sta_idFail;
        return ;
    }
    
    if (!setup_ip()) _statu_ip = AWIP_NOIP;

    _statu_current = wifi_statu_connect_sta_begin;

    wifiLoop_statu = wifi_statu_sta_begin;


}

void wifiConnect::connect_ap(){
    char ssid[80];
    char password[80];

    sprintf(ssid, "%s",     wifi_credentialAp_ptr->hostname_get().c_str());
    sprintf(password, "%s", wifi_credentialAp_ptr->psk_get().c_str());  

    WiFi.disconnect();
    WiFi.softAPdisconnect(true);
    delay(1000);

    WiFi.mode(WIFI_AP);

    if (_statu_ip == AWIP_IP) {
        IPAddress staticIP; 
        IPAddress subnet(255, 255, 255, 0);
        staticIP = adri_toolsv2Ptr_get()->string2ip(wifi_credentialAp_ptr->ip_get());
        WiFi.softAPConfig(staticIP, staticIP, subnet);
    }
    WiFi.softAP(ssid, password);

    String ap_ssid   = String(ssid);
    String ap_ip     = adri_toolsv2Ptr_get()->ip2string(WiFi.softAPIP());
    #ifdef DEBUG
        Serial.printf("\n[wfifi_connect_ap] ssid: %s - pswd: %s - ip: %s\n", ap_ssid.c_str(), password, ap_ip.c_str());
    #endif    
}

void wifiConnect::configIp(){

    #ifdef DEBUG
        Serial.printf("\n[wifiConnect configIp]\n");
    #endif

    String sIp, sSubnet, sGateway;

    sIp         = _credential_sta->ip_get();
    sSubnet     = _credential_sta->subnet_get();
    sGateway    = _credential_sta->gateway_get();
    IPAddress staticIP  = adri_toolsv2Ptr_get()->string2ip(sIp);
    IPAddress gateway   = adri_toolsv2Ptr_get()->string2ip(sGateway);
    IPAddress subnet    = adri_toolsv2Ptr_get()->string2ip(sSubnet);

    if (isValidIp(sIp, WiFi.subnetMask().toString(), WiFi.gatewayIP().toString())) {
        #ifdef DEBUG
            Serial.printf("\tassign ip to: %s \n",  sIp.c_str());
        #endif   

        if (isValidIp(sGateway)) {
            #ifdef DEBUG
                Serial.printf("\tassign gateway to: %s \n",  sGateway.c_str());
            #endif    
        } else gateway = WiFi.gatewayIP();

        if (isValidIp(sSubnet)) {
            #ifdef DEBUG
                Serial.printf("\tassign subnet to: %s \n",  sSubnet.c_str());
            #endif    
        } else subnet = WiFi.subnetMask();

            // [wifi_connect_sta] Done: 192.168.0.100 - 192.168.0.254 - 255.255.255.0
        /**
         * Change IP configuration settings disabling the dhcp client
         * @param local_ip   Static ip configuration
         * @param gateway    Static gateway configuration
         * @param subnet     Static Subnet mask
         * @param dns1       Static DNS server 1
         * @param dns2       Static DNS server 2
         */
         Serial.printf("\n[WiFi.config]\n\tstaticIP:%s - gateway:%s - subnet:%s\n\n", 
            staticIP.toString().c_str(), 
            gateway.toString().c_str(), 
            subnet.toString().c_str());

        WiFi.config(staticIP, gateway, subnet);



        // String AP_psk = "adrilight1234";
        // WiFi.softAP(b_host_name, AP_psk);

    }  else {_statu_ip = AWIP_NOIP;}
} 

void wifiConnect::connect_sta_normal(){
    char ssid[80];
    char password[80];
    sprintf(ssid, "%s",     _credential_sta->ssid_get().c_str());
    sprintf(password, "%s", _credential_sta->psk_get().c_str());   
    wifiConnect_connect_sta_normal(ssid, password, _statu_sta, &_porgress);
}

WIFI_STATU wifiConnect::wifi_loop_statu(){return wifiLoop_statu;}
void wifiConnect::wifi_loop(){
    if (_statu_connect != AWC_LOOP)                     return ;
    if (wifiLoop_statu == wifi_statu_none)              return ;
    if (wifiLoop_statu == wifi_statu_sta_isconnected)   return ;


    IPAddress ap_subnet(255, 255, 255, 0);
    IPAddress staticIP; 
    IPAddress gateway;
    IPAddress subnet;  
    // char sSSID[80];
    // char sPSWD[80]; 
    WiFiMode_t w_mod; 
    // boolean resultConnect = false;
    const char * bl = "";
    switch (wifiLoop_statu) {

        case wifi_statu_sta_begin:
            // fsprintf("\n[wifiConnect::wifi_loop - wifi_statu_sta_begin]\n");

            char ssid[80];
            char password[80];
            sprintf(ssid, "%s",     _credential_sta->ssid_get().c_str());
            sprintf(password, "%s", _credential_sta->psk_get().c_str());  

            // if (_statu_ip != AWIP_NOIP) configIp();

            // // WiFi.setAutoReconnect(true);  
            // WiFi.disconnect();
            // WiFi.softAPdisconnect(true);
            // delay(1000);
            // WiFi.mode(WIFI_STA);   
            // WiFi.begin(ssid, password);

            // ###############################################
            WiFi.disconnect(true);
            WiFi.softAPdisconnect(true);
            #if defined(ESP8266)
                WiFi.setPhyMode(WIFI_PHY_MODE_11B);
            #elif defined(ESP32)
            #else
            #endif 

            delay(1000);
            WiFi.mode(WIFI_STA);   

            if (_hostName != bl) WiFi.hostname(_hostName);
            #if defined(ESP8266)
                wifi_set_sleep_type(NONE_SLEEP_T);   
            #elif defined(ESP32)
            #else
            #endif                                 
            // ###############################################

            WiFi.begin(ssid, password);

            wifi_statu_sta_search_last = millis();

            wifiLoop_statu = wifi_statu_sta_search;
        break;

        case wifi_statu_sta_search:

            if (WiFi.status() == WL_CONNECTED){


                // Serial.printf("\n[wifiConnect::wifi_loop - wifi_statu_sta_search] : %s - %s - %s\n", 
                //     WiFi.localIP().toString().c_str(), 
                //     WiFi.gatewayIP().toString().c_str(), 
                //     WiFi.subnetMask().toString().c_str()
                // );

                if (!setup_ip()) _statu_ip = AWIP_NOIP; 
                if (_statu_ip == AWIP_IP) configIp();
            
                _statu_current = wifi_statu_connect_sta_succes;
                wifiLoop_statu = wifi_statu_sta_isconnected;
                

            }
            else {
                if ((millis()-wifi_statu_sta_search_last)>wifi_statu_sta_search_max){
                    wifiLoop_statu = wifi_statu_sta_fail;
                    _statu_current = wifi_statu_connect_sta_coFail;
                }
            }
        break;

        case wifi_statu_sta_fail:

            // fsprintf("\n[wifiConnect::wifi_loop - wifi_statu_sta_fail]\n");

            if (!wifi_credentialAp_ptr->setup_ip()) _statu_ip = AWIP_NOIP;
            _statu_current      = wifi_statu_connect_ap_begin;    
            _statu_connectSSID  = AWCS_AP;              
            setup_ap();

            w_mod = WiFi.getMode(); 
            switch(_statu_current){
                case wifi_statu_connect_ap_begin:
                    if (w_mod != WIFI_AP)   _statu_current = wifi_statu_connect_ap_coFail;
                    else                    _statu_current = wifi_statu_connect_ap_succes;
                break;
                default: break;
            }

            wifiLoop_statu = wifi_statu_sta_isconnected;
            

        break; 
        default: break;
    }


    // WiFi.mode(WIFI_STA);   
    // WiFi.setAutoReconnect(true);  

    // WiFi.disconnect();
    // delay(20);

    // #ifdef OLED_ENABLE
    //     int count = 0;
    // #endif 

    // WiFi.begin(ssid, password);

     
}


String wifi_credential_sta_get_ssid(wifi_credential_sta * obj)     {return obj->ssid_get();}
String wifi_credential_sta_get_psk(wifi_credential_sta * obj)      {return obj->psk_get();}
String wifi_credential_sta_get_ip(wifi_credential_sta * obj)       {return obj->ip_get();}
String wifi_credential_sta_get_subnet(wifi_credential_sta * obj)   {return obj->subnet_get();}
String wifi_credential_sta_get_gateway(wifi_credential_sta * obj)  {return obj->gateway_get();}

PROGMEM wifi_credential_sta_getValue wifi_credential_sta_getValues [] = {           
    {(char *)"ssid",        wifi_credential_sta_get_ssid},
    {(char *)"psk",         wifi_credential_sta_get_psk},
    {(char *)"ip",          wifi_credential_sta_get_ip},
    {(char *)"subnet",      wifi_credential_sta_get_subnet},
    {(char *)"gateway",     wifi_credential_sta_get_gateway},
};
int wifi_credential_sta_getValues_count = ARRAY_SIZE(wifi_credential_sta_getValues);



wifi_credential_sta::wifi_credential_sta(){

}

String wifi_credential_sta::ssid_get()    { return _ssid;}
String wifi_credential_sta::psk_get()     { return _psk;}
String wifi_credential_sta::ip_get()      { return _ip;}
String wifi_credential_sta::subnet_get()  { return _subnet;}
String wifi_credential_sta::gateway_get() { return _gateway;}

void wifi_credential_sta::ssid_set(String value)    { _ssid       = value;}
void wifi_credential_sta::psk_set(String value)     { _psk        = value;}
void wifi_credential_sta::ip_set(String value)      { _ip         = value;}
void wifi_credential_sta::subnet_set(String value)  { _subnet     = value;}
void wifi_credential_sta::gateway_set(String value) { _gateway    = value;}

void wifi_credential_sta::print(){

    fsprintf("\n[wifi_credential_sta print]\n")

    for (int j = 0; j < wifi_credential_sta_getValues_count; ++j) {
        fsprintf("%-5s: %-20s size:%-5d", 
            wifi_credential_sta_getValues[j].name, 
            wifi_credential_sta_getValues[j].getValue(this).c_str(),
            wifi_credential_sta_getValues[j].getValue(this).length()+1
        );
    }

    fsprintf("\n");  

}

boolean wifi_credential_sta_fromSPIFF(String * array) {
    String filename = "/" + CREDENTIAL_FILENAME + ".txt";

    File f=FILESYSTEM.open(filename,"r");
    if (!f) {
        #ifdef DEBUG
            fsprintf("\n[wifi_credential_sta_fromSPIFF] Error reading : %S\n", filename.c_str())
        #endif  
        return false;
    }
    #ifdef DEBUG
        fsprintf("\n[wifi_credential_sta_fromSPIFF]\n")
    #endif
    String xml;
    int cnt = 0;
    while (f.position()<f.size()) {
        xml = f.readStringUntil('\n');
        if (xml != "") {
            array[cnt] = xml;
            #ifdef DEBUG
                fsprintf("[%d] %s\n", cnt, xml.c_str());
            #endif
            cnt++;
        }
    }
    f.close();  
    return true;
}

void wifi_credential_set(int pos, String ssid, String psk, String ip, String subnet, String gateway) {
        wifi_credential_sta_array[pos]->ssid_set(ssid);

        wifi_credential_sta_array[pos]->psk_set(psk);

        wifi_credential_sta_array[pos]->ip_set(ip);

        wifi_credential_sta_array[pos]->subnet_set(subnet);

        wifi_credential_sta_array[pos]->gateway_set(gateway);
}

wifi_credential_sta * wifi_credential_get(int pos){
    return wifi_credential_sta_array[pos];
}

boolean wifi_credential_sta_fromSPIFF() {
    // char buffer_1[255];

    String  cr_array[CREDENTIAL_MAX+1];

    boolean cr_fromSpiff = wifi_credential_sta_fromSPIFF(cr_array);
    if (!cr_fromSpiff) {
        for (int i = 0; i < CREDENTIAL_MAX; ++i) {
            wifi_credential_sta_array[i] = new wifi_credential_sta();
            wifi_credential_set(i, "", "", "", "", "");
        }  
        wifi_credential_sta_best.last = "";
        wifi_credential_sta_best.best = "";
        return false;
    }

    wifi_credential_sta_best.last = adri_toolsv2Ptr_get()->literal_value("last", cr_array[CREDENTIAL_MAX]);
    wifi_credential_sta_best.best = adri_toolsv2Ptr_get()->literal_value("best", cr_array[CREDENTIAL_MAX]);

    for (int i = 0; i < CREDENTIAL_MAX; ++i) {
        wifi_credential_sta_array[i] = new wifi_credential_sta();

        // fsprintf("#%d - %s\n",i, cr_array[i].c_str());
        wifi_credential_set(i, 
            adri_toolsv2Ptr_get()->literal_value("ssid",       cr_array[i]).c_str(), 
            adri_toolsv2Ptr_get()->literal_value("psk",        cr_array[i]).c_str(), 
            adri_toolsv2Ptr_get()->literal_value("ip",         cr_array[i]).c_str(), 
            adri_toolsv2Ptr_get()->literal_value("subnet",     cr_array[i]).c_str(), 
            adri_toolsv2Ptr_get()->literal_value("gateway",    cr_array[i]).c_str());
    }  
    return true;


}
void wifi_credential_sta_print(){

    fsprintf("\n[wifi_credential_sta_print]\n")
    for (int i = 0; i < CREDENTIAL_MAX; ++i) {
        fsprintf("#%d ",i)
        for (int j = 0; j < wifi_credential_sta_getValues_count; ++j) {
            fsprintf("%-5s: %-20s size:%-5d", 
                wifi_credential_sta_getValues[j].name, 
                wifi_credential_sta_getValues[j].getValue(wifi_credential_sta_array[i]).c_str(),
                wifi_credential_sta_getValues[j].getValue(wifi_credential_sta_array[i]).length()+1
            );
        }
        fsprintf("\n");  
    } 

    fsprintf("\nwifi_credential_sta_best\n\tlast: %s\n\tbest: %s\n", 
        wifi_credential_sta_best.last.c_str(), 
        wifi_credential_sta_best.last.c_str()); 
}

boolean wifi_credential_sta_toSpiff(){
    String filename = "/" + CREDENTIAL_FILENAME + ".txt";

    File f=FILESYSTEM.open(filename,"w");

    if (!f) {
        #ifdef DEBUG
            fsprintf("\n[wifi_credential_sta_toSpiff] Error opening : %S\n", filename.c_str())
        #endif  
        return false;
    }

    // char buffer[120];
    String line = "";
    for (int i = 0; i < CREDENTIAL_MAX; ++i) {
        for (int j = 0; j < wifi_credential_sta_getValues_count; ++j) {
            line += adri_toolsv2Ptr_get()->literal_item(
                    wifi_credential_sta_getValues[j].name, 
                    wifi_credential_sta_getValues[j].getValue(wifi_credential_sta_array[i])); 
        }
        line += "\n";
    }

    line += adri_toolsv2Ptr_get()->literal_item("last", wifi_credential_sta_best.last);
    line += adri_toolsv2Ptr_get()->literal_item("best", wifi_credential_sta_best.best);
    line += "\n";

    #ifdef DEBUG
        fsprintf("\n[wifi_credential_sta_toSpiff]\n");
        Serial.println(line);
    #endif

    f.print(line);

    f.close();  

    return true;        
}


boolean wifi_credential_sta_toSpiff(String line){

    String filename = "/" + CREDENTIALAP_FILENAME + ".txt";

    File f=FILESYSTEM.open(filename,"w");

    if (!f) {
        #ifdef DEBUG
            fsprintf("\n[wifi_credential_sta_toSpiff] Error opening : %S\n", filename.c_str())
        #endif  
        return false;
    }

    f.print(line);

    f.close();  

    return true;        
}
void wifi_credential_ap::sav_toSpiff(){
    String line = "";

    line += adri_toolsv2Ptr_get()->literal_item("psk",     _psk);
    line += adri_toolsv2Ptr_get()->literal_item("ip",      _ip);
    line += adri_toolsv2Ptr_get()->literal_item("subnet",  _subnet);

    wifi_credential_sta_toSpiff(line);
}



boolean isValidIp(String sIp){
    char buffer[80];
    sprintf(buffer, "%s", sIp.c_str());
    IPAddress ip;
    return ip.fromString(buffer);
}
boolean isValidIp(const char * string){
    IPAddress ip;
    return ip.fromString(string);
}
boolean isValidIp(IPAddress ip){

    String s_ip = adri_toolsv2Ptr_get()->ip2string(ip);

    char buffer[80];
    sprintf(buffer, "%s", s_ip.c_str());

    if (!isValidIp(buffer)) return false;

    return true;
}
boolean isValidIp(String sIp, String sSubnet, String sGateway  ){

    IPAddress ip        = adri_toolsv2Ptr_get()->string2ip(sIp);
    IPAddress gateway   = adri_toolsv2Ptr_get()->string2ip(sGateway);
    IPAddress subnet    = adri_toolsv2Ptr_get()->string2ip(sSubnet);

    #ifdef DEBUG
        fsprintf("\n[isValidIp] ");
        fsprintf("ip: %s ",         adri_toolsv2Ptr_get()->ip2string(ip).c_str());
        fsprintf("sGateway: %s ",   adri_toolsv2Ptr_get()->ip2string(gateway).c_str());
        fsprintf("sSubnet: %s ",    adri_toolsv2Ptr_get()->ip2string(subnet).c_str());
    #endif

    for (int i = 0; i < 4; ++i)
    {
        if (subnet[i] == 255) {
            if (ip[i] != gateway[i]) return false;
        } 
    }
    boolean ret = false;
    for (int i = 0; i < 4; ++i)
    {
        if (subnet[i] < 255) {
            if ((ip[i]>0) && (ip[i]<gateway[i])) ret = true;
        } 
    }

    #ifdef DEBUG
        fsprintf(" -> result: %d\n", ret);
    #endif

    return ret;
}


void wifiConnect_connect_sta_normal(char * ssid, char * password, WiFiMode_t mod, wifiConnect_progress * _porgress) {


    #ifdef DEBUG
        fsprintf("\n[wifiConnect_connect_sta_normal v1] ssid: %s - pswd: %s \n", ssid, password);
    #endif  

    int retries = 0;
    int maxRetries = 0;   

    WiFi.begin(ssid, password);

    #ifdef DEBUG
        fsprintfs("\n");
    #endif    

    uint8_t count=0;
    while (WiFi.status() != WL_CONNECTED)  {
        retries++;

        // #ifdef OLED_ENABLE
            count+=30;
            if(_porgress!=NULL) (*_porgress)(count);
        // #endif

        if (retries > 40) {  

            WiFi.disconnect();
            delay(1000);
            
            WiFi.begin(ssid, password);

            retries = 0;
            maxRetries++;

            #ifdef DEBUG
                fsprintf("\n\tmaxRetries: %d\n", maxRetries);
            #endif
        }   

        delay(200);
        #ifdef DEBUG
            Serial.print(".");
        #endif

        if (maxRetries > 4) break;
    }






    #ifdef DEBUG
        Serial.println("");
        Serial.println("[wifiConnect_connect_sta_normal v1] Done");
    #endif  
}


// #elif defined(ESP8266)
wl_status_t wifiConnect_connect_sta_normal(char * ssid, char * password, int32_t bestChannel, uint8_t bestBSSID[], WiFiMode_t mod, wifiConnect_progress * _porgress, wl_status_t status) {


    #ifdef DEBUG
        fsprintf("\n[wifiConnect_connect_sta_normal v2] ssid: %s - pswd: %s \n", ssid, password);
    #endif  

    int retries = 0;
    int maxRetries = 0;   

    WiFi.begin(ssid, password, bestChannel, bestBSSID);

    #ifdef DEBUG
        fsprintfs("\n");
    #endif    

    uint8_t count=0;

    while (WiFi.status() != WL_CONNECTED )  {
        retries++;

        // #ifdef OLED_ENABLE
            count+=30;
            if(_porgress!=NULL) (*_porgress)(count);
        // #endif

        if (retries > 20) {  

            WiFi.disconnect();
            delay(1000);
            
            WiFi.begin(ssid, password, bestChannel, bestBSSID);

            retries = 0;
            maxRetries++;

            #ifdef DEBUG
                fsprintf("\n\tmaxRetries: %d\n", maxRetries);
            #endif
        }   

        delay(250);

        #ifdef DEBUG
            Serial.print(".");
        #endif

        if (maxRetries > 3) break;

    }

    #ifdef DEBUG
        Serial.println("");
        Serial.println("[wifiConnect_connect_sta_normal v2] Done");
    #endif  

    return WiFi.status();
}
// #endif

boolean wifi_connect_result(String debug){
    if (wifiConnect_ptr == nullptr) return false;
    if (WiFi.status() == WL_CONNECTED) {
        // #ifdef DEBUG
            if (debug!="") {
                Serial.printf("\n[wifi_connect_result -> %s] WL_CONNECTED: %s - %s - %s\n\n", 
                    debug.c_str(), 
                    WiFi.localIP().toString().c_str(), 
                    WiFi.gatewayIP().toString().c_str(), 
                    WiFi.subnetMask().toString().c_str());                
            }
        // #endif
        return true;
    } else {
        #ifdef DEBUG
            if (debug!="") Serial.printf("\n[wifi_connect_result  -> %s] Fail\n\n", debug.c_str());
        #endif
        return false;
    }

}
void wifi_connect_statu(){
    wifiConnect_instance()->print_cfg();
    // wifiConnect_instance()->print_sta();
    // wifi_credentialAp_ptr->print();

    Serial.printf("\n[WIFI_intance] localIP: %s - gatewayIP: %s - subnetMask: %s\n", 
    WiFi.localIP().toString().c_str(), 
    WiFi.gatewayIP().toString().c_str(), 
    WiFi.subnetMask().toString().c_str());

    String str_mod;
    WiFiMode_t wifiMod = WiFi.getMode();
    WiFiMode_t_toString(wifiMod, str_mod);
    fsprintf("[WiFiMode_t] %s\n", str_mod.c_str());

    // #if defined(ESP8266)
    String str_statu;
    wl_status_t wifiStatu = WiFi.status();
    wl_status_t_toString(wifiStatu, str_statu);
    fsprintf("[wl_status_t] %s\n", str_statu.c_str());
    // #endif

    #if defined(ESP8266)
    String str_station;
    station_status_t stationStatus = wifi_station_get_connect_status();
    station_status_t_toString(stationStatus, str_station);
    fsprintf("[station_status_t] %s\n", str_station.c_str());

    fsprintf("[auto_connect] %d\n", wifi_station_get_auto_connect());
    #endif


}



#define DEBUG_WIFI_MULTI(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ##__VA_ARGS__ )
// #define DEBUG_ESP_WIFI

// #if defined(ESP8266)
struct WifiAPEntry {
    char *  ssid;
    char *  passphrase;
};

// typedef std::vector<WifiAPEntry> WifiAPlist;

wl_status_t ESP8266WiFiMulti_run(wifiConnect * obj,  WiFiMode_t mod, wifiConnect_progress * porgress) {
    char ssid[80];
    char psk[80];

    wl_status_t status = WiFi.status();
    if(status == WL_DISCONNECTED || status == WL_NO_SSID_AVAIL || status == WL_IDLE_STATUS || status == WL_CONNECT_FAILED) {

        int8_t scanResult = WiFi.scanComplete();

        if(scanResult == WIFI_SCAN_RUNNING) {
            // scan is running, do nothing yet
            status = WL_NO_SSID_AVAIL;
            return status;
        } 

        if(scanResult == 0) {
            // scan done, no ssids found. Start another scan.
            #ifdef DEBUG_ESP_WIFI
                DEBUG_WIFI_MULTI("[WIFI] scan done\n");
                DEBUG_WIFI_MULTI("[WIFI] no networks found\n");
                DEBUG_WIFI_MULTI("\n\n");
                DEBUG_WIFI_MULTI("[WIFI] start scan\n");
            #endif   

            WiFi.scanDelete();

            delay(0);

            WiFi.disconnect();
            
            // scan wifi async mode
            WiFi.scanNetworks(true);
            return status;
        } 

        if(scanResult > 0) {
            // scan done, analyze
            WifiAPEntry bestNetwork { NULL, NULL };
            int bestNetworkDb = 1024;
            uint8_t bestBSSID[6];
            int32_t bestChannel;

            #ifdef DEBUG_ESP_WIFI
                DEBUG_WIFI_MULTI("[WIFI] scan done\n");
                DEBUG_WIFI_MULTI("[WIFI] %d networks found\n", scanResult);
            #endif

            delay(0);

            // bool Iknown = false;

            // for(int8_t n = 0; n < scanResult; ++n) {
            // int8_t i = 0;
            int8_t n = 0;
            int8_t pass = 0;
            while(true){


                String ssid_scan;
                int32_t rssi_scan;
                uint8_t sec_scan;
                uint8_t* BSSID_scan;
                int32_t chan_scan;
                bool hidden_scan;
                
// (uint8_t networkItem, String &ssid, uint8_t &encryptionType, int32_t &RSSI, uint8_t* &BSSID, int32_t &channel);
                WiFi.getNetworkInfo(n, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan);

                bool known = false;
                // for(auto entry : wifi_credential_sta_array) {
                for (int C = 0; C < CREDENTIAL_MAX; ++C) {
                    
                    if (ESP8266WiFiMulti_testedSearch(ssid_scan)) continue;

                    sprintf(ssid, "%s", wifi_credential_sta_array[C]->ssid_get().c_str());
                    sprintf(psk, "%s",  wifi_credential_sta_array[C]->psk_get().c_str());    
                   

                    if(ssid_scan == ssid) { // SSID match
                        if(pass > 1) known = true;
                        if(-rssi_scan < bestNetworkDb) { // best network
                            if(sec_scan == WIFI_AUTH_OPEN || (psk!=(char *)"")) { // check for passphrase if not open wlan
                                bestNetworkDb           = -rssi_scan;
                                bestChannel             = chan_scan;
                                bestNetwork.ssid        = ssid;
                                bestNetwork.passphrase  = psk;
                                memcpy((void*) &bestBSSID, (void*) BSSID_scan, sizeof(bestBSSID));
                                #ifdef DEBUG_ESP_WIFI
                                    DEBUG_WIFI_MULTI("[SSID BEST] ssid:%-12s psk: %-12s rssi_scan: %-12d chan_scan: %-12d\n", 
                                        ssid,
                                        psk,
                                        -rssi_scan,
                                        chan_scan
                                    );
                                #endif  
                                obj->_credential_sta = wifi_credential_sta_array[C];
                            }
                        }
                        if (known) break;
                    }
                    delay(1);
                }

                if(known) {
                    #ifdef DEBUG_ESP_WIFI
                        DEBUG_WIFI_MULTI(" ---> ");
                    #endif
                    break;
                } else {
                    #ifdef DEBUG_ESP_WIFI
                        DEBUG_WIFI_MULTI("      ");
                    #endif
                }
                #ifdef DEBUG_ESP_WIFI
                    DEBUG_WIFI_MULTI(" %d: [%d][%02X:%02X:%02X:%02X:%02X:%02X] %s (%d) %c\n",
                        n, chan_scan, 
                        BSSID_scan[0], BSSID_scan[1], BSSID_scan[2], BSSID_scan[3], BSSID_scan[4], BSSID_scan[5], 
                        ssid_scan.c_str(), rssi_scan, (sec_scan == ENC_TYPE_NONE) ? ' ' : '*');
                #endif

                delay(1);
                n++;
                if (n>=scanResult){n=0;pass++;}
            }

            // clean up ram
            WiFi.scanDelete();

            #ifdef DEBUG_ESP_WIFI
                DEBUG_WIFI_MULTI("\n\n");
            #endif

            delay(1);

            if(bestNetwork.ssid) {
                #ifdef DEBUG_ESP_WIFI
                    DEBUG_WIFI_MULTI("[WIFI] Connecting BSSID: %02X:%02X:%02X:%02X:%02X:%02X SSID: %s Channel: %d (%d)\n", bestBSSID[0], bestBSSID[1], bestBSSID[2], bestBSSID[3], bestBSSID[4], bestBSSID[5], bestNetwork.ssid, bestChannel, bestNetworkDb);
                #endif
                // WiFi.begin(bestNetwork.ssid, bestNetwork.passphrase, bestChannel, bestBSSID);
                // status = WiFi.status();
                
                // auto startTime = millis();
                // // wait for connection, fail, or timeout
                // while(status != WL_CONNECTED && status != WL_NO_SSID_AVAIL && status != WL_CONNECT_FAILED && (millis() - startTime) <= connectTimeoutMs) {
                //     delay(10);
                //     status = WiFi.status();
                // }
                ESP8266WiFiMulti_tested[ESP8266WiFiMulti_testedCnt] = adri_toolsv2Ptr_get()->ch_toString(bestNetwork.ssid);
                ESP8266WiFiMulti_testedCnt++;
                status = wifiConnect_connect_sta_normal(bestNetwork.ssid, bestNetwork.passphrase, bestChannel, bestBSSID, mod, porgress, status) ;

#ifdef DEBUG_ESP_WIFI
                IPAddress ip;
                uint8_t * mac;
                switch(status) {
                    case WL_CONNECTED:
                        ip = WiFi.localIP();
                        mac = WiFi.BSSID();
                        DEBUG_WIFI_MULTI("[WIFI] Connecting done.\n");
                        DEBUG_WIFI_MULTI("[WIFI] SSID: %s\n", WiFi.SSID().c_str());
                        DEBUG_WIFI_MULTI("[WIFI] IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
                        DEBUG_WIFI_MULTI("[WIFI] MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                        DEBUG_WIFI_MULTI("[WIFI] Channel: %d\n", WiFi.channel());
                        wifi_credential_sta_best.last = WiFi.SSID();
                        wifi_credential_sta_best.best = WiFi.SSID();  
                        break;
                    case WL_NO_SSID_AVAIL:
                        DEBUG_WIFI_MULTI("[WIFI] Connecting Failed AP not found.\n");
                        break;
                    case WL_CONNECT_FAILED:
                        DEBUG_WIFI_MULTI("[WIFI] Connecting Failed.\n");
                        break;
                    default:
                        DEBUG_WIFI_MULTI("[WIFI] Connecting Failed (%d).\n", status);
                        break;
                }
#endif

            } else {
                #ifdef DEBUG_ESP_WIFI
                    DEBUG_WIFI_MULTI("[WIFI] no matching wifi found!\n");
                #endif
            }

            return status;
        }
       
       
        // scan failed, or some other condition not handled above. Start another scan.
        #ifdef DEBUG_ESP_WIFI
            DEBUG_WIFI_MULTI("[WIFI] delete old wifi config...\n");
            DEBUG_WIFI_MULTI("[WIFI] start scan\n");
        #endif

        WiFi.disconnect();

        // scan wifi async mode
        WiFi.scanNetworks(true);
    }
    return status;
}
// #endif


/*
 
                WIFICONNECT_MOD_toString(_statu_connect, result);
                fsprintf("%-15s : %s\n", fsget(awc_str_connect).c_str(),        result.c_str());

                WIFICONNECTSSID_MOD_toString(_statu_connectSSID, result);
                fsprintf("%-15s : %s\n", fsget(awc_str_connectSSID).c_str(),    result.c_str());

                WiFiMode_t_toString(_statu_sta, result);
                fsprintf("%-15s : %s\n", fsget(awc_str_station).c_str(),        result.c_str());
                
                WIWIFICONFIGIP_MOD_toString(_statu_ip, result);
                fsprintf("%-15s : %s\n", fsget(awc_str_ipmod).c_str(),          result.c_str()); 

                WIFI_STATU_toString(_statu_current, result);
                fsprintf("%-15s : %s\n", fsget(awc_str_statu).c_str(),          result.c_str()); 

 */

// int wifi_credential_fromSPIFF(String * ret) { 
//     String filename = "/wifi_id.txt";
//     File f = FILESYSTEM.open(filename,"r");

//     #ifdef DEBUG
//         Serial.printf("\n[wfifi_getID_fromSPIFF]\n");
//     #endif

//     if (!f) {

//         #ifdef DEBUG
//             fsprintf("\tERROR read file: %s\n", filename.c_str()); 
//             fsprintf("[wfifi_getID_fromSPIFF] Done\n");
//         #endif

//         return 0;

//     } else {

//         String line;

//         while (f.available()) {line += char(f.read());}

//         f.close();

//         *ret = line;

//         #ifdef DEBUG
//             fsprintf("\tSUCCES read file: %s\n", filename.c_str());
//             fsprintf("[wfifi_getID_fromSPIFF] Done\n");
//         #endif  

//         return 1;
//     }
// }