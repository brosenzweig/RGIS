#include<NC.h>

NCstate NCifyContinuous (NCObjData_t *objData)
{
	
		gCont->MColumn = (NCObjColumn_t *) NCObjListFindItemByName (table->Columns, NCnameVAAverage);
		gCont->IColumn = (NCObjColumn_t *) NCObjListFindItemByName (table->Columns, NCnameVAMinimum);
		gCont->AColumn = (NCObjColumn_t *) NCObjListFindItemByName (table->Columns, NCnameVAMaximum);
		gCont->SColumn = (NCObjColumn_t *) NCObjListFindItemByName (table->Columns, NCnameVAStdDev);
}

NCstate NCify (NCObjData_t *objData)
{
	switch (objData->DType)
	{
		case NCTypePoint:
			break;
		case NCTypeLine:
			break;
		case NCTypePolygon:
			break;
		case NCTypeContGrid:
			break;
		case NCTypeDiscGrid:
			break;
		case NCTypeNetwork:
			break;
		case NCTypeTabular:
			break;
	}
}
