// #include <ESP8266WiFi.h>

#ifndef ADRI_WIFICONNECT_H
    #define ADRI_WIFICONNECT_H

    #if defined(ARDUINO_ARCH_ESP32)
        #include <WiFi.h>
    #elif defined(ARDUINO_ARCH_ESP8266)
        #include <ESP8266WebServer.h>
        #include <ESP8266WiFi.h>
        #include <ESP8266mDNS.h>
        #include <ESP8266WiFiType.h>
    #endif

    #include <Arduino.h>

    #define CREDENTIAL_MAX 3
    #define CREDENTIALAp_MAX 1
    #define CREDENTIAL_FILENAME String("wifi_credential")
    #define CREDENTIALAP_FILENAME String("wifi_credentialAp") 
    #define WIFICONNECT_FILENAME String("wifi_connect") 

    
    /**
     * @class   wifi_credential_ap
     *
     * @brief   mange wifi ifentifiant.
     *
     * @author  Adri
     * @date    18/08/2020
     */
    class wifi_credential_ap
    {
        String _ssid;
        String _psk     = "adrilight1234";
        String _ip      = "192.168.0.93";
        String _subnet  = "255.255.255.0";       
    public:

        wifi_credential_ap(const char * value);

        void hostname_set   (String value);
        void psk_set        (String value);
        void ip_set         (String value);
        void subnet_set     (String value);
        String hostname_get ();
        String psk_get      ();
        String ip_get       ();
        String subnet_get   ();        
        void print          ();        
        boolean load_fromSpiif ();        
        void sav_toSpiff    ();    
        boolean setup_id    ();
        boolean setup_ip    ();

    };
    // extern wifi_credential_ap wifi_credential_AP;
    void wifi_credential_ap_register(wifi_credential_ap * ptr);
    wifi_credential_ap * wifi_credentialAp_ptr_get();

    class wifi_credential_sta
    {
        String _ssid;
        String _psk;
        String _ip;
        String _subnet;
        String _gateway; 
    public:

        wifi_credential_sta();
        ~wifi_credential_sta(){};
        
        void ssid_set       (String value);
        void psk_set        (String value);
        void ip_set         (String value);
        void subnet_set     (String value);
        void gateway_set    (String value);
         
        String ssid_get();
        String psk_get();
        String ip_get();
        String subnet_get();
        String gateway_get(); 

        void print();      
    };

    struct wifi_credential_sta_getValue {
        char *  name;
        String  (*getValue)(wifi_credential_sta * obj);
    };

    void                    wifi_credential_set(int pos, String ssid, String psk, String ip, String subnet, String gateway) ;
    boolean                 wifi_credential_sta_fromSPIFF();
    boolean                 wifi_credential_sta_toSpiff();
    void                    wifi_credential_sta_print();
    wifi_credential_sta *   wifi_credential_get(int pos);



    class wifi_credential_sta_best
    {

    public:
        String last;
        String best;      
        String ip;      
        String psk;      
          
        wifi_credential_sta_best();
        ~wifi_credential_sta_best(){};
    };






    enum WIFICONNECT_MOD { 
        AWC_LOOP, 
        AWC_SETUP
    };   
    enum WIFICONNECTSSID_MOD { 
        AWCS_MULTI, 
        AWCS_AP, 
        AWCS_NORMAL,
        AWCS_FAIL
    };                 
    enum WIFICONFIGIP_MOD { 
        AWIP_IP, 
        AWIP_NOIP, 
    };    
    enum WIFI_STATU { 
        wifi_statu_none, 

        wifi_statu_connect_ap_begin, 
        wifi_statu_connect_ap_succes, 
        wifi_statu_connect_ap_idFail, 
        wifi_statu_connect_ap_coFail, 

        wifi_statu_connect_sta_begin, 
        wifi_statu_connect_sta_succes, 
        wifi_statu_connect_sta_coFail, 
        wifi_statu_connect_sta_idFail, 

        wifi_statu_sta_begin, 
        wifi_statu_sta_search, 
        wifi_statu_sta_isconnected, 
        wifi_statu_sta_fail
    };

    typedef void (*wifiConnect_progress)(int x);

    extern wifi_credential_sta * wifi_credential_sta_array[];
    class wifiConnect
    {
        WIFICONFIGIP_MOD        _cfgIp          = AWIP_IP;
        WiFiMode_t              _station        = WIFI_STA;
        int                     _credential_sta_pos = 0;

        WIFICONFIGIP_MOD        _statu_ip           = _cfgIp;
        WIFICONNECT_MOD         _statu_connect      = _connect;
        WIFICONNECTSSID_MOD     _statu_connectSSID  = _connectSSID;
        WIFI_STATU              _statu_current      = wifi_statu_none;
        
        const char *            _hostName   = "";
        char                    _DNSname[80];
    public:
        wifi_credential_sta *   _credential_sta;
        WiFiMode_t              _statu_sta          = _station;
        wifiConnect_progress    _porgress       = NULL;
        WIFICONNECTSSID_MOD     _connectSSID    = AWCS_MULTI;
        WIFICONNECT_MOD         _connect        = AWC_SETUP; 

        wifiConnect();
        ~wifiConnect(){};

        

        void MDSN_begin();
        void MDSN_loop();
        
        void wifi_loop();
        WIFI_STATU wifi_loop_statu();


        void hostName_set       (const char * mod);
        void connect_set        (WIFICONNECT_MOD mod);
        void connectSSID_set    (WIFICONNECTSSID_MOD mod);
        void station_set        (WiFiMode_t mod);
        void progress_set       (wifiConnect_progress mod);
        void credential_sta_set (wifi_credential_sta * mod);
        void credential_sta_pos_set (int mod);

        String dnsName_get();
        String hostName_get();
        String ip_get();
        String connect_get();
        String connectSSID_get();
        String station_get();
        String credential_sta_pos_get();

        String statuConnect();
        String statuConnectSSID();
        String statSta();
        String statuIpMod();
        String statuStatu();
        String espStationMod_get();
        String currentSSID_get();
        String currentIp_get();
        String currentPsk_get();
        String staPsk_get();
        String staSsid_get();
        IPAddress _currentIp_get();
        
        void print_cfg();
        void print_sta();

        void connect_ap();
        boolean setup_ap();

        void setupLoop();
        void setup();
        
        void stationIsSTA(boolean & result);
        void statuPrint(String header);

        void connect_sta_normal();
        void configIp();
        boolean setup_sta_normal();

        boolean loop_sta_multi();
        boolean setup_sta_multi();
        boolean WIFImulti_setup_id();

        boolean setup_id();
        boolean setup_ip();

        boolean savToSpiff();
        boolean load_fromSpiif();
    };

    struct wifiConnect_getValue {
        char *  name;
        String  (*getValue)(wifiConnect * obj);
    };
    extern int wifiConnect_getValues_count;
    extern PROGMEM wifiConnect_getValue wifiConnect_getValues [];
    wifiConnect * wifiConnect_instance();
    boolean wifi_connect_result(String debug = "");
    boolean isValidIp(String ip);
    boolean isValidIp(const char * string);
    boolean isValidIp(String sIp, String sSubnet, String sGateway  );
    wl_status_t ESP8266WiFiMulti_run(wifiConnect * obj,  WiFiMode_t mod, wifiConnect_progress * _porgress);    
    // wl_status_t ESP8266WiFiMulti_run(uint32_t connectTimeoutMs);
    void wifiConnect_connect_sta_normal(char * ssid, char * password, WiFiMode_t mod, wifiConnect_progress * _porgress);
    void wifi_connect_statu();
    void WIFICONNECTSSID_MOD_toString(WIFICONNECTSSID_MOD mod, String & result);
    void WIFICONNECT_MOD_toString(WIFICONNECT_MOD mod, String & result);
    boolean wifiConnect_load_fromSPIFF(String & result);
#endif

