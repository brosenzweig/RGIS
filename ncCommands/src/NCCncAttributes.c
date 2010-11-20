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
		{ perror("Memory allocation error in: ncgAttributes()\n"); return(NCGfailed); }
	for (argPos = 1;argPos < argNum;)
	{
		if (NCGcmArgTest(argv[argPos],"-h","--help"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCGcmArgCheck(argv,argPos,argNum))
				{ if((strcmp(argv[argPos],"extend") == 0) || (strcmp(argv[argPos],"e") == 0)) doHelp(1,NCGcmProgName(argv[0])); }
			doHelp(0,NCGcmProgName(argv[0]));
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-l","--longname"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCGcmArgCheck(argv,argPos,argNum)) || (NCGcmArgCheck(argv,argPos + 1,argNum)))
				{ fprintf (stderr,"Missing Long Name!\n"); return (NCGfailed); }
			last->text = true;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCGnameVALongName;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ perror("Memory allocation error in: ncgAttributes()\n"); return(NCGfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-n","--standardname"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCGcmArgCheck(argv,argPos,argNum)) || (NCGcmArgCheck(argv,argPos + 1,argNum)))
				{ fprintf (stderr,"Missing Standard Name!\n"); return (NCGfailed); }
			last->text = true;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCGnameVAStandardName;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ perror("Memory allocation error in: ncgAttributes()\n"); return(NCGfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-U","--units"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCGcmArgCheck(argv,argPos,argNum)) || (NCGcmArgCheck(argv,argPos + 1,argNum)))
				{ fprintf (stderr,"Missing Units!\n"); return (NCGfailed); }
			last->text = true;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCGnameVAUnits;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ perror("Memory allocation error in: ncgAttributes()"); return(NCGfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-v","--validrange"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCGcmArgCheck(argv,argPos,argNum)) ||
				 (NCGcmArgCheck(argv,argPos + 1,argNum)) ||
				 (NCGcmArgCheck(argv,argPos + 2, argNum)))
				{ fprintf (stderr,"Missing Ranges!\n"); return (NCGfailed); }
// ********************        CHECK HERE!!! ************************
			last->text = false;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->dat2 = argv[argPos + 2];
			last->attrib = NCGnameVAValidRange;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ perror("Memory allocation error in: ncgAttributes()"); return(NCGfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-o","--offset"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCGcmArgCheck(argv,argPos,argNum)) ||
				 (NCGcmArgCheck(argv,argPos + 1,argNum)))
				{ fprintf (stderr,"Missing offset!\n"); return (NCGfailed); }
			last->text = false;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCGnameVAAddOffset;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ perror("Memory allocation error in: ncgAttributes()"); return(NCGfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-s","--scalefactor"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCGcmArgCheck(argv,argPos,argNum)) ||
				 (NCGcmArgCheck(argv,argPos + 1,argNum)))
				{ fprintf (stderr,"Missing scale factor\n"); return (NCGfailed); }
			last->text = false;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCGnameVAScaleFactor;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ perror("Memory allocation error in: ncgAttributes()"); return(NCGfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-t","--title"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCGcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Title!\n"); return (NCGfailed); }
			title = argv[argPos];
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-y","--type"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCGcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Type!\n"); return (NCGfailed); }
			type = argv[argPos];
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-d","--domain"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCGcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Domain!\n"); return (NCGfailed); }
			domain = argv[argPos];
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-s","--subject"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCGcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Subject!\n"); return (NCGfailed); }
			subject = argv[argPos];
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-r","--references"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCGcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing References!\n"); return (NCGfailed); }
			ref = argv[argPos];
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-i","--institution"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCGcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Institution!\n"); return (NCGfailed); }
			inst = argv[argPos];
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-s","--source"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCGcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Source!\n"); return (NCGfailed); }
			source = argv[argPos];
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-c","--comments"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCGcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Comment!\n"); return (NCGfailed); }
			comments = argv[argPos];
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ fprintf(stderr,"Unknown option: %s!\n",argv[argPos]); return (NCGfailed); }
	argPos++;
	}
	last->next = (VarNode_t *) NULL;
	if ((argNum > 1) && (strcmp(argv[1],"-") != 0)) {
		if(nc_open(argv[1],NC_WRITE,&ncid) != NC_NOERR) { fprintf(stderr,"Error opening file: %s!\n",argv[1]); return (NCGfailed); }
	} else doHelp(0,NCGcmProgName(argv[0]));
	if(nc_redef(ncid) != NC_NOERR) { fprintf(stderr,"Cannot place into redef mode!\n"); return (NCGfailed); }

	last = head;
	while(last->next != (VarNode_t *) NULL)
	{
		if(nc_inq_varid(ncid,last->var,&varid) != NC_NOERR) { fprintf(stderr,"Error finding %s variable",last->var); return (NCGfailed); }
		if(last->text)
		{
			if(nc_put_att_text(ncid, varid, last->attrib,strlen(last->dat), last->dat) != NC_NOERR)
				{ fprintf(stderr,"Error changing attribute: %s!\n", last->attrib); return (NCGfailed); }
		}
	  	else
		{
			if(last->dat2 != (char *) NULL)
				{
				range[0] = atof(last->dat);
				range[1] = atof(last->dat2);
				if(nc_put_att_double(ncid, varid, last->attrib,NC_DOUBLE, 2, range) != NC_NOERR)
					{ fprintf(stderr,"Error changing attribute: %s!\n", last->attrib); return (NCGfailed); }
				}
			else
				{
				range[0] = atof(last->dat);
				if(nc_put_att_double(ncid, varid, last->attrib,NC_DOUBLE, 1, range) != NC_NOERR)
					{ fprintf(stderr,"Error changing attribute: %s!\n", last->attrib); return (NCGfailed); }
				}
		}
		last = last->next;
	}
	
	if((title != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCGnameGATitle,strlen(title), title) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: title!\n"); return (NCGfailed); }
	if(type != NULL) if(nc_put_att_text(ncid, NC_GLOBAL, NCGnameGADataType,strlen(type), type) != NC_NOERR)
		{ fprintf(stderr,"Error changing attribute; type!\n"); return (NCGfailed); }
	if((domain != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCGnameGADomain, strlen(domain), domain) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: domain!\n"); return (NCGfailed); }
	if((subject != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCGnameGASubject, strlen(subject), subject) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: subject!\n"); return (NCGfailed); }
	if((ref != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCGnameGAReferences, strlen(ref), ref) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: references!\n"); return (NCGfailed); }
	if((inst != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCGnameGAInstitution, strlen(inst), inst) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: institution!\n"); return (NCGfailed); }
	if((source != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCGnameGASource, strlen(source), source) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: source!\n"); return (NCGfailed); }
	if((comments != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCGnameGAComments, strlen(comments), comments) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: comments!\n"); return (NCGfailed); }

	if(nc_close(ncid) != NC_NOERR) { fprintf(stderr,"Error commiting changes to file!\n"); return (NCGfailed); }
	return 0;
}

void doHelp(bool extend, char *progName) {
	if(extend) {
		fprintf(stderr,"Usage: %s [options] <filename>\n",progName);
		fprintf(stderr,"Variable modifications:\n");
		fprintf(stderr," -l,--longname     [varname] [long_name]               => Change the extended name\n");
		fprintf(stderr," -n,--standardname [varname] [standard_name]           => Change the short name\n");
		fprintf(stderr," -U,--units        [varname] [units]                   => Change the units\n");
		fprintf(stderr," -v,--validrange   [varname] [valid_low] [valid_high]  => Change valid range of the variable\n");
		fprintf(stderr," -o,--offset       [varname] [offset]                  => Change offset of the variable\n");
		fprintf(stderr," -s,--scalefactor  [varname] [scale_factor]            => Change scale factor of the variable\n");
		fprintf(stderr,"Attribute modifications:\n");
		fprintf(stderr," -t,--title        [title]         => Change the title of the NetCDF file\n");
		fprintf(stderr," -y,--datatype     [datatype]      => Change the datatype\n");
		fprintf(stderr," -d,--domain       [domain]        => Change the domain\n");
		fprintf(stderr," -u,--subject      [subject]       => Change the subject\n");
		fprintf(stderr," -r,--references   [references]    => Change the references\n");
		fprintf(stderr," -i,--institute    [institution]   => Change the institute\n");
		fprintf(stderr," -s,--source       [source]        => Change the source\n");
		fprintf(stderr," -c,--comments     [comments]      => Change the comments\n");
	} else {
		fprintf(stderr,"Usage: %s [options] <filename>\n",progName);
		fprintf(stderr,"Variable modifications:\n");
		fprintf(stderr," -l,--longname     [varname] [long_name]\n");
		fprintf(stderr," -n,--standardname [varname] [standard_name]\n");
		fprintf(stderr," -U,--units        [varname] [units]\n");
		fprintf(stderr," -v,--validrange   [varname] [valid_low] [valid_high]\n");
		fprintf(stderr," -o,--offset       [varname] [offset]\n");
		fprintf(stderr," -s,--scalefactor  [varname] [scale_factor]\n");
		fprintf(stderr,"Attribute modifications:\n");
		fprintf(stderr," -t,--title        [title]\n");
		fprintf(stderr," -y,--datatype     [datatype]\n");
		fprintf(stderr," -d,--domain       [domain]\n");
		fprintf(stderr," -u,--subject      [subject]\n");
		fprintf(stderr," -r,--references   [references]\n");
		fprintf(stderr," -i,--institute    [institution]\n");
		fprintf(stderr," -s,--source       [source]\n");
		fprintf(stderr," -c,--comments     [comments]\n");
	}
	exit(0);
}
