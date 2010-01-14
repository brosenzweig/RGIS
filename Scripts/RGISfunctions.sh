#!/bin/bash

function RGISlookupSubject ()
{
	local variable=$(echo "${1}" | tr "[A-Z]" "[a-z]")
	
	case "${variable}" in
		(air_temperature)                           #  0
			echo "AirTemperature"
		;;
		(min_air_temperature)                       #  1
			echo "AirTemperature-Min"
		;;
		(max_air_temperature)                       #  2
			echo "AirTemperature-Max"
		;;
		(diurnal_air_temperature_range)             #  3
			echo "AirTemperature-Range"
		;;
		(c_litter_fall)                             #  4
			echo "C-LitterFall"
		;;
		(cloud_cover)                               #  5
			echo "CloudCover"
		;;
		(cropping_intensity)                        #  6
			echo "Crops-Intensity"
		;;
		(daily_precipitation_fraction)              #  7
			echo "Precipitation-DailyFraction"
		;;
		(din_areal_loading)                         #  8
			echo "DIN-AreaLoading"
		;;
		(din_point_loading)                         #  9
			echo "DIN-PointLoading"
		;;
		(discharge)                                 # 10
			echo "Discharge"
		;;
		(doc_areal_loading)                         # 11
			echo "DOC-ArealLoading"
		;;
		(doc_point_loading)                         # 12
			echo "DOC-PointLoading"
		;;
		(don_areal_loading)                         # 13
			echo "DON-ArealLoading"
		;;
		(don_point_loading)                         # 14
			echo "DON-PointLoading"
		;;
		(elevation)                                 # 15
			echo "Elevation"
		;;
		(min_elevation)                             # 16
			echo "Elevation-Min"
		;;
		(max_elevation)                             # 17
			echo "Elevation-Max"
		;;
		(field_capacity)                            # 18
			echo "Soil-FieldCapacity"
		;;
		(growing_season1)                           # 19
			echo "Crops-GrowingSeason1"
		;;
		(growing_season2)                           # 20
			echo "Crops-GrowingSeason2"
		;;
		(irrigated_area_fraction)                   # 21
			echo "Irrigation-AreaFraction"
		;;
		(irrigation_efficiency)                     # 22
			echo "Irrigation-Efficiency"
		;;
		(irrigation_gross_demand)                   # 23
			echo "IrrGrossDemand"
		;;
		(network)                                   # 24
			echo "Network"
		;;
		(n_litter_fall)                             # 25
			echo "N-LitterFall"
		;;
		(nh4_areal_loading)                         # 26
			echo "NH4-ArealLoading"
		;;
		(nh4_point_loading)                         # 27
			echo "Nh4-PointLoading"
		;;
		(no3_areal_loading)                         # 28
			echo "NO3-ArealLoading"
		;;
		(no3_point_loading)                         # 29
			echo "NO3-PointLoading"
		;;
		(other_crop_fraction)                       # 30
			echo "CropFraction-Other"
		;;
		(perennial_crop_fraction)                   # 31
			echo "CropFraction-Perennial"
		;;
		(precipitation)                             # 32
			echo "Precipitation"
		;;
		(rain_pet)                                  # 33
			echo "RainPET"
		;;
		(reservoir_capacity)                        # 34
			echo "Reservoir-Capacity"
		;;
		(rice_crop_fraction)                        # 35
			echo "CropFraction-Rice"
		;;
		(rice_percolation_rate)                     # 36
			echo "Crops-RicePercolationRate"
		;;
		(rooting_depth)                             # 37
			echo "Soil-RootingDepth"
		;;
		(runoff)                                    # 38
			echo "Runoff"
		;;
		(small_reservoir_coefficient)               # 39
			echo "SmallReservoir-Coefficient"
		;;
		(soil_moisture)                             # 40
			echo "SoilMoisture"
		;;
		(soil_moisture_change)                      # 41
			echo "SoilMoistureChange"
		;;
		(vapor_pressure)                            # 42
			echo "VaporPressure"
		;;
		(vegetables_crop_fraction)                  # 43
			echo "CropFraction-Vegetables"
		;;
		(wilting_point)                             # 44
			echo "Soil-WiltingPoint"
		;;
		(wind_speed)                                # 45
			echo "WindSpeed"
		;;
		(*)
			echo "${variable}"
			echo "Defaulting directory: ${variable}" > /dev/stderr
		;;
	esac
}


function _RGISlookupFullName ()
{
	local variable=$(echo "${1}" | tr "[A-Z]" "[a-z]")

	case "${variable}" in
		(air_temperature)                           #  0
			echo "Air Temperature"
		;;
		(max_air_temperature)                       #  1
			echo "Maximum Air Temperature"
		;;
		(min_air_temperature)                       #  2
			echo "Minimum Air Temperature"
		;;
		(diurnal_air_temperature_range)             #  3
			echo "Diurnal AirTemperature Range"
		;;
		(C_litter_fall)                             #  4
			echo "C LitterFall"
		;;
		(cloud_cover)                               #  5
			echo "Cloud Cover"
		;;
		(cropping_intensity)                        #  6
			echo "Cropping Intensity"
		;;
		(din_areal_loading)                         #  7
			echo "DIN Areal Loading"
		;;
		(din_point_loading)                         #  8
			echo "DIN Point Loading"
		;;
		(discharge)                                 #  9
			echo "Discharge"
		;;
		(don_areal_loading)                         # 10
			echo "DON Areal Loading"
		;;
		(doc_point_loading)                         # 11
			echo "DOC Point Loading"
		;;
		(doc_areal_loading)                         # 12
			echo "DOC Areal Loading"
		;;
		(don_point_loading)                         # 13
			echo "DON Point Loading"
		;;
		(daily_precipitation_fraction)              # 14
			echo "Daily Precipitation Fraction"
		;;
		(elevation)                                 # 15
			echo "Elevation"
		;;
		(max_elevation)                             # 16
			echo "Maximum Elevation"
		;;
		(min_elevation)                             # 17
			echo "Minimum Elevation"
		;;
		(field_capacity)                            # 18
			echo "Field Capacity"
		;;
		(growing_season1)                           # 19 
			echo "Growing Season 1"
		;;
		(growing_season2)                           # 20
			echo "Growing Season 2"
		;;
		(irrigated_area_fraction)                   # 21 
			echo "Irrigated Area Fraction"
		;;
		(irrigation_efficiency)                     # 22 
			echo "Irrigation Efficiency"
		;;
		(irrigation_gross_demand)                   # 23
			echo "Irrigation Gross Demand"
		;;
		(network)                                   # 24
			echo "STNetwork"
		;;
		(n_litter_fall)                             # 25
			echo "N LitterFall"
		;;
		(nh4_areal_loading)                         # 26
			echo "NH4 Areal Loading"
		;;
		(nh4_point_loading)                         # 27
			echo "NH4 Point Loading"
		;;
		(no3_areal_loading)                         # 28
			echo "NO3 Areal Loading"
		;;
		(no3_point_loading)                         # 29
			echo "NO3 Point Loading"
		;;
		(other_crop_fraction)                       # 30
			echo "Other Crop Fraction"
		;;
		(perennial_crop_fraction)                   # 31
			echo "Perennial Crop Fraction"
		;;
		(precipitation)                             # 32
			echo "Precipitation"
		;;
		(rain_pet)                                  # 33
			echo "Rain Potential ET"
		;;
		(reservoir_capacity)                        # 34
			echo "Reservoir Capacity"
		;;
		(rice_crop_fraction)                        # 35
			echo "Rice Crop Fraction"
		;;
		(rice_percolation_rate)                     # 36
			echo "Rice Percolation Rate"
		;;
		(rooting_depth)                             # 37
			echo "Rooting Depth"
		;;
		(runoff)                                    # 38
			echo "Runoff"
		;;
		(small_reservoir_coefficient)               # 39
			echo "Small-reservoir Coefficient"
		;;
		(soil_moisture)                             # 40
			echo "Soil Moisture"
		;;
		(soil_moisture_change)                      # 41
			echo "Soil Moisture Change"
		;;
		(vapor_pressure)                            # 42
			echo "Vapor Pressure"
		;;
		(vegetables_crop_fraction)                  # 43
			echo "Vegetables Crop Fraction"
		;;
		(wilting_point)                             # 44
			echo "Wilting Point"
		;;
		(wind_speed)                                # 45
			echo "Wind Speed"
		;;
		(*)
			echo "${variable}"
			echo "Defaulting Full name: ${variable}" > /dev/stderr
		;;
	esac
}

function RGISlookupShadeset ()
{
	local variable=$(echo "${1}" | tr "[A-Z]" "[a-z]")
	
	case "${variable}" in
		(air_temperature|max_air_temperature|min_air_temperature) # 0 1 2
			echo "blue-to-red"
		;;
		(diurnal_air_temperature_range)             #  3
			echo "grey"
		;;
		(c_litter_fall)                             #  4
			echo "grey"
		;;
		(cloud_cover)                               #  5
			echo "grey"
		;;
		(cropping_intensity)                        #  6
			echo "grey"
		;;
		(daily_precipitation_fraction)              #  7
			echo "grey"
		;;
		(din_areal_loading)                         #  8
			echo "grey"
		;;
		(din_point_loading)                         #  9
			echo "grey"
		;;
		(discharge)                                 # 10
			echo "blue"
		;;
		(don_areal_loading)                         # 11
			echo "grey"
		;;
		(don_point_loading)                         # 12
			echo "grey"
		;;
		(doc_areal_loading)                         # 13
			echo "grey"
		;;
		(doc_point_loading)                         # 14
			echo "grey"
		;;
		(elevation|max_elevation|min_elevation)     # 15 16 17
			echo "elevation"
		;;
		(field_capacity)                            # 18
			echo "grey"
		;;
		(growing_season1)                           # 19 
			echo "grey"
		;;
		(growing_season2)                           # 20
			echo "grey"
		;;
		(irrigated_area_fraction)                   # 21
			echo "grey"
		;;
		(irrigation_efficiency)                     # 22
			echo "grey"
		;;
		(irrigation_gross_demand)                   # 23
			echo "grey"
		;;
		(network)                                   # 24
			echo "grey"
		;;
		(n_litter_fall)                             # 25
			echo "N-LitterFall"
		;;
		(nh4_areal_loading)                         # 26
			echo "grey"
		;;
		(nh4_point_loading)                         # 27
			echo "grey"
		;;
		(no3_areal_loading)                         # 28
			echo "grey"
		;;
		(no3_point_loading)                         # 29
			echo "grey"
		;;
		(other_crop_fraction)                       # 30
			echo "grey"
		;;
		(perennial_crop_fraction)                   # 31
			echo "grey"
		;;
		(precipitation)                             # 32
			echo "blue"
		;;
		(rain_pet)                                  # 33
			echo "grey"
		;;
		(reservoir_capacity)                        # 34
			echo "grey"
		;;
		(rice_crop_fraction)                        # 35
			echo "grey"
		;;
		(rice_percolation_rate)                     # 36
			echo "grey"
		;;
		(rooting_depth)                             # 37
			echo "grey"
		;;
		(runoff)                                    # 38
			echo "blue"
		;;
		(small_reservoir_coefficient)               # 39
			echo "grey"
		;;
		(soil_moisture)                             # 40
			echo "grey"
		;;
		(soil_moisture_change)                      # 41
			echo "blue-to-red"
		;;
		(vapor_pressure)                            # 42
			echo "blue"
		;;
		(vegetables_crop_fraction)                  # 43
			echo "grey"
		;;
		(wilting_point)                             # 44
			echo "grey"
		;;
		(wind_speed)                                # 45
			echo "grey"
		;;
		(*)
			echo "grey"
			echo "Defaulting shade set: ${variable}" > /dev/stderr
		;;
	esac
}

function RGISlookupAggrMethod ()
{
	local variable=$(echo "${1}" | tr "[A-Z]" "[a-z]")
	
	case "${variable}" in
		(air_temperature|max_air_temperature|min_air_temperature)  # 0 1 2 
			echo "avg"
		;;
		(diurnal_air_temperature_range)             #  3
			echo "avg"
		;;
		(cloud_cover)                               #  4
			echo "avg"
		;;
		(cropping_intensity)                        #  5
			echo "avg"
		;;
		(daily_precipitation_fraction)              #  6
			echo "avg"
		;;
		(din_areal_loading)                         #  7
			echo "sum"
		;;
		(din_point_loading)                         #  8
			echo "sum"
		;;
		(discharge)                                 #  9
			echo "avg"
		;;
		(don_areal_loading)                         # 10
			echo "sum"
		;;
		(don_point_loading)                         # 11
			echo "sum"
		;;
		(doc_areal_loading)                         # 12
			echo "sum"
		;;
		(doc_point_loading)                         # 13
			echo "sum"
		;;
		(elevation|max_elevation|min_elevation)     # 14 15 16 
			echo "avg"
		;;
		(field_capacity)                            # 17
			echo "avg"
		;;
		(growing_season1)                           # 18
			echo "avg"
		;;
		(growing_season2)                           # 19
			echo "avg"
		;;
		(irrigated_area_fraction)                   # 20
			echo "avg"
		;;
		(irrigation_efficiency)                     # 21
			echo "avg"
		;;
		(irrigation_gross_demand)                   # 22
			echo "sum"
		;;
		(network)                                   # 23
			echo "avg"
		;;
		(nh4_areal_loading)                         # 24
			echo "sum"
		;;
		(nh4_point_loading)                         # 25
			echo "sum"
		;;
		(no3_areal_loading)                         # 26
			echo "sum"
		;;
		(no3_point_loading)                         # 27
			echo "sum"
		;;
		(other_crop_fraction)                       # 28
			echo "avg"
		;;
		(perennial_crop_fraction)                   # 29
			echo "avg"
		;;
		(precipitation)                             # 30
			echo "sum"
		;;
		(rain_pet)                                  # 31
			echo "sum"
		;;
		(reservoir_capacity)                        # 32
			echo "avg"
		;;
		(rice_crop_fraction)                        # 33
			echo "avg"
		;;
		(rice_percolation_rate)                     # 34
			echo "avg"
		;;
		(rooting_depth)                             # 35
			echo "avg"
		;;
		(runoff)                                    # 36
			echo "sum"
		;;
		(small_reservoir_coefficient)               # 37
			echo "avg"
		;;
		(soil_moisture)                             # 38
			echo "avg"
		;;
		(soil_moisture_change)                      # 39
			echo "avg"
		;;
		(vapor_pressure)                            # 40
			echo "avg"
		;;
		(vegetables_crop_fraction)                  # 41
			echo "avg"
		;;
		(wilting_point)                             # 42
			echo "avg"
		;;
		(wind_speed)                                # 43
			echo "avg"
		;;
		(*)
			echo "avg"
			echo "Defaulting aggregation method: ${variable}" > /dev/stderr
		;;
	esac
}

function RGISlookupTimeStep ()
{
	local timeStep=$(echo "${1}" | tr "[A-Z]" "[a-z]")
	
	case "${timeStep}" in
		(daily)
			echo "day"
		;;
		(monthly)
			echo "month"
		;;
		(annual)
			echo "year"
		;;
	esac
}

function RGIStimeNumberOfDays ()
{
	local    year="${1}"
	local   month=$((${2} - 1))
	local century="${year%??}"

	local monthLength[0]=31
	local monthLength[1]=28
	local monthLength[2]=31
	local monthLength[3]=30
	local monthLength[4]=31
	local monthLength[5]=30
	local monthLength[6]=31
	local monthLength[7]=31
	local monthLength[8]=30
	local monthLength[9]=31
	local monthLength[10]=30
	local monthLength[11]=31
	
	if (( month == 1))
	then
		if [[ "${year}" == "${century}00" ]]
		then
			local leapYear=$(( (${century} & 0xffffe) == ${century} ? 1 : 0))	
		else
			local leapYear=$(( (${year}    & 0xffffe) == ${year}    ? 1 : 0))
		fi
	else
		local leapYear=0
	fi
	echo $((${monthLength[${month}]} + ${leapYear}))
}

function _RGISvariableDir ()
{
	local      archive="${1}"
	local       domain="${2}"
	local     variable="${3}"

	local       varDir=$(RGISlookupSubject "${variable}")

	if [ -e "${archive}/${domain%+}/${varDir}" ]
	then
		echo "${domain%+}/${varDir}"
		return 0
	else
		if [ "${domain%+}" == "${domain}" ] # Exact domain
		then
			echo "Missing ${archive}/${domain}/${varDir}" > /dev/stderr
			echo ""
			return -1
		else # Dynamic domain
			local parent="${archive}/${domain%+}/parent"
			if [ -e "${parent}" ]
			then
				_RGISvariableDir "${archive}" "$(cat ${archive}/${domain%+}/parent)+" "${variable}" || return -1
			else
				echo "Missing ${parent}" > /dev/stderr
				echo ""
				return -1
			fi
		fi
	fi
	return 0
}

function _RGISresolutionDir ()
{
	local      archive="${1}"
	local       domain="${2}"
	local     variable="${3}"
	local      product="${4}"
	local   resolution="${5}"

	local varDir=$(_RGISvariableDir "${archive}" "${domain}" "${variable}")
	if [ "${varDir}" == "" ]
	then
		return
	else
		if [ -e "${archive}/${varDir}/${product}/${resolution%+}" ]
		then
			echo "${varDir}/${product}/${resolution%+}"
		else
			if [ "${resolution%+}" == "${resolution}" ] # Exact resolution
			then
				echo "Missing ${archive}/${varDir}/${product}/${resolution}" > /dev/stderr
				echo ""
				return -1
			else # Dynamic resolution
				if [ "${resolution%km+}" == "${resolution}" ] # Geographic
				then
					local geogNum=0
					local geogRes[${geogNum}]="15sec";  (( ++geogNum ))
					local geogRes[${geogNum}]="30sec";  (( ++geogNum ))
					local geogRes[${geogNum}]="01min";  (( ++geogNum ))
					local geogRes[${geogNum}]="03min";  (( ++geogNum ))
					local geogRes[${geogNum}]="05min";  (( ++geogNum ))
					local geogRes[${geogNum}]="06min";  (( ++geogNum ))
					local geogRes[${geogNum}]="10min";  (( ++geogNum ))
					local geogRes[${geogNum}]="15min";  (( ++geogNum ))
					local geogRes[${geogNum}]="30min";  (( ++geogNum ))
					local geogRes[${geogNum}]="60min";  (( ++geogNum ))
					local geogRes[${geogNum}]="90min";  (( ++geogNum ))
					local geogRes[${geogNum}]="150min"; (( ++geogNum ))

					for (( num = 0; num < ${geogNum}; ++num ))
					do
						if [ "${geogRes[${num}]}" == "${resolution%+}" ]; then break; fi
					done
					if (( num < geogNum))
					then
						_RGISresolutionDir "${archive}" "${domain}" "${variable}"  "${product}" "${geogRes[((${num} + 1))]}+" || return -1
					else
						echo "No coarser resolution ${domain%+} ${variable} ${product} ${resolution%+}" > /dev/stderr
						return -1
					fi
				else # Cartesian
					local cartNum=0
					local cartRes[${cartNum}]="1km";    (( ++cartNum ))
					local cartRes[${cartNum}]="2km";    (( ++cartNum ))
					local cartRes[${cartNum}]="5km";    (( ++cartNum ))
					local cartRes[${cartNum}]="10km";   (( ++cartNum ))
					local cartRes[${cartNum}]="25km";   (( ++cartNum ))
					local cartRes[${cartNum}]="50km";   (( ++cartNum ))

					for (( num = 0; num < ${cartNum}; ++num ))
					do
						if [ "${cartRes[${num}]}" == "${resolution%+}" ]; then break; fi
					done
					if (( num < cartNum))
					then
						_RGISresolutionDir "${archive}" "${domain}" "${variable}"  "${product}" "${cartRes[((${num} + 1))]}+" || return -1
					else
						echo "No coarser resolution ${domain%+} ${variable} ${product} ${resolution%+}" > /dev/stderr
						return -1
					fi
				fi
			fi
		fi
	fi
	return 0
}
function _RGIStStepDir ()
{
	local tStepType="${1}"
	local     tStep="${2}"

	case "${tstepType}" in
		(TS)
			case "${tstep}" in
				(hourly)
					echo "Hourly"
				;;
				(3hourly)
					echo "3Hourly"
				;;
				(6hourly)
					echo "6Hourly"
				;;
				(daily)
					echo "Daily"
				;;
				(monthly)
					echo "Monthly"
				;;
				(annual)
					echo "Annual"
				;;
				(*)
					echo "${tstep}"
				;;
			esac
		;;
		(LT|LTmin|LTmax|LTslope|LTrange|LTstdDev|Stats|static)
			echo "Static"
		;;
	esac
	return 0
}

function RGISdirectoryPath ()
{
	local    archive="${1}"
	local     domain="${2}"
	local   variable="${3}"
	local    product="${4}"
	local resolution="${5}"
	local  tstepType="${6}"
	local      tstep=$(echo "${7}" | tr "[A-Z]" "[a-z]")

	local        dir=$(_RGIStStepDir ${tstepType} ${tstep})
	local     varDir=$(_RGISvariableDir "${archive}" "${domain}" "${variable}")

	echo "${archive}/${varDir}/${product}/${resolution}/${dir}"
}

function RGISdirectory ()
{
	local    archive="${1}"
	local     domain="${2}"
	local   variable="${3}"
	local    product="${4}"
	local resolution="${5}"
	local  tstepType="${6}"
	local      tstep=$(echo "${7}" | tr "[A-Z]" "[a-z]")

	local dir=$(_RGIStStepDir ${tstepType} ${tstep})

	local resDir=$(_RGISresolutionDir "${archive}" "${domain}" "${variable}" "${product}" "${resolution}")
	if [ "${resDir}" == "" ]
	then
		return -1
	else
		echo "${archive}/${resDir}/${dir}"
	fi
	return 0
}

function RGISfile ()
{
	local      archive="${1}"
	local       domain="${2}"
	local     variable="${3}"
	local      product="${4}"
	local   resolution="${5}"
	local    tstepType="${6}"
	local        tstep=$(echo "${7}" | tr "[A-Z]" "[a-z]")
	local    timeRange="${8}"

	case "${tstep}" in
		(hourly)
			local tstepStr="h"
		;;
		(3hourly)
			local tstepStr="3h"
		;;
		(6hourly)
			local tstepStr="6h"
		;;
		(daily)
			local tstepStr="d"
		;;
		(monthly)
			local tstepStr="m"
		;;
		(annual)
			local tstepStr="a"
		;;
		("")
			local tstepStr=""
		;;
		(*)
			echo "Unknown time step ${tstep}" > /dev/stderr
			return -1
		;;
	esac

	case "${variable}" in
		(network)
			local extension="gdbn"
		;;
		(*)
			local extension="gdbc"
		;;
	esac

	local rgisDirectory=$(RGISdirectory "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tstepType}" "${tstep}")
	local      fileName=$(_RGISresolutionDir "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" | sed "s:/:_:g" )
	if [ "${rgisDirectory}" == "" ]
	then
		echo ""
		return -1
	fi
	if [[ "${tstepType}" == "static" ]]
	then
		local tstepType="Static"
		local tstepStr=""
	fi

	local  variableName=$(RGISlookupSubject "${variable}")
	echo "${rgisDirectory}/${fileName}_${tstepStr}${tstepType}${timeRange}.${extension}"
}

function RGIStitle ()
{
	local     domain="${1}"
	local   variable="${2}"
	local    product="${3}"
	local resolution="${4}"
	local  tstepType="${5}"
	local      tstep=$(echo "${6}" | tr "[A-Z]" "[a-z]")
	local  timeRange="${7}"
	local    version="${8}"

	
	local variableFullName=$(_RGISlookupFullName "${variable}")

	if [[ "${tstepType}" == "static" ]]
	then
		local tstepString=""
	else
		case "${tstep}" in
			(hourly)
				local tstepStr="Hourly"
			;;
			(3hourly)
				local tstepStr="3Hourly"
			;;
			(6hourly)
				local tstepStr="6Hourly"
			;;
			(daily)
				local tstepStr="Daily"
			;;
			(monthly)
				local tstepStr="Monthly"
			;;
			(annual)
				local tstepStr="Annual"
			;;
			(*)
				echo "Unknown time step ${tstep}"  > /dev/stderr
				return -1
			;;
		esac
		if [[ "${timeRange}" == "" ]]
		then
			local tstepString=", ${tstepStr}${tstepType}"
		else
			local tstepString=", ${tstepStr}${tstepType}, ${timeRange}"
		fi
	fi
	echo "${domain}, ${product} ${variableFullName} (${resolution}${tstepString}) V${version}"
	return 0
}

function RGISAppend ()
{
   local    archive="${1}"
   local     domain="${2}"
   local   variable="${3}"
   local    product="${4}"
   local resolution="${5}"
   local    version="${6}"
	local  startyear="${7}"
	local    endyear="${8}"
   local      tstep="${9}"

 	local      files=""
	local  separator=" "

	for (( year = ${startyear}; year <= ${endyear}; ++year ))
	do
		local  filename=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${tstep}" "${year}")
		local     files="${files}${separator}${filename}"
		local separator=" -a "
	done

	local filename=$(RGISfile  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${tstep}" "${startyear}-${endyear}")
	local    title=$(RGIStitle              "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${tstep}" "${startyear}-${endyear}" "${version}")
	local  subject=$(RGISlookupSubject  "${variable}")
   local shadeset=$(RGISlookupShadeset "${variable}")

	grdAppendLayers -t "${title}" -d "${domain}" -u "${subject}" -v "${version}" ${files} "${filename}"
}

function RGISAggregateTS ()
{
	local    archive="${1}"
	local     domain="${2}"
	local   variable="${3}"
	local    product="${4}"
	local resolution="${5}"
	local    version="${6}"
	local  startyear="${7}"
	local    endyear="${8}"
	local  fromTStep="${9}"
	local    toTStep="${10}"

   local     files=""
   local separator=""

   for ((year = ${startyear}; year <= ${endyear} ; ++year))
   do
      local fromFile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${fromTStep}" "${year}")
      local   toFile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${toTStep}"   "${year}")
      local    title=$(RGIStitle "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${toTStep}" "${year}" "${version}")
      local  subject=$(RGISlookupSubject    "${variable}")
      local shadeset=$(RGISlookupShadeset   "${variable}")
		local   method=$(RGISlookupAggrMethod "${variable}")

      grdTSAggr -a "${method}" -e "$(RGISlookupTimeStep ${toTStep})" -t "${title}" -d "${domain}" -u "${subject}" -s "${shadeset}" "${fromFile}" "${toFile}" || return -1
   done
}

function RGISClimatology ()
{
	local    archive="${1}"
	local     domain="${2}"
	local   variable="${3}"
	local    product="${4}"
	local resolution="${5}"
	local    version="${6}"
 	local  startyear="${7}"
	local    endyear="${8}"

	local     ltDir=$(RGISdirectory "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LT" "monthly")
	local    tsFile=$(RGISfile      "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "monthly" "${startyear}-${endyear}")
	local    ltFile=$(RGISfile      "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LT" "monthly" "${startyear}-${endyear}")
	local     title=$(RGIStitle                  "${domain}" "${variable}" "${product}" "${resolution}" "LT" "monthly" "${startyear}-${endyear}")
	local   subject=$(RGISlookupSubject  "${variable}")
	local  shadeset=$(RGISlookupShadeset "${variable}")
	
	[ -e "${ltDir}" ] || mkdir -p "${ltDir}" || return -1
	grdCycleMean	-t "${title}" -d "${domain}" -u "${subject}" -v "${version}" -s "${shadeset}" -n 12 "${tsFile}" - |\
	grdDateLayers   -e "month" - "${ltFile}" || return -1
	return 0
}

function RGISCellStats ()
{
	local    archive="${1}"
	local     domain="${2}"
	local   variable="${3}"
	local    product="${4}"
	local resolution="${5}"
	local    version="${6}"
 	local  startyear="${7}"
	local    endyear="${8}"

	local  statsDir=$(RGISdirectory  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "Stats" "annual")
	local    tsFile=$(RGISfile       "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS"    "annual" "${startyear}-${endyear}")
	local statsFile=$(RGISfile       "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "Stats" "annual" "${startyear}-${endyear}")
	local     title=$(RGIStitle                   "${domain}" "${variable}" "${product}" "${resolution}" "Stats" "annual" "${startyear}-${endyear}" "${version}")
	local   subject=$(RGISlookupSubject "${variable}")

	[ -e "${statsDir}" ] || mkdir -p "${statsDir}" || return -1
	grdCellStats    -t "${title}" -u "${variable}" -d "${domain}" -v "${version}" "${tsFile}" "${statsFile}" || return -1

	local  annualLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LT"      "annual"  "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LT"      "annual"  "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Average" -l "Average" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${annualLTfile}" || return -1

	local     maxLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTmax"    "annual"  "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTmax"    "annual"  "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Maximum" -l "Maximum" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${maxLTfile}" || return -1

	local     minLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTmin"    "annual"  "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTmin"    "annual"  "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Maximum" -l "Maximum" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${minLTfile}" || return -1

	local   rangeLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTrange"  "annual" "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTrange"  "annual" "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Range"   -l "Range"   -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${rangeLTfile}" || return -1

	local   slopeLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTslope"  "annual" "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTslope"  "annual" "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "SigSlopeB1" -l "SigSlopeB1" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${slopeLTfile}" || return -1

	local  stdDevLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTstdDev" "annual" "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTstdDev" "annual" "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "StdDev" -l "StdDev" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${stdDevLTfile}" || return -1

#	rm "${statsFile}"
	return 0
}

function RGISStatistics ()
{
	local    archive="${1}"
	local     domain="${2}"
	local   variable="${3}"
	local    product="${4}"
	local resolution="${5}"
	local    version="${6}"
	local  startyear="${7}"
	local    endyear="${8}"

	RGISAppend "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}" "monthly" || return -1
	RGISAppend "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}" "annual"  || return -1

	RGISClimatology "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}"      || return -1
	RGISCellStats   "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}"      || return -1
	local  annualTSfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS"    "annual"  "${startyear}-${endyear}")
	local monthlyTSfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS"    "monthly" "${startyear}-${endyear}")
	[ -e  "${annualTSfile}" ] && rm  "${annualTSfile}"
	[ -e "${monthlyTSfile}" ] && rm "${monthlyTSfile}"
	return 0
}
