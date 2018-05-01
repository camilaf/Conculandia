#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <cerrno>
#include <sys/wait.h>
#include "Logger.h"
#include "Personas/Turista.h"
#include "Personas/Residente.h"
#include "Seniales/SIGINT_Handler.h"
#include "Seniales/SignalHandler.h"
#include "Semaforo/Semaforo.h"
#include "Generador.h"
#include "Asistente.h"
#define ARCHIVO_ORDEN "/tmp/fifo_orden"

using namespace std;

// Formato
// ./main.cpp -v X -s Y -d
int main(int argc, char *argv[]) {

  Asistente asistente;

  if ((asistente.verificarArgumentos(argc, argv) < 0) || (asistente.inicializarContadoresConsultas() < 0)) {
    return -1;
  }

  Logger :: getInstance()->registrar("Inicio de simulacion");

  // Signal handler
  SIGINT_Handler sigint_handler;
  SignalHandler :: getInstance()->registrarHandler(SIGINT, &sigint_handler);

  // Creacion de semaforo sellos


  // Creo fifo para cola de migraciones
  if (Fifo :: crear(ARCHIVO_ORDEN) < 0) {
    return -1;
  }

  Generador generador;

  pid_t pidEmpleado = fork();
  if (pidEmpleado < 0) {
    Logger :: getInstance()->registrar("Error en fork: " + string(strerror(errno)));
    exit(EXIT_FAILURE);
  }

  else if (pidEmpleado == 0) {
    int cantEmpleados = asistente.obtenerCantidadVentanillas();
    generador.generarEmpleados(cantEmpleados);
    exit(0);
  }

  pid_t pid = fork();
  if (pid < 0) {
    Logger :: getInstance()->registrar("Error en fork: " + string(strerror(errno)));
    exit(EXIT_FAILURE);
  }

  else if (pid == 0) {
    generador.generarPersonas();
    exit(0);
  }

  asistente.iniciarMenu();
  Logger :: getInstance()->registrar("Opcion de salida seleccionada");
  kill(0, SIGINT);
  asistente.liberarContadoresConsultas();
  SignalHandler :: destruir();
  Logger :: destruir();

  cout << "Hasta la proxima!" << endl;
  return 0;
}
