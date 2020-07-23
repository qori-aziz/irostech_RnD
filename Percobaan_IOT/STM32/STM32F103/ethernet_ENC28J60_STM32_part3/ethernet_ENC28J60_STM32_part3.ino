#include <SPI.h>
#include <EtherCard_STM.h>

// Set 1 for Static IP, Set 0 for DHCP IP
#define STATIC 1
#define LedPins1 PB6
#define LedPins2 PB7
#define LedPins3 PB8
#define LedPins4 PB9

#if STATIC 
// Set the static IP address
static byte myip[] = {192,168,1,123};
static byte gwip[] = {192,168,1,1};
static byte dnsip[] = {8,8,8,8};
#endif

// Set mac address to unique value on the network host
static byte mymac[] = {0x54, 0x34, 0x41, 0x30, 0x30, 0x31};

// Set value for receive buffer
byte Ethernet::buffer[900];
BufferFiller bfill;

// Set the number pin for output
//int LedPins[] = {1,2,3,4,5};
boolean tmpPin;
boolean tmpLogin;
boolean PinStatus1;
boolean PinStatus2;
boolean PinStatus3;
boolean PinStatus4;
int stsRelay;
//-------------

const char http_OK[] PROGMEM =
"HTTP/1.0 200 OK\r\n"
"Content-Type: text/html\r\n"
"Pragma: no-cache\r\n\r\n";

const char http_Found[] PROGMEM =
"HTTP/1.0 302 Found\r\n"
"Location: /\r\n\r\n";

const char http_Unauthorized[] PROGMEM =
"HTTP/1.0 401 Unauthorized\r\n"
"Content-Type: text/html\r\n\r\n"
"<h3>401 Page Request Note Found...</h3>";

//------------
const char homePage[] PROGMEM =
    "<input name='button' type=submit value='RELAY 1'><br><br>"
    "<input name='button' type=submit value='RELAY 2'><br><br>"
    "<input name='button' type=submit value='RELAY 3'><br><br>"
    "<input name='button' type=submit value='RELAY 4'><br><br>";       

//------------------------
const char pageStart[] PROGMEM =
"<HTML>"
"<HEAD>"
"<TITLE>Relay Control</TITLE>"
"</HEAD>"
  "<BODY>"
  "<CENTER>"
     "<H1><font color=\"blue\">Control Relay Web Server</font></H1><hr />"
     "<H3><font color=\"red\">MCU STM32F103C ARDUINO IDE<br> For Implementation IoT</font></H3>"
     "<form>";
            
//------------------------            
const char pageEnd[] PROGMEM =
     "</form>"
     "<a href=\"/?MainMenu\"\">Main Menu</a>" " | "
     "<a href=\"/?GetStatus\"\">Get Status</a>" " | "
     "<a href=\"/?LogOut\"\">Log Out</a>" "<br>"
     "Created by Hendrikherdian <br>Blog: stm32projects.blogspot.com" 
     "<br />"
  "</CENTER>"
  "</BODY>"
"</HTML>";

//------------------------
const char pageLogin[] PROGMEM =
"<html><head>"
"<title>Login User</title></head>"
"<body>"
"<center>"
  "<form>"
  "<H3><font color=\"blue\">PLEASE LOGIN FIRST</font></H3><hr />"
  "Enter Username: <input type=text name=username><br><br>"
  "Enter Password: <input type=password name=password><br><br>"
  "<input name= 'login' type='submit' id='login' value='Login' />"
  "<hr/><br>"
  "</form>"
"</body></html>"; 
           
void setup()
{
  //=======================================================
  Serial1.begin(115200);
  if (ether.begin(sizeof Ethernet::buffer, mymac, PB12) == 0); // set pin PA8 to CS
  #if STATIC
  // Set the IP ethernet ENC28J60
  ether.staticSetup(myip, gwip, dnsip);
  #else
  if (!ether.dhcpSetup())
    Serial1.println("DHCP failed");
  #endif
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip); 
  ether.printIp("DNS: ", ether.dnsip);
  tmpLogin = false;

  //Setup Pin arduino as output Procedure
  pinMode(LedPins1,OUTPUT);
  pinMode(LedPins2,OUTPUT);
  pinMode(LedPins3,OUTPUT);
  pinMode(LedPins4,OUTPUT);

  digitalWrite(LedPins1,LOW);
  digitalWrite(LedPins2,LOW);
  digitalWrite(LedPins3,LOW);
  digitalWrite(LedPins4,LOW);
}

// --------------------------------------

void loop()
{
  delay(1); //
  word len = ether.packetReceive();   // check for ethernet packet
  word pos = ether.packetLoop(len);   // check for tcp packet

  if (pos) {
    bfill = ether.tcpOffset();
    // var "data" contain sending data to http request
    char *data = (char *) Ethernet::buffer + pos;
    if (strncmp("GET /", data, 5) != 0) {
      bfill.emit_p(http_Unauthorized);
    }
    else {
      data += 5;
      if (data[0] == ' ') {
      // Insert the main page here or index.html      
        if (tmpLogin==false){
        bfill.emit_p(pageLogin);}
        else {
        bfill.emit_p(pageStart);
        bfill.emit_p(homePage);
        bfill.emit_p(pageEnd);
        }
      }
 //================================================================

      // "16" = totaly string on char "?ArduinoPIN1=on ".
      else if (strncmp("?button=RELAY+1 ", data, 16) == 0) {
        tmpPin = digitalRead(LedPins1);
        if (tmpPin==false){digitalWrite(LedPins1,HIGH);}
        else{digitalWrite(LedPins1,LOW);}     
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?button=RELAY+2 ", data, 16) == 0) {
        tmpPin = digitalRead(LedPins2);
        if (tmpPin==false){digitalWrite(LedPins2,HIGH);}
        else{digitalWrite(LedPins2,LOW);}      
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?button=RELAY+3 ", data, 16) == 0) {
        tmpPin = digitalRead(LedPins3);
        if (tmpPin==false){digitalWrite(LedPins3,HIGH);}
        else{digitalWrite(LedPins3,LOW);}      
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?button=RELAY+4 ", data, 16) == 0) {
        tmpPin = digitalRead(LedPins4);
        if (tmpPin==false){digitalWrite(LedPins4,HIGH);}
        else{digitalWrite(LedPins4,LOW);}     
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?GetStatus ", data, 11) == 0) {
        // Write code here to page link
        PinStatus1=digitalRead (LedPins1);
        PinStatus2=digitalRead (LedPins2);
        PinStatus3=digitalRead (LedPins3);
        PinStatus4=digitalRead (LedPins4); 
       
        bfill.emit_p(pageStart);
        // use macro PSTR, letter $F to define string which print
        bfill.emit_p(PSTR(
        "Lamp 1 : $F<br><br>"
        "Lamp 2 : $F<br><br>"
        "Lamp 3 : $F<br><br>"
        "Lamp 4 : $F<br><br>"),
        PinStatus1?PSTR("<font color=\"red\">ON</font>"):PSTR("OFF"),
        PinStatus2?PSTR("<font color=\"red\">ON</font>"):PSTR("OFF"),
        PinStatus3?PSTR("<font color=\"red\">ON</font>"):PSTR("OFF"),
        PinStatus4?PSTR("<font color=\"red\">ON</font>"):PSTR("OFF"));
        bfill.emit_p(pageEnd); 
    }
      else if (strncmp("?MainMenu ", data, 10) == 0) {
        bfill.emit_p(pageStart);
        bfill.emit_p(homePage);
        bfill.emit_p(pageEnd);
      }
      else if (strncmp("?LogOut ", data, 8) == 0) {
        tmpLogin = false;
        bfill.emit_p(pageLogin);
      }
      else if (strncmp("?username=ypn&password=12345&login=Login ", data, 45) == 0) {
        tmpLogin = true;
        bfill.emit_p(pageStart);
        bfill.emit_p(homePage);
        bfill.emit_p(pageEnd);
      }
      else {
        // Page not found
        bfill.emit_p(pageLogin);
      }
    }
    ether.httpServerReply(bfill.position()); // send http response
  }
}
