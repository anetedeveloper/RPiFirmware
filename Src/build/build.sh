# qmake -qt=qt5 ++umiestnenieProjektu++ ++pridaneDefines++
#qmake -qt=qt5 ../ "DEFINES+=ForRaspberryPi"
qmake -qt=qt5 ../
make
rm -f *.o *.h *.cpp Makefile
