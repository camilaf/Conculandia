#include "Turista.h"

Turista :: Turista() {
  string ojos[] = {"marrones", "azules", "verdes", "grises"};
  string pelo[] = {"rubio", "negro", "gris", "marron oscuro", "marron claro", "rojo", "rosa", "azul", "violeta"};
  string piel[] = {"clara", "media", "oscura"};
  string elementosCabeza[] = {"sombrero", "gorra", "pelo suelto", "trenza", "cola de caballo", "rastas", "lentes"};
  string genero[] = {"femenino", "masculino"};

  pid_t pid = getpid();
  srand(pid);
  this->rasgos[0] = ojos[rand() % CANT_COLOR_OJOS];
  this->rasgos[1] = pelo[rand() % CANT_COLOR_PELO];
  this->rasgos[2] = piel[rand() % CANT_TONO_PIEL];
  this->rasgos[3] = elementosCabeza[rand() % CANT_ELEM_CABEZA];
  this->rasgos[4] = genero[rand() % CANT_GENERO];

  int numero = (rand() % 9) + 1;
  for(long int i = 0; i < CANT_DIGITOS_PAS - 1; i++) {
    numero = (numero * 10) + (rand() % 9) + 1;
  }
  this->numeroPasaporte = numero;

  char pasBuffer[sizeof(this->numeroPasaporte)];
  char pidBuffer[sizeof(pid)];
  sprintf(pasBuffer, "%d", this->numeroPasaporte);
  sprintf(pidBuffer, "%d", pid);
  Logger :: getInstance()->registrar(string(pidBuffer) + ": Turista creado con pasaporte: " + string(pasBuffer) + " , ojos " + this->rasgos[0] + " , pelo " + this->rasgos[1] + " , piel " + this->rasgos[2] + " , elemento particular: " + this->rasgos[3] + " , genero " + this->rasgos[4]);
}

Turista :: ~Turista() {
}

int Turista :: obtenerNumeroPasaporte() {
  return this->numeroPasaporte;
}

string Turista :: obtenerRasgos() {
  string colorOjos = this->rasgos[0];
  string colorPelo = this->rasgos[1];
  string tonoPiel = this->rasgos[2];
  string elemento = this->rasgos[3];
  string genero = this->rasgos[4];
  string rasgos = "ojos:" + colorOjos + "-pelo:" + colorPelo + "-piel:" + tonoPiel + "-elemento:" + elemento + "-genero:" + genero + "-";
  return rasgos;
}
