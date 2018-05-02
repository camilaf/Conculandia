#include "Generador.h"

Generador :: Generador() {
}

Generador :: ~Generador () {
}


/* Recibe el numero de ventanillas y sellos. Se encarga de crear cada empleado
  correspondiente a una ventanilla y que el mismo comience a atender. */
void Generador :: generarEmpleados(int cantidadEmpleados, int cantidadSellos) {
  Logger :: getInstance()->registrar("Creando semaforo para los sellos");
  Semaforo semSellos(ARCHIVO_SELLOS, cantidadSellos);
  FifoLectura colaMigraciones(ARCHIVO_ORDEN);

  for (int i = 0; i < cantidadEmpleados; i++) {
    char numVentanilla[sizeof(i)];
    sprintf(numVentanilla, "%d", i);

    pid_t pid = fork();
    if (pid < 0) {
      Logger :: getInstance()->registrar("Error en fork: " + string(strerror(errno)));
      exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
      char numVentanilla[sizeof(i)];
      sprintf(numVentanilla, "%d", i);
      string numero = string(numVentanilla);
      string nombreFifo = ARCHIVO_VENTANILLA + numero;

      Fifo :: crear(nombreFifo);
      Empleado empleado(numero, nombreFifo);
      empleado.atender(colaMigraciones, semSellos);
      exit(0);
    }
  }
  // Espero a que finalicen los empleados
  for (int i = 0; i < cantidadEmpleados; i++) {
    wait(NULL);
    Logger :: getInstance()->registrar("Termino un empleado");
  }

  Logger :: getInstance()->registrar("Se elimina el fifo de cada ventanilla");
  for (int i = 0; i < cantidadEmpleados; i++) {
    char numero[sizeof(i)];
    sprintf(numero, "%d", i);
    string nombre = ARCHIVO_VENTANILLA + string(numero);
    unlink(nombre.c_str());
  }

  Logger :: getInstance()->registrar("Eliminando semaforo para los sellos");
  semSellos.eliminar();
}

/* Metodo que recibe un buffer y un fifo de lectura.
  Lee del FifoLectura y lo almacena en el buffer. Devuelve -1
  si la lectura fue interrumpida por una senial. */
int Generador :: recibirMensaje(char *buffer, FifoLectura fifo) {
  if (fifo.leer(static_cast<void *> (buffer), BUFFSIZE) < 0) {
    if (errno == EINTR) {
      return -1;
    }
    cerr << "Error al leer del fifo: " << strerror(errno) << endl;
  }
  return 0;
}

/* Metodo que recibe un mensaje y un fifo de escritura.
  Escribe el mensaje en el FifoEscritura y devuelve -1
  si la escritura fue interrumpida por una senial. */
int Generador :: enviarMensaje(string mensaje, FifoEscritura fifo) {
  if (fifo.escribir(mensaje.c_str(), mensaje.length()) < 0) {
    if (errno == EINTR) {
      return -1;
    }
    cerr << "Error al escribir en el fifo de la persona: " << strerror(errno) << endl;
  }
  return 0;
}

/* Se ocupa de la comunicacion entre el residente y el empleado. Recibe al residente, y
  un fifo de escritura donde cada residente colocara su pid y su tipo, para poder ser
  atendido en el orden de llegada. Devuelve -1 si la comunicacion fue interrumpida, 0
  en caso contrario. */
int Generador :: comunicarResidenteEmpleado(Residente r, FifoEscritura colaMigraciones) {
  char pidBuffer[sizeof(getpid())];
  sprintf(pidBuffer, "%d", getpid());

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " crea el fifo para ser atendido");
  string nombreFifoPersona = ARCHIVO_PERSONA + string(pidBuffer);
  Fifo :: crear(nombreFifoPersona);
  FifoLectura fifoPersonaReceptor(nombreFifoPersona);

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " va a escribir en el fifo para colocarse en la cola");
  string mensaje = string(pidBuffer) + "-residente-";
  // Enviamos el mensaje para que nos atienda un empleado
  if (this->enviarMensaje(mensaje, colaMigraciones) < 0) {
    fifoPersonaReceptor.eliminar();
    return -1;
  }

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " abre el fifo para lectura (persona receptor)");
  fifoPersonaReceptor.abrir();
  char buffer[BUFFSIZE];

  // Recibimos el numero de ventanilla del empleado junto con la solicitud del documento
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
  // Enviamos el numero de documento y el genero del residente
  if (this->enviarMensaje(mensaje, fifoEmpleadoEmisor) < 0) {
    fifoEmpleadoEmisor.cerrar();
    fifoPersonaReceptor.cerrar();
    fifoPersonaReceptor.eliminar();
    return -1;
  }

  // Recibimos la respuesta final
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

/* Se ocupa de la comunicacion entre el turista y el empleado. Recibe al turista, y
  un fifo de escritura donde cada turista colocara su pid y su tipo, para poder ser
  atendido en el orden de llegada. Devuelve -1 si la comunicacion fue interrumpida, 0
  en caso contrario. */
int Generador :: comunicarTuristaEmpleado(Turista t, FifoEscritura colaMigraciones) {
  char pidBuffer[sizeof(getpid())];
  sprintf(pidBuffer, "%d", getpid());

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " crea el fifo para ser atendido");
  string nombreFifoPersona = ARCHIVO_PERSONA + string(pidBuffer);
  Fifo :: crear(nombreFifoPersona);
  FifoLectura fifoPersonaReceptor(nombreFifoPersona);

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " va a escribir en el fifo para colocarse en la cola");
  string mensaje = string(pidBuffer) + "-turista-";
  // Enviamos el pid y el tipo para que el turista sea atendido
  if (this->enviarMensaje(mensaje, colaMigraciones) < 0) {
    fifoPersonaReceptor.eliminar();
    return -1;
  }

  Logger :: getInstance()->registrar("Persona " + string(pidBuffer) + " abre el fifo para lectura (persona receptor)");
  fifoPersonaReceptor.abrir();
  char buffer[BUFFSIZE];

  // Recibimos el numero de ventanilla y la solicitud del pasaporte
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

  // Enviamos los datos del pasaporte: el numero y rasgos del turista
  if (this->enviarMensaje(mensaje, fifoEmpleadoEmisor) < 0) {
    fifoEmpleadoEmisor.cerrar();
    fifoPersonaReceptor.cerrar();
    fifoPersonaReceptor.eliminar();
    return -1;
  }

  // Recibimos el mensaje final que indica si el turista es o no deportado
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

/* Se ocupa de generar personas: turistas y residentes que seran atendidos por los empleados
  de la oficina de migraciones. */
void Generador ::generarPersonas() {
  SIGINT_Handler sigint_handler;
  SignalHandler :: getInstance()->registrarHandler(SIGINT, &sigint_handler);

  Logger :: getInstance()->registrar("Se abre el fifo para escritura (cola para migraciones)");
  // Abrimos fifo para colocar a las personas que llegan
  FifoEscritura colaMigraciones(ARCHIVO_ORDEN);
  colaMigraciones.abrir();

  while (sigint_handler.getGracefulQuit() == 0) {
    pid_t pid = fork();
    if (pid < 0) {
      Logger :: getInstance()->registrar("Error en fork: " + string(strerror(errno)));
      exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
      pid_t processId = getpid();
      srand(processId);
      bool esResidente = (rand() % 100) < PORCENTAJE_RESIDENTE;

      char pidBuffer[sizeof(processId)];
      sprintf(pidBuffer, "%d", processId);

      if (esResidente) {
        // Creamos un residente
        Logger :: getInstance()->registrar("Creando residente: " + string(pidBuffer));
        Residente r;

        if (this->comunicarResidenteEmpleado(r, colaMigraciones) < 0) {
          break;
        }
      }
      else {
        // Creamos un turista
        Logger :: getInstance()->registrar("Creando turista: " + string(pidBuffer));
        Turista t;

        if (this->comunicarTuristaEmpleado(t, colaMigraciones) < 0) {
          break;
        }
      }
      exit(0);
    }
    srand(time(0));
    int tiempoEntreLlegadas = rand() % 4 + 1;
    sleep(tiempoEntreLlegadas);
  }
  Logger :: getInstance()->registrar("Se cierra el fifo para escritura (cola para migraciones)");
  colaMigraciones.cerrar();
  colaMigraciones.eliminar();
}
