// Codico adaptado.
// Agredecimentos em especial ao Flavio do chanal Brincando com ideias
// Link para o canal - https://www.youtube.com/channel/UCcGk83PAQ5aGR7IVlD_cBaw 

#include <SoftwareSerial.h>
#include <TinyGPS.h>

SoftwareSerial serialGSM(10, 11); // RX, TX
SoftwareSerial serialGPS(6, 7);

TinyGPS gps;

bool temSMS = false;
String telefoneSMS;
String dataHoraSMS;
String mensagemSMS;
String requisicao;
String comandoGSM = "";
String ultimoGSM = "";

#define senha "3636"

void leGSM();
void leGPS();
void enviaSMS(String telefone, String mensagem);
void configuraGSM();

void setup() {

  Serial.begin(9600);
  serialGPS.begin(9600);   
  serialGSM.begin(9600); 
  
  Serial.println("Sketch Iniciado!");
  configuraGSM();

  leGPS();
}

void loop() {
static unsigned long delayLeGPS = millis();

  if ( (millis() - delayLeGPS) > 10000 ) {
     leGPS();     
     delayLeGPS = millis(); 
  }
  

  leGSM();

  if (comandoGSM != "") {
      Serial.println(comandoGSM);
      ultimoGSM = comandoGSM;
      comandoGSM = "";
  }

  if (temSMS) {

     Serial.println("Chegou Mensagem!!");
     Serial.println();
    
     Serial.print("Remetente: ");  
     Serial.println(telefoneSMS);
     Serial.println();
    
     Serial.print("Data/Hora: ");  
     Serial.println(dataHoraSMS);
     Serial.println();
    
     Serial.println("Mensagem:");  
     Serial.println(mensagemSMS);
     Serial.println();
      
     mensagemSMS.trim();

     String pingAutorizado = "ping:";
     pingAutorizado.concat(senha);

     String getlocationAutorizado = "getlocation:";
     getlocationAutorizado.concat(senha);
     
     if ( mensagemSMS == pingAutorizado ) {
        Serial.println("Enviando SMS de Resposta ao ping.");
        String resposta = "vertrongps\r";
        resposta.concat("requisicao:ping\r");
        resposta.concat("resposta:ok");
        enviaSMS(telefoneSMS, resposta);  
     }

     else if(mensagemSMS == getlocationAutorizado) {

        Serial.println("Enviando SMS de Resposta ao getlocation.");
        
        leGPS();

        // latitude // longitude
        float flat, flon;
        unsigned long age;

        gps.f_get_position(&flat, &flon, &age);

        if ( (flat == TinyGPS::GPS_INVALID_F_ANGLE) || (flon == TinyGPS::GPS_INVALID_F_ANGLE) ) {
           Serial.println("Enviando SMS de Resposta: GPS Sem Sinal.");
           String respostaSinal = "vertrongps\r";
           respostaSinal.concat("requisicao:getlocation\r");
           respostaSinal.concat("resposta:gpsfail");          
           enviaSMS(telefoneSMS, respostaSinal);
        } else {
           String urlMapa = "vertrongps\r";
           urlMapa.concat("requisicao:getlocation\r");
           
           urlMapa.concat("resposta:");
           urlMapa += String(flat,8);
           urlMapa += ":";
           urlMapa += String(flon,8);
           
           enviaSMS(telefoneSMS, urlMapa);
        }  
     }
     temSMS = false;
  }  
}

void leGSM()
{
  static String textoRec = "";
  static unsigned long delay1 = 0;
  static int count=0;  
  static unsigned char buffer[64];

  serialGSM.listen(); 
  if (serialGSM.available()) {            
 
     while(serialGSM.available()) {         
   
        buffer[count++] = serialGSM.read();     
        if(count == 64)break;
     }

     textoRec += (char*)buffer;
     delay1   = millis();
     
     for (int i=0; i<count; i++) {
         buffer[i]=NULL;
     } 
     count = 0;                       
  }


  if ( ((millis() - delay1) > 100) && textoRec != "" ) {

     if ( textoRec.substring(2,7) == "+CMT:" ) {
        temSMS = true;
     }

     if (temSMS) {
            
        telefoneSMS = "";
        dataHoraSMS = "";
        mensagemSMS = "";

        byte linha = 0;  
        byte aspas = 0;
        for (int nL=1; nL < textoRec.length(); nL++) {

            if (textoRec.charAt(nL) == '"') {
               aspas++;
               continue;
            }                        
          
            if ( (linha == 1) && (aspas == 1) ) {
               telefoneSMS += textoRec.charAt(nL);
            }

            if ( (linha == 1) && (aspas == 5) ) {
               dataHoraSMS += textoRec.charAt(nL);
            }

            if ( linha == 2 ) {
               mensagemSMS += textoRec.charAt(nL);
            }

            if (textoRec.substring(nL - 1, nL + 1) == "\r\n") {
               linha++;
            }
        }
     } else {
       comandoGSM = textoRec;
     }
     
     textoRec = "";  
  }     
}

void leGPS() {
unsigned long delayGPS = millis();

   serialGPS.listen();
   bool lido = false;
   while ( (millis() - delayGPS) < 500 ) { 
      while (serialGPS.available()) {
          char cIn = serialGPS.read(); 
          lido = gps.encode(cIn); 
      }

      if (lido) { 
         float flat, flon;
         unsigned long age;
    
         gps.f_get_position(&flat, &flon, &age);
    
         String urlMapa = "Local Identificado: https://maps.google.com/maps/?&z=10&q=";
         urlMapa += String(flat,6);
         urlMapa += ",";
         urlMapa += String(flon,6);
         Serial.println(urlMapa);
         
         break; 
      }
   }   
}

void enviaSMS(String telefone, String mensagem) {
  serialGSM.print("AT+CMGS=\"" + telefone + "\"\n");
  serialGSM.print(mensagem + "\n");
  serialGSM.print((char)26); 
}


void configuraGSM() {
   serialGSM.print("AT+CMGF=1\n;AT+CNMI=2,2,0,0,0\n;ATX4\n;AT+COLP=1\n"); 
}
