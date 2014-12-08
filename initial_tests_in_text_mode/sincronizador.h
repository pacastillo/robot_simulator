#ifndef _sincronizador_
#define _sincronizador_

#include <unistd.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#define DATO_CTRL_PETICION   10000
#define DATO_CTRL_PARAR      0
#define DATO_CTRL_REINICIAR  1
#define DATO_CTRL_DATOS      2
#define DATO_CTRL_TERMINAR   3

using namespace std;

class sincronizador {
 public:
  sincronizador(void) {}
  ~sincronizador() {}

  vector<unsigned> recibir(char * _F) {
    ifstream f;
    vector<unsigned> valores;
    unsigned s=0;

    //esperamos a que haya datos en el fichero S
    unsigned hay_datos=0;
    do {
      f.open( _F );
      if( f ) {
	f >> s;
	if( !f.eof() ){ //está vacío => esperar
	  hay_datos=1;
	}
	f.close();
	f.clear();
      }else{  //no existe aún el fich => esperar
	f.close();
	f.clear();

	sleep(1);
	/*
        for(unsigned tmpI=0;tmpI<1000;tmpI++) {
                for(unsigned tmpJ=0;tmpJ<65000;tmpJ++) {
                        asm(
                            "nop\t\n" 
                            "nop\t\n" 
                            "nop\t\n" 
                            "nop\t\n"
                        );
                }
        }
	*/
      }
    }while( !hay_datos );

    //leer los valores de los sensores (vienen en el fich S)
    f.clear();
    f.open( _F );
    f >> s; valores.push_back(s);
    f >> s; valores.push_back(s);
    f >> s; valores.push_back(s);
    f >> s; valores.push_back(s);

    f >> s; valores.push_back(s);
    f >> s; valores.push_back(s);
    f >> s; valores.push_back(s);
    f >> s; valores.push_back(s);
    f >> s; valores.push_back(s);
    f.close();
    f.clear();

    //limpiar los datos del _F
    ofstream tmp;
    tmp.open( _F , ios::trunc);
    tmp.close();
    tmp.clear();
    return (vector<unsigned>)valores;
  }

  //escribir esas salidas en A
  void enviar(char * _F, vector<unsigned> &_actuadores) {
    //limpiar los datos del _F
    ofstream tmp;
    tmp.open( _F , ios::trunc);
    tmp.close();
    tmp.clear();

    ofstream f;
    f.open( _F , ios::trunc);
    for(unsigned j=0 ; j< _actuadores.size() ; j++) {
      f << _actuadores[j] << " ";
    }
    f << endl;
    f.close();
    f.clear();
  }

};

#endif
