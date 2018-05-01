#ifndef FIFO_H_
#define FIFO_H_

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cerrno>
using namespace std;

class Fifo {
public:

  Fifo(const string nombre);
  static int crear(const string nombre);
  virtual ~Fifo();
  virtual void abrir() = 0;
  void cerrar();
  void eliminar() const;

protected:
  string nombre;
  int fd;
};

#endif /* FIFO_H_ */
