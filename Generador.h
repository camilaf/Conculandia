#ifndef GENERADOR_H_
#define GENERADOR_H_

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <cerrno>
#include "Fifo/FifoLectura.h"
#include "Fifo/FifoEscritura.h"
#include "Seniales/SIGINT_Handler.h"
#include "Seniales/SignalHandler.h"
#include "Semaforo/Semaforo.h"
#include "Logger.h"
#include "Personas/Turista.h"
#include "Personas/Residente.h"
#include "Personas/Empleado.h"
#define PORCENTAJE_RESIDENTE 60
#define ARCHIVO_SELLOS "main.cpp"
using namespace std;

class Generador {

public:
  Generador();
  ~Generador();

  void generarEmpleados(int cantidadEmpleados, int cantidadSellos);
  void generarPersonas();
  int comunicarResidenteEmpleado(Residente r, FifoEscritura fifo);
  int comunicarTuristaEmpleado(Turista t, FifoEscritura fifo);
  int recibirMensaje(char *buffer, FifoLectura fifo);
  int enviarMensaje(string mensaje, FifoEscritura fifoPersonaEmisor);
};

#endif /* GENERADOR_H_ */
