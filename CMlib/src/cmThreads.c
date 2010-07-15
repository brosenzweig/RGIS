/******************************************************************************

GHAAS Command Line Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

cmThreads.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <cm.h>

static CMreturn _CMthreadTaskGroupInitialize (CMthreadTaskGroup_p group, size_t id, size_t threadNum, size_t taskNum, size_t minTasks, size_t start) {
	size_t threadId, res, num;

	group->Id    = id;
	group->Num   = taskNum;
	group->End = group->Start = (size_t *) NULL;
	if ((group->Start = (size_t *) calloc (threadNum, sizeof (size_t))) == (size_t *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s %d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	if ((group->End  = (size_t *) calloc (threadNum, sizeof (size_t))) == (size_t *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s %d\n",__FILE__,__LINE__);
		free (group->Start);
		group->Start = (size_t *) NULL;
		return (CMfailed);
	}
	if (taskNum > threadNum * minTasks) {
		num   = (int) (floor ((float) taskNum / (float) threadNum));
		res   = taskNum % threadNum;
		for (threadId = 0;threadId < threadNum; threadId++) {
			group->Start [threadId] = start + num * threadId        + (res == 0 ? 0 : (threadId + 1 <= res ? threadId : res));
			group->End   [threadId] = group->Start [threadId] + num + (res == 0 ? 0 : (threadId + 1 <= res ? 1 : 0));
		}
	}
	else {
		for (threadId = 0;threadId < threadNum; threadId++) {
			group->Start [threadId] = start;
			group->End   [threadId] = start;
		}
	}
	return (CMsucceeded);
}

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
	if ((job->Groups      = (CMthreadTaskGroup_p) calloc (1,sizeof (CMthreadTaskGroup_t))) == (CMthreadTaskGroup_p)  NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		free (job->SortedTasks);
		free (job->Tasks);
		free (job);
		return ((CMthreadJob_p) NULL);
	}
	job->ThreadNum = (team != (CMthreadTeam_p) NULL) ? team->ThreadNum : 1;
	if (_CMthreadTaskGroupInitialize (job->Groups + 0, 0, job->ThreadNum, taskNum, 1, 0) != CMsucceeded) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		free (job->Groups);
		free (job->SortedTasks);
		free (job->Tasks);
		free (job);
		return ((CMthreadJob_p) NULL);
	}
	job->GroupNum  = 1;
	job->Group     = 0;
	job->Completed = 0;
	job->Sorted    = true;
	job->TaskNum   = taskNum;
	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
		job->SortedTasks [taskId]       = job->Tasks + taskId;
		job->Tasks [taskId].Id          = taskId;
		job->Tasks [taskId].Dependent   = (CMthreadTask_p) NULL;
		job->Tasks [taskId].DependLevel = 0;
	}
	job->UserFunc = execFunc;
	job->CommonData = (void *) commonData;
	if (allocFunc != (CMthreadUserAllocFunc) NULL) {
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
	job->Sorted = false;
	if (taskId > job->TaskNum) {
		CMmsgPrint (CMmsgAppError,"Invalid task in %s%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (taskId == dependent) return (CMsucceeded);

	if (taskId < dependent) {
		job->Tasks [taskId].Dependent = job->Tasks + dependent;
	}
	else
		CMmsgPrint (CMmsgWarning,"Invalid dependence [%d:%d] ignored!\n", dependent, taskId);
	return (CMsucceeded);
}

static int _CMthreadJobTaskCompare (const void *lPtr,const void *rPtr) {
	int ret;
	CMthreadTask_p lTaskInit = *((CMthreadTask_p *) lPtr);
	CMthreadTask_p rTaskInit = *((CMthreadTask_p *) rPtr);
	CMthreadTask_p lTask = lTaskInit;
	CMthreadTask_p rTask = rTaskInit;

	if ((ret = lTask->DependLevel - rTask->DependLevel) != 0) return (ret);

	while (((lTask = lTask->Dependent) != (CMthreadTask_p) NULL) && ((rTask = rTask->Dependent) != (CMthreadTask_p) NULL)) {
		if ((ret = lTask->DependLevel - rTask->DependLevel) != 0) return (ret);
	}
	if (lTask == rTask) return (0);
	if (lTask != (CMthreadTask_p) NULL) return (1);
	return (-1);
}

CMreturn _CMthreadJobTaskSort (CMthreadJob_p job) {
	size_t taskId, start;
	size_t group;
	CMthreadTask_p dependent;

	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
		group = 1;
		for (dependent = job->Tasks + taskId; dependent->Dependent != (CMthreadTask_p) NULL; dependent = dependent->Dependent) {
			if (dependent->Dependent->DependLevel < group) {
				dependent->Dependent->DependLevel = group;
				group++;
			}
			else break;
		}
	}
	qsort (job->SortedTasks,job->TaskNum,sizeof (CMthreadTask_p),_CMthreadJobTaskCompare);
	job->GroupNum = job->SortedTasks [job->TaskNum - 1]->DependLevel + 1;
	if ((job->Groups = (CMthreadTaskGroup_p) realloc (job->Groups, job->GroupNum * sizeof (CMthreadTaskGroup_t))) == (CMthreadTaskGroup_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	group = 0;
	job->Groups [group].Id = group;
	start = 0;
	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
		if (group != job->SortedTasks [taskId]->DependLevel) {
			if (_CMthreadTaskGroupInitialize (job->Groups + group, group, job->ThreadNum, taskId - start, 1, start) != CMsucceeded) {
				CMmsgPrint (CMmsgAppError,"Task group initialization error in: %s:%d\n",__FILE__,__LINE__);
				return (CMfailed);
			}
			group = job->SortedTasks [taskId]->DependLevel;
			start = taskId;
		}
	}
	if (_CMthreadTaskGroupInitialize (job->Groups + group, group, job->ThreadNum, taskId - start, 1, start) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError,"Task group initialization error in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	return (CMsucceeded);
}

void CMthreadJobDestroy (CMthreadJob_p job, CMthreadUserFreeFunc freeFunc) {
	size_t threadId;

	if (freeFunc != (CMthreadUserFreeFunc) NULL) {
		for (threadId = 0;threadId < job->ThreadNum; ++threadId)
			freeFunc (job->ThreadData [threadId]);
	}
	if (job->Groups != (CMthreadTaskGroup_p) NULL) free (job->Groups);
	free (job->Tasks);
	free (job);
}

static void *_CMthreadWork (void *dataPtr) {
	CMthreadData_p data = (CMthreadData_p) dataPtr;
	size_t taskId, start, end;
	CMthreadTeam_p team = (CMthreadTeam_p) data->TeamPtr;
	CMthreadJob_p  job  = team->JobPtr;
	void *commonPtr = job->CommonData, *threadData = job->ThreadData == (void **) NULL ? (void *) NULL : job->ThreadData [data->Id];

	pthread_mutex_lock   (&(team->Mutex));
	while (job->Groups [job->Group].Start [data->Id] < job->Groups [job->Group].End [data->Id]) {
		pthread_mutex_unlock (&(team->Mutex));
		start = job->Groups [job->Group].Start [data->Id];
		end   = job->Groups [job->Group].End   [data->Id];
		for (taskId = start; taskId < end; taskId++) job->UserFunc (commonPtr, threadData, job->SortedTasks [taskId]->Id);
		pthread_mutex_lock (&(team->Mutex));
		job->Completed++;
		if (job->Completed == team->ThreadNum) {
			job->Group++;
			job->Completed = 0;
			if ((job->Group < job->GroupNum) && (job->Groups [job->Group].Start [data->Id] == job->Groups [job->Group].End [data->Id])) {
				start = job->Groups [job->Group].Start [0];
				end   = job->TaskNum;
				for (taskId = start; taskId < end; taskId++) job->UserFunc (commonPtr, threadData, job->SortedTasks [taskId]->Id);
			}
			pthread_cond_broadcast (&(team->Cond));
		}
		else pthread_cond_wait (&(team->Cond), &(team->Mutex));
	}
	pthread_mutex_unlock (&(team->Mutex));
	pthread_exit ((void *) NULL);
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
		job->Group  = 0;
		if (job->Sorted == false) { _CMthreadJobTaskSort (job); job->Sorted = true; }

		for (threadId = 0; threadId < team->ThreadNum; ++threadId) {
			if ((ret = pthread_create (&(team->Threads [threadId].Thread), &thread_attr,_CMthreadWork,(void *) (team->Threads + threadId))) != 0) {
				CMmsgPrint (CMmsgSysError,"Thread creation returned with error [%d] in %s:%d\n",ret,__FILE__,__LINE__);
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
	}
	pthread_mutex_init (&(team->Mutex), NULL);
	pthread_cond_init  (&(team->Cond),  NULL);

	return (team);
}

void CMthreadTeamDestroy (CMthreadTeam_p team) {

	if (team != (CMthreadTeam_p) NULL) {
		team->Time = clock () - team->Time;
		pthread_mutex_destroy (&(team->Mutex));
		pthread_cond_destroy  (&(team->Cond));
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

