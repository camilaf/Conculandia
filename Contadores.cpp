#include "Contadores.h"

Contadores :: Contadores() {
}

Contadores :: ~Contadores() {
}

int Contadores :: crearContadores() {
  Logger :: getInstance()->registrar("Creando la memoria compartida para cada contador");
  int estadoDetenidos = this->residentesDetenidos.crear("/bin/bash", 'D', sizeof(int));
  int estadoDeportados = this->turistasDeportados.crear("/bin/ls", 'R', sizeof(int));
  int estadoIngresados = this->turistasIngresados.crear("/bin/cat", 'I', sizeof(int));

  if ((estadoDetenidos < 0) || (estadoDeportados < 0) || (estadoIngresados < 0)) {
    Logger :: getInstance()->registrar("Error al crear la memoria compartida para consultas");
    if (estadoDetenidos == 0) {
      this->residentesDetenidos.liberar();
    }
    if (estadoDeportados == 0) {
      this->turistasDeportados.liberar();
    }
    if (estadoIngresados == 0) {
      this->turistasIngresados.liberar();
    }
    return -1;
  }

  return 0;
}

int Contadores :: inicializarContadores() {
  if (this->crearContadores() < 0) {
    Logger :: getInstance()->registrar("Error al crear la memoria compartida para consultas");
    return -1;
  }

  Logger :: getInstance()->registrar("Inicializando a cero cada contador en la memoria compartida");
  int valorInicial = 0;
  this->residentesDetenidos.escribir(valorInicial);
  this->turistasDeportados.escribir(valorInicial);
  this->turistasIngresados.escribir(valorInicial);

  this->desadosarContadores();
  return 0;
}

int Contadores :: cantidadTuristasDeportados() {
  if (this->crearContadores() < 0) {
    return -1;
  }

  LockFile lock(ARCHIVO_TD);
  if (lock.tomarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al tomar el lock: " << strerror(errno) << endl;
  }
  Logger :: getInstance()->registrar("Se consultan los turistas deportados en la memoria compartida");
  int resultado = this->turistasDeportados.leer();
  char buffer[sizeof(resultado)];
  sprintf(buffer, "%d", resultado);
  Logger :: getInstance()->registrar("Lee cantidad de turistas deportados: " + string(buffer));
  if (lock.liberarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al liberar el lock: " << strerror(errno) << endl;
  }

  this->desadosarContadores();
  return resultado;
}

int Contadores :: cantidadTuristasIngresados() {
  if (this->crearContadores() < 0) {
    Logger :: getInstance()->registrar("Error al crear la memoria compartida para consultas");
    return -1;
  }

  LockFile lock(ARCHIVO_TI);
  if (lock.tomarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al tomar el lock: " << strerror(errno) << endl;
  }
  Logger :: getInstance()->registrar("Se consultan los turistas ingresados en la memoria compartida");
  int resultado = this->turistasIngresados.leer();
  char buffer[sizeof(resultado)];
  sprintf(buffer, "%d", resultado);
  Logger :: getInstance()->registrar("Lee cantidad de turistas ingresados: " + string(buffer));
  if (lock.liberarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al liberar el lock: " << strerror(errno) << endl;
  }
  this->desadosarContadores();
  return resultado;
}

int Contadores :: cantidadResidentesDetenidos() {
  if (this->crearContadores() < 0) {
    Logger :: getInstance()->registrar("Error al crear la memoria compartida para consultas");
    return -1;
  }

  LockFile lock(ARCHIVO_TI);
  if (lock.tomarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al tomar el lock: " << strerror(errno) << endl;
  }
  Logger :: getInstance()->registrar("Se consultan los residentes detenidos en la memoria compartida");
  int resultado = residentesDetenidos.leer();
  char buffer[sizeof(resultado)];
  sprintf(buffer, "%d", resultado);
  Logger :: getInstance()->registrar("Lee cantidad de residentes detenidos: " + string(buffer));
  if (lock.liberarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al liberar el lock: " << strerror(errno) << endl;
  }
  this->desadosarContadores();
  return resultado;
}

void Contadores :: incrementarTuristasDeportados() {
  if (this->crearContadores() < 0) {
    Logger :: getInstance()->registrar("Error al crear la memoria compartida para consultas");
    return;
  }

  LockFile lock(ARCHIVO_RD);
  if (lock.tomarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al tomar el lock: " << strerror(errno) << endl;
  }
  Logger :: getInstance()->registrar("Se incrementan los turistas deportados en la memoria compartida");
  int resultado = turistasDeportados.leer();
  int nuevo = resultado + 1;
  char resBuffer[sizeof(resultado)];
  sprintf(resBuffer, "%d", resultado);
  char nuevoBuffer[sizeof(nuevo)];
  sprintf(nuevoBuffer, "%d", nuevo);
  Logger :: getInstance()->registrar("Cambia cantidad de turistas deportados: " + string(resBuffer) + " por cantidad " + string(nuevoBuffer));
  turistasDeportados.escribir(nuevo);
  if (lock.liberarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al liberar el lock: " << strerror(errno) << endl;
  }
  this->desadosarContadores();
}

void Contadores :: incrementarTuristasIngresados() {
  if (this->crearContadores() < 0) {
    Logger :: getInstance()->registrar("Error al crear la memoria compartida para consultas");
    return;
  }

  LockFile lock(ARCHIVO_RD);
  if (lock.tomarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al tomar el lock: " << strerror(errno) << endl;
  }
  Logger :: getInstance()->registrar("Se incrementan los turistas ingresados en la memoria compartida");
  int resultado = turistasIngresados.leer();
  int nuevo = resultado + 1;
  char resBuffer[sizeof(resultado)];
  sprintf(resBuffer, "%d", resultado);
  char nuevoBuffer[sizeof(nuevo)];
  sprintf(nuevoBuffer, "%d", nuevo);
  Logger :: getInstance()->registrar("Cambia cantidad de turistas ingresados: " + string(resBuffer) + " por cantidad " + string(nuevoBuffer));
  turistasIngresados.escribir(nuevo);
  if (lock.liberarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al liberar el lock: " << strerror(errno) << endl;
  }
  this->desadosarContadores();
}

void Contadores :: incrementarResidentesDetenidos() {
  if (this->crearContadores() < 0) {
    Logger :: getInstance()->registrar("Error al crear la memoria compartida para consultas");
    return;
  }

  LockFile lock(ARCHIVO_RD);
  if (lock.tomarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al tomar el lock: " << strerror(errno) << endl;
  }
  Logger :: getInstance()->registrar("Se incrementan los residentes detenidos en la memoria compartida");
  int resultado = residentesDetenidos.leer();
  int nuevo = resultado + 1;
  char resBuffer[sizeof(resultado)];
  sprintf(resBuffer, "%d", resultado);
  char nuevoBuffer[sizeof(nuevo)];
  sprintf(nuevoBuffer, "%d", nuevo);
  Logger :: getInstance()->registrar("Cambia cantidad de residentes detenidos: " + string(resBuffer) + " por cantidad " + string(nuevoBuffer));
  residentesDetenidos.escribir(nuevo);
  if (lock.liberarLock() < 0) {
    if (errno == EINTR) {
      exit(0);
    }
    cerr << "Error al liberar el lock: " << strerror(errno) << endl;
  }
  this->desadosarContadores();
}

void Contadores :: desadosarContadores() {
  this->turistasDeportados.desadosar();
  this->turistasIngresados.desadosar();
  this->residentesDetenidos.desadosar();
}

void Contadores :: liberarContadores() {
  this->turistasDeportados.liberar();
  this->turistasIngresados.liberar();
  this->residentesDetenidos.liberar();
}
