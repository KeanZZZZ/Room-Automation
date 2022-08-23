#ifndef IOT_SET
#define IOT_SET

#include <Arduino.h>
#include <string>

#define ALEN(a) ((int) (sizeof(a) / sizeof(a[0]))) 

extern const char *boiler[];

typedef struct { int position; const char *replacement; } replacement_t;
void get_Html(String& html, const char *[], int, replacement_t [], int);

#define getHtml(strout, boiler, repls) get_Html(strout, boiler, ALEN(boiler), repls, ALEN(repls));


// MAC address 
extern char MAC_ADDRESS[];
void getMAC(char *);

#include <WiFi.h>
#include <WebServer.h>

extern String apSSID;           // SSID of the AP
extern WebServer webServer;     // a simple web server

void startAP();
void printIPs();
void startWebServer();
void handleNotFound();
void handleRoot();
void handleHello();

void initWebServer();
void hndlNotFound();
void hndlRoot();
void hndlWifi();
void hndlWifichz();
void hndlStatus();
void apListForm(String& f);
String ip2str(IPAddress address);


#endif
