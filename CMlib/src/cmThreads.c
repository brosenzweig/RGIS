#include <stdlib.h>
#include <cm.h>

CMthreadJob_p CMthreadJobCreate (CMthreadTeam_p team,
		                         void *commonData,
		                         size_t taskNum,
		                         CMthreadUserAllocFunc allocFunc,
		                         CMthreadUserExecFunc  execFunc) {
	size_t taskId;
	int    threadId;
	CMthreadJob_p job;

	if ((job = (CMthreadJob_p) malloc (sizeof (CMthreadJob_t))) == (CMthreadJob_p) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		return ((CMthreadJob_p) NULL);
	}
	if ((job->Tasks = (CMthreadTask_p) calloc (taskNum,sizeof (CMthreadTask_t))) == (CMthreadTask_p) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		free (job);
		return ((CMthreadJob_p) NULL);
	}
	job->TaskNum = taskNum;
	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
		job->Tasks [taskId].Id         = taskId;
		job->Tasks [taskId].Completed  = false;
		job->Tasks [taskId].Locked     = false;
		job->Tasks [taskId].Depend     = taskId;
	}
	job->LastId   = job->TaskNum;
	job->UserFunc = execFunc;
	job->CommonData = (void *) commonData;
	if (allocFunc != (CMthreadUserAllocFunc) NULL) {
		job->ThreadNum = (team != (CMthreadTeam_p) NULL) && (team->ThreadNum > 1) ? team->ThreadNum : 1;
		if ((job->ThreadData = (void **) calloc (job->ThreadNum, sizeof (void *))) == (void **) NULL) {
			CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d\n",__FILE__,__LINE__);
			free (job);
			return ((CMthreadJob_p) NULL);
		}
		for (threadId = 0;threadId < job->ThreadNum;++threadId) {
			if ((job->ThreadData [threadId] = allocFunc (commonData)) == (void *) NULL) {
				CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d\n",__FILE__,__LINE__);
				for (--threadId;threadId >= 0; --threadId) free (job->ThreadData [threadId]);
				free (job->ThreadData);
				free (job);
				return ((CMthreadJob_p) NULL);
			}
		}
	}
	else job->ThreadData = (void **) NULL;
	return (job);
}

CMreturn CMthreadJobTaskDependence (CMthreadJob_p job, size_t taskId, size_t depend) {
	if (taskId > job->TaskNum) {
		CMmsgPrint (CMmsgAppError,"Invalid task in %s%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (job->Tasks [taskId].Depend == taskId) job->Tasks [taskId].Depend = depend;
	else
		job->Tasks [taskId].Depend = job->Tasks [taskId].Depend < depend ?
		                             job->Tasks [taskId].Depend : depend;
	return (CMsucceeded);
}

void CMthreadJobDestroy (CMthreadJob_p job, CMthreadUserFreeFunc freeFunc) {
	size_t threadId;

	if (freeFunc != (CMthreadUserFreeFunc) NULL) {
		for (threadId = 0;threadId < job->ThreadNum; ++threadId)
			freeFunc (job->ThreadData [threadId]);
	}
	free (job->Tasks);
	free (job);
}

static void *_CMthreadWork (void *dataPtr) {
	CMthreadData_p data = (CMthreadData_p) dataPtr;
	int taskId;
	CMthreadTeam_p team = (CMthreadTeam_p) data->TeamPtr;
	CMthreadJob_p  job;
	clock_t start = clock ();

	pthread_mutex_lock   (&(team->Mutex));
	job = (CMthreadJob_p) team->JobPtr;
//TODO printf ("Thread#%d: Starting job [task: %d]\n",(int) data->Id,job->LastId);
	while (job->LastId > 0) {
		for (taskId = job->LastId - 1;taskId >= 0; taskId--) {
			if (job->Tasks [taskId].Completed) {
				if (taskId == (job->LastId  - 1)) job->LastId = taskId;
				continue;
			}
			if (job->Tasks [taskId].Locked)    continue;
			if ((job->Tasks [taskId].Depend == taskId) || (job->Tasks [job->Tasks [taskId].Depend].Completed)) {
				job->Tasks [taskId].Locked = true;
				if (taskId == (job->LastId  - 1)) job->LastId = taskId;

				pthread_mutex_unlock (&(team->Mutex));
				start = clock ();
				job->UserFunc (job->CommonData, job->ThreadData == (void **) NULL ? (void *) NULL : job->ThreadData [data->Id], taskId);
				data->UserTime += clock () - start;
				data->CompletedTasks++;
				pthread_mutex_lock   (&(team->Mutex));
				job->Tasks [taskId].Locked    = false;
				job->Tasks [taskId].Completed = true;
			}
		}
	}
// TODO printf ("Thread#%d: Ending job\n",(int) data->Id);
	data->ThreadTime += clock () - start;
	pthread_mutex_unlock (&(team->Mutex));
	if (data->Id > 0) pthread_exit((void *) NULL);
	return ((void *) NULL);
}

CMreturn CMthreadJobExecute (CMthreadTeam_p team, CMthreadJob_p job) {
	int ret, taskId;
	size_t threadId;
	void  *status;
	pthread_attr_t thread_attr;

	if (team != (CMthreadTeam_p) NULL) {
//TODO	printf ("Master: Starting job\n");
		pthread_attr_init (&thread_attr);
		pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
		team->JobPtr = (void *) job;
		job->LastId  = job->TaskNum;
		for (taskId = 0; taskId < job->LastId; ++taskId) {
			job->Tasks [taskId].Completed = false;
//			job->Tasks [job->Tasks [taskId].Depend].Locked = true;
		}

		for (threadId = 1; threadId < team->ThreadNum; ++threadId) {
			if ((ret = pthread_create (&(team->Threads [threadId].Thread), &thread_attr,_CMthreadWork,(void *) (team->Threads + threadId))) != 0) {
				CMmsgPrint (CMmsgAppError,"Thread creation returned with error [%d] in %s:%d\n",ret,__FILE__,__LINE__);
				free (team->Threads);
				free (team);
				return (CMfailed);
			}
		}
		status = _CMthreadWork (team->Threads);
		pthread_attr_destroy(&thread_attr);
		for (threadId = 1;threadId < team->ThreadNum;++threadId) pthread_join(team->Threads [threadId].Thread, &status);

// TODO printf ("Master: Finished job\n");
	}
	else
		for (taskId = job->LastId - 1;taskId >= 0; taskId--)
			job->UserFunc (job->CommonData,
			               job->ThreadData != (void **) NULL ? job->ThreadData [0] : (void *) NULL,
			               taskId);
	return (CMsucceeded);
}

CMthreadTeam_p CMthreadTeamCreate (size_t threadNum) {
	size_t threadId;
	CMthreadTeam_p team = (CMthreadTeam_p) NULL;

	if (threadNum < 2) return (team);
	if ((team = (CMthreadTeam_p) malloc (sizeof (CMthreadTeam_t))) == (CMthreadTeam_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation error in %s:%d\n",__FILE__,__LINE__);
		return ((CMthreadTeam_p) NULL);
	}
	if ((team->Threads = (CMthreadData_p) calloc (threadNum, sizeof(CMthreadData_t))) == (CMthreadData_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation error in %s:%d\n",__FILE__,__LINE__);
		free (team);
		return ((CMthreadTeam_p) NULL);
	}
	team->ThreadNum      = threadNum;
	team->JobPtr         = (void *) NULL;
	team->Time           = clock ();

	for (threadId = 0; threadId < team->ThreadNum; ++threadId) {
		team->Threads [threadId].Id             = threadId;
		team->Threads [threadId].TeamPtr        = (void *) team;
		team->Threads [threadId].CompletedTasks = 0;
		team->Threads [threadId].ThreadTime     = (clock_t) 0;
		team->Threads [threadId].UserTime       = (clock_t) 0;
	}
	pthread_mutex_init (&(team->Mutex),   NULL);
	return (team);
}

void CMthreadTeamDestroy (CMthreadTeam_p team, bool report) {
	size_t threadId, completedTasks = 0;

	if (team != (CMthreadTeam_p) NULL) {
// TODO printf ("Master: Discharging team\n");
		team->Time = clock () - team->Time;
		if (report) {
			for (threadId = 0;threadId < team->ThreadNum;++threadId) completedTasks += team->Threads [threadId].CompletedTasks;
			for (threadId = 0;threadId < team->ThreadNum;++threadId)
				CMmsgPrint (CMmsgInfo,"Threads#%d completed %9d tasks (%4.1f %c of the total) User time %4.1f(%4.1f) %c\n",
						(int)   team->Threads [threadId].Id,
						(int)   team->Threads [threadId].CompletedTasks,
						(float) team->Threads [threadId].CompletedTasks * 100.0 / (float) completedTasks,'%',
						(float) team->Threads [threadId].UserTime       * 100.0 / (float) team->Threads [threadId].ThreadTime,
						(float) team->Threads [threadId].UserTime       * 100.0 / (float) team->Time, '%');
		}
		pthread_mutex_destroy (&(team->Mutex));
		pthread_exit(NULL);
		free (team->Threads);
		free (team);
	}
}
