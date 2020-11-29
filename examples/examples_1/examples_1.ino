#include <adri_wifiConnect.h>
#include <adri_tools.h>
#include <adri_timer.h>
#include <adri_espwebserver_tools.h>


                 

wifiConnect 		* myWifi;	// PTR pour unr instance statique "wifiConnect"
wifi_credential_ap	* myWifiAp;	// PTR pour unr instance statique "wifi_credential_ap"

const char 			* myWifiHostname = "MY_WIFI"; 	// AP AND DNS HOSTNAME 
													
										// 	AWC_LOOP; 		WIFI CONNECT STARTUP WITH STATIC 
										// 	AWC_SETUP; 		WIFI CONNECT STARTUP WITH STATIC 
WIFICONNECT_MOD		myWifiConnectMod 	= 	AWC_LOOP;	

										// 	AWCS_MULTI;		STA CONNECT WITH MULTIPLE SSID
										// 	AWCS_NORMAL;	STA CONNECT WITH THE SELECTED SSID
WIFICONNECTSSID_MOD myWifiSSIDMod 		= 	AWCS_NORMAL;	

boolean 			myWifiOTA 			= false; 	// ENABLED OTA

int 				myWifiConnectDone	= 0;		// WIFI CONNECT LOOP POSTION 
adri_timer 			* myWifiTimer; 					// WIFI CONNECT LOOP SETUP TIMER 

adri_timer 			* myWifiTimer_print;

void setup() {
	Serial.begin(115200);
	delay(1000);
	fsprintf("\n");

	SPIFFS.begin();

	// Créations des instnce
	myWifi 		= new wifiConnect();
	myWifiAp 	= new wifi_credential_ap("");
	myWifiTimer 		= new adri_timer(3000,	"", false);
	myWifiTimer_print 	= new adri_timer(1000,	"", false);

	// Iinitialisation AP
	myWifiAp->hostname_set(ch_toString(myWifiHostname));
	wifi_credential_ap_register(myWifiAp);

	// Créations des identifiants
	if (!wifi_credential_sta_fromSPIFF()) { 	// initialisation de linstance wifi_credential_sta
		wifi_credential_set(
			1, 						// postion du ssid selectionner (0 to 2)
			"freebox_123_EXTBAS",	// ssid
			"phcaadax", 			// pswd
			"",						// ip 		(vide pour ne pas cofigurer d'ip)
			"",						// subnet 	(vide pour ne pas cofigurer d'subnet)
			""						// gateway 	(vide pour ne pas cofigurer d'gateway)
		);
		wifi_credential_set(
			0, 						
			"freebox_123_EXT", 		
			"phcaadax", 			
			"",						
			"",						
			""						
		);	
		wifi_credential_sta_toSpiff();		
	}
		wifi_credential_set(
			0, 						
			"freebox_123_EXT", 		
			"x", 			
			"",						
			"",						
			""						
		);	
		wifi_credential_sta_toSpiff();		
	wifi_credential_sta_print();


	// configuration "wifiConnect"

	// confiuration identifiants sta 
	myWifi->load_fromSpiif 				();
	myWifi->credential_sta_pos_set 		(0);

	// configuration du lancement de la coonection
	myWifi->connect_set 				(myWifiConnectMod);
	if (myWifiConnectMod == AWC_LOOP) myWifiSSIDMod = AWCS_NORMAL; 	// wifi connect loop ne supporte pas encor de multiple ssid
	myWifi->connectSSID_set 			(myWifiSSIDMod);


	myWifi->station_set 				(WIFI_STA);
	myWifi->hostName_set 				(myWifiHostname); 			// initialisation dns si ota desactiver
	myWifi->setup_id					();							// initialize les id STA

	//
	myWifiAp->psk_set 					("mywifiappsk");						// pswd AP
	myWifiAp->ip_set 					(myWifi->_credential_sta->ip_get());	// ip 	AP
	myWifiAp->print 					();	



	if (myWifiConnectMod == AWC_SETUP) {
		myWifi->setup 						();
		if(!myWifiOTA) 	myWifi->MDSN_begin	();
		else 			arduinoOTA_setup	(myWifiHostname);
		wifi_connect_statu 					();
		fsprintf("\n[myWifiConnectDone] : %s\n", on_time().c_str());
		myWifiConnectDone = 1;		
	}


	if (myWifiConnectMod == AWC_LOOP) {
		myWifiConnectDone  					= 2;
		myWifiTimer->activate 				();
		myWifiTimer_print->activate 		();
	}


}
void loop()
{
	if (myWifiConnectDone == 1) {
		if(!myWifiOTA) 	myWifi->MDSN_loop();
		else 			arduinoOTA_loop();			
	}

	if (myWifiConnectMod == AWC_LOOP) {

		if(myWifiTimer->loop_stop()) {
			myWifi->setupLoop(); 
			fsprintf("\n[connection wifi initialisée]\n");
		}

		if(myWifiTimer_print->loop()) fsprintf("\n[myWifiTimer_print] : %s\n", on_time().c_str());

		myWifi->wifi_loop();	
		if ( (myWifi->wifi_loop_statu()==wifi_statu_sta_isconnected) && (myWifiConnectDone == 2)) {

			myWifiTimer_print->activate(false);

			if(!myWifiOTA) 	myWifi->MDSN_begin	();
			else 			arduinoOTA_setup	(myWifiHostname);

			wifi_connect_statu();

			myWifiConnectDone = 1;

			fsprintf("\n[myWifiConnectDone] : %s\n", on_time().c_str());
		}		
	}
}


