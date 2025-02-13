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
#define SERVO_UP  60
#define SERVO_DOWN 45

#define MAX_POS_X 282
#define MIN_POS_X 15
#define MAX_POS_Y 195
#define MIN_POS_Y 15

#define MM_TO_STEP(X) ((int)(X * 4.44444444))

int16_t posX;
int16_t posY;


typedef struct {
   int EN;
   int DIR;
   int STEP;
} MotorPins_t;
   

void servoUp(){
   servoWrite( SERVO_N , SERVO_UP );
   delay(500);
}

void servoDown(){
   servoWrite( SERVO_N , SERVO_DOWN );
   delay(500);
}

void initServo( int servoPin ){
   // Configurar Servo
   servoConfig( 0, SERVO_ENABLE );
   servoConfig( servoPin, SERVO_ENABLE_OUTPUT );
   servoUp();
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

   posX = 0;
   posY = 0;

   initServo( SERVO_N );

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

void moveAngle(MotorPins_t motor1, MotorPins_t motor2, int angle, int stepCount, int velocity){
   int i;
   float radianAngle = angle * (PI / 180.0); // Convert angle to radians
   float auxX =  cos(radianAngle);
   int x = stepCount * auxX;
   float auxY =  sin(radianAngle);
   int y = stepCount * auxY;
   for (i = 0; i < stepCount; i++) {
      moveX(motor1, motor2, x > 0 ? RIGHT : LEFT, fabs(x), velocity);
      moveY(motor1, motor2, y > 0 ? UP : DOWN, fabs(y), velocity);
   }
}

void moveTo( MotorPins_t motor1 , MotorPins_t motor2 , int16_t X , int16_t Y ){
   int16_t Ys = MM_TO_STEP(Y);
   int16_t Xs = MM_TO_STEP(X);
   
   if ( Ys > posY ){
      moveY( motor1, motor2, UP, Ys - posY, SPEED );
      posY = Ys;
   }else{
      moveY( motor1, motor2, DOWN, posY - Ys, SPEED );
      posY = Ys;
   }
   if ( Xs > posX ){
      moveX( motor1, motor2, RIGHT, Xs - posX, SPEED );
      posX = Xs;
   }else{
      moveX( motor1, motor2, LEFT, posX - Xs, SPEED );
      posX = Xs;
   }
}

// Funcion para dibujar un cuadrado
void drawSquare(MotorPins_t motor1, MotorPins_t motor2, int side, int velocity){
   servoDown();
   moveX(motor1, motor2, RIGHT, side, velocity);
   moveY(motor1, motor2, UP, side, velocity);
   moveX(motor1, motor2, LEFT, side, velocity);
   moveY(motor1, motor2, DOWN, side, velocity);
   servoUp();
}

// Funcion para dibujar un rectangulo
void drawRectangle(MotorPins_t motor1, MotorPins_t motor2, int side1, int side2, int velocity){
   servoDown();
   moveX(motor1, motor2, RIGHT, side1, velocity);
   moveY(motor1, motor2, UP, side2, velocity);
   moveX(motor1, motor2, LEFT, side1, velocity);
   moveY(motor1, motor2, DOWN, side2, velocity);
   servoUp();
}



void drawX( MotorPins_t motor1, MotorPins_t motor2, int side, int velocity){
   servoDown();
   moveDiagonal(motor1, motor2, RIGHT, UP, 2 * side, velocity);
   servoUp();
   moveY(motor1, motor2, DOWN, side, velocity);
   servoDown();
   moveDiagonal(motor1, motor2, LEFT, UP, 2 * side, velocity);
   servoUp();
   moveY(motor1, motor2, DOWN, side, velocity);
}

void drawStar(MotorPins_t motor1, MotorPins_t motor2, int side, int velocity){
   int i;
   int starSide = side/8;   
   servoDown();
   moveAngle(motor1, motor2, 72, starSide, velocity);
   moveAngle(motor1, motor2, 288, starSide, velocity);
   moveAngle(motor1, motor2, 144, starSide, velocity);
   moveAngle(motor1, motor2, 0, starSide, velocity);
   moveAngle(motor1, motor2, 216, starSide, velocity);
   servoUp();
}

void drawTriangle(MotorPins_t motor1, MotorPins_t motor2, int side, int velocity){
   servoDown();
   moveX(motor1, motor2, RIGHT, side, velocity);
   moveDiagonal(motor1, motor2, LEFT, UP, side, velocity);
   moveDiagonal(motor1, motor2, LEFT, DOWN, side, velocity);
   servoUp();
}

// Funcion para dibujar un circulo
void drawCircle(MotorPins_t motor1, MotorPins_t motor2, int radius, int velocity) {
   int steps = 360; // Dividir el círculo en 360 pasos
   int smooth = radius/8;
   servoDown();
   for (int i = 0; i < steps; i+=smooth) {
      float angle = (2 * PI * i) / steps;
      int x = (smooth) * cos(angle);
      int y = (smooth) * sin(angle);
      
      // Mover en el eje X
      moveX(motor1, motor2, x > 0 ? RIGHT : LEFT, fabs(x), velocity);
      
      // Mover en el eje Y
      moveY(motor1, motor2, y > 0 ? UP : DOWN, fabs(y), velocity);
   }
   servoUp();
}

void drawHeart(MotorPins_t motor1, MotorPins_t motor2, int size, int velocity){
   int i, j;
   int steps = 180; // Dividir el círculo en 360 pasos
   int smooth = size/8;
   int diagSize = (int)(size * 1.3);
   servoDown();
   moveDiagonal(motor1, motor2, RIGHT, UP, diagSize, velocity);
   for(j = 0; j < 2; j++){
      delay(50);
      for (i = 0; i < ((steps / 2)+6); i+=smooth) {
         float angle = (PI * i) / (steps / 2);
         int x = (smooth) * sin(angle);
         int y = (smooth) * cos(angle);
         // Mover en el eje X
         moveX(motor1, motor2, x > 0 ? LEFT : RIGHT, fabs(x), velocity);
         // Mover en el eje Y
         moveY(motor1, motor2, y > 0 ? UP : DOWN, fabs(y), velocity);
      }
   }
   delay(50);
   moveDiagonal(motor1, motor2, RIGHT, DOWN, diagSize, velocity);
   servoUp();
}



void drawFigure( MotorPins_t motor1, MotorPins_t motor2, char c ){
   int i;
   switch(c)
   {
      case 'C':
         drawSquare(motor1, motor2, 80, SPEED);
         break;
      case 'O':
         drawCircle(motor1, motor2, 80, SPEED);
         break;
      case 'X':
         drawX(motor1, motor2, 80, SPEED);
         break;
      case 'T':
         drawTriangle(motor1, motor2, 80, SPEED);
         break;
      case 'S':
         drawStar(motor1, motor2, 80, SPEED);
         break;
      case 'H':
         drawHeart(motor1, motor2, 80, SPEED);
         break;
      default :
         for(i=0;i<3;i++){
            gpioWrite( LEDR, ON );
            gpioWrite( LED2, ON );
            delay(200);
            gpioWrite( LEDR, OFF );
            gpioWrite( LED2, OFF );
            delay(200);
         }
   }
}

void showFigure( MotorPins_t motor1, MotorPins_t motor2, char c ){
   moveTo( motor1, motor2, 0, -60 );
   drawFigure(motor1, motor2, c);
   moveTo( motor1, motor2, 0, 0 );
}

void readTECdrawFigure( MotorPins_t motor1, MotorPins_t motor2 ){
   if (!gpioRead( TEC1 )){
      gpioWrite( LED1, ON );
      moveTo( motor1, motor2, 0, -30 );
      showFigure(motor1, motor2, 'C');
      moveTo( motor1, motor2, 0, 0 );
      gpioWrite( LED1, OFF );
   }
   if (!gpioRead( TEC2 )){
      gpioWrite( LED2, ON );
      moveTo( motor1, motor2, 0, -30 );
      showFigure(motor1, motor2, 'O');
      moveTo( motor1, motor2, 0, 0 );
      gpioWrite( LED2, OFF );

   }
   if (!gpioRead( TEC3 )){
      gpioWrite( LED3, ON );
      moveTo( motor1, motor2, 0, -30 );
      showFigure(motor1, motor2, 'X');
      moveTo( motor1, motor2, 0, 0 );
      gpioWrite( LED3, OFF );
   }
   if (!gpioRead( TEC4 )){
      gpioWrite( LEDR, ON );
      gpioWrite( LEDG, ON );
      moveTo( motor1, motor2, 0, -30 );
      showFigure(motor1, motor2, 'H');
      moveTo( motor1, motor2, 0, 0 );
      gpioWrite( LEDR, OFF );
      gpioWrite( LEDG, OFF );
   }
}

void conectAPP(MotorPins_t motor1, MotorPins_t motor2){
   char receivedChar;
   if (uartReadByte(UART_232, (uint8_t*)&receivedChar)) {
      gpioWrite( LEDB, OFF );
      if (receivedChar < 'A' || receivedChar > 'Z'){
         gpioWrite( LEDR, ON );
         gpioWrite( LEDG, ON );
         while((uartReadByte(UART_232, (uint8_t*)&receivedChar) && (receivedChar < 'A' || receivedChar > 'Z'))){
         }
         gpioWrite( LEDR, OFF );
         gpioWrite( LEDG, OFF );
      }
      if (receivedChar >= 'A' && receivedChar <= 'Z') {
         showFigure(motor1, motor2, receivedChar);
         uartWriteByte(UART_232, 'F');
         //uartWriteByte(UART_USB, receivedChar);
         delay(200);
      }
      gpioWrite( LEDB, ON );
   }
}



// Funcion principal del programa
int main(void) {
   boardConfig();
   
   MotorPins_t motor1;
   MotorPins_t motor2;

   printerInit( &motor1, &motor2 );

   // Inicializar la UART
   //uartConfig(UART_USB, 9600); // Comunicacion UART con la ESP32
   uartConfig(UART_232, 9600); // Comunicacion DEBUG

   // Bucle infinito para controlar el motor
   while (1) {
      /* Prendo el led azul */
      gpioWrite( LEDB, ON );
      delay(200);
     
      //conectAPP( motor1, motor2 );
      readTECdrawFigure( motor1, motor2 );


 /*     
      motorRun(motor2, H, 100, SPEED);
      
      delay(1000);

      motorRun(motor2, AH, 100, SPEED);

      delay(1000);
*/
      /*
      if (!gpioRead( TEC1 )){
         moveTo( motor1, motor2, 30, 0 );
         delay(1000);
         moveTo( motor1, motor2, 0, 0 );
         delay(1000);
         moveTo( motor1, motor2, 0, 30 );
         delay(1000);
         moveTo( motor1, motor2, 0, 0 );
         delay(1000);
         moveTo( motor1, motor2, -30, 0 );
         delay(1000);
         moveTo( motor1, motor2, 0, 0 );
         delay(1000);
         moveTo( motor1, motor2, 0, -30 );
         delay(1000);
         moveTo( motor1, motor2, 0, 0 );
         delay(1000);
      }
      */
      /*
      if (!gpioRead( TEC1 )){
         moveTo( motor1, motor2, 30, 30 );
         delay(1000);
         moveTo( motor1, motor2, 0, 0 );
         delay(1000);
         moveTo( motor1, motor2, -30, 30 );
         delay(1000);
         moveTo( motor1, motor2, 0, 0 );
         delay(1000);
         moveTo( motor1, motor2, -30, -30 );
         delay(1000);
         moveTo( motor1, motor2, 0, 0 );
         delay(1000);
         moveTo( motor1, motor2, 30, -30 );
         delay(1000);
         moveTo( motor1, motor2, 0, 0 );
         delay(1000);
      }
      */
      gpioWrite( LEDB, OFF );
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