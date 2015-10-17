mkdir temp
cd temp
git clone http://github.com/jetspace/jetlibs
cd jetlibs
make
make install
cd ..
cd ..
rm -rf temp
