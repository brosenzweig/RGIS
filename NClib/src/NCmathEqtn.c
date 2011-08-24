#include <string.h>
#include <cm.h>
#include <NCmathEqtn.h>
#include <NCstdlib.h>
#include <NCstring.h>

// *** GLOBAL VARIABLES

// Don't forget to change numFunc in the NCmathEqtn.h if you add/remove items from this array.
static char *funcNames [] = { "abs", "sin", "asin", "cos", "acos", "tan", "atan", "ln", "log", "floor", "ceil", (char *) NULL };
static double (*functions [])(double) = { fabs, sin, asin, cos, acos, tan, atan, log, log10, floor, ceil, NULL };
static bool lisp = false;

Variable_t *_Vars = (Variable_t *) NULL;
int _VarNum = 0;

// *** PRIVATE FUNCTIONS

static Equal getIneq(char expr[], int *i)
{
	register int s = (*i == 0) ? 1 : *i;
	int strLen = strlen(expr);
	if((expr[s] == '(') || (expr[s] == '[') || (expr[s] == '{')) s = NCstringEndPar(expr,s) + 1;
	while(s+1 <= strLen)
	{
		if ((expr[s-1] == ' ') && (expr[s+1] == ' '))
		{
			if(expr[s] == '<') { *i = s; return LT; }
			if(expr[s] == '>') { *i = s; return GT; }
		}
		s++;
		if((expr[s] == '(') || (expr[s] == '[') || (expr[s] == '{')) s = NCstringEndPar(expr,s) + 1;
	}
	s = (*i == 0) ? 1 : *i;
	if((expr[s] == '(') || (expr[s] == '[') || (expr[s] == '{')) s = NCstringEndPar(expr,s) + 1;
	while(s+2 <= strLen)
	{
		if ((expr[s-1] == ' ') && (expr[s+2] == ' '))
		{
			if((expr[s] == '&') && (expr[s+1] == '&')) { *i = s; return AND; }
			if((expr[s] == '|') && (expr[s+1] == '|')) { *i = s; return OR; }
			if((expr[s] == '<') && (expr[s+1] == '=')) { *i = s; return LTE; }
			if((expr[s] == '>') && (expr[s+1] == '=')) { *i = s; return GTE; }
			if((expr[s] == '!') && (expr[s+1] == '=')) { *i = s; return NE; }
			if((expr[s] == '<') && (expr[s+1] == '>')) { *i = s; return NE; }
			if((expr[s] == '=') && (expr[s+1] == '=')) { *i = s; return EQ; }
		}
		s++;
		if((expr[s] == '(') || (expr[s] == '[') || (expr[s] == '{')) s = NCstringEndPar(expr,s) + 1;
	}
	return NOEQ;
}

static Oper getOper(char expr[], int *i)
{
	register int s = (*i == 0) ? 1 : *i;
	int c = strlen(expr) - 1;
	if((expr[c] == ')') || (expr[c] == ']') || (expr[c] == '}')) c = NCstringEndPar(expr,c) - 2;
	while(c >= s)
	{
		if ((expr[c-1] == ' ') && (expr[c+1] == ' '))
		{ // Must check for Add/Sub first
			if(expr[c] == '+') { *i = c; return ADD; }
			if(expr[c] == '-') { *i = c; return SUB; }
		}
		c--;
		if((expr[c] == ')') || (expr[c] == ']') || (expr[c] == '}')) c = NCstringEndPar(expr,c) - 2;
	}
	c = strlen(expr) - 1;
	if((expr[c] == ')') || (expr[c] == ']') || (expr[c] == '}')) c = NCstringEndPar(expr,c) - 2;
	while(c >= s)
	{
		if ((expr[c-1] == ' ') && (expr[c+1] == ' '))
		{// then check for Mul/Div
			if(expr[c] == '*') { *i = c; return MUL; }
			if(expr[c] == '/') { *i = c; return DIV; }
			if(expr[c] == '%') { *i = c; return MOD; }
		}
		c--;
		if((expr[c] == ')') || (expr[c] == ']') || (expr[c] == '}')) c = NCstringEndPar(expr,c) - 2;
	}
	c = strlen(expr) - 1;
	if((expr[c] == ')') || (expr[c] == ']') || (expr[c] == '}')) c = NCstringEndPar(expr,c) - 2;
	while(c >= s)
	{
		if ((expr[c-1] == ' ') && (expr[c+1] == ' '))
		{// finally do Ecp
			if(expr[c] == '^') { *i = c; return EXP; }
		}
		c--;
		if((expr[c] == ')') || (expr[c] == ']') || (expr[c] == '}')) c = NCstringEndPar(expr,c) - 2;
	}
	return NOOP;
}

static double (*getFunc(char expr[], int *i))(double)
{ // returns a pointer to a double function(double)
	int strLen = strlen(expr), j;
	if((expr[*i] == '(') || (expr[*i] == '[') || (expr[*i] == '{')) *i = NCstringEndPar(expr,*i) + 2;
	while(*i < strLen)
	{
		for(j = 0; j < numFunc; j++) if(NCstringMatch(expr,*i,funcNames[j])) { *i += strlen(funcNames[j]) + 1; return functions[j]; }
		(*i)++;
		if((expr[*i] == '(') || (expr[*i] == '[') || (expr[*i] == '{')) *i = NCstringEndPar(expr,*i) + 2;
	}
	return functions[numFunc]; // NOFC
}

static bool findVar(char expr[], int *i) {
	for(*i = 0; *i < _VarNum; (*i)++) if(NCstringMatch(_Vars[*i].name,0,expr)) return true;
	return false;
}

// *** PUBLIC FUNCTIONS

int NCmathGetVarNum() { return _VarNum; }
int NCmathAddVar(int colnum,char *name,bool vary)
{
	if((_Vars = realloc(_Vars,(_VarNum + 1) * sizeof(Variable_t))) == (Variable_t *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory Allocation error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
	if((_Vars[_VarNum].name = (char *) malloc(sizeof(char) * (strlen(name) + 1))) == (char *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return (NCfailed); }
	else strcpy(_Vars[_VarNum].name,name);
	_Vars[_VarNum].colnum = colnum;
	_Vars[_VarNum].vary = vary;
	return _VarNum++;
}

int NCmathGetVarColNum(int varID) { return _Vars[varID].colnum; }

void NCmathSetVarVal(int varID, double value) { _Vars[varID].val = value; }
double NCmathGetVarVal(int varID) { return _Vars[varID].val; }

bool NCmathGetVarVary(int varID) { return _Vars[varID].vary; }

char *NCmathGetVarName(int varID) { return _Vars[varID].name; }

void NCmathFreeVars() { int i; for(i = 0; i < _VarNum; i++) free(_Vars[i].name); free(_Vars); }

bool isIneq(char expr[], int *i) { return getIneq(expr,i) != NOEQ; }

void setLisp() { lisp = true; }

void unsetLisp() { lisp = false; }

// Functions specific to trees

TreeNode_t* mkTree(char expr[])
{
	int i = 0;
  	int s = 0, strLen = 0;
	TreeNode_t *head;
	if(expr == (char *) NULL) { if(GetDebug()) CMmsgPrint (CMmsgUsrError, "mkTree(): Null string passed!"); return ((TreeNode_t *) NULL); }
	if((head = malloc(sizeof(TreeNode_t))) == (TreeNode_t *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return ((TreeNode_t *) NULL); }
	if(GetDebug()) { Dprint(stderr,"mkTree(): malloc(%p)",head); }

	while(NCstringStripch(&expr,' ') || NCstringStripbr(&expr));
	strLen = strlen(expr);
	if(GetDebug()) CMmsgPrint (CMmsgUsrError, "mkTree: expr='%s'",expr);
	if(expr[0] == '(')
	{
		i = NCstringEndPar(expr,i);
		s = i;
		if((head->oper = getOper(expr,&i)) != NOOP) {
			if(GetDebug()) CMmsgPrint (CMmsgUsrError, "mkTree(): PAR:i='%d' i-1='%c' i='%c' i+1='%c'",i,expr[i-1],expr[i],expr[i+1]);
			head->left = mkTree(NCstringSubstr(expr,0,i - 2));
			head->right = mkTree(NCstringSubstr(expr,i + 2, strLen - 1));
			head->type = OPER;
		} else
		{ 
			i = s;
			if((head->func = getFunc(expr,&i)) != functions[numFunc])
			{
				if(GetDebug()) CMmsgPrint (CMmsgUsrError, "mkTree(): PAR:i='%d' i-1='%c' i='%c' i+1='%c'",i,expr[i-1],expr[i],expr[i+1]);
				head->left = NULL;
				head->right = mkTree(NCstringSubstr(expr,i, NCstringEndPar(expr,i)));
				head->type = FUNC;
			} else if(findVar(expr,&i))
			{
				head->left = head->right = NULL;
				if(_Vars[i].vary) { head->type = VAR; head->var = &(_Vars[i].val); }
				else { head->type = CONST; head->cons = _Vars[i].val; }
			} else if(NCmathIsNumber(expr))
			{
				head->left = head->right = NULL;
				head->cons = atof(expr);
				head->type = CONST;
			} else { CMmsgPrint (CMmsgUsrError, "mkTree(): PAR:Unrecognized operator: strLen:%d i:%d '%s'",strLen,i,expr); abort(); }
		}
	} else {
		i = 0;
		if((head->oper = getOper(expr,&i)) != NOOP)
		{
			if(GetDebug()) CMmsgPrint (CMmsgUsrError, "mkTree(): NOPAR:i='%d' i-1='%c' i='%c' i+1='%c'",i,expr[i-1],expr[i],expr[i+1]);
			head->left = mkTree(NCstringSubstr(expr,0,i - 2));
			head->right = mkTree(NCstringSubstr(expr,i + 2, strLen - 1));
			head->type = OPER;
		} else
		{
			i = 0;
			if((head->func = getFunc(expr,&i)) != functions[numFunc])
			{
				if(GetDebug()) CMmsgPrint (CMmsgUsrError, "mkTree(): NOPAR:i='%d' i-1='%c' i='%c' i+1='%c'",i,expr[i-1],expr[i],expr[i+1]);
				head->left = NULL;
				head->right = mkTree(NCstringSubstr(expr,i, NCstringEndPar(expr,i)));
				head->type = FUNC;
			} else if(findVar(expr,&i))
			{
				head->left = head->right = NULL;
				if(_Vars[i].vary) { head->type = VAR; head->var = &(_Vars[i].val); }
				else { head->type = CONST; head->cons = _Vars[i].val; }
			} else if(NCmathIsNumber(expr))
			{
				if(GetDebug()) CMmsgPrint (CMmsgUsrError, "mkTree(): NOPAR:i='%d' i-1='%c' i='%c' i+1='%c'",i,expr[i-1],expr[i],expr[i+1]);
				head->left = head->right = NULL;
				head->cons = atof(expr);
				head->type = CONST;
			} else { CMmsgPrint (CMmsgUsrError, "mkTree(): NOPAR: Unrecognized operator: strLen:%d i:%d '%s'",strLen,i,expr); abort(); }
		}
	}
	if(GetDebug()) CMmsgPrint (CMmsgUsrError, "mkTree(): Passing back(%s): %p",expr,expr);
	free(expr);
	return head;
}

IneqNode_t* mkTreeI(char expr[])
{
	int i = 0, s, strLen = 0;
	IneqNode_t *head;
	char *temp;
	if(expr == (char *) NULL) { if(GetDebug()) CMmsgPrint (CMmsgUsrError, "mkTreeI(): Null string passed!"); return NULL; }
	if((head = malloc(sizeof(IneqNode_t))) == (IneqNode_t *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return ((IneqNode_t *) NULL); }
	if(GetDebug()) { Dprint(stderr,"mkTreeI(): malloc(%p)",head); }

	while(NCstringStripch(&expr, ' ') || NCstringStripbr(&expr));
	strLen = strlen(expr);
	if(GetDebug()) CMmsgPrint (CMmsgUsrError, "mkTreeI: expr='%s'",expr);
	if(expr[0] == '(') i = NCstringEndPar(expr,0);
	if((head->equal = getIneq(expr,&i)) != NOEQ)
	{
		if(GetDebug()) CMmsgPrint (CMmsgUsrError, "mkTreeI(): PAR:i='%d' i-1='%c' i='%c' i+1='%c'",i,expr[i-1],expr[i],expr[i+1]);
		if((temp = NCstringSubstr(expr, 0, i - 1)) == (char *) NULL)
			{ CMmsgPrint (CMmsgUsrError, "Problem encountered with NCstringSubstr(%s, %d, %d)!",expr,0,i - 1); return ((IneqNode_t *) NULL); }
		s = (expr[i+1] == ' ') ? i + 1 : i + 2;
		while(NCstringStripch(&temp, ' ') || NCstringStripbr(&temp));
		i = 0;
		if(getIneq(temp,&i) == NOEQ) { head->lTree = true; head->lhead = mkTree(temp);
		} else { head->lTree = false; head->left = mkTreeI(temp); }
		if((temp = NCstringSubstr(expr, s, strLen)) == (char *) NULL)
			{ CMmsgPrint (CMmsgUsrError, "Problem encountered with NCstringSubstr(%s, %d, %d)!",expr,s,strLen); return ((IneqNode_t *) NULL); }
		while(NCstringStripch(&temp, ' ') || NCstringStripbr(&temp));
		i = 0;
		if(getIneq(temp,&i) == NOEQ) { head->rTree = true; head->rhead = mkTree(temp);
		} else { head->rTree = false; head->right = mkTreeI(temp); }
	}

	if(GetDebug()) CMmsgPrint (CMmsgUsrError, "mkTreeI(): Passing back(%s): %p",expr,expr);
	free(expr);
	return head;
}

void printInorder(TreeNode_t *s, FILE *out)
{
	register int i;
	if(s != NULL)
	{
		if((lisp) && (s->type == OPER)) fprintf(out,"(");
		printInorder(s->left,out);
		switch (s->type)
		{
			case CONST:
				fprintf(out,"%f",s->cons);
				break;
			case OPER:
				switch(s->oper) {
					case ADD: fprintf(out," + "); break;
					case SUB: fprintf(out," - "); break;
					case MUL: fprintf(out," * "); break;
					case DIV: fprintf(out," / "); break;
					case MOD: fprintf(out," %% "); break;
					case EXP: fprintf(out," ^ "); break;
					default: fprintf(out,"BADOPER"); break;
				}
				break;
			case VAR:
				fprintf(out,"%f",*(s->var)); // <<<<=======  this is where the table lookup happens
				break;
			case FUNC:
				for(i = 0; i < numFunc; i++) if(s->func == functions[i]) fprintf(out,"%s(",funcNames[i]);
				if(s->func == functions[numFunc]) fprintf(out,"BADFUNC!");
				break;
			default:
				fprintf(out,"BADTP");
				break;
		}
		printInorder(s->right,out);
		if(s->type == FUNC) fprintf(out,")");
		if((lisp) && (s->type == OPER)) fprintf(out,")");
	}
}

void printInorderI(IneqNode_t *s, FILE *out)
{
	if(s != NULL)
	{
		if(lisp) fprintf(out,"(");
		switch(s->equal)
		{
			case AND:
				if(s->lTree) printInorder(s->lhead,out); else printInorderI(s->left,out);
				fprintf(out," && ");
				if(s->rTree) printInorder(s->rhead,out); else printInorderI(s->right,out);
				break;
			case OR:
				if(s->lTree) printInorder(s->lhead,out); else printInorderI(s->left,out);
				fprintf(out," || ");
				if(s->rTree) printInorder(s->rhead,out); else printInorderI(s->right,out);
				break;
			case LT:
				if(s->lTree) printInorder(s->lhead,out); else printInorderI(s->left,out);
				fprintf(out," < ");
				if(s->rTree) printInorder(s->rhead,out); else printInorderI(s->right,out);
				break;
			case LTE:
				if(s->lTree) printInorder(s->lhead,out); else printInorderI(s->left,out);
				fprintf(out," <= ");
				if(s->rTree) printInorder(s->rhead,out); else printInorderI(s->right,out);
				break;
			case EQ:
				if(s->lTree) printInorder(s->lhead,out); else printInorderI(s->left,out);
				fprintf(out," == ");
				if(s->rTree) printInorder(s->rhead,out); else printInorderI(s->right,out);
				break;
			case NE:
				if(s->lTree) printInorder(s->lhead,out); else printInorderI(s->left,out);
				fprintf(out," != ");
				if(s->rTree) printInorder(s->rhead,out); else printInorderI(s->right,out);
				break;
			case GTE:
				if(s->lTree) printInorder(s->lhead,out); else printInorderI(s->left,out);
				fprintf(out," >= ");
				if(s->rTree) printInorder(s->rhead,out); else printInorderI(s->right,out);
				break;
			case GT:
				if(s->lTree) printInorder(s->lhead,out); else printInorderI(s->left,out);
				fprintf(out," > ");
				if(s->rTree) printInorder(s->rhead,out); else printInorderI(s->right,out);
				break;
			default:
				fprintf(out,"BADINEQ");
				break;
		}
		if(lisp) fprintf(out,")");
	}
}

void NCmathEqtnFixTreeI(IneqNode_t *s) {
	if(s != NULL)
	{
		if(s->lTree) {
			NCmathEqtnFixTree(&(s->lhead));
			if(s->rTree) {
				NCmathEqtnFixTree(&(s->rhead));
				if((Calculate(s->lhead) != FLOAT_NOVALUE) && (Calculate(s->rhead) != FLOAT_NOVALUE)) {
					s->equal = CalculateI(s);
					delTree(s->lhead);
					delTree(s->rhead);
					s->lhead = s->lhead = (TreeNode_t *) NULL;
				}
			} else {
				if((Calculate(s->lhead) != FLOAT_NOVALUE) && ((s->right->equal == TRUE) || (s->right->equal == FALSE))) {
					s->equal = CalculateI(s);
					delTree(s->lhead); s->lhead = (TreeNode_t *) NULL;
					delTreeI(s->right); s->right = (IneqNode_t *) NULL;
				}
			}
		} else {
			if(s->rTree) {
				NCmathEqtnFixTree(&(s->rhead));
				if((Calculate(s->rhead) != FLOAT_NOVALUE) && ((s->left->equal == TRUE) || (s->left->equal == FALSE))) {
					s->equal = CalculateI(s);
					delTreeI(s->left); s->left = (IneqNode_t *) NULL;
					delTree(s->rhead); s->rhead = (TreeNode_t *) NULL;
				}
			} else {
				if(((s->left->equal == TRUE) || (s->left->equal == FALSE)) &&
					((s->right->equal == TRUE) || (s->right->equal == FALSE))) {
					s->equal = CalculateI(s);
					delTreeI(s->left);
					delTreeI(s->right);
				}
			}
		}
	}
}

void NCmathEqtnFixTree(TreeNode_t **s) {
	float result;
	TreeNode_t *prev = (TreeNode_t *) NULL;
	if(*s != (TreeNode_t *) NULL) {
		if((result = Calculate(*s)) == FLOAT_NOVALUE) {
			NCmathEqtnFixTree(&((*s)->left));
			NCmathEqtnFixTree(&((*s)->right));
		} else {
			prev = *s;
			if((*s = malloc(sizeof(TreeNode_t))) == (TreeNode_t *) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); *s = ((TreeNode_t *) NULL); }
			if(GetDebug()) { Dprint(stderr,"NCmathEqtnFixTree(): malloc(%p)",s); }
			(*s)->type = CONST;
			(*s)->cons = result;
			(*s)->left = (*s)->right = (TreeNode_t *) NULL;
			delTree(prev);
		}
	}
}

double Calculate(TreeNode_t *s)
{
	register double right = 0.0, left = 0.0;
	if(s != NULL)
	{
		switch (s->type)
		{
			case CONST:
				if(GetDebug()) { fprintf(stderr,"%f [const] = ",s->cons); printInorder(s,stderr); }
				return s->cons;
			case OPER:
				left = Calculate(s->left);
				right = Calculate(s->right);
				if(NCmathEqualValues(right,FLOAT_NOVALUE) ||
					NCmathEqualValues(left,FLOAT_NOVALUE)) return FLOAT_NOVALUE;
				switch(s->oper)
				{
					case ADD:
						if(GetDebug()) { fprintf(stderr,"%f [oper] = ",left + right); printInorder(s,stderr); }
						return left + right;
					case SUB:
						if(GetDebug()) { fprintf(stderr,"%f [oper] = ",left - right); printInorder(s,stderr);}
						return left - right;
					case MUL:
						if(GetDebug()) { fprintf(stderr,"%f [oper] = ",left * right); printInorder(s,stderr);}
						return left * right;
					case DIV:
						if(GetDebug()) { fprintf(stderr,"%f [oper] = ",left / right); printInorder(s,stderr);}
						return left / right;
					case MOD:
						if(NCmathEqualValues(right,(double) ((int) right)) &&
							NCmathEqualValues(left,(double) ((int) left)))
						{
							if(GetDebug()) { fprintf(stderr,"%i [oper] = ",(int) left % (int) right); printInorder(s,stderr);}
							return (int) left % (int) right;
						}
						fprintf(stderr,"Can't do %f %% %f",left,right);
						abort();
					case EXP:
						if(GetDebug()) { fprintf(stderr,"%f [oper] = ",pow(Calculate(s->left),Calculate(s->right)) ); printInorder(s,stderr);}
						return pow(Calculate(s->left), Calculate(s->right));
					default: if(GetDebug()) fprintf(stderr,"BADOPER"); return FLOAT_NOVALUE;
				}
				break;
			case VAR:
				if(GetDebug()) { CMmsgPrint (CMmsgUsrError, "%f [var] = ",*(s->var)); printInorder(s,stderr);}
				return *(s->var); // <<<<=======  this is where the table lookup happens
				break;
			case FUNC:
				right = Calculate(s->right);
				if(NCmathEqualValues(right,FLOAT_NOVALUE)) return FLOAT_NOVALUE;
				if((GetDebug()) && (s->func == functions[numFunc])) CMmsgPrint (CMmsgUsrError, "WARN: Bad function used!");
				if(GetDebug()) { CMmsgPrint (CMmsgUsrError, "%f [func] = ",s->func(right)); printInorder(s,stderr);}
				return s->func(right);
				break;
			default:
				if(GetDebug()) fprintf(stderr,"BADTP");
				break;
		}
	}
	return FLOAT_NOVALUE;
}

bool CalculateI(IneqNode_t *s)
{
	if(s != NULL)
	{
		switch(s->equal)
		{
			case FALSE: return FALSE; break;
			case TRUE: return TRUE; break;
			case AND:
				if(s->lTree)
				{
					if(s->rTree) return Calculate(s->lhead) && Calculate(s->rhead);
					else return Calculate(s->lhead) && CalculateI(s->right);
				} else
				{
					if(s->rTree) return CalculateI(s->left) && Calculate(s->rhead);
					else return CalculateI(s->left) && CalculateI(s->right);
				}
				break;
			case OR:
				if(s->lTree)
				{
					if(s->rTree) return Calculate(s->lhead) || Calculate(s->rhead);
					else return Calculate(s->lhead) || CalculateI(s->right);
				} else
				{
					if(s->rTree) return CalculateI(s->left) || Calculate(s->rhead);
					else return CalculateI(s->left) || CalculateI(s->right);
				}
				break;
			case LT:
				if(s->lTree)
				{
					if(s->rTree) return Calculate(s->lhead) < Calculate(s->rhead);
					else return Calculate(s->lhead) < CalculateI(s->right);
				} else
				{
					if(s->rTree) return CalculateI(s->left) < Calculate(s->rhead);
					else return CalculateI(s->left) < CalculateI(s->right);
				}
				break;
			case LTE:
				if(s->lTree)
				{
					if(s->rTree) return Calculate(s->lhead) <= Calculate(s->rhead);
					else return Calculate(s->lhead) <= CalculateI(s->right);
				} else
				{
					if(s->rTree) return CalculateI(s->left) <= Calculate(s->rhead);
					else return CalculateI(s->left) <= CalculateI(s->right);
				}
				break;
			case EQ:
				if(s->lTree)
				{
					if(s->rTree) return Calculate(s->lhead) == Calculate(s->rhead);
					else return Calculate(s->lhead) == CalculateI(s->right);
				} else
				{
					if(s->rTree) return CalculateI(s->left) == Calculate(s->rhead);
					else return CalculateI(s->left) == CalculateI(s->right);
				}
				break;
			case NE:
				if(s->lTree)
				{
					if(s->rTree) return Calculate(s->lhead) != Calculate(s->rhead);
					else return Calculate(s->lhead) != CalculateI(s->right);
				} else
				{
					if(s->rTree) return CalculateI(s->left) != Calculate(s->rhead);
					else return CalculateI(s->left) != CalculateI(s->right);
				}
				break;
			case GTE:
				if(s->lTree)
				{
					if(s->rTree) return Calculate(s->lhead) >= Calculate(s->rhead);
					else return Calculate(s->lhead) >= CalculateI(s->right);
				} else
				{
					if(s->rTree) return CalculateI(s->left) >= Calculate(s->rhead);
					else return CalculateI(s->left) >= CalculateI(s->right);
				}
				break;
			case GT:
				if(s->lTree)
				{
					if(s->rTree) return Calculate(s->lhead) > Calculate(s->rhead);
					else return Calculate(s->lhead) > CalculateI(s->right);
				} else
				{
					if(s->rTree) return CalculateI(s->left) > Calculate(s->rhead);
					else return CalculateI(s->left) > CalculateI(s->right);
				}
				break;
			default:
				if(GetDebug()) fprintf(stderr,"BADINEQ!");
				break;
		}
	}
	return false;
}

void delTree(TreeNode_t *s)
{
	if(s != NULL)
	{
		delTree(s->left);
		delTree(s->right);
		free(s);
	}
}

void delTreeI(IneqNode_t *s)
{
	if(s != NULL)
	{
		if(s->lTree) delTree(s->lhead); else delTreeI(s->left);
		if(s->rTree) delTree(s->rhead); else delTreeI(s->right);
		free(s);
	}
}
