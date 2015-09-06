#JetSpace Desktop (SIDE)

SImple Desktop Enviroment (SIDE)

[![Documentation Status](https://readthedocs.org/projects/side/badge/?version=latest)](https://readthedocs.org/projects/side/?badge=latest)

it is not useable yet... (of course you can try it :D )

##to test:


```bash
$ sudo bash build.sh -i # You will need root rights
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

 Documentation is generated using mkdocs! you can find the markdown files in docs/
 The documentation is hosted at [rtfd.org](http://side.rtfd.org/)
