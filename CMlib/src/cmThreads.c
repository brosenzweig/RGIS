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
		job->Tasks [taskId].Dependence = job->TaskNum;
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

CMreturn CMthreadJobTaskDependence (CMthreadJob_p job, size_t taskId, size_t dependence) {
	if (taskId > job->TaskNum) {
		CMmsgPrint (CMmsgAppError,"Invalid task in %s%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (dependence > job->TaskNum) {
		CMmsgPrint (CMmsgAppError,"Invalid task dependence in %s%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (job->Tasks [taskId].Dependence < dependence) job->Tasks [taskId].Dependence = dependence;
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

	pthread_mutex_lock   (&(team->WorkerMutex));
	pthread_cond_signal  (&(team->WorkerSignal));
	pthread_mutex_unlock (&(team->WorkerMutex));

	pthread_mutex_lock   (&(team->MasterMutex));
	while (true) {
		pthread_cond_wait    (&(team->MasterSignal), &(team->MasterMutex));
//TODO		printf ("Thread#%d: Starting job\n",(int) data->Id);
		job = (CMthreadJob_p) team->JobPtr;
		if (job == (CMthreadJob_p) NULL) {
// TODO			printf ("Thread#%d: Quitting\n",(int) data->Id);
			break;
		}
		while (job->LastId > 0) {
			for (taskId = job->LastId - 1;taskId >= 0; taskId--) {
				if (job->Tasks [taskId].Completed) {
					if (taskId == (job->LastId  - 1)) job->LastId--;
					continue;
				}
				if (job->Tasks [taskId].Locked)    continue;
				if ((job->Tasks [taskId].Dependence < job->TaskNum) &&
					(job->Tasks [job->Tasks [taskId].Dependence].Completed == false)) continue;
				if (taskId == (job->LastId  - 1)) job->LastId--;
				job->Tasks [taskId].Locked = true;
				pthread_mutex_unlock (&(team->MasterMutex));
				job->UserFunc (team, job->CommonData, job->ThreadData == (void **) NULL ? (void *) NULL : job->ThreadData [data->Id], taskId);
				data->CompletedTasks++;
				pthread_mutex_lock   (&(team->MasterMutex));
				job->Tasks [taskId].Locked    = false;
				job->Tasks [taskId].Completed = true;
			}
		}
		pthread_mutex_lock   (&(team->WorkerMutex));
// TODO		printf ("Thread#%d: Ending job\n",(int) data->Id);
		pthread_cond_signal  (&(team->WorkerSignal));
		pthread_mutex_unlock (&(team->WorkerMutex));
	}
	pthread_mutex_unlock (&(team->MasterMutex));
	pthread_exit((void *) 0);
}

void CMthreadJobExecute (CMthreadTeam_p team, CMthreadJob_p job) {
	size_t completed = 0;
	int taskId;

	if (team != (CMthreadTeam_p) NULL) {
		pthread_mutex_lock     (&(team->MasterMutex));
		team->JobPtr = (void *) job;
		job->LastId  = job->TaskNum;
		pthread_cond_broadcast (&(team->MasterSignal));
		pthread_mutex_unlock   (&(team->MasterMutex));

		while (completed < team->ThreadNum) {
			pthread_mutex_lock     (&(team->WorkerMutex));
			pthread_cond_wait      (&(team->WorkerSignal), &(team->WorkerMutex));
			completed++;
			pthread_mutex_unlock   (&(team->WorkerMutex));
		}
// TODO		printf ("Master: Finished job\n");
		for (taskId = 0;taskId < job->TaskNum; ++taskId) job->Tasks [taskId].Completed = false;
		team->JobPtr  = (void *) NULL;
	}
	else {
		for (taskId = job->LastId - 1;taskId >= 0; taskId--)
			job->UserFunc (team,
			               job->CommonData,
			               job->ThreadData == (void **) NULL ? (void *) NULL : job->ThreadData [0],
			               taskId);
	}
}

CMthreadTeam_p CMthreadTeamCreate (size_t threadNum) {
	int ret;
	size_t threadId;
	pthread_attr_t thread_attr;
	CMthreadTeam_p team;

	if ((team = (CMthreadTeam_p) malloc (sizeof (CMthreadTeam_t))) == (CMthreadTeam_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation error in %s:%d\n",__FILE__,__LINE__);
		return ((CMthreadTeam_p) NULL);
	}
	if ((team->Threads = (CMthreadData_p) calloc (threadNum, sizeof(CMthreadData_t))) == (CMthreadData_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation error in %s:%d\n",__FILE__,__LINE__);
		free (team);
		return ((CMthreadTeam_p) NULL);
	}
	team->ThreadNum = threadNum;
	team->JobPtr    = (void *) NULL;

	pthread_mutex_init (&(team->MasterMutex),   NULL);
	pthread_cond_init  (&(team->MasterSignal),  NULL);
	pthread_mutex_init (&(team->WorkerMutex),   NULL);
	pthread_cond_init  (&(team->WorkerSignal),  NULL);
	pthread_mutex_init (&(team->ProcessMutex),  NULL);

	pthread_attr_init (&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

	for (threadId = 0; threadId < team->ThreadNum; ++threadId) {
		team->Threads [threadId].Id             = threadId;
		team->Threads [threadId].CompletedTasks = 0;
		team->Threads [threadId].TeamPtr        = (void *) team;
		pthread_mutex_lock (&(team->WorkerMutex));
		if ((ret = pthread_create (&(team->Threads [threadId].Thread), &thread_attr,_CMthreadWork,(void *) (team->Threads + threadId))) != 0) {
			CMmsgPrint (CMmsgAppError,"Thread creation returned with error [%d] in %s:%d\n",ret,__FILE__,__LINE__);
			free (team->Threads);
			free (team);
			return ((CMthreadTeam_p) NULL);
		}
		pthread_cond_wait    (&(team->WorkerSignal), &(team->WorkerMutex));
		pthread_mutex_unlock (&(team->WorkerMutex));
	}
	pthread_attr_destroy(&thread_attr);
	return (team);
}

void CMthreadTeamDestroy (CMthreadTeam_p team, bool report) {
	size_t threadId;
	size_t completedTasks = 0;
	void  *status;

	if (team != (CMthreadTeam_p) NULL) {
		pthread_mutex_lock (&(team->MasterMutex));
		team->JobPtr = (void *) NULL;
		pthread_cond_broadcast (&(team->MasterSignal));
		pthread_mutex_unlock (&(team->MasterMutex));
		for (threadId = 0;threadId < team->ThreadNum;++threadId) {
			pthread_join(team->Threads [threadId].Thread, &status);
			completedTasks += team->Threads [threadId].CompletedTasks;
		}
		if (report) {
			for (threadId = 0;threadId < team->ThreadNum;++threadId)
				CMmsgPrint (CMmsgInfo,"Threads#%d completed %d tasks (%6.2f of the total)\n",
						(int)   team->Threads [threadId].Id,
						(int)   team->Threads [threadId].CompletedTasks,
						(float) team->Threads [threadId].CompletedTasks * 100.0 / (float) completedTasks);
		}
		pthread_mutex_destroy (&(team->MasterMutex));
		pthread_cond_destroy  (&(team->MasterSignal));
		pthread_mutex_destroy (&(team->WorkerMutex));
		pthread_cond_destroy  (&(team->WorkerSignal));
		pthread_mutex_destroy (&(team->ProcessMutex));
		pthread_exit(NULL);
		free (team->Threads);
		free (team);
	}
}

void CMthreadTeamLock   (CMthreadTeam_p team) { if (team != (CMthreadTeam_p) NULL) pthread_mutex_lock     (&(team->ProcessMutex)); }

void CMthreadTeamUnlock (CMthreadTeam_p team) { if (team != (CMthreadTeam_p) NULL) pthread_mutex_unlock   (&(team->ProcessMutex)); }
