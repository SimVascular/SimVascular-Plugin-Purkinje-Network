# SimCardio
This is the repository for the SimCardio project. The project currently contains only the Purkinje Plugin.

# Purkinje Plugin
The Purkinje Plugin is used to create a Purkinje network on a surface model of the heart.

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

## Installing the Purkinje Plugin
The installer installs the Purkinje Plugin in
```
Unix and MacOs: /usr/local/sv/svplugins/SVDATE/Purkinje-Plugin/DATE
```
This directory contains the plugin shared libraries, a setup.sh script and Python scripts. The setup.sh script sets the **SV_CUSTOM_PLUGINS** and **SV_PLUGIN_PATH** environment variables. The **SV_CUSTOM_PLUGINS** environment variable defines the name of the pluging (org_sv_gui_qt_purkinjenetwork) and the **SV_PLUGIN_PATH** environment variable defines the locatation of the plugin shared library (e.g. liborg_sv_gui_qt_purkinjenetwork.so).

Note that **SVDATE** must match the date of the installed SimVascular application.

## Using the Purkinje Plugin 
The SimVascular application starts by executing a launch script that looks for plugins in the plugin install directory. When a plugin is found its setup.sh script is executed. The SimVascular application is then executed and loads in core and then custom plugins. Plugins are loaded using the plugin shared library (e.g. liborg_sv_gui_qt_purkinjenetwork.so).

If the plugin is successfully loaded you will see the Purkinje Plugin icon ![](images/purkinje_network.png) on the SimVascular toolbar.


