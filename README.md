# adri_wifiConnect


<pre>
Librairies
<details>
adri_wifiConnect                = https://github.com/AdriLighting/adri_wifiConnect
adri_tools_v2                   = https://github.com/AdriLighting/adri_tools_v2
adri_timer                      = https://github.com/AdriLighting/adri_timer

esp8266/arduino                 = https://github.com/esp8266/Arduino/tree/master/libraries
</details>
Dependency Graph
<details>
|-- [adri_wifiConnect] 1.0.0
|   |-- [ESP8266WebServer] 1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266mDNS] 1.2
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [adri_tools_v2] 1.0.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [adri_timer] 1.0.0</details>
</pre>

<hr>

#### examples

adri_wifiConnect
<details>
platformio.ini
<details>
<pre>
[env:nodemcuv2]
platform=espressif8266
board=nodemcuv2
framework=arduino
lib_ignore=WiFi101
lib_extra_dirs= ${env.lib_extra_dirs}
[platformio]
src_dir= ${env.src_dir}

</pre>
</details>
<pre>
Librairies
<details>
adri_wifiConnect                = https://github.com/AdriLighting/adri_wifiConnect
adri_tools_v2                   = https://github.com/AdriLighting/adri_tools_v2
adri_timer                      = https://github.com/AdriLighting/adri_timer
adri_espwebserver               = https://github.com/AdriLighting/adri_espwebserver
adri_httparseurl                = https://github.com/AdriLighting/adri_httparseurl

esp8266/arduino                 = https://github.com/esp8266/Arduino/tree/master/libraries

WebSockets                      = https://github.com/Links2004/arduinoWebSockets
ESPAsyncTCP                     = https://github.com/me-no-dev/ESPAsyncTCP
</details>
Dependency Graph
<details>
|-- [adri_wifiConnect] 1.0.0
|   |-- [ESP8266WebServer] 1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266mDNS] 1.2
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [adri_tools_v2] 1.0.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [adri_timer] 1.0.0
|-- [adri_espwebserver] 1.0.0
|   |-- [ArduinoOTA] 1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [ESP8266mDNS] 1.2
|   |   |   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266mDNS] 1.2
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266WiFi] 1.0
|   |-- [adri_tools_v2] 1.0.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [adri_httparseurl] 1.0.0
|   |   |-- [adri_tools_v2] 1.0.0
|   |   |   |-- [ESP8266WiFi] 1.0
|   |   |   |-- [LittleFS(esp8266)] 0.1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [ESP8266WebServer] 1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [WebSockets] 2.2.1
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [ESPAsyncTCP] 1.2.2
|   |   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [Ethernet(esp8266)] 1.0.4
|   |   |   |-- [SPI] 1.0
|   |   |-- [SPI] 1.0
|   |   |-- [Hash] 1.0
|   |-- [Hash] 1.0
|-- [adri_timer] 1.0.0
|-- [adri_tools_v2] 1.0.0
|   |-- [ESP8266WiFi] 1.0
|   |-- [LittleFS(esp8266)] 0.1.0
|-- [adri_httparseurl] 1.0.0
|   |-- [adri_tools_v2] 1.0.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [ESP8266WiFi] 1.0
|   |-- [LittleFS(esp8266)] 0.1.0
|-- [WebSockets] 2.2.1
|   |-- [ESP8266WiFi] 1.0
|   |-- [ESPAsyncTCP] 1.2.2
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [Ethernet(esp8266)] 1.0.4
|   |   |-- [SPI] 1.0
|   |-- [SPI] 1.0
|   |-- [Hash] 1.0
|-- [LittleFS(esp8266)] 0.1.0</details>
</pre>

</details>
<hr>