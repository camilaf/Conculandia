#ifndef SEMAFORO_H_
#define SEMAFORO_H_

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <cstring>
#include <iostream>
#include <cerrno>

using namespace std;

class Semaforo {
private:
  int id;
  int valorInicial;

  int inicializar() const;

public:

  Semaforo(const string& nombre, const int valorInicial);
  ~Semaforo();

  int p() const;
  int v() const;
  void eliminar() const;

};

#endif /* SEMAFORO_H_ */
