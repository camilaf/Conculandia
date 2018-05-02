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
#include "Entidades/Policia.h"
#include "Entidades/Ministro.h"
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

  if ((asistente.verificarArgumentos(argc, argv) < 0) || (asistente.inicializarContadoresConsultas() < 0) || (asistente.crearListadoPersonasDeRiesgo() < 0)) {
    return -1;
  }

  Logger :: getInstance()->registrar("Inicio de simulacion");

  // Signal handler
  SIGINT_Handler sigint_handler;
  SignalHandler :: getInstance()->registrarHandler(SIGINT, &sigint_handler);

  Logger :: getInstance()->registrar("Creamos fifo para la cola de migraciones y para las alertas");
  if ((Fifo :: crear(ARCHIVO_ORDEN) < 0) || (Fifo :: crear(ARCHIVO_ALERTAS) < 0)) {
    return -1;
  }

  // Generamos a la policia
  pid_t pidPolicia = fork();
  if (pidPolicia < 0) {
    Logger :: getInstance()->registrar("Error en fork: " + string(strerror(errno)));
    exit(EXIT_FAILURE);
  }

  else if (pidPolicia == 0) {
    Policia p;
    p.iniciarAlertas();
    exit(0);
  }

  // Generamos al Ministro de Seguridad
  pid_t pidMinistro = fork();
  if (pidMinistro < 0) {
    Logger :: getInstance()->registrar("Error en fork: " + string(strerror(errno)));
    exit(EXIT_FAILURE);
  }

  else if (pidMinistro == 0) {
    Ministro m;
    m.crearListado();
    m.esperarAlertas();
    exit(0);
  }

  Generador generador;
  // Generamos a los empleados
  pid_t pidEmpleado = fork();
  if (pidEmpleado < 0) {
    Logger :: getInstance()->registrar("Error en fork: " + string(strerror(errno)));
    exit(EXIT_FAILURE);
  }

  else if (pidEmpleado == 0) {
    int cantEmpleados = asistente.obtenerCantidadVentanillas();
    int cantSellos = asistente.obtenerCantidadSellos();
    generador.generarEmpleados(cantEmpleados, cantSellos);
    exit(0);
  }

  // Generamos a las personas
  pid_t pidPersona = fork();
  if (pidPersona < 0) {
    Logger :: getInstance()->registrar("Error en fork: " + string(strerror(errno)));
    exit(EXIT_FAILURE);
  }

  else if (pidPersona == 0) {
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
