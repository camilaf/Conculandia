#ifndef POLICIA_H_
#define POLICIA_H_

#include <string>
#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include "../Logger.h"

#define CANT_TOTAL_RASGOS 25
#define PORCENTAJE_PARA_REMOVER 40

using namespace std;

class Policia {
private:
  string rasgos[CANT_TOTAL_RASGOS];
  vector<string> listadoActual;

public:

  Policia();
  ~Policia();
  void enviarAlerta();

};

#endif /* POLICIA_H_ */
