#include "sapi.h"  // Biblioteca de la EDU-CIAA
#include <math.h>  // Biblioteca para funciones matemáticas
#include <string.h>  // Biblioteca para funciones de manipulación de cadenas

// Tamaño de buffer para los caraacteres de la comunicacion UART
#define BUFFER_SIZE 50

// Definir los pines para control del motor paso a paso
#define STEP_PIN1 GPIO7  // Pin GPIO7 para STEP del motor 1
#define DIR_PIN1 GPIO8  // Pin GPIO8 para Dirección (DIR) del motor 1
#define EN_PIN1 GPIO5  // Pin GPIO5 para habilitar el motor 1
#define STEP_PIN2 GPIO1  // Pin GPIO1 para STEP del motor 2
#define DIR_PIN2 GPIO3  // Pin GPIO3 para Dirección (DIR) del motor 2
#define EN_PIN2 LCD1  // Pin LCD1 para habilitar el motor 2

// Definir la velocidad y suavidad del movimiento
#define SPEED 5  // Velocidad del motor
#define SMOOTHNESS 10  // Suavidad del movimiento

// Definir el valor de PI
#define PI 3.1415926535897932384626433

// Definir el sentido de giro de los motores
#define H true  // Sentido horario
#define AH false  // Sentido antihorario
#define UP true  // Movimiento hacia arriba
#define DOWN false  // Movimiento hacia abajo
#define RIGHT true  // Movimiento hacia la derecha
#define LEFT false  // Movimiento hacia la izquierda

// Definir pines y posiciones para el servo
#define SERVO_N SERVO3  // Pin del servo
#define SERVO_UP 60  // Posición del servo hacia arriba
#define SERVO_DOWN 45  // Posición del servo hacia abajo

// Definir los límites máximos y mínimos de posición en los ejes X e Y
#define MAX_POS_X 282  // Posición máxima en el eje X
#define MIN_POS_X 15  // Posición mínima en el eje X
#define MAX_POS_Y 195  // Posición máxima en el eje Y
#define MIN_POS_Y 15  // Posición mínima en el eje Y

// Macro para convertir milímetros a pasos del motor
#define MM_TO_STEP(X) ((int)(X * 4.44444444))

// Variables globales para almacenar la posición actual en los ejes X e Y
int16_t posX;
int16_t posY;

// Estructura para almacenar los pines de control de un motor
typedef struct {
   int EN;  // Pin de habilitación del motor
   int DIR;  // Pin de dirección del motor
   int STEP;  // Pin de paso del motor
} MotorPins_t;

// Función para mover el servo hacia arriba
void servoUp() {
   servoWrite(SERVO_N, SERVO_UP);  // Mover el servo a la posición hacia arriba
   delay(500);  // Esperar 500 ms para que el movimiento se complete
}

// Función para mover el servo hacia abajo
void servoDown() {
   servoWrite(SERVO_N, SERVO_DOWN);  // Mover el servo a la posición hacia abajo
   delay(500);  // Esperar 500 ms para que el movimiento se complete
}

// Función para inicializar el servo
void initServo(int servoPin) {
    // Configurar el servo
    servoConfig(0, SERVO_ENABLE);  // Habilitar el servo
    servoConfig(servoPin, SERVO_ENABLE_OUTPUT);  // Configurar el pin del servo como salida
    servoUp();  // Mover el servo a la posición hacia arriba
}

// Función de inicialización de los motores
void motorInit(MotorPins_t *motor, int enPin, int dirPin, int stepPin) {
    // Asignar los pines de habilitación, dirección y paso al motor
    motor->EN = enPin;
    motor->DIR = dirPin;
    motor->STEP = stepPin;
    
    // Configurar los pines como salidas
    gpioConfig(motor->EN, GPIO_OUTPUT);
    gpioConfig(motor->DIR, GPIO_OUTPUT);
    gpioConfig(motor->STEP, GPIO_OUTPUT);
    
    // Habilitar el motor
    gpioWrite(motor->EN, ON);
}

// Función para inicializar la impresora
void printerInit(MotorPins_t *motor1, MotorPins_t *motor2) {
    // Inicializar el motor 1 con sus pines correspondientes
    motorInit(motor1, EN_PIN1, DIR_PIN1, STEP_PIN1);
    // Inicializar el motor 2 con sus pines correspondientes
    motorInit(motor2, EN_PIN2, DIR_PIN2, STEP_PIN2);

    // Inicializar las posiciones X e Y a 0
    posX = 0;
    posY = 0;

    // Inicializar el servo
    initServo(SERVO_N);
}


// Funcion para realizar un paso en un motor
// NO INCLUYE EL ENABLE
void motorStep(MotorPins_t motor, bool direction) {
    // Configurar la dirección del motor
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
   gpioWrite(motor.EN, OFF);  // Habilitar el motor
   delay(1);   // Esperar 1 ms para que el motor se active
   for (int i = 0; i < stepCount; i++) {
      motorStep(motor, direction);  // Realizar un paso en el motor
      delay(velocity);  // Esperar un tiempo para el siguiente paso
   }
   gpioWrite(motor.EN, ON); // Deshabilitar el motor
}

// Funcion para realizar un paso en dos motores en simultaneo
// NO INCLUYE EL ENABLE
void twoMotorStep(MotorPins_t motor1, bool direction1, MotorPins_t motor2, bool direction2) {
    // Configurar la dirección del motor
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
   // Habilitar los motores
   gpioWrite(motor1.EN, OFF);
   gpioWrite(motor2.EN, OFF);
   delay(1);
   for (int i = 0; i < stepCount; i++) {
      twoMotorStep(motor1, direction1, motor2, direction2); // Realizar un paso en los motores
      delay(velocity); // Esperar un tiempo para el siguiente paso
   }
   // Deshabilitar los motores
   gpioWrite(motor1.EN, ON);
   gpioWrite(motor2.EN, ON);
}

// Funcion para mover un paso en el eje X
// NO INCLUYE EL ENABLE
void stepMoveX(MotorPins_t motor1, MotorPins_t motor2, bool sentido){
   twoMotorStep(motor1, sentido, motor2, sentido);
}

// Funcion para mover en el eje X
// Ambos motores deben moverse en el mismo sentido para realizar el movimiento
void moveX(MotorPins_t motor1, MotorPins_t motor2, bool sentido, int stepCount, int velocity){
   twoMotorRun(motor1, sentido, motor2, sentido, stepCount, velocity);
}

// Funcion para mover un paso en el eje Y
// NO INCLUYE EL ENABLE
void stepMoveY(MotorPins_t motor1, MotorPins_t motor2, bool sentido){
   twoMotorStep(motor1, sentido, motor2, !sentido);
}

// Funcion para mover en el eje Y
// Ambos motores deben moverse en sentidos opuestos para realizar el movimiento
void moveY(MotorPins_t motor1, MotorPins_t motor2, bool sentido, int stepCount, int velocity){
   twoMotorRun(motor1, sentido, motor2, !sentido, stepCount, velocity);
}

// Funcion para mover en diagonal
// Si se mueve uno solo de los motores se realiza un movimiento en diagonal
void moveDiagonal(MotorPins_t motor1, MotorPins_t motor2, bool sentidoX, bool sentidoY, int stepCount, int velocity){
   if ( sentidoX ^ sentidoY ) {
      motorRun(motor2, sentidoX, stepCount, velocity);
   }else{
      motorRun(motor1, sentidoX, stepCount, velocity);
   }
}

// Funcion para mover en un angulo
void moveAngle(MotorPins_t motor1, MotorPins_t motor2, int angle, int stepCount, int velocity){
   int i;
   float radianAngle = angle * (PI / 180.0); // Convertir el ángulo a radianes
   float auxX =  cos(radianAngle); // Calcular el coseno del ángulo
   int x = stepCount * auxX; // Calcular el desplazamiento en X
   float auxY =  sin(radianAngle); // Calcular el seno del ángulo
   int y = stepCount * auxY; // Calcular el desplazamiento en Y
   for (i = 0; i < stepCount; i++) {
      moveX(motor1, motor2, x > 0 ? RIGHT : LEFT, fabs(x), velocity);
      moveY(motor1, motor2, y > 0 ? UP : DOWN, fabs(y), velocity);
   }
}

void moveTo( MotorPins_t motor1 , MotorPins_t motor2 , int16_t X , int16_t Y ){
   int16_t Ys = MM_TO_STEP(Y); // Convertir la posición en milímetros a pasos
   int16_t Xs = MM_TO_STEP(X); // Convertir la posición en milímetros a pasos
   
   if ( Ys > posY ){ // Si la posición en Y es mayor a la actual
      moveY( motor1, motor2, UP, Ys - posY, SPEED ); // Mover hacia arriba
      posY = Ys; // Actualizar la posición en Y
   }else{ // Si la posición en Y es menor a la actual
      moveY( motor1, motor2, DOWN, posY - Ys, SPEED ); // Mover hacia abajo
      posY = Ys; // Actualizar la posición en Y
   }
   if ( Xs > posX ){ // Si la posición en X es mayor a la actual
      moveX( motor1, motor2, RIGHT, Xs - posX, SPEED ); // Mover hacia la derecha
      posX = Xs; // Actualizar la posición en X
   }else{ // Si la posición en X es menor a la actual
      moveX( motor1, motor2, LEFT, posX - Xs, SPEED ); // Mover hacia la izquierda
      posX = Xs; // Actualizar la posición en X
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

// Funcion para dibujar una X
void drawX( MotorPins_t motor1, MotorPins_t motor2, int side, int velocity){
   // Dibujar primer diagonal
   servoDown();
   moveDiagonal(motor1, motor2, RIGHT, UP, 2 * side, velocity);
   servoUp();
   moveY(motor1, motor2, DOWN, side, velocity);
   // Dibujar segunda diagonal
   servoDown();
   moveDiagonal(motor1, motor2, LEFT, UP, 2 * side, velocity);
   servoUp();
   moveY(motor1, motor2, DOWN, side, velocity);
}

// Funcion para dibujar una estrella
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

// Funcion para dibujar un triangulo
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

// Funcion para dibujar un corazon
void drawHeart(MotorPins_t motor1, MotorPins_t motor2, int size, int velocity){
   int i, j;
   int steps = 180; // Dividir el círculo en 360 pasos
   int smooth = size/8;
   int diagSize = (int)(size * 1.3);
   servoDown();
   moveDiagonal(motor1, motor2, RIGHT, UP, diagSize, velocity);
   // Dibujar dos semicirculos
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

// Funcion para dibujar una figura
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
         // Parpadear los leds en caso de error
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

// Funcion para dibujar una figura y mostrarla
void showFigure( MotorPins_t motor1, MotorPins_t motor2, char c ){
   moveTo( motor1, motor2, 0, -60 );
   drawFigure(motor1, motor2, c);
   moveTo( motor1, motor2, 0, 0 );
}

// Funcion para leer los botones y dibujar una figura
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
   char receivedChar, receivedLetter;
   if (uartReadByte(UART_232, (uint8_t*)&receivedChar)) {
      gpioWrite( LEDB, OFF );
      if (receivedChar != '.'){
         gpioWrite( LEDR, ON );
         gpioWrite( LEDG, ON );
         while((uartReadByte(UART_232, (uint8_t*)&receivedChar) && (receivedChar != '.'))){
         }
         gpioWrite( LEDR, OFF );
         gpioWrite( LEDG, OFF );
      }
      if (receivedChar == '.') {
         if ((uartReadByte(UART_232, (uint8_t*)&receivedChar))&&(receivedChar == '*')) {
            if ((uartReadByte(UART_232, (uint8_t*)&receivedLetter))&&((receivedLetter >= 'A' && receivedLetter <= 'Z')||(receivedLetter >= '0' && receivedLetter <= '9'))) {
               if ((uartReadByte(UART_232, (uint8_t*)&receivedChar))&&(receivedChar == '*')) {
                  if ((uartReadByte(UART_232, (uint8_t*)&receivedChar))&&(receivedChar == '.')) {
                     showFigure(motor1, motor2, receivedLetter);
                     uartWriteByte(UART_232, 'F');
                     delay(200);
                  }
               }
            }
         }
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
     
      conectAPP( motor1, motor2 );
      readTECdrawFigure( motor1, motor2 );

      gpioWrite( LEDB, OFF );
      delay(200);

    }

    return 0;
}