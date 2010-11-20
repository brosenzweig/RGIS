#include<NC.h>

NCGstate NCGifyContinuous (NCGObjData_t *objData)
{
	
		gCont->MColumn = (NCGObjColumn_t *) NCGObjListFindItemByName (table->Columns, NCGnameVAAverage);
		gCont->IColumn = (NCGObjColumn_t *) NCGObjListFindItemByName (table->Columns, NCGnameVAMinimum);
		gCont->AColumn = (NCGObjColumn_t *) NCGObjListFindItemByName (table->Columns, NCGnameVAMaximum);
		gCont->SColumn = (NCGObjColumn_t *) NCGObjListFindItemByName (table->Columns, NCGnameVAStdDev);
}

NCGstate NCGify (NCGObjData_t *objData)
{
	switch (objData->DType)
	{
		case NCGTypePoint:
			break;
		case NCGTypeLine:
			break;
		case NCGTypePolygon:
			break;
		case NCGTypeContGrid:
			break;
		case NCGTypeDiscGrid:
			break;
		case NCGTypeNetwork:
			break;
		case NCGTypeTabular:
			break;
	}
}
