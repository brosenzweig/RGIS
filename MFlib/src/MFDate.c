/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

MFDate.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>


static struct MFDate_s {
	int Year;
	int Month;
	int Day;
	int DayOfYear;
} _MFStartDate, _MFEndDate, _MFCurrentDate;

char *MFDateTimeStepString (int tStep) {
	switch (tStep) {
		case MFTimeStepYear:  return ("year");
		case MFTimeStepMonth: return ("month");
		case MFTimeStepDay:   return ("day");
		default: fprintf (stderr,"Warning: Invalid time step [%d] in: MFDateTimeStepString ()\n",tStep); break;
	}
	return ("");
}

char *MFDateTimeStepUnit (int tStep) {
	switch (tStep) {
		case MFTimeStepYear:  return ("yr");
		case MFTimeStepMonth: return ("mo");
		case MFTimeStepDay:   return ("d");
		default: CMmsgPrint (CMmsgAppError,"Error: Invalid time step [%d] in: %s:%d\n",tStep,__FILE__,__LINE__); break;
	}
	return ("");
}

static int _MFDateLeapYear (int year) {
	int leapYear = 0;

	if (year == MFDefaultMissingInt) return (0);

	if ((year & 0x03) == 0x00) leapYear = 1;
	if ((year % 100)  == 0) {
		if (((year / 100) & 0x03) == 0) leapYear = 1;
		else  leapYear = 0;
	}
	return (leapYear);
}

static int _MFDateMonthLength (int year,int month) {
	int nDays [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	
	if ((month < 1) || (month > 12)) return (MFDefaultMissingInt);

	return (nDays [month - 1] + (month == 2 ? _MFDateLeapYear (year) : 0));
}

static bool _MFDateSet (struct MFDate_s *date,char *dateString) {
	int month;
	date->Year = date->Month = date->Day = MFDefaultMissingInt;
	if ((strncmp (dateString,MFDateClimatologyStr,strlen (MFDateClimatologyStr)) != 0) &&
		 (sscanf (dateString,"%d",&(date->Year)) != 1)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid date [%s] in: %s:%d\n",dateString,__FILE__,__LINE__);
		return (false);
	}
	date->DayOfYear = 183;
	if (strlen (dateString) == 4) return (true);
	if (sscanf (dateString + 5,"%d",&(date->Month)) != 1) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid date [%s] in: %s:%d\n",dateString,__FILE__,__LINE__);
		return (false);
	}
	date->DayOfYear = 0;
	for (month = 1;month < date->Month;++month) date->DayOfYear += _MFDateMonthLength (date->Year,month);
	if (strlen (dateString) == 7) { date->DayOfYear +=_MFDateMonthLength (date->Year,month) / 2; return (true); }
	if (sscanf (dateString + 8,"%d",&(date->Day)) != 1) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid date [%s] in: %s:%d\n",dateString,__FILE__,__LINE__);
		return (false);
	}
	date->DayOfYear += date->Day;
	return (true);
}

static int _MFDateCompare (struct MFDate_s *date0,struct MFDate_s *date1) {
	if ((date0->Year != MFDefaultMissingInt) && (date1->Year != MFDefaultMissingInt)) {
		if (date0->Year < date1->Year) return (-1);
		if (date0->Year > date1->Year) return  (1);
	}
	if (date0->Month == MFDefaultMissingInt) return (date1->Month == MFDefaultMissingInt ? 0 : -1);
	else {
		if (date1->Month == MFDefaultMissingInt) return (0);
		if (date0->Month < date1->Month) return (-1);
		if (date0->Month > date1->Month) return  (1);
	}
	if (date0->Day   == MFDefaultMissingInt) return (date1->Day   == MFDefaultMissingInt ? 0 : -1);
	else {
		if (date1->Day   == MFDefaultMissingInt) return (0);
		if (date0->Day   < date1->Day)   return (-1);
		if (date0->Day   > date1->Day)   return  (1);
	}
	return (0);
}


int MFDateGetCurrentDay   () { return (_MFCurrentDate.Day); }
int MFDateGetCurrentMonth () { return (_MFCurrentDate.Month); }
int MFDateGetCurrentYear  () { return (_MFCurrentDate.Year); }
int MFDateGetDayOfYear    () { return (_MFCurrentDate.DayOfYear); }
int MFDateGetMonthLength  () { return (_MFDateMonthLength (_MFCurrentDate.Year,_MFCurrentDate.Month)); }

bool MFDateSetStart   (char *startDate)   { return (_MFDateSet (&_MFStartDate,  startDate)); } 
bool MFDateSetEnd     (char *endDate)     { return (_MFDateSet (&_MFEndDate,    endDate)); }
bool MFDateSetCurrent (char *currentDate) {
	if (_MFDateSet (&_MFCurrentDate,currentDate))
		return (_MFDateCompare (&_MFStartDate,&_MFCurrentDate) < 0 ? MFDateSetStart (currentDate): true);
	return (false);
}

void MFDateRewind () {
	_MFCurrentDate.Year      = _MFStartDate.Year;
	_MFCurrentDate.Month     = _MFStartDate.Month;
	_MFCurrentDate.Day       = _MFStartDate.Day;
	_MFCurrentDate.DayOfYear = _MFStartDate.DayOfYear;
}

char *MFDateAdvance () {
	int month;

	_MFCurrentDate.Day++;
	if (_MFCurrentDate.Day > _MFDateMonthLength (_MFCurrentDate.Year,_MFCurrentDate.Month)) {
		_MFCurrentDate.Day = 1;
		_MFCurrentDate.Month++;
		if (_MFCurrentDate.Month > 12) {
			_MFCurrentDate.Month = 1;
			if (_MFCurrentDate.Year == MFDefaultMissingInt) return ((char *) NULL);
			_MFCurrentDate.Year++;
		}
	}
	_MFCurrentDate.DayOfYear = 0;
	for (month = 1;month < _MFCurrentDate.Month;++month)
		_MFCurrentDate.DayOfYear += _MFDateMonthLength (_MFCurrentDate.Year,month);
	_MFCurrentDate.DayOfYear += _MFCurrentDate.Day;
	return (_MFDateCompare (&_MFCurrentDate,&_MFEndDate) <= 0 ? MFDateGetCurrent () : (char *) NULL);	
}

char *MFDateGetCurrent () {
	static char time [MFDateStringLength];

	if (_MFCurrentDate.Year != MFDefaultMissingInt) sprintf (time,"%4d",_MFCurrentDate.Year);
	else strcpy (time,MFDateClimatologyStr);

	if (_MFCurrentDate.Month   == MFDefaultMissingInt) return (time);
	sprintf (time + strlen (time),"-%02d",_MFCurrentDate.Month);
	if (_MFCurrentDate.Day     == MFDefaultMissingInt) return (time);
	sprintf (time + strlen (time),"-%02d",_MFCurrentDate.Day);
	return (time);
}

bool MFDateCompare (char *time0,char *time1) {
	int pos, len;
	pos = (strncmp (time0,MFDateClimatologyStr,strlen (MFDateClimatologyStr)) == 0) ||
	      (strncmp (time1,MFDateClimatologyStr,strlen (MFDateClimatologyStr)) == 0) ? 4 : 0;
	len = strlen (time0 + pos) < strlen (time1 + pos) ? strlen (time0 + pos) : strlen (time1 + pos);
	if ((strcmp (time0 + pos, "-02-29") == 0) && (strcmp (time1 + pos, "-02-28") == 0)) return (true);
	return (strncmp (time0 + pos,time1 + pos,len) == 0 ? true : false);
}

int  MFDateTimeStepLength () { return (1); }
