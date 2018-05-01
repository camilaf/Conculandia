#include "FifoLectura.h"

FifoLectura :: FifoLectura(const string nombre) : Fifo(nombre) {
}

FifoLectura :: ~FifoLectura () {
}

void FifoLectura :: abrir() {
  this->fd = open(this->nombre.c_str(), O_RDONLY);
  if (this->fd < 0) {
    cerr << "Error al abrir el fifo: " << strerror(errno) << endl;
  }
}

ssize_t FifoLectura :: leer(void* buffer, const ssize_t buffsize) const {
  return read(this->fd, buffer, buffsize);
}
