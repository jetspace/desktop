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
cd ../side-config
make
make install
make tester
./test
cd ../side-log
make
make install
make tester
./test
cd ../side-plugin
make
make install
make tester
./test

echo "Testing done!"
