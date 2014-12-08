#include "sincronizador.h"

int main(void) {

  for(unsigned cont=0; cont<10; cont++){
    // OBTENER LOS VALORES DE LOS SENSORES EN sensores

    vector<unsigned> sensores; sensores.push_back(cont); sensores.push_back(6); sensores.push_back(29);
    for(unsigned j=0  ;  j < sensores.size()  ;  j++) {
      cout << "ROBOT:valores_sensores => " << sensores[j] << endl;
    }
    
    //enviarlos y esperar recibir una actuación
    sincronizador *sincr = new sincronizador();
    sincr->enviar("S",sensores);
    vector<unsigned> actuacion = sincr->recibir("A");
    
    // PINTAR EL ROBOT EN SU NUEVA POSICIÓN SEGUN LA ACTUACIÓN
    for(unsigned i=0  ;  i < actuacion.size()  ;  i++) {
      cout << "P2:actuacion <= " << actuacion[i] << endl;
    }

    cout << endl << endl << endl;
    sleep(3);
  }
  return 0;
}
