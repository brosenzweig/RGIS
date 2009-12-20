/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

DBDate.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>

static DBInt _DBNumberOfDays [12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static void _DBSetFebruary (DBInt leapYear) {_DBNumberOfDays [1] = leapYear ? 29 : 28; }

DBInt DBDateIsLeapYear (DBInt year)

	{
	DBInt leapYear = false;
	
	if (year == DBDefaultMissingIntVal) return (leapYear);
	if ((year & 0x03) == 0x00) leapYear = true;
	if (((DBInt) (year / 100)) * 100 == year)
		{
		if (((year / 100) & 0x03) == 0) leapYear = true;
		else	leapYear = false;
		}
	return (leapYear);
	}

int DBNumberOfDays (DBInt year, DBInt month) {
	_DBSetFebruary(DBDateIsLeapYear (year));
	return (_DBNumberOfDays[month - 1]);
}

DBInt DBDateIsLeapYear (DBDate date) { return ( DBDateIsLeapYear (date.Year ())); }

void DBDate::Set (DBInt year,DBInt month,DBInt day,DBInt hour,DBInt min)

	{
	MonthsVAR = 0;
	MinutesVAR = 0;
	FlagsVAR = 0;
	if (year != DBDefaultMissingIntVal)
		{
		_DBSetFebruary (DBDateIsLeapYear (year));
		FlagsVAR |= DBTimeStepYear;
		MonthsVAR += year * 12;
		}
	else	FlagsVAR = 0;
	if (month != DBDefaultMissingIntVal)
		{
		FlagsVAR |= DBTimeStepMonth;
		MonthsVAR += month;
		if (day != DBDefaultMissingIntVal)
			{
			FlagsVAR |= DBTimeStepDay;
			MinutesVAR += day * 24 * 60;
			if (hour != DBDefaultMissingIntVal)
				{
				FlagsVAR |= DBTimeStepHour;
				MinutesVAR += hour * 60;
				if (min != DBDefaultMissingIntVal)
					{
					FlagsVAR |= DBTimeStepMinute;
					MinutesVAR += min;
					}
				else FlagsVAR &= ~DBTimeStepMinute;
				}
			else FlagsVAR &= ~(DBTimeStepHour | DBTimeStepMinute);
			}
		else FlagsVAR &= ~(DBTimeStepDay | DBTimeStepHour | DBTimeStepMinute);
		}
	else FlagsVAR &= ~(DBTimeStepMonth | DBTimeStepDay | DBTimeStepHour | DBTimeStepMinute);
	}

DBInt DBDate::Year () const

	{
	if ((FlagsVAR & DBTimeStepYear) != DBTimeStepYear) return (DBDefaultMissingIntVal);
	return (MonthsVAR / 12);
	}
		
DBInt DBDate::YearsAD () const { return (MonthsVAR / 12); }
		
DBInt DBDate::Month () const

	{
	if ((FlagsVAR & DBTimeStepMonth) != DBTimeStepMonth) return (DBDefaultMissingIntVal);
	return (MonthsVAR - ((FlagsVAR & DBTimeStepYear) == DBTimeStepYear ? YearsAD () * 12 : 0) ); 
	}

DBInt DBDate::Day () const

	{
	DBInt flags = DBTimeStepDay;
	if ((FlagsVAR & flags) != flags) return (DBDefaultMissingIntVal);
	return (MinutesVAR / (24 * 60));
	}

DBInt DBDate::Hour () const

	{
	DBInt flags = DBTimeStepDay | DBTimeStepHour;
	if ((FlagsVAR & flags) != flags) return (DBDefaultMissingIntVal);
	return (MinutesVAR / 60 - Day () * 24);
	}

DBInt DBDate::Minute () const
			
	{
	DBInt flags = DBTimeStepDay | DBTimeStepHour | DBTimeStepMinute;
	if ((FlagsVAR & flags) != flags) return (DBDefaultMissingIntVal);
	return (MinutesVAR - Day () * 24 * 60 - Hour () * 60);
	}

DBInt DBDate::Week () const { return (DaysAD () / 7); }

DBInt DBDate::MonthsAD () const

	{
	if ((FlagsVAR & DBTimeStepMonth) == DBTimeStepMonth) return (MonthsVAR);
	if ((FlagsVAR & DBTimeStepYear)  == DBTimeStepYear)  return (YearsAD () * 12 + 6);
	else return (6);
	}

DBInt DBDate::DaysAD () const
	{
	DBInt month, days = 0, i;
	DBInt year = YearsAD ();
	
	_DBSetFebruary (DBDateIsLeapYear (year));
	days += year * 365 + year / 4 - year / 100 + year / 400;

	if ((FlagsVAR & DBTimeStepMonth) == DBTimeStepMonth) month = Month () ; else month = 5;
	for (i = 0;i < month;++i) days += _DBNumberOfDays [month];
	if ((FlagsVAR & DBTimeStepDay) == DBTimeStepDay) days += Day () ; else days += 15;
	return (days); 
	}

DBInt DBDate::HoursAD () const
	{
	DBInt hours = DaysAD () * 24;
	return ((FlagsVAR & DBTimeStepHour) == DBTimeStepHour ? hours + Hour () : hours + 12);
	}

DBInt DBDate::MinutesAD () const
	{
	DBInt minutes = HoursAD () * 60;
	return ((FlagsVAR & DBTimeStepMinute) == DBTimeStepMinute ? minutes + Minute () : minutes + 30);
	}

void DBDate::Set (const char *dateStr)

	{
	DBInt year, month, day, hour, min;

	year	= (strlen (dateStr) >  3) && (sscanf (dateStr,	 "%4d",&year) == 1)	? year		: DBDefaultMissingIntVal;
	month	= (strlen (dateStr) >  5) && (sscanf (dateStr+5, "%2d",&month) == 1)	? month - 1 : DBDefaultMissingIntVal;
	day	= (strlen (dateStr) >  8) && (sscanf (dateStr+8, "%2d",&day) == 1)	? day - 1	: DBDefaultMissingIntVal;
	hour	= (strlen (dateStr) > 11) && (sscanf (dateStr+11,"%2d",&hour) == 1)	? hour		: DBDefaultMissingIntVal;
	min	= (strlen (dateStr) > 14) && (sscanf (dateStr+14,"%2d",&min) == 1)	? min			: DBDefaultMissingIntVal;
	Set (year,month,day,hour,min);
	}

char *DBDate::Get () const

	{
	static char dateStr [DBStringLength];

	if ((FlagsVAR & DBTimeStepYear) == DBTimeStepYear)
			sprintf (dateStr,"%04d",Year ());
	else	sprintf (dateStr,"XXXX");
	if ((FlagsVAR & DBTimeStepMonth) == DBTimeStepMonth)
		sprintf (dateStr + strlen (dateStr),"-%02d",Month () + 1);
	if ((FlagsVAR & DBTimeStepDay) == DBTimeStepDay)
		sprintf (dateStr + strlen (dateStr),"-%02d",Day () + 1);
	if ((FlagsVAR & DBTimeStepHour) == DBTimeStepHour)
		sprintf (dateStr + strlen (dateStr)," %02d",Hour ());
	if ((FlagsVAR & DBTimeStepMinute) == DBTimeStepMinute)
		sprintf (dateStr + strlen (dateStr),":%02d",Minute ());
	return (dateStr);
	}

DBFloat DBDate::DecimalYear () const
	{
	DBFloat decDate;
	DBInt month, day;

	decDate  = (FlagsVAR & DBTimeStepYear)  == DBTimeStepYear  ? Year () : 0.0;
	if ((FlagsVAR & DBTimeStepMonth) != DBTimeStepMonth) return (decDate + 0.5);
	if ((FlagsVAR & DBTimeStepDay)   != DBTimeStepDay)   return (decDate + ((DBFloat) Month () + 0.5) / 12.0);
	day = (DBDateIsLeapYear (Year ()) && (Month () > 1)) ? 1 : 0;
	for (month = 0;month < Month ();++month) day += _DBNumberOfDays [month];
	if ((FlagsVAR & DBTimeStepHour)  != DBTimeStepHour)
		return (decDate + (DBFloat) day / (DBDateIsLeapYear (Year ()) ? 365.0 : 366.0));
	if ((FlagsVAR & DBTimeStepMinute) != DBTimeStepMinute)
		return (decDate + ((DBFloat) day + Hour () / 24) / (DBDateIsLeapYear (Year ()) ? 365.0 : 366.0));
	return (decDate + ((DBFloat) day + (Hour () + Minute () / 60.0) / 24) / (DBDateIsLeapYear (Year ()) ? 365.0 : 366.0));
	}

DBFloat DBDate::DecimalMonth () const
	{
	DBFloat decDate;
	DBInt month, nDays;

	decDate  = (FlagsVAR & DBTimeStepYear)  == DBTimeStepYear  ? Year () * 12 : 0.0;
	if ((FlagsVAR & DBTimeStepMonth) != DBTimeStepMonth) return (decDate + 6.0);
	if ((FlagsVAR & DBTimeStepDay)   != DBTimeStepDay)   return (decDate + ((DBFloat) Month () + 0.5) / 12.0);
	month = Month ();
	nDays = _DBNumberOfDays [month] + ((month == 1) && DBDateIsLeapYear (Year ()) ? 1 : 0);
	if ((FlagsVAR & DBTimeStepHour)  != DBTimeStepHour)
		return (decDate + Month () + ((DBFloat) Day () / (DBFloat) nDays));
	if ((FlagsVAR & DBTimeStepMinute) != DBTimeStepMinute)
		return (decDate + ((DBFloat) Day () + Hour () / 24) / (DBDateIsLeapYear (Year ()) ? 365.0 : 366.0));
	return (decDate + ((DBFloat) Day () + (Hour () + Minute () / 60.0) / 24) / (DBDateIsLeapYear (Year ()) ? 365.0 : 366.0));
	}

DBDate operator+ (DBDate date0,DBDate date1)

	{
	DBDate date;
	DBInt year = date0.Year (), month = date0.Month (), day = date0.Day ();
	DBInt hour = date0.Hour (), minute = date0.Minute ();

	_DBSetFebruary (DBDateIsLeapYear (year));
	if (((date0.FlagsVAR &  DBTimeStepMinute) == DBTimeStepMinute) &&
		 ((date1.FlagsVAR &  DBTimeStepMinute) == DBTimeStepMinute))
		{
		if ((minute += date1.Minute ()) >= 60)
			{
			minute -= 60;
			hour += 1;
			}
		}
	else	minute = DBDefaultMissingIntVal;
		
	if (((date0.FlagsVAR &  DBTimeStepHour) == DBTimeStepHour) &&
		 ((date1.FlagsVAR &  DBTimeStepHour) == DBTimeStepHour))
		{
		if ((hour += date1.Hour ()) >= 24)
			{
			hour -= 24;
			day += 1;
			}
		}
	else hour = DBDefaultMissingIntVal;

	if (((date0.FlagsVAR &  DBTimeStepDay) == DBTimeStepDay) &&
		 ((date1.FlagsVAR &  DBTimeStepDay) == DBTimeStepDay))
		{
		if ((day += date1.Day ()) >= _DBNumberOfDays [month])
			{
			day -= _DBNumberOfDays [month];
			month += 1;
			}
		}
	else day = DBDefaultMissingIntVal;

	if (((date0.FlagsVAR &  DBTimeStepMonth) == DBTimeStepMonth) &&
		 ((date1.FlagsVAR &  DBTimeStepMonth) == DBTimeStepMonth))
		{
		if ((month += date1.Month ()) >= 12)
			{
			month -= 12;
			year += 1;
			}
		}
	else month = DBDefaultMissingIntVal;
	if (((date0.FlagsVAR &  DBTimeStepYear) == DBTimeStepYear) &&
		 ((date1.FlagsVAR &  DBTimeStepYear) == DBTimeStepYear))
		year += date1.Year ();
	else	year =  DBDefaultMissingIntVal;
	date.Set (year,month,day,hour,minute);
	return (date);
	}

bool operator< (DBDate date0,DBDate date1)

	{
	DBInt year0, year1;

	year0 = (date0.FlagsVAR &  DBTimeStepYear) == DBTimeStepYear ? date0.Year () :
			 ((date1.FlagsVAR &  DBTimeStepYear) == DBTimeStepYear ? date1.Year () : DBFault);
	year1 = (date1.FlagsVAR &  DBTimeStepYear) == DBTimeStepYear ? date1.Year () :
			 ((date0.FlagsVAR &  DBTimeStepYear) == DBTimeStepYear ? date0.Year () : DBFault);
	if (year0 < year1) return (true);
	if (year0 > year1) return (false);

	if ((date0.FlagsVAR &  DBTimeStepMonth)	!= DBTimeStepMonth)		return (false);
	if	((date1.FlagsVAR &  DBTimeStepMonth)	!= DBTimeStepMonth)		return (false);
	if (date0.Month () < date1.Month ())	return (true);
	if (date0.Month () > date1.Month ())	return (false);
	if ((date0.FlagsVAR &  DBTimeStepDay)		!= DBTimeStepDay)			return (false);
	if ((date1.FlagsVAR &  DBTimeStepDay)		!= DBTimeStepDay)			return (false);
	if (date0.Day () < date1.Day ())			return (true);
	if (date0.Day () > date1.Day ())			return (false);
	if ((date0.FlagsVAR &  DBTimeStepHour)		!= DBTimeStepHour)		return (false);
	if	((date1.FlagsVAR &  DBTimeStepHour)		!= DBTimeStepHour)		return (false);
	if (date0.Hour () < date1.Hour ())		return (true);
	if (date0.Hour () > date1.Hour ())		return (false);
	if ((date0.FlagsVAR &  DBTimeStepMinute)	!= DBTimeStepMinute)		return (false);
	if ((date1.FlagsVAR &  DBTimeStepMinute)	!= DBTimeStepMinute)		return (false);
	if (date0.Minute () < date1.Minute ()) return (true);
	return (false);
	}

bool operator> (DBDate date0,DBDate date1)

	{
	DBInt year0, year1;

	year0 = (date0.FlagsVAR &  DBTimeStepYear) == DBTimeStepYear ? date0.Year () :
			 ((date1.FlagsVAR &  DBTimeStepYear) == DBTimeStepYear ? date1.Year () : DBFault);
	year1 = (date1.FlagsVAR &  DBTimeStepYear) == DBTimeStepYear ? date1.Year () :
			 ((date0.FlagsVAR &  DBTimeStepYear) == DBTimeStepYear ? date0.Year () : DBFault);
	if (year0 > year1) return (true);
	if (year0 < year1) return (false);

	if ((date0.FlagsVAR &  DBTimeStepMonth)	!= DBTimeStepMonth)	return (false);
	if	((date1.FlagsVAR &  DBTimeStepMonth) 	!= DBTimeStepMonth)	return (false);
	if (date0.Month () > date1.Month ()) 	return (true);
	if (date0.Month () < date1.Month ()) 	return (false);
	if ((date0.FlagsVAR &  DBTimeStepDay)		!= DBTimeStepDay)		return (false);
	if ((date1.FlagsVAR &  DBTimeStepDay)		!= DBTimeStepDay)		return (false);
	if (date0.Day () > date1.Day ()) 		return (true);
	if (date0.Day () < date1.Day ()) 		return (false);
	if ((date0.FlagsVAR &  DBTimeStepHour)		!= DBTimeStepHour)	return (false);
	if ((date1.FlagsVAR &  DBTimeStepHour)		!= DBTimeStepHour)	return (false);
	if (date0.Hour () > date1.Hour ()) 		return (true);
	if (date0.Hour () < date1.Hour ()) 		return (false);
	if ((date0.FlagsVAR &  DBTimeStepMinute)	!= DBTimeStepMinute)	return (false);
	if ((date1.FlagsVAR &  DBTimeStepMinute)	!= DBTimeStepMinute)	return (false);
	if (date0.Minute () > date1.Minute ()) return (true);
	return (false);
	}

bool operator== (DBDate date0,DBDate date1)

	{
	DBInt year0 = DBFault, year1 = DBFault;

	if ((date0.FlagsVAR &  DBTimeStepYear) == DBTimeStepYear) year0 = date0.Year ();
	if ((date1.FlagsVAR &  DBTimeStepYear) == DBTimeStepYear) year1 = date1.Year ();
	if ((year0 == DBFault) || (year1 == DBFault)) year0 = year1 = DBFault;
	if (year0 != year1) return (false);

	if ((date0.FlagsVAR &  DBTimeStepMonth) != DBTimeStepMonth)			return (true);
	if ((date1.FlagsVAR &  DBTimeStepMonth) != DBTimeStepMonth)			return (true);
	if (date0.Month () != date1.Month ())		return (false);
	if ((date0.FlagsVAR &  DBTimeStepDay) != DBTimeStepDay)				return (true);
	if ((date1.FlagsVAR &  DBTimeStepDay) != DBTimeStepDay)				return (true);
	if (date0.Day () != date1.Day ())			return (false);
	if ((date0.FlagsVAR &  DBTimeStepHour) != DBTimeStepHour)			return (true);
	if ((date1.FlagsVAR &  DBTimeStepHour) != DBTimeStepHour)			return (true);
	if (date0.Hour () != date1.Hour ()) 		return (false);
	if ((date0.FlagsVAR &  DBTimeStepMinute) != DBTimeStepMinute)		return (true);
	if ((date1.FlagsVAR &  DBTimeStepMinute) != DBTimeStepMinute)		return (true);
	if (date0.Minute () != date1.Minute ())	return (false);
	return (true);
	}

bool operator!= (DBDate date0,DBDate date1) { return (date0 == date1 ? false : true); }

bool operator<= (DBDate date0,DBDate date1) { return (date0 < date1 ? true : (date0 == date1 ? true : false)); }

bool operator>= (DBDate date0,DBDate date1) { return (date0 > date1 ? true : (date0 == date1 ? true : false)); }
