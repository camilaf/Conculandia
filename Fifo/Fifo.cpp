#include "Fifo.h"

Fifo :: Fifo(const string nombre) : nombre(nombre), fd(-1) {
}

int Fifo :: crear(const string nombre) {
  int res = mknod(static_cast<const char*> (nombre.c_str()), S_IFIFO | 0666, 0);
  if (res < 0) {
    cerr << "Error al crear el fifo: " << strerror(errno) << endl;
    return -1;
  }
  return 0;
}

Fifo :: ~Fifo () {
}

void Fifo :: cerrar() {
  if (this->fd >= 0) {
    close(this->fd);
    this->fd = -1;
  }
}

void Fifo :: eliminar() const {
  unlink(this->nombre.c_str());
}
