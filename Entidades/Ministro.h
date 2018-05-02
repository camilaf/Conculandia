#ifndef MINISTRO_H_
#define MINISTRO_H_
#include <iostream>
#include <ctype.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <cerrno>
#include <vector>
#include <algorithm>

#include "../Logger.h"
#include "../Lock/LockFile.h"
#include "../MemoriaCompartida/MemoriaCompartida.h"
#include "../Fifo/FifoLectura.h"
#include "../Seniales/SIGINT_Handler.h"
#include "../Seniales/SignalHandler.h"

#define BUFFSIZE 100
#define ARCHIVO_ALERTAS "/tmp/fifo_alertas"
#define ARCHIVO_PERSONAS_RIESGO "listado.txt"
#define TAM_LISTA 25
using namespace std;

class Ministro {
private:
  vector<string> listadoActual;
  void agregarRasgo(string rasgo);
  void quitarRasgo(string rasgo);
  string imprimirListado();

public:
  Ministro();
  ~Ministro();

  vector<string> consultarListado();
  int crearListado();
  void esperarAlertas();

};

#endif /* MINISTRO_H_ */
