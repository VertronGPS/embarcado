#include "SIM900.h"
#include <SoftwareSerial.h>
#include <TinyGPS.h>

#include "sms.h"
SMSGSM sms;

SoftwareSerial serial1(5, 6); // RX, TX
TinyGPS gps1;

//Simple sketch to send and receive SMS.
  
int numdata;
boolean started=false;
char smsbuffer[160];
char n[20];
  
//debug begin
char sms_position;
char phone_number[20]; // array for the phone number string
char sms_text[100];
int i;
//debug end

void setup() {
   serial1.begin(9600);
   Serial.begin(9600);


   Serial.println("Inciciando shield GSM/GPRS...");

   Serial.println("GSM Shield testing.");
      //Start configuration of shield with baudrate.
      //For http uses is raccomanded to use 4800 or slower.
      if (gsm.begin(9600)) 
      {
          Serial.println("\nstatus=READY");
          started=true;
      } 
      else 
          Serial.println("\nstatus=IDLE");
  
      if(started) 
      {
          //Enable this two lines if you want to send an SMS.
          if (sms.SendSMS("+5587996681305", "Vertron GPS: SMS com Arduino+SIM900 enviado com sucesso!")) {
            Serial.println("\nSMS sent OK");
          }
         
         //if NO SPACE ,you need delte SMS  from position 1 to position 20
         //please enable this four lines
         //for(i=1;i<=20;i++)
         //{
             //sms.DeleteSMS(i);
         //}

        Serial.println("Shield GSM/GPRS iniciada com sucesso...");
         
      }
   
   Serial.println("Aguardando o sinal dos satelites...");
}

void loop() {
  bool recebido = false;

  Serial.println("Tentando receber sinal: ");
  
  while (serial1.available()) {
     char cIn = serial1.read();
     recebido = gps1.encode(cIn);
  }

  if (recebido) {
     Serial.println("----------------------------------------");
     
     //Latitude e Longitude
     long latitude, longitude;
     unsigned long idadeInfo;
     gps1.get_position(&latitude, &longitude, &idadeInfo);     

     if (latitude != TinyGPS::GPS_INVALID_F_ANGLE) {
        Serial.print("Latitude: ");
        Serial.println(float(latitude) / 1000000, 6);
     }

     if (longitude != TinyGPS::GPS_INVALID_F_ANGLE) {
        Serial.print("Longitude: ");
        Serial.println(float(longitude) / 1000000, 6);
     }

     if (idadeInfo != TinyGPS::GPS_INVALID_AGE) {
        Serial.print("Idade da Informacao (ms): ");
        Serial.println(idadeInfo);
     }


     //Dia e Hora
     int ano;
     byte mes, dia, hora, minuto, segundo, centesimo;
     gps1.crack_datetime(&ano, &mes, &dia, &hora, &minuto, &segundo, &centesimo, &idadeInfo);

     Serial.print("Data (GMT): ");
     Serial.print(dia);
     Serial.print("/");
     Serial.print(mes);
     Serial.print("/");
     Serial.println(ano);

     Serial.print("Horario (GMT): ");
     Serial.print(hora);
     Serial.print(":");
     Serial.print(minuto);
     Serial.print(":");
     Serial.print(segundo);
     Serial.print(":");
     Serial.println(centesimo);


     //altitude
     float altitudeGPS;
     altitudeGPS = gps1.f_altitude();

     if ((altitudeGPS != TinyGPS::GPS_INVALID_ALTITUDE) && (altitudeGPS != 1000000)) {
        Serial.print("Altitude (cm): ");
        Serial.println(altitudeGPS);
     }


     //velocidade
     float velocidade;
     //velocidade = gps1.speed();        //nós
     velocidade = gps1.f_speed_kmph();   //km/h
     //velocidade = gps1.f_speed_mph();  //milha/h
     //velocidade = gps1.f_speed_mps();  //milha/segundo

     Serial.print("Velocidade (km/h): ");
     Serial.println(velocidade, 2);  //Conversão de Nós para Km/h



     //sentito (em centesima de graus)
     unsigned long sentido;
     sentido = gps1.course();

     Serial.print("Sentido (grau): ");
     Serial.println(float(sentido) / 100, 2);


     //satelites e precisão
     unsigned short satelites;
     unsigned long precisao;
     satelites = gps1.satellites();
     precisao =  gps1.hdop();

     if (satelites != TinyGPS::GPS_INVALID_SATELLITES) {
        Serial.print("Satelites: ");
        Serial.println(satelites);
     }

     if (precisao != TinyGPS::GPS_INVALID_HDOP) {
        Serial.print("Precisao (centesimos de segundo): ");
        Serial.println(precisao);
     }

     //float distancia_entre;
     //distancia_entre = gps1.distance_between(lat1, long1, lat2, long2);

     //float sentido_para;
     //sentido_para = gps1.course_to(lat1, long1, lat2, long2);
     
  }
}
