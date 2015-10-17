#!&usr/bin/env sh
#
# Copyright (C) 2015 Marius Messerschmidt
# This file is used for travis-ci automated testing
#

./install_libs.sh

make
if [ "$?" != "0" ]
then
	return 1;
fi

make install
if [ "$?" != "0" ]
then
	return 1;
fi

make tester -C lib/side-app/

./lib/side-app/test
if [ "$?" != "0" ]
then
	return 1;
fi

make tester -C lib/side-config/

./lib/side-config/test
if [ "$?" != "0" ]
then
	return 1;
fi

make tester -C lib/side-log/

./lib/side-log/test
if [ "$?" != "0" ]
then
	return 1;
fi

make tester -C lib/side-plugin/

./lib/side-plugin/test
if [ "$?" != "0" ]
then
	return 1;
fi
