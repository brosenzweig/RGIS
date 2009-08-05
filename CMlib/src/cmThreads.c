#include <stdlib.h>
#include <cm.h>

CMthreadJob_p CMthreadJobCreate (size_t taskNum, CMthreadUserFunc userFunc, void *userData) {
	size_t taskId;
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
	job->UserFunc = userFunc;
	job->UserData = (void *) userData;
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

void CMthreadJobDestroy (CMthreadJob_p job) {
	free (job->Tasks);
	free (job);
}
static void *_CMthreadWork (void *dataPtr) {
	CMthreadData_p data = (CMthreadData_p) dataPtr;
	int taskId;
	bool dependence;
	CMthreadTeam_p team = (CMthreadTeam_p) data->TeamPtr;
	CMthreadJob_p  job;

	pthread_mutex_lock   (&(team->WorkerMutex));
	pthread_cond_signal  (&(team->WorkerSignal));
	pthread_mutex_unlock (&(team->WorkerMutex));

	pthread_mutex_lock   (&(team->MasterMutex));
	while (true) {
		pthread_cond_wait    (&(team->MasterSignal), &(team->MasterMutex));
		printf ("Thread#%d: Starting job\n",(int) data->Id);
		job = team->Job;
		if (job == (CMthreadJob_p) NULL) {
			printf ("Thread#%d: Quitting\n",(int) data->Id);
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
//				printf ("Thread#%d: working on task %d (%d)\n",(int) data->Id, (int) taskId,(int) job->LastId);
				pthread_mutex_unlock (&(team->MasterMutex));
				job->UserFunc (job->UserData, taskId);
				data->CompletedTasks++;
				pthread_mutex_lock   (&(team->MasterMutex));
				job->Tasks [taskId].Locked    = false;
				job->Tasks [taskId].Completed = true;
			}
		}
		pthread_mutex_lock   (&(team->WorkerMutex));
		printf ("Thread#%d: Ending job\n",(int) data->Id);
		pthread_cond_signal  (&(team->WorkerSignal));
		pthread_mutex_unlock (&(team->WorkerMutex));
	}
	pthread_mutex_unlock (&(team->MasterMutex));
	pthread_exit((void *) 0);
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
	team->Job       = (CMthreadJob_p) NULL;

	pthread_mutex_init (&(team->MasterMutex),  NULL);
	pthread_cond_init  (&(team->MasterSignal), NULL);
	pthread_mutex_init (&(team->WorkerMutex),  NULL);
	pthread_cond_init  (&(team->WorkerSignal), NULL);

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

void CMthreadTeamJobExecute (CMthreadTeam_p team, CMthreadJob_p job) {
	size_t completed = 0;
	int taskId;

	pthread_mutex_lock     (&(team->MasterMutex));
	team->Job   = job;
	job->LastId = job->TaskNum;
	pthread_cond_broadcast (&(team->MasterSignal));
	pthread_mutex_unlock   (&(team->MasterMutex));

	while (completed < team->ThreadNum) {
		pthread_mutex_lock     (&(team->WorkerMutex));
		pthread_cond_wait      (&(team->WorkerSignal), &(team->WorkerMutex));
		completed++;
		pthread_mutex_unlock   (&(team->WorkerMutex));
	}
	printf ("Master: Finished job\n");
	for (taskId = 0;taskId < job->TaskNum; ++taskId) job->Tasks [taskId].Completed = false;
	team->Job  = (CMthreadJob_p) NULL;
}

void CMthreadTeamDestroy (CMthreadTeam_p team) {
	size_t threadId;
	size_t completedTasks = 0;
	void  *status;

	pthread_mutex_lock (&(team->MasterMutex));
	team->Job = (CMthreadJob_p) NULL;
	pthread_cond_broadcast (&(team->MasterSignal));
	pthread_mutex_unlock (&(team->MasterMutex));
	for (threadId = 0;threadId < team->ThreadNum;++threadId) {
		pthread_join(team->Threads [threadId].Thread, &status);
		completedTasks += team->Threads [threadId].CompletedTasks;
	}
	for (threadId = 0;threadId < team->ThreadNum;++threadId) {
		printf ("Threads#%d completed %d tasks (%6.2f of the total)\n",
				(int)   team->Threads [threadId].Id,
				(int)   team->Threads [threadId].CompletedTasks,
				(float) team->Threads [threadId].CompletedTasks * 100.0 / (float) completedTasks);
	}
	pthread_mutex_destroy (&(team->MasterMutex));
	pthread_cond_destroy  (&(team->MasterSignal));
	pthread_mutex_destroy (&(team->WorkerMutex));
	pthread_cond_destroy  (&(team->WorkerSignal));
	pthread_exit(NULL);
	free (team->Threads);
	free (team);
}
