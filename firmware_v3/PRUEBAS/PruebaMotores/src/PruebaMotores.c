#include "sapi.h"  // Biblioteca de la EDU-CIAA
#include <math.h>
#include <string.h>

// Buffer para almacenar la figura recibida
#define BUFFER_SIZE 50
char figuraRecibida[BUFFER_SIZE];
int index = 0;


// Definir los pines para control del motor paso a paso
#define STEP_PIN1 GPIO7  // GPIO1 para STEP
#define DIR_PIN1 GPIO8  // GPIO3 para Direcci�n (DIR)
#define EN_PIN1 GPIO5
#define STEP_PIN2 GPIO1  // GPIO1 para STEP
#define DIR_PIN2 GPIO3  // GPIO3 para Direcci�n (DIR)
#define EN_PIN2 LCD1

#define SPEED 5
#define SMOOTHNESS 10

#define PI 3.1415926535897932384626433

//Definir el sentido de giro de los motores
#define H true
#define AH false
#define UP true
#define DOWN false
#define RIGHT true
#define LEFT false

#define SERVO_N   SERVO3
#define SERVO_UP  70
#define SERVO_DOWN 45

#define MAX_POS_X 282
#define MIN_POS_X 15
#define MAX_POS_Y 195
#define MIN_POS_Y 15

#define MM_TO_STEP(X) ((int)(X * 4.44444444))

int posX;
int posY;


typedef struct {
   int EN;
   int DIR;
   int STEP;
} MotorPins_t;
   

void initServo( int servoPin ){
   // Configurar Servo
   servoConfig( 0, SERVO_ENABLE );
   servoConfig( servoPin, SERVO_ENABLE_OUTPUT );
}

void servoUp( int servoPin ){
   servoWrite( servoPin, SERVO_UP );
   delay(250);
}

void servoDown( int servoPin ){
   servoWrite( servoPin, SERVO_DOWN );
   delay(250);
}

// Funcion de inicializacion de los motores
void motorInit(MotorPins_t * motor, int enPin, int dirPin, int stepPin) {
   
   motor->EN = enPin;
   motor->DIR = dirPin;
   motor->STEP = stepPin;
   
   gpioConfig(motor->EN, GPIO_OUTPUT);
   gpioConfig(motor->DIR, GPIO_OUTPUT);
   gpioConfig(motor->STEP, GPIO_OUTPUT);
   
   
   gpioWrite(motor->EN, ON);
   
}

void printerInit( MotorPins_t * motor1, MotorPins_t * motor2 ){

   motorInit( motor1, EN_PIN1, DIR_PIN1, STEP_PIN1 );
   motorInit( motor2, EN_PIN2, DIR_PIN2, STEP_PIN2 );

   posX = 100;
   posY = 100;

   initServo( SERVO_N );
   servoUp( SERVO_N );

}

// Funcion para realizar un paso en un motor
// NO INCLUYE EL ENABLE
void motorStep(MotorPins_t motor, bool direction) {
    // Configurar la direcci�n del motor
    if (direction) {
        gpioWrite(motor.DIR, ON);  // Sentido horario
    } else {
        gpioWrite(motor.DIR, OFF);   // Sentido antihorario
    }
    
    // Generar un pulso en el pin STEP
    gpioWrite(motor.STEP, ON);  // Enviar pulso alto
    delay(1);
    gpioWrite(motor.STEP, OFF);   // Enviar pulso bajo
    delay(1);
}

// Funcion para realizar varios pasos en un motor
void motorRun(MotorPins_t motor, bool direction, int stepCount, int velocity){
   gpioWrite(motor.EN, OFF);
   delay(1);
   for (int i = 0; i < stepCount; i++) {
      motorStep(motor, direction);
      delay(velocity);
   }
   gpioWrite(motor.EN, ON);
}

// Funcion para realizar un paso en dos motores en simultaneo
// NO INCLUYE EL ENABLE
void twoMotorStep(MotorPins_t motor1, bool direction1, MotorPins_t motor2, bool direction2) {
    // Configurar la direcci�n del motor
    if (direction1) {
        gpioWrite(motor1.DIR, ON);  // Sentido horario
    } else {
        gpioWrite(motor1.DIR, OFF);   // Sentido antihorario
    }
    if (direction2) {
        gpioWrite(motor2.DIR, ON);  // Sentido horario
    } else {
        gpioWrite(motor2.DIR, OFF);   // Sentido antihorario
    }
    
    // Generar un pulso en el pin STEP
    gpioWrite(motor1.STEP, ON);  // Enviar pulso alto
    gpioWrite(motor2.STEP, ON);  // Enviar pulso alto
    delay(1);
    gpioWrite(motor1.STEP, OFF);   // Enviar pulso bajo
    gpioWrite(motor2.STEP, OFF);   // Enviar pulso bajo
    delay(1);
}

// Funcion para realizar varios pasos en dos motores en simultaneo
void twoMotorRun(MotorPins_t motor1, bool direction1, MotorPins_t motor2, bool direction2, int stepCount, int velocity){
   gpioWrite(motor1.EN, OFF);
   gpioWrite(motor2.EN, OFF);
   delay(1);
   for (int i = 0; i < stepCount; i++) {
      twoMotorStep(motor1, direction1, motor2, direction2);
      delay(velocity);
   }
   gpioWrite(motor1.EN, ON);
   gpioWrite(motor2.EN, ON);
}

int insideXlimits( int cant , int sentido){
   /*int posFin = posX + (cant * ( sentido ? 1 : -1 ));
   if (( posFin < MAX_POS_X )&&( posFin > MIN_POS_X )){
      return 1;
   }else{
      return 0;
   }*/
   return 1;
}

// Funcion para mover un paso en el eje X
// NO INCLUYE EL ENABLE
void stepMoveX(MotorPins_t motor1, MotorPins_t motor2, bool sentido){
   /*if ( insideXlimits( 1 , sentido ) ){
      twoMotorStep(motor1, sentido, motor2, sentido);
      posX += ( sentido ? 1 : -1 );
   }*/
   twoMotorStep(motor1, sentido, motor2, sentido);
}

// Funcion para mover en el eje X
// Ambos motores deben moverse en el mismo sentido para realizar el movimiento
void moveX(MotorPins_t motor1, MotorPins_t motor2, bool sentido, int stepCount, int velocity){
   /*if ( insideXlimits( stepCount , sentido ) ){
      twoMotorRun(motor1, sentido, motor2, sentido, stepCount, velocity);
      posX += ( sentido ? stepCount : -stepCount );
   }*/
   twoMotorRun(motor1, sentido, motor2, sentido, stepCount, velocity);
}

int insideYlimits( int cant , int sentido){
   /*int posFin = posY + (cant * ( sentido ? 1 : -1 ));
   if (( posFin < MAX_POS_Y )&&( posFin > MIN_POS_Y )){
      return 1;
   }else{
      return 0;
   }*/
   return 1;
}

// Funcion para mover un paso en el eje Y
// NO INCLUYE EL ENABLE
void stepMoveY(MotorPins_t motor1, MotorPins_t motor2, bool sentido){
   /*if ( insideYlimits( 1 , sentido ) ){
      twoMotorStep(motor1, !sentido, motor2, !sentido);
      posY += ( sentido ? 1 : -1 );
   }*/
   twoMotorStep(motor1, sentido, motor2, !sentido);
}

// Funcion para mover en el eje Y
// Ambos motores deben moverse en sentidos opuestos para realizar el movimiento
void moveY(MotorPins_t motor1, MotorPins_t motor2, bool sentido, int stepCount, int velocity){
   /*if ( insideYlimits( stepCount , sentido ) ){
      twoMotorRun(motor1, !sentido, motor2, !sentido, stepCount, velocity);
      posY += ( sentido ? stepCount : -stepCount );
   }*/
   twoMotorRun(motor1, sentido, motor2, !sentido, stepCount, velocity);
}


void moveDiagonal(MotorPins_t motor1, MotorPins_t motor2, bool sentidoX, bool sentidoY, int stepCount, int velocity){
   if ( sentidoX ^ sentidoY ) {
      motorRun(motor2, sentidoX, stepCount, velocity);
   }else{
      motorRun(motor1, sentidoX, stepCount, velocity);
   }
}

// Funcion para dibujar un cuadrado
void drawSquare(MotorPins_t motor1, MotorPins_t motor2, int side, int velocity){
   servoDown( SERVO_N );
   moveX(motor1, motor2, RIGHT, side, velocity);
   moveY(motor1, motor2, UP, side, velocity);
   moveX(motor1, motor2, LEFT, side, velocity);
   moveY(motor1, motor2, DOWN, side, velocity);
   servoUp( SERVO_N );
}

// Funcion para dibujar un rectangulo
void drawRectangle(MotorPins_t motor1, MotorPins_t motor2, int side1, int side2, int velocity){
   servoDown( SERVO_N );
   moveX(motor1, motor2, RIGHT, side1, velocity);
   moveY(motor1, motor2, UP, side2, velocity);
   moveX(motor1, motor2, LEFT, side1, velocity);
   moveY(motor1, motor2, DOWN, side2, velocity);
   servoUp( SERVO_N );
}

// Funcion para dibujar un circulo
void drawCircle(MotorPins_t motor1, MotorPins_t motor2, int radius, int velocity) {
   int steps = 360; // Dividir el círculo en 360 pasos
   servoDown( SERVO_N );
   for (int i = 0; i < steps; i++) {
      float angle = (2 * PI * i) / steps;
      int x = radius * cos(angle);
      int y = radius * sin(angle);
      
      // Mover en el eje X
      moveX(motor1, motor2, x > 0 ? RIGHT : LEFT, fabs(x), velocity);
      
      // Mover en el eje Y
      moveY(motor1, motor2, y > 0 ? UP : DOWN, fabs(y), velocity);
   }
   servoUp( SERVO_N );

}

void drawX( MotorPins_t motor1, MotorPins_t motor2, int side, int velocity){
   servoDown( SERVO_N );
   moveDiagonal(motor1, motor2, RIGHT, UP, 2 * side, velocity);
   servoUp( SERVO_N );
   moveY(motor1, motor2, DOWN, side, velocity);
   servoDown( SERVO_N );
   moveDiagonal(motor1, motor2, LEFT, UP, 2 * side, velocity);
   servoUp( SERVO_N );
   moveY(motor1, motor2, DOWN, side, velocity);
}

void readTECdrawFigure( MotorPins_t motor1, MotorPins_t motor2 ){
   if (!gpioRead( TEC1 )){
      gpioWrite( LED1, ON );
      drawSquare(motor1, motor2, 80, SPEED);
      gpioWrite( LED1, OFF );
   }
   if (!gpioRead( TEC2 )){
      gpioWrite( LED2, ON );
      drawCircle(motor1, motor2, 3, SPEED);
      gpioWrite( LED2, OFF );

   }
   if (!gpioRead( TEC3 )){
      gpioWrite( LED3, ON );
      drawX(motor1, motor2, 80, SPEED);
      gpioWrite( LED3, OFF );
   }
}

void drawFigure( MotorPins_t motor1, MotorPins_t motor2, char c ){
   int i;
   switch(c)
   {
      case 'C':
         drawSquare(motor1, motor2, MM_TO_STEP(30), SPEED);
         break;
      case 'O':
         drawCircle(motor1, motor2, 4, SPEED);
         break;
      case 'X':
         drawX(motor1, motor2, MM_TO_STEP(30), SPEED);
         break;
      default :
         for(i=0;i<10;i++){
            gpioWrite( LEDR, ON );
            gpioWrite( LED2, ON );
            delay(200);
            gpioWrite( LEDR, OFF );
            gpioWrite( LED2, OFF );
            delay(200);
         }
   }
}

// Funcion principal del programa
int main(void) {
   boardConfig();
   
   MotorPins_t motor1;
   MotorPins_t motor2;

   printerInit( &motor1, &motor2 );

   // Inicializar la UART
   //uartConfig(UART_232, 115200); // Comunicacion UART con la ESP32
   uartConfig(UART_232, 9600); // Comunicaci�n DEBUG
   
   char receivedChar;

    // Bucle infinito para controlar el motor
   while (1) {
      /* Prendo el led azul */
      gpioWrite( LEDG, ON );

      delay(200);

      if (uartReadByte(UART_232, (uint8_t*)&receivedChar)) {
         gpioWrite( LEDG, OFF );
         drawFigure(motor1, motor2, receivedChar);
         delay(200);
         uartWriteByte(UART_232, 'F');
         delay(200);
         gpioWrite( LEDG, ON );
      }

      gpioWrite( LEDG, OFF );

      delay(200);
      
      
/*
      gpioWrite( LED2, ON );
      drawX(motor1, motor2, 80, SPEED);
      gpioWrite( LED2, OFF ); 


      gpioWrite( LED2, ON );
      drawCircle(motor1, motor2, 10, SPEED);
      gpioWrite( LED2, OFF ); 


      gpioWrite( LED2, ON );
      drawSquare(motor1, motor2, 80, SPEED);
      gpioWrite( LED2, OFF );
*/
/*
      gpioWrite( LEDR, ON );
      gpioWrite( LED3, ON );
      // Realizar 200 pasos en sentido horario motor 1
      motorRun(motor1, H, 200, SPEED);
      gpioWrite( LEDR, OFF );
      gpioWrite( LED3, OFF );

      // Pausa antes de cambiar de direcci�n
      delay(1000);

      gpioWrite( LEDG, ON );
      gpioWrite( LED3, ON );
      // Realizar 200 pasos en sentido horario motor 2
      motorRun(motor2, H, 200, SPEED);
      gpioWrite( LEDG, OFF );
      gpioWrite( LED3, OFF );

      // Pausa antes de repetir el ciclo
      delay(1000);

      gpioWrite( LEDR, ON );
      gpioWrite( LED1, ON );
      // Realizar 200 pasos en sentido antihorario motor 1
      motorRun(motor1, AH, 200, SPEED);
      gpioWrite( LEDR, OFF );
      gpioWrite( LED1, OFF );

      // Pausa antes de cambiar de direcci�n
      delay(1000);

      gpioWrite( LEDG, ON );
      gpioWrite( LED1, ON );
      // Realizar 200 pasos en sentido antihorario motor 2
      motorRun(motor2, AH, 200, SPEED);
      gpioWrite( LEDG, OFF );
      gpioWrite( LED1, OFF );

      // Pausa antes de repetir el ciclo
      delay(1000);

      gpioWrite( LED2, ON );
      twoMotorRun(motor1, H, motor2, H, 200, SPEED);
      gpioWrite( LED2, OFF );
*/
      //delay(1000);

    }

    return 0;
}