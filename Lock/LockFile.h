#ifndef LOCKFILE_H_
#define LOCKFILE_H_

#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <cerrno>

using namespace std;

class LockFile {
private:
  struct flock fl;
  int fd;
  string nombre;


public:
  LockFile(const string nombre);
  ~LockFile();

  int tomarLock();
  int liberarLock();

};

#endif /* LOCKFILE_H_ */
