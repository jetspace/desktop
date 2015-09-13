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
cd ../side-config
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
cd ../search
make
cd ../open
make

#tools
cd ../tools
cd editor
make
cd ../view
make
cd ../filemanager
make
cd ..
cd ..


#Install and Run
if [ "$1" == "-r" ]
then
sudo make install
make installuserconf
make
fi

#Install
if [ "$1" == "-i" ]
then
sudo make install
make installuserconf
fi
