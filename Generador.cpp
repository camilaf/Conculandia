#include "Generador.h"

Generador :: Generador() {
}

Generador :: ~Generador () {
}

void Generador ::generarEmpleados(int cantidad) {
  for (int i = 0; i < cantidad; i++) {
    char numVentanilla[sizeof(i)];
    sprintf(numVentanilla, "%d", i);
    pid_t pid = fork();
    if (pid == 0) {
      char numVentanilla[sizeof(i)];
      sprintf(numVentanilla, "%d", i);
      string numero = string(numVentanilla);
      string nombreFifo = ARCHIVO_VENTANILLA + numero;
      Fifo :: crear(nombreFifo);
      Empleado empleado(numero, nombreFifo);
      empleado.atender();
      exit(0);
    }
  }
}

int Generador :: recibirMensaje(char *buffer, FifoLectura fifo) {
  if (fifo.leer(static_cast<void *> (buffer), BUFFSIZE) < 0) {
    if (errno == EINTR) {
      return -1;
    }
    cerr << "Error al leer del fifo: " << strerror(errno) << endl;
  }
  return 0;
}

int Generador :: enviarMensaje(string mensaje, FifoEscritura fifo) {
  if (fifo.escribir(mensaje.c_str(), mensaje.length()) < 0) {
    if (errno == EINTR) {
      return -1;
    }
    cerr << "Error al escribir en el fifo de la persona: " << strerror(errno) << endl;
  }
  return 0;
}

int Generador :: comunicarResidenteEmpleado(Residente r, FifoEscritura colaMigraciones) {
  char pidBuffer[sizeof(getpid())];
  sprintf(pidBuffer, "%d", getpid());
  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " crea el fifo para ser atendido");
  string nombreFifoPersona = ARCHIVO_PERSONA + string(pidBuffer);
  Fifo :: crear(nombreFifoPersona);
  FifoLectura fifoPersonaReceptor(nombreFifoPersona);

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " va a escribir en el fifo para colocarse en la cola");
  string mensaje = string(pidBuffer) + "-residente";

  if (this->enviarMensaje(mensaje, colaMigraciones) < 0) {
    fifoPersonaReceptor.eliminar();
    return -1;
  }


  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " abre el fifo para lectura (persona receptor)");
  fifoPersonaReceptor.abrir();
  char buffer[BUFFSIZE];

  if (this->recibirMensaje(buffer, fifoPersonaReceptor) < 0) {
    fifoPersonaReceptor.cerrar();
    fifoPersonaReceptor.eliminar();
    return -1;
  }

  string ventanilla = string(strtok(buffer, ":"));
  string mensajeDocumento = string(strtok(NULL, ":"));

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " lee numero de ventanilla: " + ventanilla + " y mensaje: " + mensajeDocumento);
  memset(buffer, 0, BUFFSIZE);

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " abre el fifo para escritura (empleado emisor)");
  FifoEscritura fifoEmpleadoEmisor(ARCHIVO_VENTANILLA + ventanilla);
  fifoEmpleadoEmisor.abrir();
  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " va a escribir en el fifo (empleado emisor)");
  int numDocumento = r.obtenerNumeroDocumento();
  char docuBuffer[sizeof(numDocumento)];
  sprintf(docuBuffer, "%d", numDocumento);
  mensaje = string(docuBuffer) + "-" + r.obtenerGenero();

  if (this->enviarMensaje(mensaje, fifoEmpleadoEmisor) < 0) {
    fifoEmpleadoEmisor.cerrar();
    fifoPersonaReceptor.cerrar();
    fifoPersonaReceptor.eliminar();
    return -1;
  }

  if (this->recibirMensaje(buffer, fifoPersonaReceptor) < 0) {
    fifoEmpleadoEmisor.cerrar();
    fifoPersonaReceptor.cerrar();
    fifoPersonaReceptor.eliminar();
    return -1;
  }

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " lee veredicto: " + string(buffer));
  memset(buffer, 0, BUFFSIZE);

  Logger :: getInstance()->registrar("Se cierra el fifo para para escritura (empleado emisor)");
  fifoEmpleadoEmisor.cerrar();
  Logger :: getInstance()->registrar("Se cierra el fifo para lectura (persona receptor)");
  fifoPersonaReceptor.cerrar();
  fifoPersonaReceptor.eliminar();
  return 0;
}

int Generador :: comunicarTuristaEmpleado(Turista t, FifoEscritura colaMigraciones) {
  char pidBuffer[sizeof(getpid())];
  sprintf(pidBuffer, "%d", getpid());
  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " crea el fifo para ser atendido");
  string nombreFifoPersona = ARCHIVO_PERSONA + string(pidBuffer);
  Fifo :: crear(nombreFifoPersona);
  FifoLectura fifoPersonaReceptor(nombreFifoPersona);

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " va a escribir en el fifo para colocarse en la cola");
  string mensaje = string(pidBuffer) + "-turista";

  if (this->enviarMensaje(mensaje, colaMigraciones) < 0) {
    fifoPersonaReceptor.eliminar();
    return -1;
  }

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " abre el fifo para lectura (persona receptor)");
  fifoPersonaReceptor.abrir();
  char buffer[BUFFSIZE];

  if (this->recibirMensaje(buffer, fifoPersonaReceptor) < 0) {
    fifoPersonaReceptor.cerrar();
    fifoPersonaReceptor.eliminar();
    return -1;
  }

  string ventanilla = string(strtok(buffer, ":"));
  string mensajePasaporte = string(strtok(NULL, ":"));

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " lee numero de ventanilla: " + ventanilla + " y mensaje: " + mensajePasaporte);
  memset(buffer, 0, BUFFSIZE);

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " abre el fifo para escritura (empleado emisor)");
  FifoEscritura fifoEmpleadoEmisor(ARCHIVO_VENTANILLA + ventanilla);
  fifoEmpleadoEmisor.abrir();
  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " va a escribir en el fifo (empleado emisor)");
  int numPasaporte = t.obtenerNumeroPasaporte();
  char pasBuffer[sizeof(numPasaporte)];
  sprintf(pasBuffer, "%d", numPasaporte);
  mensaje = string(pasBuffer) + "-" + t.obtenerRasgos();

  if (this->enviarMensaje(mensaje, fifoEmpleadoEmisor) < 0) {
    fifoEmpleadoEmisor.cerrar();
    fifoPersonaReceptor.cerrar();
    fifoPersonaReceptor.eliminar();
    return -1;
  }

  if (this->recibirMensaje(buffer, fifoPersonaReceptor) < 0) {
    fifoEmpleadoEmisor.cerrar();
    fifoPersonaReceptor.cerrar();
    fifoPersonaReceptor.eliminar();
    return -1;
  }

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " lee veredicto: " + string(buffer));
  memset(buffer, 0, BUFFSIZE);

  Logger :: getInstance()->registrar("Se cierra el fifo para para escritura (empleado emisor)");
  fifoEmpleadoEmisor.cerrar();
  Logger :: getInstance()->registrar("Se cierra el fifo para lectura (persona receptor)");
  fifoPersonaReceptor.cerrar();
  fifoPersonaReceptor.eliminar();
  return 0;
}

void Generador ::generarPersonas() {
  SIGINT_Handler sigint_handler;
  SignalHandler :: getInstance()->registrarHandler(SIGINT, &sigint_handler);

  Logger :: getInstance()->registrar("Se abre el fifo para escritura (cola para migraciones)");
  // Abrimos fifo para colocar a las personas que llegan
  FifoEscritura colaMigraciones(ARCHIVO_ORDEN);
  colaMigraciones.abrir();
  while (sigint_handler.getGracefulQuit() == 0) {
    pid_t pid = fork();
    if (pid == 0) {
      pid_t processId = getpid();
      srand(processId);
      bool esResidente = (rand() % 100) < PORCENTAJE_RESIDENTE;
      char pidBuffer[sizeof(processId)];
      sprintf(pidBuffer, "%d", processId);

      if (esResidente) {
        Logger :: getInstance()->registrar("Creando residente: " + string(pidBuffer));
        Residente r;

        if (this->comunicarResidenteEmpleado(r, colaMigraciones) < 0) {
          break;
        }
      }
      else {
        Logger :: getInstance()->registrar("Creando turista: " + string(pidBuffer));
        Turista t;

        if (this->comunicarTuristaEmpleado(t, colaMigraciones) < 0) {
          break;
        }
      }
      exit(0);
    }
    srand(time(0));
    int tiempoEntreLlegadas = rand() % 10 + 1;
    sleep(tiempoEntreLlegadas);
  }
  Logger :: getInstance()->registrar("Se cierra el fifo para escritura (cola para migraciones)");
  colaMigraciones.cerrar();
  colaMigraciones.eliminar();
}
