/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

CMDgrdImport.C

balazs.fekete@unh.edu

*******************************************************************************/
/*
 modified by pete weissbrod
 pfw@unh.edu
*/

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

using namespace std;
/*=========================================================================*/
#define RGISLaoutByRow			0
#define RGISLaoutByCol			1

#define RGISItemOrderTopDown	0
#define RGISItemOrderBottomUp	1

#define RGISReferenceCenter	0
#define RGISReferenceLLCorner	1

#define RGISGridBinary			0
#define RGISGridASCII			1

#define RGISBinTypeByte			0
#define RGISBinTypeShort		1
#define RGISBinTypeLong			2
#define RGISBinTypeSingle		3
#define RGISBinTypeDouble		4

/*==========================================================================*/

int confOk, colNum, rowNum, tempInt;
char *text, listFileName [DBDataFileNameLen], buffer [256];
char outFile[DBDataFileNameLen];
float cellWidth, cellHeight, llXCoord, llYCoord;
static int layout = 0, rowOrder = 0, fileType = 0, binaryType = 0, byteOrder = 0;
static int llCellCol, llCellRow = 0, skipHeader = 0, skipPad = 0, listFile = false;
static float missingVal = DBDefaultMissingFloatVal;
float temp;
bool Batch_Mode = false;
bool Continuous_Grid_Mode = false;

void showUsage(char argv[]);
int readHeader(bool ListFileMode);

void skipComments(){
	while(cin.peek()=='#' || cin.peek()=='\t' || cin.peek()== ' ')
		cin.ignore(1000000,'\n'); //skip the whole line!
}

int Exists_In_Arguments(char* argShort, char* argLong, int argc, char* argv[]){
	int i = 1;
	while(i<argc){
		if((strcmp(argShort,argv[i]) == 0) || (strcmp(argLong,argv[i]) == 0)) return i+1;
		i++;
	}
	return -1;
}


int Open_File(int argc, char* argv[]){
	int i=1;
	char* test;
	while(i<argc) {
		test = argv[i];
		if(*test!='-') {
			if (access (argv[i],R_OK) == 0) {
				if (fopen (argv[i],"r") == (FILE *) NULL) {
					fprintf (stderr,"File opening error in: %s", argv [i]);
					return (0);
					}
				else
					strcpy(listFileName,argv[i]);
				return 1;
			}
		}
	i++;
	}
	printf("no input file name was found.. Im going to read in from stdin!.\n");
	return 0;
}


int getValuesStandardIn(bool batchValue){

	if(batchValue == false) cout << "\tFile Name?> ";
	skipComments();
	cin >> listFileName;

	if(batchValue == false) cout << "\tNumber Of Columns?> ";
	skipComments();
	cin >> colNum;

	if(batchValue == false) cout << "\tNumber Of Rows?> ";
	skipComments();
	cin >> rowNum;
	if(colNum<=0 || rowNum <=0){cout << "Err-- the column/row numbers must be greater than 0!\n"; return 0;}

	if(batchValue == false) cout << "\tGrid Layout: 0=By Row, 1=By Column?> ";
	skipComments();
	cin >> layout;
	if(layout<0 || layout > 1){cout << "Err-- layout number is not a boolean!\n"; return 0;}

	if(batchValue == false) cout << "\tRow Order: 0=Top Down, 1=Bottom Up?> ";
	skipComments();
	cin >> rowOrder;
	if(rowOrder<0 || rowOrder > 1){cout << "Err-- row order number is not a boolean!\n"; return 0;}
  
	if(batchValue == false) cout << "\tFile Type: 0=Binary, 1=Ascii?> ";
	skipComments();
	cin >> fileType;
	if(fileType<0 || fileType>1){cout << "File type number is not a boolean!\n"; return 0;}

	if(batchValue == false) cout << "\tBinary Type: 0=byte, 1=short, 2=long, 3=float, 4=double?> ";
	skipComments();
	cin >> binaryType;
	if(binaryType<0 || binaryType>4){cout << "ERR- Binary_Type: invalid numbers!\n"; return 0;}

	if(batchValue == false) cout << "\tByte Order: 0=Big Endian, 1=Little Endian (ZERO if the file is ascii!)?> ";
	skipComments();
	cin >> byteOrder;
	if(byteOrder<0 || byteOrder>1){cout << "ERR- Byte Order: invalid numbers!\n"; return 0;}

	if(batchValue == false) cout << "\tLLCell_(Column)?> ";
	skipComments();
	cin >> llCellCol;

	if(batchValue == false) cout << "\tLLCell_(Row)?> ";
	skipComments();
	cin >> llCellRow;

	if(batchValue == false) cout << "\tCell size? (X)> ";
	skipComments();
	cin >> cellWidth;
	if(cellWidth <=0){cout << "ERR- cell width: Invalid number!\n"; return 0;}

	if(batchValue == false) cout << "\tCell size? (Y)> ";
	skipComments();
	cin >> cellHeight;
	if(cellHeight <0){cout << "ERR- cell height: Invalid number!\n"; return 0;}

	if(batchValue == false) cout << "\tLLCorner? (X)> ";
	skipComments();
	cin >> llXCoord;

	if(batchValue == false) cout << "\tLLCorner? (Y)> ";
	skipComments();
	cin >> llYCoord;

	if(batchValue == false) cout << "\tMissing Data Value?> ";
	skipComments();
	cin >> missingVal;

	if(batchValue == false) cout << "\tHeader?> ";
	skipComments();
	cin >> skipHeader;
	if(skipHeader<0){cout << "Err-- invalid value for header field\n"; return 0;}

	if(batchValue == false) cout << "\tList File? (0=no, 1=yes)> ";
	skipComments();
	cin >> listFile;
	if(listFile>1 || listFile<0){cout << "ERR- listfile: Invalid number!\n"; return 0;}

	if(batchValue == false) cout << "\tOuptut file?>";
	skipComments();
	cin >> outFile;

	if(batchValue == false) cout << "\tGrid Type: 0=discrete, 1=continuous?> ";
	skipComments();
	cin >> tempInt;
	if(tempInt<0 || tempInt > 1){cout << "Err-- Grid type field has invalid value!\n"; return 0;}
	if(tempInt == 0) Continuous_Grid_Mode = false; else Continuous_Grid_Mode = true;

	return 1;
}
	
int getValuesFromFileHeader(bool batchMode){

	bool ListFileMode = false;

	fileType = 1; //set the file type to ascii
	if(batchMode == true) skipComments();

	if(batchMode == false) cout << "\tBinary Type: 0=byte, 1=short, 2=long, 3=float, 4=double?> ";  
	skipComments();
	cin >> binaryType;
	if(binaryType<0 || binaryType>4){cout << "ERR- Binary_Type: invalid numbers!\n"; return 0;}

	if(batchMode == false) cout << "\tMissing Data Value?> ";
	skipComments();
	cin >> missingVal;

	if(batchMode == false) cout << "\tList File? (0=no, 1=yes)> ";
	skipComments();
	cin >> listFile;
	if(listFile>1 || listFile<0){cout << "ERR- listfile: Invalid number!\n"; return 0;}
	if(listFile == 1) ListFileMode = true;

	if(batchMode == false) cout << "\tOuptut file?>";
	skipComments();
	cin >> outFile;

	if(batchMode == false) cout << "\tGrid Type: 0=discrete, 1=continuous?> ";
	skipComments();
	cin >> tempInt;
	if(tempInt<0 || tempInt > 1){cout << "Err-- Grid type field has invalid value!\n"; return 0;}
	if(tempInt == 0) Continuous_Grid_Mode = false; else Continuous_Grid_Mode = true;

	//I assume these to be defaults for all ascii files
	llCellCol = 0;
	llCellRow = 0;
	skipHeader = 6;

	if(!readHeader(ListFileMode)) return 0;
	else return 1;
}
  
int readHeader(bool ListFileMode){
	char JUNK [40];
	char testFileName [DBDataFileNameLen];

	if(ListFileMode == true){
		ifstream textFile(listFileName);
		textFile >> testFileName;
	}
	else strcpy(testFileName,listFileName);

	ifstream infile(testFileName);
	bool cellCenter = false;

	while(infile.good()){
		infile >> JUNK;    infile >> colNum;                   //number of columns
		infile >> JUNK;    infile >> rowNum; infile >> JUNK;   //number of rows
		if(strcmp(JUNK,"XLLCENTER") == 0) cellCenter = true;   //is this xllcorner or xllcenter?
		infile >> llXCoord;                                    //llxcoord
		infile >> JUNK;    infile >> llYCoord;                 //llycoord
		infile >> JUNK;    infile >> cellWidth; cellHeight = cellWidth; //cellsize
		infile >> JUNK;    infile >> missingVal;               //nodata_val

		if(cellCenter){ 
			//if we are measuring from the center of a cell instead of the corner
			//set it so it reads from the corner.
			llXCoord = llXCoord - (cellWidth/2);
			llYCoord = llYCoord - (cellWidth/2);
			}

		return 1;
		}
  
	cout << "I couldn't read the file: " << listFileName << "... check the file for errors!\n";
	if(ListFileMode)
	cout << "List file: the problem could either be in there or in " << testFileName << "\n";
	return 0;
}  
       
int main(int argc, char* argv[])
{
	if(Exists_In_Arguments("-h","--help",argc,argv) != -1){showUsage(argv[0]); return 0;}
	if(Exists_In_Arguments("-b","--batch",argc,argv) != -1) Batch_Mode = true;

	if(Open_File(argc,argv) == 0){
		if(!getValuesStandardIn(Batch_Mode)) return 0;
		}
	else
		{
		//we have the file name, and we can get all of the values we need
		//from opening the file
		if(!getValuesFromFileHeader(Batch_Mode)) return 0;
		}

	DBObjData *grdData;

	if(Continuous_Grid_Mode)
		grdData = new DBObjData ("",DBTypeGridContinuous);
	else 
		grdData = new DBObjData ("",DBTypeGridDiscrete);
  
  
	grdData -> Name("Untitled");
	FILE *inFILE, *lstFILE = (FILE *) NULL;
	char fileName [DBDataFileNameLen], recordName [DBStringLength];
	DBInt pathLen, itemSize, chunk, i, j, row, col, recordLen;
	DBCoordinate coord;
	DBRegion extent;
	DBObjTable *layerTable = grdData->Table (DBrNLayers);  
	DBObjTable *itemTable  = grdData->Table (DBrNItems);
	DBObjTableField *missingValueFLD   = grdData->Type () == DBTypeGridContinuous ?
		itemTable->Field (DBrNMissingValue) : (DBObjTableField *) NULL;
	DBObjTableField *rowNumFLD		= layerTable->Field (DBrNRowNum);  
	DBObjTableField *colNumFLD 	= layerTable->Field (DBrNColNum);
	DBObjTableField *cellWidthFLD = layerTable->Field (DBrNCellWidth);
	DBObjTableField *cellHeightFLD= layerTable->Field (DBrNCellHeight);
	DBObjTableField *valueTypeFLD = layerTable->Field (DBrNValueType);
	DBObjTableField *valueSizeFLD = layerTable->Field (DBrNValueSize);
	DBObjTableField *layerFLD 		= layerTable->Field (DBrNLayer);
	DBObjRecord *layerRec, *itemRec, *dataRec;
	DBGridIO *gridIO;

	coord.X = llXCoord;
	coord.Y = llYCoord;
	extent.Expand (coord);
	coord.X += colNum * cellWidth;
	coord.Y += rowNum * cellHeight;
	extent.Expand (coord);
	grdData->Extent (extent);
	if ((llXCoord >= -180.0) && (coord.X <= 180.0) && (llYCoord >= -90.) && (coord.Y <= 90.))
		grdData->Projection (DBProjectionSpherical);
	else
		grdData->Projection (DBProjectionCartesian);
	grdData->Precision  ((int) floor ((log (cellWidth < cellHeight ? cellWidth : cellHeight) - 1)));

	switch (binaryType)
		{
		case RGISBinTypeByte:
			itemSize = sizeof (DBByte);
			break;
		case RGISBinTypeShort:
			itemSize = sizeof (DBShort);
			break;
		case RGISBinTypeLong:
			itemSize = sizeof (DBInt);
			break;
		case RGISBinTypeSingle:
			itemSize = sizeof (DBFloat4);
			break;
		case RGISBinTypeDouble:
			itemSize = sizeof (DBFloat);
			break;
		default:
			perror ("Invalid Data Type in: DBNetworkToGrid 	()");
			delete grdData;
			return 0;
		}
  
	for (pathLen = strlen (listFileName) - 1;(pathLen > 0) && (listFileName [pathLen] != '/');--pathLen);
	if (listFileName [pathLen] == '/') ++pathLen;
	if (listFile)
		{
		if ((lstFILE = fopen (listFileName,"r")) == (FILE *) NULL)
			{
			perror ("List File Openining Error in: RGISToolsImportGridCBK ()");
			delete grdData;
			return 0;
			}
		listFileName [pathLen] = '\0';
	}

	while (true)
		{
		if (listFile)
			{
			if (fgets (buffer,sizeof (buffer) - 2,lstFILE) != buffer)	break;
			if (buffer [strlen (buffer) - 1] == '\n') buffer [strlen (buffer) - 1] = '\0';
			if (buffer [0] == '/')	strncpy (fileName,buffer,sizeof (fileName) - 1);
			else
				sprintf (fileName,"%s%s",listFileName,buffer);
			for (i = strlen (fileName) - 1;(i > 0) && fileName [i] != '/';--i);
			if (fileName [i] == '/') ++i;
			strncpy (recordName,fileName + i,sizeof (recordName) - 1);
			}
		else
			{
			if (layerTable->ItemNum () > 0) break;
			strncpy (fileName,listFileName,sizeof (fileName) - 1);
			strncpy (recordName,listFileName + pathLen,sizeof (recordName) - 1);
			}

		if ((inFILE = fopen (fileName,"r")) == (FILE *) NULL)
			{ perror ("File Openining Error in: RGISToolsImportGridCBK ()"); delete grdData; return 0; }
		if (fileType == RGISGridBinary)
			while (skipHeader > 0)
				{
				chunk = skipHeader < (int) sizeof (buffer) ? skipHeader : (int) sizeof (buffer);
				if (fread (buffer,chunk,1,inFILE) != 1)
					{
					perror ("File Reading Error in: RGISToolsImportGridCBK ()");
					fclose (inFILE);
					delete grdData;
					}
				skipHeader -= chunk;
				}
      else
			for (chunk = 0;chunk < skipHeader;++chunk)
			do fgets (buffer,sizeof (buffer) - 2,inFILE);
		while (buffer [strlen (buffer) - 1] != '\n');

		for (i = 0;i < (int) strlen (recordName);++i) if (recordName [i] == '.') recordName [i] = ' ';
		layerTable->Add (recordName);
		if ((layerRec = layerTable->Item ()) == (DBObjRecord *) NULL)
			{ fclose (inFILE); delete grdData; return 0; }
		rowNumFLD->Int (layerRec,rowNum);
		colNumFLD->Int (layerRec,colNum);
		cellWidthFLD->Float  (layerRec,cellWidth);
		cellHeightFLD->Float (layerRec,cellHeight);
		switch (binaryType)
			{
			case RGISBinTypeByte:
			case RGISBinTypeShort:
			case RGISBinTypeLong:
				valueTypeFLD->Int (layerRec,DBTableFieldInt);
				break;
			case RGISBinTypeSingle:
			case RGISBinTypeDouble:
				valueTypeFLD->Int (layerRec,DBTableFieldFloat);
				break;
			default:
				perror ("Invalid Data Type in: DBNetworkToGrid 	()");
				fclose (inFILE); delete grdData;
				return 0;
			}
		valueSizeFLD->Int (layerRec,itemSize);
		if ((dataRec = new DBObjRecord (layerRec->Name (),colNum * rowNum * valueSizeFLD->Int (layerRec),valueSizeFLD->Int (layerRec))) == (DBObjRecord *) NULL)
			{ fclose (inFILE); delete grdData; return 0; }
		(grdData->Arrays ())->Add (dataRec);
		layerFLD->Record (layerRec,dataRec); 
		if (grdData->Type () == DBTypeGridContinuous)
			{
			itemTable->Add (layerRec->Name ());
			if ((itemRec = itemTable->Item ()) == (DBObjRecord *) NULL)
				{ fclose (inFILE); delete grdData; return 0; }
			missingValueFLD->Float (itemRec,(DBFloat) missingVal);
			}
		if (fileType == RGISGridBinary)
			{
			recordLen = (layout == RGISLaoutByRow ? colNum : rowNum) * itemSize + skipPad;
			for (j = 0;(chunk = fread (buffer,1,sizeof (buffer),inFILE)) > 0;++j)
			for (i = 0;i < chunk;i += itemSize)
				{//this is the itemsize declaration
				//it is dependent upon the 
				if (byteOrder != DBByteOrder ())
				switch (itemSize)
					{
					default:
						break;
					case 2:
						DBByteOrderSwapHalfWord (buffer + i);
						break;
					case 4:
						DBByteOrderSwapWord (buffer + i);
						break;
					case 8:
						DBByteOrderSwapLongWord (buffer + i);
						break;
					}
				if (layout == RGISLaoutByRow)
					{
					if ((row =  (j * sizeof (buffer) + i) / recordLen) >= rowNum) continue;
					if ((col = ((j * sizeof (buffer) + i) % recordLen) / itemSize) >= colNum) continue;
					}
				else
					{
					if ((col =  (j * sizeof (buffer) + i) / recordLen) >= colNum) continue;
					if ((row = ((j * sizeof (buffer) + i) % recordLen) / itemSize) >= rowNum) continue;
					}
				if (rowOrder == RGISItemOrderBottomUp) row = rowNum - row - 1;
				col -= llCellCol;
				col = col < 0 ? colNum + col : col;
				row -= llCellRow;
				row = row < 0 ? rowNum + row : row;
				memcpy (((char *) dataRec->Data ()) + (row * colNum + col) * itemSize,buffer + i,itemSize);
				}
			}
		else
			{
			DBFloat val;
			i = j = 0;
			while (fscanf (inFILE,"%lf",&val) == 1)
				{
				if (j >= rowNum) continue;
				if (i >= colNum) continue;
				switch (binaryType)
					{
					case RGISBinTypeByte:
						((DBByte *) buffer) [0] = (DBByte) val;
						break;
					case RGISBinTypeShort:
						((DBShort *) buffer) [0] = (DBShort) val;
						break;
					case RGISBinTypeLong:
						((DBInt *) buffer) [0] = (DBInt) val;
						break;
					case RGISBinTypeSingle:
						((DBFloat4 *) buffer) [0] = (DBFloat4) val;
						break;
					case RGISBinTypeDouble:
						((DBFloat *) buffer) [0] = (DBFloat) val;
						break;
					}
				col = i; row = j;
				if (rowOrder == RGISItemOrderBottomUp) row = rowNum - row - 1;
				col -= llCellCol;
				col = col < 0 ? colNum + col : col;
				row -= llCellRow;
				row = row < 0 ? rowNum + row : row;
				memcpy (((char *) dataRec->Data ()) + (row * colNum + col) * itemSize,buffer,itemSize);
				if (layout == RGISLaoutByRow)
					{
					i += 1;
					if (i >= colNum) { i = 0; j += 1; }
					}
				else
					{
					j += 1;
					if (j >= rowNum) { j = 0; i += 1; }
					}
				}
			}
		fclose (inFILE);
		}
	if (listFile) fclose (lstFILE);
	gridIO = new DBGridIO (grdData);
	if (grdData->Type () == DBTypeGridContinuous)
		{
		DBObjTableField *missingValueFLD = itemTable->Field (DBrNMissingValue);
		missingValueFLD->Float (itemTable->Item (layerRec->Name ()),(DBFloat) missingVal);
		gridIO->RecalcStats ();
		}
	else
		{
		DBInt intVal;
		DBObjRecord *symRec = (grdData->Table (DBrNSymbols))->Add ("Default Symbol");
		DBObjTableField *gridValueFLD  = itemTable->Field (DBrNGridValue);
		DBObjTableField *gridSymbolFLD = itemTable->Field (DBrNSymbol);
		DBObjTableField *symbolIDFLD	 = (grdData->Table (DBrNSymbols))->Field (DBrNSymbolID);
		DBObjTableField *foregroundFLD = (grdData->Table (DBrNSymbols))->Field (DBrNForeground);
		DBObjTableField *backgroundFLD = (grdData->Table (DBrNSymbols))->Field (DBrNBackground);
		DBObjTableField *styleFLD = (grdData->Table (DBrNSymbols))->Field (DBrNStyle);

		symbolIDFLD->Int (symRec,0);
		foregroundFLD->Int (symRec,1);
		backgroundFLD->Int (symRec,0);
		styleFLD->Int (symRec,0);
		for (dataRec = (grdData->Arrays ())->First ();dataRec != (DBObjRecord *) NULL;dataRec = (grdData->Arrays ())->Next ())
			{
			for (i = 0;i < colNum * rowNum;++i)
				{
				if (itemSize == (int) sizeof (DBByte))
					intVal = (DBInt) (*((DBByte *)  ((char *) dataRec->Data () + i * itemSize)));
				else intVal = (DBInt) (*((DBShort *) ((char *) dataRec->Data () + i * itemSize)));

				sprintf (buffer,"Category%d",intVal);
				if ((itemRec = itemTable->Item (buffer)) == (DBObjRecord *) NULL)
					{
					if ((itemRec = itemTable->Add (buffer)) == (DBObjRecord *) NULL)
						{
						perror ("Item Object Creation Error in: RGISToolsImportGridCBK ()");
						delete gridIO; delete grdData;
						return 0;
						}
					gridValueFLD->Int (itemRec,intVal);
					gridSymbolFLD->Record (itemRec,symRec);
					}
				intVal = itemRec->RowID ();
				if (itemSize == (int) sizeof (DBByte)) *((DBByte *)  ((char *) dataRec->Data () + i * itemSize)) = intVal;
				else *((DBShort *) ((char *) dataRec->Data () + i * itemSize)) = intVal;
				}
			}
		itemTable->ListSort (gridValueFLD);
		for (dataRec = (grdData->Arrays ())->First ();dataRec != (DBObjRecord *) NULL;dataRec = (grdData->Arrays ())->Next ())
			{
			for (i = 0;i < colNum * rowNum;++i)
				{
				if (itemSize == (int) sizeof (DBByte))
					intVal = (DBInt) (*((DBByte *)  ((char *) dataRec->Data () + i * itemSize)));
				else intVal = (DBInt) (*((DBShort *) ((char *) dataRec->Data () + i * itemSize)));
				itemRec = itemTable->Item (intVal);
				intVal = itemRec->ListPos ();

				if (itemSize == (int) sizeof (DBByte))	*((DBByte *)  ((char *) dataRec->Data () + i * itemSize)) = intVal;
				else *((DBShort *) ((char *) dataRec->Data () + i * itemSize)) = intVal;
				}
			}
		itemTable->ItemSort ();
		gridIO->DiscreteStats ();
		}
	delete gridIO;
	if(grdData->Write (outFile) == DBFault) return(-1);
	return 0;
}

void showUsage(char progName[]){
	fprintf(stdout,"%s - imports data files into ghdb format\n",progName);
	fprintf(stdout,"  options:\n");
	fprintf(stdout,"    -b,--batch (optional) turns the prompt for information off\n");
	fprintf(stdout,"    -h,--help (optional) shows usage\n");
	fprintf(stdout,"    <filename> (optional) grdimport will attempt to automatically\n");
	fprintf(stdout,"    read the standard header from an ascii file.\n");
	fprintf(stdout,"  examples:\n");
	fprintf(stdout,"    grdImport.Linux -b < completeInputFile.txt\n");
	fprintf(stdout,"      Don't prompt for information, just read completeFile.txt for arguments\n");
	fprintf(stdout,"    grdImport.Linux -b someFile.asc < partialInputFile.txt\n");
	fprintf(stdout,"      Don't prompt for information, just read the header of the ascii file and\n");
	fprintf(stdout,"      partialInputFile.txt for all of the arguments.\n");
	fprintf(stdout,"    grdImport.Linux <--prompt for all information step by step\n");
	fprintf(stdout,"      grdImport.Linux someFile.asc <--get as much info as you can out of\n");
	fprintf(stdout,"      the header, and prompt for whatever else is needed.\n\n");
	exit(0);
}
