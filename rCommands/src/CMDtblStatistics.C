/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDtblStatistics.C

andras@ob.sr.unh.edu

*******************************************************************************/

#include <math.h>
#include <cm.h>
#include <DB.H>
#include <DBif.H>

enum Functions { BAD, NUM, NONNULL, MIN, MAX, SUM, AVG, NAVG, PCT, MED, MOD, DEV };

struct Groups
	{
	DBObjTableField *srcFLD;
	DBObjTableField *dstFLD;
	};

struct Values
	{
	int occur;
	DBFloat val;
	Values *next;
	};

class FieldOptions
	{
	public:
		FieldOptions()
			{
			Name = new char[1];
			Name = '\0';
			reName = new char[1];
			reName = '\0';
			isInteger = false;
			print = false;
			func = BAD;
			headval = tailVal = new Values;
			next = (FieldOptions *) NULL;
			field = (DBObjTableField *) NULL;
			}
		FieldOptions( const Functions funcname, const char *oldName, const FieldOptions *nxt)
			{
			Name = new char[strlen(oldName)+1];
			strcpy(Name,oldName);
			reName = new char[1];
			reName = '\0';
			isInteger = false;
			print = false;
			func = funcname;
			headval = tailVal = new Values;
			next = (FieldOptions *) nxt;
			field = (DBObjTableField *) NULL;
			}
		FieldOptions( const Functions funcname, const char *oldName, const char *newName, const FieldOptions *nxt)
			{
			Name = new char[strlen(oldName)+1];
			strcpy(Name,oldName);
			reName = new char[strlen(newName)+1];
			strcpy(reName,newName);
			isInteger = false;
			print = false;
			func = funcname;
			headval = tailVal = new Values();
			next = (FieldOptions *) nxt;
			field = (DBObjTableField *) NULL;
			}
		~FieldOptions()
			{
			Values *p;
			delete Name;
			delete reName;
			p = headval;
			while(p) { headval = headval->next; delete p; p = headval; }
			}
		char* getNewName() const { return reName; }
		void setNewName( const char *newName ) { delete reName; reName = new char[strlen(newName) + 1]; strcpy(reName,newName); }
		char* getOldName() const { return Name; }
		Functions getFunc() const { return func; }
		char* getFuncName() const
			{
			if     (func == NUM)     return ((char *) "num");
			else if(func == NONNULL) return ((char *) "nonnull");
			else if(func == MIN)     return ((char *) "min");
			else if(func == MAX)     return ((char *) "max");
			else if(func == SUM)     return ((char *) "sum");
			else if(func == AVG)     return ((char *) "avg");
			else if(func == NAVG)    return ((char *) "navg");
			else if(func == PCT)     return ((char *) "pct");
			else if(func == MED)     return ((char *) "med");
			else if(func == MOD)     return ((char *) "mod");
			else if(func == DEV)     return ((char *) "dev");
			return ((char *) "bad");
			}
		bool isInt() const { return isInteger; }
		void setInt() { isInteger = true; }
		bool getPrint() const { return print; }
		void setPrint(const bool input) { print = input; }
		Values *getHead() const { return headval; }
		bool operator == (const char *str) const { return (strcmp(Name,str) == 0); }
		bool operator != (const char *str) const { return (strcmp(Name,str) != 0); }
		FieldOptions *next; // pointer to next element in list
		Values *tailVal;    // pointer to last value entered in list
		DBObjTableField *field;
	private:
		char *Name, *reName;
		bool isInteger;
		Values *headval; // used if !(NUM || NONNULL || MIN || MAX || SUM)
		bool print;
			// 0  don't keep
			// 1  keep
		Functions func;
	};

void doHelp(bool extended, char *arg0)
	{
	if(extended)
		{
		CMmsgPrint (CMmsgInfo,"%s [options] [field] ... <input file> ...\n",CMprgName(arg0));
		CMmsgPrint (CMmsgInfo,"**fieldnames must match fieldnames in first line of datafile.**\nValid flags:\n");
		CMmsgPrint (CMmsgInfo,"  -a,--table   [name]              => Set the name of the table to use.\n");
		CMmsgPrint (CMmsgInfo,"  -c,--ascii                       => Output as ASCII\n");
		CMmsgPrint (CMmsgInfo,"  -d,--domain  [domain]            => Specify domain of output table.\n");
		CMmsgPrint (CMmsgInfo,"  -f,--field   [operation] [field] => Perform 'operation' on field values, where operation\n");
		CMmsgPrint (CMmsgInfo,"     is one of the following:\n");
		CMmsgPrint (CMmsgInfo,"       num                  = Output number of rows inside group.\n");
		CMmsgPrint (CMmsgInfo,"       nonnull field        = Number of non-NULL field values inside group.\n");
		CMmsgPrint (CMmsgInfo,"       min field            = Output the minimum value in the given field.\n");
		CMmsgPrint (CMmsgInfo,"       max field            = Output the maximum value in the given field.\n");
		CMmsgPrint (CMmsgInfo,"       sum field            = Output the sum of values in the given field.\n");
		CMmsgPrint (CMmsgInfo,"       avg field            = Output the mean of non-NULL values in given field.\n");
		CMmsgPrint (CMmsgInfo,"       navg field           = Output the mean of all values in given field.\n");
		CMmsgPrint (CMmsgInfo,"       pct field percentage = Percentage function [ x : F(x) = p/100 ].\n");
		CMmsgPrint (CMmsgInfo,"       med field            = Output the median of values in given field.\n");
		CMmsgPrint (CMmsgInfo,"       mod field            = Output the mode of values in given field.\n");
		CMmsgPrint (CMmsgInfo,"       dev field            = Output standard deviation of values in given field.\n");
		CMmsgPrint (CMmsgInfo,"     Field values are assumed to be numeric.\n");
		CMmsgPrint (CMmsgInfo,"  -g,--group  [field]              => Perform selected operations on data groups, selected\n");
		CMmsgPrint (CMmsgInfo,"     by having the same value in the group field.  Process whole input if no group is\n");
		CMmsgPrint (CMmsgInfo,"     given.  Select groups based on groups of fields if multiple groups are given.\n");
		CMmsgPrint (CMmsgInfo,"  -h,--help                        => Print this usage information.\n");
		CMmsgPrint (CMmsgInfo,"  -o,--output  [filename]          => Specify output filename, else use STDOUT\n");
		CMmsgPrint (CMmsgInfo,"  -r,--rename  [name]              => Set the name of the operation column.\n");
		CMmsgPrint (CMmsgInfo,"  -s,--subject [subject]           => Specify subject of output table.\n");
		CMmsgPrint (CMmsgInfo,"  -t,--title   [title]             => Specify title of output table.\n");
		CMmsgPrint (CMmsgInfo,"  -v,--version [version]           => Specify version of output table.\n");
		}
	else
		{
		CMmsgPrint (CMmsgInfo,"%s [options] [field] ... <input file> ...\n",CMprgName(arg0));
		CMmsgPrint (CMmsgInfo,"  -a,--table   [name]\n");
		CMmsgPrint (CMmsgInfo,"  -c,--ascii\n");
		CMmsgPrint (CMmsgInfo,"  -d,--domain  [domain]\n");
		CMmsgPrint (CMmsgInfo,"  -f,--field   [operation] [field]\n");
		CMmsgPrint (CMmsgInfo,"  -g,--group   [field]\n");
		CMmsgPrint (CMmsgInfo,"  -h,--help    => for more help type -h e, or --help extend.\n");
		CMmsgPrint (CMmsgInfo,"  -o,--output  [filename]\n");
		CMmsgPrint (CMmsgInfo,"  -r,--rename  [name]\n");
		CMmsgPrint (CMmsgInfo,"  -s,--subject [subject]\n");
		CMmsgPrint (CMmsgInfo,"  -t,--title   [title]\n");
		CMmsgPrint (CMmsgInfo,"  -v,--version [version]\n");
		}
	exit(1);
	}

bool FLDExists(FieldOptions *head, const char *name, const Functions func, FieldOptions **place)
	{
	FieldOptions *p, *prev;
	p = head->next;
	prev = head;
	while((p != (FieldOptions *) NULL) && ((p->getFunc() != func) || (strcmp(p->getOldName(),name) != 0))) { prev = p; p = p->next; }
	if (p) { *place = p; return true; } // returns where item was found
	*place = prev; // return last element
	return false;
	}

bool FLDExists(FieldOptions *head, const char *name, const Functions func, FieldOptions **prev, FieldOptions **p)
	{
	*p = head->next;
	*prev = head;
	while((*p != (FieldOptions *) NULL) && (((*p)->getFunc() != func) || (strcmp((*p)->getOldName(),name) != 0)))
		{ *prev = *p; *p = (*p)->next; }
	if (*p) return true; // returns where item was found
	return false; // return last element
	}

FieldOptions* FOHierarchy(const char *funcname, const char *oldName, char *newName, int pct, FieldOptions *head)
	{
	FieldOptions *last = (FieldOptions *) NULL;
	bool rmNew = false;
	if(!strcmp(funcname,"num"))
		{
		if(FLDExists(head,"NUM",NUM,&last) && last->getPrint())
			fprintf(stderr,"** Warning: Already have a -f %s %s specified once!\n",last->getFuncName(),last->getOldName());
		if(newName == (char *) NULL) { newName = new char[4]; strcpy(newName,"NUM"); rmNew = true; }
		last->next = new FieldOptions(NUM, "NUM", newName, last->next); last = last->next;
		}
	else if(!strcmp(funcname,"nonnull"))
		{
		if(FLDExists(head,oldName,NONNULL,&last) && last->getPrint())
			fprintf(stderr,"** Warning: Already have a -f %s %s specified once!\n",last->getFuncName(),last->getOldName());
		if(newName == (char *) NULL) { newName = new char[strlen(oldName) + 10]; sprintf(newName,"NONNULL(%s)",oldName); rmNew = true; }
		last->next = new FieldOptions(NONNULL, oldName, newName, last->next); last = last->next;
		}
	else if(!strcmp(funcname,"min"))
		{
		if(FLDExists(head,oldName,MIN,&last) && last->getPrint())
			fprintf(stderr,"** Warning: Already have a -f %s %s specified once!\n",last->getFuncName(),last->getOldName());
		if(newName == (char *) NULL) { newName = new char[strlen(oldName) + 6]; sprintf(newName,"MIN(%s)",oldName); rmNew = true; }
		last->next = new FieldOptions(MIN, oldName, newName, last->next); last = last->next;
		}
	else if(!strcmp(funcname,"max"))
		{
		if(FLDExists(head,oldName,MAX,&last) && last->getPrint())
			fprintf(stderr,"** Warning: Already have a -f %s %s specified once!\n",last->getFuncName(),last->getOldName());
		if(newName == (char *) NULL) { newName = new char[strlen(oldName) + 6]; sprintf(newName,"MAX(%s)",oldName); rmNew = true; }
		last->next = new FieldOptions(MAX, oldName, newName, last->next); last = last->next;
		}
	else if(!strcmp(funcname,"sum"))
		{
		if(FLDExists(head,oldName,SUM,&last) && last->getPrint())
			fprintf(stderr,"** Warning: Already have a -f %s %s specified once!\n",last->getFuncName(),last->getOldName());
		if(newName == (char *) NULL) { newName = new char[strlen(oldName) + 6]; sprintf(newName,"SUM(%s)",oldName); rmNew = true; }
		last->next = new FieldOptions(SUM, oldName, newName, last->next); last = last->next;
		}
	else if(!strcmp(funcname,"avg"))
		{
		FOHierarchy("sum", oldName, (char *) NULL,-1, head);
		FOHierarchy("nonnull", oldName, (char *) NULL,-1, head);
		if(FLDExists(head,oldName,AVG,&last) && last->getPrint())
			fprintf(stderr,"** Warning: Already have a -f %s %s specified once!\n",last->getFuncName(),last->getOldName());
		if(newName == (char *) NULL) { newName = new char[strlen(oldName) + 6]; sprintf(newName,"AVG(%s)",oldName); rmNew = true; }
		last->next = new FieldOptions(AVG, oldName, newName, last->next); last = last->next;
		}
	else if(!strcmp(funcname,"navg"))
		{
		FOHierarchy("sum", oldName, (char *) NULL,-1, head);
		FOHierarchy("num", oldName, (char *) NULL,-1, head);
		if(FLDExists(head,oldName,NAVG,&last) && last->getPrint())
			fprintf(stderr,"** Warning: Already have a -f %s %s specified once!\n",last->getFuncName(),last->getOldName());
		if(newName == (char *) NULL) { newName = new char[strlen(oldName) + 7]; sprintf(newName,"NAVG(%s)",oldName); rmNew = true; }
		last->next = new FieldOptions(NAVG, oldName, newName, last->next); last = last->next;
		}
	else if(!strcmp(funcname,"pct"))
		{
		if(FLDExists(head,oldName,PCT,&last) && last->getPrint())
			fprintf(stderr,"** Warning: Already have a -f %s %s specified once!\n",last->getFuncName(),last->getOldName());
		if(newName == (char *) NULL) { newName = new char[strlen(oldName) + 6]; sprintf(newName,"PCT(%s)",oldName); rmNew = true; }
		last->next = new FieldOptions(PCT, oldName, newName, last->next); last = last->next;
		last->getHead()->occur = pct;
		}
	else if(!strcmp(funcname,"med"))
		{
		if(FLDExists(head,oldName,MED,&last) && last->getPrint())
			fprintf(stderr,"** Warning: Already have a -f %s %s specified once!\n",last->getFuncName(),last->getOldName());
		if(newName == (char *) NULL) { newName = new char[strlen(oldName) + 6]; sprintf(newName,"MED(%s)",oldName); rmNew = true; }
		last->next = new FieldOptions(MED, oldName, newName, last->next); last = last->next;
		last->getHead()->occur = 50;
		}
	else if(!strcmp(funcname,"mod"))
		{
		if(FLDExists(head,oldName,MOD,&last) && last->getPrint())
			fprintf(stderr,"** Warning: Already have a -f %s %s specified once!\n",last->getFuncName(),last->getOldName());
		if(newName == (char *) NULL) { newName = new char[strlen(oldName) + 6]; sprintf(newName,"MOD(%s)",oldName); rmNew = true; }
		last->next = new FieldOptions(MOD, oldName, newName, last->next); last = last->next;
		}
	else if(!strcmp(funcname,"dev"))
		{
		FOHierarchy("avg", oldName, (char *) NULL,-1, head);
		FOHierarchy("num", oldName, (char *) NULL,-1, head);
		if(FLDExists(head,oldName,DEV,&last) && last->getPrint())
			fprintf(stderr,"** Warning: Already have a -f %s %s specified once!\n",last->getFuncName(),last->getOldName());
		if(newName == (char *) NULL) { newName = new char[strlen(oldName) + 6]; sprintf(newName,"DEV(%s)",oldName); rmNew = true; }
		last->next = new FieldOptions(DEV, oldName, newName, last->next); last = last->next;
		}
	else
		{ fprintf(stderr,"Unknown -f function: %s!\n",funcname); exit(DBFault); }
	if(rmNew) delete newName;
	return last;
	}

DBObjRecord* FindMatch(const DBObjRecord *inRecord, DBObjTable *outTable, const Groups **groups, const int numGrpNames)
	{
	static int lastFind = -2;
	DBObjRecord *outRecord;
	int i = 0, itemNum = outTable->ItemNum();
	if(lastFind == -1) // Check last entry
		{
		outRecord = outTable->Item(itemNum - 1);
		while (i < numGrpNames && DBTableFieldMatch (groups[i]->srcFLD,inRecord,groups[i]->dstFLD,outRecord)) i++;
		if(i == numGrpNames) return outRecord;
		}
	else if(lastFind != -2) // Check last successful find
		{
		outRecord = outTable->Item(lastFind);
		while (i < numGrpNames && DBTableFieldMatch (groups[i]->srcFLD,inRecord,groups[i]->dstFLD,outRecord)) i++;
		if(i == numGrpNames) return outRecord;
      if(lastFind != itemNum - 1) // Check last entry
         {
         outRecord = outTable->Item(itemNum - 1);
         while (i < numGrpNames && DBTableFieldMatch (groups[i]->srcFLD,inRecord,groups[i]->dstFLD,outRecord)) i++;
         if(i == numGrpNames) return outRecord;
         }
		}
	for(int outRecID = 0; outRecID < itemNum;++outRecID) // if all else fails...
		{
		outRecord = outTable->Item(outRecID);
		i = 0;
		while (i < numGrpNames && DBTableFieldMatch (groups[i]->srcFLD,inRecord,groups[i]->dstFLD,outRecord)) i++;
		if(i == numGrpNames) { lastFind = outRecID; return outRecord; }
		}
	lastFind = -1;
	return (DBObjRecord *) NULL;
}

int main (int argc,char *argv [])
	{
	int argPos, argNum = argc, numGrpNames = 0, i = 0;
	char **groupnames, *rename = (char *) NULL, *tableName = (char *) NULL;
	bool ascii = false;
	FieldOptions *head = (FieldOptions *) NULL, *p = (FieldOptions *) NULL, *temp = (FieldOptions *) NULL;
	Groups **groups = (Groups **) NULL;
	FILE *outFile = (FILE *) NULL;
	DBObjData *inData, *outData;
	DBObjTable *inTable, *outTable;
	DBObjTableField *field;
	DBObjRecord *inRecord, *outRecord;
	DBObjectLIST<DBObjTableField> *fields;

	if(argc <= 2) { doHelp(false,argv[0]); return(DBSuccess); }
	outData = new DBObjData("Untitled", DBTypeTable);
	outTable = outData->Table(DBrNItems);

	head = new FieldOptions(BAD,"","", (FieldOptions *) NULL);
	groupnames = (char **) malloc(sizeof(char *));

	for (argPos = 1;argPos < argNum;)
		{
		if (CMargTest(argv[argPos],"-f","--field"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing operation and field after -f!\n"); return (CMfailed); }
			if(!strcmp(argv[argPos],"pct"))
				{
				if (argNum <= argPos + 2)
					{ CMmsgPrint (CMmsgUsrError,"Missing field and/or percentage after -f pct!\n"); return (CMfailed); }
				p = FOHierarchy(argv[argPos],argv[argPos+1],rename,atoi(argv[argPos+2]),head);
				argNum = CMargShiftLeft(argPos,argv,argNum);
				argNum = CMargShiftLeft(argPos,argv,argNum);
				}
			else if(!strcmp(argv[argPos],"num"))
				{
				char *num = new char[4];
				strcpy(num,"Num");
				p = FOHierarchy(argv[argPos],num,rename,-1,head);
				}
			else
				{
				if (argNum < argPos + 1)
					{ CMmsgPrint (CMmsgUsrError,"Missing operation or field after -f %s!\n",argv[argPos]); return (CMfailed); }
				p = FOHierarchy(argv[argPos],argv[argPos+1],rename,-1,head);
				argNum = CMargShiftLeft(argPos,argv,argNum);
				}
			p->setPrint(true);
			rename = (char *) NULL;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-g","--group"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing groupname!\n"); return (CMfailed); }
			if((groupnames = (char **) realloc(groupnames,(numGrpNames + 1) * sizeof(char *))) == (char **) NULL)
				{ perror ("Memory allocation error!\n"); return(DBFault); }
			groupnames[numGrpNames] = argv[argPos];
			numGrpNames++;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-h","--help"))
			{
			argNum = CMargShiftLeft (argPos,argv,argNum);
			if(CMargTest(argv[argPos],"e","extend"))
				{
				doHelp(true,argv[0]);
				argNum = CMargShiftLeft (argPos,argv,argNum);
				}
			else doHelp(false,argv[0]);
			}
		if (CMargTest(argv[argPos],"-c","--ascii"))
			{
			ascii = true;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-a","--table"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing table name!\n"); return (CMfailed); }
			tableName = argv[argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-r","--rename"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing field after -r!\n"); return (CMfailed); }
			rename = argv[argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-o","--output"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing output filename!\n"); return (CMfailed); }
			if((outFile = fopen(argv[argPos],"w")) == (FILE *) NULL)
				{ CMmsgPrint (CMmsgUsrError,"Cannot open file %s",argv[argPos]); return (CMfailed); }
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-t","--title"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!\n"); return (CMfailed); }
			outData->Name(argv[argPos]);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-s","--subject"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!\n"); return (CMfailed); }
			outData->Document(DBDocSubject,argv[argPos]);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-d","--domain"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!\n"); return (CMfailed); }
			outData->Document(DBDocGeoDomain,argv[argPos]);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-v","--version"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing version!\n"); return (CMfailed); }
			outData->Document(DBDocVersion,argv[argPos]);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ fprintf(stderr,"Unknown option: %s!\n",argv[argPos]); return (CMfailed); }
		argPos++;
		}

	if(outFile == (FILE *) NULL) outFile = stdout;
	if(head->next == (FieldOptions *) NULL) return(DBSuccess);

	inData = new DBObjData();
	if ((argNum > 1) && (strcmp(argv[1],"-") != 0)) inData->Read(argv[1]);
	else inData->Read(stdin);

	if (outData->Name() == (char *) NULL) outData->Name("Untitled");
	if (outData->Document(DBDocSubject) == (char *) NULL) outData->Document(DBDocSubject,inData->Document(DBDocSubject));
	if (outData->Document(DBDocGeoDomain) == (char *) NULL) outData->Document(DBDocGeoDomain,inData->Document(DBDocGeoDomain));
	if (outData->Document(DBDocVersion) == (char *) NULL) outData->Document(DBDocVersion,inData->Document(DBDocVersion));

	if(tableName == (char *) NULL) tableName = DBrNItems;
	if((inTable = inData->Table(tableName)) == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Invalid table!\n"); delete inData; return (CMfailed); }

	if((groups = (Groups **) malloc(numGrpNames * sizeof(Groups *))) == (Groups **) NULL)
		{ perror("Memory allocation error!\n"); return(DBFault); }
	for(i = 0; i < numGrpNames; i++)
		{
		if((field = inTable->Field(groupnames[i])) == (DBObjTableField *) NULL)
			{ fprintf(stderr,"Invalid group name: %s\n",groupnames[i]); return (CMfailed); }
		if(DBTableFieldIsCategory(field))
			{
			groups[i] = new Groups();
			groups[i]->srcFLD = field;
			groups[i]->dstFLD = new DBObjTableField(*field);
			outTable->AddField(groups[i]->dstFLD);
//			fprintf(stderr,"Added Group: %s\n",groups[i]->dstFLD->Name());
			}
		else fprintf(stderr,"Group %s is not Category!\n",groupnames[i]);
		}
	delete groupnames;

	p = head->next;
	temp = head;
	while(p->next)
		{
		FieldOptions *duplicate = (FieldOptions *) NULL, *prev = p;
		if(!p->getPrint() && FLDExists(p,p->getOldName(),p->getFunc(),&duplicate))
			{ temp->next = p->next; delete p; p = temp->next; continue; }
		while(FLDExists(prev,prev->getOldName(),prev->getFunc(),&prev,&duplicate) && !duplicate->getPrint())
			{ prev->next = duplicate->next; delete duplicate; }
		temp = p;
		p = p->next;
		}
//	p = head->next;
//	while(p) { fprintf(stderr,"Added: o:%s n:%s p:",p->getOldName(),p->getNewName()); if(p->getPrint()) fprintf(stderr,"true\n"); else fprintf(stderr,"false\n"); p = p->next; }

	fields = inTable->Fields();
	p = head->next;
	while(p)
		{
		field = fields->Item (p->getOldName());
		if (p->getFunc() == MIN || p->getFunc() == MAX)
			p->field = new DBObjTableField(p->getNewName(),field->Type(),field->Format(),field->Length());
		else if(p->getFunc() == NUM || p->getFunc() == NONNULL)
			p->field = new DBObjTableField(p->getNewName(), DBTableFieldInt, DBHiddenField, sizeof (DBInt));
		else p->field = new DBObjTableField(p->getNewName(), DBTableFieldFloat, DBHiddenField, sizeof(DBFloat));
		if(p->getFunc() != NUM && p->getFunc() != NONNULL)
			{
//			if ((field = inTable->Field(p->getOldName())) == (DBObjTableField *) NULL)
			if (field == (DBObjTableField *) NULL)
				{
				fprintf(stderr,"Invalid field name: %s\n",p->getOldName());
				return(DBFault);
				}
			if (!DBTableFieldIsNumeric(field))
				{
				fprintf(stderr,"Field is not Numeric: %s\n",p->getOldName());
				return(DBFault);
				}
			}
		outTable->AddField(p->field);
		p = p->next;
		}
// MAKE SURE TO TEST FOR SPEED BY DECLARING INTS OUTSIDE OF FOR LOOPS!!!

	for (int inRecID = 0;inRecID < inTable->ItemNum();++inRecID)
		{
		inRecord = inTable->Item(inRecID);
		if ((outRecord = FindMatch(inRecord, outTable,(const Groups**) groups, numGrpNames)) != (DBObjRecord *) NULL)
			{
			p = head->next;
			while(p)
				{
				field = fields->Item (p->getOldName());
				switch(p->getFunc())
					{
					default:
						break;
					case NUM:
						p->field->Int(outRecord,p->field->Int(outRecord) + 1);
						break;
					case NONNULL:
						if (p->isInt())
							{
							if (field->Int(inRecord) != field->IntNoData()) p->field->Int(outRecord,p->field->Int(outRecord) + 1);
							}
						else
							{
							if (!CMmathEqualValues(field->Float(inRecord),field->FloatNoData()))
								p->field->Int(outRecord,p->field->Int(outRecord) + 1);
							}
						break;
					case MIN:
						if (p->isInt())
							{
							if(field->Int(inRecord) != field->IntNoData())
								{
								if (p->field->Int(outRecord) != p->field->IntNoData())
									{ if (field->Int(inRecord) < p->field->Int(outRecord)) p->field->Int(outRecord,field->Int(inRecord)); }
								else { p->field->Int(outRecord,field->Int(inRecord)); }
								}
							}
						else
							{
							if (!CMmathEqualValues(field->Float(inRecord),field->FloatNoData()))
								{
								if (!CMmathEqualValues(p->field->Float(inRecord),p->field->FloatNoData()))
									{ if (field->Float(inRecord) < p->field->Float(outRecord)) p->field->Float(outRecord,field->Float(inRecord)); }
								else { p->field->Float(outRecord,field->Float(inRecord)); }
								}
							}
						break;
					case MAX:
						if (p->isInt())
							{
							if(field->Int(inRecord) != field->IntNoData())
								{
								if (p->field->Int(outRecord) != p->field->IntNoData())
									{ if (field->Int(inRecord) > p->field->Int(outRecord)) p->field->Int(outRecord,field->Int(inRecord)); }
								else { p->field->Int(outRecord,field->Int(inRecord)); }
								}
							}
						else
							{
							if (!CMmathEqualValues(field->Float(inRecord),field->FloatNoData()))
								{
								if (!CMmathEqualValues(p->field->Float(inRecord),p->field->FloatNoData()))
									{ if (field->Float(inRecord) > p->field->Float(outRecord)) p->field->Float(outRecord,field->Float(inRecord)); }
								else { p->field->Float(outRecord,field->Float(inRecord)); }
								}
							}
						break;
					case SUM:
						if (p->isInt())
							{
							if(field->Int(inRecord) != field->IntNoData())
								p->field->Int(outRecord,p->field->Int(outRecord) + field->Int(inRecord));
							}
							else
							{
							if (!CMmathEqualValues(field->Float(inRecord),field->FloatNoData()))
								p->field->Float(outRecord,p->field->Float(outRecord) + field->Float(inRecord));
							}
						break;
					case DEV:
					case PCT:
					case MED:
						p->tailVal = p->tailVal->next = new Values();
						p->tailVal->val = field->Float(inRecord);
						p->tailVal->next = 0;
						break;
					case MOD:
						Values *cur = p->getHead();
						while(cur->next && !CMmathEqualValues(cur->val,field->Float(inRecord))) cur = cur->next;
						if(cur->next) cur->occur++;
						else
							{
							p->tailVal->val = field->Float(inRecord);
							p->tailVal->occur = 1;
							p->tailVal = p->tailVal->next = new Values();
							}
						break;
					}
				p = p->next;
				}
			}
		else
			{
			outRecord = outTable->Add();

			for(i = 0; i < numGrpNames; i++)
				{
				switch (groups[i]->srcFLD->Type())
					{
					default:
					case DBTableFieldString:
						groups[i]->dstFLD->String(outRecord,groups[i]->srcFLD->String(inRecord));
						break;
					case DBTableFieldInt:
						groups[i]->dstFLD->Int(outRecord,groups[i]->srcFLD->Int(inRecord));
						break;
					}
				}
			p = head->next;
			while(p)
				{
				field = fields->Item (p->getOldName());
				switch(p->getFunc())
					{
					default:
					case BAD:
						break;
					case NUM:
						p->setInt();
						p->field->Int(outRecord,1);
						break;
					case NONNULL:
						if (field->Type() == DBTableFieldInt)
							{
							p->setInt();
							if (field->Int(inRecord) != field->IntNoData()) p->field->Int(outRecord,1);
							else p->field->Int(outRecord,0);
							}
						else
							{
							if (!CMmathEqualValues(field->Float(inRecord),field->FloatNoData())) p->field->Int(outRecord,1);
							else p->field->Int(outRecord,0);
							}
						break;
					case MIN:
						if (field->Type() == DBTableFieldInt)
							{ p->setInt(); if (field->Int(inRecord) != field->IntNoData()) p->field->Int(outRecord,field->Int(inRecord)); }
						else
							{
							if(!CMmathEqualValues(field->Float(inRecord),field->FloatNoData()))
								p->field->Float(outRecord,field->Float(inRecord));
							}
						break;
					case MAX:
						if (field->Type() == DBTableFieldInt)
							{ p->setInt(); if (field->Int(inRecord) != field->IntNoData()) p->field->Int(outRecord,field->Int(inRecord)); }
						else
							{
							if(!CMmathEqualValues(field->Float(inRecord),field->FloatNoData()))
								p->field->Float(outRecord,field->Float(inRecord));
							}
						break;
					case SUM:
						if (field->Type() == DBTableFieldInt)
							{
							p->setInt();
							if (field->Int(inRecord) != field->IntNoData()) p->field->Int(outRecord,field->Int(inRecord));
							else p->field->Int(outRecord,0);
							}
						else
							{
							if(!CMmathEqualValues(field->Float(inRecord),field->FloatNoData())) p->field->Float(outRecord,field->Float(inRecord));
							else p->field->Float(outRecord,0.0);
							}
						break;
					case DEV:
					case PCT:
					case MED:
						p->tailVal = p->tailVal->next = new Values();
						p->tailVal->val = field->Float(inRecord);
						p->tailVal->next = 0;
						break;
					case MOD:
						p->tailVal->val = field->Float(inRecord);
						p->tailVal->occur = 1;
						p->tailVal = p->tailVal->next = new Values();
						break;
					}
				p = p->next;
				}
			}
		}

	for(int outRecID = 0; outRecID < outTable->ItemNum();++outRecID)
		{
		outRecord = outTable->Item(outRecID);
		p = head->next;
		FieldOptions *sum, *num;
		DBFloat mod;
		int occurrance;
		float i;
		bool mult;
		Values *cur;
		while(p)
			{
			field = fields->Item (p->getOldName());
			switch(p->getFunc())
				{
				case AVG:
					if((FLDExists(head,p->getOldName(),SUM,&sum)) && (FLDExists(head,p->getOldName(),NONNULL,&num)))
						p->field->Float(outRecord,sum->field->Float(outRecord) / (DBFloat) (num->field->Int(outRecord)));
					else fprintf(stderr,"Program Error! Could not find SUM or NONNULL in linked list! Please contact the GHAAS developers group!\n");
					break;
				case NAVG:
					if((FLDExists(head,p->getOldName(),SUM,&sum)) && (FLDExists(head,"NUM",NUM,&num)))
						p->field->Float(outRecord,sum->field->Float(outRecord) / (DBFloat) (num->field->Int(outRecord)));
					else fprintf(stderr,"Program Error! Could not find SUM or NUM in linked list! Please contact the GHAAS developers group!\n");
					break;
				case PCT:
				case MED:
					i = (float) (inTable->ItemNum() * p->getHead()->occur * 0.01) - 1;
					cur = p->getHead()->next;
					while(i > 0) { cur = cur->next; i--; }
					p->field->Float(outRecord,(cur->val + cur->next->val) / 2);
					break;
				case MOD:
					mod = 0.0;
					occurrance = 0;
					mult = false;
					cur = p->getHead();
					while(cur)
						{
						if(cur->occur > occurrance)
							{
							mod = cur->val;
							occurrance = cur->occur;
							mult = false;
							}
						else if(cur->occur == occurrance) mult = true;
						cur = cur->next;
						}
					if(mult) fprintf(stderr,"** Warning, multiple answers for MOD, listing first found!\n");
					p->field->Float(outRecord,mod);
					break;
				default:
					break;
				}
			p = p->next;
			}
		}

	p = head->next;
	while(p)
		{
		if(p->getFunc() == DEV)
			{
			FieldOptions *avg, *num;
			field = fields->Item (p->getOldName());
			if((FLDExists(head,p->getOldName(),AVG,&avg)) && (FLDExists(head,"NUM",NUM,&num)))
				{
				for(int outRecID = 0; outRecID < outTable->ItemNum();++outRecID)
					{
					outRecord = outTable->Item(outRecID);
					DBFloat sum = 0.0;
					Values *cur = p->getHead()->next;
					while(cur)
						{
//						sum += (DBFloat) (pow((cur->val - avg->field->Float(outRecord)),2));
						DBFloat add = (cur->val - avg->field->Float(outRecord));
						sum += (DBFloat) (add * add);
						cur = cur->next;
						}
					sum = sqrt(sum /(DBFloat) num->field->Int(outRecord));
					p->field->Float(outRecord,sum);
					}
				}
			else fprintf(stderr,"Program Error! Could not find AVG or NUM in linked list! Please contact the GHAAS developers group!\n");
			}
		p = p->next;
		}
// DELETE unnecessary fields which were for temp storage
	fields = outTable->Fields();
	p = head->next;
	while(p)
		{
		if(!p->getPrint()) outTable->DeleteField(fields->Item(p->getNewName()));
		p = p->next;
		}

	p = head->next;
	while(p)
		{
		if ((strcmp(p->field->Format(),DBHiddenField) == 0) && p->getPrint())
			{
			if(p->isInt())
				{
				int maxval = 0;
				for(int outRecID = 0; outRecID < outTable->ItemNum();++outRecID)
					{
					int val = p->field->Int(outTable->Item(outRecID));
					if (maxval < val) maxval = val; else if (maxval < -val) maxval = -val;
					}
				p->field->Format(DBMathIntAutoFormat(maxval));
				}
			else
				{
				float maxval = 0.0;
				for(int outRecID = 0; outRecID < outTable->ItemNum();++outRecID)
					{
					float val = p->field->Float(outTable->Item(outRecID));
					if (maxval < val) maxval = val; else if (maxval < -val) maxval = -val;
					}
				p->field->Format(DBMathFloatAutoFormat(maxval));
				}
			}
//		fprintf(stderr,"Format(%s)%d,%d: '%s'\n",p->getFuncName(),p->isInt(),p->getPrint(),p->field->Format());
		p = p->next;
		}

	DBObjectLIST<DBObjTableField> *list = new DBObjectLIST<DBObjTableField> ("Sorted Field List");
	for(i = 0; i < numGrpNames; i++) list->Add(groups[i]->dstFLD);
	outTable->ListSort(list);

if (ascii) DBExportASCIITable(outTable,outFile); else outData->Write(outFile);

/* CLEANUP ************************************************/
	if(outFile != stdout) fclose(outFile);
	for(i = 0; i < numGrpNames; i++) delete groups[i]->dstFLD;
	delete groups;
	p = head;
	while(p)
		{
		head = head->next;
		delete p;
		p = head;
		}
	return(DBSuccess);

}
