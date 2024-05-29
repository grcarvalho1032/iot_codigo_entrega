#include <EspMQTTClient.h>
#include "HX711.h"

HX711 scale;

//Definicoes e constantes
char SSIDName[] = "Wokwi-GUEST"; //nome da rede WiFi
char SSIDPass[] = ""; //senha da rede WiFI

char BrokerURL[] = "broker.hivemq.com"; //URL do broker MQTT
char BrokerUserName[] = ""; //nome do usuario para autenticar no broker MQTT
char BrokerPassword[] = ""; //senha para autenticar no broker MQTT
char MQTTClientName[] = ""; //nome do cliente MQTT
int BrokerPort = 1883; //porta do broker MQTT

String TopicoPrefixo = "projetoiot"; //prefixo do topico
String Topico_01 = TopicoPrefixo+"/QntRacao"; //nome do topico 01
String Topico_02 = TopicoPrefixo+"/PoteRacao"; //nome do topico 02

EspMQTTClient clienteMQTT(SSIDName, SSIDPass, BrokerURL, BrokerUserName, BrokerPassword, MQTTClientName, BrokerPort); //inicializa o cliente MQTT

//Este prototipo de funcao deve estar sempre presente
void onConnectionEstablished() {
}

int val = 0;
float poteracao = 250; //Peso do pote com ração
int pinoPIR = 15; // O pino do sensor de movimento PIR é definido como GPIO15

void setup() {
  Serial.begin(9600); // Configura a taxa de transmissão para 9600

  scale.begin(18, 19);

  clienteMQTT.enableDebuggingMessages();

  pinMode(pinoPIR, INPUT); // Configura o sensor como modo de entrada

  scale.set_scale(2.f);
}

void loop() {

  Serial.println("Loop iniciado!");
  Serial.println(" ");

  clienteMQTT.loop();
  
  if (clienteMQTT.isWifiConnected() == 1) {
    Serial.println("Conectado ao WiFi!");
  } else {
    Serial.println("Nao conectado ao WiFi!");
  }

  if (clienteMQTT.isMqttConnected() == 1) {
    Serial.println("Conectado ao broker MQTT!");
  } else {
    Serial.println("Nao conectado ao broker MQTT!");
  }
 
  val = random(2); //val = digitalRead(pinoPIR); // Lê o valor do sensor
  if (val == 1) { // Há movimento próximo, saída de nível alto

    if (poteracao >= 30){
      float qntcomeu = (random(31, 50)); // Simulando a quantidade que o animal comeu
      if(poteracao < qntcomeu){ // Se o gato raspou o pote
        Serial.println(" "); 
        Serial.print("O animal comeu ");
        Serial.print(poteracao);
        Serial.println("g e o pote ficou vazio!"); // Imprimindo o valor do peso no monitor serial
        clienteMQTT.publish(Topico_01, String(poteracao) + "g");
        poteracao = 0;
        Serial.println("Hora de reabastecer!");
        clienteMQTT.publish(Topico_02, String(poteracao) + "g");
        poteracao = poteracao + 200;
        Serial.println("O pote foi reabastecido com mais 200g de ração!");
        Serial.println(" ");
      }else{
        poteracao = poteracao - qntcomeu;
        Serial.println(" "); 
        Serial.print("O animal comeu: ");
        Serial.print(qntcomeu);
        Serial.println("g"); // Imprimindo o valor do peso no monitor serial
        clienteMQTT.publish(Topico_01, String(qntcomeu) + "g");
        Serial.println(" ");
      }
    }else{
      Serial.println(" "); 
      Serial.println("Hora de reabastecer!");
      clienteMQTT.publish(Topico_02, String(poteracao) + "g");
      poteracao = poteracao + 200;
      Serial.println("O pote foi reabastecido com mais 200g de ração!");
      Serial.println(" ");
    }

  } else { // Se não houver movimento próximo, saída de nível baixo
    Serial.println("Sem movimento por aqui!");
    Serial.println(" ");
  }

  Serial.println("O loop será reiniciado em 3 segundos!");
  Serial.println(" ");

  delay(3000);

}
