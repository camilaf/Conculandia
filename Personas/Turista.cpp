#include "Turista.h"

Turista :: Turista() {
  this->asignarRasgos();
  this->asignarPasaporte();

  pid_t pid = getpid();
  char pidBuffer[sizeof(pid)];
  sprintf(pidBuffer, "%d", pid);
  char pasBuffer[sizeof(this->numeroPasaporte)];
  sprintf(pasBuffer, "%d", this->numeroPasaporte);
  Logger :: getInstance()->registrar(string(pidBuffer) + ": Turista creado con pasaporte: " + string(pasBuffer) + " , ojos " + this->rasgos[0] + " , pelo " + this->rasgos[1] + " , piel " + this->rasgos[2] + " , elemento particular: " + this->rasgos[3] + " , genero " + this->rasgos[4]);
}

Turista :: ~Turista() {
}

/* Metodo que devuelve el numero de pasaporte del turista. */
int Turista :: obtenerNumeroPasaporte() {
  return this->numeroPasaporte;
}

/* Metodo que devuelve un string con los rasgos del turista separados por '-'. */
string Turista :: obtenerRasgos() {
  string colorOjos = this->rasgos[0];
  string colorPelo = this->rasgos[1];
  string tonoPiel = this->rasgos[2];
  string elemento = this->rasgos[3];
  string genero = this->rasgos[4];
  string rasgos = "ojos:" + colorOjos + "-pelo:" + colorPelo + "-piel:" + tonoPiel + "-elemento:" + elemento + "-genero:" + genero + "-";
  return rasgos;
}

/* Metodo que asigna cada tipo de rasgo al turista.
  Los tipos son: color de ojos, color de pelo, tono de piel,
  elementos particulares y el genero del turista. */
void Turista :: asignarRasgos() {
  // Definimos los posibles rasgos de la persona
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
}

/* Metodo que asigna el numero de pasaporte del turista.
  Se toma al pasaporte como un numero de CANT_DIGITOS_PAS digitos del 1 al 9. */
void Turista :: asignarPasaporte() {
  int numero = (rand() % 9) + 1;
  for(long int i = 0; i < CANT_DIGITOS_PAS - 1; i++) {
    numero = (numero * 10) + (rand() % 9) + 1;
  }
  this->numeroPasaporte = numero;
}
