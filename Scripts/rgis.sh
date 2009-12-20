#!/bin/bash
#@(#)GHAAS RiverGIS V2.1 Start Script (02-Jan-24)

if [ -z "${GHAAS_DIR}" ]; then
	if [ -e "/usr/local/share/ghaas" ]; then
		export GHAAS_DIR="/usr/local/share/ghaas"
	else
		if [ -e "/usr/local/app/ghaas" ]; then
			export GHAAS_DIR="/usr/local/app/ghaas"
		else
			if [ -e "/net/app/ghaas" ]; then
				export GHAAS_DIR="/net/app/ghaas"
			fi
		fi
	fi
	if [ -z "${GHAAS_DIR}" ]; then
		echo GHAAS_DIR is not defined.
		exit
	fi
fi

if [ ${XUSERFILESEARCHPATH:-unset} != unset ]
then
   unset XUSERFILESEARCHPATH
fi

export XAPPLRESDIR="$GHAAS_DIR/XResources"

GHAAS_EXEC=${GHAAS_DIR}/bin/rgis21

if [ -f ${GHAAS_DIR}/Messages/RGIS.msg ]; then
	cat ${GHAAS_DIR}/Messages/RGIS.msg
fi

exec $GHAAS_EXEC $* &
