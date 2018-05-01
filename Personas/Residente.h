#ifndef RESIDENTE_H_
#define RESIDENTE_H_

#include "../Logger.h"
#include <string>
#include <unistd.h>
#include <cstdlib>
#define CANT_DIGITOS_DOCU 8

using namespace std;

class Residente {
private:
  int numeroDocumento;
  string genero;

public:
  Residente();
  ~Residente();

  int obtenerNumeroDocumento();
  int comunicarEmpleado();
  string obtenerGenero();

};

#endif /* RESIDENTE_H_ */
