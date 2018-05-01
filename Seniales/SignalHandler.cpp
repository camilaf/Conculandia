#include "SignalHandler.h"

SignalHandler* SignalHandler :: instance = NULL;
EventHandler* SignalHandler :: signal_handlers[NSIG];

SignalHandler :: SignalHandler() {
}

SignalHandler* SignalHandler :: getInstance() {
  if (instance == NULL) {
    instance = new SignalHandler();
  }
  return instance;
}

void SignalHandler :: destruir() {
  if (instance != NULL) {
    delete(instance);
    instance = NULL;
  }
}

EventHandler* SignalHandler :: registrarHandler(int signum, EventHandler* eh) {
  EventHandler* old_eh = SignalHandler :: signal_handlers[signum];
  SignalHandler :: signal_handlers[signum] = eh;

  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = SignalHandler :: dispatcher;
  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, signum);
  int res = sigaction(signum, &sa, 0);
  if (res < 0) {
    cerr << "Error al asociar un signal handler: " << strerror(errno) << endl;
  }
  return old_eh;
}

void SignalHandler :: dispatcher(int signum) {
  if (SignalHandler :: signal_handlers[signum] != 0) {
    SignalHandler :: signal_handlers[signum]->handleSignal(signum);
  }
}

int SignalHandler :: removerHandler(int signum) {
  SignalHandler :: signal_handlers[signum] = 0;
  return 0;
}
