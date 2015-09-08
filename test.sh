#!&usr/bin/env sh
#
# Copyright (C) 2015 Marius Messerschmidt
# This file is used for travis-ci automated testing
#

cd lib
cd side-app
make
make install
make tester
./test
if [ "$?" != "0" ]
then
exit 1
fi

cd ../side-config
make
make install
make tester
./test
if [ "$?" != "0" ]
then
exit 2
fi

cd ../side-log
make
make install
make tester
./test
if [ "$?" != "0" ]
then
exit 3
fi

cd ../side-plugin
make
make install
make tester
./test
if [ "$?" != "0" ]
then
exit 4
fi

echo "Testing done!"
