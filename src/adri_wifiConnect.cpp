#include "adri_wifiConnect.h"

#include <arduino.h>

#include <adri_tools.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define DEBUG



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

wifi_credential_ap::wifi_credential_ap(String value){
    hostname_set(value);
}

void wifi_credential_ap::hostname_set(String value){

    char buffer[80];
    sprintf(buffer, "%s_", value.c_str());
    build_host_name(buffer);

    _ssid = String(b_host_name);
}
void wifi_credential_ap::psk_set(String value)      { _psk       = value;}
void wifi_credential_ap::ip_set(String value)       { _ip        = value;}
void wifi_credential_ap::subnet_set(String value)   { _subnet    = value;}
String wifi_credential_ap::psk_get()                { return _psk;    }
String wifi_credential_ap::ip_get()                 { return _ip;     }
String wifi_credential_ap::subnet_get()             { return _subnet; }
String wifi_credential_ap::hostname_get()           { return _ssid;   }
void wifi_credential_ap::print(){
    fsprintf("\n[wifi_credential_ap print]\n")
    fsprintf("%-15s : %s\n", fsget(awc_str_ssid).c_str(),   _ssid.c_str());
    fsprintf("%-15s : %s\n", fsget(awc_str_psk).c_str(),    _psk.c_str());
    fsprintf("%-15s : %s\n", fsget(awc_str_ip).c_str(),     _ip.c_str());
    fsprintf("%-15s : %s\n", fsget(awc_str_subnet).c_str(), _subnet.c_str());

}
boolean wifi_credential_ap_fromSPIFF(String & result) {

    String filename = "/" + CREDENTIALAP_FILENAME + ".txt";

    File f=SPIFFS.open(filename,"r");

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
void wifi_credential_ap::load_fromSpiif(){

    String result = "";

    boolean load = wifi_credential_ap_fromSPIFF(result);

    if (!load) return;

    String value = "";

    value = literal_value("psk",    result);
    _psk = value;

    value = literal_value("ip",     result);
    _ip = value;

    value = literal_value("subnet", result);
    _subnet = value;

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



wifi_credential_sta * wifi_credential_sta_array[CREDENTIAL_MAX];

wifi_credential_sta_best wifi_credential_sta_best;
wifi_credential_sta_best::wifi_credential_sta_best(){}


wifiConnect::wifiConnect(){}

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
        case WIFI_SHUTDOWN: result = "WIFI_SHUTDOWN";   break;
        case WIFI_RESUME:   result = "WIFI_RESUME";     break;
        default :           result = "WIFI_STA";        break;
    }
}
void WiFiMode_t_stringToMod(String mod, WiFiMode_t & result){
    if      (mod == "WIFI_STA")     result = WIFI_STA;
    else if (mod == "WIFI_AP_STA")  result = WIFI_AP_STA;
    else                            result = WIFI_STA;
}

String wifiConnect::ip_get()                    { String result; WIWIFICONFIGIP_MOD_toString    (_cfgIp,        result); return result;}
String wifiConnect::connect_get()               { String result; WIFICONNECT_MOD_toString       (_connect,      result); return result;}
String wifiConnect::connectSSID_get()           { String result; WIFICONNECTSSID_MOD_toString   (_connectSSID,  result); return result;}
String wifiConnect::station_get()               { String result; WiFiMode_t_toString            (_station,      result); return result;}
String wifiConnect::credential_sta_pos_get()    { return String(_credential_sta_pos);}

String wifiConnect_get_ip(wifiConnect * obj)             {return obj->ip_get();}
String wifiConnect_get_connect(wifiConnect * obj)        {return obj->connect_get();}
String wifiConnect_get_connectSSID(wifiConnect * obj)    {return obj->connectSSID_get();}
String wifiConnect_get_station(wifiConnect * obj)        {return obj->station_get();}
String wifiConnect_get_credential_sta(wifiConnect * obj) {return obj->credential_sta_pos_get();}

PROGMEM wifiConnect_getValue wifiConnect_getValues [] = {           
    {"ip",              wifiConnect_get_ip},
    {"connect",         wifiConnect_get_connect},
    {"connectSSID",     wifiConnect_get_connectSSID},
    {"station",         wifiConnect_get_station},
    {"credential_sta",  wifiConnect_get_credential_sta},
};
int wifiConnect_getValues_count = ARRAY_SIZE(wifiConnect_getValues);

boolean wifiConnect::savToSpiff(){
    String filename = "/" + WIFICONNECT_FILENAME + ".txt";

    File f=SPIFFS.open(filename,"w");

    if (!f) {
        #ifdef DEBUG
            fsprintf("\n[wifiConnect_toSpiff] Error opening : %S\n", filename.c_str())
        #endif  
        return false;
    }

    char buffer[120];
    String line = "";

    for (int j = 0; j < wifiConnect_getValues_count; ++j) {
        line += literal_item(
                wifiConnect_getValues[j].name, 
                wifiConnect_getValues[j].getValue(this)); 
    }

    f.print(line);

    f.close();  

    return true;        
}
boolean wifiConnect_load_fromSPIFF(String & result) {

    String filename = "/" + WIFICONNECT_FILENAME + ".txt";

    File f=SPIFFS.open(filename,"r");

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
void wifiConnect::load_fromSpiif(){

    String result = "";

    boolean load = wifiConnect_load_fromSPIFF(result);

    if (!load) return;

    String value = "";

    value = literal_value("connect",     result);
    WIFICONNECT_MOD_stringToMod(value, _connect);

    value = literal_value("connectSSID", result);
    WIFICONNECTSSID_MOD_stringToMod(value, _connectSSID);

    value = literal_value("credential_sta", result);
    if (value != "") credential_sta_pos_set(value.toInt());

    value = literal_value("station", result);
    WiFiMode_t_stringToMod(value, _station);   

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

     _statu_sta          = WIFI_AP;

    if (!wifi_credential_AP.setup_id()) _statu_current = wifi_statu_connect_ap_idFail;
    else {
        if (!wifi_credential_AP.setup_ip()) _statu_ip = AWIP_NOIP;
        _statu_current      = wifi_statu_connect_ap_begin;                
        connect_ap();
    }    
}

boolean wifiConnect::setup_id(){
    String sSSID, sPsk;

    _credential_sta = wifi_credential_sta_array[_credential_sta_pos];

    sSSID       = _credential_sta->ssid_get();
    sPsk        = _credential_sta->psk_get();

    if ((sSSID == "") || (sPsk == "")) {
        return false;
    }

    print_sta();

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

    if (!setup_id()) {
        _statu_current = wifi_statu_connect_sta_idFail;
        return false;
    }
    
    if (!setup_ip()) _statu_ip = AWIP_NOIP;

    switch(_statu_connect) {
        case AWC_SETUP:

            _statu_current = wifi_statu_connect_sta_begin;
            connect_sta_normal();

            return true;
        break;
    }

    return false;
}

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
void wifiConnect::setup(){
    
    boolean resultConnect;

    _statu_ip           = _cfgIp;
    _statu_sta          = _station;
    _statu_connect      = _connect;
    _statu_connectSSID  = _connectSSID;
    _statu_current      = wifi_statu_none;

    statuPrint("\n[statuPrint] START\n");

    if (_station == WIFI_AP_STA){
        _statu_connectSSID  = AWCS_AP;
        setup_ap();
        _statu_ip           = _cfgIp;
        _statu_sta          = _station;
        _statu_connect      = _connect;
        _statu_connectSSID  = _connectSSID;
        _statu_current      = wifi_statu_none;
    }   else setup_ap();

    setup_sta_normal();

    WiFiMode_t w_mod = WiFi.getMode(); 
    switch(_statu_current){
        case wifi_statu_connect_sta_begin:
            resultConnect = wifi_connect_result();
            if (resultConnect) {
                _statu_current = wifi_statu_connect_sta_succes;
                if (_statu_ip != AWIP_NOIP) configIp();
            } else {
                _statu_current = wifi_statu_connect_sta_coFail;
            }
        break;   
        case wifi_statu_connect_ap_begin:
            if (w_mod != WIFI_AP) _statu_current = wifi_statu_connect_ap_coFail;
        break;
            
    }

    statuPrint("\n[statuPrint] END\n");

    Serial.printf("%s - %s - %s\n\n", 
        WiFi.localIP().toString().c_str(), 
        WiFi.gatewayIP().toString().c_str(), 
        WiFi.subnetMask().toString().c_str()
    );
}

void wifiConnect::connect_ap(){
    char ssid[80];
    char password[80];
    sprintf(ssid, "%s",     wifi_credential_AP.hostname_get().c_str());
    sprintf(password, "%s", wifi_credential_AP.psk_get().c_str());  

    WiFi.disconnect();
    WiFi.mode(_statu_sta);
    WiFi.softAP(ssid, password);
    delay(500);

    if (_statu_ip == AWIP_IP) {
        IPAddress staticIP; 
        IPAddress subnet(255, 255, 255, 0);
        staticIP = string2ip(wifi_credential_AP.ip_get());
        // staticIP = {192,168,0,1};
        WiFi.softAPConfig(staticIP, staticIP, subnet);
    }

    String ap_ssid   = String(ssid);
    String ap_ip     = ip2string(WiFi.softAPIP());
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
    IPAddress staticIP  = string2ip(sIp);
    IPAddress gateway   = string2ip(sGateway);
    IPAddress subnet    = string2ip(sSubnet);

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

    #ifdef DEBUG
        fsprintf("\n[wifi_connect] ssid: %s - pswd: %s \n", ssid, password);
    #endif  

    int retries = 0;
    int maxRetries = 0;   

    WiFi.disconnect();
    delay(1000);

    WiFi.mode(_statu_sta);   
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
            
            WiFi.mode(_statu_sta);   
            WiFi.begin(ssid, password);

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

        if (maxRetries > 4) break;
    }

    #ifdef DEBUG
        Serial.println("");
        Serial.println("[wifi_connect] Done");
    #endif  
}
boolean wifi_connect_result(){

    if (WiFi.status() == WL_CONNECTED) {
        #ifdef DEBUG
            Serial.printf("\n[wifi_connect_result] WL_CONNECTED: %s - %s - %s\n\n", 
                WiFi.localIP().toString().c_str(), 
                WiFi.gatewayIP().toString().c_str(), 
                WiFi.subnetMask().toString().c_str());
        #endif
        return true;
    } else {
        #ifdef DEBUG
            Serial.printf("\n[wifi_connect_result] Fail\n\n");
        #endif
        return false;
    }

}




String wifi_credential_sta_get_ssid(wifi_credential_sta * obj)     {return obj->ssid_get();}
String wifi_credential_sta_get_psk(wifi_credential_sta * obj)      {return obj->psk_get();}
String wifi_credential_sta_get_ip(wifi_credential_sta * obj)       {return obj->ip_get();}
String wifi_credential_sta_get_subnet(wifi_credential_sta * obj)   {return obj->subnet_get();}
String wifi_credential_sta_get_gateway(wifi_credential_sta * obj)  {return obj->gateway_get();}

PROGMEM wifi_credential_sta_getValue wifi_credential_sta_getValues [] = {           
    {"ssid",        wifi_credential_sta_get_ssid},
    {"psk",         wifi_credential_sta_get_psk},
    {"ip",          wifi_credential_sta_get_ip},
    {"subnet",      wifi_credential_sta_get_subnet},
    {"gateway",     wifi_credential_sta_get_gateway},
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

    File f=SPIFFS.open(filename,"r");
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

void wifi_credential_sta_fromSPIFF() {
    char buffer_1[255];

    String  cr_array[CREDENTIAL_MAX+1];

    boolean cr_fromSpiff = wifi_credential_sta_fromSPIFF(cr_array);
    if (!cr_fromSpiff) {
        for (int i = 0; i < CREDENTIAL_MAX; ++i) {
            wifi_credential_sta_array[i] = new wifi_credential_sta();
            wifi_credential_set(i, "", "", "", "", "");
        }  
        wifi_credential_sta_best.last = "";
        wifi_credential_sta_best.best = "";
        return;
    }

    wifi_credential_sta_best.last = literal_value("last", cr_array[CREDENTIAL_MAX]);
    wifi_credential_sta_best.best = literal_value("best", cr_array[CREDENTIAL_MAX]);

    for (int i = 0; i < CREDENTIAL_MAX; ++i) {
        wifi_credential_sta_array[i] = new wifi_credential_sta();

        // fsprintf("#%d - %s\n",i, cr_array[i].c_str());
        wifi_credential_set(i, 
            literal_value("ssid",       cr_array[i]).c_str(), 
            literal_value("psk",        cr_array[i]).c_str(), 
            literal_value("ip",         cr_array[i]).c_str(), 
            literal_value("subnet",     cr_array[i]).c_str(), 
            literal_value("gateway",    cr_array[i]).c_str());
    }  


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

    File f=SPIFFS.open(filename,"w");

    if (!f) {
        #ifdef DEBUG
            fsprintf("\n[wifi_credential_sta_toSpiff] Error opening : %S\n", filename.c_str())
        #endif  
        return false;
    }

    char buffer[120];
    String line = "";
    for (int i = 0; i < CREDENTIAL_MAX; ++i) {
        for (int j = 0; j < wifi_credential_sta_getValues_count; ++j) {
            line += literal_item(
                    wifi_credential_sta_getValues[j].name, 
                    wifi_credential_sta_getValues[j].getValue(wifi_credential_sta_array[i])); 
        }
        line += "\n";
    }

    line += literal_item("last", wifi_credential_sta_best.last);
    line += literal_item("best", wifi_credential_sta_best.best);
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

    File f=SPIFFS.open(filename,"w");

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

    line += literal_item("psk",     _psk);
    line += literal_item("ip",      _ip);
    line += literal_item("subnet",  _subnet);

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

    String s_ip = ip2string(ip);

    char buffer[80];
    sprintf(buffer, "%s", s_ip.c_str());

    if (!isValidIp(buffer)) return false;

    return true;
}
boolean isValidIp(String sIp, String sSubnet, String sGateway  ){

    IPAddress ip        = string2ip(sIp);
    IPAddress gateway   = string2ip(sGateway);
    IPAddress subnet    = string2ip(sSubnet);

    fsprintf("\n[isValidIp] ");
    fsprintf("ip: %s ",         ip2string(ip).c_str());
    fsprintf("sGateway: %s ",   ip2string(gateway).c_str());
    fsprintf("sSubnet: %s ",    ip2string(subnet).c_str());

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

    fsprintf(" -> result: %d\n", ret);

    return ret;
}


#define DEBUG_WIFI_MULTI(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ##__VA_ARGS__ )
#define DEBUG_ESP_WIFI

struct WifiAPEntry {
    char *  ssid;
    char *  passphrase;
};

// typedef std::vector<WifiAPEntry> WifiAPlist;

wl_status_t ESP8266WiFiMulti_run(uint32_t connectTimeoutMs) {
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
            DEBUG_WIFI_MULTI("[WIFI] scan done\n");
            DEBUG_WIFI_MULTI("[WIFI] no networks found\n");
            WiFi.scanDelete();
            DEBUG_WIFI_MULTI("\n\n");
            delay(0);
            WiFi.disconnect();
            DEBUG_WIFI_MULTI("[WIFI] start scan\n");
            // scan wifi async mode
            WiFi.scanNetworks(true);
            return status;
        } 

        if(scanResult > 0) {
            // scan done, analyze
            WifiAPEntry bestNetwork { NULL, NULL };
            int bestNetworkDb = INT_MIN;
            uint8 bestBSSID[6];
            int32_t bestChannel;

            DEBUG_WIFI_MULTI("[WIFI] scan done\n");
            delay(0);

            DEBUG_WIFI_MULTI("[WIFI] %d networks found\n", scanResult);
            for(int8_t i = 0; i < scanResult; ++i) {

                String ssid_scan;
                int32_t rssi_scan;
                uint8_t sec_scan;
                uint8_t* BSSID_scan;
                int32_t chan_scan;
                bool hidden_scan;

                WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);

                bool known = false;
                for(auto entry : wifi_credential_sta_array) {
                    sprintf(ssid, "%s", entry->ssid_get().c_str());
                    sprintf(psk, "%s",  entry->psk_get().c_str());                     
                    if(ssid_scan == ssid) { // SSID match
                        DEBUG_WIFI_MULTI("[SSID match] ssid:%s psk: %s\n", entry->ssid_get().c_str(),entry->psk_get().c_str());
                        known = true;
                        if(rssi_scan > bestNetworkDb) { // best network
                            if(sec_scan == ENC_TYPE_NONE || psk) { // check for passphrase if not open wlan
                                bestNetworkDb           = rssi_scan;
                                bestChannel             = chan_scan;
                                bestNetwork.ssid        = ssid;
                                bestNetwork.passphrase  = psk;
                                memcpy((void*) &bestBSSID, (void*) BSSID_scan, sizeof(bestBSSID));
                            }
                        }
                        break;
                    }
                }

                if(known) {
                    DEBUG_WIFI_MULTI(" ---> ");
                } else {
                    DEBUG_WIFI_MULTI("      ");
                }

                DEBUG_WIFI_MULTI(" %d: [%d][%02X:%02X:%02X:%02X:%02X:%02X] %s (%d) %c\n",
                    i, chan_scan, 
                    BSSID_scan[0], BSSID_scan[1], BSSID_scan[2], BSSID_scan[3], BSSID_scan[4], BSSID_scan[5], 
                    ssid_scan.c_str(), rssi_scan, (sec_scan == ENC_TYPE_NONE) ? ' ' : '*');
                delay(0);

                if(known) break;
            }

            // clean up ram
            WiFi.scanDelete();

            DEBUG_WIFI_MULTI("\n\n");
            delay(0);

            if(bestNetwork.ssid) {
                DEBUG_WIFI_MULTI("[WIFI] Connecting BSSID: %02X:%02X:%02X:%02X:%02X:%02X SSID: %s Channel: %d (%d)\n", bestBSSID[0], bestBSSID[1], bestBSSID[2], bestBSSID[3], bestBSSID[4], bestBSSID[5], bestNetwork.ssid, bestChannel, bestNetworkDb);

                WiFi.begin(bestNetwork.ssid, bestNetwork.passphrase, bestChannel, bestBSSID);
                status = WiFi.status();
                
                auto startTime = millis();
                // wait for connection, fail, or timeout
                while(status != WL_CONNECTED && status != WL_NO_SSID_AVAIL && status != WL_CONNECT_FAILED && (millis() - startTime) <= connectTimeoutMs) {
                    delay(10);
                    status = WiFi.status();
                }
                
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
                DEBUG_WIFI_MULTI("[WIFI] no matching wifi found!\n");
            }

            return status;
        }
       
       
        // scan failed, or some other condition not handled above. Start another scan.
        DEBUG_WIFI_MULTI("[WIFI] delete old wifi config...\n");
        WiFi.disconnect();

        DEBUG_WIFI_MULTI("[WIFI] start scan\n");
        // scan wifi async mode
        WiFi.scanNetworks(true);
    }
    return status;
}






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
//     File f = SPIFFS.open(filename,"r");

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