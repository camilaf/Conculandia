#include "LockFile.h"

LockFile :: LockFile(const string nombre) {
  this->nombre = nombre;
  this->fl.l_type = F_WRLCK;
  this->fl.l_whence = SEEK_SET;
  this->fl.l_start = 0;
  this->fl.l_len = 0;
  this->fd = open(this->nombre.c_str(), O_CREAT | O_WRONLY, 0777);
  if (this->fd < 0) {
    cerr << "open() failure: " << strerror(errno) << endl;
  }
}

int LockFile :: tomarLock() {
  this->fl.l_type = F_WRLCK;
  return fcntl(this->fd, F_SETLKW, &(this->fl));
}

int LockFile :: liberarLock() {
  this->fl.l_type = F_UNLCK;
  return fcntl(this->fd, F_SETLK, &(this->fl));
}

LockFile :: ~LockFile () {
  close(this->fd);
}
