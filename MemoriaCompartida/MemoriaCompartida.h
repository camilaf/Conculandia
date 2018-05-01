#ifndef MEMORIACOMPARTIDA_H_
#define MEMORIACOMPARTIDA_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <iostream>
#include <cerrno>
using namespace std;

template <class T> class MemoriaCompartida {
private:
  int shmId;
  T *ptrDatos;
  int cantidadProcesosAdosados() const;

public:

  MemoriaCompartida();
  ~MemoriaCompartida();
  int crear(const string archivo, const char letra, size_t tamanio);
  void liberar();
  void desadosar();
  void escribir(const T& dato);
  T leer() const;

};

template <class T> MemoriaCompartida<T> :: MemoriaCompartida() {
  this->shmId = 0;
  this->ptrDatos = NULL;
}

template <class T> MemoriaCompartida<T> :: ~MemoriaCompartida () {
}

template <class T> int MemoriaCompartida<T> :: crear(const string archivo, const char letra, size_t tamanio) {
  key_t clave = ftok(archivo.c_str(), letra);
  if (clave < 0) {
    cerr << "Error al generar la clave para la memoria compartida: " << strerror(errno) << endl;
    return -1;
  }
  else {
    this->shmId = shmget(clave, tamanio, 0644 | IPC_CREAT);
    if (this->shmId < 0) {
      cerr << "Error al crear la memoria compartida: " << strerror(errno) << endl;
      return -1;
    }
    else {
      void *ptrTemporal = shmat(this->shmId, NULL, 0);
      if (ptrTemporal == (void *) -1) {
        cerr << "Error al adosar la memoria compartida: " << strerror(errno) << endl;
        return -1;
      }

      this->ptrDatos = static_cast<T*> (ptrTemporal);
      return 0;
    }
  }
}

template <class T> void MemoriaCompartida<T> :: liberar() {
  int procesosAdosados = this->cantidadProcesosAdosados();
  if (procesosAdosados != 0) {
    int resuladoDetach = shmdt(static_cast<void *> (this->ptrDatos));
    if (resuladoDetach < 0) {
        cerr << "Error al desadosar la memoria compartida: " << strerror(errno) << endl;
    }
  }

  procesosAdosados = this->cantidadProcesosAdosados();
  if (procesosAdosados == 0) {
    int resultado = shmctl(this->shmId, IPC_RMID, NULL);
    if (resultado < 0) {
      cerr << "Error al destruir la memoria compartida: " << strerror(errno) << endl;
    }
  }
}

template <class T> void MemoriaCompartida<T> :: desadosar() {
  int resuladoDetach = shmdt(static_cast<void *> (this->ptrDatos));
  if (resuladoDetach < 0) {
      cerr << "Error al desadosar la memoria compartida: " << strerror(errno) << endl;
  }
}

template <class T> void MemoriaCompartida<T> :: escribir(const T& dato) {
  *(this->ptrDatos) = dato;
}

template <class T> T MemoriaCompartida<T> :: leer() const {
  return *(this->ptrDatos);
}

template <class T> int MemoriaCompartida<T> :: cantidadProcesosAdosados() const {
  shmid_ds estado;
  shmctl(this->shmId, IPC_STAT, &estado);
  return estado.shm_nattch;
}

#endif /* MEMORIACOMPARTIDA_H_ */
