#!/bin/bash

echo "compilando el CONTROL"

g++ -Wall control.cc -o control

echo "compilando el ROBOT"

g++ -DHAVE_CONFIG_H  -I.  -I/tmp/gtkmm/include/gtkmm-2.0 -I/tmp/gtkmm/lib/gtkmm-2.0/include   -I/tmp/gtkmm/lib/sigc++-1.2/include -I/tmp/gtkmm/include/sigc++-1.2 -I/tmp/gtkmm/include/gtk-2.0 -I/tmp/gtkmm/lib/gtk-2.0/include -I/tmp/gtkmm/include/atk-1.0 -I/tmp/gtkmm/include/pango-1.0 -I/usr/X11R6/include -I/tmp/gtkmm/include/glib-2.0 -I/tmp/gtkmm/lib/glib-2.0/include    -O2 -Wall -c -o robot.o `test -f 'robot.cc' || echo './'`robot.cc

g++  -O2 -Wall -o robot robot.o -Wl,--export-dynamic  /tmp/gtkmm/lib/libglibmm-2.0.so -L/tmp/gtkmm/lib /tmp/gtkmm/lib/libpangomm-1.0.so /tmp/gtkmm/lib/libatkmm-1.0.so /tmp/gtkmm/lib/libgdkmm-2.0.so /tmp/gtkmm/lib/libgtkmm-2.0.so /tmp/gtkmm/lib/libsigc-1.2.so /tmp/gtkmm/lib/libgtk-x11-2.0.so /tmp/gtkmm/lib/libgdk-x11-2.0.so /tmp/gtkmm/lib/libatk-1.0.so /tmp/gtkmm/lib/libgdk_pixbuf-2.0.so -lm /tmp/gtkmm/lib/libpangox-1.0.so /tmp/gtkmm/lib/libpango-1.0.so /tmp/gtkmm/lib/libgobject-2.0.so /tmp/gtkmm/lib/libgmodule-2.0.so -ldl /tmp/gtkmm/lib/libglib-2.0.so -Wl,--rpath -Wl,/tmp/gtkmm/lib
