#!/bin/bash
#Simple build script
cd panel
make
cd ../wallpaper-service
make
cd ../settings
make
cd ..


if [ "$1" == "-r" ]
then
make
fi
