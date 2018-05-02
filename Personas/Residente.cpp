#include "Residente.h"

Residente :: Residente() {
  this->asignarDocumento();
  this->asignarGenero();

  pid_t pid = getpid();
  char docBuffer[sizeof(this->numeroDocumento)];
  char pidBuffer[sizeof(pid)];
  sprintf(docBuffer, "%d", this->numeroDocumento);
  sprintf(pidBuffer, "%d", pid);
  Logger :: getInstance()->registrar(string(pidBuffer) + ": Residente creado con documento: " + string(docBuffer) + " , genero " + this->genero);
}

Residente :: ~Residente() {
}

/* Metodo que devuelve el numero de documento del residente. */
int Residente :: obtenerNumeroDocumento() {
  return this->numeroDocumento;
}

/* Metodo que devuelve el genero del residente. */
string Residente :: obtenerGenero() {
  return this->genero;
}

/* Metodo que asigna el numero de documento del residente.
  Se toma al documento como un numero de CANT_DIGITOS_DOCU digitos del 1 al 9. */
void Residente :: asignarDocumento() {
  pid_t pid = getpid();
  srand(pid);
  int numero = (rand() % 9) + 1;
  for(long int i = 0; i < CANT_DIGITOS_DOCU - 1; i++) {
    numero = (numero * 10) + (rand() % 9) + 1;
  }
  this->numeroDocumento = numero;
}

/* Metodo que asigna el genero del residente. */
void Residente :: asignarGenero() {
  pid_t pid = getpid();
  srand(pid);
  if ((rand() % 100) < 50) {
    this->genero = "masculino";
  }
  else {
    this->genero = "femenino";
  }
}
