cat shared/info.h | grep VERSION | sed 's/#define VERSION//g' | sed 's/\"//g'
