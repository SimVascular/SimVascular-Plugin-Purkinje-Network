
# This shell script sets the environment variables used to add the Purkinje plugin for a
# development version of SV.
# 
export SV_CUSTOM_PLUGINS=org_sv_gui_qt_purkinjenetwork
export SV_PLUGIN_PATH=$PWD/build/lib/plugins
export LD_LIBRARY_PATH=$PWD/build/lib/

# Note that the 'sv' launch script in SimVascular/build/SimVascular-build must be modified
# to use the developement shared libraries and Python scripts rather than those that might be
# installed.
#
# In the $PROJECT_DIR/SimVascular/build/SimVascular-build/sv script
#
#   1) Comment out: 
#
#        'export SV_PLUGIN_INSTALL_DIR=' 
#
#   2) Add: 
#
#        export PYTHONPATH=$PYTHONPATH:$PROJECT_DIR/SimCardio/Code/Source/sv4gui/Modules/PurkinjeNetwork/python/fractal-tree/:
#
#      after: 
#
#        export PYTHONPATH=$PYTHONPATH:$SV_PLUGIN_PATH



