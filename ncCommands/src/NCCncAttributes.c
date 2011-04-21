#include <cm.h>
#include <NC.h>
void doHelp(bool extend,char *progName);

typedef struct VarNode_s
{
	char *var, *dat, *dat2, *attrib;
	bool text;
	struct VarNode_s *next;
} VarNode_t;

int main (int argc,char *argv [])
{
	int argPos, argNum = argc, ncid, varid;
	double range[2];
	char *title = NULL, *type = NULL, *domain = NULL, *subject = NULL, *ref = NULL, *inst = NULL, *source = NULL, *comments = NULL;
	VarNode_t *head, *last;
	if((last = head = malloc(sizeof(VarNode_t))) == (VarNode_t *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return(NCfailed); }
	for (argPos = 1;argPos < argNum;)
	{
		if (NCcmArgTest(argv[argPos],"-h","--help"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum))
				{ if((strcmp(argv[argPos],"extend") == 0) || (strcmp(argv[argPos],"e") == 0)) doHelp(1,NCcmProgName(argv[0])); }
			doHelp(0,NCcmProgName(argv[0]));
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-l","--longname"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCcmArgCheck(argv,argPos,argNum)) || (NCcmArgCheck(argv,argPos + 1,argNum)))
				{ CMmsgPrint (CMmsgUsrError, "Missing Long Name!"); return (NCfailed); }
			last->text = true;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCnameVALongName;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return(NCfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-n","--standardname"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCcmArgCheck(argv,argPos,argNum)) || (NCcmArgCheck(argv,argPos + 1,argNum)))
				{ CMmsgPrint (CMmsgUsrError, "Missing Standard Name!"); return (NCfailed); }
			last->text = true;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCnameVAStandardName;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d"); return(NCfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-U","--units"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCcmArgCheck(argv,argPos,argNum)) || (NCcmArgCheck(argv,argPos + 1,argNum)))
				{ CMmsgPrint (CMmsgUsrError, "Missing Units!"); return (NCfailed); }
			last->text = true;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCnameVAUnits;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return(NCfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-v","--validrange"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCcmArgCheck(argv,argPos,argNum)) ||
				 (NCcmArgCheck(argv,argPos + 1,argNum)) ||
				 (NCcmArgCheck(argv,argPos + 2, argNum)))
				{ CMmsgPrint (CMmsgUsrError, "Missing Ranges!"); return (NCfailed); }
// ********************        CHECK HERE!!! ************************
			last->text = false;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->dat2 = argv[argPos + 2];
			last->attrib = NCnameVAValidRange;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return(NCfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-o","--offset"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCcmArgCheck(argv,argPos,argNum)) ||
				 (NCcmArgCheck(argv,argPos + 1,argNum)))
				{ CMmsgPrint (CMmsgUsrError, "Missing offset!"); return (NCfailed); }
			last->text = false;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCnameVAAddOffset;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return(NCfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-s","--scalefactor"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCcmArgCheck(argv,argPos,argNum)) ||
				 (NCcmArgCheck(argv,argPos + 1,argNum)))
				{ CMmsgPrint (CMmsgUsrError,"Missing scale factor!"); return (NCfailed); }
			last->text = false;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCnameVAScaleFactor;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return(NCfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-t","--title"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { CMmsgPrint (CMmsgUsrError, "Missing Title!"); return (NCfailed); }
			title = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-y","--type"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { CMmsgPrint (CMmsgUsrError, "Missing Type!"); return (NCfailed); }
			type = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-d","--domain"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { CMmsgPrint (CMmsgUsrError, "Missing Domain!"); return (NCfailed); }
			domain = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-s","--subject"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { CMmsgPrint (CMmsgUsrError, "Missing Subject!"); return (NCfailed); }
			subject = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-r","--references"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { CMmsgPrint (CMmsgUsrError, "Missing References!"); return (NCfailed); }
			ref = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-i","--institution"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { CMmsgPrint (CMmsgUsrError, "Missing Institution!"); return (NCfailed); }
			inst = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-s","--source"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { CMmsgPrint (CMmsgUsrError, "Missing Source!"); return (NCfailed); }
			source = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-c","--comments"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { CMmsgPrint (CMmsgUsrError, "Missing Comment!"); return (NCfailed); }
			comments = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError, "Unknown option: %s!",argv[argPos]); return (NCfailed); }
	argPos++;
	}
	last->next = (VarNode_t *) NULL;
	if ((argNum > 1) && (strcmp(argv[1],"-") != 0)) {
		if(nc_open(argv[1],NC_WRITE,&ncid) != NC_NOERR) { CMmsgPrint (CMmsgUsrError, "Error opening file: %s!",argv[1]); return (NCfailed); }
	} else doHelp(0,NCcmProgName(argv[0]));
	if(nc_redef(ncid) != NC_NOERR) { CMmsgPrint (CMmsgUsrError, "Cannot place into redef mode!"); return (NCfailed); }

	last = head;
	while(last->next != (VarNode_t *) NULL)
	{
		if(nc_inq_varid(ncid,last->var,&varid) != NC_NOERR) { CMmsgPrint (CMmsgUsrError, "Error finding %s variable",last->var); return (NCfailed); }
		if(last->text)
		{
			if(nc_put_att_text(ncid, varid, last->attrib,strlen(last->dat), last->dat) != NC_NOERR)
				{ CMmsgPrint (CMmsgUsrError, "Error changing attribute: %s!", last->attrib); return (NCfailed); }
		}
	  	else
		{
			if(last->dat2 != (char *) NULL)
				{
				range[0] = atof(last->dat);
				range[1] = atof(last->dat2);
				if(nc_put_att_double(ncid, varid, last->attrib,NC_DOUBLE, 2, range) != NC_NOERR)
					{ CMmsgPrint (CMmsgUsrError, "Error changing attribute: %s!", last->attrib); return (NCfailed); }
				}
			else
				{
				range[0] = atof(last->dat);
				if(nc_put_att_double(ncid, varid, last->attrib,NC_DOUBLE, 1, range) != NC_NOERR)
					{ CMmsgPrint (CMmsgUsrError, "Error changing attribute: %s!", last->attrib); return (NCfailed); }
				}
		}
		last = last->next;
	}
	
	if((title != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGATitle,strlen(title), title) != NC_NOERR))
		{ CMmsgPrint (CMmsgUsrError, "Error changing attribute: title!"); return (NCfailed); }
	if(type != NULL) if(nc_put_att_text(ncid, NC_GLOBAL, NCnameGADataType,strlen(type), type) != NC_NOERR)
		{ CMmsgPrint (CMmsgUsrError, "Error changing attribute; type!"); return (NCfailed); }
	if((domain != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGADomain, strlen(domain), domain) != NC_NOERR))
		{ CMmsgPrint (CMmsgUsrError, "Error changing attribute: domain!"); return (NCfailed); }
	if((subject != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGASubject, strlen(subject), subject) != NC_NOERR))
		{ CMmsgPrint (CMmsgUsrError, "Error changing attribute: subject!"); return (NCfailed); }
	if((ref != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGAReferences, strlen(ref), ref) != NC_NOERR))
		{ CMmsgPrint (CMmsgUsrError, "Error changing attribute: references!"); return (NCfailed); }
	if((inst != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGAInstitution, strlen(inst), inst) != NC_NOERR))
		{ CMmsgPrint (CMmsgUsrError, "Error changing attribute: institution!"); return (NCfailed); }
	if((source != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGASource, strlen(source), source) != NC_NOERR))
		{ CMmsgPrint (CMmsgUsrError, "Error changing attribute: source!"); return (NCfailed); }
	if((comments != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGAComments, strlen(comments), comments) != NC_NOERR))
		{ CMmsgPrint (CMmsgUsrError, "Error changing attribute: comments!"); return (NCfailed); }

	if(nc_close(ncid) != NC_NOERR) { CMmsgPrint (CMmsgUsrError, "Error commiting changes to file!"); return (NCfailed); }
	return 0;
}

void doHelp(bool extend, char *progName) {
	if(extend) {
		CMmsgPrint (CMmsgUsrError, "Usage: %s [options] <filename>",progName);
		CMmsgPrint (CMmsgUsrError, "Variable modifications:");
		CMmsgPrint (CMmsgUsrError, " -l,--longname     [varname] [long_name]               => Change the extended name");
		CMmsgPrint (CMmsgUsrError, " -n,--standardname [varname] [standard_name]           => Change the short name");
		CMmsgPrint (CMmsgUsrError, " -U,--units        [varname] [units]                   => Change the units");
		CMmsgPrint (CMmsgUsrError, " -v,--validrange   [varname] [valid_low] [valid_high]  => Change valid range of the variable");
		CMmsgPrint (CMmsgUsrError, " -o,--offset       [varname] [offset]                  => Change offset of the variable");
		CMmsgPrint (CMmsgUsrError, " -s,--scalefactor  [varname] [scale_factor]            => Change scale factor of the variable");
		CMmsgPrint (CMmsgUsrError, "Attribute modifications:");
		CMmsgPrint (CMmsgUsrError, " -t,--title        [title]         => Change the title of the NetCDF file");
		CMmsgPrint (CMmsgUsrError, " -y,--datatype     [datatype]      => Change the datatype");
		CMmsgPrint (CMmsgUsrError, " -d,--domain       [domain]        => Change the domain");
		CMmsgPrint (CMmsgUsrError, " -u,--subject      [subject]       => Change the subject");
		CMmsgPrint (CMmsgUsrError, " -r,--references   [references]    => Change the references");
		CMmsgPrint (CMmsgUsrError, " -i,--institute    [institution]   => Change the institute");
		CMmsgPrint (CMmsgUsrError, " -s,--source       [source]        => Change the source");
		CMmsgPrint (CMmsgUsrError, " -c,--comments     [comments]      => Change the comments");
	} else {
		CMmsgPrint (CMmsgUsrError, "Usage: %s [options] <filename>",progName);
		CMmsgPrint (CMmsgUsrError, "Variable modifications:");
		CMmsgPrint (CMmsgUsrError, " -l,--longname     [varname] [long_name]");
		CMmsgPrint (CMmsgUsrError, " -n,--standardname [varname] [standard_name]");
		CMmsgPrint (CMmsgUsrError, " -U,--units        [varname] [units]");
		CMmsgPrint (CMmsgUsrError, " -v,--validrange   [varname] [valid_low] [valid_high]");
		CMmsgPrint (CMmsgUsrError, " -o,--offset       [varname] [offset]");
		CMmsgPrint (CMmsgUsrError, " -s,--scalefactor  [varname] [scale_factor]");
		CMmsgPrint (CMmsgUsrError, "Attribute modifications:");
		CMmsgPrint (CMmsgUsrError, " -t,--title        [title]");
		CMmsgPrint (CMmsgUsrError, " -y,--datatype     [datatype]");
		CMmsgPrint (CMmsgUsrError, " -d,--domain       [domain]");
		CMmsgPrint (CMmsgUsrError, " -u,--subject      [subject]");
		CMmsgPrint (CMmsgUsrError, " -r,--references   [references]");
		CMmsgPrint (CMmsgUsrError, " -i,--institute    [institution]");
		CMmsgPrint (CMmsgUsrError, " -s,--source       [source]");
		CMmsgPrint (CMmsgUsrError, " -c,--comments     [comments]");
	}
	exit(0);
}
