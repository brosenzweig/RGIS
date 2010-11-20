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
		{ perror("Memory allocation error in: ncgAttributes()\n"); return(NCfailed); }
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
				{ fprintf (stderr,"Missing Long Name!\n"); return (NCfailed); }
			last->text = true;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCnameVALongName;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ perror("Memory allocation error in: ncgAttributes()\n"); return(NCfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-n","--standardname"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCcmArgCheck(argv,argPos,argNum)) || (NCcmArgCheck(argv,argPos + 1,argNum)))
				{ fprintf (stderr,"Missing Standard Name!\n"); return (NCfailed); }
			last->text = true;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCnameVAStandardName;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ perror("Memory allocation error in: ncgAttributes()\n"); return(NCfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-U","--units"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if ((NCcmArgCheck(argv,argPos,argNum)) || (NCcmArgCheck(argv,argPos + 1,argNum)))
				{ fprintf (stderr,"Missing Units!\n"); return (NCfailed); }
			last->text = true;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCnameVAUnits;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ perror("Memory allocation error in: ncgAttributes()"); return(NCfailed); }
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
				{ fprintf (stderr,"Missing Ranges!\n"); return (NCfailed); }
// ********************        CHECK HERE!!! ************************
			last->text = false;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->dat2 = argv[argPos + 2];
			last->attrib = NCnameVAValidRange;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ perror("Memory allocation error in: ncgAttributes()"); return(NCfailed); }
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
				{ fprintf (stderr,"Missing offset!\n"); return (NCfailed); }
			last->text = false;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCnameVAAddOffset;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ perror("Memory allocation error in: ncgAttributes()"); return(NCfailed); }
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
				{ fprintf (stderr,"Missing scale factor\n"); return (NCfailed); }
			last->text = false;
			last->var = argv[argPos];
			last->dat = argv[argPos + 1];
			last->attrib = NCnameVAScaleFactor;
			if((last = last->next = malloc(sizeof(VarNode_t))) == NULL)
				{ perror("Memory allocation error in: ncgAttributes()"); return(NCfailed); }
			last->dat = last->dat2 = (char *) NULL;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-t","--title"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Title!\n"); return (NCfailed); }
			title = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-y","--type"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Type!\n"); return (NCfailed); }
			type = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-d","--domain"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Domain!\n"); return (NCfailed); }
			domain = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-s","--subject"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Subject!\n"); return (NCfailed); }
			subject = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-r","--references"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing References!\n"); return (NCfailed); }
			ref = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-i","--institution"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Institution!\n"); return (NCfailed); }
			inst = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-s","--source"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Source!\n"); return (NCfailed); }
			source = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-c","--comments"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing Comment!\n"); return (NCfailed); }
			comments = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ fprintf(stderr,"Unknown option: %s!\n",argv[argPos]); return (NCfailed); }
	argPos++;
	}
	last->next = (VarNode_t *) NULL;
	if ((argNum > 1) && (strcmp(argv[1],"-") != 0)) {
		if(nc_open(argv[1],NC_WRITE,&ncid) != NC_NOERR) { fprintf(stderr,"Error opening file: %s!\n",argv[1]); return (NCfailed); }
	} else doHelp(0,NCcmProgName(argv[0]));
	if(nc_redef(ncid) != NC_NOERR) { fprintf(stderr,"Cannot place into redef mode!\n"); return (NCfailed); }

	last = head;
	while(last->next != (VarNode_t *) NULL)
	{
		if(nc_inq_varid(ncid,last->var,&varid) != NC_NOERR) { fprintf(stderr,"Error finding %s variable",last->var); return (NCfailed); }
		if(last->text)
		{
			if(nc_put_att_text(ncid, varid, last->attrib,strlen(last->dat), last->dat) != NC_NOERR)
				{ fprintf(stderr,"Error changing attribute: %s!\n", last->attrib); return (NCfailed); }
		}
	  	else
		{
			if(last->dat2 != (char *) NULL)
				{
				range[0] = atof(last->dat);
				range[1] = atof(last->dat2);
				if(nc_put_att_double(ncid, varid, last->attrib,NC_DOUBLE, 2, range) != NC_NOERR)
					{ fprintf(stderr,"Error changing attribute: %s!\n", last->attrib); return (NCfailed); }
				}
			else
				{
				range[0] = atof(last->dat);
				if(nc_put_att_double(ncid, varid, last->attrib,NC_DOUBLE, 1, range) != NC_NOERR)
					{ fprintf(stderr,"Error changing attribute: %s!\n", last->attrib); return (NCfailed); }
				}
		}
		last = last->next;
	}
	
	if((title != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGATitle,strlen(title), title) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: title!\n"); return (NCfailed); }
	if(type != NULL) if(nc_put_att_text(ncid, NC_GLOBAL, NCnameGADataType,strlen(type), type) != NC_NOERR)
		{ fprintf(stderr,"Error changing attribute; type!\n"); return (NCfailed); }
	if((domain != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGADomain, strlen(domain), domain) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: domain!\n"); return (NCfailed); }
	if((subject != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGASubject, strlen(subject), subject) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: subject!\n"); return (NCfailed); }
	if((ref != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGAReferences, strlen(ref), ref) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: references!\n"); return (NCfailed); }
	if((inst != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGAInstitution, strlen(inst), inst) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: institution!\n"); return (NCfailed); }
	if((source != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGASource, strlen(source), source) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: source!\n"); return (NCfailed); }
	if((comments != NULL) && (nc_put_att_text(ncid, NC_GLOBAL, NCnameGAComments, strlen(comments), comments) != NC_NOERR))
		{ fprintf(stderr,"Error changing attribute: comments!\n"); return (NCfailed); }

	if(nc_close(ncid) != NC_NOERR) { fprintf(stderr,"Error commiting changes to file!\n"); return (NCfailed); }
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
