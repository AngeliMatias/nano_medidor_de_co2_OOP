/*----------------------------------------------------------
    Medidor de CO2 en Arduino Nano utilizando OOP
  ----------------------------------------------------------*/
#include "Medidor.h"
Medidor medidor;
long loops = 0;

void setup() {
  medidor.iniciar();
}

void loop() {
  medidor.verificarEstadoPulsador();
  if(loops % 30 == 0) {
    medidor.presentarMedidor();
    loops = 0;
  } 
  medidor.sensarCO2();
  loops++;
}
