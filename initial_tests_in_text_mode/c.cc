#include "sincronizador.h"

int main(void) {
  //esperar los valores de los sensores

  for(unsigned cont=0; cont<10; cont++){

    sincronizador *sincr = new sincronizador();
    vector<unsigned> valores = sincr->recibir("S");
    
    // RECOGER LOS valores de los sensores
    // APLICAR LOS valores AL MLP
    // OBTENER SALIDAS DEL MLP
    for(unsigned i=0  ;  i < valores.size()  ;  i++) {
      cout << " <- ARENA:valores_sensores <= " << valores[i] << endl;
    }
    vector<unsigned> acciones; 
                    acciones.push_back(90); 
		    acciones.push_back(20);
    for(unsigned j=0  ;  j < acciones.size()  ;  j++) {
      cout << " -> CTRL: decision(act) => " << acciones[j] << endl;
    }
    
    //enviar las actuaciones
    sincr->enviar("A",acciones);

    cout << endl << endl << endl;
  }
  return 0;
}
