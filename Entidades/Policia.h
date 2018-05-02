#ifndef POLICIA_H_
#define POLICIA_H_

#include <string>
#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <algorithm>
#include "../Logger.h"
#include "../Fifo/FifoEscritura.h"
#include "../Seniales/SIGINT_Handler.h"
#include "../Seniales/SignalHandler.h"

#define ARCHIVO_ALERTAS "/tmp/fifo_alertas"
#define CANT_TOTAL_RASGOS 25
#define PORCENTAJE_PARA_REMOVER 20

using namespace std;

class Policia {
private:
  vector<string> rasgosRestantes;
  vector<string> rasgosActuales;
  string seleccionarRasgo(vector<string> listaRasgos);
  int enviarAlerta(FifoEscritura fifoAlertas);

public:

  Policia();
  ~Policia();
  void iniciarAlertas();

};

#endif /* POLICIA_H_ */
