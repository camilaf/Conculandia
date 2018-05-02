#include "Logger.h"

#define TIMESTAMP_SIZE 30

Logger* Logger :: instance = NULL;
bool Logger :: modoDebug = false;

Logger :: Logger() {
  this->fd = open(LOGFILE, O_WRONLY | O_CREAT, 0644);
  if (this->fd < 0) {
    cerr << "Error al abrir el log: " << strerror(errno) << endl;
  }
}

void Logger :: setDebug(bool debug) {
  modoDebug = debug;
}

Logger* Logger :: getInstance() {
  if (instance == NULL) {
    instance = new Logger();
  }
  return instance;
}

Logger :: ~Logger () {
  close(this->fd);
}

void Logger :: destruir() {
  if (instance != NULL) {
    delete(instance);
    instance = NULL;
  }
}

void Logger :: registrar(string mensaje) {
  if (modoDebug) {
    LockFile lock("log.txt");
    if (lock.tomarLock() < 0) {
      if (errno == EINTR) {
        exit(0);
      }
      cerr << "Error al tomar el lock: " << strerror(errno) << endl;
    }
    char buffer[TIMESTAMP_SIZE];
    time_t now = time(0);
    strftime(buffer, TIMESTAMP_SIZE, "[ %Y-%m-%d %H:%M:%S ] ", localtime(&now));

    mensaje = string(buffer) + mensaje + "\n";
    lseek(this->fd, 0, SEEK_END);
    write(this->fd, mensaje.c_str(), mensaje.length());

    if (lock.liberarLock() < 0) {
      if (errno == EINTR) {
        exit(0);
      }
      cerr << "Error al liberar el lock: " << strerror(errno) << endl;
    }
  }
}
