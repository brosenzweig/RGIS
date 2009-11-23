#!/bin/bash

##########################################################################################################
#
# MFlib Shell scripts functions for new RGISarchive layout.
#
##########################################################################################################

_fwMAXPROC=1
_fwPASSNUM=5
_fwRESTART="off"

function _fwDataSource()
{
	for (( fwI = 0; fwI < ${#_fwDSourceARRAY[@]} ; ++fwI ))
	do
		local fwTEMP=(${_fwDSourceARRAY[${fwI}]})
		[ "${fwTEMP[0]}${fwTEMP[1]}" == "${1}${2}" ] && { echo "${_fwDSourceARRAY[${fwI}]}"; return 0; }
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
	_fwLENGTHCORRECTION=""
	     _fwPOSTPROCESS="on"
          _fwPREPROCESS="auto"
          _fwPURGEFILES="on"
	         FwWARNINGS="on"
	        _fwTESTONLY="off"
	    _fwOPTIONSPRINT="off"
	     _fwDAILYOUTPUT="off"
	          FwVERBOSE="off"
	while [ "${1}" != "" ]
	do
		case ${1} in
			(-a|--restart)
				shift
				case ${1} in
					(on|off)
						_fwRESTART="${1}"
					;;
					(*)
						echo "Invalid --restart argument [${1}]"
					;;
				esac
			;;
			(-s|--spinup)
				shift
				case ${1} in
					(on|off)
						_fwSPINUP="${1}"
					;;
					(*)
						echo "Invalid --spinup argument [${1}]"
					;;
				esac
			;;
			(-f|--finalrun)
				shift
				case ${1} in
					(on|off)
						_fwFINALRUN="${1}"
					;;
					(*)
						echo "Invalid --finalrun argument [${1}]"
					;;
				esac
			;;
			(-l|--lengthcorrection)
				shift
				_fwLENGTHCORRECTION="-lc ${1}"	
			;;
			(-n|--passnum)
				shift
				if (( "${1}" < 1 || "${1}" > 20 ))
				then
					echo "Invalid --preprocess argument [${1}]"
				else
					_fwPASSNUM="${1}"
				fi
			;;
			(-r|--preprocess)
				shift
				case ${1} in
					(auto|forced)
						_fwPREPROCESS="${1}"
					;;
					(*)
						echo "Invalid --preprocess argument [${1}]"
					;;
				esac
			;;
			(-p|--postprocess)
				shift
				case ${1} in
					(on|off)
						_fwPOSTPROCESS="${1}"
					;;
					(*)
						echo "Invalid --postprocess argument [${1}]"
					;;
				esac
			;;
			(-u|--purgefiles)
				shift
				case ${1} in
					(on|off)
						_fwPURGEFILES="${1}"
					;;
					(*)
						echo "Invalid --purgfiles argument [${1}]"
					;;
				esac
			;;
			(-W|--warnings)
				shift
				case ${1} in
					(on|off)
						FwWARNINGS="${1}"
					;;
					(*)
						echo "Invalid --warnings argument [${1}]"
					;;
				esac
			;;
			(-O|--optionsprint)
				_fwOPTIONSPRINT="on"
			;;
			(-P|--processors)
				shift
				if (( "${1}" < 0 || "${1}" > 16 ))
				then
					echo "Invalid --process number [${1}]"
				else
					_fwMAXPROC="${1}"
				fi
			;;
			(-T|--testonly)
				_fwTESTONLY="on"
			;;
			(-D|--dailyoutput)
				shift
				case ${1} in
					(on|off)
						_fwDAILYOUTPUT="${1}"
					;;
					(*)
						echo "Invalid --dailyoutput argument [${1}]"
					;;
				esac
			;;
			(-V|--verbose)
				FwVERBOSE="on"
			;;
			(-h|--help)
				_fwPROGNAME="${0##*/}" # I don't know how this one works.
				echo "${_fwPROGNAME} [-s on|off] [-f on|off] [-p on|off] -W on|off -T -V"
				echo "           -a, --restart     on|off"
				echo "           -s, --spinup      on|off"
				echo "           -f, --finalrun    on|off"
				echo "           -l, --lengthcorrection    [value]"
				echo "           -n, --passnum     [num]"
				echo "           -r, --preprocess  auto|forced"
				echo "           -P, --processors  [# of processors]"
				echo "           -p, --postprocess on|off"
				echo "           -u, --purgefiles  on|off"
				echo "           -W, --warnings    on|off"
				echo "           -T, --testonly"
				echo "           -O, --optionsprint"
				echo "           -D, --dailyoutput  on|off"
				echo "           -V, --verbose"
				return -1
		esac
		shift
	done
	if [ "${_fwRESTART}" == "on" ]; then _fwSPINUP="off"; fi
	return 0
}

function FwInit()
{
	      _fwModelBIN=${1}	
       _fwDomainNAME=${2}
   _fwRGISDomainFILE=${3}
       _fwGDSWorkDIR=${4}
  	_fwRGISResultsDIR=${5}
	if [ "${6}" != "" ]; then export _fwRGISBIN="${6}/"; else export _fwRGISBIN=""; fi

	    _fwDomainTYPE="${_fwRGISDomainFILE##*.}"
         FwDomainRES="${_fwRGISDomainFILE%.*}"
         FwDomainRES="${FwDomainRES%_*}"
         FwDomainRES="${FwDomainRES##*_}"
	[ "${_fwRGISDomainFILE}" == "${FwDomainRES}" ] && FwDomainRES="unset"
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
			  FwDomainRES=""
			_fwOutputTYPE="gdbt"
			_fwDomainTYPE="Point"
		;;
	esac
	 _fwGDSDomainDIR="${_fwGDSWorkDIR}/${_fwDomainNAME}/${_fwDomainTYPE}_${FwDomainRES}"
	_fwGDSDomainFILE="${_fwGDSDomainDIR}/${_fwDomainNAME}${_fwDomainTYPE}_${FwDomainRES}.ds"
}

function FwDataSrc()
{
	unset _fwDSourceARRAY
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
	if (( _fwMAXPROC > 1))
	then
		echo "-P ${_fwMAXPROC}"
	fi
	for (( fwI = 0; fwI < ${#_fwOptionARRAY[@]} ; ++fwI ))
	do
		echo "-p ${_fwOptionARRAY[${fwI}]}"
	done
}

function FwOptions()
{
	(( fwI = 0 ))
	(( fwVARnum   = 0 ))
	(( fwINPUTnum = 0 ))
	(( fwSTATEnum = 0 ))
	unset _fwVariableARRAY
	unset _fwInputARRAY
	unset _fwStateARRAY
	unset _fwOptionARRAY
	while [ "${1}" != "" ]
	do
		local fwOPTION=(${1})
		_fwOptionARRAY[${fwI}]="${fwOPTION[0]}=${fwOPTION[1]}"
		(( ++fwI ))
		shift
	done
	local fwMessageOPTIONS="-m sys_error=off -m app_error=off -m usr_error=off -m debug=off -m warning=off -m info=on"
	if [ "${_fwOPTIONSPRINT}" == "on" ]
	then
		echo "-m sys_error=on"
		echo "-m app_error=on"
		echo "-m usr_error=on"
		echo "-m debug=on"
		echo "-m warning=on"
		echo "-T"
		echo "$(_fwOptionList)"
		return -1
	fi
	local fwLINES=($(${_fwModelBIN} ${_fwGDSDomain} $(_fwOptionList) -T ${fwMessageOPTIONS} | grep "XXXX"  | cut -c15-45,58-64,77-80,81-85,86-94))
	for (( fwVARnum = 0; fwVARnum < ${#fwLINES[@]} / 5 ; ++fwVARnum ))
	do
		_fwVariableITEM="${fwLINES[(( fwVARnum * 5    ))]}"
		local fwVARTYPE="${fwLINES[(( fwVARnum * 5 + 1))]}"
		local     fwSET="${fwLINES[(( fwVARnum * 5 + 2))]}"
		local    fwFLUX="${fwLINES[(( fwVARnum * 5 + 3))]}"
		local fwINITIAL="${fwLINES[(( fwVARnum * 5 + 4))]}"
		
		if [ "${fwFLUX}"    == "no"  ]
		then
			_fwVariableARRAY[${fwVARnum}]="${_fwVariableITEM} avg"
		else
			_fwVariableARRAY[${fwVARnum}]="${_fwVariableITEM} sum"
		fi
		if [ "${fwSET}"     == "no"  ]
		then
			_fwInputARRAY[${fwINPUTnum}]="${_fwVariableITEM}"
			(( ++fwINPUTnum ))
		fi
		if [ "${fwINITIAL}" == "yes" ]
		then
			_fwStateARRAY[${fwSTATEnum}]="${_fwVariableITEM}"
			(( ++fwSTATEnum ))
		fi
	done
}

function FwOutputs()
{
	unset _fwOutputARRAY
	local fwI
	local fwJ
	(( fwI = 0 ))
	while [ "${1}"	!= "" ]
	do
		if [ "$(_fwVariable ${1})" == "" ]
		then
			[ "${FwWARNINGS}" == "on" ] && echo "Skipping output variable: ${1}."
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
		if  [[ "${fwSOURCE[@]}" == ""      ]]
		then
			echo "  ${fwInputITEM[0]} is missing from data sources!"
		elif [ "${fwSOURCE[3]}" == "const" ]
		then
			echo "  ${fwInputITEM[0]} Constant [${fwSOURCE[4]}] input"		
		elif [ "${fwSOURCE[3]}" == "pipe"  ]
		then
			echo "  ${fwInputITEM[0]} Piped input"		
		elif [ "${fwSOURCE[3]}" == "file"  ]
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

function FwGDSFilename()
{
	local fwVARIABLE="${1}"
	local     fwMODE="${2}"
	local  fwVERSION="${3}"
	local     fwYEAR="${4}"
	local     fwSTEP="${5}"
	if [ "${fwYEAR}" == "" ]
	then
		local fwFILENAME="${_fwGDSDomainDIR}/${fwVERSION}/${_fwDomainNAME}_${fwMODE}_${fwVARIABLE}_${fwVERSION}_${FwDomainRES}_${fwSTEP}LT.gds"
	else
		local fwFILENAME="${_fwGDSDomainDIR}/${fwVERSION}/${_fwDomainNAME}_${fwMODE}_${fwVARIABLE}_${fwVERSION}_${FwDomainRES}_${fwSTEP}TS${fwYEAR}.gds"
	fi
	echo ${fwFILENAME}
}

function FwRGISFilename()
{
	local fwVARIABLE="${1}"
	local  fwVERSION="${2}"
	local     fwSTEP="${3}"
	local     fwYEAR="${4}"
	if [ "${fwYEAR}" == "" ]
	then
		local fwFILENAME="${_fwRGISResultsDIR}/${_fwDomainNAME}/${fwVARIABLE}/${_fwDomainNAME}_${fwVARIABLE}_${fwVERSION}_${FwDomainRES}_${fwSTEP}LT.gdbc"
	else
		local fwFILENAME="${_fwRGISResultsDIR}/${_fwDomainNAME}/${fwVARIABLE}/${_fwDomainNAME}_${fwVARIABLE}_${fwVERSION}_${FwDomainRES}_${fwSTEP}TS${fwYEAR}.gdbc"
	fi
	echo "${fwFILENAME}"
}

function _fwPreprocess()
{
	local fwDOSTATE="${1}"
	local    fwYEAR="${2}"
	local fwPROC
	[ "${FwVERBOSE}" == "on" ] && echo "      Preprocessing ${fwYEAR} started:  $(date '+%Y-%m-%d %H:%M:%S')"
	(( fwPROC = 0 ))
	[ -e "${_fwGDSDomainDIR}"        ] || mkdir -p "${_fwGDSDomainDIR}"
	[ "${_fwPREPROCESS}" == "forced" ] && rm -f "${_fwGDSDomainFILE}";
	[ -e "${_fwGDSDomainFILE}"       ] || ${_fwRGISBIN}rgis2domain ${_fwLENGTHCORRECTION} "${_fwRGISDomainFILE}" "${_fwGDSDomainFILE}";

	if [ "${fwDOSTATE}" == "dostate" ]
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
				[ "${FwVERBOSE}" == "on" ] && echo "         ${fwInputITEM} State Constant"
			elif [ "${fwSOURCE[3]}" == "file" ]
			then
				[ "${FwVERBOSE}" == "on" ] && echo "         ${fwInputITEM} State File input"
				if [ -e "${fwSOURCE[4]}" ]
				then
					local fwFILENAME="$(FwGDSFilename "${fwInputITEM}" "State" "${fwSOURCE[2]}" "${fwYEAR}" "d")"
					[ "${_fwPREPROCESS}" == "forced" ] && rm -f "${fwFILENAME}"
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
			[ "${FwVERBOSE}" == "on" ] && echo "         ${fwInputITEM} Constant input"
		elif [ "${fwSOURCE[3]}" == "file" ]
		then
			[ "${FwVERBOSE}" == "on" ] && echo "         ${fwInputITEM} File input"
			[ -e "${fwSOURCE[4]}" ] || { echo "  ${fwInputITEM} datafile [${fwSOURCE[4]}] is missing!"; return -1; }
			[ -e "${_fwGDSDomainDIR}/${fwSOURCE[2]}" ] || mkdir -p "${_fwGDSDomainDIR}/${fwSOURCE[2]}"
			local fwFILENAME="$(FwGDSFilename "${fwInputITEM}" "Input" "${fwSOURCE[2]}" "${fwInYEAR}" "d")"
			[ "${_fwPREPROCESS}" == "forced" ] && rm -f "${fwFILENAME}"
			[ -e "${fwFILENAME}" ] && continue
			[ -e "${_fwRGISDomainFILE}" ] || { echo "Missing domain file: ${_fwRGISDomainFILE}"; return -1; }
			[ "${fwPROC}" -ge "${_fwMAXPROC}" ] && { wait; (( fwPROC = 0 )) ; }
			${_fwRGISBIN}rgis2ds -m "${_fwRGISDomainFILE}" "${fwSOURCE[4]}" "${fwFILENAME}" &
			(( ++fwPROC ))
		fi
	done
	wait
	[ "${FwVERBOSE}" == "on" ] && echo "      Preprocessing ${fwYEAR} finished: $(date '+%Y-%m-%d %H:%M:%S')"
	return 0
}

function _fwPostprocess()
{
   local fwVERSION="${1}"
	local    fwYEAR="${2}"
	local fwPROC
	if [ "${fwYEAR}" == "" ]; then local fwSUFFIX="LT"; else local fwSUFFIX="TS${fwYEAR}"; fi
	[ "${FwVERBOSE}" == "on" ] && { echo "      Postprocessing ${fwYEAR} started:  $(date '+%Y-%m-%d %H:%M:%S')"; }
	(( fwPROC = 0 ))
	for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
	do
		local fwVARIABLE="${_fwOutputARRAY[${fwI}]}"
		local    fwAMODE="$(_fwVariable "${fwVARIABLE}")"
		[ "${fwAMODE}" == "" ] && { echo "Skipping undefinded variable [${fwVARIABLE}]"; continue; }
		local fwGDSFileNAME="$(FwGDSFilename "${fwVARIABLE}" "Output" "${fwVERSION}" "${fwYEAR}" "d")"
		[ -e "${fwGDSFileNAME}" ] || local fwGDSFileNAME="$(FwGDSFilename "${fwVARIABLE}" "State" "${fwVERSION}" "${fwYEAR}" "d")"
		[ -e "${fwGDSFileNAME}" ] || { echo "Skipping missing variable [${fwVARIABLE}]"; echo ${fwGDSFileNAME}; continue; }
		[ -e "${_fwRGISResultsDIR}/${_fwDomainNAME}/${fwVARIABLE}" ] || mkdir -p "${_fwRGISResultsDIR}/${_fwDomainNAME}/${fwVARIABLE}"
		if [ "${_fwDAILYOUTPUT}" == "on" ]
		then
			local fwRGISFileNAME="$(FwRGISFilename "${fwVARIABLE}" "${fwVERSION}" "d" "${fwYEAR}")"
			[ "${fwPROC}" -ge "${_fwMAXPROC}" ] && { wait; (( fwPROC = 0 )); }
			${_fwRGISBIN}ds2rgis -t "${_fwDomainNAME}, ${fwVARIABLE} ${fwVERSION} (${FwDomainRES}, Daily${fwSUFFIX})"\
			                     -m ${_fwRGISDomainFILE} -d "${_fwDomainNAME}" -u "${fwVARIABLE}" -s blue ${fwGDSFileNAME} ${fwRGISFileNAME} &
			(( ++fwPROC ))
		fi
		[ "${fwPROC}" -ge "${_fwMAXPROC}" ] && { wait; (( fwPROC = 0 )); }
		local fwRGISFileNAME="$(FwRGISFilename "${fwVARIABLE}" "${fwVERSION}" "m" "${fwYEAR}")"
		${_fwRGISBIN}dsAggregate -e month -a ${fwAMODE} ${fwGDSFileNAME} |\
		${_fwRGISBIN}ds2rgis -t "${_fwDomainNAME}, ${fwVARIABLE} ${fwVERSION} (${FwDomainRES}, Monthly${fwSUFFIX})"\
		                     -m ${_fwRGISDomainFILE}  -d "${_fwDomainNAME}" -u "${fwVARIABLE}" -s blue - ${fwRGISFileNAME} &
		(( ++fwPROC ))
		[ "${fwPROC}" -ge "${_fwMAXPROC}" ] && { wait; (( fwPROC = 0 )); }
		local fwRGISFileNAME="$(FwRGISFilename "${fwVARIABLE}" "${fwVERSION}" "a" "${fwYEAR}")"
		${_fwRGISBIN}dsAggregate -e year -a ${fwAMODE} ${fwGDSFileNAME} |\
		${_fwRGISBIN}ds2rgis -t "${_fwDomainNAME}, ${fwVARIABLE} ${fwVERSION} (${FwDomainRES}, Yearly${fwSUFFIX})"\
		                     -m ${_fwRGISDomainFILE} -d "${_fwDomainNAME}" -u "${fwVARIABLE}"  -s blue - ${fwRGISFileNAME} &
		(( ++fwPROC ))
	done
	wait
	[ "${FwVERBOSE}" == "on" ] && { echo "      Postprocessing ${fwYEAR} finished: $(date '+%Y-%m-%d %H:%M:%S')"; }
	return 0
}

function _fwSpinup()
{
	local fwVERSION="${1}"
	local fwPASS
	local fwInputITEM
	local fwOutputITEM

	local fwOptionsFILE="${_fwGDSLogDIR}/Spinup0_Options.log"
	local     fwUserLOG="file:${_fwGDSLogDIR}/Spinup0_UserError.log"
	local    fwDebugLOG="file:${_fwGDSLogDIR}/Spinup0_Debug.log"
	local  fwWarningLOG="file:${_fwGDSLogDIR}/Spinup0_Warnings.log"
	local     fwInfoLOG="file:${_fwGDSLogDIR}/Spinup0_Info.log"

	[ "${FwVERBOSE}" == "on" ] && echo "Initialization started:  $(date '+%Y-%m-%d %H:%M:%S')"
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
			if  [ "${fwSOURCE[3]}" == "const" ]
			then
				echo "-i ${fwSOURCE[0]}=const:${fwSOURCE[4]}"
			else
				echo "-i ${fwSOURCE[0]}=file:$(FwGDSFilename "${fwSOURCE[0]}" "Input" "${fwSOURCE[2]}" "" "d")"
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
						echo "-i ${fwSOURCE[0]}=file:$(FwGDSFilename "${fwSOURCE[0]}" "State" "${fwSOURCE[2]}" "" "d")"
					fi
				fi
			done
		else
			for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
			do
				local fwOutputITEM=(${_fwStateARRAY[${fwI}]})
				echo "-i ${fwOutputITEM}=file:$(FwGDSFilename "${fwOutputITEM[0]}" "State" "${fwVERSION}" "" "d")"
			done
		fi
		for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		do
			local fwOutputITEM=(${_fwStateARRAY[${fwI}]})
			echo "-o ${fwOutputITEM}=file:$(FwGDSFilename "${fwOutputITEM[0]}" "State"  "${fwVERSION}" "" "d")"
		done
		for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
		do
			local fwOutputITEM=(${_fwOutputARRAY[${fwI}]})
			if [ "$(_fwState ${fwOutputITEM})" == "" ]
			then
				echo "-o ${fwOutputITEM}=file:$(FwGDSFilename "${fwOutputITEM[0]}" "Output" "${fwVERSION}" "" "d")"
			fi
		done)

		echo "${fwOptions}" > ${fwOptionsFILE}
		[ "${FwVERBOSE}" == "on" ] && echo "   Passnum [${fwPASS}] started:  $(date '+%Y-%m-%d %H:%M:%S')"
		if echo ${fwOptions} | xargs ${_fwModelBIN}
		then
			[ "${FwVERBOSE}" == "on" ] && echo "   Passnum [${fwPASS}] finished: $(date '+%Y-%m-%d %H:%M:%S')"
			local fwOptionsFILE="${_fwGDSLogDIR}/SpinupN_Options.log"
			local     fwUserLOG="file:${_fwGDSLogDIR}/SpinupN_UserError.log"
			local    fwDebugLOG="file:${_fwGDSLogDIR}/SpinupN_Debug.log"
			local  fwWarningLOG="file:${_fwGDSLogDIR}/SpinupN_Warnings.log"
			local     fwInfoLOG="file:${_fwGDSLogDIR}/SpinupN_Info.log"
		else
			[ "${FwVERBOSE}" == "on" ] && echo "   Passnum [${fwPASS}] failed:   $(date '+%Y-%m-%d %H:%M:%S')"
			return -1
		fi
	done
	[ "${_fwPOSTPROCESS}" == "on" ] && { _fwPostprocess "${fwVERSION}" "" || return -1; }
	[ "${FwVERBOSE}"      == "on" ] && echo "Initialization finished: $(date '+%Y-%m-%d %H:%M:%S')"
	return 0
}

function _fwRun()
{
   local     fwVERSION="${1}"
	local   fwStartYEAR="${2}"
	local     fwEndYEAR="${3}"

	[ "${FwVERBOSE}" == "on" ] && echo "Model run started:  $(date '+%Y-%m-%d %H:%M:%S')"
	for (( fwYEAR = fwStartYEAR; fwYEAR <= fwEndYEAR; ++fwYEAR ))
	do
		if (( fwYEAR == fwStartYEAR ))
		then
			if [ "$_fwRESTART}" == "off" ]; then fwDOSTATE="dostate"; else fsDOSTATE="nostate"; fi
		else fsDOSTATE="nostate";
		fi
		_fwPreprocess "${fwDOSTATE}" "${fwYEAR}" || return -1

		local fwOptionsFILE="${_fwGDSLogDIR}/Run${fwYEAR}_Options.log"
		local     fwUserLOG="file:${_fwGDSLogDIR}/Run${fwYEAR}_UserError.log"
		local    fwDebugLOG="file:${_fwGDSLogDIR}/Run${fwYEAR}_Debug.log"
		local  fwWarningLOG="file:${_fwGDSLogDIR}/Run${fwYEAR}_Warnings.log"
		local     fwInfoLOG="file:${_fwGDSLogDIR}/Run${fwYEAR}_Info.log"

		fwOptions=$(echo ${_fwGDSDomainFILE} -s "${fwYEAR}-01-01" -n "${fwYEAR}-12-31"
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
				echo "-i ${fwInputITEM}=file:$(FwGDSFilename "${fwInputITEM[0]}" "State" "${fwVERSION}" "" "d")"
			done
		else
			for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		 	do
				local fwInputITEM=(${_fwStateARRAY[${fwI}]})
				echo "-i ${fwInputITEM}=file:$(FwGDSFilename "${fwInputITEM[0]}" "State" "${fwVERSION}" "$(( fwYEAR - 1 ))" "d")"
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
					echo "-i ${fwSOURCE[0]}=file:$(FwGDSFilename "${fwSOURCE[0]}" "Input" "${fwSOURCE[2]}" "" "d")"
				fi
			else
				if [ "${fwSOURCE[3]}" == "const" ]
				then
					echo "-i ${fwSOURCE[0]}=const:${fwSOURCE[4]}"
				else
					echo "-i ${fwSOURCE[0]}=file:$(FwGDSFilename "${fwSOURCE[0]}" "Input" "${fwSOURCE[2]}" "${fwYEAR}" "d")"
				fi
			fi
		done
		for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
 		do
			local fwStateITEM=(${_fwStateARRAY[${fwI}]})
			echo "-o ${fwStateITEM}=file:$(FwGDSFilename "${fwStateITEM}" "State" "${fwVERSION}" "${fwYEAR}" "d")"
 		done
		for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
 		do
			local fwOutputITEM=${_fwOutputARRAY[${fwI}]}
			if [ "$(_fwState ${fwOutputITEM})" == "" ]
			then
				echo "-o ${fwOutputITEM}=file:$(FwGDSFilename "${fwOutputITEM[0]}" "Output" "${fwVERSION}" "${fwYEAR}" "d")"
			fi
 		done)
		echo "${fwOptions}" > ${fwOptionsFILE}
		[ "${FwVERBOSE}" == "on" ] && echo "   Running year [${fwYEAR}] started:  $(date '+%Y-%m-%d %H:%M:%S')"
		if echo ${fwOptions} | xargs ${_fwModelBIN}
		then
			[ "${_fwPOSTPROCESS}" == "on" ] && { _fwPostprocess "${fwVERSION}" "${fwYEAR}" || return -1; }
			[ "${FwVERBOSE}"      == "on" ] && echo "   Running year [${fwYEAR}] finished: $(date '+%Y-%m-%d %H:%M:%S')"
		else
			[ "${FwVERBOSE}"      == "on" ] && echo "   Running year [${fwYEAR}] failed:   $(date '+%Y-%m-%d %H:%M:%S')"
			return -1
		fi
		if [ "${_fwPURGEFILES}" == "on" ] && (( fwYEAR != fwStartYEAR ))
		then
			for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
			 do
				local fwInputITEM=(${_fwStateARRAY[${fwI}]})
				rm -f "$(FwGDSFilename "${fwInputITEM[0]}" "State" "${fwVERSION}" "$(( fwYEAR - 1 ))" "d")"
			done
		fi
	done
	if [ "${_fwPURGEFILES}" == "on" ]
	then
		for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		 do
			local fwInputITEM=(${_fwStateARRAY[${fwI}]})
			rm -f "$(FwGDSFilename "${fwInputITEM[0]}" "State" "${fwVERSION}" "$(( fwYEAR - 1 ))" "d")"
		done
	fi

	[ "${FwVERBOSE}" == "on" ] && echo "Model run finished: $(date '+%Y-%m-%d %H:%M:%S')"
	return 0
}

function FwRun()
{
	local   fwVERSION="${1}"
	local fwStartYEAR="${2}"
	local   fwEndYEAR="${3}"
	     _fwGDSLogDIR="${_fwGDSDomainDIR}/${fwVERSION}/logs"

	[ "${_fwTESTONLY}" == "on" ] && { { _fwPrintTest || return -1; } && return 0; }
	[ -e "${_fwGDSLogDIR}" ] || mkdir -p ${_fwGDSLogDIR}
	[ "${_fwSPINUP}"   == "on" ] && { _fwSpinup "${fwVERSION}"                                 || { echo "Spinup failed";    return -1; } }
	[ "${_fwFINALRUN}" == "on" ] && { _fwRun    "${fwVERSION}" "${fwStartYEAR}" "${fwEndYEAR}" || { echo "Final run failed"; return -1; } }
	return 0
}
