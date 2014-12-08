CXX=g++ 
CXXFLAGS= -Wall -O4 # -g

###############################################################################
FILES = control.cc robot.cc sincronizador.h Makefile
SRC = control.cc robot.cc
BIN = control robot

###############################################################################

all:	${BIN}

clean:	
	rm -f ${BIN} *~ *.o core A S

control:	control.cc sincronizador.h Makefile
	g++  ${CXXFLAGS}  control.cc -o control
robot:	robot.cc sincronizador.h Makefile
	g++ -DHAVE_CONFIG_H  -I.  -I/usr/local/include/gtkmm-2.0 -I/usr/local/lib/gtkmm-2.0/include   -I/usr/local/lib/sigc++-1.2/include -I/usr/local/include/sigc++-1.2 -I/usr/local/include/gtk-2.0 -I/usr/local/lib/gtk-2.0/include -I/usr/local/include/atk-1.0 -I/usr/local/include/pango-1.0 -I/usr/X11R6/include -I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include    -O2 -Wall -c -o robot.o `test -f 'robot.cc' || echo './'`robot.cc           &&            g++   ${CXXFLAGS}  -o robot robot.o -Wl,--export-dynamic  /usr/local/lib/libglibmm-2.0.so -L/usr/local/lib /usr/local/lib/libpangomm-1.0.so /usr/local/lib/libatkmm-1.0.so /usr/local/lib/libgdkmm-2.0.so /usr/local/lib/libgtkmm-2.0.so /usr/local/lib/libsigc-1.2.so /usr/local/lib/libgtk-x11-2.0.so /usr/local/lib/libgdk-x11-2.0.so /usr/local/lib/libatk-1.0.so /usr/local/lib/libgdk_pixbuf-2.0.so -lm /usr/local/lib/libpangox-1.0.so /usr/local/lib/libpango-1.0.so /usr/local/lib/libgobject-2.0.so /usr/local/lib/libgmodule-2.0.so -ldl /usr/local/lib/libglib-2.0.so -Wl,--rpath -Wl,/usr/local/lib

etsii:	robot.cc sincronizador.h Makefile
	g++ -DHAVE_CONFIG_H  -I.  -I/fenix/depar/atc/pedro/gtkmm/include/gtkmm-2.0 -I/fenix/depar/atc/pedro/gtkmm/lib/gtkmm-2.0/include   -I/fenix/depar/atc/pedro/gtkmm/lib/sigc++-1.2/include -I/fenix/depar/atc/pedro/gtkmm/include/sigc++-1.2 -I/fenix/depar/atc/pedro/gtkmm/include/gtk-2.0 -I/fenix/depar/atc/pedro/gtkmm/lib/gtk-2.0/include -I/fenix/depar/atc/pedro/gtkmm/include/atk-1.0 -I/fenix/depar/atc/pedro/gtkmm/include/pango-1.0 -I/usr/X11R6/include -I/fenix/depar/atc/pedro/gtkmm/include/glib-2.0 -I/fenix/depar/atc/pedro/gtkmm/lib/glib-2.0/include    -g -O2 -Wall -c -o robot.o `test -f 'robot.cc' || echo './'`robot.cc              &&            g++  ${CXXFLAGS}  -o robot robot.o -Wl,--export-dynamic  /fenix/depar/atc/pedro/gtkmm/lib/libglibmm-2.0.so -L/fenix/depar/atc/pedro/gtkmm/lib /fenix/depar/atc/pedro/gtkmm/lib/libpangomm-1.0.so /fenix/depar/atc/pedro/gtkmm/lib/libatkmm-1.0.so /fenix/depar/atc/pedro/gtkmm/lib/libgdkmm-2.0.so /fenix/depar/atc/pedro/gtkmm/lib/libgtkmm-2.0.so /fenix/depar/atc/pedro/gtkmm/lib/libsigc-1.2.so /fenix/depar/atc/pedro/gtkmm/lib/libgtk-x11-2.0.so /fenix/depar/atc/pedro/gtkmm/lib/libgdk-x11-2.0.so /fenix/depar/atc/pedro/gtkmm/lib/libatk-1.0.so /fenix/depar/atc/pedro/gtkmm/lib/libgdk_pixbuf-2.0.so -lm /fenix/depar/atc/pedro/gtkmm/lib/libpangox-1.0.so /fenix/depar/atc/pedro/gtkmm/lib/libpango-1.0.so /fenix/depar/atc/pedro/gtkmm/lib/libgobject-2.0.so /fenix/depar/atc/pedro/gtkmm/lib/libgmodule-2.0.so -ldl /fenix/depar/atc/pedro/gtkmm/lib/libglib-2.0.so -Wl,--rpath -Wl,/fenix/depar/atc/pedro/gtkmm/lib

###############################################################################

.NOEXPORT:
