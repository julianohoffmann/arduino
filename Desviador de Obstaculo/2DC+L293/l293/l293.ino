// Robô Autônomo Ultrassonico com L293

// Inclui a Biblioteca AFMotor
#include <AFMotor.h>

// Inclui a Biblioteca Ultrasonic
#include <Ultrasonic.h>

// Pinos para o sensor ultrassônico na L293
int pino_Trig = 10;
int pino_Echo = 9;

// Defina a potência dos motores: 0 a 255
int potencia = 200;

// Portas da Motor Shield
AF_DCMotor Motor_Esquerda(3);
AF_DCMotor Motor_Direita(2);

// Define a função que realizará a medida da distância
Ultrasonic Sensor(pino_Trig, pino_Echo);

// Variável para armazenar a distância
int distancia;

void setup() {
// Inicia com os motores parados
Motor_Esquerda.run(RELEASE);
Motor_Direita.run(RELEASE);

// Tempo para iniciar
delay(3000);
}
void frente(int potencia)
{
Motor_Esquerda.setSpeed(potencia);
Motor_Direita.setSpeed(potencia);
Motor_Esquerda.run(FORWARD);
Motor_Direita.run(FORWARD);
}
void manobra(int potencia)
{

// Potência dos motores
Motor_Esquerda.setSpeed(potencia * 0.6);
Motor_Direita.setSpeed(potencia * 0.6);
Motor_Esquerda.run(RELEASE);
Motor_Direita.run(RELEASE);
delay(500);
Motor_Esquerda.run(FORWARD);
Motor_Direita.run(BACKWARD);
delay(1000);
Motor_Esquerda.run(RELEASE);
Motor_Direita.run(RELEASE);
delay(500);
}

void loop() {
// Armazena a distância na variável
distancia = Sensor.read();

// Distância para a manobra
if (distancia <= 40)
{

// Função da manobra
manobra(potencia);
}

// Senão continue
else
{
// Função mover para frente
frente(potencia);
}
}