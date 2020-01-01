#!/bin/bash
####################################################################
# Title    :  new_BUILD.sh                                         #
# Author   :  ?Pascal Brachet? modified by Denis Meiswinkel        #
# Date     :  02-11-2015                                           #
# Category :  Texmaker build script                                #
# Status   :  Beta (WiP)                                           #
# Version  :  4.5                                                  #
####################################################################


# colors
txtred="\e[0;31m" # red
txtgrn="\e[0;32m" # green
txtrst="\e[0m"    # text reset (black)

# variables
PLATFORM="unknown"
UNAME=$(uname) # runs uname

#-----------------------------------------------------------------------------------------------------------------------------#

function main {
	echo "Texmaker compilation:"
	echo "----------------------------------------"
	while [[ -z ${QTDIR} ]]; do # loop until input is provided
		read -p "Enter full path to QT (qmake): " QTDIR # get path to qmake
			if [[ ! -f ${QTDIR}/qmake ]]; then # check if gmake is present in the provided path
				echo -ne "${txtred}Error${txtrst}: Unable to find qmake in \"${QTDIR}\"\n"
				QTDIR=""
			fi
	done
	echo -ne "\nTrying to detect OS ..."
	if [[ "${UNAME}" == "Linux" ]]; then # try to autodetect OS using uname
		SYSTEM="1"
		PLATFORM="Linux"
		echo -ne "\r                       " # clear output
		echo -ne "\rDetected OS: ${PLATFORM}\n"
	elif [[ "${UNAME}" == "darwin" ]]; then
		SYSTEM="2"
		PLATFORM="MACOSX"
		echo -ne "\r                       " # clear output
		echo -ne "\rDetected OS: ${PLATFORM}\n"
	else
		echo -ne "\rUnable to detect system automatically!\n"
		while [[ -z ${SYSTEM} ]]; do
			read -p "Enter System type (1: UNIX ; 2: MACOSX): " SYSTEM
		done
	fi
	echo -ne "\n" # just print a space to make a nicer output
	PATH=${QTDIR}/bin:${PATH} # set system variables ...
	LD_LIBRARY_PATH=${QTDIR}/lib:${LD_LIBRARY_PATH}
	DYLD_LIBRARY_PATH=${QTDIR}/lib:${DYLD_LIBRARY_PATH}
	export QTDIR PATH LD_LIBRARY_PATH DYLD_LIBRARY_PATH # ... and export them

	if [[ ${SYSTEM} == "1" ]]; then # run if linux is detected
		while [[ -z ${PREFIX} ]]; do # get installation path for Texmaker
			read -p "Enter installation path (example: /usr , /usr/local or /opt): " PREFIX
		done
		while [[ -z ${DESKTOPDIR} ]]; do # get installation path for .desktop file
			read -p "Enter directory for the desktop file (example: /usr/share/applications): " DESKTOPDIR
		done
		while [[ -z ${ICONDIR} ]]; do # get installation path for Desktop-icon
			read -p "Enter directory for the icon file (example: /usr/share/pixmaps): " ICONDIR
		done
		${QTDIR}/qmake PREFIX=${PREFIX} DESKTOPDIR=${DESKTOPDIR} ICONDIR=${ICONDIR} texmaker.pro # run qmake
		QMAKE_EXIT="$?" # get exit code of qmake
		if [[ $? == ${QMAKE_EXIT} ]]; then # check if qmake was successful
			echo -ne "\nqmake [${txtgrn}Successful${txtrst}]\n"
		else
			echo -ne "\nqmake [${txtred}Failed${txtrst}]\n"
			echo -ne "Please check qmake output for errors.\n"
			exit 1
		fi
		echo -ne "\nStarting compilation:\n"
		make # run make
		MAKE_EXIT="$?" # get exit code of make
		wait
		if [[ $? == ${MAKE_EXIT} ]]; then # check if make was successful
			echo -ne "\nCompiling [${txtgrn}Successful${txtrst}]\n"
		else
			echo -ne "\nCompiling [${txtred}Failed${txtrst}]\n"
			echo -ne "Please check make output for errors.\n"
			exit 1
		fi
		read -p "Do you want to install [y/n]? : " INSTALL
		if [[ ${INSTALL} == "y" ]]; then
			make install # run make install
			MAKE_INSTALL_EXIT="$?" # get exit code of make install
			wait
			if [[ $? == ${MAKE_INSTALL_EXIT} ]]; then # check if make install was successful
				echo -ne "\nInstallation [${txtgrn}Successful${txtrst}]\n"
			else
				echo -ne "\nInstallation [${txtred}Failed${txtrst}]\n"
				echo -ne "Please check make output for errors.\n"
				exit 1
			fi
			echo "Compilation and installation is done"
			exit 0
		elif [[ ${INSTALL} == "n" ]]; then
			echo "Compilation finished"
			exit 0
		fi
	fi

	if [[ ${SYSTEM} == "2" ]]; then # if MacOS is detected
		${QTDIR}/bin/qmake -macx -spec macx-clang texmaker.pro
		make
		make install
		echo "compilation and installation finished"
		exit 0
	fi

	echo "If you can read this something went wrong ..."
	exit 1
}

function HELP {
	cat <<-"EOF"
	Texmaker compilation script version 5.0

	usage:
	./BUILD.sh [-h]

	optional arguments:
	-h, –help show this help message and exit
	EOF
	exit 0
}

function welcome {
	clear
	cat <<-"EOF" # made using http://www.kammerl.de/ascii/AsciiSignature.php (Font: "big")
	***************************************************************************
	*                _______                        _                         *
	*               |__   __|                      | |                        *
	*                  | | _____  ___ __ ___   __ _| | _____ _ __             *
	*                  | |/ _ \ \/ / "_ ` _ \ / _` | |/ / _ \ "__|            *
	*                  | |  __/>  <| | | | | | (_| |   <  __/ |               *
	*                  |_|\___/_/\_\_| |_| |_|\__,_|_|\_\___|_|               *
	*                                                                         *
	*                                                                         *
	*       http://www.xm1math.net/texmaker/                                  *
	*                                                                         *
	***************************************************************************
	EOF
	main ${@}
}

#-----------------------------------------------------------------------------------------------------------------------------#

if [[ ${1} == "-h" || ${1} == "-help" ]]; then
	HELP
else
	welcome ${@}
fi
	
