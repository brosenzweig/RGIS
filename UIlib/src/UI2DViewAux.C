/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

UI2DViewAux.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <UI.H>

void UI2DView::SetExtent (DBRegion extent)

	{
	XPoint wlist [5], xy0, xy1;
	DBFloat u,v;
	DBCoordinate lowerLeft = extent.LowerLeft, upperRight = extent.UpperRight;
	double windowratio, mapratio;

	RequiredEXT = extent;
	wlist [0].x = Image->width  / 2;
	wlist [0].y = Image->height / 2;

	windowratio = (double) Image->width / (double) Image->height;
	mapratio = (double) (upperRight.X - lowerLeft.X) / (double) (upperRight.Y - lowerLeft.Y);

	if (mapratio > windowratio)
		{
		wlist [1].x = Image->width;
		wlist [1].y = (short) (wlist [0].y - wlist [0].x / mapratio);
		wlist [2].x = 0;
		wlist [2].y = (short) (wlist [0].y + wlist [0].x / mapratio);
		}
	else
		{
		wlist [1].x = (short) (wlist [0].x + wlist [0].y * mapratio);
		wlist [1].y = 0;
		wlist [2].x = (short) (wlist [0].x - wlist [0].y * mapratio);
		wlist [2].y = Image->height;
		}
	MultipX = (upperRight.X - lowerLeft.X) / (double) (wlist [1].x - wlist [2].x);
	MultipY = (upperRight.Y - lowerLeft.Y) / (double) (wlist [1].y - wlist [2].y);
	TransX  = lowerLeft.X  - (double) (wlist [2].x) * MultipX;
	TransY  = upperRight.Y - (double) (wlist [1].y) * MultipY;

	MultipU = (double) (wlist [1].x - wlist [2].x) / (upperRight.X - lowerLeft.X);
	MultipV = (double) (wlist [1].y - wlist [2].y) / (upperRight.Y - lowerLeft.Y);
	TransU  = (double) (wlist [2].x) - (lowerLeft.X  * MultipU);
	TransV  = (double) (wlist [1].y) - (upperRight.Y * MultipV);

	xy0.x = 0;					xy0.y = 0;
	xy1.x = Image->width;	xy1.y = Image->height;

	Window2Map (xy0.x,xy0.y,&u,&v);
	lowerLeft.X = u; upperRight.X = u;
	lowerLeft.Y = v; upperRight.Y = v;

	Window2Map (xy0.x,xy1.y,&u,&v);
	lowerLeft.X  = u < lowerLeft.X   ? u : lowerLeft.X;
	lowerLeft.Y  = v < lowerLeft.Y   ? v : lowerLeft.Y;
	upperRight.X = u > upperRight.X  ? u : upperRight.X;
	upperRight.Y = v > upperRight.Y  ? v : upperRight.Y;

	Window2Map (xy1.x,xy1.y,&u,&v);
	lowerLeft.X  = u < lowerLeft.X  ? u : lowerLeft.X;
	lowerLeft.Y  = v < lowerLeft.Y  ? v : lowerLeft.Y;
	upperRight.X = u > upperRight.X ? u : upperRight.X;
	upperRight.Y = v > upperRight.Y ? v : upperRight.Y;

	Window2Map (xy1.x,xy0.y,&u,&v);
	lowerLeft.X  = u < lowerLeft.X  ? u : lowerLeft.X;
	lowerLeft.Y  = v < lowerLeft.Y  ? v : lowerLeft.Y;
	upperRight.X = u > upperRight.X ? u : upperRight.X;
	upperRight.Y = v > upperRight.Y ? v : upperRight.Y;

	ViewEXT.LowerLeft = lowerLeft;		ViewEXT.UpperRight = upperRight;
	}

void UI2DView::SetActiveExtent (DBRegion extent)

	{
	XPoint poly [4];
	DBCoordinate lowerLeft  = ViewEXT.LowerLeft,	upperRight  = ViewEXT.UpperRight;
	DBCoordinate eLowerLeft = extent.LowerLeft, 	eUpperRight = extent.UpperRight;
	
	if (fabs (((eUpperRight.X - eLowerLeft.X) / (upperRight.X - lowerLeft.X))) < 0.03)
		{
		eUpperRight.X = eUpperRight.X + fabs (upperRight.X - lowerLeft.X) *.015;
		eLowerLeft.X  = eLowerLeft.X  - fabs (upperRight.X - lowerLeft.X) *.015;
		}
	if (fabs (((eUpperRight.Y - eLowerLeft.Y) / (upperRight.Y - lowerLeft.Y))) < 0.03)
		{
		eUpperRight.Y = eUpperRight.Y + fabs (upperRight.Y - lowerLeft.Y) *.015;
		eLowerLeft.Y  = eLowerLeft.Y  - fabs (upperRight.Y - lowerLeft.Y) *.015;
		}
	if (DrawRegion != NULL) XDestroyRegion (DrawRegion);
	lowerLeft.X = eLowerLeft.X > lowerLeft.X ? eLowerLeft.X : lowerLeft.X;
	lowerLeft.Y = eLowerLeft.Y > lowerLeft.Y ? eLowerLeft.Y : lowerLeft.Y;
	upperRight.X = eUpperRight.X < upperRight.X ? eUpperRight.X : upperRight.X;
	upperRight.Y = eUpperRight.Y < upperRight.Y ? eUpperRight.Y : upperRight.Y;

	ActiveEXT.LowerLeft = lowerLeft; ActiveEXT.UpperRight = upperRight;
	Map2Window (lowerLeft.X,  lowerLeft.Y,  &(poly [0].x), &(poly [0].y));
	Map2Window (lowerLeft.X,  upperRight.Y, &(poly [1].x), &(poly [1].y));
	Map2Window (upperRight.X, upperRight.Y, &(poly [2].x), &(poly [2].y));
	Map2Window (upperRight.X, lowerLeft.Y,  &(poly [3].x), &(poly [3].y));
	poly [0].x = poly [0].x > 5 ? poly [0].x - 5 : 0;
	poly [0].y = poly [0].y < Image->height - 5 ? poly [0].y + 5 : Image->height;

	poly [1].x = poly [1].x > 6 ? poly [1].x - 6 : 0;
	poly [1].y = poly [1].y > 6 ? poly [1].y - 6 : 0;

	poly [2].x = poly [2].x < Image->width  - 6 ? poly [2].x + 6 : Image->width;
	poly [2].y = poly [2].y > 6 ? poly [2].y - 6 : 0;

	poly [3].x = poly [3].x < Image->width  - 6 ? poly [3].x + 6 : Image->width;
	poly [3].y = poly [3].y < Image->height - 6 ? poly [3].y + 6 : Image->height;
	DrawRegion = XPolygonRegion (poly,4,EvenOddRule);
	}

void UI2DView::Size ()
 
	{
	DBInt bitmap_pad;
	XPoint poly [4];
	XWindowAttributes xwa;
	char *data;

	XGetWindowAttributes (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),&xwa);
	switch (xwa.depth)
		{
		case 8:	bitmap_pad = 8;	break;
		case 15:
		case 16:	bitmap_pad = 16;	break;
		default:	bitmap_pad = 32;	break;
		}

	if (Image != NULL)
		{
		if ((xwa.width == Image->width) && (xwa.height == Image->height)) return;
		else XDestroyImage (Image);
		}
	if ((data = (char *) malloc (xwa.width * xwa.height * (bitmap_pad >> 0x03))) == (char *) NULL)
		{ perror ("Memory Allocation Error in: UI2DView::Size ()"); return; }
	Image = XCreateImage(XtDisplay(DrawingAreaW),xwa.visual,xwa.depth,ZPixmap,0,data,xwa.width,xwa.height,bitmap_pad,0);
	if (FullRegion != NULL) XDestroyRegion (FullRegion);
	poly [0].x = poly [1].x = 0; poly [2].x = poly [3].x = xwa.width; 
	poly [0].y = poly [3].y = 0; poly [1].y = poly [2].y = xwa.height;
	FullRegion = XPolygonRegion (poly,4,EvenOddRule);
	UILoop ();
	return;
	}

void UI2DView::Clear ()

	{
	short x,y,width,height;
	DBCoordinate lowerLeft = ActiveEXT.LowerLeft, upperRight = ActiveEXT.UpperRight;

	Map2Window (lowerLeft.X, upperRight.Y,&x,&y);
	Map2Window (upperRight.X,lowerLeft.Y,&width,&height);
	width  -= x;
	height -= y;

	XAddPixel (Image,UIColor (UIColorStandard,0));
	XClearArea (XtDisplay (DrawingAreaW),XtWindow (DrawingAreaW),x,y,width,height,false);
	}

int UI2DView::Window2Map (short wx,short wy,double *mx,double *my)

	{
	DBCoordinate lowerLeft = ViewEXT.LowerLeft, upperRight = ViewEXT.UpperRight;
	*mx = MultipX * (double) wx + TransX;
	*my = MultipY * (double) wy + TransY;

	if (*mx > upperRight.X) return (false);
	if (*my > upperRight.Y) return (false);
	if (*mx < lowerLeft.X)  return (false);
	if (*my < lowerLeft.Y)  return (false);
	return (true);
	}

int UI2DView::Map2Window (double mx,double my,short *wx,short *wy)

	{
	*wx = (short) (MultipU * mx + TransU);
	*wy = (short) (MultipV * my + TransV);
	if (*wx > Image->width)  return (false);
	if (*wy > Image->height) return (false);
	if (*wx < 0) return (false);
	if (*wy < 0) return (false);
	return (true);
	}
