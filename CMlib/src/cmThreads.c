/******************************************************************************

GHAAS Command Line Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

cmThreads.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdlib.h>
#include <unistd.h>
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
	if ((job->SortedTasks = (CMthreadTask_p *) calloc (taskNum,sizeof (CMthreadTask_p))) == (CMthreadTask_p *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		free (job->Tasks);
		free (job);
		return ((CMthreadJob_p) NULL);
	}
	job->Sorted  = false;
	job->TaskNum = taskNum;
	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
		job->SortedTasks [taskId]       = job->Tasks + taskId;
		job->Tasks [taskId].Id          = taskId;
		job->Tasks [taskId].Completed   = false;
		job->Tasks [taskId].Locked      = false;
		job->Tasks [taskId].Dependent   = taskId;
		job->Tasks [taskId].DependNum   = 0;
		job->Tasks [taskId].DependCount = 0;
		job->Tasks [taskId].DependLevel = 0;
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

CMreturn CMthreadJobTaskDependent (CMthreadJob_p job, size_t taskId, size_t dependent) {
	if (taskId > job->TaskNum) {
		CMmsgPrint (CMmsgAppError,"Invalid task in %s%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (taskId == dependent) return (CMsucceeded);

	if (taskId < dependent) {
		job->Tasks [taskId].Dependent = dependent;
		job->Tasks [job->Tasks [taskId].Dependent].DependNum += 1;
	}
	else
		CMmsgPrint (CMmsgWarning,"Invalid dependence [%d:%d] ignored!\n", dependent, taskId);
	return (CMsucceeded);
}

static int _CMthreadJobTaskCompare (const void *leftPtr,const void *rightPtr) {
	CMthreadTask_p *leftTask  = (CMthreadTask_p *) leftPtr;
	CMthreadTask_p *rightTask = (CMthreadTask_p *) rightPtr;
	return ((*leftTask)->DependLevel - (*rightTask)->DependLevel);
}

static void _CMthreadJobTaskSort (CMthreadJob_p job) {
	size_t taskId, dependId;
	size_t level;

	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
		level = 1;
		for (dependId = taskId; dependId != job->Tasks [dependId].Dependent; dependId = job->Tasks [dependId].Dependent) {
			if (job->Tasks [job->Tasks [dependId].Dependent].DependLevel < level)
				job->Tasks [job->Tasks [dependId].Dependent].DependLevel = level;
			level++;
		}
	}
	qsort (job->SortedTasks,job->TaskNum,sizeof (CMthreadTask_p),_CMthreadJobTaskCompare);
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
	CMthreadJob_p  job  = team->JobPtr;
	clock_t start = clock ();

	while (job->LastId < job->TaskNum) {
		pthread_mutex_lock   (&(team->Mutex));
		for (taskId = job->LastId;taskId < job->TaskNum; ++taskId) {
			if (job->SortedTasks [taskId]->Completed) {
				if (taskId == job->LastId) job->LastId = taskId + 1;
				continue;
			}
			if (job->SortedTasks [taskId]->Locked)    continue;
			if (job->SortedTasks [taskId]->DependCount == job->SortedTasks [taskId]->DependNum) {
				job->SortedTasks [taskId]->Locked = true;
				job->Tasks [job->SortedTasks [taskId]->Dependent].Locked = true;

				pthread_mutex_unlock (&(team->Mutex));
				start = clock ();
				job->UserFunc (job->CommonData, job->ThreadData == (void **) NULL ? (void *) NULL : job->ThreadData [data->Id], job->SortedTasks [taskId]->Id);
				data->UserTime += clock () - start + 1;
				data->CompletedTasks++;
				pthread_mutex_lock   (&(team->Mutex));
				job->SortedTasks [taskId]->Locked      = false;
				job->Tasks [job->SortedTasks [taskId]->Dependent].Locked = false;
				job->SortedTasks [taskId]->Completed   = true;
				job->Tasks [job->SortedTasks [taskId]->Dependent].DependCount += 1;
				job->SortedTasks [taskId]->DependCount = 0;
			}
		}
		pthread_mutex_unlock (&(team->Mutex));
	}

	data->ThreadTime += clock () - start;
	if (data->Id > 0) pthread_exit((void *) NULL); // Only slave threads need to exit.
	return ((void *) NULL);
}

CMreturn CMthreadJobExecute (CMthreadTeam_p team, CMthreadJob_p job) {
	int ret, taskId;
	size_t threadId;
	void  *status;
	pthread_attr_t thread_attr;

	if (team != (CMthreadTeam_p) NULL) {
		pthread_attr_init (&thread_attr);
		pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
		team->JobPtr = (void *) job;
		job->LastId  = 0;
		if (job->Sorted == false) { _CMthreadJobTaskSort (job); job->Sorted = true; }

		for (taskId = 0; taskId < job->TaskNum; ++taskId) {
			job->Tasks [taskId].Completed = false;
			job->Tasks [taskId].Locked    = false;
// TODO		printf ("Task: %4d Depend: %4d\n", (int) taskId, (int) job->Tasks [taskId].Depend);
		}

		for (threadId = 0; threadId < team->ThreadNum; ++threadId) {
			if ((ret = pthread_create (&(team->Threads [threadId].Thread), &thread_attr,_CMthreadWork,(void *) (team->Threads + threadId))) != 0) {
				CMmsgPrint (CMmsgAppError,"Thread creation returned with error [%d] in %s:%d\n",ret,__FILE__,__LINE__);
				free (team->Threads);
				free (team);
				return (CMfailed);
			}
		}
		pthread_attr_destroy(&thread_attr);
		for (threadId = 0;threadId < team->ThreadNum;++threadId) pthread_join(team->Threads [threadId].Thread, &status);
	}
	else
		for (taskId = 0;taskId < job->TaskNum; ++taskId)
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
		team->Time = clock () - team->Time;
		if (report) {
			for (threadId = 0;threadId < team->ThreadNum;++threadId) completedTasks += team->Threads [threadId].CompletedTasks;
			for (threadId = 0;threadId < team->ThreadNum;++threadId)
				if (team->Threads [threadId].CompletedTasks > 0)
					CMmsgPrint (CMmsgInfo,"Threads#%d completed %9d tasks (%4.1f %c of the total) User time %4.1f(%4.1f) %c\n",
						(int)   team->Threads [threadId].Id,
						(int)   team->Threads [threadId].CompletedTasks,
						(float) team->Threads [threadId].CompletedTasks * 100.0 / (float) completedTasks,'%',
						(float) team->Threads [threadId].UserTime       * 100.0 / (float) team->Threads [threadId].ThreadTime,
						(float) team->Threads [threadId].UserTime       * 100.0 / (float) team->Time, '%');
				else
					CMmsgPrint (CMmsgInfo,"Threads#%d completed %9d tasks (%4.1f %c of the total) User time %4.1f(%4.1f) %c\n",
						(int)   team->Threads [threadId].Id,
						(int)   team->Threads [threadId].CompletedTasks,
						(float) 0.0, '%',
						(float) 0,
						(float) 0.0, '%');
		}
		pthread_mutex_destroy (&(team->Mutex));
		free (team->Threads);
		free (team);
	}
}

void CMthreadLock (void *teamPtr) {
	CMthreadTeam_t *team = (CMthreadTeam_t *) teamPtr;
	pthread_mutex_lock (&(team->Mutex));
}

void CMthreadUnlock (void *teamPtr) {
	CMthreadTeam_t *team = (CMthreadTeam_t *) teamPtr;
	pthread_mutex_unlock (&(team->Mutex));
}

