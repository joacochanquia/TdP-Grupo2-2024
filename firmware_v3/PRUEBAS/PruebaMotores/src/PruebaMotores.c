#include "sapi.h"  // Biblioteca de la EDU-CIAA

// Definir los pines para control del motor paso a paso
#define STEP_PIN1 GPIO7  // GPIO1 para STEP
#define DIR_PIN1 GPIO8  // GPIO3 para Dirección (DIR)
#define EN_PIN1 GPIO5
#define STEP_PIN2 GPIO1  // GPIO1 para STEP
#define DIR_PIN2 GPIO3  // GPIO3 para Dirección (DIR)
#define EN_PIN2 LCD1

#define SMOOTHNESS 20

//Definir el sentido de giro de los motores
#define H true
#define AH false

typedef struct {
   int EN;
   int DIR;
   int STEP;
} MotorPins_t;
   

// Función para inicializar los pines GPIO
void MotorInit(MotorPins_t motor) {
   
   gpioConfig(motor.EN, GPIO_OUTPUT);
   gpioConfig(motor.DIR, GPIO_OUTPUT);
   gpioConfig(motor.STEP, GPIO_OUTPUT);
   
   gpioWrite(motor.EN, ON);
   
}

// Función para realizar un paso del motor
void Motor_Step(MotorPins_t motor, bool direction) {
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

//Función para realizar un paso con dos motores
void Two_Motor_Step(MotorPins_t motor1, bool direction1, MotorPins_t motor2, bool direction2) {
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

void Motor_Run(MotorPins_t motor, bool direction, int stepCount, int velocity){
   gpioWrite(motor.EN, OFF);
   delay(1);
   for (int i = 0; i < stepCount; i++) {
      Motor_Step(motor, direction);
      delay(velocity);
   }
   gpioWrite(motor.EN, ON);
}

void Two_Motor_Run(MotorPins_t motor1, bool direction1, MotorPins_t motor2, bool direction2, int stepCount, int velocity){
   gpioWrite(motor1.EN, OFF);
   gpioWrite(motor2.EN, OFF);
   delay(1);
   for (int i = 0; i < stepCount; i++) {
      Two_Motor_Step(motor1, direction1, motor2, direction2);
      delay(velocity);
   }
   gpioWrite(motor1.EN, ON);
   gpioWrite(motor2.EN, ON);
}

void MoveX(MotorPins_t motor1, MotorPins_t motor2, bool sentido, int stepCount, int velocity){
   Two_Motor_Run(motor1, sentido, motor2, sentido, stepCount, velocity);
}

void MoveY(MotorPins_t motor1, MotorPins_t motor2, bool sentido, int stepCount, int velocity){
   Two_Motor_Run(motor1, sentido, motor2, !sentido, stepCount, velocity);
}

// Función principal del programa
int main(void) {
   boardConfig();
   
   MotorPins_t motor1;
   motor1.EN = EN_PIN1;
   motor1.DIR = DIR_PIN1;
   motor1.STEP = STEP_PIN1;
   MotorInit(motor1);
   
   MotorPins_t motor2;
   motor2.EN = EN_PIN2;
   motor2.DIR = DIR_PIN2;
   motor2.STEP = STEP_PIN2;
   MotorInit(motor2);
   
    // Bucle infinito para controlar el motor
    while (1) {
       /* Prendo el led azul */
      gpioWrite( LEDB, ON );

      delay(500);

      /* Apago el led azul */
      gpioWrite( LEDB, OFF );
       
      delay(500);
       
        
         
         gpioWrite( LEDR, ON );
        // Realizar 200 pasos en sentido horario motor 1
         Motor_Run(motor1, H, 200, 5);
         gpioWrite( LEDR, OFF );
       
         // Pausa antes de cambiar de dirección
         delay(1000);
        
         gpioWrite( LEDG, ON );
        // Realizar 200 pasos en sentido horario motor 2
        Motor_Run(motor2, H, 200, 5);
         gpioWrite( LEDG, OFF );
        
        // Pausa antes de repetir el ciclo
         delay(1000);
         
         gpioWrite( LEDR, ON );
        // Realizar 200 pasos en sentido antihorario motor 1
         Motor_Run(motor1, AH, 200, 5);
         gpioWrite( LEDR, OFF );
       
         // Pausa antes de cambiar de dirección
         delay(1000);
        
         gpioWrite( LEDG, ON );
        // Realizar 200 pasos en sentido antihorario motor 2
        Motor_Run(motor2, AH, 200, 5);
         gpioWrite( LEDG, OFF );
        
        // Pausa antes de repetir el ciclo
         delay(1000);
         
         gpioWrite( LED1, ON );
         Two_Motor_Run(motor1, AH, motor2, H, 200, 5);
         gpioWrite( LED1, OFF );
         
         delay(1000);
         
    }

    return 0;
}