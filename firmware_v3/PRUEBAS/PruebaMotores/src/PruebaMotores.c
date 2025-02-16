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
void moveAngleOld(MotorPins_t motor1, MotorPins_t motor2, int angle, int stepCount, int velocity){
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

// Funcion para mover en un angulo
void moveAngle(MotorPins_t motor1, MotorPins_t motor2, int angle, int stepCount, int velocity){
   int i;
   int smooth = 6;
   float radianAngle = angle * (PI / 180.0); // Convertir el ángulo a radianes
   float auxX =  cos(radianAngle); // Calcular el coseno del ángulo
   int x = smooth * auxX; // Calcular el desplazamiento en X
   float auxY =  sin(radianAngle); // Calcular el seno del ángulo
   int y = smooth * auxY; // Calcular el desplazamiento en Y
   for (i = 0; i < stepCount; i += smooth) {
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
void drawStarOld(MotorPins_t motor1, MotorPins_t motor2, int side, int velocity){
   int i;  
   servoDown();
   moveAngle(motor1, motor2, 72, side, velocity);
   moveAngle(motor1, motor2, 288, side, velocity);
   moveAngle(motor1, motor2, 144, side, velocity);
   moveAngle(motor1, motor2, 0, side, velocity);
   moveAngle(motor1, motor2, 216, side, velocity);
   servoUp();
}

// Funcion para dibujar una estrella
void drawStar(MotorPins_t motor1, MotorPins_t motor2, int side, int velocity){
   int i;  
   servoDown();
   moveAngle(motor1, motor2, 68, side, velocity);
   moveAngle(motor1, motor2, 292, (int)(side*1.1), velocity);
   moveAngle(motor1, motor2, 144, (int)(side*1.25), velocity);
   moveAngle(motor1, motor2, 0, (int)(side*0.95), velocity);
   moveAngle(motor1, motor2, 214, (int)(side*1.2), velocity);
   servoUp();
}

// Funcion para dibujar un triangulo
void drawTriangleOld(MotorPins_t motor1, MotorPins_t motor2, int side, int velocity){
   servoDown();
   moveX(motor1, motor2, RIGHT, side, velocity);
   moveDiagonal(motor1, motor2, LEFT, UP, side, velocity);
   moveDiagonal(motor1, motor2, LEFT, DOWN, side, velocity);
   servoUp();
}

// Funcion para dibujar un triangulo
void drawTriangle(MotorPins_t motor1, MotorPins_t motor2, int side, int velocity){
   servoDown();
   moveX(motor1, motor2, RIGHT, side, velocity);
   delay(50);
   moveAngle(motor1, motor2, 122, side, velocity);
   delay(50);
   moveAngle(motor1, motor2, 238, side, velocity);
   servoUp();
}

// Funcion para dibujar un circulo
void drawCircleOld(MotorPins_t motor1, MotorPins_t motor2, int radius, int velocity) {
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

// Funcion para dibujar un circulo
void drawCircle(MotorPins_t motor1, MotorPins_t motor2, int radius, int velocity) {
   int steps = 5 * radius; 
   int smooth = 5;
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
void drawHeartOld(MotorPins_t motor1, MotorPins_t motor2, int size, int velocity){
   int i, j;
   int steps = 180; // Dividir el círculo en 360 pasos
   int smooth = 10;
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

// Funcion para dibujar un corazon
void drawHeartCute(MotorPins_t motor1, MotorPins_t motor2, int size, int velocity){
   int i, j;
   int steps = 180; // Dividir el círculo en 360 pasos
   int smooth = 5;
   int diagSizeA = (int)(size * 1.1);
   int diagSizeB = (int)(size * 1.25);
   servoDown();
   moveDiagonal(motor1, motor2, RIGHT, UP, diagSizeA, velocity);
   // Dibujar dos semicirculos
   for(j = 0; j < 2; j++){
      delay(50);
      for (i = 0; i < ((steps / 2)); i+=smooth) {
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
   moveDiagonal(motor1, motor2, RIGHT, DOWN, diagSizeB, velocity);
   servoUp();
}

// Funcion para dibujar un corazon
void drawHeart(MotorPins_t motor1, MotorPins_t motor2, int size, int velocity){
   int i, j;
   int steps = (int)(size*2.25);
   int smooth = 5;
   int diagSizeA = (int)(size * 1.0);
   int diagSizeB = (int)(size * 1.3);
   servoDown();
   moveDiagonal(motor1, motor2, RIGHT, UP, diagSizeA, velocity);
   // Dibujar dos semicirculos
   delay(50);
   for (i = -15; i < ((steps / 2)); i+=smooth) {
      float angle = (PI * i) / (steps / 2);
      int x = (smooth) * sin(angle);
      int y = (smooth) * cos(angle);
      // Mover en el eje X
      moveX(motor1, motor2, x > 0 ? LEFT : RIGHT, fabs(x), velocity);
      // Mover en el eje Y
      moveY(motor1, motor2, y > 0 ? UP : DOWN, fabs(y), velocity);
   }
   delay(50);
   for (i = 0; i < ((steps / 2)+10); i+=smooth) {
      float angle = (PI * i) / (steps / 2);
      int x = (smooth) * sin(angle);
      int y = (smooth) * cos(angle);
      // Mover en el eje X
      moveX(motor1, motor2, x > 0 ? LEFT : RIGHT, fabs(x), velocity);
      // Mover en el eje Y
      moveY(motor1, motor2, y > 0 ? UP : DOWN, fabs(y), velocity);
   }
   delay(50);
   moveDiagonal(motor1, motor2, RIGHT, DOWN, diagSizeB, velocity);
   servoUp();
}

void showError(){
   int i;
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

// Funcion para dibujar una figura de un tamaño variable
void drawFigureVar( MotorPins_t motor1, MotorPins_t motor2, char c, int size ){
   switch(c)
   {
   case 'C':
      drawSquare(motor1, motor2, size, SPEED);
      break;
   case 'O':
      drawCircle(motor1, motor2, size, SPEED);
      break;
   case 'X':
      drawX(motor1, motor2, size, SPEED);
      break;
   case 'T':
      drawTriangle(motor1, motor2, size, SPEED);
      break;
   case 'S':
      drawStar(motor1, motor2, size, SPEED);
      break;
   case 'H':
      drawHeart(motor1, motor2, size, SPEED);
      break;
   default :
      showError();
      break;
   }
}

// Funcion para mostrar una figura de tamaño variable
void showFigureVar( MotorPins_t motor1, MotorPins_t motor2, char c, int size ){
   moveTo( motor1, motor2, 0, -60 );
   drawFigureVar(motor1, motor2, c, size);
   moveTo( motor1, motor2, 0, 0 );
}

// Funcion para dibujar una figura de tamaño estandar
void drawFigure( MotorPins_t motor1, MotorPins_t motor2, char c ){
   drawFigureVar(motor1, motor2, c, 80);
}

// Funcion para dibujar una figura de tamaño estandar y mostrarla
void showFigure( MotorPins_t motor1, MotorPins_t motor2, char c ){
   showFigureVar(motor1, motor2, c, 80);
}

// Funcion para dibujar un tablero de TA-TE-TI
void drawBoard( MotorPins_t motor1, MotorPins_t motor2 ){
   moveTo( motor1, motor2, -60, -40);
   servoDown();
   moveTo( motor1, motor2, 60, -40);
   servoUp();
   moveTo( motor1, motor2, 60, -80);
   servoDown();
   moveTo( motor1, motor2, -60, -80);
   servoUp();
   moveTo( motor1, motor2, -20, 0);
   servoDown();
   moveTo( motor1, motor2, -20, -120);
   servoUp();
   moveTo( motor1, motor2, 20, -120);
   servoDown();
   moveTo( motor1, motor2, 20, 0);
   servoUp();
   moveTo( motor1, motor2, 0, 0);
}

// Funcion para moverse dentro del tablero de TA-TE-TI
void moveBoard( MotorPins_t motor1, MotorPins_t motor2, char x, char y){
   int posX = ((x - 0x30) * 40) - 95;
   int posY = ((y - 0x30) * -40) + 5;
   moveTo( motor1, motor2, posX, posY );
}

// Funcion para leer los botones y dibujar una figura
void readTECdrawFigure( MotorPins_t motor1, MotorPins_t motor2 ){
   if (!gpioRead( TEC1 )){
      gpioWrite( LED1, ON );
      showFigure(motor1, motor2, 'T');
      gpioWrite( LED1, OFF );
   }
   if (!gpioRead( TEC2 )){
      gpioWrite( LED2, ON );
      showFigureVar(motor1, motor2, 'T', 40);
      gpioWrite( LED2, OFF );

   }
   if (!gpioRead( TEC3 )){
      gpioWrite( LED3, ON );
      showFigureVar(motor1, motor2, 'T', 100);
      gpioWrite( LED3, OFF );
   }
   if (!gpioRead( TEC4 )){
      gpioWrite( LEDR, ON );
      gpioWrite( LEDG, ON );
      showFigureVar(motor1, motor2, 'T', 160);
      //drawBoard(motor1, motor2);
      gpioWrite( LEDR, OFF );
      gpioWrite( LEDG, OFF );
   }
}

// Funcion para leer los botones y dibujar una figura
void readTECdrawAngle( MotorPins_t motor1, MotorPins_t motor2 ){
   if (!gpioRead( TEC1 )){
      gpioWrite( LED1, ON );
      moveTo( motor1, motor2, 0, -60 );
      servoDown();
      moveAngle(motor1, motor2, 0, 80, SPEED);
      servoUp();
      moveAngle(motor1, motor2, 180, 80, SPEED);
      moveTo( motor1, motor2, 0, 0 );
      gpioWrite( LED1, OFF );
   }
   if (!gpioRead( TEC2 )){
      gpioWrite( LED2, ON );
      moveTo( motor1, motor2, 0, -60 );
      servoDown();
      moveAngle(motor1, motor2, 45, 80, SPEED);
      servoUp();
      moveAngle(motor1, motor2, 225, 80, SPEED);
      moveTo( motor1, motor2, 0, 0 );
      gpioWrite( LED2, OFF );

   }
   if (!gpioRead( TEC3 )){
      gpioWrite( LED3, ON );
      moveTo( motor1, motor2, 0, -60 );
      servoDown();
      moveAngle(motor1, motor2, 60, 80, SPEED);
      servoUp();
      moveAngle(motor1, motor2, 240, 80, SPEED);
      moveTo( motor1, motor2, 0, 0 );
      gpioWrite( LED3, OFF );
   }
   if (!gpioRead( TEC4 )){
      gpioWrite( LEDR, ON );
      gpioWrite( LEDG, ON );
      moveTo( motor1, motor2, 0, -60 );
      servoDown();
      moveAngle(motor1, motor2, 120, 80, SPEED);
      servoUp();
      moveAngle(motor1, motor2, 300, 80, SPEED);
      moveTo( motor1, motor2, 0, 0 );
      gpioWrite( LEDR, OFF );
      gpioWrite( LEDG, OFF );
   }
}

void conectAPPold(MotorPins_t motor1, MotorPins_t motor2){
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

void conectAPPTHIS(MotorPins_t motor1, MotorPins_t motor2){
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
                     //uartWriteByte(UART_USB, receivedChar);
                     delay(200);
                  }
               }
            }
         }
      }
      gpioWrite( LEDB, ON );
   }
}

void drawInBoard(MotorPins_t motor1, MotorPins_t motor2, char figure, int x, char y){
   moveBoard(motor1, motor2, x, y);
   if ((figure == 'X') || (figure == 'C') || (figure == 'S') || (figure == 'T')){
      drawFigureVar(motor1, motor2, figure, 120);
   }else{
      moveX(motor1, motor2, RIGHT, 80, SPEED);
      drawFigureVar(motor1, motor2, figure, 120);
      moveX(motor1, motor2, LEFT, 80, SPEED);
   }
   moveTo( motor1, motor2, 0, 0 );
}

void modeTATETI(MotorPins_t motor1, MotorPins_t motor2, char * buffer, int bufferIndex){
   int i;
   switch (buffer[1])
   {
   case 'T':
      drawBoard(motor1, motor2);
      break;
   case 'D':
      drawInBoard(motor1, motor2, buffer[2], buffer[3], buffer[4]);
      break;
   case 'L':

      break;
   default:
      showError();
      break;
   }
   
}

int receiveString(char * buffer, int * bufferIndex, int bufferSize) {
   char receivedChar;
   int indice = *bufferIndex;
   if ((uartReadByte(UART_232, (uint8_t*)&receivedChar)) && (receivedChar == '*')) {
      while ((uartReadByte(UART_232, (uint8_t*)&receivedChar)) && ((indice < (bufferSize - 1)))) {
         buffer[indice++] = receivedChar;
         if (receivedChar == '.') {
            break;
         }
      }
      // Null-terminate the buffer
      buffer[indice] = '\0';
      // Process the buffer
      if (buffer[indice - 2] == '*' && buffer[indice - 1] == '.') {
         indice -= 2;
         buffer[indice] = '\0';
         *bufferIndex = indice;
         return 1;
      }else{
         return 0;
      }
   }
}

void conectAPP(MotorPins_t motor1, MotorPins_t motor2) {
   char receivedChar;
   char buffer[BUFFER_SIZE];
   int bufferIndex = 0;

   // Read the initial byte
   if (uartReadByte(UART_232, (uint8_t*)&receivedChar)) {
      // Handle non-period initial byte
      if (receivedChar != '.') {
         gpioWrite(LEDR, ON);
         gpioWrite(LEDG, ON);
         // Read until a period is received
         while (uartReadByte(UART_232, (uint8_t*)&receivedChar) && (receivedChar != '.')) {
            // Do nothing, just wait for the period
         }
         gpioWrite(LEDR, OFF);
         gpioWrite(LEDG, OFF);
      }
      
      // Clear the buffer
      memset(buffer, 0, BUFFER_SIZE);
      gpioWrite(LEDB, OFF);

      // Handle command sequence
      if (receivedChar == '.') {
         if (receiveString(buffer, &bufferIndex, BUFFER_SIZE)){
            switch (buffer[0])
            {
            case 'D':
               showFigure(motor1, motor2, buffer[1]);
               break;
            case 'J':
               modeTATETI(motor1, motor2, buffer, bufferIndex);
               break;
            default:
               showError();
               break;
            }
            uartWriteByte(UART_232, 'F');
            delay(200);
         }
      }

      gpioWrite(LEDB, ON);
   }
}

void readTECandTATETI(MotorPins_t motor1, MotorPins_t motor2){
   if (!gpioRead( TEC1 )){
      gpioWrite( LED1, ON );
      uartWriteString(UART_232, ".*JT*.");
      delay(200);
      gpioWrite( LED1, OFF );
   }
   if (!gpioRead( TEC2 )){
      gpioWrite( LED2, ON );
      uartWriteString(UART_232, ".*JDO21*.");
      delay(200);
      gpioWrite( LED2, OFF );
   }
   if (!gpioRead( TEC3 )){
      gpioWrite( LED3, ON );
      uartWriteString(UART_232, ".*JDO32*.");
      delay(200);
      gpioWrite( LED3, OFF );
   }
   if (!gpioRead( TEC4 )){
      gpioWrite( LEDR, ON );
      gpioWrite( LEDG, ON );
      uartWriteString(UART_232, ".*JDX33*.");
      delay(200);
      gpioWrite( LEDR, OFF );
      gpioWrite( LEDG, OFF );
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
      //readTECdrawFigure( motor1, motor2 );
      //readTECandTATETI( motor1, motor2 );
      //readTECdrawAngle( motor1, motor2 );

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