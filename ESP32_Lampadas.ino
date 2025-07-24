#include <WiFi.h>
#include <PubSubClient.h>

// Configurações WiFi
const char* ssid = "AAPM-510";
const char* password = "aapmSenai#";

// Configurações MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";

// Definição dos pinos das lâmpadas
const int LAMPADA_SALA = 25;
const int LAMPADA_COZINHA = 26;
const int LED_WIFI = 33;

// Estados das lâmpadas
bool estadoSala = false;
bool estadoCozinha = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  
  // Configurar pinos como saída
  pinMode(LAMPADA_SALA, OUTPUT);
  pinMode(LAMPADA_COZINHA, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  
  // Inicializar lâmpadas desligadas e LED WiFi desligado
  digitalWrite(LAMPADA_SALA, HIGH);
  digitalWrite(LAMPADA_COZINHA, HIGH);
  digitalWrite(LED_WIFI, LOW);
  
  // Conectar ao WiFi
  setup_wifi();
  
  // Configurar MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Piscar LED durante conexão
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_WIFI, HIGH);
    delay(250);
    digitalWrite(LED_WIFI, LOW);
    delay(250);
    Serial.print(".");
  }

  // WiFi conectado - acender LED fixo
  digitalWrite(LED_WIFI, HIGH);
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("LED WiFi: LIGADO (conectado)");
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.print(topic);
  Serial.print(". Mensagem: ");
  
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);

  // Converter tópico para String para facilitar comparação
  String topicStr = String(topic);
  
  // Controle da lâmpada da sala
  if (topicStr == "senai928/lampada/sala/ligar") {
    if (messageTemp == "1") {
      digitalWrite(LAMPADA_SALA, LOW);
      estadoSala = true;
      Serial.println("Lâmpada da sala LIGADA");
    }
  }
  else if (topicStr == "senai928/lampada/sala/desligar") {
    if (messageTemp == "0") {
      digitalWrite(LAMPADA_SALA, HIGH);
      estadoSala = false;
      Serial.println("Lâmpada da sala DESLIGADA");
    }
  }
  
  // Controle da lâmpada da cozinha
  else if (topicStr == "senai928/lampada/cozinha/ligar") {
    if (messageTemp == "1") {
      digitalWrite(LAMPADA_COZINHA, LOW);
      estadoCozinha = true;
      Serial.println("Lâmpada da cozinha LIGADA");
    }
  }
  else if (topicStr == "senai928/lampada/cozinha/desligar") {
    if (messageTemp == "0") {
      digitalWrite(LAMPADA_COZINHA, HIGH);
      estadoCozinha = false;
      Serial.println("Lâmpada da cozinha DESLIGADA");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    
    // Criar um ID de cliente aleatório
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao MQTT!");
      
      // Inscrever nos tópicos
      client.subscribe("senai928/lampada/sala/ligar");
      client.subscribe("senai928/lampada/sala/desligar");
      client.subscribe("senai928/lampada/cozinha/ligar");
      client.subscribe("senai928/lampada/cozinha/desligar");
      
      Serial.println("Inscrito nos tópicos MQTT");
    } else {
      Serial.print("Falha na conexão MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void loop() {
  // Verificar status do WiFi e controlar LED
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_WIFI, HIGH);  // WiFi conectado - LED ligado
  } else {
    // WiFi desconectado - piscar LED rapidamente
    digitalWrite(LED_WIFI, HIGH);
    delay(100);
    digitalWrite(LED_WIFI, LOW);
    delay(100);
    Serial.println("WiFi desconectado! Tentando reconectar...");
    
    // Tentar reconectar WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && millis() % 5000 != 0) {
      delay(100);
    }
  }
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Opcional: Enviar status das lâmpadas periodicamente
  static unsigned long lastMsg = 0;
  unsigned long now = millis();
  if (now - lastMsg > 30000) { // A cada 30 segundos
    lastMsg = now;
    
    // Publicar status atual das lâmpadas
    String statusSala = estadoSala ? "1" : "0";
    String statusCozinha = estadoCozinha ? "1" : "0";
    
    client.publish("senai928/lampada/sala/status", statusSala.c_str());
    client.publish("senai928/lampada/cozinha/status", statusCozinha.c_str());
    
    Serial.println("Status enviado - Sala: " + statusSala + ", Cozinha: " + statusCozinha);
  }
}