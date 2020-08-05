#include <adri_wifiConnect.h>
#include <adri_tools.h>

wifi_credential_ap wifi_credential_AP("reveil");

void _test_p(int p){fsprintf("\n wificonnect: %d\n", p);}
void setup()
{
    Serial.begin(115200);
    delay(3000);
    Serial.println("");
	bool fsOK = SPIFFS.begin();
	// SPIFFS.remove("/"+CREDENTIAL_FILENAME+".txt");
	// SPIFFS.remove("/"+CREDENTIALAP_FILENAME+".txt");
	// 
	wifi_credential_sta_fromSPIFF();
	// wifi_credential_set(0, "freebox_123_EXTBAS", "phcaadax", "192.168.0.93", "255.255.255.0", "192.168.0.254");
	// wifi_credential_set(1, "sd", "phcaadax", "192.168.0.93", "255.255.255.0", "192.168.0.254");
	wifi_credential_set(2, "freebox_123_EXT", "phcaadax", "192.168.0.93", "255.255.255.0", "192.168.0.254");
	wifi_credential_sta_print();

	// fsprintf("test\n");
	// fsprintf("\n%d\n", isValidIp("192.168.1.1", "255.255.255.0", "192.168.0.254" ));
	// fsprintf("\n%d\n", isValidIp("192.168.0.1", "255.255.255.0", "192.168.0.254" ));
	// fsprintf("\n%d\n", isValidIp("192.168.0.255", "255.255.255.0", "192.168.0.254" ));
	// fsprintf("\n%d\n", isValidIp("192.168.0.25", "255.255.255.0", "192.168.0.254" ));

	wifi_credential_AP.load_fromSpiif();
	wifi_credential_AP.psk_set("adrilight1234");
	wifi_credential_AP.print();

	wifiConnect wifiConnect;
	// wifiConnect.progress_set(_test_p);

	fsprintf("\n[wifiConnect Start]\n");
		wifiConnect.load_fromSpiif();
		wifiConnect.credential_sta_pos_set(2);
		wifiConnect.connect_set(AWC_SETUP);
		wifiConnect.connectSSID_set(AWCS_NORMAL);
		wifiConnect.station_set(WIFI_AP_STA);
		// wifiConnect.progress_set(_test_p);
		wifiConnect.print_cfg();
		wifiConnect.setup();

	fsprintf("\n[wifiConnect Start] Done\n");


	wifiConnect.savToSpiff();
	wifi_credential_AP.sav_toSpiff();
	wifi_credential_sta_toSpiff();   

test();
	// WiFi.mode(WIFI_STA);
	// while (true) {
	// 	if (ESP8266WiFiMulti_run(15000) == WL_CONNECTED) break;
	// 	yield();
	// }
        // Serial.printf("\n[wifi_connect_sta] Done: pi: %s - gw: %s - sub: %s - dns0:%s - dns1:%s - dns2:%s\n", 
        //     WiFi.localIP().toString().c_str(), 
        //     WiFi.gatewayIP().toString().c_str(), 
        //     WiFi.subnetMask().toString().c_str(),
        //     WiFi.dnsIP(0).toString().c_str(),
        //     WiFi.dnsIP(1).toString().c_str(),
        //     WiFi.dnsIP(2).toString().c_str()
        //     );
	// wifi_credential_sta_toSpiff();                      

}
void loop()
{

}

int test() {
	int mod = 3;
	switch (mod) {
	    case 2:
	      break;
	    case 3:
	    	fsprintf("\ntest 3\n");
	    	return 3;
	      break;
	}

	fsprintf("\ntest bot\n");
}