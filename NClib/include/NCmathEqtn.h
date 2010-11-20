#include <NCmath.h>

// *** STRUCTURES USED

typedef enum { CONST, VAR, OPER, FUNC, NOTP } Type;
typedef enum { ADD, SUB, MUL, DIV, MOD, EXP, NOOP } Oper;
typedef enum { FALSE, TRUE, AND, OR, LT, LTE, EQ, NE, GTE, GT, NOEQ } Equal;

typedef struct TreeNode
{
	Type type;
	union {
		double cons;
		double *var;
		Oper oper;
		double (*func)(double);
	};
	struct TreeNode *left, *right;
} TreeNode_t;

typedef struct IneqNode
{
	Equal equal;
	bool lTree, rTree;
	union
	{
		TreeNode_t *lhead;
		struct IneqNode *left;
	};
	union
	{
		TreeNode_t *rhead;
		struct IneqNode *right;
	};
} IneqNode_t;

typedef struct Variable
{
	char *name;
	bool vary;
	int colnum;
	double val;
} Variable_t;
/* Use this structure if your equation has variable names in it.
 * the way to use it is the following:
 * name = name of the variable as it appears in your equation
 * vary = true if you plan to run Calculate() or CalculateI() multiple times with different
 *        values. In this case set val = the value of the variable.
 *      = false if it's non varying. set val = to the value of the variable, then run mkTree()
 *        or mkITree()> If you set val to anything after mkTree() was run, it will make no changes
 *        unless you run mkTree() again.
 * colnum = just a storage place for the user to know which column from the table the variable
 *          should be taken out of.
 * An array of these variables should be placed in the Variable_t *vars, and varNum should be
 * updated as the current size of the vars array.
*/

// *** VARIABLES USED

#define numFunc 11 // this number must correspond to the size of the array of *funcNames[]
                   // and *functions[]

extern Variable_t *_Vars;
extern int _VarNum;

// *** FUNCTIONS

bool isIneq(char [], int *); // returns true if the given equation has an inequality, and sets
                             // int to it's location. If your equation has an inequality, you
                             // must use the functions with an 'I' at the end.
int NCGmathGetVarNum();
int NCGmathAddVar(int colnum, char *name, bool vary);
int NCGmathGetVarColNum(int varID);
void NCGmathSetVarVal(int varID, double value);
double NCGmathGetVarVal(int varID);
bool NCGmathGetVarVary(int varID);
char *NCGmathGetVarName(int varID);
void NCGmathFreeVars();
void setLisp(); // Enable extra parentheses while using printInorder() or printInorderI()
void unsetLisp();
TreeNode_t* mkTree(char []);
IneqNode_t* mkTreeI(char []);
void printInorder(TreeNode_t *,FILE *);
void printInorderI(IneqNode_t *, FILE *);
void NCGmathEqtnFixTree(TreeNode_t **); // simplifies the equation the best it can
void NCGmathEqtnFixTreeI(IneqNode_t *); // simplifies the expression the best it can
double Calculate(TreeNode_t *);
bool   CalculateI(IneqNode_t *);
void delTree(TreeNode_t *);
void delTreeI(IneqNode_t *);
