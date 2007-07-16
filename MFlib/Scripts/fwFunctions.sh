#!/bin/bash

_fwMAXPROC=4
_fwPASSNUM=5

function _fwDataSource()
{
	for (( fwI = 0; fwI < ${#_fwDSourceARRAY[@]} ; ++fwI ))
	do
		local fwTEMP=(${_fwDSourceARRAY[${fwI}]})
		[[ "${fwTEMP[0]}${fwTEMP[1]}" == "${1}${2}" ]] && { echo "${_fwDSourceARRAY[${fwI}]}"; return 0; }
	done
	echo ""
	return -1
}

function _fwVariable()
{
	local fwRETURN=""
	for (( fwI = 0; fwI < ${#_fwVariableARRAY[@]} ; ++fwI ))
	do
		local fwTEMP=(${_fwVariableARRAY[${fwI}]})
		[ "${fwTEMP[0]}" == "${1}" ] && fwRETURN=${fwTEMP[1]}
	done
	echo "${fwRETURN}"
}

function _fwOutput()
{
	local fwRETURN=""
	for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
	do
		local fwTEMP=(${_fwOutputARRAY[${fwI}]})
		[ "${fwTEMP}" == "${1}" ] && fwRETURN=${fwTEMP}
	done
	echo "${fwRETURN}"
}

function _fwState()
{
	local fwRETURN=""
	for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
	do
		local fwTEMP=(${_fwStateARRAY[${fwI}]})
		[ "${fwTEMP[0]}" == "${1}" ] && fwRETURN=${fwTEMP[0]}
	done
	echo "${fwRETURN}"
}

function FwArguments()
{
	     _fwSPINUP="on"
	   _fwFINALRUN="on"
	_fwPOSTPROCESS="on"
	   _fwWARNINGS="on"
	   _fwTESTONLY="off"
	    _fwVERBOSE="off"
	while [ "${1}" != "" ]
	do
		case ${1} in
			(-s|--spinup)
				shift
				case ${1} in
					(on)
						_fwSPINUP="on"
					;;
					(off)
						_fwSPINUP="off"
					;;
				esac
			;;
			(-f|--finalrun)
				shift
				case ${1} in
					(on)
						_fwFINALRUN="on"
					;;
					(off)
						_fwFINALRUN="off"
					;;
				esac
			;;
			(-p|--postrocess)
				shift
				case ${1} in
					(on)
						_fwPOSTPROCESS="on"
					;;
					(off)
						_fwPOSTPROCESS="off"
					;;
				esac
			;;
			(-W|--warnings)
				shift
				case ${1} in
					(on)
						_fwWARNINGS="on"
					;;
					(off)
						_fwWARNINGS="off"
					;;
				esac
			;;
			(-T|--testonly)
				_fwTESTONLY="on"
			;;
			(-V|--verbose)
				_fwVERBOSE="on"
			;;
		esac
		shift
	done
}

function FwInit()
{
	      _fwModelBIN=${1}	
       _fwVersionSTR=${2}
     _fwDomainPREFIX=${3}
   _fwRGISDomainFILE=${4}
       _fwGDSWorkDIR=${5}
  	_fwRGISResultsDIR=${6}
	       _fwRGISBIN=${7}

	  _fwDomainNAME=$(${_fwRGISBIN}getHeader -d ${_fwRGISDomainFILE} | sed "s/Domain: //")
	  _fwDomainTYPE=$(echo ${_fwRGISDomainFILE} | sed "s:.*\.\(gdb.\):\1:")
	   _fwDomainRES=$(echo ${_fwRGISDomainFILE} | sed "s:.*_\(.*\)\.gdb.:\1:")
	[ "${_fwRGISDomainFILE}" == "${_fwDomainRES}" ] && _fwDomainRES="unset"
	case ${_fwDomainTYPE} in
		(gdbn)
			_fwOutputTYPE="gdbc"
			_fwDomainTYPE="Network"
		;;
		(gdbc|gdbd)
			_fwOutputTYPE="gdbc"
			_fwDomainTYPE="Grid"
		;;
		(*)
			 _fwDomainRES=""
			_fwOutputTYPE="gdbt"
			_fwDomainTYPE="Point"
		;;
	esac
	 _fwGDSDomainDIR="${_fwGDSWorkDIR}/${_fwDomainNAME}/${_fwDomainTYPE}_${_fwDomainRES}"
	_fwGDSDomainFILE="${_fwGDSDomainDIR}/${_fwDomainPREFIX}${_fwDomainTYPE}_${_fwDomainRES}.ds"
	    _fwGDSLogDIR="${_fwGDSDomainDIR}/${_fwVersionSTR}/logs"

}

function FwDataSrc()
{
	(( fwI = 0 ))
	while [ "${1}"	!= "" ]
	do
		local fwDSOURCE=(${1})
		_fwDSourceARRAY[${fwI}]="${fwDSOURCE[0]} ${fwDSOURCE[1]} ${fwDSOURCE[2]} ${fwDSOURCE[3]} ${fwDSOURCE[4]}"
		(( ++fwI ))
		shift
	done
}

function _fwOptionList()
{
	for (( fwI = 0; fwI < ${#_fwOptionARRAY[@]} ; ++fwI ))
	do
		echo "-p ${_fwOptionARRAY[${fwI}]}"
	done
}

function FwOptions()
{
	(( fwI = 0 ))
	unset _fwVariableARRAY
	unset _fwInputARRAY
	unset _fwStateARRAY
	while [ "${1}" != "" ]
	do
		local fwOPTION=(${1})
		_fwOptionARRAY[${fwI}]="${fwOPTION[0]}=${fwOPTION[1]}"
		(( ++fwI ))
		shift
	done
	(( fwVARnum   = 0 ))
	(( fwINPUTnum = 0 ))
	(( fwSTATEnum = 0 ))
	local fwMessageOPTIONS="-m sys_error=off -m app_error=off -m usr_error=off -m debug=off -m warning=off -m info=on"
	local fwLINES=($(${_fwModelBIN} ${_fwGDSDomain} $(_fwOptionList) -T ${fwMessageOPTIONS} | grep "XXXX"  | cut -c15-45,58-64,77-80,81-85,86-94))
	for (( fwVARnum = 0; fwVARnum < ${#fwLINES[@]} / 5 ; ++fwVARnum ))
	do
		_fwVariableITEM="${fwLINES[(( fwVARnum * 5    ))]}"
		local fwVARTYPE="${fwLINES[(( fwVARnum * 5 + 1))]}"
		local     fwSET="${fwLINES[(( fwVARnum * 5 + 2))]}"
		local    fwFLUX="${fwLINES[(( fwVARnum * 5 + 3))]}"
		local fwINITIAL="${fwLINES[(( fwVARnum * 5 + 4))]}"
		
		if [[ "${fwFLUX}" == "no" ]]
		then
			_fwVariableARRAY[${fwVARnum}]="${_fwVariableITEM} avg"
		else
			_fwVariableARRAY[${fwVARnum}]="${_fwVariableITEM} sum"
		fi
		if [[ "${fwSET}"  == "no" ]]
		then
			_fwInputARRAY[${fwINPUTum}]="${_fwVariableITEM}"
			(( ++fwINPUTum ))
		fi
		if [[ "${fwINITIAL}" == "yes" ]]
		then
			_fwStateARRAY[${fwSTATEnum}]="${_fwVariableITEM}"
			(( ++fwSTATEnum ))
		fi
	done
}

function FwOutputs()
{
	local fwI
	local fwJ
	(( fwI = 0 ))
	while [[ "${1}"	!= "" ]]
	do
		if [ "$(_fwVariable ${1})" == "" ]
		then
			[ "${_fwWARNINGS}" == "on" ] && echo "Skipping output variable: ${1}."
		else
			_fwOutputARRAY[${fwI}]="${1}"
			(( ++fwI ))
		fi
		shift
	done
}

function _fwPrintTest()
{
	echo "Variables"
	for (( fwI = 0; fwI < ${#_fwVariableARRAY[@]} ; ++fwI ))
	do
		echo "  ${_fwVariableARRAY[${fwI}]}"
	done
	echo "Inputs"
	for (( fwI = 0; fwI < ${#_fwInputARRAY[@]} ; ++fwI ))
	do
		local fwInputITEM=(${_fwInputARRAY[${fwI}]})
		local   fwSOURCE=($(_fwDataSource "${fwInputITEM[0]}" "static"))
		if   [[ "${fwSOURCE[@]}" == "" ]]
		then
			echo "  ${fwInputITEM[0]} is missing from data sources!"
		elif [[ "${fwSOURCE[3]}" == "const" ]]
		then
			echo "  ${fwInputITEM[0]} Constant [${fwSOURCE[4]}] input"		
		elif [[ "${fwSOURCE[3]}" == "pipe" ]]
		then
			echo "  ${fwInputITEM[0]} Piped input"		
		elif [[ "${fwSOURCE[3]}" == "file" ]]
		then
			if [ -e ${fwSOURCE[4]} ]
			then
				echo "  ${fwSOURCE[@]}"
			else
				echo "  ${fwInputITEM[0]} datafile [${fwSOURCE[4]}] is missing!"
			fi
		fi
	done
	echo "States"
	for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
	do
		echo "  ${_fwStateARRAY[${fwI}]}"
	done
	echo "Outputs"
	for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
	do
		echo "  ${_fwOutputARRAY[${fwI}]}"
	done
}

function _fwGDSFilename()
{
	local fwVARIABLE="${1}"
	local     fwMODE="${2}"
	local  fwVERSION="${3}"
	local     fwYEAR="${4}"
	local     fwSTEP="${5}"
	if [[ "${fwYEAR}" == "" ]]
	then
		local fwFILENAME="${_fwGDSDomainDIR}/${fwVERSION}/${_fwDomainPREFIX}_${fwMODE}_${fwVERSION}_${fwVARIABLE}_${fwSTEP}LTM_${_fwDomainRES}.gds"
	else
		local fwFILENAME="${_fwGDSDomainDIR}/${fwVERSION}/${_fwDomainPREFIX}_${fwMODE}_${fwVERSION}_${fwVARIABLE}_${fwSTEP}TS${fwYEAR}_${_fwDomainRES}.gds"
	fi
	echo ${fwFILENAME}
}

function _fwRGISFilename()
{
	local fwVARIABLE="${1}"
	local  fwVERSION="${2}"
	local     fwYEAR="${3}"
	local     fwSTEP="${4}"
	if [[ "${fwYEAR}" == "" ]]
	then
		local fwFILENAME="${_fwRGISResultsDIR}/${fwVARIABLE}/${_fwDomainPREFIX}_${fwVERSION}_${fwVARIABLE}_${fwSTEP}LTM_${_fwDomainRES}.gdbc"
	else
		local fwFILENAME="${_fwRGISResultsDIR}/${fwVARIABLE}/${_fwDomainPREFIX}_${fwVERSION}_${fwVARIABLE}_${fwSTEP}TS${fwYEAR}_${_fwDomainRES}.gdbc"
	fi
	echo "${fwFILENAME}"
}

function _fwPreprocess()
{
	local fwDOSTATE="${1}"
	local    fwYEAR="${2}"
	local fwPROC
	[ "${_fwVERBOSE}" == "on" ] && echo "      Preprocessing ${fwYEAR} started:  $(date)"
	(( fwPROC = 0 ))
	[ -e "${_fwGDSDomainDIR}"  ] || mkdir -p "${_fwGDSDomainDIR}"
	[ -e "${_fwGDSDomainFILE}" ] || ${RGISBINDIR}rgis2domain "${_fwRGISDomainFILE}"  "${_fwGDSDomainFILE}";

	if [[ "${fwDOSTATE}" == "dostate" ]]
	then
		for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		do
			local fwStateITEM=${_fwStateARRAY[${fwI}]}
			local    fwSOURCE=($(_fwDataSource "${fwInputITEM}" "static"))
			[ "${fwSOURCE[0]}" == "" ] && continue
			[ "${fwSOURCE[1]}" == "" ] && { echo "  ${fwStateITEM} data type is missing!";                 return -1; }
			[ "${fwSOURCE[2]}" == "" ] && { echo "  ${fwStateITEM} version is missing!";                   return -1; }
			[ "${fwSOURCE[3]}" == "" ] && { echo "  ${fwStateITEM} data source type is missing!";          return -1; }
			[ "${fwSOURCE[4]}" == "" ] && { echo "  ${fwStateITEM} data source specification is missing!"; return -1; }
			if [[ "${fwSOURCE[3]}" == "const" ]]
			then
				[ "${_fwVERBOSE}" == "on" ] && echo "         ${fwInputITEM} State Constant"
			elif [[ "${fwSOURCE[3]}" == "file" ]]
			then
				[ "${_fwVERBOSE}" == "on" ] && echo "         ${fwInputITEM} State File input"
				if [ -e "${fwSOURCE[4]}" ]
				then
					local fwFILENAME="$(_fwGDSFilename "${fwInputITEM}" "State" "${fwSOURCE[2]}" "${fwYEAR}" "d")"
					if [ ! -e "${fwFILENAME}" ]
					then
						[ -e "${_fwRGISDomainFILE}" ] || { echo "Missing domain file: ${_fwRGISDomainFILE}"; return -1; }
						[ "${fwPROC}" -ge "${_fwMAXPROC}" ] && { wait ; (( fwPROC = 0 )) ; }
						${_fwRGISBIN}rgis2ds -m "${_fwRGISDomainFILE}" "${fwSOURCE[4]}" "${fwFILENAME}" &
						(( ++fwPROC ))
					fi
				else
					echo "  ${fwInputITEM} datafile [${fwSOURCE[4]}] is missing!"
				fi
			fi
		done
	fi

	for (( fwI = 0; fwI < ${#_fwInputARRAY[@]} ; ++fwI ))
	do
		local fwInputITEM=${_fwInputARRAY[${fwI}]}
		if [ "${fwYEAR}" == "" ]
		then
			local    fwSOURCE=($(_fwDataSource "${fwInputITEM}" "static"))
		else
			local    fwSOURCE=($(_fwDataSource "${fwInputITEM}" "dynamic" | sed "s:xxxx:${fwYEAR}:"))
			if [ "${fwSOURCE[0]}" == "" ]
			then
				local fwSOURCE=($(_fwDataSource "${fwInputITEM}" "static"))
				fwInYEAR=""
			else
				fwInYEAR=${fwYEAR}
			fi
		fi
		[ "${fwSOURCE[0]}" == "" ] && { echo "  ${fwInputITEM} is missing from data sources!";         return -1; }
		[ "${fwSOURCE[1]}" == "" ] && { echo "  ${fwInputITEM} data type is missing!";                 return -1; }
		[ "${fwSOURCE[2]}" == "" ] && { echo "  ${fwInputITEM} version is missing!";                   return -1; }
		[ "${fwSOURCE[3]}" == "" ] && { echo "  ${fwInputITEM} data source type is missing!";          return -1; }
		[ "${fwSOURCE[4]}" == "" ] && { echo "  ${fwInputITEM} data source specification is missing!"; return -1; }
		if [ "${fwSOURCE[3]}" == "const" ]
		then
			[ "${_fwVERBOSE}" == "on" ] && echo "         ${fwInputITEM} Constant input"
		elif [ "${fwSOURCE[3]}" == "file" ]
		then
			[ "${_fwVERBOSE}" == "on" ] && echo "         ${fwInputITEM} File input"
			[ -e "${fwSOURCE[4]}" ] || { echo "  ${fwInputITEM} datafile [${fwSOURCE[4]}] is missing!"; return -1; }
			[ -e "${_fwGDSDomainDIR}/${fwSOURCE[2]}" ] || mkdir -p "${_fwGDSDomainDIR}/${fwSOURCE[2]}"
			local fwFILENAME="$(_fwGDSFilename "${fwInputITEM}" "Input" "${fwSOURCE[2]}" "${fwInYEAR}" "d")"
			[ -e "${fwFILENAME}" ] && continue
			[ -e "${_fwRGISDomainFILE}" ] || { echo "Missing domain file: ${_fwRGISDomainFILE}"; return -1; }
			[ "${fwPROC}" -ge "${_fwMAXPROC}" ] && { wait; (( fwPROC = 0 )) ; }
			${_fwRGISBIN}rgis2ds -m "${_fwRGISDomainFILE}" "${fwSOURCE[4]}" "${fwFILENAME}" &
			(( ++fwPROC ))
		fi
	done
	wait
	[ "${_fwVERBOSE}" == "on" ] && echo "      Preprocessing ${fwYEAR} finished: $(date)"
	return 0
}

function _fwPostprocess()
{
	local fwYEAR="${1}"
	local fwPROC
	if [ "${fwYEAR}" == "" ]; then local fwSUFFIX="LTM"; else local fwSUFFIX="TS${fwYEAR}"; fi
	[ "${_fwVERBOSE}" == "on" ] && { echo "      Postprocessing ${fwYEAR} started:  $(date)"; }
	(( fwPROC = 0 ))
	for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
	do
		local fwVARIABLE="${_fwOutputARRAY[${fwI}]}"
		local    fwAMODE="$(_fwVariable "${fwVARIABLE}")"
		[ "${fwAMODE}" == "" ] && { echo "Skipping undefinded variable [${fwVARIABLE}]"; continue; }
		local fwGDSFileNAME="$(_fwGDSFilename "${fwVARIABLE}" "Output" "${_fwVersionSTR}" "${fwYEAR}" "d")"
		[ -e "${fwGDSFileNAME}" ] || local fwGDSFileNAME="$(_fwGDSFilename "${fwVARIABLE}" "State" "${_fwVersionSTR}" "${fwYEAR}" "d")"
		[ -e "${fwGDSFileNAME}" ] || { echo "Skipping missing variable [${fwVARIABLE}]"; echo ${fwGDSFileNAME}; continue; }
		[ -e "${_fwRGISResultsDIR}/${fwVARIABLE}" ] || mkdir -p "${_fwRGISResultsDIR}/${fwVARIABLE}"
		local fwRGISFileNAME="$(_fwRGISFilename  "${fwVARIABLE}" "${_fwVersionSTR}" "${fwYEAR}" "d")"
		[ "${fwPROC}" -ge "${_fwMAXPROC}" ] && { wait; (( fwPROC = 0 )); }
		${_fwRGISBIN}ds2rgis -t "${_fwDomainNAME}, ${fwVARIABLE} ${_fwVersionSTR} (${_fwDomainRES}, Daily${fwSUFFIX}})"\
		                     -m ${_fwRGISDomainFILE} -s blue ${fwGDSFileNAME} ${fwRGISFileNAME} &
		[ "${fwPROC}" -ge "${_fwMAXPROC}" ] && { wait; (( fwPROC = 0 )); }
		local fwRGISFileNAME="$(_fwRGISFilename  "${fwVARIABLE}" "${_fwVersionSTR}" "${fwYEAR}" "m")"
		${_fwRGISBIN}dsAggregate -e month -a ${fwAMODE} ${fwGDSFileNAME} |\
		${_fwRGISBIN}ds2rgis -t "${_fwDomainNAME}, ${fwVARIABLE} ${_fwVersionSTR} (${_fwDomainRES}, Monthly${fwSUFFIX}})"\
		                     -m ${_fwRGISDomainFILE} -s blue - ${fwRGISFileNAME} &
		[ "${fwPROC}" -ge "${_fwMAXPROC}" ] && { wait; (( fwPROC = 0 )); }
		local fwRGISFileNAME="$(_fwRGISFilename  "${fwVARIABLE}" "${_fwVersionSTR}" "${fwYEAR}" "a")"
		${_fwRGISBIN}dsAggregate -e year -a ${fwAMODE} ${fwGDSFileNAME} |\
		${_fwRGISBIN}ds2rgis -t "${_fwDomainNAME}, ${fwVARIABLE} ${_fwVersionSTR} (${_fwDomainRES}, Yearly${fwSUFFIX}})"\
		                     -m ${_fwRGISDomainFILE} -s blue - ${fwRGISFileNAME} &
	done
	wait
	[ "${_fwVERBOSE}" == "on" ] && { echo "      Postprocessing ${fwYEAR} finished: $(date)"; }
	return 0
}

function _fwSpinup()
{
	local fwPASS
	local fwInputITEM
	local fwOutputITEM

	local fwOptionsFILE="${_fwGDSLogDIR}/Spinup0_Options.log"
	local     fwUserLOG="file:${_fwGDSLogDIR}/Spinup0_UserError.log"
	local    fwDebugLOG="file:${_fwGDSLogDIR}/Spinup0_Debug.log"
	local  fwWarningLOG="file:${_fwGDSLogDIR}/Spinup0_Warnings.log"
	local     fwInfoLOG="file:${_fwGDSLogDIR}/Spinup0_Info.log"

	[ "${_fwVERBOSE}" == "on" ] && echo "Initialization started:  $(date)"
	_fwPreprocess "dostate" "" || return -1
	for ((fwPASS = 1; fwPASS <= _fwPASSNUM; ++fwPASS))
	do
		fwOptions=$(echo ${_fwGDSDomainFILE}
		echo "-m sys_error=on"
		echo "-m app_error=on"
		echo "-m usr_error=${fwUserLOG}"
		echo "-m     debug=${fwDebugLOG}"
		echo "-m   warning=${fwWarningLOG}"
		echo "-m      info=${fwInfoLOG}"
		echo "$(_fwOptionList)"
		for (( fwI = 0; fwI < ${#_fwInputARRAY[@]} ; ++fwI ))
		do
			local     fwSOURCE=($(_fwDataSource "${_fwInputARRAY[${fwI}]}" "static"))
			if [[ "${fwSOURCE[@]}" == ""      ]]; then echo "Missing data source" > /dev/stderr; return -1; fi
			if [[ "${fwSOURCE[3]}" == "const" ]]
			then
				echo "-i ${fwSOURCE[0]}=const:${fwSOURCE[4]}"
			else
				echo "-i ${fwSOURCE[0]}=file:$(_fwGDSFilename "${fwSOURCE[0]}" "Input" "${fwSOURCE[2]}" "" "d")"
			fi
		done
		if (( fwPASS == 1 ))
		then
			for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
			do
				local     fwSOURCE=($(_fwDataSource "${_fwStateARRAY[${fwI}]}" "static"))
				if [[ "${fwSOURCE[@]}" != "" ]]
				then
					if [[ "${fwSOURCE[3]}" == "const" ]]
					then
						echo "-i ${fwSOURCE[0]}=const:${fwSOURCE[4]}"
					else
						echo "-i ${fwSOURCE[0]}=file:$(_fwGDSFilename "${fwSOURCE[0]}" "State" "${fwSOURCE[2]}" "" "d")"
					fi
				fi
			done
		else
			for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
			do
				local fwOutputITEM=(${_fwStateARRAY[${fwI}]})
				echo "-i ${fwOutputITEM}=file:$(_fwGDSFilename "${fwOutputITEM[0]}" "State" "${_fwVersionSTR}" "" "d")"
			done
		fi
		for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		do
			local fwOutputITEM=(${_fwStateARRAY[${fwI}]})
			echo "-o ${fwOutputITEM}=file:$(_fwGDSFilename "${fwOutputITEM[0]}" "State"  "${_fwVersionSTR}" "" "d")"
		done
		for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
		do
			local fwOutputITEM=(${_fwOutputARRAY[${fwI}]})
			if [ "$(_fwState ${fwOutputITEM})" == "" ]
			then
				echo "-o ${fwOutputITEM}=file:$(_fwGDSFilename "${fwOutputITEM[0]}" "Output" "${_fwVersionSTR}" "" "d")"
			fi
		done)

		echo "${fwOptions}" > ${fwOptionsFILE}
		local fwEXEC="${_fwModelBIN} ${fwOptions}"
		[ "${_fwVERBOSE}" == "on" ] && echo "   Passnum [${fwPASS}] started:  $(date)"
		if ${fwEXEC}
		then
			[ "${_fwVERBOSE}" == "on" ] && echo "   Passnum [${fwPASS}] finished: $(date)"
			local fwOptionsFILE="${_fwGDSLogDIR}/SpinupN_Options.log"
			local     fwUserLOG="file:${_fwGDSLogDIR}/SpinupN_UserError.log"
			local    fwDebugLOG="file:${_fwGDSLogDIR}/SpinupN_Debug.log"
			local  fwWarningLOG="file:${_fwGDSLogDIR}/SpinupN_Warnings.log"
			local     fwInfoLOG="file:${_fwGDSLogDIR}/SpinupN_Info.log"
		else
			[ "${_fwVERBOSE}" == "on" ] && echo "   Passnum [${fwPASS}] failed:   $(date)"
			return -1
		fi
	done
	[ "${_fwPOSTPROCESS}" == "on" ] && { _fwPostprocess "" || return -1; }
	[ "${_fwVERBOSE}" == "on" ] && echo "Initialization finished: $(date)"
	return 0
}

function _fwRun()
{
	local   fwStartYEAR="${1}"
	local     fwEndYEAR="${2}"

	[ "${_fwVERBOSE}" == "on" ] && echo "Model run started:  $(date)"
	for (( fwYEAR = fwStartYEAR; fwYEAR <= fwEndYEAR; ++fwYEAR ))
	do
		[ "${_fwVERBOSE}" == "on" ] && echo "   Running year [${fwYEAR}] started:  $(date)"
		if (( fwYEAR == fwStartYEAR )); then fwDOSTATE="dostate"; else fsDOSTATE="nostate"; fi
		_fwPreprocess "${fwDOSTATE}" "${fwYEAR}" || return -1

		local fwOptionsFILE="${_fwGDSLogDIR}/Run${fwYEAR}_Options.log"
		local     fwUserLOG="file:${_fwGDSLogDIR}/Run${fwYEAR}_UserError.log"
		local    fwDebugLOG="file:${_fwGDSLogDIR}/Run${fwYEAR}_Debug.log"
		local  fwWarningLOG="file:${_fwGDSLogDIR}/Run${fwYEAR}_Warnings.log"
		local     fwInfoLOG="file:${_fwGDSLogDIR}/Run${fwYEAR}_Info.log"

		fwOptions=$(echo ${_fwGDSDomainFILE}
		echo "-m sys_error=on"
		echo "-m app_error=on"
		echo "-m usr_error=${fwUserLOG}"
		echo "-m     debug=${fwDebugLOG}"
		echo "-m   warning=${fwWarningLOG}"
		echo "-m      info=${fwInfoLOG}"
		echo "$(_fwOptionList)"
		if (( fwYEAR == fwStartYEAR ))
		then
			for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		 	do
				local fwInputITEM=(${_fwStateARRAY[${fwI}]})
				echo "-i ${fwInputITEM}=file:$(_fwGDSFilename "${fwInputITEM[0]}" "State" "${_fwVersionSTR}" "" "d")"
			done
		else
			for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		 	do
				local fwInputITEM=(${_fwStateARRAY[${fwI}]})
				echo "-i ${fwInputITEM}=file:$(_fwGDSFilename "${fwInputITEM[0]}" "State" "${_fwVersionSTR}" "$(( fwYEAR - 1 ))" "d")"
			done
		fi
		for (( fwI = 0; fwI < ${#_fwInputARRAY[@]} ; ++fwI ))
		do
			local  fwInputITEM=(${_fwInputARRAY[${fwI}]})
			local     fwSOURCE=($(_fwDataSource "${_fwInputARRAY[${fwI}]}" "dynamic"))
			if [ "${fwSOURCE[0]}" == ""      ]
			then
				local  fwSOURCE=($(_fwDataSource "${_fwInputARRAY[${fwI}]}" "static"))
				if [ "${fwSOURCE[0]}" == ""      ]
				then
					echo "Missing time series input: ${_fwInputARRAY[${fwI}]}" > /dev/stderr
					return -1
				fi
				if [ "${fwSOURCE[3]}" == "const" ]
				then
					echo "-i ${fwSOURCE[0]}=const:${fwSOURCE[4]}"
				else
					echo "-i ${fwSOURCE[0]}=file:$(_fwGDSFilename "${fwSOURCE[0]}" "Input" "${fwSOURCE[2]}" "" "d")"
				fi
			else
				if [ "${fwSOURCE[3]}" == "const" ]
				then
					echo "-i ${fwSOURCE[0]}=const:${fwSOURCE[4]}"
				else
					echo "-i ${fwSOURCE[0]}=file:$(_fwGDSFilename "${fwSOURCE[0]}" "Input" "${fwSOURCE[2]}" "${fwYEAR}" "d")"
				fi
			fi
		done
		for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
 		do
			local fwStateITEM=(${_fwStateARRAY[${fwI}]})
			echo "-o ${fwStateITEM}=file:$(_fwGDSFilename "${fwStateITEM}" "State" "${_fwVersionSTR}" "${fwYEAR}" "d")"
 		done
		for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
 		do
			local fwOutputITEM=${_fwOutputARRAY[${fwI}]}
			if [ "$(_fwState ${fwOutputITEM})" == "" ]
			then
				echo "-o ${fwOutputITEM}=file:$(_fwGDSFilename "${fwOutputITEM[0]}" "Output" "${_fwVersionSTR}" "${fwYEAR}" "d")"
			fi
 		done)
		echo "${fwOptions}" > ${fwOptionsFILE}
		local fwEXEC="${_fwModelBIN} ${fwOptions}"
		if ${fwEXEC}
		then
			[ "${_fwPOSTPROCESS}" == "on" ] && { _fwPostprocess "${fwYEAR}" || return -1; }
			[ "${_fwVERBOSE}" == "on" ] && echo "   Running year [${fwYEAR}] finished: $(date)"
		else
			[ "${_fwVERBOSE}" == "on" ] && echo "   Running year [${fwYEAR}] failed:   $(date)"
			return -1
		fi
	done
	[ "${_fwVERBOSE}" == "on" ] && echo "Model run finished: $(date)"
	return 0
}

function FwRun()
{
	local fwStartYEAR=${1}
	local   fwEndYEAR=${2}

	[ "${_fwTESTONLY}" == "on" ] && { { _fwPrintTest || return -1; } && return 0; }
	[ -e "${_fwGDSLogDIR}" ] || mkdir -p ${_fwGDSLogDIR}
	[ "${_fwSPINUP}"   == "on" ] && { _fwSpinup                          || { echo "Spinup failed";    return -1; } }
	[ "${_fwFINALRUN}" == "on" ] && { _fwRun ${fwStartYEAR} ${fwEndYEAR} || { echo "Final run failed"; return -1; } }
	return 0
}
