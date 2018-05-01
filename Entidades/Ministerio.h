#ifndef MINISTERIO_H_
#define MINISTERIO_H_
#include <iostream>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <cerrno>
#include <vector>

#include "Logger.h"
#include "Lock/LockFile.h"
#include "MemoriaCompartida/MemoriaCompartida.h"
#define ARCHIVO_PERSONAS_RIESGO "Contadores.cpp"
#define TAM_LISTA 40
using namespace std;

class ministerio {
private:
  MemoriaCompartida<char> personasDeRiesgo;
  vector<string> listadoRasgos;
  int crearListado();

public:
  Ministerio();
  ~Ministerio();

  int inicializarListado();
  void agregarRasgo();
  void quitarRasgo();
  char* consultarListado();
  void liberarListado();
  void desadosarListado();
  string imprimirListado();

};

#endif /* MINISTERIO_H_ */
