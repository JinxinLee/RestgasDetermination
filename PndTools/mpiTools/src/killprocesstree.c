/*	This is derived from pstree written by Fred Hucht (c) 1993-2007	*
 *	EMail: fred AT thp.Uni-Duisburg.de			        *
 *	Feel free to copy and redistribute in terms of the	        *
 * 	GNU public license. 					        *
 *                                                                      *
 * $Id: pstree.c,v 2.32 2007-10-26 21:39:50+02 fred Exp $               *
 *                                                                      *
 * 29/05/08 - modified by JGM for usage in combi with boss_worker_mpi   *
 *            purpose: kill processes (incl. daughters) in case of      *
 *                     time-out                                         *
 * 24/03/10 - only use SIGTERM and SIGKILL signals to terminate         * 
 *                                                                      *
 */
#define MAXLINE 1024
#define SLEEPTIME 1

extern int  dummy_mode;    /* Variable initialized in boss_worker_mpi.c */
extern int  verbose_mode;  /* Variable initialized in boss_worker_mpi.c */

#if defined(_AIX) || defined(___AIX)	/* AIX >= 3.1 */
/* Under AIX, we directly read the process table from the kernel */
# ifndef _AIX50
/* problems with getprocs() under AIX 5L
 * workaround contributed by Chris Benesch <chris AT fdbs.com> */
#  define USE_GetProcessesDirect
# endif /*_AIX50*/
#  define HAS_TERMDEF
extern char *termdef(int, char);
#  define _ALL_SOURCE
#  include <procinfo.h>
#  define USE_GETPROCS

#  ifdef USE_GETPROCS
#    define IFNEW(a,b) a
#    define ProcInfo procsinfo
extern getprocs(struct procsinfo *, int, struct fdsinfo *, int, pid_t *, int);
#  else /*USE_GETPROCS*/
#    define IFNEW(a,b) b
#    define ProcInfo procinfo
extern getproc(struct procinfo *, int, int);
extern getuser(struct procinfo *, int, void *, int);
#  endif /*USE_GETPROCS*/

extern getargs(struct ProcInfo *, int, char *, int);

/*#  define PSCMD 	"ps -ekf"
  #  define PSFORMAT 	"%s %ld %ld %*20c %*s %[^\n]"*/
#  define HAS_PGID
#  define UID2USER
#  define PSCMD 	"ps -eko uid,pid,ppid,pgid,thcount,args"
#  define PSFORMAT 	"%ld %ld %ld %ld %ld %[^\n]"
#  define PSVARS	&P[i].uid, &P[i].pid, &P[i].ppid, &P[i].pgid, &P[i].thcount, P[i].cmd
#  define PSVARSN	6
/************************************************************************/
#elif defined(__linux) || (defined __alpha && defined(_SYSTYPE_BSD) || defined (Tru64))
/* TRU64 contributed by Frank Parkin <fparki AT acxiom.co.uk>
 */
#  ifdef __linux
#    define USE_GetProcessesDirect
#    include <glob.h>
#    include <sys/stat.h>
#  endif
#  define UID2USER
#  define HAS_PGID
#  define PSCMD 	"ps -eo uid,pid,ppid,pgid,args"
#  define PSFORMAT 	"%ld %ld %ld %ld %[^\n]"
#  define PSVARS	&P[i].uid, &P[i].pid, &P[i].ppid, &P[i].pgid, P[i].cmd
#  define PSVARSN	5
/************************************************************************/
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
/* NetBSD contributed by Gary D. Duzan <gary AT wheel.tiac.net>
 * FreeBSD contributed by Randall Hopper <rhh AT ct.picker.com> 
 * Darwin / Mac OS X patch by Yuji Yamano <yyamano AT kt.rim.or.jp>
 * wide output format fix for NetBSD by Jeff Brown <jabrown AT caida.org>
 * (Net|Open|Free)BSD & Darwin merged by Ralf Meyer <ralf AT thp.Uni-Duisburg.DE>
 */
#  define HAS_PGID
#  define PSCMD 	"ps -axwwo user,pid,ppid,pgid,command"
#  define PSFORMAT 	"%s %ld %ld %ld %[^\n]"
#  define PSVARS	P[i].name, &P[i].pid, &P[i].ppid, &P[i].pgid, P[i].cmd
#  define PSVARSN	5
#  define ZOMBIES_HAVE_PID_0
/************************************************************************/
#elif defined(sun) && (!defined(__SVR4)) /* Solaris 1.x */
/* contributed by L. Mark Larsen <mlarsen AT ptdcs2.intel.com> */
/* new cpp criteria by Pierre Belanger <belanger AT risq.qc.ca> */
#  define solaris1x
#  define UID2USER
#  ifdef mc68000
/* contributed by Paul Kern <pkern AT utcc.utoronto.ca> */
#    define PSCMD 	"ps laxw"
#    define PSFORMAT 	"%*7c%ld %ld %ld %*d %*d %*d %*x %*d %*d %*x %*14c %[^\n]"
#    define uid_t	int
#    define NEED_STRSTR
#  else
#    define PSCMD 	"ps jaxw"
#    define PSFORMAT 	"%ld %ld %*d %*d %*s %*d %*s %ld %*s %[^\n]"
#    define PSVARS 	&P[i].ppid, &P[i].pid, &P[i].uid, P[i].cmd
#    define PSVARSN	4
#  endif
/************************************************************************/
#elif defined(sun) && (defined(__SVR4)) /* Solaris 2.x */
/* contributed by Pierre Belanger <belanger AT risq.qc.ca> */
#  define solaris2x
#  define PSCMD         "ps -ef"
#  define PSFORMAT      "%s %ld %ld %*d %*s %*s %*s %[^\n]"
/************************************************************************/
#elif defined(bsdi)
/* contributed by Dean Gaudet <dgaudet AT hotwired.com> */
#  define UID2USER
#  define PSCMD 	"ps laxw"
#  define PSFORMAT 	"%ld %ld %ld %*d %*d %*d %*d %*d %*s %*s %*s %*s %[^\n]"
/************************************************************************/
#elif defined(_BSD)	/* Untested */
#  define UID2USER
#  define PSCMD 	"ps laxw"
#  define PSFORMAT 	"%*d %*c %ld %ld %ld %*d %*d %*d %*x %*d %d %*15c %*s %[^\n]"
/************************************************************************/
#elif defined(__convex)	/* ConvexOS */
#  define UID2USER
#  define PSCMD 	"ps laxw"
#  define PSFORMAT 	"%*s %ld %ld %ld %*d %*g %*d %*d %*21c %*s %[^\n]"
/************************************************************************/
#else			/* HP-UX, A/UX etc. */
#  define PSCMD 	"ps -ef"
#  define PSFORMAT 	"%s %ld %ld %*20c %*s %[^\n]"
#endif
/*********************** end of configurable part ***********************/

#ifndef PSVARS 		/* Set default */
# ifdef UID2USER
#  define PSVARS	&P[i].uid, &P[i].pid, &P[i].ppid, P[i].cmd
# else
#  define PSVARS	P[i].name, &P[i].pid, &P[i].ppid, P[i].cmd
# endif
# define PSVARSN	4
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>		/* For str...() */
#include <signal.h>
#ifdef NEED_SNPRINTF
#include <stdarg.h>
int snprintf(char *, int, char *, ...);
#endif
#include <unistd.h>		/* For getopt() */
#include <pwd.h>		/* For getpwnam() */

#include <sys/ioctl.h>		/* For TIOCGSIZE/TIOCGWINSZ */
//#include <termios.h>

#ifdef DEBUG
# include <errno.h>
#endif

#ifdef NEED_STRSTR
static char *strstr(char *, char *);
#endif

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

int MyPid, NProc, Columns, RootPid;
short showall = TRUE, soption = FALSE, Uoption = FALSE;
char *name = "", *str = NULL, *Progname;
long ipid = -1;
char *input = NULL;

int atLdepth=0;    /* LOPTION - track how deep in the print chain we are */
int maxLdepth=100; /* LOPTION - will be changed by -l n option */

#ifdef DEBUG
int debug = FALSE;
#endif

struct Proc {
  long uid, pid, ppid, pgid;
  char name[32], cmd[MAXLINE];
  int  print;
  long parent, child, sister;
  unsigned long thcount;
} *P;

#ifdef UID2USER
void uid2user(uid_t uid, char *name, int len) {
#define NUMUN 128
  static struct un_ {
    uid_t uid;
    char name[32];
  } un[NUMUN];
  static short n = 0;
  short i;
  char uid_name[32];
  char *found;
#ifdef DEBUG
  if (name == NULL) {
    for (i = 0; i < n; i++)
      fprintf(stderr, "uid = %3d, name = %s\n", un[i].uid, un[i].name);
    return;
  }
#endif
  for (i = n - 1; i >= 0 && un[i].uid != uid; i--);
  if (i >= 0) { /* found locally */
    found = un[i].name;
  } else {
    struct passwd *pw = getpwuid(uid);
    if (pw) {
      found = pw->pw_name;
    } else {
      /* fix by Stan Sieler & Philippe Torche */
      snprintf(uid_name, sizeof(uid_name), "#%d", uid);
      found = uid_name;
    }
    if (n < NUMUN) {
      un[n].uid = uid;
      strncpy(un[n].name, found, 9);
      un[n].name[8] = '\0';
      n++;
    }
  }
  strncpy(name, found, len);
  name[len-1] = '\0';
}
#endif

#if defined(_AIX) || defined(___AIX)	/* AIX 3.x / 4.x */
int GetProcessesDirect(void) {
  int i, nproc, maxnproc = 1024;
  
  struct ProcInfo *proc;
  int idx;
#ifndef USE_GETPROCS
  struct userinfo user;
#endif
  
  do {
    proc = malloc(maxnproc * sizeof(struct ProcInfo));
    if (proc == NULL) {
      fprintf(stderr, "Problems with malloc.\n");
      exit(1);
    }
    
    /* Get process table */
    idx = 0;
    nproc = IFNEW(getprocs(proc, sizeof(struct procsinfo), NULL, 0,
			   &idx, maxnproc),
		  getproc(proc, maxnproc, sizeof(struct procinfo))
		  );
#ifdef DEBUG
    idx = errno; /* Don't ask... */
    if (debug)
      fprintf(stderr,
	      "nproc = %d maxnproc = %d" IFNEW(" idx = %d ","") "\n",
	      nproc, maxnproc, idx);
    errno = idx;
#endif
#ifdef USE_GETPROCS
    if (nproc == -1) {
      perror("getprocs");
      exit(1);
    } else if (nproc == maxnproc) {
      nproc = -1;
    }
#endif
    if (nproc == -1) {
      free(proc);
      maxnproc *= 2;
    } 
  } while (nproc == -1);
  
  P = malloc((nproc+1) * sizeof(struct Proc));
  if (P == NULL) {
    fprintf(stderr, "Problems with malloc.\n");
    exit(1);
  }
  
  for (i = 0; i < nproc; i++) {

#ifndef USE_GETPROCS
    getuser(&proc[i],sizeof(struct procinfo),
	    &user,   sizeof(struct userinfo));
#endif
    P[i].uid     = proc[i].pi_uid;
    P[i].pid     = proc[i].pi_pid;
    P[i].ppid    = proc[i].pi_ppid;
    P[i].pgid    = proc[i].pi_pgrp;
    P[i].thcount = IFNEW(proc[i].pi_thcount, 1);
    
    uid2user(P[i].uid, P[i].name, sizeof(P[i].name));
    
    if (IFNEW(proc[i].pi_state,proc[i].pi_stat) == SZOMB) {
      strcpy(P[i].cmd, "<defunct>");
    } else {
      char *c = P[i].cmd;
      int ci = 0;
      getargs(&proc[i], sizeof(struct procinfo), c, MAXLINE - 2);
      c[MAXLINE-2] = c[MAXLINE-1] = '\0';

      /* Collect args. Stop when we encounter two '\0' */
      while (c[ci] != '\0' && (ci += strlen(&c[ci])) < MAXLINE - 2)
	c[ci++] = ' ';
      
      /* Drop trailing blanks */
      ci = strlen(c);
      while (ci > 0 && c[ci-1] == ' ') ci--;
      c[ci] = '\0';
      
      /* Replace some unprintables with '?' */
      for (ci = 0; c[ci] != '\0'; ci++)
	if (c[ci] == '\n' || c[ci] == '\t') c[ci] = '?';
      
      /* Insert [ui_comm] when getargs returns nothing */
      if (c[0] == '\0') {
	int l = strlen(IFNEW(proc[i].pi_comm,user.ui_comm));
	c[0] = '[';
	strcpy(c+1, IFNEW(proc[i].pi_comm,user.ui_comm));
	c[l+1] = ']';
	c[l+2] = '\0';
      }
    }
#ifdef DEBUG
    if (debug)
      fprintf(stderr,
	      "%d: uid=%5ld, name=%8s, pid=%5ld, ppid=%5ld, pgid=%5ld, tsize=%7u, dvm=%4u, "
	      "thcount=%2d, cmd[%d]='%s'\n",
	      i, P[i].uid, P[i].name, P[i].pid, P[i].ppid, P[i].pgid,
	      IFNEW(proc[i].pi_tsize,user.ui_tsize),
	      IFNEW(proc[i].pi_dvm,user.ui_dvm),
	      proc[i].pi_thcount,
	      strlen(P[i].cmd),P[i].cmd);
#endif
    P[i].parent = P[i].child = P[i].sister = -1;
    P[i].print = FALSE;
  }
  free(proc);
  return nproc;
}

#endif /* _AIX */

#ifdef __linux
int GetProcessesDirect(void) {
  glob_t globbuf;
  unsigned int i, j;
  
  glob("/proc/[0-9]*", GLOB_NOSORT, NULL, &globbuf);
  
  P = calloc(globbuf.gl_pathc, sizeof(struct Proc));
  if (P == NULL) {
    fprintf(stderr, "Problems with malloc.\n");
    exit(1);
  }
  
  for (i = j = 0; i < globbuf.gl_pathc; i++) {
    char name[32];
    int c;
    FILE *tn;
    struct stat stat;
    int k = 0;
    
    snprintf(name, sizeof(name), "%s%s",
	     globbuf.gl_pathv[globbuf.gl_pathc - i - 1], "/stat");
    tn = fopen(name, "r");
    if (tn == NULL) continue; /* process vanished since glob() */
    fscanf(tn, "%ld %s %*c %ld %ld",
	   &P[j].pid, P[j].cmd, &P[j].ppid, &P[j].pgid);
    fstat(fileno(tn), &stat);
    P[j].uid = stat.st_uid;
    fclose(tn);
    P[j].thcount = 1;
    
    snprintf(name, sizeof(name), "%s%s",
	     globbuf.gl_pathv[globbuf.gl_pathc - i - 1], "/cmdline");
    tn = fopen(name, "r");
    if (tn == NULL) continue;
    while (k < MAXLINE - 1 && EOF != (c = fgetc(tn))) {
      P[j].cmd[k++] = c == '\0' ? ' ' : c;
    }
    if (k > 0) P[j].cmd[k] = '\0';
    fclose(tn);
    
    uid2user(P[j].uid, P[j].name, sizeof(P[j].name));
    
#ifdef DEBUG
    if (debug) fprintf(stderr,
		       "uid=%5ld, name=%8s, pid=%5ld, ppid=%5ld, pgid=%5ld, thcount=%ld, cmd='%s'\n",
		       P[j].uid, P[j].name, P[j].pid, P[j].ppid, P[j].pgid, P[j].thcount, P[j].cmd);
#endif
    P[j].parent = P[j].child = P[j].sister = -1;
    P[j].print  = FALSE;
    j++;
  }
  globfree(&globbuf);
  return j;
}
#endif /* __linux */

int GetProcesses(void) {
  FILE *tn;
  int i = 0;
  char line[MAXLINE], command[] = PSCMD;
  
  /* file read code contributed by Paul Kern <pkern AT utcc.utoronto.ca> */
  if (input != NULL) {
    if (strcmp(input, "-") == 0)
      tn = stdin;
    else if (NULL == (tn = fopen(input,"r"))) {
      perror(input);
      exit(1);
    }
  } else {
#ifdef DEBUG
    if (debug) fprintf(stderr, "calling '%s'\n", command);
#endif
    if (NULL == (tn = (FILE*)popen(command,"r"))) {
      perror("Problems with pipe");
      exit(1);
    }
  }
#ifdef DEBUG
  if (debug) fprintf(stderr, "popen:errno = %d\n", errno);
#endif
  
  if (NULL == fgets(line, MAXLINE, tn)) { /* Throw away header line */
    fprintf(stderr, "No input.\n");
    exit(1);
  }
  
#ifdef DEBUG
  if (debug) fputs(line, stderr);
#endif
  
  P = malloc(sizeof(struct Proc));
  if (P == NULL) {
    fprintf(stderr, "Problems with malloc.\n");
    exit(1);
  }
  
  while (NULL != fgets(line, MAXLINE, tn)) {
    int len, num;
    len = strlen(line);
#ifdef DEBUG
    if (debug) {
      fprintf(stderr, "len=%3d ", len);
      fputs(line, stderr);
    }
#endif
    
    if (len == MAXLINE - 1) { /* line too long, drop remaining stuff */
      char tmp[MAXLINE];
      while (MAXLINE - 1 == strlen(fgets(tmp, MAXLINE, tn)));
    }      
    
    P = realloc(P, (i+1) * sizeof(struct Proc));
    if (P == NULL) {
      fprintf(stderr, "Problems with realloc.\n");
      exit(1);
    }
    
    memset(&P[i], 0, sizeof(*P));
    
#ifdef solaris1x
    { /* SunOS allows columns to run together.  With the -j option, the CPU
       * time used can run into the numeric user id, so make sure there is
       * space between these two columns.  Also, the order of the desired
       * items is different. (L. Mark Larsen <mlarsen AT ptdcs2.intel.com>)
       */
      char buf1[45], buf2[MAXLINE];
      buf1[44] = '\0';
      sscanf(line, "%44c%[^\n]", buf1, buf2);
      snprintf(line, sizeof(line), "%s %s", buf1, buf2);
    }
#endif
    
    num = sscanf(line, PSFORMAT, PSVARS);
    
    if (num != PSVARSN) {
#ifdef DEBUG
      if (debug) fprintf(stderr, "dropped line, num=%d != %d\n", num, PSVARSN);
#endif
      continue;
    }
    
#ifdef UID2USER	/* get username */
    uid2user(P[i].uid, P[i].name, sizeof(P[i].name));
#endif

#ifdef DEBUG
    if (debug) fprintf(stderr,
		      "uid=%5ld, name=%8s, pid=%5ld, ppid=%5ld, pgid=%5ld, thcount=%ld, cmd='%s'\n",
		      P[i].uid, P[i].name, P[i].pid, P[i].ppid, P[i].pgid, P[i].thcount, P[i].cmd);
#endif
    P[i].parent = P[i].child = P[i].sister = -1;
    P[i].print  = FALSE;
    i++;
  }
  if (input != NULL)
    fclose(tn);
  else
    pclose(tn);
  return i;
}

#ifdef ZOMBIES_HAVE_PID_0
void FixZombies(void) {
  int me, num = 0;
  for (me = 0; me < NProc; me++) {
    if (P[me].pid == 0) num++;
  }
  if (num > 1) for (me = 0; me < NProc; me++) {
    if (P[me].pid == 0 && P[me].ppid != 0 && P[me].ppid != -1) {
      P[me].pid = -1;
#ifdef DEBUG
      if (debug) fprintf(stderr,
			 "fixed zombie %s with ppid %d\n",
			 P[me].cmd, P[me].ppid);
#endif
    }
  }
}
#endif

int get_pid_index(long pid) {
  int me;
  for (me = NProc - 1;me >= 0 && P[me].pid != pid; me--); /* Search process */
  return me;
}

#define EXIST(idx) ((idx) != -1)

void MakeTree(void) {
  /* Build the process hierarchy. Every process marks itself as first child
   * of it's parent or as sister of first child of it's parent */
  int me;  
  for (me = 0; me < NProc; me++) {
    int parent;
    parent = get_pid_index(P[me].ppid);
    if (parent != me && parent != -1) { /* valid process, not me */
      P[me].parent = parent;
      if (P[parent].child == -1) /* first child */
	P[parent].child = me;
      else {
	int sister;
	for (sister = P[parent].child; EXIST(P[sister].sister); sister = P[sister].sister);
	P[sister].sister = me;
      }
    }
  }
}

void MarkChildren(int me) {
  int child;
  P[me].print = TRUE;
  for (child = P[me].child; EXIST(child); child = P[child].sister)
    MarkChildren(child);
}

void MarkProcs(void) {
  int me;
  for (me = 0; me < NProc; me++) {
    if (showall) {
      P[me].print = TRUE;
    } else {
      int parent;
      if (0 == strcmp(P[me].name, name)		/* for -u */
	 || (Uoption &&
	     0 != strcmp(P[me].name, "root"))	/* for -U */
	 || P[me].pid == ipid			/* for -p */
	 || (soption
	     && NULL != strstr(P[me].cmd, str)
	     && P[me].pid != MyPid)		/* for -s */
	 ) {
	/* Mark parents */
	for (parent = P[me].parent; EXIST(parent); parent = P[parent].parent) {
	  P[parent].print = TRUE;
	}
	/* Mark children */
	MarkChildren(me);
      }
    }
#if 0 /* experimental thread compression */
    {
      int parent = P[me].parent;
      int ancestor; /* oldest parent with same cmd */
      if (0 == strcmp(P[me].cmd, P[parent].cmd)) {
	P[me].print = FALSE;
	for (parent = P[me].parent;
	     EXIST(parent) && (0 == strcmp(P[me].cmd, P[parent].cmd));
	     parent = P[parent].parent) {
	  ancestor = parent;
	}
	fprintf(stderr, "%d: %d\n",
		P[me].pid,
		P[ancestor].pid);
	P[ancestor].thcount++;
      }
    }
#endif
  }
}

void DropProcs(void) {
  int me;
  for (me = 0; me < NProc; me++) if (P[me].print) {
    int child, sister;
    /* Drop children that won't print */
    for (child = P[me].child;
	 EXIST(child) && !P[child].print; child = P[child].sister);
    P[me].child = child;
    /* Drop sisters that won't print */
    for (sister = P[me].sister;
	 EXIST(sister) && !P[sister].print; sister = P[sister].sister);
    P[me].sister = sister;
  }
}

int KillTree(int rank,int idx)
{
  int child,cnt,retval;
//int sig[4]={SIGINT,SIGHUP,SIGTERM,SIGKILL};
      int sig[2]={SIGTERM,SIGKILL};

  if (!(EXIST(idx))) return 0;

  if (verbose_mode)
    {
      printf("<W:%i> Killing \"%s\" with PID=%i\n",rank,P[idx].cmd,(int) P[idx].pid);
      fflush(stdout);
    }

  cnt=0;
  while (cnt<2)
    {
      retval=kill(P[idx].pid,sig[cnt++]);
      if (retval) break;
      sleep(SLEEPTIME);
    }

  for (child = P[idx].child; EXIST(child); child = P[child].sister)
    {
      KillTree(rank,child);
    }
  return 0;
}

int CompareNames(char *cmd1, char *cmd2)
{
  char one[MAXLINE],two[MAXLINE];
  int  cnt;

  sprintf(one,"%s",cmd1);
  sprintf(two,"%s",cmd2);

  /* Remove spaces at the end of the strings */

  cnt=strlen(one)-1;
  while (cnt)
    {
      if (!(one[cnt]==' ')) break;
      one[cnt]='\0';
      cnt--;
    }
  cnt=strlen(two)-1;
  while (cnt)
    {
      if (!(two[cnt]==' ')) break;
      two[cnt]='\0';
      cnt--;
    }

  if (strlen(two)<strlen(one)) return 1;
  if (strcmp(one,&two[strlen(two)-strlen(one)])) return 1;

  return 0;
}

int GetIndexFromName(char *str)
{
  int me;

  for (me = NProc - 1;me >= 0 && (CompareNames(str,(char *) P[me].cmd)==1); me--); /* Search process */
  return me;
}

void KillProcessAndDaughters(int rank, char *cmd)
{
  int index;

#ifdef USE_GetProcessesDirect
  NProc = input == NULL ? GetProcessesDirect() : GetProcesses();
#else
  NProc = GetProcesses();
#endif
  MakeTree();
  MarkProcs();
  DropProcs();

  index=GetIndexFromName(cmd);
  KillTree(rank,index);

  free(P);
  return;
}

#ifdef NEED_STRSTR
/* Contributed by Paul Kern <pkern AT utcc.utoronto.ca> */
static char * strstr(s1, s2)
     register char *s1, *s2;
{
  register int n1, n2;
  
  if (n2 = strlen(s2))
    for (n1 = strlen(s1); n1 >= n2; s1++, n1--)
      if (strncmp(s1, s2, n2) == 0)
	return s1;
  return NULL;
}
#endif /* NEED_STRSTR */

#ifdef NEED_SNPRINTF
/* Contributed by Michael E White */
int snprintf(char *name, int namesiz, char *format, ...)
{
  int retval;
  char bigbuf[1024] = {'\0'};  
  va_list ap;
  va_start(ap, format);
  retval = vsprintf(bigbuf,format,ap);
  va_end(ap);
  if (retval > namesiz) retval = namesiz;
  strncpy(name, bigbuf, retval);
  name[retval] = '\0';
  return retval;
}
#endif  /* NEED_SNPRINTF */
