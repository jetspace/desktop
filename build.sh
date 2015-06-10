#!/bin/bash
#Simple build script

#BUILDING LIBS
cd lib/side-app
make
make install
cd ../side-plugin
make
make install
cd ../side-log
make
make install
cd ../..

#BUILDING BIN
cd panel
make
cd ../wallpaper-service
make
cd ../settings
make
cd ../plugins
make
cd ../session
make
cd ../utility
make
cd ../notify
make
cd ../search
make

#tools
cd ../tools
cd editor
make
cd ..
cd ..


#Install and Run
if [ "$1" == "-r" ]
then
sudo make install
make
fi

#Install
if [ "$1" == "-i" ]
then
sudo make install
fi
