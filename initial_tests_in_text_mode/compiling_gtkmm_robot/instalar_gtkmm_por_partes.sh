#!/bin/bash

tar xvfpz pkgconfig-0.14.0.tar.gz && tar xvfpz glib-2.2.1.tar.gz && tar xvfpz atk-1.2.0.tar.gz && tar xvfpz pango-1.2.1.tar.gz && tar xvfpz libsigc++-1.2.3.tar.gz && tar xvfpz gtk+-2.2.1.tar.gz && tar xvfpz gtkmm-2.2.1.tar.gz

cd pkgconfig-0.14.0 && ./configure && make && make install
cd ..

cd glib-2.2.1  && ./configure && make && make install
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
cd ..

cd atk-1.2.0 && ./configure && make && make install
cd ..

cd pango-1.2.1  && ./configure && make && make install
cd ..

cd libsigc++-1.2.3  && ./configure && make && make install
cd ..

cd gtk+-2.2.1  && ./configure && make && make install
cd ..

cd gtkmm-2.2.1 && ./configure && make && make install
cd ..

echo "."
echo "."
echo "."
echo " ----------- YA HE TERMINADO -------------"
