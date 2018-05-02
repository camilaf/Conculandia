#include "Ministro.h"

Ministro :: Ministro() {
}

Ministro :: ~Ministro() {
}

/* Se ocupa de abrir el fifo de lectura sobre el cual el Ministro de Seguridad recibira
  las alertas para agregar o eliminar algun rasgo del listado de Personas de Riesgo. */
void Ministro :: esperarAlertas() {
  SIGINT_Handler sigint_handler;
  SignalHandler :: getInstance()->registrarHandler(SIGINT, &sigint_handler);

  Logger :: getInstance()->registrar("El Ministro de Seguridad se prepara para recibir alertas");
  FifoLectura fifoAlertas(ARCHIVO_ALERTAS);
  fifoAlertas.abrir();

  while (sigint_handler.getGracefulQuit() == 0) {
    char buffer[BUFFSIZE];
    // Lee la siguiente alerta
    ssize_t bytesLeidos = fifoAlertas.leer(static_cast<void *> (buffer), BUFFSIZE);
    if (bytesLeidos <= 0) {
      if ((errno == EINTR) || (bytesLeidos == 0)) {
        break;
      }
      cerr << "Error al leer del fifo: " << strerror(errno) << endl;
    }

    string comando = string(strtok(buffer, "-"));
    string rasgo = string(strtok(NULL, "-"));
    Logger :: getInstance()->registrar("El Ministro de Seguridad lee el comando: " + comando + " rasgo: " + rasgo);

    if (comando == "Agregar") {
      this->agregarRasgo(rasgo);
    }
    else {
      this->quitarRasgo(rasgo);
    }
    memset(buffer, 0, BUFFSIZE);
  }
  Logger :: getInstance()->registrar("El Ministro de Seguridad cierra el fifo de alertas");
  fifoAlertas.cerrar();
}

/* Metodo que se ocupa de crear el listado de rasgos que los empleados iran consultando.
  Devuelve -1 en caso de error, y 0 en caso contrario. */
int Ministro :: crearListado() {
  fstream listado(ARCHIVO_PERSONAS_RIESGO);

  Logger :: getInstance()->registrar("Se crea el listado de Personas de Riesgo");
  listado.open(ARCHIVO_PERSONAS_RIESGO, fstream :: in | fstream :: out |fstream :: trunc);

  if (listado.fail() && (errno != 0)) {
    cerr << "Error al crear el listado Personas de Riesgo: " << strerror(errno) << endl;
    return -1;
  }
  listado.close();
  return 0;
}

/* Se encarga de consultar el listado de Personas de Riesgo, devolviendo un vector
  con los rasgos que se encuentran en la misma. Para el acceso al recurso compartido
  se usa un lock sobre el file. */
vector<string> Ministro:: consultarListado() {
  LockFile lock(ARCHIVO_PERSONAS_RIESGO);

  if (lock.tomarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al tomar el lock: " << strerror(errno) << endl;
  }
  Logger :: getInstance()->registrar("Se consulta el listado de Personas de Riesgo");

  ifstream listadoRasgos(ARCHIVO_PERSONAS_RIESGO);
  string rasgo;
  vector<string> vectorRasgos;
  string listado = "";

  // Obtengo los rasgos del listado
  while (getline(listadoRasgos, rasgo)) {
    listado += rasgo + " - ";
    vectorRasgos.push_back(rasgo);
  }

  Logger :: getInstance()->registrar("Lee listado: '" + listado + "'");
  listadoRasgos.close();

  if (lock.liberarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al liberar el lock: " << strerror(errno) << endl;
  }
  return vectorRasgos;
}

/* Se encarga de agregar un rasgo en el listado de Personas de Riesgo. */
void Ministro :: agregarRasgo(string rasgo) {
  LockFile lock(ARCHIVO_PERSONAS_RIESGO);

  if (lock.tomarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al tomar el lock: " << strerror(errno) << endl;
  }
  Logger :: getInstance()->registrar("Se agrega el rasgo '" + string(rasgo) + "' al listado");

  ofstream listadoRasgos(ARCHIVO_PERSONAS_RIESGO, ios :: out | ios :: trunc);
  // Agrego el rasgo al vector y escribo los rasgos
  this->listadoActual.push_back(rasgo);
  for (size_t i = 0; i < listadoActual.size(); i++) {
    listadoRasgos << listadoActual[i] << endl;
  }
  listadoRasgos.close();

  string nuevoListado = this->imprimirListado();
  Logger :: getInstance()->registrar("Nuevo listado '" + nuevoListado + "'");

  if (lock.liberarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al liberar el lock: " << strerror(errno) << endl;
  }
}

/* Se encarga de eliminar un rasgo en el listado de Personas de Riesgo. */
void Ministro :: quitarRasgo(string rasgo) {
  LockFile lock(ARCHIVO_PERSONAS_RIESGO);
  if (lock.tomarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al tomar el lock: " << strerror(errno) << endl;
  }
  Logger :: getInstance()->registrar("Se quita el rasgo '" + string(rasgo) + "' al listado en la memoria compartida");

  ofstream listadoRasgos(ARCHIVO_PERSONAS_RIESGO, ios :: out | ios :: trunc);
  // Elimino el rasgo del vector y escribo los rasgos restantes
  vector<string> :: iterator it = find(this->listadoActual.begin(), this->listadoActual.end(), rasgo);
  if (it != this->listadoActual.end()) {
    this->listadoActual.erase(it);
  }

  for (size_t i = 0; i < listadoActual.size(); i++) {
    listadoRasgos << listadoActual[i] << endl;
  }
  listadoRasgos.close();

  string nuevoListado = this->imprimirListado();
  Logger :: getInstance()->registrar("Nuevo listado '" + nuevoListado + "'");

  if (lock.liberarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al liberar el lock: " << strerror(errno) << endl;
  }
}

string Ministro :: imprimirListado() {
  if (this->listadoActual.size() == 0) {
    return "";
  }

  string listado =  this->listadoActual[0];
  for (size_t i = 1; i < this->listadoActual.size(); i++) {
    listado += " - " + this->listadoActual[i];
  }
  return listado;
}
