#!/bin/bash
#
#******************************************************************************
# ld_config.sh (xpccut)
#------------------------------------------------------------------------------
##
# \file       	ld_config.sh
# \library    	libxpccut
# \author     	Chris Ahlstrom
# \date        2009-04-11
# \updates     2015-10-05
# \version    	$Revision$ (1.1)
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#     This script makes sure that either /etc/ld.so.conf.d/xpc.conf
#     or /etc/ld.so.conf is updated to include the path to the installed
#     XPCCUT library, and then it runs 'ldconfig' to bless this new item.
#
#------------------------------------------------------------------------------

#******************************************************************************
# Checks to see if the user is root.  Not the best check, but ....
#------------------------------------------------------------------------------

function check_if_root()
{
   if [ $UID != 0 ] ; then

      echo "You must login as root (or use sudo) to perform this operation."
      return 1

   fi
   return 0
}

#******************************************************************************
# Appends a line of text to a file, if the text is not already present.
#------------------------------------------------------------------------------

function append_a_line()
{
   FILE="$1"
   LINE="$2"
   grep $LINE $FILE >> /dev/null
   if [ $? == 1 ] ; then
      echo $LINE >> $FILE
   else
      echo "'$LINE' already present in '$FILE'"
   fi
}

#******************************************************************************
# Find the current directory and update the appropriate ldconfig file or
# directory.
#------------------------------------------------------------------------------

function upgrade_ld_so_conf()
{
   ETC_LD_FILE="/etc/ld.so.conf"
   ETC_LD_DIR="/etc/ld.so.conf.d"
   ETC_LD_XPC_FILE="/etc/ld.so.conf.d/xpc.conf"
   LINE_TO_ADD="$1"
   echo "Updating library path configuration !!!"

# Check to see if the distro supports an ld directory.  If so, we will want
# to add our own configuration file to this directory (if necessary) and 
# make sure the configuration file contains the line for the current
# installation directory of the library.  Otherwise, we will use the older
# ld configuration file.

   if [ -d $ETC_LD_DIR ] ; then

      echo "Found the standard '$ETC_LD_DIR' directory, will use that."

      if [ ! -f $ETC_LD_XPC_FILE ] ; then

         echo "We need to create the '$ETC_LD_XPC_FILE' configuration file"
         touch $ETC_LD_XPC_FILE

      fi

   else

      echo "No '$ETC_LD_DIR' directory, will check for the ld config file..."
      ETC_LD_XPC_FILE=$ETC_LD_FILE

   fi


   if [ -f $ETC_LD_XPC_FILE ] ; then

      append_a_line $ETC_LD_XPC_FILE $LINE_TO_ADD

   else

      echo "? Could not find '$ETC_LD_XPC_FILE', aborting...!"
      return 1

   fi

}

#******************************************************************************
# "main()"
#------------------------------------------------------------------------------

check_if_root

if [ $? == 0 ] ; then

   upgrade_ld_so_conf $1
   /sbin/ldconfig

fi

#******************************************************************************
# scripts/ld_config.sh (xpccut)
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 et ft=sh
#------------------------------------------------------------------------------
