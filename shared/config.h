/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

//Contains all build switches (defines)


//Choose your DisplayServer
//X_WINDOW_SYSTEM: X11/Xorg, use the X protocol
  #define X_WINDOW_SYSTEM
//WAYLAND: Use the Wayland protocol (unstable), for example with WESTON
  //#define WAYLAND
//MIR: Canonicals X alternative, we will only support when it will become popular...
  //#define MIR

//Unset to prevent python from being included in the build
#define LOAD_PYTHON_MODULES
