#ifndef SIGNALHANDLER_H_
#define SIGNALHANDLER_H_

#include <signal.h>
#include <stdio.h>
#include <memory.h>
#include <string>
#include <iostream>
#include <cerrno>

#include "EventHandler.h"
using namespace std;

class SignalHandler {
private:
  static SignalHandler* instance;
  static EventHandler* signal_handlers[NSIG];

  SignalHandler();
  static void dispatcher(int signum);

public:
  static SignalHandler* getInstance();
  static void destruir();
  EventHandler* registrarHandler(int signum, EventHandler* eh);
  int removerHandler(int signum);
};

#endif /* SIGNALHANDLER_H_ */
