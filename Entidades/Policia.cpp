#include "Policia.h"

Policia :: Policia() {
  // Todos los rasgos posibles
  string rasgos[CANT_TOTAL_RASGOS] = {"ojos:marrones", "ojos:azules", "ojos:verdes", "ojos:grises", "pelo:rubio", "pelo:negro", "pelo:gris", "pelo:marron oscuro", "pelo:marron claro", "pelo:rojo", "pelo:rosa", "pelo:azul", "pelo:violeta", "piel:clara", "piel:media", "piel:oscura", "genero:femenino", "genero:masculino", "elemento:sombrero", "elemento:gorra", "elemento:pelo suelto", "elemento:trenza", "elemento:cola de caballo", "elemento:rastas", "elemento:lentes"};
  for (int i = 0; i < CANT_TOTAL_RASGOS; i++) {
    this->rasgosRestantes.push_back(rasgos[i]);
  }
}

Policia :: ~Policia() {
}

/* Se encarga de abrir el fifo de escritura sobre el cual se comunicara
  con el Ministro de Seguridad, indicandole si debe quitar o agregar
  rasgos del listado de Personas de Riesgo. Envia alertas entre 5 y
  18 segundos. */
void Policia :: iniciarAlertas() {
  SIGINT_Handler sigint_handler;
  SignalHandler :: getInstance()->registrarHandler(SIGINT, &sigint_handler);

  Logger :: getInstance()->registrar("La policia se prepara para enviar alertas");
  FifoEscritura fifoAlertas(ARCHIVO_ALERTAS);
  fifoAlertas.abrir();

  while (sigint_handler.getGracefulQuit() == 0) {
    if (this->enviarAlerta(fifoAlertas) < 0) {
      break;
    }
    Logger :: getInstance()->registrar("Alerta enviada");

    srand(time(0));
    int tiempoEntreAlertas = (rand() % 14) + 5;
    sleep(tiempoEntreAlertas);
  }
  Logger :: getInstance()->registrar("La policia cierra el fifo de alertas");
  fifoAlertas.cerrar();
  fifoAlertas.eliminar();
}

/* Recibe un fifo de escritura sobre el cual enviara la alerta con la
  indicacion seguida del rasgo a agregar o quitar. Devuelve -1 si
  fue interrumpido por una senial. */
int Policia :: enviarAlerta(FifoEscritura fifoAlertas) {
  srand(time(0));
  string indicacion;
  string rasgo;

  // Agrega un rasgo si no hay rasgos actualmente en el listado, y no se agrega si ya se encuentran todos los rasgos
  if ((this->rasgosActuales.size() == 0) || ((rand() % 100 >= PORCENTAJE_PARA_REMOVER) && (this->rasgosActuales.size() != CANT_TOTAL_RASGOS))) {
    // Agrego rasgo
    rasgo = this->seleccionarRasgo(this->rasgosRestantes);
    Logger :: getInstance()->registrar("Se envia alerta para agregar rasgo " + rasgo);

    // Borramos el rasgo de la lista de rasgos restantes
    vector<string> :: iterator it = find(this->rasgosRestantes.begin(), this->rasgosRestantes.end(), rasgo);
    if (it != this->rasgosRestantes.end()) {
      this->rasgosRestantes.erase(it);
    }

    // Agregamos el rasgo en el listado
    this->rasgosActuales.push_back(rasgo);
    indicacion = "Agregar";
  }
  else {
    // Elimino rasgo
    rasgo = this->seleccionarRasgo(this->rasgosActuales);
    Logger :: getInstance()->registrar("Se envia alerta para eliminar rasgo " + rasgo);

    // Borramos el rasgo de la lista de rasgos restantes
    vector<string> :: iterator it = find(this->rasgosActuales.begin(), this->rasgosActuales.end(), rasgo);
    if (it != this->rasgosActuales.end()) {
      this->rasgosActuales.erase(it);
    }

    // Agregamos el rasgo en el listado
    this->rasgosRestantes.push_back(rasgo);
    indicacion = "Quitar";
  }

  string mensaje = indicacion + "-" + rasgo + "-";
  Logger :: getInstance()->registrar("La policia envia la alerta " + mensaje);
  
  // Enviamos la alerta
  if (fifoAlertas.escribir(mensaje.c_str(), mensaje.length()) < 0) {
    if (errno == EINTR) {
      return -1;
    }
    cerr << "Error al escribir en el fifo de alertas: " << strerror(errno) << endl;
  }
  return 0;
}

/* Recibe un vector con rasgos, selecciona uno al azar y lo devuelve. */
string Policia :: seleccionarRasgo(vector<string> listaRasgos) {
  srand(time(0));
  string rasgo = listaRasgos[rand() % listaRasgos.size()];
  return rasgo;
}
