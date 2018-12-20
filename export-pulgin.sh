# This is a script that demonstrates how to set the SimVascular environment varables
# use to load a custom plugin.
#
#unset SV_CUSTOM_PLUGINS
#unset SV_PLUGIN_PATH
export SV_CUSTOM_PLUGINS=$SV_CUSTOM_PLUGINS:org_sv_gui_qt_purkinjenetwork 
export SV_PLUGIN_PATH=$SV_PLUGIN_PATH:$PWD/build/lib/plugins/


