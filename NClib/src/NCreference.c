#include<NCdsHandle.h>

void NCGreferenceInitialize (NCGreference_t *ref)
{
	ref->Num    = 0;
	ref->Idx    = (int *)    NULL;
	ref->Weight = (double *) NULL;
}

void NCGreferenceClear (NCGreference_t *ref)
{
	if (ref->Num > (size_t) 0) free (ref->Idx);
	if (ref->Num > (size_t) 1) free (ref->Weight);
	NCGreferenceInitialize (ref);
}
