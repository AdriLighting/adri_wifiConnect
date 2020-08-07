#include <adri_wifiConnect.h>
#include <adri_tools.h>
#include <adri_espwebserver.h>
#include <adri_espwebserver_tools.h>
#include <adri_udp.h>


const char * hostname = "reveil";

wifi_credential_ap 	wifi_credential_AP(hostname);
adri_webserver 		clientServer(80); 
adri_socket 		socketServer(81); 

void _test_p(int p){fsprintf("\n wificonnect: %d\n", p);}

void setup()
{
    Serial.begin(115200);
    delay(3000);
    Serial.println("");

	bool fsOK = SPIFFS.begin();

	// SPIFFS.remove("/"+CREDENTIAL_FILENAME+".txt");
	// SPIFFS.remove("/"+CREDENTIALAP_FILENAME+".txt");
	
	wifi_credential_sta_fromSPIFF();
	// wifi_credential_set(0, "freebox_123_EXTBAS", "phcaadax", "192.168.0.93", "255.255.255.0", "192.168.0.254");
	// wifi_credential_set(1, "sd", "phcaadax", "192.168.0.93", "255.255.255.0", "192.168.0.254");
	// wifi_credential_set(2, "freebox_123_EXT", "phcaadax", "192.168.0.93", "255.255.255.0", "192.168.0.254");
	wifi_credential_sta_print();


	wifi_credential_AP.load_fromSpiif();
	// wifi_credential_AP.psk_set("adrilight1234");
	wifi_credential_AP.print();

	wifiConnect wifiConnect;
	wifiConnect.load_fromSpiif();

	fsprintf("\n[wifiConnect Start]\n");
		wifiConnect.credential_sta_pos_set(2);
		wifiConnect.connect_set(AWC_SETUP);
		wifiConnect.connectSSID_set(AWCS_NORMAL);
		wifiConnect.station_set(WIFI_AP_STA);
		wifiConnect.hostName_set(hostname);
		// wifiConnect.progress_set(_test_p);
		wifiConnect.print_cfg();
		wifiConnect.setup();
	fsprintf("\n[wifiConnect Start] Done\n");

	wifiConnect.savToSpiff();
	wifi_credential_AP.sav_toSpiff();
	wifi_credential_sta_toSpiff();   


 	#ifdef ADRI_WEBSERVER_REPONSE_H
	int pos = requestReponse_initialize("/hello");
	requestReponse_protocol 	(pos, requestProtocol_http);
	requestReponse_mod_set 		(pos, requestType_name);
	requestReponse_reponseMod 	(pos, requestReponseType_none);
	requestReponse_parseMod 	(pos, requestParseCmd_fromFunc);
	requestReponse_func 		(pos, cmd_test2);
	#endif
	
	clientServer.filesystem_ok(fsOK);
	if (fsOK) clientServer.filesystem_set(&SPIFFS);

	arduinoOTA_setup(hostname);

	clientServer.initialize(80);
	socketServer.setup();

// Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());
// Serial.printf("\n[wifi_connect_sta] Done: pi: %s - gw: %s - sub: %s - dns0:%s - dns1:%s - dns2:%s\n", 
//     WiFi.localIP().toString().c_str(), 
//     WiFi.gatewayIP().toString().c_str(), 
//     WiFi.subnetMask().toString().c_str(),
//     WiFi.dnsIP(0).toString().c_str(),
//     WiFi.dnsIP(1).toString().c_str(),
//     WiFi.dnsIP(2).toString().c_str()
//     );

}
void loop()
{
	clientServer.handleLoop();
	socketServer.loop();
	arduinoOTA_loop();	
}
void cmd_test2(){
	Serial.println("cmd_test2");
	clientServer.replyOKWithMsg("HELL WORLD");
}

