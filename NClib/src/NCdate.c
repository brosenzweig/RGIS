#include <stdio.h>
#include <string.h>
#include <math.h>
#include <NCtime.h>

bool NCGtimeLeapYear (int year)
{ return (year == 0 ?  false : ((year&0x03) != 0x00 ? false : ((year%100)  == 0 ? (((year/100)&0x03) == 0 ? true : false) : true))); }

size_t NCGtimeMonthLength (int year, size_t month)
{
	int nDays [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	month = month - 1;
	if (year > 0) year += (int) month / 12;
	month = month % 12;
	return (nDays [month] + (((month == 1) && NCGtimeLeapYear (year)) ? 1 : 0));
}

size_t NCGtimeDayOfYear (int year, size_t month, size_t day)
{
	size_t i;

	for (i = 1;i < month - 1;i++) day += NCGtimeMonthLength (year,month);
	return (day);
}

/*
bool NCGtimeLeapYear (utUnit *tUnit, double t)
{
	int year, month, day, hour, minute;
	float second;
	if (utCalendar (t, tUnit, &year, &month, &day, &hour, &minute, &second) != 0)
	{ fprintf (stderr,"Time Conversion error in: NCGtimeLeapYear ()\n"); return (false); }
	return (NCGtimeLeapYear (year));
}

size_t NCGtimeMonthLength (utUnit *tUnit, double t)
{
	int nDays [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int year, month, day, hour, minute;
	float second;

	if (utCalendar (t, tUnit, &year, &month, &day, &hour, &minute, &second) != 0)
	{ fprintf (stderr,"Time Conversion error in: NCGtimeMonthLength ()\n"); return (0); }

	month = month - 1;
	if (year > 0) year += (int) month / 12;
	month = month % 12;

	return (nDays [month] + (_NCGtimeLeapYear (year) ? 1 : 0));
}
*/

NCGstate NCGtimeParse (const char *timeStr, NCGtimeStep timeStep, utUnit *tUnit, double *t)
{
	double second;
	int year, month, day, hour, minute;

	second = 0.0;
	minute = hour = 0;
	day    = 1;
	month  = 1;
	if      (sscanf (timeStr,     "%04d", &year)  != 1)  return (NCGfailed);
	else if (sscanf (timeStr +  5,"%02d", &month) != 1)  month  = 1;
	else if (sscanf (timeStr +  8,"%02d", &day)   != 1)  day    = 1;
	else if (sscanf (timeStr + 11,"%02d", &hour)  != 1)  hour   = 0;
	else if (sscanf (timeStr + 14,"%02d", &minute) != 1) minute = 0;
	if (utInvCalendar (year,month,day,hour,minute,second,tUnit,t) != 0)
	{ fprintf (stderr,"Time Conversion error in: NCGtimeParse ()\n"); return (NCGfailed); }
	return (NCGsucceeded);
}

NCGstate NCGtimePrint (NCGtimeStep timeStep, utUnit *tUnit, double t, char *timeStr)
{
	float second;
	int year, month, day, hour, minute;

	if (utCalendar (t, tUnit, &year, &month, &day, &hour, &minute, &second) != 0)
	{ fprintf (stderr,"Time Conversion error in: NCGtimePrint ()\n"); return (NCGfailed); }
	switch (timeStep)
	{
		case NCGtimeYear:   sprintf (timeStr,"%04d",                          year); break;
		case NCGtimeMonth:  sprintf (timeStr,"%04d-%02d",                     year, month); break;
		case NCGtimeDay:    sprintf (timeStr,"%04d-%02d-%02d",                year, month, day); break;
		case NCGtimeHour:   sprintf (timeStr,"%04d-%02d-%02d %02d",           year, month, day, hour); break;
		case NCGtimeMinute: sprintf (timeStr,"%04d-%02d-%02d %02d:%02d",      year, month, day, hour, minute); break;
		case NCGtimeSecond: sprintf (timeStr,"%04d-%02d-%02d %02d:%02d %.1f", year, month, day, hour, minute,second); break;
	}
	return (NCGsucceeded);
}

const char *NCGtimeStepString (NCGtimeStep timeStep)
{
	switch (timeStep)
	{
		case NCGtimeYear:   return ("year"); 
		case NCGtimeMonth:  return ("month");
		case NCGtimeDay:    return ("day");
		case NCGtimeHour:   return ("hour");
		case NCGtimeMinute: return ("minute");
	}
	return ((char *) NULL);
}
