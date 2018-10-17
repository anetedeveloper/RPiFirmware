# Rallo

## Ubuntu
### make app
```
rm -f QTTcpServer
svn revert --recursive ~/Documents/QtDeviceServer/
svn status ~/Documents/QtDeviceServer/ | grep ^\? | cut -c9- | xargs -d \\n rm $
svn up ~/Documents/QtDeviceServer/
qmake -qt=qt5 ~/Documents/QtDeviceServer/Trunk/Src/ "DEFINES+=ForRaspberryPi"
make
rm -f *.o *.h *.cpp Makefile
cp QTTcpServer ~/
```

## Ubuntu
### Add/Remove user
```
sudo adduser rallo
sudo usermod -aG sudo rallo
sudo visudo
rallo ALL=(ALL) NOPASSWD: ALL
```
### Change hostname
```
\etc\hostname
\etc\hosts
```
### Remove splash screen
```
sudo nano /boot/cmdline.txt
```
- add "logo.nologo"
```
sudo nano /boot/config.txt
```
 - add "disable_splash=1"
### Update device
```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get clean
sudo apt-get dist-upgrade
```
### Xserver Xorg
```
sudo apt-get install --no-install-recommends xserver-xorg xinit x11-xserver-utils
```
### Libs for running QT application
```
sudo apt-get install libqt5serialport5-dev
```
### VNC Server
```
sudo apt-get install x11vnc
x11vnc -storepasswd
echo "alias vnc='x11vnc -rfbauth .vnc/passwd -q -display :0 &'" | sudo tee -a ~/.bash_aliases
```
### Startup programs
```
sudo nano /etc/rc.local 

/home/rallo/scripts/temp.sh &
sudo xinit /home/rallo/RalloApp -p 15000 -- &
```
### Set timezone
```
sudo dpkg-reconfigure tzdata
```
### Test Serial read

```
sudo apt install minicom
sudo minicom -s
```
 - Serial device /dev/serial0
 - 9600 8N1
 - CLOSE: CTRL+A, X
 

## Export exe z QT

```

run:
Qt 5.7 64-bit for Desktop (MSVC 2015)

Usage: windeployqt.exe [options] [files]

cd C:\projekty\QtDeviceServerTrunk\build-Rallo-Desktop_Qt_5_7_0_MSVC2015_64bit-Release\release
windeployqt.exe --quick .
COPY /-Y RalloApp.exe "..\..\..\ctecky\Trunk\Src\Emulators\QtEmulator\QTTcpServer.exe"

```
