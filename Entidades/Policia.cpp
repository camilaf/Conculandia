#include "Policia.h"

Policia :: Policia() {
  string rasgos[CANT_TOTAL_RASGOS] = {"ojos:marrones", "ojos:azules", "ojos:verdes", "ojos:grises", "pelo:rubio", "pelo:negro", "pelo:gris", "pelo:marron oscuro", "pelo:marron claro", "pelo:rojo", "pelo:rosa", "pelo:azul", "pelo:violeta", "piel:clara", "piel:media", "piel:oscura", "genero:femenino", "genero:masculino", "elemento:sombrero", "elemento:gorra", "elemento:pelo suelto", "elemento:trenza", "elemento:cola de caballo", "elemento:rastas", "elemento:lentes"};

  pid_t pid = getpid();
  srand(pid);
  this->rasgos[0] = ojos[rand() % CANT_COLOR_OJOS];
  this->rasgos[1] = pelo[rand() % CANT_COLOR_PELO];
  this->rasgos[2] = piel[rand() % CANT_TONO_PIEL];
  this->rasgos[3] = elementosCabeza[rand() % CANT_ELEM_CABEZA];
  this->rasgos[4] = genero[rand() % CANT_GENERO];

  int numero = (rand() % 9) + 1;
  for(long int i = 0; i < CANT_DIGITOS - 1; i++) {
    numero = (numero * 10) + (rand() % 9) + 1;
  }
  this->numeroPasaporte = numero;

  char pasBuffer[sizeof(this->numeroPasaporte)];
  char pidBuffer[sizeof(pid)];
  sprintf(pasBuffer, "%d", this->numeroPasaporte);
  sprintf(pidBuffer, "%d", pid);
  Logger :: getInstance()->registrar(string(pidBuffer) + ": Turista creado con pasaporte: " + string(pasBuffer) + " , ojos " + this->rasgos[0] + " , pelo " + this->rasgos[1] + " , piel " + this->rasgos[2] + " , elemento particular: " + this->rasgos[3] + " , genero " + this->rasgos[4]);
}

Policia :: ~Policia() {
}

void Policia :: enviarAlerta() {
  srand(time(0));
  if ((listadoActual.size() == 0) || (rand() % 100 >= PORCENTAJE_PARA_REMOVER) {
    // AGREGAR RASGO
  }
  else {
    // SACAR RASGO
  }
}
