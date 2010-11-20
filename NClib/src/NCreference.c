#include<NCdsHandle.h>

void NCreferenceInitialize (NCreference_t *ref)
{
	ref->Num    = 0;
	ref->Idx    = (int *)    NULL;
	ref->Weight = (double *) NULL;
}

void NCreferenceClear (NCreference_t *ref)
{
	if (ref->Num > (size_t) 0) free (ref->Idx);
	if (ref->Num > (size_t) 1) free (ref->Weight);
	NCreferenceInitialize (ref);
}
