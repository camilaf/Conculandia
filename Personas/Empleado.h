#ifndef EMPLEADO_H_
#define EMPLEADO_H_

#include "../Seniales/SIGINT_Handler.h"
#include "../Seniales/SignalHandler.h"
#include "../Logger.h"
#include "../Fifo/FifoLectura.h"
#include "../Fifo/FifoEscritura.h"
#include "../Lock/LockFile.h"
#include "../Semaforo/Semaforo.h"
#include "../Contadores.h"
#include "../Entidades/Ministro.h"
#include <stdio.h>
#include <vector>
#include <algorithm>
#define VALOR_CAPTURA_MUJERES 80
#define VALOR_CAPTURA_HOMBRES 40
#define ARCHIVO_ORDEN "/tmp/fifo_orden"
#define ARCHIVO_VENTANILLA "/tmp/fifo_ventanilla_"
#define ARCHIVO_PERSONA "/tmp/fifo_persona_"
#define BUFFSIZE 100
#define CANT_TIPOS 5
using namespace std;

class Empleado {
private:
  string numero;
  vector <string> fifoPersonas;
  FifoLectura fifoEmpleadoReceptor;
  bool verificarCondicionesIngreso(int numeroDocumento, string genero);
  bool verificarListadoPersonas(vector<string> rasgos);
  int recibirMensaje(char *buffer, FifoLectura fifo);
  int enviarMensaje(string mensaje, FifoEscritura fifoPersonaEmisor);
  int atenderResidente(FifoEscritura fifoPersonaEmisor);
  int atenderTurista(FifoEscritura fifoPersonaEmisor, Semaforo semSellos);
  void realizarTimbrado();

public:

  Empleado(string numero, string nombreFifo);
  ~Empleado();

  void atender(FifoLectura colaMigraciones, Semaforo semSellos);
};

#endif /* EMPLEADO_H_ */
