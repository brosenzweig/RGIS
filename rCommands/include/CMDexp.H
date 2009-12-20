/******************************************************************************

GHAAS RiverGIS Commands V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

CMDexp.h

balazs.fekete@unh.edu

*******************************************************************************/

class CMDExpression
	{
	private:
		DBInt TempVar;
		union	{
				char *NamePTR;
				DBObjTableField *FieldPTR;
				} Var;
		DBMathOperand *Operand;
		DBObjTable *TablePTR;
	public:
		CMDExpression (char *fieldName, char *expression,DBInt tmpVar)
			{ Var.NamePTR = fieldName; Operand = new DBMathOperand (expression); TempVar = tmpVar; }
		CMDExpression (char *fieldName, char *expression)
			{ Var.NamePTR = fieldName; Operand = new DBMathOperand (expression); TempVar = true; }
		DBInt Expand (DBObjectLIST<DBObject> *variables) { return (Operand->Expand (variables)); }
		DBInt Configure (DBObjTable *table)
			{
			DBObjTableField *fieldPTR;
			TablePTR = table;
			if ((fieldPTR = table->Field (Var.NamePTR)) == (DBObjTableField *) NULL)
				{
				fieldPTR = new DBObjTableField (Var.NamePTR,DBTableFieldFloat,"%10.3f",sizeof (DBFloat),false);
				table->AddField (fieldPTR);
				if (TempVar) fieldPTR->Flags (DBObjectFlagSelected,DBSet);
				else fieldPTR->Flags (DBObjectFlagSelected,DBClear);
				}
			else
				{
				if (TempVar)
					{
					if ((fieldPTR->Flags() & DBObjectFlagSelected) == DBObjectFlagSelected)
						TempVar = false;
					}
				else
					{
					if ((fieldPTR->Flags() & DBObjectFlagSelected) == DBObjectFlagSelected)
						fieldPTR->Flags (DBObjectFlagSelected,DBClear);
					}
				}
			Var.FieldPTR = fieldPTR;
			return (Operand->Configure (table->Fields ()));
			}
		void Evaluate (DBObjRecord *record)
			{
			switch (Var.FieldPTR->Type ())
				{
				case DBVariableString:	Var.FieldPTR->String	(record,Operand->String (record));	break;
				case DBVariableInt:		Var.FieldPTR->Int		(record,Operand->Int		(record));	break;
				case DBVariableFloat:	Var.FieldPTR->Float	(record,Operand->Float	(record));	break;
				}
			}
		~CMDExpression ()
			{
			if ((TempVar) && (Var.FieldPTR->Flags() & DBObjectFlagSelected) == DBObjectFlagSelected)
				TablePTR->DeleteField (Var.FieldPTR);
			}
	};
