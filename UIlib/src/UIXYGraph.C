/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

UIXYGraph.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Scale.h>
#include <Xm/ScrolledW.h>
#include <Xm/ScrollBar.h>
#include <Xm/DrawingA.h>
#include <UI.H>

UIDataSeries::UIDataSeries (char *name,DBCoordinate *coords,DBInt obsNum,DBInt symbol) : DBObject (name,sizeof (UIDataSeries))

	{
	if ((DataPTR = (void *) calloc (sizeof (DBCoordinate),obsNum)) == (void *) NULL)
		{ perror ("Memory Allocation Error in: UIXYSeries::UIXYSeries ()"); return; }
	TypeVAR = UIDataXYSeries;
	SymbolVAR = symbol;
	Initialize ();
	for (ObsNumVAR = 0;ObsNumVAR < obsNum;++ObsNumVAR)
		{
		((DBCoordinate *) DataPTR) [ObsNumVAR] = coords [ObsNumVAR];
		XYRangeVAR.Expand (((DBCoordinate *) DataPTR) [ObsNumVAR]);
		}
	}

UIDataSeries::UIDataSeries (char *name,DBObservation *obs,DBInt obsNum,DBInt symbol) : DBObject (name,sizeof (UIDataSeries))

	{
	if ((DataPTR = (void *) calloc (sizeof (DBObservation),obsNum)) == (void *) NULL)
		{ perror ("Memory Allocation Error in: UIXYSeries::UIXYSeries ()"); return; }
	TypeVAR = UIDataTimeSeries;
	SymbolVAR = symbol;
	Initialize ();
	for (ObsNumVAR = 0;ObsNumVAR < obsNum;++ObsNumVAR)
		{
		((DBObservation *) DataPTR) [ObsNumVAR] = obs [ObsNumVAR];
		ObsRangeVAR.Expand (((DBObservation *) DataPTR) [ObsNumVAR]);
		}
	}

UIDataSeries::UIDataSeries (char *name,DBObjTable *table,DBObjTableField *idFLD, DBObjTableField *xAxisFLD, DBObjTableField *yAxisFLD,char *idString,DBInt symbol) : DBObject (name,sizeof (UIDataSeries))

	{
	DBInt objSize, rowID, varNum;
	DBObjRecord *record;

	TypeVAR = xAxisFLD->Type () == DBTableFieldDate ? UIDataTimeSeries : UIDataXYSeries;
	objSize = xAxisFLD->Type () == DBTableFieldDate ? sizeof (DBObservation) : sizeof (DBCoordinate);
	SymbolVAR = symbol;
	Initialize ();
	ObsNumVAR = 0;
	varNum = 1;
	DataPTR = (void *) NULL;
	NoDataVAR = yAxisFLD->FloatNoData ();
	for (rowID = 0;rowID < table->ItemNum ();++rowID)
		{
		record = table->Item (rowID);
		if (strcmp (idFLD->String (record),idString) == 0)
			{
			if (varNum > ObsNumVAR)
				{
				if ((DataPTR = (void *) realloc (DataPTR,objSize * varNum)) == (void *) NULL)
					{ perror ("Memory Allocation Error in: UIDataSeries::UIDataSeries ()"); ObsNumVAR = 0; return; }
				ObsNumVAR = varNum;
				}
			if (TypeVAR == UIDataXYSeries)
				{
				((DBCoordinate *) DataPTR) [varNum - 1].X = xAxisFLD->Float (record);
				((DBCoordinate *) DataPTR) [varNum - 1].Y = yAxisFLD->Float (record);
				if (CMmathEqualValues (((DBCoordinate *) DataPTR) [varNum - 1].X,xAxisFLD->FloatNoData ())) continue;
				if (CMmathEqualValues (((DBCoordinate *) DataPTR) [varNum - 1].Y,yAxisFLD->FloatNoData ())) continue;
				XYRangeVAR.Expand (((DBCoordinate *) DataPTR) [varNum - 1]);
				}
			else
				{
				((DBObservation *) DataPTR) [varNum - 1].Date  = xAxisFLD->Date (record);
				((DBObservation *) DataPTR) [varNum - 1].Value = yAxisFLD->Float (record);
				if (CMmathEqualValues (((DBObservation *) DataPTR) [varNum - 1].Value,yAxisFLD->FloatNoData ())) continue;
				ObsRangeVAR.Expand (((DBObservation *) DataPTR) [varNum - 1]);
				}
			varNum++;
			}
		}
	if (TypeVAR == UIDataXYSeries)
			qsort (DataPTR,ObsNumVAR,objSize,(int (*) (const void *, const void *)) DBMathCoordinateXCompare);
	else	qsort (DataPTR,ObsNumVAR,objSize,(int (*) (const void *, const void *)) DBMathObservationTimeCompare);
	}

static void _UIXYGraphScaleChangedCBK (Widget widget,UIXYGraph *graph,XmScaleCallbackStruct *callData)

	{	widget = widget;	graph->SetViewPercent (callData->value);	graph->DrawSeries (true); }

static void _UIXYGraphScrollBarChangedCBK (Widget widget,UIXYGraph *graph,XmScaleCallbackStruct *callData)

	{ widget = widget;	graph->SetViewOffset (callData->value);	graph->DrawSeries (true); }

static void _UIXYGraphDrawingResizeCBK (Widget widget,Widget yAxis,XmDrawingAreaCallbackStruct *callData)

	{
	Dimension height;

	callData = callData;
	XtVaGetValues (widget,XmNheight,&height, NULL); 
	XtVaSetValues (yAxis, XmNheight,height - 20, NULL);
	if (XtIsRealized (widget))
		{
		XClearWindow (XtDisplay (widget),XtWindow (widget));
		XClearWindow (XtDisplay (yAxis), XtWindow (yAxis));
		}
	}

static void _UIXYGraphDrawingExposeCBK (Widget widget,UIXYGraph *graph,XmDrawingAreaCallbackStruct *callData)

	{ widget = widget; callData = callData; graph->DrawSeries (false); }

static void _UIXYyAxisExposeCBK (Widget widget,UIXYGraph *graph,XmDrawingAreaCallbackStruct *callData)

	{ widget = widget; callData = callData; graph->DrawYAxis (false); }

void UIXYGraph::SetViewPercent (DBInt percent)

	{
	int value, sliderSize ,increment, pageIncr;

	XmScrollBarGetValues (ScrollBarWGT,&value,&sliderSize,&increment, &pageIncr);
	sliderSize = percent;
	value = value + sliderSize > 100 ? 100 - sliderSize : value;
	pageIncr  = (int) (sliderSize * 0.9) < 100 - sliderSize ? (int) (sliderSize * 0.9) : 100 - sliderSize;
	increment = (int) (sliderSize * 0.1) < pageIncr ? (int) (sliderSize * 0.1) : pageIncr;
	XmScrollBarSetValues (ScrollBarWGT,value,sliderSize,increment,pageIncr,False);
	ViewPercentVAR = percent;
	ViewOffsetVAR = value;
	}

void UIXYGraph::SetViewOffset (DBInt offset) { ViewOffsetVAR = offset; }
	
static DBFloat _UIXYGraphCalculateStep (DBFloat *min,DBFloat *max,DBInt stepNum,char *format)

	{
	DBFloat step, range;
	DBInt power, integers, decimals;
	
	if ((range = fabs (*max - *min)) < 0.000001)
		{
		sprintf (format + 1,"8.5f");
		format [0] = '%';
		return (1.0);
		}

	power = (DBInt) floor (log10 (range / stepNum));
	step = pow (10.0,(double) power + 1.0);
	if (step * stepNum > range) step = step / 2.0;
	if (step * stepNum > range) step = step / 2.0;
	if (step * stepNum > range) step =  pow (10.0,(double) power);
	*min = floor (*min / step) * step;
	*max = ceil  (*max / step) * step;
	decimals = 2 - power > 0 ? 2 - power : 0;
	integers = *max > range ? abs ((DBInt) ceil (log10 (*max))) : abs ((DBInt) ceil (log10 (range)));
	sprintf (format + 1,"%d.%df",2 + integers + decimals,decimals);
	format [0] = '%';
	return (step);
	}
	
static DBDate _UITimeGraphCalculateStep (DBDate *min,DBDate *max,DBInt stepNum,char *format)

	{
	DBDate step;
	DBInt range;
	
	range = abs (max->MinutesAD () - min->MinutesAD ()) / stepNum;
	step.Set (0,0,0,0,1);
	strcpy (format,"%4d-%2d-%2d[%2d:%2d]");
	if (range > 2)		step.Set (0,0,0,0,2);
	if (range > 5)		step.Set (0,0,0,0,5);
	if (range > 10)	step.Set (0,0,0,0,10);
	if (range > 15)	step.Set (0,0,0,0,15);
	if (range > 30)	step.Set (0,0,0,0,30);
	if ((range = abs (max->HoursAD () - min->HoursAD ()) / stepNum) > 0)
		{
		step.Set (0,0,0,1);
		strcpy (format,"%4d-%2d-%2d[%2d]");
		if (range > 2)		step.Set (0,0,0,2);
		if (range > 3)		step.Set (0,0,0,3);
		if (range > 6)		step.Set (0,0,0,6);
		if (range > 12)	step.Set (0,0,0,12);
		}
	if ((range = abs (max->DaysAD () - min->DaysAD ()) / stepNum) > 0)
		{
		step.Set (0,0,1);
		strcpy (format,"%4d-%2d-%2d");
		if (range > 2)		step.Set (0,0,2);
		if (range > 5)		step.Set (0,0,5);
		if (range > 7)		step.Set (0,0,7);
		if (range > 10)	step.Set (0,0,10);
		}
	if ((range = abs (max->MonthsAD () - min->MonthsAD ()) / stepNum) > 0)
		{
		step.Set (0,1);
		strcpy (format,"%4d-%2d");
		if (range > 2) step.Set (0,2);
		if (range > 3) step.Set (0,3);
		if (range > 6) step.Set (0,6);
		}
	if ((range = abs (max->YearsAD () - min->YearsAD ()) / stepNum) > 0)
		{
		step.Set (1);
		strcpy (format,"%4d");
		if (range > 2)		step.Set (2);
		if (range > 5)		step.Set (5);
		if (range > 10)	step.Set (10);
		if (range > 20)	step.Set (20);
		if (range > 50)	step.Set (50);
		if (range > 100)	step.Set (100);
		if (range > 200)	step.Set (200);
		if (range > 500)	step.Set (500);
		}
	if (step.Year ()	> 0)
		{
		min->Set (((DBInt) floor ((double) min->Year ()	/ (double) step.Year ())) * step.Year ());
		max->Set (((DBInt) ceil  ((double) max->Year ()	/ (double) step.Year ())) * step.Year ());
		}
	else if (step.Month () > 0)
		{
		min->Set (min->Year (),((DBInt) floor ((double) min->Month ()	/ (double) step.Month ())) * step.Month ());
		max->Set (max->Year (),((DBInt) ceil  ((double) max->Month ()	/ (double) step.Month ())) * step.Month ());
		}
	else if (step.Day () > 0)
		{
		min->Set (min->Year (),min->Month (),((DBInt) floor ((double) min->Day ()	/ (double) step.Day ())) * step.Day ());
		max->Set (max->Year (),max->Month (),((DBInt) ceil  ((double) max->Day ()	/ (double) step.Day ())) * step.Day ());
		}
	else if (step.Hour () > 0)
		{
		min->Set (min->Year (),min->Month (),min->Day (),((DBInt) floor ((double) min->Hour ()	/ (double) step.Hour ()))	* step.Hour ());	
		max->Set (max->Year (),max->Month (),max->Day (),((DBInt) ceil  ((double) max->Hour ()	/ (double) step.Hour ()))	* step.Hour ());
		}
	else
		{
		min->Set (min->Year (),min->Month (),min->Day (),min->Hour (),((DBInt) floor ((double )min->Minute () / (double) step.Minute ()))* step.Minute ());
		max->Set (max->Year (),max->Month (),max->Day (),max->Hour (),((DBInt) ceil  ((double )max->Minute () / (double) step.Minute ()))* step.Minute ());
		}
	return (step);
	}

void UIXYGraph::SetRange ()

	{
	Dimension width, height;
	UIDataSeries *series;

	XYRangeVAR.Initialize ();
	ObsRangeVAR.Initialize ();
	if ((series = First ()) == (UIDataSeries *) NULL) return;

	XtVaGetValues (DrawingAreaWGT,XmNwidth, &width,XmNheight, &height, NULL);
	if (series->Type () == UIDataXYSeries)
		{
		for (;series != (UIDataSeries *) NULL;series = Next ())
			XYRangeVAR.Expand (series->XYRange ());
		XYStepVAR.X = _UIXYGraphCalculateStep (&XYRangeVAR.LowerLeft.X,&XYRangeVAR.UpperRight.X,width / 100,XFormatSTR);
		XYStepVAR.Y = _UIXYGraphCalculateStep (&XYRangeVAR.LowerLeft.Y,&XYRangeVAR.UpperRight.Y,height / 40,YFormatSTR);
		}
	else
		{
		for (;series != (UIDataSeries *) NULL;series = Next ())
			ObsRangeVAR.Expand (series->ObsRange ());
		ObsStepVAR.Date  = _UITimeGraphCalculateStep (&ObsRangeVAR.Min.Date,&ObsRangeVAR.Max.Date,width / 100,XFormatSTR);
		ObsStepVAR.Value = _UIXYGraphCalculateStep (&ObsRangeVAR.Min.Value,&ObsRangeVAR.Max.Value,height / 40,YFormatSTR);
		}
	}

void UIXYGraph::DrawSeries (int clear)

	{
	DBInt i;
	UIDataSeries *series = First ();
	char numberStr [DBStringLength];
	DBInt stepNum, x, x0, y0, x1, y1;
	Dimension width, height, yAxisWidth, yAxisHeight, strWidth, strHeight;
	XGCValues xgcv;
	XmString string;
	
	if (series == (UIDataSeries *) NULL) return;
	if (XtIsRealized (DrawingAreaWGT) == False) return;
	if (clear)	XClearWindow (XtDisplay (DrawingAreaWGT),	XtWindow (DrawingAreaWGT));

	XtVaGetValues (DrawingAreaWGT,XmNwidth,	&width, 		XmNheight,	&height, 		NULL);
	XtVaGetValues (YAxisWGT,		XmNwidth,	&yAxisWidth, XmNheight,	&yAxisHeight,	NULL);

	xgcv.foreground = UIColor (UIColorStandard,1);
	XChangeGC (XtDisplay (DrawingAreaWGT),Gc,GCForeground,&xgcv);
	XDrawLine (XtDisplay (DrawingAreaWGT),XtWindow (DrawingAreaWGT),Gc,0,yAxisHeight,width,yAxisHeight);
	width -= yAxisWidth;
	if (series->Type () == UIDataXYSeries)
		{
		DBCoordinate coord;
		for (stepNum = 0;stepNum <= (DBInt) ((XYRangeVAR.UpperRight.X - XYRangeVAR.LowerLeft.X) / XYStepVAR.X); ++stepNum)
			{
			x = yAxisWidth + (DBInt) ((stepNum * XYStepVAR.X) * (width * 100 / ViewPercentVAR) / (XYRangeVAR.UpperRight.X - XYRangeVAR.LowerLeft.X)) - 1;
			x -= width  * ViewOffsetVAR / ViewPercentVAR;
			XDrawLine (XtDisplay (DrawingAreaWGT),XtWindow (DrawingAreaWGT),Gc,x,yAxisHeight,x,yAxisHeight + 5);
			sprintf (numberStr,XFormatSTR,stepNum * XYStepVAR.X + XYRangeVAR.LowerLeft.X);
			string = XmStringCreate (numberStr,UICharSetNormal);
			XmStringExtent (UISmallFontList (),string,&strWidth,&strHeight);
			XmStringDraw (XtDisplay (DrawingAreaWGT),XtWindow (DrawingAreaWGT), UISmallFontList (),
					      string,Gc,x - strWidth,yAxisHeight + 5,strWidth,XmALIGNMENT_END,XmSTRING_DIRECTION_L_TO_R,NULL);
			XmStringFree (string);
			}
		for (series = First ();series != (UIDataSeries *) NULL;series = Next ())
			{
			xgcv.foreground = UIColor (UIColorStandard,series->Symbol ());
			XChangeGC (XtDisplay (DrawingAreaWGT),Gc,GCForeground,&xgcv);
			coord = series->XYData (0);
			x0 = yAxisWidth  + (DBInt) ((coord.X - XYRangeVAR.LowerLeft.X) * (width  * 100 / ViewPercentVAR) / (XYRangeVAR.UpperRight.X - XYRangeVAR.LowerLeft.X)) - 1;
			x0 -= width * ViewOffsetVAR / ViewPercentVAR;
			y0 = yAxisHeight - (DBInt) ((coord.Y - XYRangeVAR.LowerLeft.Y) * yAxisHeight / (XYRangeVAR.UpperRight.Y - XYRangeVAR.LowerLeft.Y));
			for (i = 1;i < series->ObsNum ();++i)
				{
				coord = series->XYData (i);
				if (CMmathEqualValues (coord.Y,series->NoData ())) continue;
				x1 = yAxisWidth  + (DBInt) ((coord.X - XYRangeVAR.LowerLeft.X) * (width * 100 / ViewPercentVAR) / (XYRangeVAR.UpperRight.X - XYRangeVAR.LowerLeft.X)) - 1;
				x1 -= width * ViewOffsetVAR / ViewPercentVAR;
				y1 = yAxisHeight - (DBInt) ((coord.Y - XYRangeVAR.LowerLeft.Y) * yAxisHeight / (XYRangeVAR.UpperRight.Y - XYRangeVAR.LowerLeft.Y));
				XDrawLine (XtDisplay (DrawingAreaWGT),XtWindow (DrawingAreaWGT),Gc,x0,y0,x1,y1);
				x0 = x1;
				y0 = y1;
				}
			}
		}
	else
		{
		DBDate date = ObsRangeVAR.Min.Date;
		DBObservation obs;
		for (date = ObsRangeVAR.Min.Date;date.MinutesAD () <= ObsRangeVAR.Max.Date.MinutesAD ();date = date + ObsStepVAR.Date)
			{
			x = yAxisWidth + (DBInt) ((width * 100 / ViewPercentVAR) * ((DBFloat) (date.MinutesAD () - ObsRangeVAR.Min.Date.MinutesAD ())) /
				(ObsRangeVAR.Max.Date.MinutesAD () - ObsRangeVAR.Min.Date.MinutesAD ())) - 1;
			x -= width * ViewOffsetVAR / ViewPercentVAR;
			XDrawLine (XtDisplay (DrawingAreaWGT),XtWindow (DrawingAreaWGT),Gc,x,yAxisHeight,x,yAxisHeight + 5);
			string = XmStringCreate (date.Get (),UICharSetNormal);
			XmStringExtent (UISmallFontList (),string,&strWidth,&strHeight);
			XmStringDraw (XtDisplay (DrawingAreaWGT),XtWindow (DrawingAreaWGT), UISmallFontList (),
					      string,Gc,x - strWidth,yAxisHeight + 5,strWidth,XmALIGNMENT_END,XmSTRING_DIRECTION_L_TO_R,NULL);
			XmStringFree (string);
			}
		for (series = First ();series != (UIDataSeries *) NULL;series = Next ())
			{
			xgcv.foreground = UIColor (UIColorStandard,series->Symbol ());
			XChangeGC (XtDisplay (DrawingAreaWGT),Gc,GCForeground,&xgcv);
			obs = series->ObsData (0);
			x0 = (DBInt) ((width * 100.0 / ViewPercentVAR) * ((DBFloat) (obs.Date.MinutesAD () - ObsRangeVAR.Min.Date.MinutesAD ())) / (ObsRangeVAR.Max.Date.MinutesAD () - ObsRangeVAR.Min.Date.MinutesAD ()));
			x0 -= (DBInt) (width * ((double) ViewOffsetVAR / (double) ViewPercentVAR));
			x0 += yAxisWidth;
			y0 = yAxisHeight - (DBInt) ((obs.Value - ObsRangeVAR.Min.Value) * yAxisHeight / (ObsRangeVAR.Max.Value - ObsRangeVAR.Min.Value));
			for (i = 1;i < series->ObsNum ();++i)
				{
				obs = series->ObsData (i);
				if (CMmathEqualValues (obs.Value,series->NoData ())) continue;
				x1 = (DBInt) ((width * 100 / ViewPercentVAR) * ((DBFloat) (obs.Date.MinutesAD () - ObsRangeVAR.Min.Date.MinutesAD ())) / (ObsRangeVAR.Max.Date.MinutesAD () - ObsRangeVAR.Min.Date.MinutesAD ())) - 1;
				x1 -= width * ViewOffsetVAR / ViewPercentVAR;
				x1 += yAxisWidth;
				y1 = yAxisHeight - (DBInt) ((obs.Value - ObsRangeVAR.Min.Value) * yAxisHeight / (ObsRangeVAR.Max.Value - ObsRangeVAR.Min.Value));
				XDrawLine (XtDisplay (DrawingAreaWGT),XtWindow (DrawingAreaWGT),Gc,x0,y0,x1,y1);
				x0 = x1;
				y0 = y1;
				}
			}
		}
	}

void UIXYGraph::DrawYAxis (int clear)

	{
	char numberStr [DBStringLength];
	DBInt stepNum, y;
	Dimension width, height, strWidth, strHeight;
	XGCValues xgcv;
	XmString string;
	UIDataSeries *series = First ();
	
	if (series == (UIDataSeries *) NULL)	return;
	if (XtIsRealized (YAxisWGT) == False)	return;
	if (clear) XClearWindow (XtDisplay (YAxisWGT), XtWindow (YAxisWGT));
	XtVaGetValues (YAxisWGT,XmNwidth,	&width, XmNheight,	&height, NULL);

	xgcv.foreground = UIColor (UIColorStandard,1);
	XChangeGC (XtDisplay (YAxisWGT),Gc,GCForeground,&xgcv);
	XDrawLine (XtDisplay (YAxisWGT),XtWindow (YAxisWGT),Gc,width - 1,0,width - 1,height - 1);
	if (series->Type () == UIDataXYSeries)
		for (stepNum = 0;stepNum < (DBInt) ((XYRangeVAR.UpperRight.Y - XYRangeVAR.LowerLeft.Y) / XYStepVAR.Y); ++stepNum)
			{
			y = height - (DBInt) ((stepNum * XYStepVAR.Y) * height / (XYRangeVAR.UpperRight.Y - XYRangeVAR.LowerLeft.Y));
			XDrawLine (XtDisplay (YAxisWGT),XtWindow (YAxisWGT),Gc,width - 5,y,width - 1,y);
			sprintf (numberStr,YFormatSTR,stepNum * XYStepVAR.Y + XYRangeVAR.LowerLeft.Y);
			string = XmStringCreate (numberStr,UICharSetNormal);
			XmStringExtent (UISmallFontList (),string,&strWidth,&strHeight);
			XmStringDraw (XtDisplay (YAxisWGT),XtWindow (YAxisWGT), UISmallFontList (),
					      string,Gc,width - strWidth - 5, y - strHeight, strWidth,XmALIGNMENT_END,XmSTRING_DIRECTION_L_TO_R,NULL);
			XmStringFree (string);
			}
	else
		for (stepNum = 0;stepNum < (DBInt) ((ObsRangeVAR.Max.Value - ObsRangeVAR.Min.Value) / ObsStepVAR.Value); ++stepNum)
			{
			y = height - (DBInt) ((stepNum * ObsStepVAR.Value) * height / (ObsRangeVAR.Max.Value - ObsRangeVAR.Min.Value));
			XDrawLine (XtDisplay (YAxisWGT),XtWindow (YAxisWGT),Gc,width - 5,y,width - 1,y);
			sprintf (numberStr,YFormatSTR,stepNum * ObsStepVAR.Value + ObsRangeVAR.Min.Value);
			string = XmStringCreate (numberStr,UICharSetNormal);
			XmStringExtent (UISmallFontList (),string,&strWidth,&strHeight);
			XmStringDraw (XtDisplay (YAxisWGT),XtWindow (YAxisWGT), UISmallFontList (),
					      string,Gc,width - strWidth - 5, y - strHeight, strWidth,XmALIGNMENT_END,XmSTRING_DIRECTION_L_TO_R,NULL);
			XmStringFree (string);
			}
		
	}

void UIXYGraph::Initialize (Widget parent,char *title,Arg wargs [],int argNum,XtCallbackProc callbackProc,XtPointer callData)
	
	{
	Widget scrolledWindow, workArea;
	XmString string;
	XGCValues xgcv;

	MainWGT = XtCreateManagedWidget ("UIXYGraphMainForm",xmFormWidgetClass,parent,wargs,argNum);
	XtVaSetValues (MainWGT,XmNshadowThickness,	0,	NULL);

	string = XmStringCreate (title,UICharSetBold);
	LabelWGT = XtVaCreateManagedWidget ("UIXYGraphNameLabel",xmLabelWidgetClass,MainWGT,
								XmNtopAttachment,				XmATTACH_FORM,
								XmNtopOffset,					2,
								XmNleftAttachment,			XmATTACH_FORM,
								XmNleftOffset,					2,
								XmNrightAttachment,			XmATTACH_FORM,
								XmNrightOffset,				2,
								XmNlabelString,				string,
								NULL);
	XmStringFree (string);

	ScaleWGT =  XtVaCreateManagedWidget ("UIXYGraphScale",xmScaleWidgetClass,MainWGT,
								XmNtopAttachment,				XmATTACH_WIDGET,
								XmNtopWidget,					LabelWGT,
								XmNtopOffset,					2,
								XmNleftAttachment,			XmATTACH_FORM,
								XmNleftOffset,					2,
								XmNbottomAttachment,			XmATTACH_FORM,
								XmNbottomOffset,				20,
								XmNorientation,				XmVERTICAL,
								XmNmaximum, 					100,
								XmNminimum,						10,
								XmNvalue,   					100,
								XmNwidth,						16,
								NULL);
	XtAddCallback (ScaleWGT,XmNdragCallback,			(XtCallbackProc) _UIXYGraphScaleChangedCBK,(XtPointer) this);
	XtAddCallback (ScaleWGT,XmNvalueChangedCallback,(XtCallbackProc) _UIXYGraphScaleChangedCBK,(XtPointer) this);
	scrolledWindow  = XtVaCreateManagedWidget ("UIXYGraphScrolledWindow",xmScrolledWindowWidgetClass,MainWGT,
								XmNtopAttachment,				XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,					ScaleWGT,
								XmNtopOffset,					0,
								XmNleftAttachment,			XmATTACH_WIDGET,
								XmNleftWidget,					ScaleWGT,
								XmNleftOffset,					4,
								XmNrightAttachment,			XmATTACH_FORM,
								XmNrightOffset,				4,
								XmNbottomAttachment,			XmATTACH_FORM,
								XmNbottomOffset,				4,
								XmNorientation,				XmHORIZONTAL,
								XmNscrollBarDisplayPolicy,	XmSTATIC,
								XmNshadowThickness,			0,
								NULL);
	workArea =  XtVaCreateManagedWidget ("UIXYGraphDrawingArea",xmFormWidgetClass,scrolledWindow,
								XmNshadowThickness,			0,
								NULL);
	ScrollBarWGT = XtVaCreateManagedWidget ("UIXYGraphScrollBar",xmScrollBarWidgetClass,scrolledWindow,
								XmNorientation, 				XmHORIZONTAL,
								XmNsliderSize, 				100,
								NULL);
	XtAddCallback (ScrollBarWGT,XmNvalueChangedCallback,(XtCallbackProc) _UIXYGraphScrollBarChangedCBK,(XtPointer) this);
	XtAddCallback (ScrollBarWGT,XmNdragCallback,(XtCallbackProc) _UIXYGraphScrollBarChangedCBK,(XtPointer) this);
	XmScrolledWindowSetAreas (scrolledWindow,ScrollBarWGT,(Widget) NULL,workArea);
	YAxisWGT = XtVaCreateManagedWidget ("UIXYGraphYAxis",xmDrawingAreaWidgetClass,workArea,
								XmNtopAttachment,				XmATTACH_FORM,
								XmNleftAttachment,			XmATTACH_FORM,
								XmNbottomAttachment,			XmATTACH_FORM,
								XmNbottomOffset,				20,
								XmNbackground,					UIColor (UIColorStandard,0),
								XmNwidth,						60,
								NULL);
	DrawingAreaWGT = XtVaCreateManagedWidget ("UIXYGraphDrawingArea",xmDrawingAreaWidgetClass,workArea,
								XmNtopAttachment,				XmATTACH_FORM,
								XmNleftAttachment,			XmATTACH_FORM,
								XmNrightAttachment,			XmATTACH_FORM,
								XmNbottomAttachment,			XmATTACH_FORM,
								XmNbackground,					UIColor (UIColorStandard,0),
								NULL);
	xgcv.foreground = UIColor (UIColorStandard,1);
	Gc = XtGetGC (DrawingAreaWGT,GCForeground,&xgcv);
	SetViewPercent (100);
	XtAddCallback (DrawingAreaWGT,XmNexposeCallback,(XtCallbackProc) _UIXYGraphDrawingExposeCBK,(XtPointer) this);
	XtAddCallback (DrawingAreaWGT,XmNresizeCallback,(XtCallbackProc) _UIXYGraphDrawingResizeCBK,(XtPointer) YAxisWGT);
	XtAddCallback (DrawingAreaWGT,XmNexposeCallback,(XtCallbackProc) _UIXYyAxisExposeCBK,(XtPointer) this);
	}
 
UIXYGraph::UIXYGraph () : DBObjectLIST<UIDataSeries> ("UIXYGraph",sizeof (UIXYGraph))
	{ MainWGT = ScrollBarWGT = ScaleWGT = DrawingAreaWGT = (Widget) NULL; }

void UIXYGraph::Add (UIDataSeries *series)

	{
	UIDataSeries *firstSeries = First ();
	
	if ((firstSeries != (UIDataSeries *) NULL) && (firstSeries->Type () != series->Type ()))
		{ fprintf (stderr,"Invalid Data Series in: UIXYGraph::AddSeries ()\n"); return; }
	DBObjectLIST<UIDataSeries>::Add (series);
	SetRange ();
	}

void UIXYGraph::Remove (UIDataSeries *series)

	{
	DBObjectLIST<UIDataSeries>::Remove (series);
	SetRange ();
	}
