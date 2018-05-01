#include "Asistente.h"

#define PORCENTAJE_CAPTURA 25
#define TAM 25

Asistente :: Asistente() {
}

Asistente :: ~Asistente () {
}

void Asistente ::iniciarMenu() {
  int opcion;
  cout << endl;
  cout << "BIENVENIDOS A LA OFICINA DE MIGRACIONES DE CONCULANDIA!" << endl << endl;
  while(opcion != 4) {
    cout << "** Menu Principal **: Elija entre las siguientes opciones:" << endl << endl;
    cout << "- Ingrese 1 para consultar la cantidad de turistas extranjeros deportados" << endl;
    cout << "- Ingrese 2 para consultar la cantidad de turistas extranjeros que ingresaron al pais" << endl;
    cout << "- Ingrese 3 para consultar la cantidad de residentes de Conculandia enviados a la Oficina de Policia" << endl;
    cout << "- Ingrese 4 para salir" << endl << endl;
    cin >> opcion;
    while (cin.fail()) {
      cin.clear();
      cin.ignore();
      cout << "Ingrese una opcion valida: " << endl;
      cin >> opcion;
    }

    if (opcion == 1) {
      cout << "-> Turistas deportados de Conculandia: " << this->consultarTuristasDeportados() << endl << endl;
    }

    if (opcion == 2) {
      cout << "-> Turistas que ingresaron a Conculandia: " << this->consultarTuristasIngresados() << endl << endl;
    }

    if (opcion == 3) {
      cout << "-> Residentes de Conculandia enviados a la Oficina de Policia: " << this->consultarResidentesDetenidos() << endl << endl;
    }
  }
}

int Asistente :: verificarArgumentos(int argc, char *argv[]) {
  this->debug = false;
  if (argc < CANT_MIN_PARAMETROS) {
    cout << "Argumentos faltantes" << endl;
    return -1;
  }

  int argumento;
  while ((argumento = getopt(argc, argv, "v:s:d")) != -1) {
    switch (argumento) {
      case 'v':
        this->ventanillas = atoi(optarg);
        break;
      case 's':
        this->sellos = atoi(optarg);
        break;
      case 'd':
        this->debug = true;
        break;
      default:
        cerr << "Error al parsear los argumentos" << endl;
        return -1;
    }
  }

  if ((this->ventanillas < 1) || (this->sellos < 1)) {
    cerr << "El valor de ventanillas y sellos debe ser mayor a cero" << endl;
    return -1;
  }
  Logger :: setDebug(this->debug);
  Logger :: getInstance()->registrar("------------- Configuracion inicial -------------");

  char buffer[TAM];
  sprintf(buffer, "%d", ventanillas);
  Logger :: getInstance()->registrar("Cantidad de ventanillas: " + string(buffer));
  sprintf(buffer, "%d", sellos);
  Logger :: getInstance()->registrar("Cantidad de sellos: " + string(buffer));

  return 0;
}

int Asistente :: inicializarContadoresConsultas() {
  return c.inicializarContadores();
}

int Asistente :: obtenerCantidadVentanillas() {
  return this->ventanillas;
}

int Asistente :: consultarTuristasDeportados() {
  return c.cantidadTuristasDeportados();
}

int Asistente :: consultarTuristasIngresados() {
  return c.cantidadTuristasIngresados();
}

int Asistente :: consultarResidentesDetenidos() {
  return c.cantidadResidentesDetenidos();
}

void Asistente :: liberarContadoresConsultas() {
  Logger :: getInstance()->registrar("Liberando contadores en memoria compartida");
  c.liberarContadores();
}
