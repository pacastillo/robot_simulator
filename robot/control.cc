#include "sincronizador.h"

void pedir_estado(void) {
    sincronizador *sincr = new sincronizador();
    vector<unsigned> sensores = (vector<unsigned>) sincr->recibir("S");

    cout << "sensores_ANTES_DE_PEDIR_INFORMACION: <";
    for(unsigned i=0  ;  i < sensores.size()  ;  i++)  cout << " " << sensores[i] ;
    cout << " > " << endl;

    vector<unsigned> acciones; // enviarle valores en [0,100)
    cout << "\t CONTROL :: enviamos la peticion de datos..." << endl;    
    acciones.push_back(DATO_CTRL_PETICION); 
    acciones.push_back(DATO_CTRL_DATOS);

    sincr->enviar("A", acciones);
}

void recoger_estado(void) {
    sincronizador *sincr = new sincronizador();
    vector<unsigned> sensores = (vector<unsigned>) sincr->recibir("S");

    cout << "estado_del_robot: <";
    for(unsigned i=0  ;  i < sensores.size()  ;  i++)  cout << " " << sensores[i] ;
    cout << " > " << endl;

    vector<unsigned> acciones; // enviarle valores en [0,100)
    cout << "\t CONTROL :: enviamos la peticion de reiniciar..." << endl;
    acciones.push_back(DATO_CTRL_PETICION); 
    acciones.push_back(DATO_CTRL_OK);

    sincr->enviar("A", acciones);
}

void parar_robot(void) {
    sincronizador *sincr = new sincronizador();
    vector<unsigned> sensores = (vector<unsigned>) sincr->recibir("S");

    cout << "estado_del_robot: <";
    for(unsigned i=0  ;  i < sensores.size()  ;  i++)  cout << " " << sensores[i] ;
    cout << " > " << endl;

    vector<unsigned> acciones; // enviarle valores en [0,100)
    cout << "\t CONTROL :: enviamos la peticion de PARAR..." << endl;
    acciones.push_back(DATO_CTRL_PETICION); 
    acciones.push_back(DATO_CTRL_PARAR);

    sincr->enviar("A", acciones);
}

int main(void) {
  srand( time(0) );

  for(unsigned cont=0; cont<500; cont++){
    // RECOGER LOS valores de los sensores
    // APLICAR LOS valores AL MLP
    // OBTENER SALIDAS DEL MLP


    sincronizador *sincr = new sincronizador();
    vector<unsigned> sensores = (vector<unsigned>) sincr->recibir("S");

    cout << "sensores: <";
    for(unsigned i=0  ;  i < sensores.size()  ;  i++) {
      cout << " " << sensores[i] ;
    }
    cout << " > " << endl;

    vector<unsigned> acciones; // enviarle valores en [0,100)

    // si ha llegado a la meta... parar (que no se mueva)
    if( sensores[3]<20 ){
      cout << "\t CONTROL :: pedimos datos..." << endl;    
      acciones.push_back(DATO_CTRL_PETICION); 
      acciones.push_back(DATO_CTRL_PARAR);  // parar

    }else{
      
      if( sensores[0] && !sensores[1] && sensores[2] ) { //i=n,c=0,d=m
	acciones.push_back(30); acciones.push_back(30);  // avanzar
	cout << "\t AVANZAR" << endl;
      }
      
      if( !sensores[1] ) { //i=0,c=0,d=0
	acciones.push_back(30); acciones.push_back(30);  // avanzar
	cout << "\t AVANZAR" << endl;
      }
      
      if( sensores[0] && sensores[1] && !sensores[2] ) { //i=n,c=m,d=0
	acciones.push_back(30); acciones.push_back(5);  // giro dcha
	cout << "\t GIRO DCHA" << endl;
      }
      
      if( !sensores[0] && sensores[1] && sensores[2] ) { //i=0,c=m,d=n
	acciones.push_back(5); acciones.push_back(30);  // giro izq
	cout << "\t GIRO IZQ" << endl;
      }
      
      if( !sensores[0] && sensores[1] && !sensores[2] ) { //i=0,c=n,d=0
	unsigned r = 1+(int) (100.0*rand()/(RAND_MAX+1.0));
	if( r<50 ){
	  acciones.push_back(30); acciones.push_back(5);  // giro dcha
	  cout << "\t GIRO DCHA **********************" << endl;
	}else{
	  acciones.push_back(5); acciones.push_back(30);  // giro izq
	  cout << "\t GIRO IZQ **********************" << endl;
	}
      }
      
      if( sensores[0] && sensores[1] && sensores[2] ) { //i=n,c=m,d=r
	if( sensores[0] > sensores[2] ) {
	  acciones.push_back(30); acciones.push_back(0);  // giro dcha
	  cout << "\t GIRO DCHA in-extremis" << endl;
	}else{
	  if( sensores[2] > sensores[0] ) {
	    acciones.push_back(0); acciones.push_back(30);  // giro izq
	    cout << "\t GIRO IZQ in-extremis" << endl;
	  }else{
	    unsigned r = 1+(int) (100.0*rand()/(RAND_MAX+1.0));
	    if( r<50 ){
	      acciones.push_back(30); acciones.push_back(5);  // giro dcha
	      cout << "\t GIRO DCHA --------------------" << endl;
	    }else{
	      acciones.push_back(5); acciones.push_back(30);  // giro izq
	      cout << "\t GIRO IZQ ---------------------" << endl;
	    }
	  }
	}
      }

    }
    
    //enviar las actuaciones
    sincr->enviar("A", acciones);

  }

  pedir_estado();
  sleep(3);
  recoger_estado();
  sleep(3);
  parar_robot();

  return 0;
}
