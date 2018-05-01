#include "Empleado.h"

Empleado :: Empleado(string numero, string nombreFifo) : numero(numero), colaMigraciones(ARCHIVO_ORDEN), fifoEmpleadoReceptor(nombreFifo) {
}

Empleado :: ~Empleado () {
}

void Empleado :: prepararComunicacion() {
  Logger :: getInstance()->registrar("Se abre el fifo para lectura (cola para migraciones)");
  this->colaMigraciones.abrir();
}

int Empleado :: recibirMensaje(char *buffer, FifoLectura fifo) {
  if (fifo.leer(static_cast<void *> (buffer), BUFFSIZE) < 0) {
    if (errno == EINTR) {
      return -1;
    }
    cerr << "Error al leer del fifo: " << strerror(errno) << endl;
  }
  return 0;
}

int Empleado :: enviarMensaje(string mensaje, FifoEscritura fifo) {
  if (fifo.escribir(mensaje.c_str(), mensaje.length()) < 0) {
    if (errno == EINTR) {
      return -1;
    }
    cerr << "Error al escribir en el fifo de la persona: " << strerror(errno) << endl;
  }
  return 0;
}

int Empleado :: atenderResidente(FifoEscritura fifoPersonaEmisor) {
  char buffer[BUFFSIZE];
  Logger :: getInstance()->registrar("Empleado " + this->numero + " escribe su numero de ventanilla (persona emisor)");
  string mensaje = this->numero + ":Le pido su documento de identidad.";
  if (this->enviarMensaje(mensaje, fifoPersonaEmisor) < 0) {
    return -1;
  }
  Logger :: getInstance()->registrar("Empleado " + this->numero + " abre el fifo para lectura (empleado receptor)");
  this->fifoEmpleadoReceptor.abrir();
  if (this->recibirMensaje(buffer, this->fifoEmpleadoReceptor) < 0) {
    return -1;
  }

  char* numeroDocumentoPersona = strtok(buffer, "-");
  char* token = strtok(NULL, "-");
  string generoPersona = string(token);

  Logger :: getInstance()->registrar("Empleado " + this->numero + " lee del fifo (empleado receptor) numero: " + string(numeroDocumentoPersona) + ", genero " + generoPersona);
  bool puedeEntrar = this->verificarCondicionesIngreso(atoi(numeroDocumentoPersona), generoPersona);
  memset(buffer, 0, BUFFSIZE);
  if (puedeEntrar) {
    Logger :: getInstance()->registrar("Empleado " + this->numero + " deja pasar al residente (persona emisor)");
    mensaje = "Feliz ingreso a Conculandia.";
    if (this->enviarMensaje(mensaje, fifoPersonaEmisor) < 0) {
      return -1;
    }
  }
  else {
    Contadores c;
    c.incrementarResidentesDetenidos();
    Logger :: getInstance()->registrar("Empleado " + this->numero + " no deja pasar al residente (persona emisor)");
    mensaje = "A la Oficina de Policia.";
    if (this->enviarMensaje(mensaje, fifoPersonaEmisor) < 0) {
      return -1;
    }
  }
  return 0;
}

int Empleado :: atenderTurista(FifoEscritura fifoPersonaEmisor) {
  char buffer[BUFFSIZE];
  Logger :: getInstance()->registrar("Empleado " + this->numero + " escribe su numero de ventanilla (persona emisor)");
  string mensaje = this->numero + ":Le pido su pasaporte.";
  if (this->enviarMensaje(mensaje, fifoPersonaEmisor) < 0) {
    return -1;
  }
  Logger :: getInstance()->registrar("Empleado " + this->numero + " abre el fifo para lectura (empleado receptor)");
  this->fifoEmpleadoReceptor.abrir();
  if (this->recibirMensaje(buffer, this->fifoEmpleadoReceptor) < 0) {
    return -1;
  }

  char* numeroPasaportePersona = strtok(buffer, "-");
  string rasgos[CANT_TIPOS];
  for (int i = 0; i < CANT_TIPOS; i++) {
    rasgos[i] = string(strtok(NULL, "-"));
  }

  Logger :: getInstance()->registrar("Empleado " + this->numero + " lee del fifo (empleado receptor) pasaporte: " + string(numeroPasaportePersona) + ", " + string(rasgos[0]) + ", " + string(rasgos[1]) + ", " + string(rasgos[2]) + ", " + string(rasgos[3]) + ", " + string(rasgos[4]));
  bool puedeEntrar = this->verificarListadoPersonas(rasgos);
  memset(buffer, 0, BUFFSIZE);
  Contadores c;
  if (puedeEntrar) {
    Logger :: getInstance()->registrar("Empleado " + this->numero + " deja pasar al turista y busca un sello (persona emisor)");
    // Sellos
    c.incrementarTuristasIngresados();
    mensaje = "Feliz ingreso a Conculandia.";
    if (this->enviarMensaje(mensaje, fifoPersonaEmisor) < 0) {
      return -1;
    }
  }
  else {
    c.incrementarTuristasDeportados();
    Logger :: getInstance()->registrar("Empleado " + this->numero + " no deja pasar al turista (persona emisor)");
    mensaje = "Deportado.";
    if (this->enviarMensaje(mensaje, fifoPersonaEmisor) < 0) {
      return -1;
    }
  }
  return 0;
}

void Empleado :: atender() {
  SIGINT_Handler sigint_handler;
  SignalHandler :: getInstance()->registrarHandler(SIGINT, &sigint_handler);

  this->prepararComunicacion();
  LockFile lock(ARCHIVO_ORDEN);

  while (sigint_handler.getGracefulQuit() == 0) {
    Logger :: getInstance()->registrar("Empleado " + this->numero + " listo para atender");
    char buffer[BUFFSIZE];

    if (lock.tomarLock() < 0) {
      if (errno == EINTR) {
        break;
      }
      cerr << "Error al tomar el lock: " << strerror(errno) << endl;
    }

    if (sigint_handler.getGracefulQuit() != 0) {
      break;
    }
    if (this->recibirMensaje(buffer, this->colaMigraciones) < 0) {
      break;
    }
    Logger :: getInstance()->registrar("Empleado " + this->numero + " recibe a la persona");

    char *token;
    token = strtok(buffer, "-");
    string pidPersona = string(token);
    token = strtok(NULL, "-");
    string tipoPersona = string(token);
    memset(buffer, 0, BUFFSIZE);

    if (lock.liberarLock() < 0) {
      if (errno == EINTR) {
        break;
      }
      cerr << "Error al liberar el lock: " << strerror(errno) << endl;
    }

    Logger :: getInstance()->registrar("Empleado " + this->numero + " lee del fifo (cola para migraciones) pid: " + pidPersona + ", tipo: " + tipoPersona);

    Logger :: getInstance()->registrar("Empleado " + this->numero + " abre el fifo para escritura (persona emisor)");
    FifoEscritura fifoPersonaEmisor(ARCHIVO_PERSONA + pidPersona);
    this->fifoPersonas.push_back(ARCHIVO_PERSONA + pidPersona);
    fifoPersonaEmisor.abrir();
    if (tipoPersona == "residente") {
      if (this->atenderResidente(fifoPersonaEmisor)) {
        fifoPersonaEmisor.cerrar();
        break;
      }
    }
    else {
      if (this->atenderTurista(fifoPersonaEmisor)) {
        fifoPersonaEmisor.cerrar();
        break;
      }
    }

    Logger :: getInstance()->registrar("Empleado " + this->numero + " cierra el fifo para escritura (persona emisor)");
    fifoPersonaEmisor.cerrar();
  }
  Logger :: getInstance()->registrar("Empleado " + this->numero + " cierra el fifo para lectura (cola para migraciones)");
  this->colaMigraciones.cerrar();
  Logger :: getInstance()->registrar("Empleado " + this->numero + " cierra el fifo para lectura (empleado receptor)");
  this->fifoEmpleadoReceptor.cerrar();
  this->fifoEmpleadoReceptor.eliminar();
  for (size_t i = 0; i < fifoPersonas.size(); i++) {
    unlink(fifoPersonas[i].c_str());
  }
}

bool Empleado :: verificarCondicionesIngreso(int numeroDocumento, string genero) {
  srand(numeroDocumento);
  int random = rand() % 100;
  char numBuffer[sizeof(random)];
  sprintf(numBuffer, "%d", random);
  Logger :: getInstance()->registrar("Valor random para captura de " + string(numBuffer));
  if (random < VALOR_CAPTURA_HOMBRES) {
    if (genero == "femenino") {
      return true;
    }
    return false;
  }
  else if (random < VALOR_CAPTURA_MUJERES) {
    if (genero == "masculino") {
      return true;
    }
    return false;
  }
  return true;
}

bool Empleado :: verificarListadoPersonas(string* rasgos) {
  return false;
}
