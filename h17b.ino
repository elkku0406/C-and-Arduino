//Author: Elias Hattukangas


//Kirjastot
#include <SPI.h>
#include <WiFiNINA.h>

//Sensori
#define sensorPin A0

char ssid[] = "linksys";			//SSID nimi
char pass[] = "";					//Mahdollinen salasana
int keyIndex = 0;					//Indeksin avaimen parametri
int adc_key_in = 0;					//adc_key_in paramentri

int status = WL_IDLE_STATUS;		//Status
WiFiServer server(80);				//Serverin aika
int admuunnos10 = 0;				//AD-muunnos;
float ilmankosteus_float = 0;		//Ilmankosteuden float
unsigned char ilmankosteus = 0;		//Ilmankosteuden parametri

//Laskee ilmankosteuden
void laske_kosteus(void){
  adc_key_in = analogRead(0);									//Lukee digitaalisia sensoreita
  ilmankosteus_float = (adc_key_in - 0.16*1023)/(0.0062*1023);	//Ilmankosteuden laskukaava
  ilmankosteus = ilmankosteus_float;							//Asettaa ilmankosteuden arvon
  //Jos ilmankosteuden arvo on enemm�n kuin 100% se on silloin 100%
  if(ilmankosteus >= 100){
    ilmankosteus = 100;
  }
}

void setup() {
  Serial.begin(9600);												//Mit� USB:ta luetaan
  //Yritet��n kirjautua networkkiin
  while (status != WL_CONNECTED) {									
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  server.begin();													//K�ynnist�� serverin

    
  Serial.print("SSID: ");										
  Serial.println(WiFi.SSID());										//Kirjoittaa SSID:n
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);												//Kirjoittaa ip-osoitteen
  
}

void loop() {
  
  laske_kosteus();													//Tee laske_kosteus funktio
  int reading = analogRead(sensorPin);								//Lukee analogista sensoria

  WiFiClient client = server.available();							//Clientti
  if (client) {														//Jos clientill� on arvoa
    Serial.println("new client");           
    String currentLine = "";                
    while (client.connected()) {									//Kun clientti on yhdistynyt
      if (client.available()) {										//Jos clientti on vapaana
        char c = client.read();										//C:n arvo = clientin luku
        Serial.write(c);											//Kirjoittaa parametri c_n
        if (c == '\n') {											//Jos c = '\n'

          if (currentLine.length() == 0) {							//Jos currentLinen pituus on 0
			
            client.print("Ilmankosteus: ");							//Kirjoittaa Ilmankosteus:
            client.print(ilmankosteus);							//Kirjoittaa ilmankosteuden arvon
            client.print("%");										//Kirjoittaa % symbolin
            break;
          } else {													//Muuten
            currentLine = "";										//currentLine on tyhj�
          }
        } else if (c != '\r') {										//Jos arvo on '\r'
          currentLine += c;											//currentLinen arvo saa yhden c:n parametrin arvon lis��
        }
      }
    }

    client.stop();													//Pys�ytt�� nykyisen clientin
    Serial.println("client disonnected");							//Kirjoittaa disconnected
  }
}
