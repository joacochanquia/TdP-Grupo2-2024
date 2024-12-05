#include "sapi.h"  // Biblioteca de la EDU-CIAA

// Definir los pines para control del motor paso a paso
#define STEP_PIN1 GPIO7  // GPIO1 para STEP
#define DIR_PIN1 GPIO8  // GPIO3 para Direcci�n (DIR)
#define EN_PIN1 GPIO5
#define STEP_PIN2 GPIO1  // GPIO1 para STEP
#define DIR_PIN2 GPIO3  // GPIO3 para Direcci�n (DIR)
#define EN_PIN2 LCD1

#define SMOOTHNESS 10

//Definir el sentido de giro de los motores
#define H true
#define AH false
#define UP true
#define DOWN false
#define RIGHT true
#define LEFT false

#define SERVO_N   SERVO3

typedef struct {
   int EN;
   int DIR;
   int STEP;
} MotorPins_t;
   

// Funcion de inicializacion de los motores
void MotorInit(MotorPins_t * motor, int enPin, int dirPin, int stepPin) {
   
   motor->EN = enPin;
   motor->DIR = dirPin;
   motor->STEP = stepPin;
   
   gpioConfig(motor->EN, GPIO_OUTPUT);
   gpioConfig(motor->DIR, GPIO_OUTPUT);
   gpioConfig(motor->STEP, GPIO_OUTPUT);
   
   
   gpioWrite(motor->EN, ON);
   
}

// Funcion para realizar un paso en un motor
void MotorStep(MotorPins_t motor, bool direction) {
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

//Funcion para realizar un paso en dos motores en simultaneo
void TwoMotorStep(MotorPins_t motor1, bool direction1, MotorPins_t motor2, bool direction2) {
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

// Funcion para realizar varios pasos en un motor
void MotorRun(MotorPins_t motor, bool direction, int stepCount, int velocity){
   gpioWrite(motor.EN, OFF);
   delay(1);
   for (int i = 0; i < stepCount; i++) {
      MotorStep(motor, direction);
      delay(velocity);
   }
   gpioWrite(motor.EN, ON);
}

// Funcion para realizar varios pasos en dos motores en simultaneo
void TwoMotorRun(MotorPins_t motor1, bool direction1, MotorPins_t motor2, bool direction2, int stepCount, int velocity){
   gpioWrite(motor1.EN, OFF);
   gpioWrite(motor2.EN, OFF);
   delay(1);
   for (int i = 0; i < stepCount; i++) {
      TwoMotorStep(motor1, direction1, motor2, direction2);
      delay(velocity);
   }
   gpioWrite(motor1.EN, ON);
   gpioWrite(motor2.EN, ON);
}

// Funcion para mover en el eje X
// Ambos motores deben moverse en el mismo sentido para realizar el movimiento
void MoveX(MotorPins_t motor1, MotorPins_t motor2, bool sentido, int stepCount, int velocity){
   TwoMotorRun(motor1, sentido, motor2, sentido, stepCount, velocity);
}

// Funcion para mover en el eje Y
// Ambos motores deben moverse en sentidos opuestos para realizar el movimiento
void MoveY(MotorPins_t motor1, MotorPins_t motor2, bool sentido, int stepCount, int velocity){
   TwoMotorRun(motor1, sentido, motor2, !sentido, stepCount, velocity);
}

// Funcion para dibujar un cuadrado
void drawSquare(MotorPins_t motor1, MotorPins_t motor2, int side, int velocity){
   MoveX(motor1, motor2, RIGHT, side, velocity);
   MoveY(motor1, motor2, DOWN, side, velocity);
   MoveX(motor1, motor2, LEFT, side, velocity);
   MoveY(motor1, motor2, UP, side, velocity);
}

// Funcion para dibujar un rectangulo
void drawRectangle(MotorPins_t motor1, MotorPins_t motor2, int side1, int side2, int velocity){
   MoveX(motor1, motor2, RIGHT, side1, velocity);
   MoveY(motor1, motor2, DOWN, side2, velocity);
   MoveX(motor1, motor2, LEFT, side1, velocity);
   MoveY(motor1, motor2, UP, side2, velocity);
}
 

// Funcion principal del programa
int main(void) {
   boardConfig();
   
   bool_t valor = 0;

   uint8_t servoAngle = 0; // 0 a 180 grados

   // Configurar Servo
   valor = servoConfig( 0, SERVO_ENABLE );

   valor = servoConfig( SERVO_N, SERVO_ENABLE_OUTPUT );

   // Usar Servo
   valor = servoWrite( SERVO_N, servoAngle );
   servoAngle = servoRead( SERVO_N );
   
   MotorPins_t motor1;
   MotorInit(&motor1, EN_PIN1, DIR_PIN1, STEP_PIN1);
   
   MotorPins_t motor2;
   MotorInit(&motor2, EN_PIN2, DIR_PIN2, STEP_PIN2);
   
    // Bucle infinito para controlar el motor
    while (1) {
      /* Prendo el led azul */
      gpioWrite( LEDB, ON );

      delay(500);

      /* Apago el led azul */
      gpioWrite( LEDB, OFF );

      delay(500);
       
       
      gpioWrite( LED2, ON );
      drawSquare(motor1, motor2, 200, 5);
      gpioWrite( LED2, OFF );
       
/*
      gpioWrite( LEDR, ON );
      gpioWrite( LED3, ON );
      // Realizar 200 pasos en sentido horario motor 1
      MotorRun(motor1, H, 200, 5);
      gpioWrite( LEDR, OFF );
      gpioWrite( LED3, OFF );

      // Pausa antes de cambiar de direcci�n
      delay(1000);

      gpioWrite( LEDG, ON );
      gpioWrite( LED3, ON );
      // Realizar 200 pasos en sentido horario motor 2
      MotorRun(motor2, H, 200, 5);
      gpioWrite( LEDG, OFF );
      gpioWrite( LED3, OFF );

      // Pausa antes de repetir el ciclo
      delay(1000);

      gpioWrite( LEDR, ON );
      gpioWrite( LED1, ON );
      // Realizar 200 pasos en sentido antihorario motor 1
      MotorRun(motor1, AH, 200, 5);
      gpioWrite( LEDR, OFF );
      gpioWrite( LED1, OFF );

      // Pausa antes de cambiar de direcci�n
      delay(1000);

      gpioWrite( LEDG, ON );
      gpioWrite( LED1, ON );
      // Realizar 200 pasos en sentido antihorario motor 2
      MotorRun(motor2, AH, 200, 5);
      gpioWrite( LEDG, OFF );
      gpioWrite( LED1, OFF );

      // Pausa antes de repetir el ciclo
      delay(1000);

      gpioWrite( LED2, ON );
      TwoMotorRun(motor1, H, motor2, H, 200, 5);
      gpioWrite( LED2, OFF );
*/
      delay(1000);

    }

    return 0;
}