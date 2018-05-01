#ifndef CONTADORES_H_
#define CONTADORES_H_
#include <iostream>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <cerrno>

#include "Logger.h"
#include "Lock/LockFile.h"
#include "MemoriaCompartida/MemoriaCompartida.h"
#define ARCHIVO_RD "Asistente.cpp"
#define ARCHIVO_TD "Generador.cpp"
#define ARCHIVO_TI "Logger.cpp"
using namespace std;

class Contadores {
private:
  MemoriaCompartida<int> residentesDetenidos;
  MemoriaCompartida<int> turistasDeportados;
  MemoriaCompartida<int> turistasIngresados;
  int crearContadores();

public:
  Contadores();
  ~Contadores();

  int inicializarContadores();
  void incrementarTuristasDeportados();
  void incrementarTuristasIngresados();
  void incrementarResidentesDetenidos();
  int cantidadTuristasDeportados();
  int cantidadTuristasIngresados();
  int cantidadResidentesDetenidos();
  void liberarContadores();
  void desadosarContadores();

};

#endif /* CONTADORES_H_ */
