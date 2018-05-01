#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <fstream>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include "Lock/LockFile.h"
#define LOGFILE "log.txt"
using namespace std;

class Logger {
private:
  static Logger* instance;
  static bool modoDebug;
  int fd;

public:
  Logger();
  ~Logger();

  static void setDebug(bool debug);
  void registrar(string mensaje);
  static Logger* getInstance();
  static void destruir();

};

#endif /* LOGGER_H_ */
