#JetSpace Desktop (SIDE)

SImple Desktop Enviroment (SIDE)

[![Documentation Status](https://readthedocs.org/projects/side/badge/?version=latest)](https://readthedocs.org/projects/side/?badge=latest)
[![Build Status](https://travis-ci.org/jetspace/desktop.svg)](https://travis-ci.org/jetspace/desktop)

##to build:

If you do not have the JetSpaceLibs installed do:

`./install_libs.sh`

Then continue to the 'real' build:

```bash
$ sudo bash build.sh -if # You will need root rights (f overrides your old settings, so only use it if you installing a new install, else use -i)
$ cd ~ # OR cd $HOME
$ cp .xinitrc xinitrc_backup # Save old xinit config
$ echo "side-session" > .xinitrc
$ startx
```
later you can just use
```bash
startx
```

##Preview:

![Version 0.36](http://s7.postimg.org/x472b6pmz/side_0_36.png  "Screenshot Version 0.36 with XFWM4")
running with xfwm4

## Documentation

 Documentation is generated using sphinx! you can find the documentation files in docs/
 The documentation is hosted at [rtfd.org](http://side.rtfd.org/)
