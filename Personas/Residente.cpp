#include "Residente.h"

Residente :: Residente() {
  pid_t pid = getpid();
  srand(pid);
  int numero = (rand() % 9) + 1;
  for(long int i = 0; i < CANT_DIGITOS_DOCU - 1; i++) {
    numero = (numero * 10) + (rand() % 9) + 1;
  }

  this->numeroDocumento = numero;

  if ((rand() % 100) < 50) {
    this->genero = "masculino";
  }
  else {
    this->genero = "femenino";
  }
  char docBuffer[sizeof(this->numeroDocumento)];
  char pidBuffer[sizeof(pid)];
  sprintf(docBuffer, "%d", this->numeroDocumento);
  sprintf(pidBuffer, "%d", pid);
  Logger :: getInstance()->registrar(string(pidBuffer) + ": Residente creado con documento: " + string(docBuffer) + " , genero " + this->genero);
}

Residente :: ~Residente() {
}

int Residente :: obtenerNumeroDocumento() {
  return this->numeroDocumento;
}

string Residente :: obtenerGenero() {
  return this->genero;
}
