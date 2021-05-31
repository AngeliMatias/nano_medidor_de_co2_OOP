/*----------------------------------------------------------
    Librería para crear objeto Medidor
  ----------------------------------------------------------*/
#ifndef MEDIDOR_H
#define MEDIDOR_H
#include <Arduino.h>

#define STR_LEN 12
#define NOTE_C7  2093

class Medidor {
private:
  byte rx_pin;
  byte tx_pin;
  byte ledR_pin;
  byte ledG_pin;
  byte ledB_pin;
  byte buzzer_pin;
  byte pulsador_pin;
  String numero_de_serie;
  
  char str_to_print[STR_LEN]={'A','i','r','e',' ','N','u','e','v','o'}; 
  void displayPrint(int posicion, int linea, String texto);
  void scrollingText(uint8_t scrolled_by);
  
public:
  Medidor(void);
  void imprimirCO2(int co2ppm);
  void logoUNAHUR();
  void scrollAireNuevo();
  void calibrar();
  void rgb(char color);
  void sonarAlarma(int duracionNota);
  void alarma(int veces, int duracionNota, char color);
  void alarmaCO2(int veces, int duracionNota);
  void iniciar();
  void calentar();
  void verificarEstadoPulsador();
  void presentarMedidor();
  void sensarCO2();
};


#endif
