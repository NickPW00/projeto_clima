#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h> //INCLUSÃO DE BIBLIOTECA
#include <Adafruit_BMP280.h> //INCLUSÃO DE BIBLIOTECA

#define BH1750_ADDR 0x23  // Endereço padrão do sensor BH1750 (GY-30)
#define DHTPIN 2     // Pino digital conectado ao sensor DHT22
#define DHTTYPE DHT22 // Definindo o tipo de sensor como DHT22

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp; //OBJETO DO TIPO Adafruit_BMP280 (I2C)

const int pinSensorUV = A0;  // Pino analógico ao qual o sensor UV está conectado

void setup() {
  Serial.begin(9600);
  Serial.println(F("Leitura do Sensor DHT22:"));
  Serial.println(F("Luminosidade com Sensor GY-30 (BH1750)"));
  Wire.begin();
  dht.begin();
  if(!bmp.begin(0x76)){ //SE O SENSOR NÃO FOR INICIALIZADO NO ENDEREÇO I2C 0x76, FAZ
    Serial.println(F("Sensor BMP280 não foi identificado! Verifique as conexões.")); 
    while(1); //SEMPRE ENTRE NO LOOP
  }
}

void loop() {
  float humidity = dht.readHumidity();     // Leia a umidade
  float temperature = dht.readTemperature(); // Leia a temperatura em Celsius
  float luminosidade = lerLuminosidade();
  
  int valorAnalogico = analogRead(pinSensorUV);  // Lê o valor analógico do sensor
  int percentualUV = map(valorAnalogico, 0, 1023, 0, 100);

  // GY-30 - deve ter o CI para pôr tudo certinho
  if (luminosidade >= 0) {
    Serial.print("Luminosidade: ");
    Serial.print(luminosidade);
    Serial.println(" lux");
  }

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Erro ao ler do sensor DHT22!"));
    return;
  }

  // DTH22
  Serial.print(F("Umidade: "));
  Serial.print(humidity);
  Serial.print(F("% | Temperatura: "));
  Serial.print(temperature);
  Serial.println(F("°C"));

  Serial.print(F("Temperatura: ")); 
  Serial.print(bmp.readTemperature()); 
  Serial.println(" *C (Grau Celsius)"); 

  //BMP-280
  Serial.print(F("Pressão: ")); 
  Serial.print(bmp.readPressure()); 
  Serial.println(" Pa (Pascal)"); 

  Serial.print(F("Altitude aprox.: ")); 
  Serial.print(bmp.readAltitude(1013.25),0); 
  Serial.println(" m (Metros)"); 

  //UV
  Serial.print("Valor UV: ");
  Serial.print(valorAnalogico);
  Serial.print(" | Percentual UV: ");
  Serial.println(percentualUV);

  Serial.println("-----------------------------------"); //IMPRIME UMA LINHA NO MONITOR SERIAL

  delay(2000); // Aguarde 5 segundos antes da próxima leitura
}

float lerLuminosidade() {
  Wire.beginTransmission(BH1750_ADDR);
  Wire.write(0x10); // Inicia medição com modo de alta resolução, tambem pode por  0x77
  Wire.endTransmission();

  delay(200); // Aguarde o sensor realizar a medição

  Wire.requestFrom(BH1750_ADDR, 2);
  
  if (Wire.available() >= 2) {
    uint16_t valor = Wire.read() << 8 | Wire.read();
    float luminosidade = valor / 1.2; // Converte o valor bruto para lux
    
    return luminosidade;
  } else {
    Serial.println("Erro ao ler o sensor BH1750");
    return -1.0; // Valor negativo indica erro
  }
}

