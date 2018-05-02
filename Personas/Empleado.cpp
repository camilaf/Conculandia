#include "Empleado.h"

Empleado :: Empleado(string numero, string nombreFifo) : numero(numero), fifoEmpleadoReceptor(nombreFifo) {
}

Empleado :: ~Empleado () {
}

/* Metodo que recibe un buffer y un fifo de lectura.
  Lee del FifoLectura y lo almacena en el buffer. Devuelve -1
  si la lectura fue interrumpida por una senial. */
int Empleado :: recibirMensaje(char *buffer, FifoLectura fifo) {
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
int Empleado :: enviarMensaje(string mensaje, FifoEscritura fifo) {
  if (fifo.escribir(mensaje.c_str(), mensaje.length()) < 0) {
    if (errno == EINTR) {
      return -1;
    }
    cerr << "Error al escribir en el fifo de la persona: " << strerror(errno) << endl;
  }
  return 0;
}

/* Metodo que recibe un fifo de escritura fifoPersonaEmisor. Mediante el mismo,
  el empleado le envia mensajes al residente que quiere ingresar a Conculandia,
  y a su vez recibe las respuestas del residente a sus solicitudes mediante el
  fifo de lectura fifoEmpleadoReceptor. Devuelve 0 en caso de exito, y -1 si la
  comunicacion fue interrumpida por una senial. */
int Empleado :: atenderResidente(FifoEscritura fifoPersonaEmisor) {
  char buffer[BUFFSIZE];
  Logger :: getInstance()->registrar("Empleado " + this->numero + " escribe su numero de ventanilla (persona emisor)");

  // Solicitamos al residente su documento y le indicamos el numero de ventanilla del empleado
  string mensaje = this->numero + ":Le pido su documento de identidad.";
  if (this->enviarMensaje(mensaje, fifoPersonaEmisor) < 0) {
    return -1;
  }

  Logger :: getInstance()->registrar("Empleado " + this->numero + " abre el fifo para lectura (empleado receptor)");
  this->fifoEmpleadoReceptor.abrir();
  // Esperamos el documento del residente
  if (this->recibirMensaje(buffer, this->fifoEmpleadoReceptor) < 0) {
    return -1;
  }

  char* numeroDocumentoPersona = strtok(buffer, "-");
  string generoPersona = string(strtok(NULL, "-"));
  Logger :: getInstance()->registrar("Empleado " + this->numero + " lee del fifo (empleado receptor) numero: " + string(numeroDocumentoPersona) + ", genero " + generoPersona);

  // Verificamos si el residente puede o no ingresar a Conculandia
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
    // Incrementamos la cantidad de residentes en la Oficina de Policia
    c.incrementarResidentesDetenidos();
    Logger :: getInstance()->registrar("Empleado " + this->numero + " no deja pasar al residente (persona emisor)");
    mensaje = "A la Oficina de Policia.";
    if (this->enviarMensaje(mensaje, fifoPersonaEmisor) < 0) {
      return -1;
    }
  }
  return 0;
}

/* Metodo que recibe un fifo de escritura fifoPersonaEmisor y el semaforo semSellos.
  Mediante el fifo, el empleado le envia mensajes al turista que quiere ingresar a
  Conculandia, y a su vez recibe las respuestas del residente a sus solicitudes mediante
  el fifo de lectura fifoEmpleadoReceptor. Devuelve 0 en caso de exito, y -1 si la
  comunicacion fue interrumpida por una senial. */
int Empleado :: atenderTurista(FifoEscritura fifoPersonaEmisor, Semaforo semSellos) {
  char buffer[BUFFSIZE];
  Logger :: getInstance()->registrar("Empleado " + this->numero + " escribe su numero de ventanilla (persona emisor)");

  // Solicitamos el pasaporte del turista
  string mensaje = this->numero + ":Le pido su pasaporte.";
  if (this->enviarMensaje(mensaje, fifoPersonaEmisor) < 0) {
    return -1;
  }

  Logger :: getInstance()->registrar("Empleado " + this->numero + " abre el fifo para lectura (empleado receptor)");
  this->fifoEmpleadoReceptor.abrir();
  // Esperamos los datos del pasaporte del turista: numero de pasaporte y cada rasgo
  if (this->recibirMensaje(buffer, this->fifoEmpleadoReceptor) < 0) {
    return -1;
  }

  char* numeroPasaportePersona = strtok(buffer, "-");
  vector<string> rasgos;
  for (int i = 0; i < CANT_TIPOS; i++) {
    rasgos.push_back(string(strtok(NULL, "-")));
  }
  Logger :: getInstance()->registrar("Empleado " + this->numero + " lee del fifo (empleado receptor) pasaporte: " + string(numeroPasaportePersona) + ", " + string(rasgos[0]) + ", " + string(rasgos[1]) + ", " + string(rasgos[2]) + ", " + string(rasgos[3]) + ", " + string(rasgos[4]));

  // Verificamos si el turista se encuentra o no en el listado de Personas de Riesgo
  bool puedeEntrar = this->verificarListadoPersonas(rasgos);
  memset(buffer, 0, BUFFSIZE);
  Contadores c;
  if (puedeEntrar) {
    Logger :: getInstance()->registrar("Empleado " + this->numero + " deja pasar al turista y busca un sello");
    // El empleado busca un sello para efectuar el timbrado sobre el pasaporte
    if (semSellos.p() < 0) {
      return -1;
    }
    Logger :: getInstance()->registrar("Empleado " + this->numero + " obtiene un sello");
    this->realizarTimbrado();
    Logger :: getInstance()->registrar("Empleado " + this->numero + " va a liberar el sello");
    if (semSellos.v() < 0) {
      return -1;
    }

    // Incrementamos la cantidad de turistas que ingresaron a Conculandia
    c.incrementarTuristasIngresados();
    mensaje = "Feliz ingreso a Conculandia.";
    if (this->enviarMensaje(mensaje, fifoPersonaEmisor) < 0) {
      return -1;
    }
  }
  else {
    // Incrementamos la cantidad de turistas que fueron deportados
    c.incrementarTuristasDeportados();
    Logger :: getInstance()->registrar("Empleado " + this->numero + " no deja pasar al turista");
    mensaje = "Deportado.";
    if (this->enviarMensaje(mensaje, fifoPersonaEmisor) < 0) {
      return -1;
    }
  }
  return 0;
}

/* Recibe un fifo de lectura colaMigraciones y un semaforo semSellos.
  Para atender a una persona se queda esperando a que escriba su pid en el fifo,
  y luego empieza la comunicacion, dependiendo de si la persona es residente o
  turista. */
void Empleado :: atender(FifoLectura colaMigraciones, Semaforo semSellos) {
  SIGINT_Handler sigint_handler;
  SignalHandler :: getInstance()->registrarHandler(SIGINT, &sigint_handler);

  Logger :: getInstance()->registrar("Se abre el fifo para lectura (cola para migraciones)");
  colaMigraciones.abrir();
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
    // El empleado lee el mensaje de la primera persona en la cola de migraciones: su pid y tipo
    if (this->recibirMensaje(buffer, colaMigraciones) < 0) {
      break;
    }
    Logger :: getInstance()->registrar("Empleado " + this->numero + " recibe a la persona");

    string pidPersona = string(strtok(buffer, "-"));
    string tipoPersona = string(strtok(NULL, "-"));
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
      if (this->atenderResidente(fifoPersonaEmisor) < 0) {
        fifoPersonaEmisor.cerrar();
        break;
      }
    }
    else {
      if (this->atenderTurista(fifoPersonaEmisor, semSellos) < 0) {
        fifoPersonaEmisor.cerrar();
        break;
      }
    }

    Logger :: getInstance()->registrar("Empleado " + this->numero + " cierra el fifo para escritura (persona emisor)");
    fifoPersonaEmisor.cerrar();
  }
  Logger :: getInstance()->registrar("Empleado " + this->numero + " cierra el fifo para lectura (cola para migraciones)");
  colaMigraciones.cerrar();
  Logger :: getInstance()->registrar("Empleado " + this->numero + " cierra el fifo para lectura (empleado receptor)");
  this->fifoEmpleadoReceptor.cerrar();
  for (size_t i = 0; i < fifoPersonas.size(); i++) {
    unlink(fifoPersonas[i].c_str());
  }
}

/* Recibe el numero de documento y el genero del residente y decide si
  el residente puede ingresar a su pais. Devuelve true si puede ingresar
  a Conculandia, y false si debe ser detenido. */
bool Empleado :: verificarCondicionesIngreso(int numeroDocumento, string genero) {
  srand(numeroDocumento);
  int random = rand() % 100;

  char numBuffer[sizeof(random)];
  sprintf(numBuffer, "%d", random);
  Logger :: getInstance()->registrar("Valor random para captura de " + string(numBuffer));

  if ((random < VALOR_CAPTURA_HOMBRES) && (genero == "masculino")){
    return false;
  }
  else if ((random < VALOR_CAPTURA_MUJERES) && (genero == "femenino")) {
    return false;
  }
  return true;
}

/* Recibe un vector con los rasgos del turista y los compara con el listado
  de Personas de Riesgo. Si algun elemento coincide no lo deja ingresar a
  a Conculandia devolviendo false, en caso contrario puede ingresar. */
bool Empleado :: verificarListadoPersonas(vector<string> rasgos) {
  Ministro m;
  vector<string> listado = m.consultarListado();
  for (size_t i = 0; i < rasgos.size(); i++) {
    vector<string> :: iterator it = find(listado.begin(), listado.end(), rasgos[i]);
    if (it != listado.end()) {
      Logger :: getInstance()->registrar("Turista de empleado " + this->numero + " tiene rasgo que se encuentra en el listado: " + rasgos[i]);
      return false;
    }
  }
  return true;
}

/* Realiza el timbrado sobre el pasaporte del turista. */
void Empleado :: realizarTimbrado() {
  Logger :: getInstance()->registrar("Empleado " + this->numero + " busca una pagina libre y sella el pasaporte");
  sleep(3);
}
