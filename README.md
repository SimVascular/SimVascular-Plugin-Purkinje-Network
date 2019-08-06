# SimCardio
This is the repository for the SimCardio project. The project currently contains only the Purkinje Plugin.

# Purkinje Plugin
The Purkinje Plugin is used to create a Purkinje network on a surface model of the heart.

**Note: The Purkinje Plugin code is under active developement and is not stable. There is limited error checking in the Python script and the GUI is still in developement. **

## Building the Purkinje Plugin Shared Libraries
The shared libraries defining a SimVascular custom plugin are built from the Purkinje Plugin project source using CMake. The Purkinje Plugin CMake code uses CMake macros from the SimVascular project and the CMake.config file from a local SimVascular build so a SimVascular local build must be present. The Purkinje Plugin is built from source using the following steps

```
git clone https://github.com/SimVascular/SimCardio.git
cd SimCardio
mkdir build
cd build
ccmake -DACCEPT_DOWNLOAD_EXTERNALS=ON ..
make 
```

Next set the environment variables that SimVascular uses to identify and load custom plugins

```
cd $PROJECT_DIR/SimCardio 
export SV_CUSTOM_PLUGINS=$SV_CUSTOM_PLUGINS:org_sv_gui_qt_purkinjenetwork
export SV_PLUGIN_PATH=$SV_PLUGIN_PATH:$PWD/build/lib/plugins/
export LD_LIBRARY_PATH=$PWD/build/lib/
```
To use the Purkinje Plugin with the local SimVascular development build the **sv** launch script in SimVascular/build/SimVascular-build must be modified to use the Purkinje Plugin developement shared libraries and Python scripts rather than those that might be installed.

In the $PROJECT_DIR/SimVascular/build/SimVascular-build/sv script

  1) Comment out: 

        'export SV_PLUGIN_INSTALL_DIR=' 

   2) Add: 

        export PYTHONPATH=$PYTHONPATH:$PROJECT_DIR/SimCardio/Modules/PurkinjeNetwork/python/fractal-tree/:

      after: 

       export PYTHONPATH=$PYTHONPATH:$SV_PLUGIN_PATH


## Building the Purkinje Plugin Installer
The Purkinje Plugin installer is created using the following steps
```
cd build
cpack
```

## Installing the Purkinje Plugin
The installer installs the Purkinje Plugin in
```
Unix and MacOs: /usr/local/sv/svplugins/SVDATE/Purkinje-Plugin/PLUGINDATE
```
This directory contains the plugin shared libraries, a setup.sh script and Python scripts. The setup.sh script sets the **SV_CUSTOM_PLUGINS** and **SV_PLUGIN_PATH** environment variables. The **SV_CUSTOM_PLUGINS** environment variable defines the name of the pluging (org_sv_gui_qt_purkinjenetwork) and the **SV_PLUGIN_PATH** environment variable defines the locatation of the plugin shared library (e.g. liborg_sv_gui_qt_purkinjenetwork.so).

Note that **SVDATE** must match the date of the installed SimVascular application. To work around this restriction you can modify **SVDATE** to match the installed SimVascular application. This should work with the latest installed SimVascular application that uses the latest externals.

## Using the Purkinje Plugin 
The SimVascular application starts by executing a launch script that looks for plugins in the plugin install directory. When a plugin is found its setup.sh script is executed. The SimVascular application is then executed and loads in core and then custom plugins. Plugins are loaded using the plugin shared library (e.g. liborg_sv_gui_qt_purkinjenetwork.so).

If the plugin is successfully loaded you will see the Purkinje Plugin icon <img src="images/purkinje_network.png" alt="alt text" width="20" height="20"> on the SimVascular toolbar.

Example projects are found in the **example-projects** directory under the SimCardio project.

# Purkinje Plugin ideal heart project
The Purkinje Plugin ideal heart project generates a Purkinje network on an idealized geometric model of the heart. The project is loaded from the **example-projects/purkinje-network-ideal-heart** directory under the SimCardio project.

<img src="images/ideal-heart-1.png" alt="alt text"> 



<h3>HTML</h3>
<p>

<div style="background-color: #F0F0F0; padding: 10px; border: 1px solid #e6e600; border-left: 6px solid #e6e600">
</p>

When starting SimVascular a Purkinje Plugin tool panel may be present before a project is opened. Delete this panel before opening a project.
</div>


