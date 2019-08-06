# SimCardio
This is the repository for the SimCardio project. The project currently contains only the Purkinje Plugin.

## Building the Purkinje Plugin Shared Libraries
The shared libraries defining a SimVascular custom plugin are built from the Purkinje Plugin project source using CMake. The Purkinje Plugin CMake code uses CMake macros from the SimVascular project and the CMake.config file from a SimVascular build so a SimVascular build must be present. The Purkinje Plugin is built from source using the following steps

```
git clone https://github.com/SimVascular/SimCardio.git
cd SimCardio
mkdir build
cd build
ccmake -DACCEPT_DOWNLOAD_EXTERNALS=ON ..
make 
```

Next set two environment variables that SimVascular uses to identify and load custom plugins

```
cd SimCardio 

export SV_CUSTOM_PLUGINS=$SV_CUSTOM_PLUGINS:org_sv_gui_qt_purkinjenetwork

export SV_PLUGIN_PATH=$SV_PLUGIN_PATH:$PWD/build/lib/plugins/
```

SimVascular is then run by executing the launch script from the commnd line
```
Linux:  /usr/local/sv/simvascular/DATE/simvascular
MacOS:  /Applications/SimVascular.app/Contents/MacOS/Simvascular
```
## Building the Purkinje Plugin Installer
The Purkinje Plugin installer is created using the following steps
```
cd build
cpack
```
