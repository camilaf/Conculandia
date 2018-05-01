#ifndef TURISTA_H_
#define TURISTA_H_

#include <string>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include "../Logger.h"
#define CANT_TIPOS 5
#define CANT_COLOR_OJOS 4
#define CANT_COLOR_PELO 9
#define CANT_TONO_PIEL 3
#define CANT_ELEM_CABEZA 7
#define CANT_GENERO 2
#define CANT_DIGITOS_PAS 6

using namespace std;

class Turista {
private:
  string rasgos[CANT_TIPOS];
  int numeroPasaporte;

public:

  Turista();
  ~Turista();
  int obtenerNumeroPasaporte();
  string obtenerRasgos();

};

#endif /* TURISTA_H_ */
