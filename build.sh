make build

#Install
if [ "$1" == "-i" ]
then
sudo make install
fi
if [ "$1" == "-if" ]
then
sudo make install
make installuserconf
fi
