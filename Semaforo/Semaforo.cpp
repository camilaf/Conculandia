#include "Semaforo.h"

Semaforo :: Semaforo(const string& nombre, const int valorInicial) {
  this->valorInicial = valorInicial;
  key_t clave = ftok(nombre.c_str(), 'a');
  if (clave < 0) {
    cerr << "Error al generar la clave para el semaforo: " << strerror(errno) << endl;
  }
  this->id = semget(clave, 1, 0666 | IPC_CREAT);
  if (this->id < 0) {
    cerr << "Error al crear el semaforo: " << strerror(errno) << endl;
  }
  this->inicializar();
}

Semaforo :: ~Semaforo() {
}

int Semaforo :: inicializar() const {
  union semnum {
    int val;
    struct semid_ds* buf;
    ushort* array;
  };

  semnum init;
  init.val = this->valorInicial;
  int resultado = semctl(this->id, 0, SETVAL, init);
  if (resultado < 0) {
    cerr << "Error al inicializar el semaforo: " << strerror(errno) << endl;
  }
  return resultado;
}

int Semaforo :: p() const {
  struct sembuf operacion;

  operacion.sem_num = 0;
  operacion.sem_op = -1;
  operacion.sem_flg = SEM_UNDO;

  int resultado = semop(this->id, &operacion, 1);
  if (resultado < 0) {
    cerr << "Error al efectuar la operacion p sobre el semaforo: " << strerror(errno) << endl;
  }
  return resultado;
}

int Semaforo :: v() const {
  struct sembuf operacion;

  operacion.sem_num = 0;
  operacion.sem_op = 1;
  operacion.sem_flg = SEM_UNDO;

  int resultado = semop(this->id, &operacion, 1);
  if (resultado < 0) {
    cerr << "Error al efectuar la operacion v sobre el semaforo: " << strerror(errno) << endl;
  }
  return resultado;
}

void Semaforo :: eliminar() const {
  semctl(this->id, 0, IPC_RMID);
}
