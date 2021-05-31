#include "Medidor.h"
#include <MHZ19_uart.h>     
#include <LiquidCrystal_I2C.h>

MHZ19_uart sensor;
LiquidCrystal_I2C display(0x27,16,2);  

Medidor::Medidor(void) {
  rx_pin = 5;
  tx_pin = 6;
  ledR_pin = 4;
  ledG_pin = 3;
  ledB_pin = 2;
  buzzer_pin = 7;
  pulsador_pin = 8;
  numero_de_serie = "0000";
}

void Medidor::iniciar() {
  pinMode(ledR_pin, OUTPUT);
  pinMode(ledG_pin, OUTPUT);
  pinMode(ledB_pin, OUTPUT);
  rgb('a');
  pinMode(buzzer_pin, OUTPUT);
  pinMode(pulsador_pin, INPUT_PULLUP);
  Serial.begin(9600); 
  display.begin();          
  display.clear();
  display.backlight();
  alarma(1, 250, 'b');
  Serial.print("N° de serie " + numero_de_serie + "\n");
  Serial.print("INICIANDO \n");
  displayPrint(0, 0, "N/S: " + numero_de_serie);
  displayPrint(0, 1, "INICIANDO");       
  logoUNAHUR();
  delay(10000);
  calentar();
}

void Medidor::calentar() {
  display.clear();
  sensor.begin(rx_pin, tx_pin); 
  sensor.setAutoCalibration(false);
  Serial.print("Calentando, espere 1 minuto \n");
  displayPrint(0, 0, "Calentando");       
  displayPrint(0, 1, "Espere 1 minuto"); 
  delay(60000); 
  display.clear();
  alarma(3, 250,'g');
}

void Medidor::verificarEstadoPulsador() {
  if (digitalRead(pulsador_pin) == LOW) {
    alarma(1, 250, 'b');
    calibrar();
  }
}

void Medidor::presentarMedidor() {
  Serial.print("AireNuevo UNAHUR \n"); 
  Serial.print("MEDIDOR de CO2 \n");  
  display.clear();
  displayPrint(0, 0, "AireNuevo UNAHUR");     
  displayPrint(0, 1, "MEDIDOR de CO2");       
  delay(5000);
}

void Medidor::sensarCO2() {
  display.clear();
  displayPrint(0, 0, "Aire Nuevo");
  while(sensor.getPPM() >= 1200) {  
    alarmaCO2(1, 250);                             
    imprimirCO2(sensor.getPPM());
  }
  int co2ppm = sensor.getPPM();
  imprimirCO2(co2ppm);
  if(co2ppm >= 1000){
    alarmaCO2(4, 500);             
  }
  else if(co2ppm >= 800) {
    alarmaCO2(2, 1000);
  }
  else if(co2ppm >= 600) {
    rgb('y');
  }
  else if(co2ppm < 800) {
    rgb('g');
  }
  scrollAireNuevo();
  delay(5000);  
}

void Medidor::displayPrint(int posicion, int linea, String texto) {
  display.setCursor(posicion, linea);
  display.print(texto);
}

void Medidor::imprimirCO2(int co2ppm) {
  Serial.print("CO2: " + String(co2ppm) + "ppm \n");     
  displayPrint(0, 1, "          ");                      
  displayPrint(0, 1, "CO2: " + String(co2ppm) + "ppm");   
  logoUNAHUR();                                           
}

void Medidor::logoUNAHUR() {
  // Primero creamos todos los caracteres necesarios para dibujar el logo
  byte UNAHUR1[] = { 
    B11100,
    B11110,
    B11111,
    B11111,
    B11111,
    B11111,
    B01111,
    B00111
  };
  byte UNAHUR2[] = { 
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
  };
  byte UNAHUR3[] = { 
    B00111,
    B01111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11110,
    B11100
  };
  display.createChar(0, UNAHUR1); 
  display.createChar(1, UNAHUR2); 
  display.createChar(2, UNAHUR3); 
  display.setCursor(13, 0);       
  display.write(0);               
  display.setCursor(14, 0);      
  display.write(1);
  display.setCursor(15, 0);       
  display.write(2);
  display.setCursor(13, 1);      
  display.write(2);
  display.setCursor(14, 1);      
  display.write(1);
  display.setCursor(15, 1);       
  display.write(0);
}

void Medidor::scrollingText(uint8_t scrolled_by) {
  for (uint8_t i=0;i<11;i++) {
    display.setCursor(i,0);
    if (scrolled_by>=11) scrolled_by=0;                                 // Si el indice es mayor que el array de caracteres y ya imprimió el espacio vacío, vuelve al inicio
    if (scrolled_by<10) display.print(str_to_print[scrolled_by]);       // Si el indice está dentro del array de caracteres, imprime el caracter
    else display.print(' ');                                            // Si el indice es mayor que el array de caracteres, imprime un espacio vacio para que el mensaje no se vea como "Aire NuevoAire Nuevo"
    scrolled_by++;
  }
}

void Medidor::scrollAireNuevo() {
  for (uint8_t i=0;i<STR_LEN;i++) {
    scrollingText(i);
    delay(500);
  }
}

void Medidor::calibrar()
{
  const long segundosEspera = 1800;         // Cantidad de segundos a esperar, son 30 minutos pero en segundos para mayor comodidad
  long segundosPasados = 0;                 // Cuenta segundos pasados
  // Print por serial
  Serial.print("COMIENZA CALIBRACION \n");  // Notificamos que empezó el proceso de calibración
  // Print por display
  display.clear();                          // Limpiamos pantalla e imprimimos el mensaje por display
  displayPrint(0, 0, "COMIENZA");
  displayPrint(0, 1, "CALIBRACION");
  delay(10000);                             // Espera 10 segundos
 
  while(segundosPasados <= segundosEspera) {                      // Espera media hora
    if (++segundosPasados % 60 == 0) {                            // Si los segundos pasados son múltiplo de 60
    // Print por serial                                           // Imprimimos por serial para no llegar el monitor de mensajes innecesarios
      Serial.print(String(segundosPasados / 60) + " minutos \n");  
    }
    // Print por pantalla
    display.clear();                                              // Borra pantalla  
    displayPrint(0, 0, "CALIBRANDO");                             // Notificamos que se sigue calibrando
    displayPrint(0, 1, String(segundosPasados / 60) + " minutos");// Escribimos los minutos pasados
    delay(1000); // Espera 1 segundo
    }
  sensor.calibrateZero();                     // Ejecuta la primer calibración
  // Print por serial
  Serial.print("PRIMERA CALIBRACION \n");     // Avisamos por serial que ya se ejecuto la primer calibración
  // Print por display
  display.clear();                            // Limpio pantalla
  displayPrint(0, 0, "PRIMERA");              // Avisamos por display que ya se ejecutó la primer calibración
  displayPrint(0, 1, "CALIBRACION");     
  alarma(1, 250, 'b');                        // Avisamos que terminó la primera calibración y mantenemos el led en azul
  delay(60000);                               // Espera 1 minuto
  sensor.calibrateZero();                     // Calibra por segunda vez por las dudas
  // Print por serial
  Serial.print("SEGUNDA CALIBRACION \n");
  // Print por display
  display.clear();                            // Limpio pantalla     
  displayPrint(0, 0, "SEGUNDA");        
  displayPrint(0, 1, "CALIBRACION");    
  alarma(1, 250, 'b');                        // Avisamos que terminó la segunda calibración y mantenemos el led en azul
  delay(10000); // Espera 10 segundos
  // Print por serial
  Serial.print("CALIBRACION TERMINADA \n");
  // Print por display
  display.clear();                            // Borra pantalla  
  displayPrint(0, 0, "CALIBRACION");       
  displayPrint(0, 1, "TERMINADA");    
  alarma(5, 250, 'g');                        // Avisamos que finalizó el proceso de calibración entero y ponemos el led en verde para mostrar que vuelve a su funcionamiento normal
  delay(10000); // Espera 10 segundos 
}

void Medidor::rgb(char color) {
  // Switch funciona solo con int o char
  // Como el led es anodo comun, pasamos los valos de manera inversa
  switch (color) {
    case 'r': //red
      digitalWrite(ledR_pin, LOW);
      digitalWrite(ledG_pin, HIGH);
      digitalWrite(ledB_pin, HIGH);
      break;
    case 'g': //green
      digitalWrite(ledR_pin, HIGH);
      digitalWrite(ledG_pin, LOW);
      digitalWrite(ledB_pin, HIGH);
      break;
    case 'y': //yellow
      digitalWrite(ledR_pin, LOW);
      digitalWrite(ledG_pin, LOW);
      digitalWrite(ledB_pin, HIGH);
      break;
    case 'b': //blue
      digitalWrite(ledR_pin, HIGH);
      digitalWrite(ledG_pin, HIGH);
      digitalWrite(ledB_pin, LOW);
      break;
    case 'a': //apagado
      digitalWrite(ledR_pin, HIGH);
      digitalWrite(ledG_pin, HIGH);
      digitalWrite(ledB_pin, HIGH);
      break;
  }
}

void Medidor::alarma(int veces, int duracionNota, char color) {
  rgb(color);                                 // Pongo el rgb en el color deseado
  for(int i=0; i<veces; i++) {
    tone(buzzer_pin, NOTE_C7, duracionNota);   // Hago sonar el buzzer, la nota c7 es la que más fuerte suena
    delay(duracionNota);                      // Espero lo que dura la nota
    noTone(buzzer_pin);                        // Silencio el buzzer
    delay(duracionNota);                      // Delay entre alarmas
  }
}

void Medidor::alarmaCO2(int veces, int duracionNota) {
  for(int i=0; i<veces; i++) {
    if(i<veces-1 or veces==1) {
      rgb('r');                                 // Prendo el led
      tone(buzzer_pin, NOTE_C7, duracionNota);   // Hago sonar el buzzer, la nota c7 es la que más fuerte suena
      delay(duracionNota);                      // Espero lo que dura la nota
      noTone(buzzer_pin);                        // Silencio el buzzer
      rgb('a');                                 // Apago el led
      delay(duracionNota);                      // Delay entre alarmas
    }
    else {
      rgb('r');
      tone(buzzer_pin, NOTE_C7, duracionNota);   // Hago sonar el buzzer, la nota c7 es la que más fuerte suena
      delay(duracionNota);                      // Espero lo que dura la nota
      noTone(buzzer_pin);                        // Silencio el buzzer
      delay(duracionNota);                      // Delay entre alarmas
    }
  }
}
