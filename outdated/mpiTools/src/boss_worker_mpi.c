///////////////////////////////////////////////////////////////////////////////////////////
//
// JGM, April 2008 - messchendorp@kvi.nl
//
// Boss-Worker MPI program with generic scripting
//
// Usage:
//
//       mpirun -np [NPROCS] boss_worker_mpi -j <path+file> [JOBFILE]
//                                           -s <path>      [PATH TO LOCAL SCRATCH]
//                                           -m <path+file> [PATH AND SCRIPT TO MOVE FILES]
//                                           -t <val>       [TIMEOUT IN SECS]
//                                           -r <val>       [INITIAL RUNID NUMBER]
//                                           -b <Mb>        [MIN SCRATCH SPACE]
//                                           -p <val>       [MAX PENDING JOBS]
//                                           -l <val>       [MAX LOAD LEVEL]
//                                           -n <level>     [NICE LEVEL]
//                                           -k             [KEEP FILES IN SCRATCH}
//                                           -c             [CLEAR SCRATCH BEFORE JOB START]
//                                           -v             [BE VERBOSE]
//                                           -d             [RUN IN DUMMY MODE]
//
// with [NPROCS] the number of processes. One of the processes will be the boss,
// and the remaining processes are the workers who obtain work packages from the
// boss. The variable [JOBFILE] refers to the path and filename describing the jobs 
// to be distributed among the workers. For more information, we refer to the README file.
// Most of the parameters mentioned above can be specified, and overruled, in the [JOBFILE].
//
// Johan Messchendorp, 24/02/2011.
//
////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <pthread.h>
// modified to reference the master mpi.h file, to meet the MPI standard spec.
#include <mpi.h>

//
// Some default settings, can be overwritten by supplying arguments to executable
//

#define JOBDESFILE        "jobs.in"                            // Default job description file
#define MOVEFILES         "~/bin/movefiles"                   // Path and program to move files
#define SCRATCH           "/tmp"                             // Use this as temporarily disk space
#define DEFAULT_TIMEOUT   3600                              // Default time out period for script and move process (secs)
#define DEFAULT_MINDISK   512                              // Default minimum required buffer disk space (Mb)
#define DEFAULT_MAXDIR    4                               // Default maximum jobs/open directories pending
#define DEFAULT_MAXLOAD   100                            // Default maximum load 
#define REMOVE_TIMEOUT    60                            // Timeout for the cleanup system calls
#define DEFAULT_NICELEVEL 0                            // Default Nice level for the jobs
//
// The various communication identifiers, TAGs
//

#define REQUESTTAG       1
#define TASKTAG          2
#define ABORTTAG         3
#define SCRIPTTAG        4
#define MOVETAG          5
#define FINISHTAG        6
#define SLEEPTAG         7
#define BARRIERTAG       8

#define JOB_REQUEST       0                                    // No job result, request for job
#define JOB_OK            1                                   // Job result OK
#define WORKER_IDLE       2                                  // Worker finished all his jobs and went in idle mode
#define JOB_INPUT_ERROR  -1                                 // Copy of input data failed
#define JOB_SCRIPT_ERROR -2                                // Running the script failed
#define JOB_MOVE_ERROR   -3                               // Moving the data failed

#define SLEEPTIME        5                                  // Sleep time in case worker has 
                                                           // to wait for copying process or other workers 
                                                          // to finish (secs)
#define BOSSRANK         0                               // Rank number of boss process, better stick to 0!

#define VERYLONGCHARSIZE 1024
#define LONGCHARSIZE     256
#define SHORTCHARSIZE    16

#define MPELOGFORMAT    "SLOG"                 // Log format of MPE, for profiling.

#define JOBSTRINGSIZE    128                   // Maximum length of one of the parameters defining the job
#define DEFAULT_JOBSIZE  5                     // Initial memory in units of JOBSTRINGSIZE allocated to job 
                                               // (automatically resized on the fly)

#define MAX_HOSTNAME_LENGTH 32

extern void KillProcessAndDaughters(int, char *);   // External function implemented in killprocesstree.c

//
// Global variables used by boss/worker 
//

char jobdescription_filename[VERYLONGCHARSIZE];// The filename containing the information about all jobs to be carried our
char scratch_path[VERYLONGCHARSIZE];           // The path name pointing to the directory for local buffer storage
char move_files[VERYLONGCHARSIZE];             // Name and path of the executable used to move files
int  keep_buffer;                              // In case this flag set, the buffer output will be kept
int  clear_scratch;                            // In case this flag set, the scratch directory will be first cleared
int  dummy_mode;                               // Flag which is set to one in case of a dry/dummy run
int  verbose_mode;                             // Verbosity flag
unsigned int timeout[2];                       // Timeout (s) of workers: 
                                               // [0]=job timeout; 
                                               // [1]=sleep period in case of too many running jobs/disk space problem
unsigned int runid;                            // The current Run Identification number
unsigned int minimum_disk_space;               // The minimum available disk space required for a worker to be active (MBytes)
unsigned int maximum_running_jobs;             // Maximum allowed running jobs per worker. Exceeding this number will put the worker to sleep
double maximum_load;                           // Maximum allowed load of a worker. Exceeding this number will put the worker to sleep
int number_of_running_jobs;                    // Number of presently running jobs (worker)
int ndonejobs;                                 // Number of jobs finished successfully
int rank;                                      // Rank of the process
int nice_level;                                // Nice level

pthread_mutex_t job_mutex    = PTHREAD_MUTEX_INITIALIZER; // Mutex lock for writing static variable "ndonejobs"
pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex lock for writing "thread_list" and "number_of_running_jobs"

typedef struct {
  int   size;                                        // Size of the string "array"
  char *array;                                       // String containing details of the job description
  int   timeout;                                     // Timeout (s) period after which job is killed
  int   nice;                                        // Nice level of the job
} job_description;                                   // Structure describing contents of a job

typedef struct {
  int   jobid;                                       // Job identification number (=runid)
  int   worker;                                      // Worker ID = rank number
  int   timeout;                                     // Timeout (s) period after which process killed
  char  output[VERYLONGCHARSIZE];                    // String containing the output device
  char  movecmd[VERYLONGCHARSIZE];                   // String containing the program for moving data
  char  scratch[VERYLONGCHARSIZE];                   // String containing the scratch directory
} thread_info;                                       // Structure containing the information necessary to carry out the MoveJob (thread)

typedef struct {
  pthread_cond_t  cond;                              // Conditional variable
  pthread_mutex_t mutex;                             // Mutex variable
  char           *command;                           // String containing the system call argument
  int             retval;                            // Return value of the thread
} system_info;                                       // Structure containing the information necessary to make a threaded system call

typedef struct thread_list {
  pthread_t     thread_id;                           // The ID of the thread
  thread_info*  info;                                // Pointer to the thread information
  void*         next;                                // Pointer to the next item in the list
} thread_list ;                                      // Linked list containing the information about running "MoveJob" threads

static thread_list *head = NULL;                     // The pointer to the head of the linked list of type "thread_list"
static thread_list *tail = NULL;                     // The pointer to the tail of the linked list

//
// void PrintOptions()
// -------------------
//
// Description: Routine which prints all the available command line options for start of the program
// Input:       none
// Output:      none
// Depends on:  static variables define globally 
//

void PrintOptions()
{
  printf("\n<I> Usage: boss_worker_mpi [OPTIONS]\n\n")  ;
  printf("    with following OPTIONS:\n\n");
  printf("    -?             --- This message\n");
  printf("    -j <path/file> --- Job description path+filename (default=%s)\n",
	 jobdescription_filename);
  printf("    -s <path>      --- Path to local buffer directory (default=%s)\n",
	 scratch_path);
  printf("    -b <Mb>        --- Minimum requested buffer space (default=%u Mb)\n",
	 minimum_disk_space);
  printf("    -p <number>    --- Maximum allowed pending threads for a worker (default=%i)\n",
	 maximum_running_jobs);
  printf("    -l <val>       --- Maximum allowed load of a worker (default=%.2f)\n",
	 maximum_load);
  printf("    -m <path>      --- Path and copying script (default=%s)\n",
	 move_files);
  printf("    -t <seconds>   --- Time out for script, copy, and move processes (default=%u secs)\n",
	 timeout[0]);
  printf("    -r <number>    --- Starting run identification number (default=%u)\n",
	 runid);
  printf("    -n <level>     --- Nice level of the jobs (default=%d)\n",
	 nice_level);
  printf("    -k             --- Do not delete files in buffer space\n");
  printf("    -c             --- Clear scratch space before starting the job\n");
  printf("    -v             --- Be verbose\n");
  printf("    -d             --- Enable dummy mode\n\n");
   
  return;
}

//
// int ReadArguments(unsigned int argc, char **argv)
// -------------------------------------------------
//
// Description: Reads all the command line arguments and fills the corresponding global variables
// Input:       argv: command line input parameters, argc: number of input parameters
// Output:      fills static variables, returns 1 on success, 0 in case of an unknown parameter
// Depends on:  static variables define globally 
//

int ReadArguments(unsigned int argc,char **argv)
{
  unsigned int i;
  time_t now;

  /* Set defaults */

  sprintf(jobdescription_filename,"%s",JOBDESFILE);
  sprintf(scratch_path,"%s",SCRATCH);
  sprintf(move_files,"%s",MOVEFILES);
  timeout[0]=DEFAULT_TIMEOUT;
  timeout[1]=DEFAULT_TIMEOUT;
  minimum_disk_space=DEFAULT_MINDISK;
  maximum_running_jobs=DEFAULT_MAXDIR;
  maximum_load=DEFAULT_MAXLOAD;
  nice_level=DEFAULT_NICELEVEL;
  dummy_mode=0;
  verbose_mode=0;
  keep_buffer=0;
  clear_scratch=0;

  time(&now); // Use time as starting value for the run ID
  runid=now;

  /* Read input and overwrite defaults */

  for (i=1;i<argc;i++)
    {
      if (!(strcmp(argv[i],"-j")))
	{
	  i++;
	  strcpy(jobdescription_filename,argv[i]);
	}
      else if (!(strcmp(argv[i],"-s")))
	{
	  i++;
	  strcpy(scratch_path,argv[i]);
	}
      else if (!(strcmp(argv[i],"-b")))
	{
	  i++;
	  minimum_disk_space=atoi(argv[i]);
	}
      else if (!(strcmp(argv[i],"-p")))
	{
	  i++;
	  maximum_running_jobs=atoi(argv[i]);
	}
      else if (!(strcmp(argv[i],"-l")))
	{
	  i++;
	  maximum_load=atof(argv[i]);
	}
      else if (!(strcmp(argv[i],"-m")))
	{
	  i++;
	  strcpy(move_files,argv[i]);
	}
      else if (!(strcmp(argv[i],"-t")))
	{
	  i++;
	  timeout[0]=atoi(argv[i]);
	  timeout[1]=timeout[0];
	}
      else if (!(strcmp(argv[i],"-d")))
	{
	  dummy_mode=1;
	}
      else if (!(strcmp(argv[i],"-k")))
	{
	  keep_buffer=1;
	}
      else if (!(strcmp(argv[i],"-c")))
	{
	  clear_scratch=1;
	}
      else if (!(strcmp(argv[i],"-v")))
	{
	  verbose_mode=1;
	}
      else if (!(strcmp(argv[i],"-r")))
	{
	  i++;
	  runid=atoi(argv[i]);
	}
      else if (!(strcmp(argv[i],"-n")))
	{
	  i++;
	  nice_level=atoi(argv[i]);
	}
      else if (!(strcmp(argv[i],"-?")))
	{
	  if (rank==BOSSRANK) // avoid printing this message for each process
	    {
	      PrintOptions();
	    }
	  return 0;
	}
      else
	{
	  if (rank==BOSSRANK) // avoid printing this message for each process
	    {
	      fprintf(stderr,"<E> Unknown option %s\n",argv[i]);
	      PrintOptions();
	    }
	  return 0;
	}
    }
  return 1;
}

//
// int GetFreeDiskSpace()
// ----------------------
//
// Description: Calculates the available disk space
// Input:       none
// Output:      returns the amount of disk space available on scratch in MBytes
//              if negative, then program could not get information from filesystem
// Depends on:  static variables define globally 
//

int GetFreeDiskSpace()
{
  struct statvfs fiData;

  if((statvfs(scratch_path,&fiData)) < 0 ) 
    {
      fprintf(stderr,"<W:%i> Failed to obtain disk space for \"%s\": proceed anyway!\n", rank, scratch_path);
      fflush(stderr);
      return -1;
    } 

  return ((fiData.f_bfree/1024)*(fiData.f_bsize/1024));  /* in MBytes */
}

//
// double GetAverageLoad(int period)
// ---------------------------------
//
// Description: Obtains the average load for a period of
//              1 min (period=0), 5 min (period=1), or 15 min (period=2)
// Input:       period, either 0,1, or 2 (see above)
// Output:      returns the load average, in case of failure, returns a value of -1
// Depends on:  static variables define globally 
//

double GetAverageLoad(int period)
{
  double loads[3];

  if ((period<0) || (period>2)) 
    {
      fprintf(stderr,"<W:%i> GetAverageLoad called with invalid period of %i\n", rank, period);
      fflush(stderr);
      return -1.;
    }
  if (getloadavg(loads,3)<0) return -1.;
  return loads[period];
}

//
// char* FindTheFilename(char *in)
// -------------------------------
//
// Description: Extract the filename of a string containing path+filename
// Input:       string containing path+filename "in"
// Output:      returns the filename
// Depends on:  none
//

char* FindTheFilename(char *in)
{
  int begin,cnt, length;
  char *out;

  length=0;
  while (1)
    {
      if (in[length++]=='\0')
        {
          break;
        }
    }

  begin=0;
  for (cnt=0; cnt<length; cnt++)
    {
      if (in[cnt]=='/')
        {
          begin=cnt+1;
        }
    }
  out = (char *) malloc((length-begin)*sizeof(char));

  sprintf(out,"%s",&in[begin]);
  return out ;
}

//
// double GetCPUTime(struct tms *ctb, struct tms *cte)
// ---------------------------------------------------
//
// Description: Calculate the CPU times
// Input:       ctb, cte - time information obtained via "times" (b-beginning, e-end)
// Output:      returns the amount of CPU time for a period between ctb and cte;
// Depends on:  none
//

double GetCPUTime(struct tms *ctb, struct tms *cte)
{
    return ((double) (( (double)((cte->tms_cutime+cte->tms_utime+cte->tms_cstime+cte->tms_stime) - (ctb->tms_cutime+ctb->tms_utime+ctb->tms_cstime+ctb->tms_stime)))/((double) sysconf(_SC_CLK_TCK))));
}

//
// void AddItemToList(thread_list *new)
// ------------------------------------
//
// Description: Add an item to a linked list of type thread_list
// Input:       new item to be added to the list "new"
// Output:      none
// Depends on:  static variables "head" and "tail"
//

void AddItemToList(thread_list *new)
{
  new->next=NULL;
  if (!head) 
    {
      head=new;
    }
  else
    {
      tail->next=new;
    }
  tail=new;
  return;
}

//
// void RemoveItemFromList(thread_list *item)
// ------------------------------------------
//
// Description: Remove an item from a linked list of type thread_list
// Input:       item to be removed from list, "item"
// Output:      none
// Depends on:  static variables "head" and "tail"
//

void RemoveItemFromList(thread_list *item)
{
  thread_list *check  = head;
  thread_list *before = NULL;

  if (!item) return;

  while (check) 
    {
      if (check == item)
	{
	  if (!before)
	    {
	      if (!check->next)
		{
		  head=tail=NULL;
		}
	      else
		{
		  head=check->next;
		}
	    }
	  else
	    {
	      if (!check->next)
		{
		  tail=before;
		  tail->next=NULL;
		}
	      else
		{
		  before->next=check->next;
		}
	    }
	  break;
	}
      before = check;
      check = check->next;
    }

  free(item);

  return;
}


//
// void* RunSystem(void *in)
// -------------------------
//
// Description: Thread task which runs a system call
// Input:       Pointer to a structure system_info
// Output:      exits and returns the output of system()
// Depends on:  none
//

void* RunSystem(void *in)
{
  system_info *info = (system_info *) in;

  info->retval=((system(info->command) >> 8) & 255);

  pthread_mutex_lock(&(info->mutex));
  pthread_cond_signal(&(info->cond));
  pthread_mutex_unlock(&(info->mutex));

  pthread_exit(NULL);
}

//
// int MakeSystemCallWithTimeOut(char *command, int to)
// ----------------------------------------------------
//
// Description: Makes a system call by creating thread calling RunSystem
// Input:       "command" for the system call, and timeout "to" (s)
// Output:      returns 0 in case of success, otherwise non-zero
// Depends on:  none
//

int MakeSystemCallWithTimeOut(char *command, int to)
{
  int             retval;
  system_info     system_call;
  struct timespec ts;
  struct timeval  tp;
  pthread_t       systemThread;

  system_call.command = command;

  pthread_mutex_init(&(system_call.mutex),NULL);
  pthread_cond_init(&(system_call.cond),NULL);

  if (verbose_mode)
    {
      printf("<W:%i> Making a system call \"%s\" with a timeout of %i secs\n",rank,system_call.command,to);
      fflush(stdout);
    }

  pthread_mutex_lock(&(system_call.mutex));

  retval = pthread_create(&systemThread,
			  NULL,
			  (void *) RunSystem,
			  (void *) &system_call);
  if (retval)
    {
      fprintf(stderr,"<W:%i> Error creating thread %d\n",rank,retval);

      pthread_mutex_unlock(&(system_call.mutex));
      pthread_mutex_destroy(&(system_call.mutex));
      pthread_cond_destroy(&(system_call.cond));
      return retval;
    }

  gettimeofday(&tp, NULL);
  ts.tv_sec  = tp.tv_sec;
  ts.tv_nsec = tp.tv_usec * 1000;
  ts.tv_sec += to;

  retval = pthread_cond_timedwait(&(system_call.cond), &(system_call.mutex), &ts);
  if (retval)
    {
      if (ETIMEDOUT == retval) 
        {
	  fprintf(stderr,"<W:%i> Time-out in waiting for system call \"%s\" to finish!\n",rank,command);
	  fflush(stderr);
	  
	  KillProcessAndDaughters(rank,system_call.command); 
        }
      else
        {
	  fprintf(stderr,"<W:%i> The value specified by cond, mutex or abstime in pthread_cond-timedwait is invalid.\n",rank);
	  fflush(stderr);
	  
	  KillProcessAndDaughters(rank,system_call.command); 
        }
    }
  else 
    {
      retval=system_call.retval;
    }

  pthread_mutex_unlock(&(system_call.mutex));

  pthread_join(systemThread, NULL);
    
  pthread_mutex_destroy(&(system_call.mutex));
  pthread_cond_destroy(&(system_call.cond));

  return retval;
}

//
// void* MoveJob(void *in)
// ------------------------
//
// Description: Thread task which moves the data from the buffer to the storage device
// Input:       Pointer to a structure "thread_info" containing the relevant information 
// Output:      exits the thread
// Depends on:  various static variables, thread created in "DoJob"
//

void* MoveJob(void *in)
{
  char       command[VERYLONGCHARSIZE];
  char       retval=0;
  double     now,start;
  struct tms cnow,cstart;
  int        msg[7];

  thread_info *info = (thread_info *) in;

  pthread_mutex_lock(&thread_mutex);

  thread_list *new = (thread_list *) malloc(sizeof(thread_list));
  new->thread_id=pthread_self();
  new->info=info;
  AddItemToList(new);

  pthread_mutex_unlock(&thread_mutex);

  start=((double) times(&cstart)/((double) sysconf(_SC_CLK_TCK)));

  if (!strcmp(info->output,"NULL"))
    {
      if (!keep_buffer)
	{
	  sprintf(command,"%s %s/%u NULL 0 1",info->movecmd,info->scratch,info->jobid);
	}
    }
  else
    {
      if (keep_buffer)
	{
	  sprintf(command,"%s %s/%u %s/%u 1 0",info->movecmd,info->scratch,info->jobid,info->output,info->jobid);
	}
      else
	{
	  sprintf(command,"%s %s/%u %s/%u 1 1",info->movecmd,info->scratch,info->jobid,info->output,info->jobid);
	}
    }

  if (!dummy_mode) 
    {
      retval=MakeSystemCallWithTimeOut(command,info->timeout);
      if (retval)
	{
	  fprintf(stderr,"<W:%i> Error executing \"%s\"; return value is %i\n",info->worker,command,retval);
	  fflush(stderr);

	  if (!keep_buffer)
	    {
	      sprintf(command,"%s %s/%u NULL 0 1",info->movecmd,info->scratch,info->jobid);
	      MakeSystemCallWithTimeOut(command,REMOVE_TIMEOUT);
	    }
	}
    }

  if (verbose_mode || dummy_mode)
    {
      printf("<W:%i> %s\n",info->worker,command);fflush(stdout);
    }

  if (0==retval)
    {
      msg[0]=JOB_OK;

      pthread_mutex_lock(&job_mutex);
      ndonejobs++;
      pthread_mutex_unlock(&job_mutex);
    }
  else
    {
      msg[0]=JOB_MOVE_ERROR;
    }

  now=((double) times(&cnow)/((double) sysconf(_SC_CLK_TCK)));

  msg[1]=info->jobid;
  msg[2]=(int) (now-start);
  msg[3]=(int) GetCPUTime(&cstart,&cnow);
  msg[4]=GetFreeDiskSpace();
  msg[5]=number_of_running_jobs;
  msg[6]=(int) (100*GetAverageLoad(0));

  if (verbose_mode)
    {
      printf("<W:%i> Sending copy result to boss: %i/%i/%i/%i/%i/%i/%i\n",info->worker,msg[0],msg[1],msg[2],msg[3],msg[4],msg[5],msg[6]);
      fflush(stdout);
    }
  MPI_Send (msg, 7, MPI_INT, BOSSRANK, MOVETAG, MPI_COMM_WORLD);

  if (verbose_mode)
    {
      printf("<W:%i> Move thread finished for run id %i\n",info->worker,info->jobid);
      fflush(stdout);
    }

  pthread_mutex_lock(&thread_mutex);

  RemoveItemFromList(new);

  if (info) free(info);

  number_of_running_jobs--;
 
  pthread_mutex_unlock(&thread_mutex);

  pthread_exit(NULL);
}

//
// int DoJob(unsigned int *info, job_description* job, unsigned int *time_elapsed)
// -------------------------------------------------------------------------------
//
// Description: Main routine which carries out the job given by the boss:
//              1) Copy input data if necessary
//              2) Run the script as requested in the job description
//              3) Move the output data to storage device --> thread process "MoveJob"
// Input:       Array "info" containing job ID (runid), 
//              a complete job description "job", and a pointer to elapsed time
// Output:      return "JOB_OK" on success, otherwise "JOB_ERROR". Also returns time elapsed and cputime.
// Depends on:  various static variables, function called by "DoWorker"
//

int DoJob(unsigned int *info, job_description *job, double *time_elapsed, double *time_comp)
{
  unsigned int  i;
  char            command[VERYLONGCHARSIZE],scratch_dir[VERYLONGCHARSIZE],retval;
  char *          scriptname=NULL;
  pthread_t *     moveThread;
  thread_info *   move_info=NULL;
  double          tb,te;
  struct tms      ctb,cte;

  setpriority(PRIO_PROCESS,getpid(),job->nice);

  tb=((double) times(&ctb)/((double) sysconf(_SC_CLK_TCK)));

  sprintf(scratch_path,"%s",&(job->array[3*JOBSTRINGSIZE]));	
  sprintf(move_files,"%s",&(job->array[4*JOBSTRINGSIZE]));	
  sprintf(scratch_dir,"%s/%u",scratch_path,info[0]);

  if (!dummy_mode) 
    {
      if (clear_scratch)
	{
	 if (0 == access(scratch_dir,F_OK))
	   {
	     if (verbose_mode || dummy_mode)
	       {
		 printf("<W:%i> Removing scratch directory %s\n",rank,scratch_dir);
		 fflush(stdout);
	       }
	     sprintf(command,"%s %s NULL 0 1",move_files,scratch_dir);
	     retval=MakeSystemCallWithTimeOut(command,REMOVE_TIMEOUT);
             if (retval)
	       {
		 fprintf(stderr,"<W:%i> Error removing directory \"%s\"\n",rank,scratch_dir);
		 fflush(stderr);
	       }
	   }
	}

      if (!(0==mkdir(scratch_dir,0777)))
        {
          fprintf(stderr,"<W:%i> Error creating directory \"%s\": %s\n",rank,scratch_dir,strerror(errno));
          fflush(stderr);
          return JOB_INPUT_ERROR;
        }

      if (!(0==chdir(scratch_dir)))
	{
          fprintf(stderr,"<W:%i> Error changing to directory \"%s\": %s\n",rank,scratch_dir,strerror(errno));
          fflush(stderr);
          return JOB_INPUT_ERROR;
        }
    }

  if (verbose_mode || dummy_mode)
    {
      printf("<W:%i> Creating and changing to directory %s\n",rank,scratch_dir);
      fflush(stdout);
    }

  /* Input */

  if (!strcmp(&(job->array[JOBSTRINGSIZE]),"NULL"))
    {
      if (verbose_mode || dummy_mode)
	{
	  printf("<W:%i> No input specified!\n",rank);
	  fflush(stdout);
	}
    }
  else
    {
      sprintf(command,"%s %s ../%u/. 1 0",move_files,&(job->array[JOBSTRINGSIZE]),info[0]);
      if (!dummy_mode)
	{
	  te=((double) times(&cte)/((double) sysconf(_SC_CLK_TCK)));
	  retval=MakeSystemCallWithTimeOut(command,(int) (job->timeout-(te-tb)));
	  if (retval)
	    {
	      fprintf(stderr,"<W:%i> Error executing \"%s\"; return value is %i\n",rank,command,retval);
	      fflush(stderr);

	      if (!keep_buffer)
		{
		  sprintf(command,"%s %s/%u NULL 0 1",move_files,scratch_path,info[0]);
		  retval=MakeSystemCallWithTimeOut(command,REMOVE_TIMEOUT);
		}
	      te=((double) times(&cte)/((double) sysconf(_SC_CLK_TCK)));
	      *time_elapsed=(te-tb);
	      *time_comp=GetCPUTime(&ctb,&cte);
	      return JOB_INPUT_ERROR;
	    }

	}
      if (verbose_mode || dummy_mode)
	{
	  printf("<W:%i> %s\n",rank,command);
	  fflush(stdout);
	} 
    }

  /* The script */

  /* Copy the script to the local scratch disk */

  sprintf(command,"%s %s ../%u/. 1 0",move_files,&(job->array[0]),info[0]);
  if (!dummy_mode)
    {
      te=((double) times(&cte)/((double) sysconf(_SC_CLK_TCK)));
      retval=MakeSystemCallWithTimeOut(command,(int) (job->timeout-(te-tb)));
      if (retval)
	{
	  fprintf(stderr,"<W:%i> Error executing \"%s\"; return value is %i\n",rank,command,retval);
	  fflush(stderr);

	  if (!keep_buffer)
	    {
	      sprintf(command,"%s %s/%u NULL 0 1",move_files,scratch_path,info[0]);
	      retval=MakeSystemCallWithTimeOut(command,REMOVE_TIMEOUT);
	    }  
	  te=((double) times(&cte)/((double) sysconf(_SC_CLK_TCK)));
	  *time_elapsed=(te-tb);
	  *time_comp=GetCPUTime(&ctb,&cte);
	  return JOB_INPUT_ERROR;
	}
    }

  if (verbose_mode || dummy_mode)
    {
      printf("<W:%i> %s\n",rank,command);
      fflush(stdout);
    } 
  
  scriptname = FindTheFilename(&(job->array[0]));

  sprintf(command,"./%s",scriptname);
  chmod(command,S_IRWXU); /* Set the file permissions properly */ 

  if (verbose_mode)
    {
      printf("<W:%i> The script filename is %s\n",rank,scriptname); 
      fflush(stdout);
    }

  /* Run the script */

  sprintf(command,"./%s %i",scriptname,info[0]);

  for (i=5; i<info[1]; i++)
    {
      sprintf(&command[strlen(command)]," %s",&(job->array[i*JOBSTRINGSIZE]));
    }

  if (scriptname) free(scriptname);

  if (!dummy_mode) 
    {
      te=((double) times(&cte)/((double) sysconf(_SC_CLK_TCK)));
      retval=MakeSystemCallWithTimeOut(command,(int) (job->timeout-(te-tb)));
      if (retval)
	{
	  fprintf(stderr,"<W:%i> Error executing \"%s\"; return value is %i\n",rank,command,retval);
	  fflush(stderr);

	  if (!keep_buffer)
	    {
	      sprintf(command,"%s %s/%u NULL 0 1",move_files,scratch_path,info[0]);
	      retval=MakeSystemCallWithTimeOut(command,REMOVE_TIMEOUT);
	    }
	  te=((double) times(&cte)/((double) sysconf(_SC_CLK_TCK)));
	  *time_elapsed=(te-tb);
	  *time_comp=GetCPUTime(&ctb,&cte);
	  return JOB_SCRIPT_ERROR;
	}
    }

  if (verbose_mode || dummy_mode)
    {
      printf("<W:%i> %s\n",rank,command);
      fflush(stdout);
    }
  
  /* Output: create a thread which moves the data in parallel */

  move_info=(thread_info *) malloc(sizeof(thread_info));
  move_info->jobid=info[0];
  move_info->worker=rank;
  te=((double) times(&cte)/((double) sysconf(_SC_CLK_TCK)));
  move_info->timeout=(int) job->timeout-(te-tb);
  sprintf(move_info->output,"%s",&(job->array[2*JOBSTRINGSIZE]));
  sprintf(move_info->movecmd,"%s",&(job->array[4*JOBSTRINGSIZE]));	
  sprintf(move_info->scratch,"%s",scratch_path);	

  if (verbose_mode)
    {
      printf("<W:%i> Creating thread for move job\n",rank); 
      fflush(stdout);
    }

  moveThread = (pthread_t *)malloc(sizeof(pthread_t));
  if (NULL==moveThread)
    {
      fprintf(stderr,"<W:%i> Error allocating memory for move thread\n",rank);
      te=((double) times(&cte)/((double) sysconf(_SC_CLK_TCK)));
      *time_elapsed=(te-tb);
      *time_comp=GetCPUTime(&ctb,&cte);
      return JOB_MOVE_ERROR;
    }

  retval = pthread_create(moveThread,
			  NULL,
			  (void *) MoveJob,
			  (void *) move_info);
  if (retval)
    {
      fprintf(stderr,"<W:%i> Error creating move thread %d\n",rank,retval);
      if (moveThread) free(moveThread);
      te=((double) times(&cte)/((double) sysconf(_SC_CLK_TCK)));
      *time_elapsed=(te-tb);
      *time_comp=GetCPUTime(&ctb,&cte);
      return JOB_MOVE_ERROR;
    }

  retval = pthread_detach(*moveThread);
  if (retval)
    {
      fprintf(stderr,"<W:%i> Error detaching move thread %d\n",rank,retval);
      if (moveThread) free(moveThread);
      te=((double) times(&cte)/((double) sysconf(_SC_CLK_TCK)));
      *time_elapsed=(te-tb);
      *time_comp=GetCPUTime(&ctb,&cte);
      return JOB_MOVE_ERROR;
    }

  if (moveThread) free(moveThread);

  te=((double) times(&cte)/((double) sysconf(_SC_CLK_TCK)));
  *time_elapsed=(te-tb); 
  *time_comp=GetCPUTime(&ctb,&cte);

  return JOB_OK;
}

//
// int GetAJob(FILE *fp, int *npar, int *barrier, job_description *job)
// --------------------------------------------------------------------
//
// Description: Routine which reads the next job from a job description file (called by boss process)
// Input:       File pointer of the job description file
// Output:      returns the number of jobs (split level), the number of parameters of a job "npar", 
//              a flag indicating whether a BARRIER command was found, and fills the structure "job"
// Depends on:  various static variables, function called by "DoBoss"
//

int GetAJob(FILE *fp, int *npar, int *barrier, job_description *job)
{
  char line[VERYLONGCHARSIZE],key[JOBSTRINGSIZE];
  int ores,njobs,i;

  (*barrier) = 0;

  while (1)
    {
      ores=fscanf(fp,"%s",key);
      if (EOF==ores) return 0;
      if (!strcmp(key,"JOB"))
        {
          *npar=5;
          fscanf(fp,"%s",key);
          njobs=atoi(key);
          fscanf(fp,"%s",&(job->array[0]));                          /* Script to run */
	  fscanf(fp,"%s",&(job->array[JOBSTRINGSIZE]));              /* Input */
          fscanf(fp,"%s",&(job->array[2*JOBSTRINGSIZE]));            /* Output */
	  sprintf(&(job->array[3*JOBSTRINGSIZE]),"%s",scratch_path); /* Scratch output */
	  sprintf(&(job->array[4*JOBSTRINGSIZE]),"%s",move_files);   /* Move command */
          fscanf(fp,"%s",key);
          (*npar)+=atoi(key);

	  if (((*npar)*JOBSTRINGSIZE*sizeof(char))>(job->size))
            {
	      if (verbose_mode)
		{
		  printf("<B> Reallocating job description memory %p:%i-->",job->array,job->size);
		}

	      job->array = (char *) realloc(job->array,(*npar)*JOBSTRINGSIZE*sizeof(char));
	      job->size  = (*npar)*JOBSTRINGSIZE*sizeof(char);

	      if (verbose_mode)
		{
		  printf("%p:%i\n",job->array,job->size);
		  fflush(stdout);
		}
            }
	  
          for (i=5; i<(*npar); i++)
            {
              fscanf(fp,"%s",&(job->array[i*JOBSTRINGSIZE]));
            }
          if (njobs) return njobs;
        }
      else if (!strcmp(key,"#")) /* Skip line */
        {
          if ((fgets(line,sizeof(line),fp))==NULL)
            {
              return 0;
            }
        }
      else if (!strcmp(key,"BARRIER")) /* Stop looking through database */
        {
	  (*barrier) = 1;
          return 0;
        }
      else if (!strcmp(key,"STOP")) /* Stop looking through database */
        {
	  fseek(fp,0,SEEK_END);
          return 0;
        }
      else if (!strcmp(key,"RUNID")) /* (re)set run ID */
        {
	  fscanf(fp,"%s",key);
          runid=atoi(key);

	  if (verbose_mode)
	    {
	      printf("<B> Changed run ID to %i\n",runid);
	    }
	}
      else if (!strcmp(key,"TIMEOUT")) /* Set time-out */
        {
	  fscanf(fp,"%s",key);
          timeout[0]=atoi(key);
	  timeout[1]=timeout[0];

	  if (verbose_mode)
	    {
	      printf("<B> Changed timeout to %i secs\n",timeout[0]);
	    }
	}
      else if (!strcmp(key,"TIMEOUTS")) /* Set time-outs */
        {
	  fscanf(fp,"%s",key);
          timeout[0]=atoi(key);
	  fscanf(fp,"%s",key);
	  timeout[1]=atoi(key);

	  if (verbose_mode)
	    {
	      printf("<B> Changed timeouts to %i/%i secs\n",timeout[0],timeout[1]);
	    }
	}
      else if (!strcmp(key,"SCRATCH")) /* Set scratch directory */
        {
	  fscanf(fp,"%s",key);
	  sprintf(scratch_path,"%s",key);

	  if (verbose_mode)
	    {
	      printf("<B> Changed scratch directory to %s\n",scratch_path);
	    }
	}
      else if (!strcmp(key,"MOVECMD")) /* Set move program */
        {
	  fscanf(fp,"%s",key);
	  sprintf(move_files,"%s",key);

	  if (verbose_mode)
	    {
	      printf("<B> Changed move program to %s\n",move_files);
	    }
	}
      else if (!strcmp(key,"MINDISKSPACE")) /* Set minimum required disk space */
        {
	  fscanf(fp,"%s",key);
	  minimum_disk_space=atoi(key);

	  if (verbose_mode)
	    {
	      printf("<B> Changed minimum required disk space to %i MBytes\n",minimum_disk_space);
	    }
	}
      else if (!strcmp(key,"MAXRUNTHREADS")) /* Set maximum allowed running threads */
        {
	  fscanf(fp,"%s",key);
	  maximum_running_jobs=atoi(key);

	  if (verbose_mode)
	    {
	      printf("<B> Changed maximum allowed running threads to %i\n",maximum_running_jobs);
	    }
	}
      else if (!strcmp(key,"MAXLOAD")) /* Set maximum allowed load */
        {
	  fscanf(fp,"%s",key);
	  maximum_load=atof(key);

	  if (verbose_mode)
	    {
	      printf("<B> Changed maximum allowed load to %.2f\n",maximum_load);
	    }
	}
      else if (!strcmp(key,"NICE")) /* Set nice level */
        {
	  fscanf(fp,"%s",key);
	  nice_level=atoi(key);

	  if (verbose_mode)
	    {
	      printf("<B> Changed nice level to %d\n",nice_level);
	    }
	}
    }
}

//
// void DoBoss(FILE *fp, FILE *fp_log, int nworkers, double* wtime, 
//             double *cputime, double *cputime_jobs_success, int* reqjobs)
// ------------------------------------------------------------------------
//
// Description: Main running loop of the boss process: listen to request of a worker and answers him accordingly
// Input:       File pointer to the job description file, pointer to log file, number of workers
// Output:      Total time needed by the boss (walltime, cputime, and cputime of successful jobs), and 
//              the total number of requested jobs
// Depends on:  Various global variables
//

void DoBoss(FILE *fp, FILE *fp_log, int nworkers, double* wtime, 
	    double *cputime, double *cputime_jobs_success, int* reqjobs)
{
  int             msg[7],nacworkers,npar;
  int             i,barrier,number_of_idle_workers;
  int             njobs=0, ndonescripts, nrunjobs, nfailjobs;
  unsigned int    buf[4];
  char            jobstatus[16],jobinfo[16];
  char            timestring[16];
  job_description job;
  MPI_Status      status;
  time_t          time_now;
  struct tm *     time_ptr;
  double          now,start;
  struct tms      cnow,cstart;

  job.array = (char *) malloc(DEFAULT_JOBSIZE*JOBSTRINGSIZE*sizeof(char));
  job.size  = DEFAULT_JOBSIZE*JOBSTRINGSIZE*sizeof(char);

  start=((double) times(&cstart)/((double) sysconf(_SC_CLK_TCK)));

  (*reqjobs)=nrunjobs=nfailjobs=ndonejobs=ndonescripts=0;

  nacworkers=nworkers;
  barrier=0;
  number_of_idle_workers=0;
  *cputime_jobs_success=0;

  fprintf(fp_log,"-------------------------------------------------------------------------------------------------------------------\n");
  fprintf(fp_log,"HH:MM:SS\tLink\t\tAction\t\tWhat\t\tRun ID\t\tWall Time\tCPU%%\tRUN/OK/FAIL\n");
  fprintf(fp_log,"-------------------------------------------------------------------------------------------------------------------\n");
  fflush(fp_log);

  while (nacworkers)
    {
      if ((!njobs) && (!barrier))
	{
	  njobs=GetAJob(fp,&npar,&barrier,&job);
	  
	  if (verbose_mode)
	    {
	      printf("<B> Got job from database with multiplicity %i and barrier %i\n",njobs,barrier);
	      fflush(stdout);
	      if (njobs)
		{
		  for (i=0; i<npar; i++)
		    {
		      printf("\t%i=%s\n",i,&(job.array[i*JOBSTRINGSIZE]));
		      fflush(stdout);
		    }
		}  
	    }
	}
      
      if (verbose_mode)
	{
	  printf("<B> Start listening...\n"); 
	  fflush(stdout);
	}

      MPI_Recv(msg,7,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

      time(&time_now);
      time_ptr = localtime(&time_now);
      strftime(timestring, 16, "%H:%M:%S", time_ptr);

      if (verbose_mode)
	{
	  printf("<B> Received message from worker %i with tag %i and msg=%i/%i/%i/%i/%i/%i/%i\n",
		 status.MPI_SOURCE,status.MPI_TAG,msg[0],msg[1],msg[2],msg[3],msg[4],msg[5],msg[6]);
	  fflush(stdout);
	}

      switch (status.MPI_TAG)
	{
	case BARRIERTAG:
	  if (WORKER_IDLE==msg[0])
	    {
	      number_of_idle_workers++;
	      
	      if (verbose_mode)
		{
		  printf("<B> Number of idle workers is %i\n",number_of_idle_workers);
		  fflush(stdout);
		}
	      
	      if (number_of_idle_workers==nworkers)
		{
		  if (verbose_mode)
		    {
		      printf("<B> Wait for barrier...\n");
		      fflush(stdout);
		    }

		  MPI_Barrier(MPI_COMM_WORLD);

		  if (verbose_mode)
		    {
		      printf("<B> Barrier released!\n");
		      fflush(stdout);
		    }
		  
		  barrier=0;
		  number_of_idle_workers=0;
		}
	    }
	  break;

	case REQUESTTAG: 
	  if (verbose_mode)
	    {
	      printf("<B> Received request tag from worker %i\n",status.MPI_SOURCE);
	      fflush(stdout);
	    }
	  
	  if (barrier)
	    {
	      if (verbose_mode)
		{
		  printf("<B> Sending barrier signal to worker %i\n",status.MPI_SOURCE);
		  fflush(stdout);
		}
	      
	      buf[0]=buf[1]=buf[2]=buf[3]=0;
	      
	      MPI_Send (buf, 4, MPI_UNSIGNED, status.MPI_SOURCE, BARRIERTAG, MPI_COMM_WORLD);  
	      
	      fprintf(fp_log,"%s\tB ---> W%i\tWAIT\t\tBARRIER\t\t-\t\t-\t\t-\t%i/%i/%i\n",
		      timestring,status.MPI_SOURCE,nrunjobs,ndonejobs,nfailjobs);
	      fflush(fp_log);
	    }
	  else
	    {
	      if (njobs)
		{
		  /* Not enough disk space available, too many pending jobs, or too much load, put worker to sleep! */
		  if ((msg[4]<minimum_disk_space && msg[4]>0) || msg[5]>=maximum_running_jobs || msg[6]>=((int) 100*maximum_load)) 
		    {
		      if (verbose_mode)
			{
			  printf("<B> Sending sleep signal to worker %i\n",status.MPI_SOURCE);
			  fflush(stdout);
			}

		      buf[0]=buf[1]=buf[3]=0;
		      buf[2]=timeout[1];

		      if (msg[4]<minimum_disk_space) 
			{
			  sprintf(jobstatus,"DISK FULL   ");		    
			  buf[0] |= (1<<0);
			}
		      if (msg[5]>=maximum_running_jobs) 
			{
			  sprintf(jobstatus,"PENDING     ");		    
			  buf[0] |= (1<<1);
			}
		      if (msg[6]>=((int) (100*maximum_load))) 
			{
			  sprintf(jobstatus,"OVERLOAD    ");		    
			  buf[0] |= (1<<2);
			}

		      MPI_Send (buf, 4, MPI_UNSIGNED, status.MPI_SOURCE, SLEEPTAG, MPI_COMM_WORLD);
		      
		      fprintf(fp_log,"%s\tB ---> W%i\tSLEEP\t\t%s\t-\t\t%.2i:%.2i:%.2i\t-\t%i/%i/%i\n",
			      timestring,status.MPI_SOURCE,jobstatus,
			      buf[2]/3600,((buf[2]%3600)/60),((buf[2]%3600)%60),
			      nrunjobs,ndonejobs,nfailjobs);
		      fflush(fp_log);
		    }
		  else
		    {
		      buf[0]=runid;
		      buf[1]=npar;
		      buf[2]=timeout[0];
		      buf[3]=(unsigned int) nice_level;
		      
		      if (verbose_mode)
			{
			  printf("<B> Sending job to worker %i, %i/%i/%i/%i\n",
				 status.MPI_SOURCE,buf[0],buf[1],buf[2],buf[3]);
			  fflush(stdout);
			}
		      (*reqjobs)++;
		      
		      MPI_Send (buf, 4, MPI_UNSIGNED, status.MPI_SOURCE, TASKTAG, MPI_COMM_WORLD);
		      MPI_Send (job.array, (buf[1]*JOBSTRINGSIZE), MPI_CHAR, status.MPI_SOURCE, TASKTAG, MPI_COMM_WORLD);
		      
		      nrunjobs++;

		      fprintf(fp_log,"%s\tB ---> W%i\tRUN\t\tJOB\t\t%i\t\t-\t\t-\t%i/%i/%i\n",
			      timestring,status.MPI_SOURCE,buf[0],nrunjobs,ndonejobs,nfailjobs);
		      fflush(fp_log);

		      runid++;
		      njobs--;  
		    }
		}
	      else
		{
		  if (verbose_mode)
		    {
		      printf("<B> Sending abort signal to worker %i\n",status.MPI_SOURCE);
		      fflush(stdout);
		    }
		  
		  buf[0]=buf[1]=buf[2]=buf[3]=0;
		  MPI_Send (buf, 4, MPI_UNSIGNED, status.MPI_SOURCE, ABORTTAG, MPI_COMM_WORLD);  
	      
		  fprintf(fp_log,"%s\tB ---> W%i\tABORT\t\tNO JOBS\t\t-\t\t-\t\t-\t%i/%i/%i\n",
			  timestring,status.MPI_SOURCE,nrunjobs,ndonejobs,nfailjobs);
		  fflush(fp_log);
		}
	    }
	  break;

	case SCRIPTTAG:
	  if (JOB_OK==msg[0])
	    {
	      sprintf(jobstatus,"DONE    ");
	      sprintf(jobinfo,  "SCRIPT");
	      
	      ndonescripts+=msg[0];
	      *cputime_jobs_success+=(double) msg[3];
	    }
	  else
	    {
	      sprintf(jobstatus,"FAILED  ");
	      
	      switch (msg[0])
		{
		case JOB_INPUT_ERROR:
		  sprintf(jobinfo, "CP INP");
		  break;
		
		case JOB_SCRIPT_ERROR:
		  sprintf(jobinfo, "SCRIPT");
		  break;

		case JOB_MOVE_ERROR:
		  sprintf(jobinfo, "MOVE  ");
		  break;

		default:
		  sprintf(jobinfo, "JOB   ");
		  break;
		}

	      nrunjobs--;
	      nfailjobs++;
	    }
	  
	  if (msg[2])
	    {
	      fprintf(fp_log,"%s\tB <--- W%i\t%s\t%s\t\t%i\t\t%.2i:%.2i:%.2i\t%.0f\t%i/%i/%i\n",
		      timestring,status.MPI_SOURCE,jobstatus,jobinfo,msg[1],
		      msg[2]/3600,((msg[2]%3600)/60),((msg[2]%3600)%60),
		      100*((float) msg[3])/((float) msg[2]),
		      nrunjobs,ndonejobs,nfailjobs);
	    }
	  else
	    {
	      fprintf(fp_log,"%s\tB <--- W%i\t%s\t%s\t\t%i\t\t%.2i:%.2i:%.2i\t-\t%i/%i/%i\n",
		      timestring,status.MPI_SOURCE,jobstatus,jobinfo,msg[1],
		      msg[2]/3600,((msg[2]%3600)/60),((msg[2]%3600)%60),
		      nrunjobs,ndonejobs,nfailjobs);
	    }
	  fflush(fp_log);
	  break;

	case MOVETAG:
	  nrunjobs--;
	  
	  if (verbose_mode)
	    {
	      printf("<B> Received copy tag from worker %i\n",status.MPI_SOURCE);
	      fflush(stdout);
	    }

	  if (JOB_OK==msg[0])
	    {
	      sprintf(jobstatus,"DONE        ");
	      ndonejobs++;
	      *cputime_jobs_success+=(double) msg[3];
	    }
	  else
	    {
	      sprintf(jobstatus,"FAILED      ");
	      nfailjobs++;
	    }

	  if (msg[2])
	    {
	      fprintf(fp_log,"%s\tB <--- W%i\t%s\tMOVE\t\t%i\t\t%.2i:%.2i:%.2i\t%.0f\t%i/%i/%i\n",
		      timestring,status.MPI_SOURCE,jobstatus,msg[1],
		      msg[2]/3600,((msg[2]%3600)/60),((msg[2]%3600)%60),
		      100*((float) msg[3])/((float) msg[2]),
		      nrunjobs,ndonejobs,nfailjobs);
	    }
	  else
	    {
	      fprintf(fp_log,"%s\tB <--- W%i\t%s\tMOVE\t\t%i\t\t%.2i:%.2i:%.2i\t-\t%i/%i/%i\n",
		      timestring,status.MPI_SOURCE,jobstatus,msg[1],
		      msg[2]/3600,((msg[2]%3600)/60),((msg[2]%3600)%60),
		      nrunjobs,ndonejobs,nfailjobs);
	    }
	  fflush(fp_log);
	  break;

	case FINISHTAG:
	  if (verbose_mode)
	    {
	      printf("<B> Received finish tag from worker %i\n",status.MPI_SOURCE);
	      fflush(stdout);
	    }

	  if (msg[2])
	    {
	      fprintf(fp_log,"%s\tB <--- W%i\tFINISHED\tALL\t\t-\t\t%.2i:%.2i:%.2i\t%.0f\t%i/%i/%i\n",
		      timestring,status.MPI_SOURCE,
		      msg[2]/3600,((msg[2]%3600)/60),((msg[2]%3600)%60),
		      100*((float) msg[3])/((float) msg[2]),
		      nrunjobs,ndonejobs,nfailjobs);
	    }
	  else
	    {
	      fprintf(fp_log,"%s\tB <--- W%i\tFINISHED\tALL\t\t-\t\t%.2i:%.2i:%.2i\t-\t%i/%i/%i\n",
		      timestring,status.MPI_SOURCE,
		      msg[2]/3600,((msg[2]%3600)/60),((msg[2]%3600)%60),
		      nrunjobs,ndonejobs,nfailjobs);
	    }
	  fflush(fp_log);

	  nacworkers--;
	  break;

	default:	
	  fprintf(stderr,"<B> Error: Unknown tag %i from worker %i\n",status.MPI_TAG,status.MPI_SOURCE);
	  fflush(stderr);
	  break;
	}
    }

  fprintf(fp_log,"-------------------------------------------------------------------------------------------------------------------\n");
  fflush(fp_log);

  now=((double) times(&cnow)/((double) sysconf(_SC_CLK_TCK)));
  (*wtime) = now - start;
  (*cputime) = ((double)((cnow.tms_cutime+cnow.tms_utime+cnow.tms_cstime+cnow.tms_stime) - (cstart.tms_cutime+cstart.tms_utime+cstart.tms_cstime+cstart.tms_stime)))/((double) sysconf(_SC_CLK_TCK));

  if (job.array) free(job.array);

  return;
}

//
// void DoWorker(double* wtime, double *cputime, double * *cputime_jobs_success, int* reqjobs)
// -------------------------------------------------------------------------------------------
//
// Description: Main running loop of the worker process: sends and retrieves a request to boss
// Input:       none
// Output:      Total time needed by the worker (walltime+cputime+cputime spent in successful jobs), 
//              and the total number of requested jobs
// Depends on:  Various global variables
//

void DoWorker(double* wtime, double *cputime, double *cputime_jobs_success, int* reqjobs)
{
  int             msg[7], abort;
  unsigned int    buf[4];
  MPI_Status      status;
  double          elapse_time, comp_time;
  job_description job;
  double          now,start;
  struct tms      cnow,cstart;

  job.array = (char *) malloc(DEFAULT_JOBSIZE*JOBSTRINGSIZE*sizeof(char));
  job.size  = DEFAULT_JOBSIZE*JOBSTRINGSIZE*sizeof(char);

  start=((double) times(&cstart)/((double) sysconf(_SC_CLK_TCK)));

  *reqjobs=0;
  abort=0;
  *cputime_jobs_success=0;

  pthread_mutex_lock(&job_mutex);
  ndonejobs=0;
  pthread_mutex_unlock(&job_mutex);

  pthread_mutex_lock(&thread_mutex);
  number_of_running_jobs=0;
  pthread_mutex_unlock(&thread_mutex);

  while (!abort)
    {
      msg[0]=JOB_REQUEST;
      msg[1]=-1;
      msg[2]=0;
      msg[3]=0;
      msg[4]=GetFreeDiskSpace();
      msg[5]=number_of_running_jobs;
      msg[6]=(int) (100*GetAverageLoad(0));

      /* Request master for a task */
      if (verbose_mode)
	{
	  printf("<W:%i> Sending request to boss %i/%i/%i/%i/%i/%i/%i\n",rank,msg[0],msg[1],msg[2],msg[3],msg[4],msg[5],msg[6]); 
	  fflush(stdout);
	}

      MPI_Send(msg, 7, MPI_INT, BOSSRANK, REQUESTTAG, MPI_COMM_WORLD);

      MPI_Recv(buf, 4, MPI_UNSIGNED, BOSSRANK, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      if (verbose_mode)
	{
	  printf("<W:%i> Received message from boss %i with tag %i\n",
		 rank,status.MPI_SOURCE,status.MPI_TAG);
	  fflush(stdout);
	  printf("<W:%i> Message is %i/%i/%i\n",
		 rank,buf[0],buf[1],buf[2]);
	  fflush(stdout);
	}

      switch (status.MPI_TAG)
	{
      /* Is message a stop or barrier message? */
	case ABORTTAG:
	case BARRIERTAG:
	  if (verbose_mode)
	    {
	      printf("<W:%i> Waiting for all processes to finish\n",rank);
	      fflush(stdout);
	    }

          while (number_of_running_jobs)
            {
	      if (verbose_mode)
		{
		  pthread_mutex_lock(&thread_mutex);

		  thread_list *check = head;		  
		  while (check)
		    {
		      printf("<W:%i> Job with id=%i still moving data\n",rank,check->info->jobid);
		      fflush(stdout);

		      check=check->next;
		    }
		  pthread_mutex_unlock(&thread_mutex);
		}
              sleep(SLEEPTIME);
            }

          if (status.MPI_TAG == ABORTTAG)
	    {
	      if (verbose_mode)
		{
		  printf("<W:%i> Aborting...\n",rank);
		  fflush(stdout);
		}
	      abort=1;
	    }
	  else /* Barrier */
	    {
	      msg[0]=WORKER_IDLE;
	      msg[1]=-1;
	      msg[2]=0;
	      msg[3]=0;
	      msg[4]=GetFreeDiskSpace();
	      msg[5]=number_of_running_jobs;
	      msg[6]=(int) (100*GetAverageLoad(0));	  

	      if (verbose_mode)
		{
		  printf("<W:%i> Sending barrier response to boss %i/%i/%i/%i/%i/%i/%i\n",rank,msg[0],msg[1],msg[2],msg[3],msg[4],msg[5],msg[6]); 
		  fflush(stdout);
		}

	      MPI_Send(msg, 7, MPI_INT, BOSSRANK, BARRIERTAG, MPI_COMM_WORLD);

	      if (verbose_mode)
		{
		  printf("<W:%i> Wait for barrier...\n",rank); 
		  fflush(stdout);
		}

	      MPI_Barrier(MPI_COMM_WORLD);
	    }
	  break;

      /* Is message a sleep signal? */
	case SLEEPTAG:
   	  if (verbose_mode)
	    {
	      printf("<W:%i> Worker put to sleep for %i secs\n",rank,buf[2]);
	      fflush(stdout);
	      if (buf[0] & (1<<0))
		{
		  printf("<W:%i> Reason: not enough buffer disk space available: %i\n",rank,GetFreeDiskSpace());
		  fflush(stdout);
		}
	      if (buf[0] & (1<<1))
		{
		  printf("<W:%i> Reason: too many pending jobs: %i\n",rank,number_of_running_jobs);
		  fflush(stdout);
		}
	      if (buf[0] & (1<<2))
		{
		  printf("<W:%i> Reason: worker is overloaded: %.2f\n",rank,GetAverageLoad(0));
		  fflush(stdout);
		}
	    }

	  sleep(buf[2]);
	  break;

	case TASKTAG:
       	  if ((buf[1]*JOBSTRINGSIZE*sizeof(char))>job.size)
	    {
	      if (verbose_mode)
		{
		  printf("<W:%i> Reallocating job description memory %p:%i-->",rank,job.array,job.size);
		}
	      
	      job.array   = (char *) realloc(job.array,buf[1]*JOBSTRINGSIZE*sizeof(char));
	      job.size    = buf[1]*JOBSTRINGSIZE*sizeof(char);
	      job.timeout = buf[2];
	      job.nice    = (int) buf[3];

	      if (verbose_mode)
		{
		  printf("%p:%i\n",job.array,job.size);
		  fflush(stdout);
		}
	    }

	  MPI_Recv(job.array, buf[1]*JOBSTRINGSIZE, MPI_CHAR, BOSSRANK, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	  (*reqjobs)++;

	  pthread_mutex_lock(&thread_mutex);
	  number_of_running_jobs++;
	  pthread_mutex_unlock(&thread_mutex);	  
	  
	  msg[0]=DoJob(buf,&job,&elapse_time,&comp_time);

	  if (!(JOB_OK == msg[0]))
	    {
	      pthread_mutex_lock(&thread_mutex);
	      number_of_running_jobs--;
	      pthread_mutex_unlock(&thread_mutex);
	    }
	  else
	    {
	      *cputime_jobs_success+=comp_time;
	    }
	  
	  msg[1]=buf[0]; /* Also return RUNID */
	  msg[2]=(int) elapse_time;
	  msg[3]=(int) comp_time;
	  msg[4]=GetFreeDiskSpace();
	  msg[5]=number_of_running_jobs;
	  msg[6]=(int) (100*GetAverageLoad(0));

	  if (verbose_mode)
	    {
	      printf("<W:%i> Sending script result to boss %i/%i/%i/%i/%i/%i/%i\n",rank,msg[0],msg[1],msg[2],msg[3],msg[4],msg[5],msg[6]); 
	      fflush(stdout);
	    }

	  MPI_Send(msg, 7, MPI_INT, BOSSRANK, SCRIPTTAG, MPI_COMM_WORLD);
	  break;

	default:
	  fprintf(stderr,"<W:%i> Error: Unknown tag %i from boss\n",rank,status.MPI_TAG);
	  fflush(stderr);
	  break;
	}
    }
  
  if (verbose_mode)
    {
      printf("<W:%i> Sending finish signal to BOSS\n",rank);
      fflush(stdout);
    }

  now=((double) times(&cnow)/((double) sysconf(_SC_CLK_TCK)));
  (*wtime) = now - start;
  (*cputime) = GetCPUTime(&cstart,&cnow);

  msg[0]=0;msg[1]=-1;msg[2]=(int) (*wtime);msg[3]=(int) (*cputime);msg[4]=GetFreeDiskSpace();msg[5]=number_of_running_jobs;
  msg[6]=(int) (100*GetAverageLoad(0));

  MPI_Send (msg, 7, MPI_INT, BOSSRANK, FINISHTAG, MPI_COMM_WORLD);

  if (verbose_mode)
    {
      printf("<W:%i> Accomplished %i jobs\n",rank,ndonejobs); 
      fflush(stdout);
    }

  if (job.array) free(job.array);

  return ;
}

//
// MAIN: Read command line arguments and, depending on rank number, starts a "DoBoss" or "DoWorker" process
//       At the end, statistics are printed
// 

int main(int argc, char *argv[])
{
  int      i,size,nworkers,nreqjobs;;
  double   wtime,cputime,sumwtime,sumcputime,
           cputime_success;
  int *    nrjobsbuf=NULL;
  int *    nrreqjobsbuf=NULL;
  double * wtimebuf=NULL;
  double * cputimebuf=NULL;
  double * cputimeokbuf=NULL;
  char *   hostnamebuf=NULL;
  char     hostname[MAX_HOSTNAME_LENGTH];
  char     jobslog_filename[VERYLONGCHARSIZE];
  FILE *   fp_jobfile;
  FILE *   fp_jobfile_log;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (!ReadArguments(argc,argv))
    {
      MPI_Abort(MPI_COMM_WORLD,errno);
    }

  if (size<2)
    {
      fprintf(stderr,"<E> What do you expect? That the boss does the work himself? Create at least two processes!\n"); 
      fflush(stderr);
      MPI_Abort(MPI_COMM_WORLD,errno);
      exit(-1);
    }

  if (BOSSRANK==rank) // Boss
    {
      fp_jobfile = fopen(jobdescription_filename,"r");

      if (!fp_jobfile)
	{
	  fprintf(stderr,"<E> Error opening job description file %s\n",jobdescription_filename); 
	  fflush(stderr);
	  MPI_Abort(MPI_COMM_WORLD,errno);
	  exit(-1);
	}

      sprintf(jobslog_filename,"%s.log",jobdescription_filename);
      fp_jobfile_log = fopen(jobslog_filename,"w");

      if (!fp_jobfile_log)
	{
	  fprintf(stderr,"<E> Error opening jobs log file %s\n",jobslog_filename); 
	  fflush(stderr);
	  MPI_Abort(MPI_COMM_WORLD,errno);
	  exit(-1);
	}

      nworkers=size-1;
      
      DoBoss(fp_jobfile, fp_jobfile_log, nworkers, &wtime, &cputime, &cputime_success, &nreqjobs);

      nrjobsbuf    = (int*)    malloc(size*sizeof(int));
      nrreqjobsbuf = (int*)    malloc(size*sizeof(int));
      wtimebuf     = (double*) malloc(size*sizeof(double));
      cputimebuf   = (double*) malloc(size*sizeof(double));
      cputimeokbuf = (double*) malloc(size*sizeof(double));
      hostnamebuf  = (char*)   malloc(size*MAX_HOSTNAME_LENGTH*sizeof(char));

    }
  else         // Worker 
    {
      DoWorker(&wtime, &cputime, &cputime_success, &nreqjobs);
    }

  gethostname(hostname,sizeof(hostname));
  
  MPI_Gather(&ndonejobs,       1, MPI_INT,    nrjobsbuf,     1, MPI_INT,    BOSSRANK, MPI_COMM_WORLD);
  MPI_Gather(&nreqjobs,        1, MPI_INT,    nrreqjobsbuf,  1, MPI_INT,    BOSSRANK, MPI_COMM_WORLD);
  MPI_Gather(&wtime,           1, MPI_DOUBLE, wtimebuf,      1, MPI_DOUBLE, BOSSRANK, MPI_COMM_WORLD);
  MPI_Gather(&cputime,         1, MPI_DOUBLE, cputimebuf,    1, MPI_DOUBLE, BOSSRANK, MPI_COMM_WORLD);
  MPI_Gather(&cputime_success, 1, MPI_DOUBLE, cputimeokbuf,  1, MPI_DOUBLE, BOSSRANK, MPI_COMM_WORLD);
  MPI_Gather(hostname,         MAX_HOSTNAME_LENGTH, MPI_CHAR, 
	     hostnamebuf,      MAX_HOSTNAME_LENGTH, MPI_CHAR, 
	     BOSSRANK,         MPI_COMM_WORLD);


  if (BOSSRANK==rank)
    {
      sumwtime=0;
      sumcputime=0;
      fprintf(fp_jobfile_log,"Proc\tWall time\tCPU time\tReq. jobs\tAccompl. jobs\tHostname\n");
      fprintf(fp_jobfile_log,"-------------------------------------------------------------------------------------------------------------------\n");
      for (i=0; i<size; i++)
	{
	  if (BOSSRANK!=i) sumwtime+=wtimebuf[i];
	  if (BOSSRANK!=i) sumcputime+=cputimeokbuf[i];
	  fprintf(fp_jobfile_log,"%i\t%.2i:%.2i:%.2i\t%.2i:%.2i:%.2i\t%i\t\t%i\t\t%s",i,
		  ((int)wtimebuf[i])/3600,((((int)wtimebuf[i])%3600)/60),((((int)wtimebuf[i])%3600)%60),
		  ((int)cputimebuf[i])/3600,((((int)cputimebuf[i])%3600)/60),((((int)cputimebuf[i])%3600)%60),
		  nrreqjobsbuf[i],nrjobsbuf[i],&hostnamebuf[i*MAX_HOSTNAME_LENGTH]);
	  if (BOSSRANK==i) 
	    {
	      fprintf(fp_jobfile_log,"\t(BOSS)\n");
	    }
	  else
	    {
	      fprintf(fp_jobfile_log,"\n");
	    }
	}
      fprintf(fp_jobfile_log,"-------------------------------------------------------------------------------------------------------------------\n");
      if (sumwtime)
      {
	  fprintf(fp_jobfile_log,"SUM CPU time of OK jobs..........................\t%id:%ih:%im:%is (%.0f%% of SUM Wall time)\n",
		  (((int)sumcputime)/3600)/24,(((int)sumcputime)/3600)%24,((((int)sumcputime)%3600)/60),((((int)sumcputime)%3600)%60),
	          100.*sumcputime/sumwtime);
      }
      if (wtimebuf[BOSSRANK])
      {
	  fprintf(fp_jobfile_log,"Speed-up (SUM CPU time OK jobs/Wall time boss)...\t%.2f\n",cputimeokbuf[BOSSRANK]/wtimebuf[BOSSRANK]);
      }
      fprintf(fp_jobfile_log,"-------------------------------------------------------------------------------------------------------------------\n");

      fclose(fp_jobfile);
      fclose(fp_jobfile_log);
      
      if (nrjobsbuf)    free(nrjobsbuf);
      if (nrreqjobsbuf) free(nrreqjobsbuf);
      if (wtimebuf)     free(wtimebuf);
      if (cputimebuf)   free(cputimebuf);
      if (cputimeokbuf) free(cputimeokbuf);
      if (hostnamebuf)  free(hostnamebuf);
    }

  MPI_Finalize();

  exit(EXIT_SUCCESS);
}
