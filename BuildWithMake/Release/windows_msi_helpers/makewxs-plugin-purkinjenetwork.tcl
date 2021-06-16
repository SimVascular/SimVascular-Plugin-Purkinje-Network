# Copyright (c) Stanford University, The Regents of the University of
#               California, and others.
#
# All Rights Reserved.
#
# See Copyright-SimVascular.txt for additional details.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set SV_VERSION [lindex $argv 0]
set SV_PLATFORM [lindex $argv 1]
set SV_TIMESTAMP [lindex $argv 2]
set SV_PLUGIN_TIMESTAMP [lindex $argv 3]
set DIST_DIR_PLUGIN_TOP [lindex $argv 4]
set SV_MAJOR_VER_NO [lindex $argv 5]
set SV_PLUGIN_MAJOR_VER_NO [lindex $argv 6]
set SV_PLUGIN_MINOR_VER_NO [lindex $argv 7]
set SV_PLUGIN_PATCH_VER_NO [lindex $argv 8]

global pwd
if {$tcl_platform(platform) == "windows"} {
  set pwd [pwd]
} else {
  #assume cygwin
  set pwd [exec cygpath -m [pwd]]
}

puts "building wxs for $argv"

set outputRegistry 0

proc file_find {dir wildcard args} {

  global pwd
  global SV_TIMESTAMP
  global SV_PLUGIN_TIMESTAMP
  
  if {[llength $args] == 0} {
     set rtnme {}
  } else {
     upvar rtnme rtnme
  }

  foreach j $dir {
    set files [glob -nocomplain [file join $j $wildcard]]
    # print out headers
    global idno
    global outfp
    set id [incr idno]
    global component_ids
    lappend component_ids $id
    set guid [exec tmp/uuidgen.exe 1]
    puts $outfp "<Component Id='id[format %04i $id]' Guid='$guid' Win64='yes'>"
    global outputRegistry
      
    if {!$outputRegistry} {
	  
	set outputRegistry 1
	  
        puts $outfp "<RegistryKey Root='HKLM' Key='Software\\WOW6432Node\\SimVascular\\Plugins\\$SV_TIMESTAMP\\Purkinjenetwork\\$SV_PLUGIN_TIMESTAMP'>"
        puts $outfp "  <RegistryValue Type=\"string\" Name='InstallDir' Value='\[INSTALLDIR\]' />"
	puts $outfp "  <RegistryValue Type=\"string\" Name='SimVascularTimeStamp' Value='$SV_TIMESTAMP'  />"
        puts $outfp "  <RegistryValue Type=\"string\" Name='PluginTimeStamp' Value='$SV_PLUGIN_TIMESTAMP'  />"
	puts $outfp "</RegistryKey>"
	
	puts $outfp "<RegistryKey Root='HKLM' Key='Software\\WOW6432Node\\SimVascular\\Plugins\\$SV_TIMESTAMP\\Purkinjenetwork\\$SV_PLUGIN_TIMESTAMP\\ENVIRONMENT_VARIABLES'>"
        puts $outfp "  <RegistryValue Type=\"string\" Name='PATH' Value='\[INSTALLDIR\]'  />"
        puts $outfp "  <RegistryValue Type=\"string\" Name='SV_PLUGIN_PATH' Value='\[INSTALLDIR\]' />"
        puts $outfp "  <RegistryValue Type=\"string\" Name='SV_CUSTOM_PLUGINS' Value='org_sv_gui_qt_purkinjenetwork' />"	
	puts $outfp "</RegistryKey>"

    }
    foreach i $files {
      global outfp
      if {![file isdirectory $i]} {
        global idno
	set id [incr idno]
	puts $outfp "<File Id='id[format %04i $id]' Name='[file tail $i]' Source='$pwd/$i' DiskId='1' />"
      }
      lappend rtnme $i
    }
    set id [incr idno]
    puts $outfp "<RemoveFile Id='id[format %04i $id]' On='uninstall' Name='*.*' />"
    set id [incr idno]
    puts $outfp "<RemoveFolder Id='id[format %04i $id]' On='uninstall' />"
    puts $outfp "</Component>"

    set files [glob -nocomplain [file join $j *]]
    foreach i $files {
      if {[file isdirectory $i] == 1} {
	if {[file tail $i] != ".svn"} {
          global outfp
          global idno
	  set id [incr idno]
          global curdirID
          set curdirID "id[format %04i $id]"
	  puts $outfp "<Directory Id='id[format %04i $id]' Name='[file tail $i]'>"

          file_find $i $wildcard 1
          puts $outfp "</Directory>"
	}
      }
    }
  }
  return [lsort -unique $rtnme]

}

puts "building wxs for $argv"

set component_ids {}

set idno 1000

set outfp [open tmp/simvascular-plugin-purkinjenetwork.wxs w]

puts $outfp "<?xml version='1.0' encoding='windows-1252'?>"
puts $outfp "<Wix xmlns=\"http://schemas.microsoft.com/wix/2006/wi\""
puts $outfp "     xmlns:util=\"http://schemas.microsoft.com/wix/UtilExtension\">"

set uniqueId [exec tmp/uuidgen.exe 1]
set uniqueUpgradeCode [exec tmp/uuidgen.exe 1]
set uniquePackageId [exec tmp/uuidgen.exe 1]

puts $outfp "<Product Name='SimVascular ($SV_TIMESTAMP) Purkinjenetwork Plugin' Id='$uniqueId' UpgradeCode='$uniqueUpgradeCode' Language='1033' Codepage='1252' Version='$SV_PLUGIN_MAJOR_VER_NO.$SV_PLUGIN_MINOR_VER_NO.$SV_PLUGIN_PATCH_VER_NO' Manufacturer='SimVascular'>"

puts $outfp "<Package Id='$uniquePackageId' Keywords='Installer' Description='SimVascular Licensed Modules Installer' Comments='SimVascular Purkinjenetwork Plugin' Manufacturer='SimVascular' InstallerVersion='200' Languages='1033' Compressed='yes' Platform='x64' SummaryCodepage='1252'/>"

puts $outfp "<WixVariable Id=\"WixUILicenseRtf\" Value=\"License.rtf\" />"
puts $outfp "<WixVariable Id=\"WixUIBannerBmp\" Value=\"windows_msi_helpers/msi-banner.bmp\" />"
puts $outfp "<WixVariable Id=\"WixUIDialogBmp\" Value=\"windows_msi_helpers/msi-dialog.bmp\" />"

puts $outfp "<Media Id='1' Cabinet='Sample.cab' EmbedCab='yes' />"
puts $outfp "<Property Id='INSTALLLEVEL' Value='999' />"
puts $outfp "<Property Id='ALLUSERS' Value='1' />"

puts $outfp "<Directory Id='TARGETDIR' Name='SourceDir'>"
puts $outfp "\t<Directory Id='ProgramFiles64Folder' Name='PFiles'>"
puts $outfp "\t\t<Directory Id='id19' Name='SimVascular'>"
puts $outfp "\t\t<Directory Id='id911' Name='Plugins'>"
puts $outfp "\t\t<Directory Id='id912' Name='$SV_TIMESTAMP'>"
puts $outfp "\t\t<Directory Id='id913' Name='Purkinjenetwork'>"
puts $outfp "\t\t\t<Directory Id='INSTALLDIR' Name='$SV_PLUGIN_TIMESTAMP'>"

file_find $DIST_DIR_PLUGIN_TOP/ *

puts $outfp "\t\t\t</Directory>"
puts $outfp "\t\t\t</Directory>"
puts $outfp "\t\t</Directory>"
puts $outfp "\t\t</Directory>"
puts $outfp "\t\t</Directory>"
puts $outfp "\t</Directory>"
puts $outfp "</Directory>"

puts $outfp "<Feature Id='Complete' Title='$SV_VERSION Modules' Description='The complete package.' Display='expand' Level='1' ConfigurableDirectory='INSTALLDIR'>"
puts $outfp "\t<Feature Id='Main' Title='Main' Description='Required Files' Display='expand' Level='1'>"

# need components to match directories above!
foreach i $component_ids {
  puts $outfp "\t\t<ComponentRef Id='id$i' />"
}

puts $outfp "\t</Feature>"
puts $outfp "</Feature>"

puts $outfp "<Property Id='WIXUI_INSTALLDIR' Value='INSTALLDIR' />		<UIRef Id='WixUI_InstallDir' />"
puts $outfp "<Icon Id='idico' SourceFile='$pwd\\windows_msi_helpers\\simvascular.ico' />"
puts $outfp "</Product>"
puts $outfp "</Wix>"

close $outfp
