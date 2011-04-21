/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

UI2DViewDisp.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/ToggleB.h>
#include <UI.H>

int UI2DView::DrawMesh ()

	{
	int power;
	Dimension width, maxWidth, height;
	DBFloat mx, my, mesh = 1.0;
	short wx,wy;
	char string [DBStringLength], formatStr [DBStringLength];
	DBCoordinate trans, viewLL = ViewEXT.LowerLeft, viewUR = ViewEXT.UpperRight;
	GC gc;
	XGCValues xgcv;
	XmFontContext fonContext;
	XmStringCharSet charSet;
	XFontStruct *font;
	XmString xmString;

	if (!XmFontListInitFontContext (&fonContext,UISmallFontList()))	return (DBFault);
	if (!XmFontListGetNextFont (fonContext,&charSet,&font))           return (DBFault);
	XmFontListFreeFontContext (fonContext);

	xgcv.foreground = UIColor (UIColorStandard,1);
	xgcv.background = UIColor (UIColorStandard,0);
	xgcv.font = font->fid;
	gc = XtGetGC (DrawingAreaW,GCForeground | GCBackground | GCFont,&xgcv);

	mesh =  viewUR.X - viewLL.X > viewUR.Y - viewLL.Y ?
			  viewUR.X - viewLL.X : viewUR.Y - viewLL.Y;

	if (mesh >= 1.0)	power = (DBInt) ceil  (log10 (mesh)) - 1;
	else					power = (DBInt) floor (log10 (mesh)) + 1;

	if (mesh / pow ((double) 10.0,(double) power) >= (double) 5.0) mesh = pow ((double) 10.0,(double) power);
	else	mesh = pow ((double) 10.0,(double) power) / 2.0;

	strcpy (formatStr,"%.0lf");
	if (power > 4) power = 4;
	formatStr [3] = '0' + 4 - power;

	trans.X = (int) (viewLL.X / mesh) * mesh;
	trans.Y = (int) (viewLL.Y / mesh) * mesh;

	my = viewLL.Y;
	for (mx = trans.X; mx < viewUR.X; mx += mesh)
		{
		Map2Window (mx,my,&wx,&wy);
		XDrawLine (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),gc,wx,0,wx,Height ());
		}
	for (mx = trans.X - mesh / 2;mx < viewUR.X; mx += mesh)
		{
		Map2Window (mx,my,&wx,&wy);
		XDrawLine (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),gc,wx,Height (),wx,Height () - 2);
		XDrawLine (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),gc,wx,0,wx,2);
		}

	mx = viewUR.X;
	for (my = trans.Y;my < viewUR.Y; my += mesh)
		{
		Map2Window (mx,my,&wx,&wy);
		XDrawLine (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),gc,0,wy,Width (),wy);
		}
	for (my = trans.Y - mesh / 2;my < viewUR.Y; my += mesh)
		{
		Map2Window (mx,my,&wx,&wy);
		XDrawLine (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),gc,0,wy,2,wy);
		XDrawLine (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),gc,Width (),wy,Width () - 2,wy);
		}

	my = viewLL.Y;
	maxWidth = 0;
	for (mx = trans.X;mx < viewUR.X; mx += mesh)
		{
		sprintf (string,formatStr,mx);
		xmString = XmStringCreate (string,UICharSetNormal);
		XmStringExtent (UISmallFontList (),xmString,&width,&height);
		XmStringFree (xmString);
		if (width > maxWidth) maxWidth = width;
		}

	for (mx = trans.X;mx < viewUR.X; mx += mesh)
		{
		Map2Window (mx,my,&wx,&wy);
		sprintf (string,formatStr,mx);
		xmString = XmStringCreate (string,UICharSetNormal);
		XmStringExtent (UISmallFontList (),xmString,&width,&height);
		XmStringDraw (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),UISmallFontList (),xmString,
						  gc, wx - maxWidth,Height () - height, maxWidth, XmALIGNMENT_END, XmSTRING_DIRECTION_L_TO_R, NULL);
		XmStringFree (xmString);
		}
	mx = viewUR.X;
	for (my = trans.Y;my < viewUR.Y; my += mesh)
		{
		Map2Window (mx,my,&wx,&wy);
		sprintf (string,formatStr,my);
		xmString = XmStringCreate (string,UICharSetNormal);
		XmStringExtent (UISmallFontList (),xmString,&width,&height);
		XmStringDraw (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),UISmallFontList (),xmString,
						  gc,5,wy - height,maxWidth,XmALIGNMENT_CENTER,XmSTRING_DIRECTION_L_TO_R,NULL);
		XmStringFree (xmString);
		}

	XtReleaseGC (DrawingAreaW,gc);
	return (DBSuccess);
	}

void UI2DView::DrawPointObject (DBVPointIF *pntIF,DBObjRecord *record,GC gc)

	{
	short sX,sY;
	DBInt foreground, style;
	DBCoordinate coord;
	Pixmap pixmap;

	if (ViewEXT.InRegion (coord = pntIF->Coordinate  (record)) == false) return;

	foreground = (record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected ?
					 pntIF->ItemBackground (record) : pntIF->ItemForeground (record);
	style = pntIF->ItemStyle (record);
	pixmap = UIMarker (style,UIColor (UIColorStandard,foreground),UIColor (UIColorStandard,0));
	Map2Window (coord.X,coord.Y,&sX,&sY);
	XCopyArea (XtDisplay (DrawingAreaW),pixmap,XtWindow (DrawingAreaW),gc,0,0,11,11,sX - 5,sY -5);
	XFreePixmap (XtDisplay (DrawingAreaW),pixmap);
	}

void UI2DView::DrawPoints (DBObjData *data,GC gc)

	{
	DBVPointIF *pntIF;
	DBObjRecord *record;

	if (ActiveEXT.InRegion (data->Extent ()) == false) return;
	pntIF = new DBVPointIF (data);
	for (record = pntIF->FirstItem ();record != (DBObjRecord *) NULL;record = pntIF->NextItem ())
		DrawPointObject (pntIF,record,gc);
	delete pntIF;
	}

void UI2DView::DrawLineObject (DBVLineIF *lineIF, DBObjRecord *record,GC gc)

	{
	DBInt vertex, lWidth, lStyle;
	DBCoordinate *coord, nodeCoord;
	XGCValues xgcv;

	if (ViewEXT.InRegion (lineIF->Extent (record)) == false) return;
	switch (lineIF->ItemStyle (record) >> 0x02)
		{
		case 0:	lStyle = LineSolid;			break;
		case 1:	lStyle = LineOnOffDash;		break;
		case 2:	lStyle = LineDoubleDash;	break;
		}
	lWidth = lineIF->ItemStyle (record) & 0x03;
	if ((record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
		{
		xgcv.foreground = UIColor (UIColorStandard,lineIF->ItemBackground (record));
		xgcv.background = UIColor (UIColorStandard,lineIF->ItemForeground (record));
		}
	else
		{
		xgcv.foreground = UIColor (UIColorStandard,lineIF->ItemForeground (record));
		xgcv.background = UIColor (UIColorStandard,lineIF->ItemBackground (record));
		}
	xgcv.line_width = lWidth;
	xgcv.line_style = lStyle;
	XChangeGC (XtDisplay (DrawingAreaW),gc,GCForeground | GCBackground | GCLineWidth | GCLineStyle,&xgcv);

	if (MaxVertexNumVAR < lineIF->VertexNum (record) + 2)
		{
		PointARR = PointARR = (XPoint *) realloc (PointARR,(lineIF->VertexNum (record) + 2) * sizeof (XPoint));
		if (PointARR == NULL) { CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__); return; }
		MaxVertexNumVAR =  lineIF->VertexNum (record) + 2;
		}
	nodeCoord = lineIF->FromCoord (record);
	Map2Window (nodeCoord.X,nodeCoord.Y,&PointARR [0].x,&PointARR [0].y);
	if (lineIF->VertexNum (record) > 0)
		{
		coord = lineIF->Vertexes (record);
		for (vertex = 0;vertex < lineIF->VertexNum (record);++vertex)
			Map2Window (coord [vertex].X,coord [vertex].Y,&PointARR [vertex + 1].x,&PointARR [vertex + 1].y);
		}
	else vertex = 0;
	nodeCoord = lineIF->ToCoord (record);
	Map2Window (nodeCoord.X,nodeCoord.Y,&PointARR [vertex + 1].x,&PointARR [vertex + 1].y);
	XDrawLines (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),gc,PointARR,vertex + 2,CoordModeOrigin);
	}

void UI2DView::DrawLines (DBObjData *data,GC gc)

	{
	DBVLineIF *lineIF;
	DBObjRecord *record;

	if (ActiveEXT.InRegion (data->Extent ()) == false) return;
	lineIF = new DBVLineIF (data);

	for (record = lineIF->FirstItem ();record != (DBObjRecord *) NULL;record = lineIF->NextItem ())
		DrawLineObject (lineIF,record,gc);
	delete lineIF;
	}

void UI2DView::DrawPolyObject (DBVPolyIF *polyIF, DBObjRecord *record,GC gc)

	{
	DBInt vertex, i;
	DBCoordinate *coords;
	XGCValues xgcv;

	if (ViewEXT.InRegion (polyIF->Extent (record)) == false) return;
	xgcv.foreground = UIColor (UIColorStandard,polyIF->ItemForeground (record));
	xgcv.background = UIColor (UIColorStandard,polyIF->ItemBackground (record));
	XChangeGC (XtDisplay (DrawingAreaW),gc,GCForeground | GCBackground,&xgcv);
	if (polyIF->VertexNum (record) > MaxVertexNumVAR)
		{
		PointARR = (XPoint *) realloc (PointARR,polyIF->VertexNum (record) * sizeof (XPoint));
		if (PointARR == (XPoint *) NULL)
			{ CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__); return; }
		MaxVertexNumVAR = polyIF->VertexNum (record);
		}
	if ((coords = polyIF->Vertexes (record)) == (DBCoordinate *) NULL) return;
	i = 0;
	for (vertex = 0;vertex < polyIF->VertexNum (record);++vertex)
		{
		Map2Window (coords [vertex].X,coords [vertex].Y,&PointARR [i].x,&PointARR [i].y);
		if ((i == 0) || (PointARR [i].x != PointARR [i - 1].x) || (PointARR [i].y != PointARR [i - 1].y)) i++;
		}
	XFillPolygon (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),gc,PointARR,i,Complex,CoordModeOrigin);
	}

void UI2DView::DrawPolygons (DBObjData *data,GC gc)

	{
	DBObjRecord *record;
	DBVPolyIF *polyIF;

	if (ActiveEXT.InRegion (data->Extent ()) == false) return;
	polyIF = new DBVPolyIF (data);

	for (record = polyIF->FirstItem ();record != (DBObjRecord *) NULL;record = polyIF->NextItem ())
		DrawPolyObject (polyIF,record,gc);
	delete polyIF;
	}

void UI2DView::DrawVectorAnnotation (DBVectorIF *vectorIF, DBObjRecord *record,GC gc)

	{
	DBInt flags;
	DBShort sX, sY, offsetX, offsetY;
	Dimension width, height;
	DBCoordinate coord;
	DBObjData *dbData = vectorIF->Data ();
	DBRegion extent;
	XmString xmString;

	extent = dbData->Extent (record);
	if (ViewEXT.InRegion (extent) == false) return;
	if ((dbData->Flags () & DBDataFlagDispModeVecAnnotSelect) == DBDataFlagDispModeVecAnnotSelect) { flags = record->Flags ();record->Flags (DBObjectFlagSelected,true); }
	if ((record->Flags () & DBObjectFlagSelected) != DBObjectFlagSelected) return;
	coord.X = (extent.UpperRight.X + extent.LowerLeft.X) / 2.0;
	coord.Y = (extent.UpperRight.Y + extent.LowerLeft.Y) / 2.0;
	Map2Window (coord.X,coord.Y,&sX,&sY);
	xmString = XmStringCreate (record->Name (),UICharSetNormal);
	XmStringExtent (UINormalFontList (),xmString,&width,&height);
	if (dbData->Type () == DBTypeVectorPoint)
			{ offsetX = 0; offsetY = height; }
	else	{ offsetX = width / 2; offsetY = 0; }
	XmStringDraw   (XtDisplay (DrawingAreaW), XtWindow (DrawingAreaW), UINormalFontList (), xmString,
			gc,sX - offsetX,sY - offsetY,width, XmALIGNMENT_CENTER,XmSTRING_DIRECTION_L_TO_R,NULL);
	XmStringFree (xmString);
	if ((dbData->Flags () & DBDataFlagDispModeVecAnnotSelect) == DBDataFlagDispModeVecAnnotSelect) record->Flags (DBObjectFlagSelected,(flags & DBObjectFlagSelected) == DBObjectFlagSelected);
	}

void UI2DView::DrawVectorAnnotations (DBObjData *data,GC gc)

	{
	DBObjRecord *record;
	DBVectorIF *vectorIF;

	if (ActiveEXT.InRegion (data->Extent ()) == false) return;
	vectorIF = new DBVectorIF (data);

	for (record = vectorIF->FirstItem ();record != (DBObjRecord *) NULL;record = vectorIF->NextItem ())
		DrawVectorAnnotation (vectorIF,record,gc);
	delete vectorIF;
	}

static DBInt _UI2DViewContPosShades;
static DBInt _UI2DViewContNegShades;
static DBFloat _UI2DViewContMaxValue;
static DBFloat _UI2DViewContMinValue;

static DBInt _UI2DViewContinuousSingleShadeFunc (DBGridIF *gridIF,DBCoordinate coord)

	{
	DBPosition pos;
	DBInt retPixel;
	DBFloat value;

	if (gridIF->Coord2Pos (coord,pos) == DBFault) return (DBFault);
	if (gridIF->Value (pos,&value) == false) return (DBFault);
	retPixel = (DBInt) ((DBFloat) (UIColorNum (_UI2DViewContPosShades)) * (value - _UI2DViewContMinValue) / (_UI2DViewContMaxValue - _UI2DViewContMinValue));
	if (retPixel == UIColorNum (_UI2DViewContPosShades)) retPixel -= 1;
	return (UIColor (_UI2DViewContPosShades,retPixel));
	}

static DBInt _UI2DViewContinuousDualShadeFunc (DBGridIF *gridIF,DBCoordinate coord)

	{
	DBPosition pos;
	DBInt retPixel;
	DBFloat value;

	if (gridIF->Coord2Pos (coord,pos) == DBFault) return (DBFault);
	if (gridIF->Value (pos,&value) == false) return (DBFault);
	if (value >= 0.0)
		{
		retPixel = _UI2DViewContMinValue < 0.0 ?
					(DBInt) ((DBFloat) (UIColorNum (_UI2DViewContPosShades)) * value / _UI2DViewContMaxValue) :
					(DBInt) ((DBFloat) (UIColorNum (_UI2DViewContPosShades)) * (value - _UI2DViewContMinValue) / (_UI2DViewContMaxValue - _UI2DViewContMinValue));
		if (retPixel >= UIColorNum (_UI2DViewContPosShades)) retPixel -= 1;
		return (UIColor (_UI2DViewContPosShades,retPixel));
		}
	else
		{
		retPixel = _UI2DViewContMaxValue > 0.0 ?
					(DBInt) floor ((DBFloat) (UIColorNum (_UI2DViewContNegShades)) * (0.0 - value) / (0.0 - _UI2DViewContMinValue)) :
					(DBInt) floor ((DBFloat) (UIColorNum (_UI2DViewContNegShades)) * (_UI2DViewContMaxValue - value) / (_UI2DViewContMaxValue - _UI2DViewContMinValue));
		if (retPixel >= UIColorNum (_UI2DViewContNegShades)) retPixel -= 1;
		return (UIColor (_UI2DViewContNegShades,retPixel));
		}
	}

static DBInt _UI2DViewDiscreteShadeFunc (DBGridIF *gridIF,DBCoordinate coord)

	{
	DBInt symbol = gridIF->SymbolFGColor (coord);

	if (symbol == DBFault) return (DBFault);
	if (symbol == gridIF->SymbolBGColor (coord)) return (DBFault);
	return (UIColor (UIColorStandard,symbol));
	}

void UI2DView::DrawGrid (DBObjData *data,GC gc)

	{
	DBInt pixel;
	XPoint winPos, startPos, endPos;
	DBPosition pos;
	DBCoordinate coord, centerCoord;
	DBFloat mapX, mapY;
	DBGridIF *gridIF;
	DBInt (*pixelFunc) (DBGridIF *, DBCoordinate);

	if (ActiveEXT.InRegion (data->Extent ()) == false) return;
	gridIF = new DBGridIF (data);
	switch (data->Type ())
		{
		case DBTypeGridContinuous:
			switch (data->Flags () & DBDataFlagDispModeContShadeSets)
				{
				case DBDataFlagDispModeContStandard:
					_UI2DViewContPosShades = UIColorStandard;
					pixelFunc = _UI2DViewContinuousSingleShadeFunc;
					break;
				case DBDataFlagDispModeContGreyScale:
					_UI2DViewContPosShades = UIColorGreyScale;
					pixelFunc = _UI2DViewContinuousSingleShadeFunc;
					break;
				case DBDataFlagDispModeContBlueScale:
					_UI2DViewContPosShades = UIColorBlueScale;
					pixelFunc = _UI2DViewContinuousSingleShadeFunc;
					break;
				case DBDataFlagDispModeContBlueRed:
					_UI2DViewContPosShades = UIColorRedScale;
					_UI2DViewContNegShades = UIColorBlueScale;
					pixelFunc = _UI2DViewContinuousDualShadeFunc;
					break;
				case DBDataFlagDispModeContElevation:
					_UI2DViewContPosShades = UIColorElevation;
					_UI2DViewContNegShades = UIColorBlueScale;
					pixelFunc = _UI2DViewContinuousDualShadeFunc;
					break;
				}
			break;
		case DBTypeGridDiscrete:
				pixelFunc = _UI2DViewDiscreteShadeFunc;
			break;
		}

	if (((ViewEXT.UpperRight.X - ViewEXT.LowerLeft.X) / gridIF->CellWidth ())  > (Image->width >> 0x01) ||
		 ((ViewEXT.UpperRight.Y - ViewEXT.LowerLeft.Y) / gridIF->CellHeight ()) > (Image->height >> 0x01))
		{
		startPos.x = startPos.y = 0; endPos.x = Image->width; endPos.y = Image->height;
		Map2Window (ViewEXT.LowerLeft.X,ViewEXT.UpperRight.Y,&winPos.x,&winPos.y);
		startPos.x = winPos.x > startPos.x ? winPos.x : startPos.x;
		startPos.y = winPos.y > startPos.y ? winPos.y : startPos.y;
		Map2Window (ViewEXT.UpperRight.X,ViewEXT.LowerLeft.Y,&winPos.x,&winPos.y);
		endPos.x = winPos.x < endPos.x ? winPos.x : endPos.x;
		endPos.y = winPos.y < endPos.y ? winPos.y : endPos.y;

		if (data->Type () == DBTypeGridContinuous)
			{
			_UI2DViewContMaxValue = gridIF->Maximum ();
			_UI2DViewContMinValue = gridIF->Minimum ();
			}

		for (winPos.y = startPos.y;winPos.y < endPos.y;++winPos.y)
			for (winPos.x = startPos.x;winPos.x < endPos.x;++winPos.x)
				{
				Window2Map (winPos.x,winPos.y,&mapX, &mapY);
				coord.X = mapX; coord.Y = mapY;
				if ((pixel = (*pixelFunc) (gridIF,coord)) != DBFault)
					XPutPixel (Image, winPos.x, winPos.y, pixel);
				}
		XPutImage (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW), gc, Image,0,0,0,0, Image->width, Image->height);
		}
	else
		{
		XPoint cellBox;
		DBCoordinate startCoord, endCoord;
		DBCoordinate dataExtentLL = data->Extent ().LowerLeft;
		DBCoordinate dataExtentUR = data->Extent ().UpperRight;

		Map2Window (ViewEXT.LowerLeft.X,ViewEXT.LowerLeft.Y + gridIF->CellHeight (),&startPos.x,&startPos.y);
		Map2Window (ViewEXT.LowerLeft.X + gridIF->CellWidth (), ViewEXT.LowerLeft.Y,&endPos.x,&endPos.y);
		XGCValues xgcv;

		cellBox.x = endPos.x - startPos.x;
		cellBox.y = endPos.y - startPos.y;
		startCoord.X = dataExtentLL.X > ViewEXT.LowerLeft.X ? dataExtentLL.X : dataExtentLL.X
						 + floor ((ViewEXT.LowerLeft.X - dataExtentLL.X) / gridIF->CellWidth  ()) * gridIF->CellWidth  ();
		startCoord.Y = dataExtentLL.Y > ViewEXT.LowerLeft.Y ? dataExtentLL.Y : dataExtentLL.Y
						 + floor ((ViewEXT.LowerLeft.Y - dataExtentLL.Y) / gridIF->CellHeight ()) * gridIF->CellHeight ();
		endCoord.X = dataExtentUR.X < ViewEXT.UpperRight.X ? dataExtentUR.X: ViewEXT.UpperRight.X;
		endCoord.Y = dataExtentUR.Y < ViewEXT.UpperRight.Y ? dataExtentUR.Y : ViewEXT.UpperRight.Y;

		if (DBTypeGridContinuous == data->Type ())
				{
				DBFloat value;
				_UI2DViewContMaxValue = -DBHugeVal;
				_UI2DViewContMinValue =  DBHugeVal;
				for (coord.Y = startCoord.Y;coord.Y < endCoord.Y;coord.Y = coord.Y + gridIF->CellHeight ())
					for (coord.X = startCoord.X;coord.X < endCoord.X;coord.X = coord.X + gridIF->CellWidth ())
						{
						if (gridIF->Coord2Pos (coord,pos) == DBFault) continue;
						if (gridIF->Value (pos,&value))
							{
							_UI2DViewContMaxValue = _UI2DViewContMaxValue > value ? _UI2DViewContMaxValue : value;
							_UI2DViewContMinValue = _UI2DViewContMinValue < value ? _UI2DViewContMinValue : value;
							}
						}
				}

		for (coord.Y = startCoord.Y;coord.Y < endCoord.Y;coord.Y = coord.Y + gridIF->CellHeight ())
			for (coord.X = startCoord.X;coord.X < endCoord.X;coord.X = coord.X + gridIF->CellWidth ())
				{
				centerCoord.X = coord.X + gridIF->CellWidth ()  / 2.0;
				centerCoord.Y = coord.Y + gridIF->CellHeight () / 2.0;
				if ((pixel = (*pixelFunc) (gridIF,centerCoord)) == DBFault) continue;
				xgcv.foreground = pixel;
				XChangeGC (XtDisplay (DrawingAreaW),gc,GCForeground,&xgcv);
				Map2Window (coord.X,coord.Y,&winPos.x,&winPos.y);
				XDrawRectangle (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),gc,
						winPos.x,winPos.y - (int) cellBox.y,(int) cellBox.x,(int) cellBox.y);
				XFillRectangle (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),gc,
						winPos.x,winPos.y - (int) cellBox.y,(int) cellBox.x,(int) cellBox.y);
				}
		}
	delete gridIF;
	}

void UI2DView::DrawNetworkCellBox (DBNetworkIF *netIF,DBObjRecord *cellRec,GC gc)

	{
	DBCoordinate coord [2];
	XPoint box [2];

	coord [0] = netIF->Center (cellRec) - (netIF->CellSize () / 2.0);
	coord [1] = netIF->Center (cellRec) + (netIF->CellSize () / 2.0);
	Map2Window (coord [0].X,coord [0].Y,&box [0].x,&box [1].y);
	Map2Window (coord [1].X,coord [1].Y,&box [1].x,&box [0].y);
	box [1].x -= box [0].x;
	box [1].y -= box [0].y;
	XDrawRectangle (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),gc,
						(int) box [0].x,(int) box [0].y,(int) box [1].x,(int) box [1].y);
	XFillRectangle (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),gc,
						(int)box [0].x,(int) box [0].y,(int) box [1].x,(int) box [1].y);
	}

void UI2DView::DrawNetworkCell(DBNetworkIF *netIF,DBObjRecord *cellRec,DBUnsigned displayColor, DBUnsigned displayMode, GC gc)

	{
	DBInt basinID;
	DBCoordinate coord [2];
	DBShort sX0, sY0, sX1, sY1;
	XGCValues xgcv;

	coord [0] = netIF->Center (cellRec);
	coord [1] = netIF->Delta  (cellRec);
	coord [1].X = coord [0].X + coord [1].X;
	coord [1].Y = coord [0].Y + coord [1].Y;
	Map2Window (coord [0].X,coord [0].Y,&sX0,&sY0);
	Map2Window (coord [1].X,coord [1].Y,&sX1,&sY1);

	basinID = netIF->CellBasinID (cellRec);
	switch (displayColor)
		{
		default:
		case DBDataFlagDispModeNetColorUniform:
			xgcv.foreground = (cellRec->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected ?
									 UIColor (UIColorStandard,UIStandardRed) : UIColor (UIColorStandard,UIStandardBlack);
			break;
		case DBDataFlagDispModeNetColorBasin:
			xgcv.foreground = (cellRec->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected ?
									 UIColor (UIColorStandard,UIStandardRed) :
									 UIColor (UIColorStandard,basinID == DBFault ? 1 : netIF->Color (netIF->Basin (cellRec)));
			break;
		case DBDataFlagDispModeNetColorSymbol:
			xgcv.foreground = (cellRec->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected ?
									 UIColor (UIColorStandard,basinID != DBFault ? netIF->ItemBackground (netIF->Basin (cellRec)) : 1) :
					 				 UIColor (UIColorStandard,basinID != DBFault ? netIF->ItemForeground (netIF->Basin (cellRec)) : 2);
			break;
		}
	if (displayMode == DBDataFlagDispModeNetDirection)
		{
		xgcv.line_width = 1;
		XChangeGC (XtDisplay (DrawingAreaW),gc,GCForeground | GCLineWidth,&xgcv);
		XDrawLine (XtDisplay (DrawingAreaW),XtWindow  (DrawingAreaW),gc,sX0, sY0, sX1,sY1);

		sX1 = (sX1 + sX0) / 2;	sY1 = (sY1 + sY0) / 2;
		xgcv.line_width = 2;
		XChangeGC (XtDisplay (DrawingAreaW),gc,GCForeground | GCLineWidth,&xgcv);
		XDrawLine (XtDisplay (DrawingAreaW),XtWindow  (DrawingAreaW),gc,sX0, sY0, sX1,sY1);
		}
	else
		{
		xgcv.line_width = netIF->CellOrder (cellRec);
		xgcv.line_width = xgcv.line_width > 1 ? xgcv.line_width - 1 : 1;
		XChangeGC (XtDisplay (DrawingAreaW),gc,GCForeground | GCLineWidth,&xgcv);
		XDrawLine (XtDisplay (DrawingAreaW),XtWindow  (DrawingAreaW),gc,sX0, sY0, sX1,sY1);
		}
	}

void UI2DView::DrawNetwork(DBObjData *data,GC gc)

	{
	DBInt cellID;
	DBUnsigned displayColor	= data->Flags () & DBDataFlagDispModeNetColors;
	DBUnsigned displayMode	= data->Flags () & DBDataFlagDispModeNetDirection;
	DBRegion dataExtent = data->Extent ();
	DBPosition pos, ll, ur;
	DBNetworkIF *netIF;
	DBObjRecord *cellRec, *basinRec;
	XGCValues xgcv;

	if (ActiveEXT.InRegion (dataExtent) == false) return;
	netIF = new DBNetworkIF (data);

	xgcv.cap_style = CapRound;
	xgcv.background = 0;
	xgcv.line_width = 1;
	xgcv.line_style = LineSolid;
	XChangeGC (XtDisplay (DrawingAreaW),gc,GCBackground | GCLineWidth | GCLineStyle | GCCapStyle,&xgcv);

	ll.Col = ActiveEXT.LowerLeft.X < dataExtent.LowerLeft.X ? 0 :
				(DBShort) (floor ((ActiveEXT.LowerLeft.X - dataExtent.LowerLeft.X) / netIF->CellWidth ()));
	ll.Row = ActiveEXT.LowerLeft.Y < dataExtent.LowerLeft.Y ? 0 :
				(DBShort) (floor ((ActiveEXT.LowerLeft.Y - dataExtent.LowerLeft.Y) / netIF->CellHeight ()));
	ur.Col = ActiveEXT.UpperRight.X > dataExtent.UpperRight.X ? netIF->ColNum () - 1 :
				(DBShort) (ceil ((ActiveEXT.UpperRight.X - dataExtent.LowerLeft.X) / netIF->CellWidth ()));
	ur.Row = ActiveEXT.UpperRight.Y > dataExtent.UpperRight.Y ? netIF->RowNum () - 1 :
				(DBShort) (ceil ((ActiveEXT.UpperRight.Y - dataExtent.LowerLeft.Y) / netIF->CellHeight ()));

	if (((ur.Col - ll.Col) * 4 < netIF->ColNum ()) || ((ur.Row - ll.Row) * 4 < netIF->RowNum ()))
		{
		xgcv.foreground = UIColor (UIColorStandard,UIStandardYellow);
		XChangeGC (XtDisplay (DrawingAreaW),gc,GCForeground,&xgcv);
		for (pos.Row = ll.Row;pos.Row < ur.Row;++pos.Row)
			for (pos.Col = ll.Col;pos.Col < ur.Col;++pos.Col)
				{
				if ((cellRec = netIF->Cell (pos)) == (DBObjRecord *) NULL) continue;
				if (((cellRec->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected) &&
					 (netIF->Cell (netIF->CellPosition (cellRec)) != (DBObjRecord *) NULL))
					DrawNetworkCellBox (netIF,cellRec,gc);
				}
		for (pos.Row = ll.Row;pos.Row < ur.Row;++pos.Row)
			for (pos.Col = ll.Col;pos.Col < ur.Col;++pos.Col)
				{
				if ((cellRec = netIF->Cell (pos)) == (DBObjRecord *) NULL) continue;
				DrawNetworkCell(netIF,cellRec,displayColor,displayMode,gc);
				}
		}
	else
		{
		DBInt basinID = DBFault;

		xgcv.foreground = UIColor (UIColorStandard,UIStandardYellow);
		XChangeGC (XtDisplay (DrawingAreaW),gc,GCForeground,&xgcv);
		for (cellID = 0;cellID < netIF->CellNum ();++cellID)
			{
			cellRec = netIF->Cell (cellID);
			if (((cellRec->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected) &&
				 (netIF->Cell (netIF->CellPosition (cellRec)) != (DBObjRecord *) NULL))
				{
				if (basinID != netIF->CellBasinID (cellRec))
					{
					basinID = netIF->CellBasinID (cellRec);
					basinRec = netIF->Basin (cellRec);
					if ((basinRec != (DBObjRecord *) NULL) && (ActiveEXT.InRegion (data->Extent (basinRec)) == false))
						{ cellID += netIF->CellBasinCells (cellRec) - 1; continue; }
					}
				DrawNetworkCellBox (netIF,cellRec,gc);
				}
			}

		xgcv.foreground = UIColor (UIColorStandard,1);
		XChangeGC (XtDisplay (DrawingAreaW),gc,GCForeground,&xgcv);
		basinID = DBFault;
		for (cellID = 0;cellID < netIF->CellNum ();++cellID)
			{
			cellRec = netIF->Cell (cellID);
			if (netIF->Cell (netIF->CellPosition (cellRec)) == (DBObjRecord *) NULL) continue;

			if (basinID != netIF->CellBasinID (cellRec))
				{
				basinID = netIF->CellBasinID (cellRec);
				basinRec = netIF->Basin (cellRec);
				if ((basinRec != (DBObjRecord *) NULL) && (ViewEXT.InRegion (data->Extent (basinRec)) == false))
					{ cellID += netIF->CellBasinCells (cellRec) - 1; continue; }
				}
			DrawNetworkCell(netIF,cellRec,displayColor,displayMode,gc);
			}
		}
	delete netIF;
	}

void UI2DView::Draw ()

	{
	GC gc = NULL;
	XGCValues xgcv;
	XSetWindowAttributes xswa;
	XmFontContext fonContext;
	XmStringCharSet charSet;
	XFontStruct *font;
	DBDataset *dataset = UIDataset ();
	DBObjData *data;

	xswa.cursor = RegenCursor;
	XChangeWindowAttributes (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),CWCursor,&xswa);
	Clear ();

	if (!XmFontListInitFontContext (&fonContext,UISmallFontList())) return;
	if (!XmFontListGetNextFont (fonContext,&charSet,&font)) 	return;
	XmFontListFreeFontContext (fonContext);

	xgcv.foreground = UIColor (UIColorStandard,0);
	xgcv.foreground = UIColor (UIColorStandard,1);
	xgcv.font = font->fid;
	gc = XtGetGC (DrawingAreaW,GCForeground|GCBackground|GCFont,&xgcv);

	XSetRegion (XtDisplay (DrawingAreaW),gc,DrawRegion);
	if (((dataset->DataList ())->Flags () & DBDataLISTFlagSmartSort) == DBDataLISTFlagSmartSort)
		{
		for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
			if ((data->Flags () & DBDataFlagDisplay) == DBDataFlagDisplay)
				if (data->Type () == DBTypeGridContinuous)	DrawGrid (data,gc);
		for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
			if ((data->Flags () & DBDataFlagDisplay) == DBDataFlagDisplay)
				if (data->Type () == DBTypeGridDiscrete)		DrawGrid (data,gc);
		for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
			if ((data->Flags () & DBDataFlagDisplay) == DBDataFlagDisplay)
				if (data->Type () == DBTypeVectorPolygon)		DrawPolygons (data,gc);
		for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
			if ((data->Flags () & DBDataFlagDisplay) == DBDataFlagDisplay)
				if (data->Type () == DBTypeVectorLine)			DrawLines (data,gc);
		for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
			if ((data->Flags () & DBDataFlagDisplay) == DBDataFlagDisplay)
				if (data->Type () == DBTypeNetwork)				DrawNetwork (data,gc);
		for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
			if ((data->Flags () & DBDataFlagDisplay) == DBDataFlagDisplay)
				if (data->Type () == DBTypeVectorPoint)		DrawPoints (data,gc);
		}
	else	for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
		if ((data->Flags () & DBDataFlagDisplay) == DBDataFlagDisplay)
			switch (data->Type ())
				{
				case DBTypeVectorPoint:		DrawPoints (data,gc);	break;
				case DBTypeVectorLine:		DrawLines (data,gc);		break;
				case DBTypeVectorPolygon:	DrawPolygons (data,gc);	break;
				case DBTypeGridDiscrete:
				case DBTypeGridContinuous:	DrawGrid (data,gc);		break;
				case DBTypeNetwork:			DrawNetwork (data,gc);	break;
				default:	break;
				}
	for (data = dataset->FirstData ();data != (DBObjData *) NULL;data = dataset->NextData ())
		if (((data->Flags () & DBDataFlagDisplay) == DBDataFlagDisplay) &&
			  ((data->Type () & DBTypeVector) == DBTypeVector) &&
			  (((data->Flags () & DBDataFlagDispModeVecAnnotate) == DBDataFlagDispModeVecAnnotate) ||
			   ((data->Flags () & DBDataFlagDispModeVecAnnotSelect) == DBDataFlagDispModeVecAnnotSelect)))
			{
			XSetRegion (XtDisplay (DrawingAreaW),gc,FullRegion);
			XChangeGC (XtDisplay (DrawingAreaW),gc,GCForeground,&xgcv);
			DrawVectorAnnotations (data,gc);
			}
	if (XmToggleButtonGetState (MeshOptionW)) DrawMesh ();
	xswa.cursor = ActiveCursor;
	XChangeWindowAttributes (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),CWCursor,&xswa);
	XtReleaseGC (DrawingAreaW,gc);
	}


void UI2DView::Draw (DBRegion extent)

	{
	if (ViewEXT.InRegion (extent) == false) return;
	SetActiveExtent (extent);
	Draw ();
	SetActiveExtent ();
	}
