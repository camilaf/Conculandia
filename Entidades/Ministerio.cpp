#include "Ministerio.h"

Ministerio :: Ministerio() {
}

Ministerio :: ~Ministerio() {
}

int Ministerio :: crearListado() {
  Logger :: getInstance()->registrar("Creando la memoria compartida para el listado de Personas de Riesgo");
  int estadoListado = this->personasDeRiesgo.crear(ARCHIVO_PERSONAS_RIESGO, 'P', sizeof(char) * TAM_LISTA);

  if (estadoListado < 0) {
    Logger :: getInstance()->registrar("Error al crear la memoria compartida para el listado");
    return -1;
  }
  return 0;
}

int Ministerio :: inicializarListado() {
  if (this->crearListado() < 0) {
    Logger :: getInstance()->registrar("Error al crear la memoria compartida para el listado");
    return -1;
  }

  Logger :: getInstance()->registrar("Inicializando el listado de Personas de Riesgo en la memoria compartida");
  string valorInicial = "";
  this->personasDeRiesgo.escribir(valorInicial.c_str());

  this->desadosarListado();
  return 0;
}

 char* Ministerio:: consultarListado() {
  if (this->crearListado() < 0) {
    return -1;
  }

  LockFile lock(ARCHIVO_PERSONAS_RIESGO);
  if (lock.tomarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al tomar el lock: " << strerror(errno) << endl;
  }
  Logger :: getInstance()->registrar("Se consulta el listado de Personas de Riesgo en la memoria compartida");
  char resultado[TAM_LISTA] = this->personasDeRiesgo.leer();
  Logger :: getInstance()->registrar("Lee listado: " + string(resultado));
  if (lock.liberarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al liberar el lock: " << strerror(errno) << endl;
  }

  this->desadosarListado();
  return resultado;
}

void Ministerio :: agregarRasgo(string rasgo) {
  if (this->crearListado() < 0) {
    Logger :: getInstance()->registrar("Error al crear la memoria compartida para modificar el listado");
    return -1;
  }
  listadoRasgos.push_back(rasgo);
  string nuevoListado = this->imprimirListado();
  LockFile lock(ARCHIVO_PERSONAS_RIESGO);
  if (lock.tomarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al tomar el lock: " << strerror(errno) << endl;
  }
  Logger :: getInstance()->registrar("Se agrega el rasgo '" + rasgo + "' al listado en la memoria compartida");
  Logger :: getInstance()->registrar("Nuevo listado: " + nuevoListado);
  this->personasDeRiesgo.escribir(nuevoListado.c_str());
  if (lock.liberarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al liberar el lock: " << strerror(errno) << endl;
  }
  this->desadosarListado();
}

void Ministerio :: quitarRasgo(string rasgo) {
  if (this->crearListado() < 0) {
    Logger :: getInstance()->registrar("Error al crear la memoria compartida para modificar el listado");
    return -1;
  }

  vector<string> :: iterator it = find(listadoRasgos.begin(), listadoRasgos.end(), rasgo);
  if (it == listadoRasgos.end()) {
    return;
  }
  listadoRasgos.erase(it);
  string nuevoListado = this->imprimirListado();
  LockFile lock(ARCHIVO_PERSONAS_RIESGO);
  if (lock.tomarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al tomar el lock: " << strerror(errno) << endl;
  }
  Logger :: getInstance()->registrar("Se quita el rasgo '" + rasgo + "' al listado en la memoria compartida");
  Logger :: getInstance()->registrar("Nuevo listado: " + nuevoListado);
  this->personasDeRiesgo.escribir(nuevoListado.c_str());
  if (lock.liberarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al liberar el lock: " << strerror(errno) << endl;
  }
  this->desadosarListado();
}

void Ministerio :: desadosarListado() {
  this->personasDeRiesgo.desadosar();
}

void Ministerio :: liberarListado() {
  this->personasDeRiesgo.liberar();
}

string Ministerio :: imprimirListado() {
  if (listadoRasgos.size() == 0) {
    return ";"
  }

  string listado = listadoRasgos[0];
  for (size_t i = 1; i < listadoRasgos.size(); i++) {
    listado += "-" + listadoRasgos[i];
  }
  return listado;
}
