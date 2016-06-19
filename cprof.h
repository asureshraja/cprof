#define _GNU_SOURCE
#include <sys/resource.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <string.h>
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/times.h>
#include <sys/vtimes.h>
#include <execinfo.h>
#include <mcheck.h>
#define READ 0
#define WRITE 1
#define _init_cprof_   signal(SIGSEGV, signal_callback_handler);
#define _show_line_info_ printf("Thread:%ld File:%s Function:%s Line:%d\n",pthread_self(),__FILE__,__FUNCTION__,__LINE__ );
#define _show_res_usage_info_ get_res_usage();
#define _print_virtmem_usage_ printf("Virtual Memory usage:%dkb\n",getVirtualMemoryUsage());
#define _print_mem_usage_ printf("Main Memory usage:%dkb\n",getRAMusage());
#define _start_trace_ char buf[1024]; sprintf (buf, "sudo ltrace -x -tt -b -T -n 5 -p %d",getpid()); int r=0; int w=1; pid_t p = popen2(buf, &r, &w); sleep(1);
#define _stop_trace_ pclose2(p);
#define _start_time_trace_ char buf[1024]; sprintf (buf, "sudo ltrace -x -tt -b -c -p %d",getpid()); int r=0; int w=1; pid_t p = popen2(buf, &r, &w); sleep(1);
#define _stop_time_trace_ pclose2(p);
#define _start_detail_trace_ char buf[1024]; sprintf (buf, "sudo ltrace -S -x -tt -n 5 -b -p %d",getpid()); int r=0; int w=1; pid_t p = popen2(buf, &r, &w); sleep(1);
#define _stop_detail_trace_ pclose2(p);
#define _start_testing_(x)  int __;for (__ = 0; __ < x; ++__){
#define _start_cases_ switch (__){
#define _start_case_(x) case x:{printf("\nExecuting test case %d block\n",__);
#define _stop_case_ printf("End of test case %d Execution\n",__ );}break;
#define _stop_cases_ default:printf("%s\n","No test case executed. Wrong test case number." );break;}
#define _start_verification_ switch (__){printf("\nExecuting test case %d Verification block\n",__);
#define _start_case_verify_(x) case x:{printf("starting test case %d verification\n",__ );
#define _stop_case_verify printf("End of test case %d verification\n",__ );}break;
#define _stop_verification_ default:printf("%s\n","Verification Failed." );break;}
#define _stop_testing_ }
#define _show_backtrace_ show_backtrace();
pid_t
popen2(const char *command, int *infp, int *outfp)
{
        int p_stdin[2], p_stdout[2];
        pid_t pid;

        if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
                return -1;

        pid = fork();

        if (pid < 0)
                return pid;
        else if (pid == 0)
        {
                dup2(p_stdin[READ], STDIN_FILENO);
                dup2(p_stdout[WRITE], STDOUT_FILENO);

                //close unuse descriptors on child process.
                close(p_stdin[READ]);
                close(p_stdin[WRITE]);
                close(p_stdout[READ]);
                close(p_stdout[WRITE]);

                //can change to any exec* function family.
                execl("/bin/bash", "bash", "-c", command, NULL);
                perror("execl");
                exit(1);
        }

        // close unused descriptors on parent process.
        close(p_stdin[READ]);
        close(p_stdout[WRITE]);

        if (infp == NULL)
                close(p_stdin[WRITE]);
        else
                *infp = p_stdin[WRITE];

        if (outfp == NULL)
                close(p_stdout[READ]);
        else
                *outfp = p_stdout[READ];

        return pid;
}

int
pclose2(pid_t pid) {
        char buf[256];
        sprintf(buf,"kill %ld", pid);
        system(buf);
        // kill(pid,9);
        return 1;
}
int parseLine(char* line){
        int i = strlen(line);
        while (*line < '0' || *line > '9') line++;
        line[i-3] = '\0';
        i = atoi(line);
        return i;
}

int getRAMusage(){  //Note: this value is in KB!
        FILE* file = fopen("/proc/self/status", "r");
        int result = -1;
        char line[128];


        while (fgets(line, 128, file) != NULL) {
                if (strncmp(line, "VmRSS:", 6) == 0) {
                        result = parseLine(line);
                        break;
                }
        }
        fclose(file);
        return result;
}

int getVirtualMemoryUsage(){  //Note: this value is in KB!
        FILE* file = fopen("/proc/self/status", "r");
        int result = -1;
        char line[128];


        while (fgets(line, 128, file) != NULL) {
                if (strncmp(line, "VmSize:", 7) == 0) {
                        result = parseLine(line);
                        break;
                }
        }
        fclose(file);
        return result;
}
void get_res_usage(){
        struct rusage *res;
        res=malloc(sizeof(struct rusage));
        getrusage(RUSAGE_THREAD,res);
        printf("\nUser Mode Time Spent:%ld\nKernel Mode Time Spent:%ld\nMaximum Resident Set size:%ldkb\nNo of Page Faults Serviced Without I/O:%ld\nNo of Page Faults Serviced that required I/O:%ld\nNo of FileSystem Input Operation:%ld\nNo of FileSystem Output OP:%ld\nNo of Voluntary Context Switches:%ld\nNo of Involuntary Context Switches:%ld\n",res->ru_stime.tv_sec,res->ru_utime.tv_sec,res->ru_maxrss,res->ru_minflt,res->ru_majflt, res->ru_inblock,res->ru_oublock,res->ru_nvcsw,res->ru_nivcsw);
        free(res);
}

int getFileAndLine (unw_word_t addr, char *file, size_t flen, int *line)
{
	char buf[1024];
	char *p;

	// prepare command to be executed
	// our program need to be passed after the -e parameter
	sprintf (buf, "/usr/bin/addr2line -C -e ./a.out -f -i %lx", addr);
	FILE* f = popen (buf, "r");

	if (f == NULL)
	{
		perror (buf);
		return 0;
	}

	// get function name
	fgets (buf, 1024, f);

	// get file and line
	fgets (buf, 1024, f);

	if (buf[0] != '?')
	{
		int l;
		char *p = buf;

		// file name is until ':'
		while (*p != ':')
		{
			p++;
		}

		*p++ = 0;
		// after file name follows line number
		strcpy (file , buf);
		sscanf (p,"%d", line);
	}
	else
	{
		strcpy (file,"unkown");
		*line = 0;
	}

	pclose(f);
}

void show_backtrace ()
{
	char name[256];
	unw_cursor_t cursor; unw_context_t uc;
	unw_word_t ip, sp, offp;

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);

	while (unw_step(&cursor) > 0)
	{
		char file[256];
		int line = 0;
		name[0] = '\0';
		unw_get_proc_name(&cursor, name, 256, &offp);
		unw_get_reg(&cursor, UNW_REG_IP, &ip);
		unw_get_reg(&cursor, UNW_REG_SP, &sp);
		//printf ("%s ip = %lx, sp = %lx\n", name, (long) ip, (long) sp);
		getFileAndLine((long)ip, file, 256, &line);
		printf("%s in file %s line %d\n", name, file, line-1);
	}
}
void sigsegv_show_backtrace ()
{
	char name[256];
	unw_cursor_t cursor; unw_context_t uc;
	unw_word_t ip, sp, offp;

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);

	while (unw_step(&cursor) > 0)
	{
		char file[256];
		int line = 0;
		name[0] = '\0';
		unw_get_proc_name(&cursor, name, 256, &offp);
		unw_get_reg(&cursor, UNW_REG_IP, &ip);
		unw_get_reg(&cursor, UNW_REG_SP, &sp);
		//printf ("%s ip = %lx, sp = %lx\n", name, (long) ip, (long) sp);
		getFileAndLine((long)ip, file, 256, &line);
		printf("%s in file %s line %d\n", name, file, line-2);
	}
}

void
unwind_n(size_t depth, size_t n,...)
{
    int i, j;
    va_list ap;
    struct a { struct a *next; } *ebp;
    struct {
        size_t size;
        char *addr;
        ptrdiff_t offset;
    } vars[n];

#if __x86_64__
    asm volatile("movq      %%rbp, %0;" : "=r" (ebp));
#else
    asm volatile("movl      %%ebp, %0;" : "=r" (ebp));
#endif

    ebp = ebp->next;

    va_start(ap, n);
    for (i = 0; i < n; i++) {
        vars[i].offset = (vars[i].addr = va_arg(ap, char *))
            	- (char *)ebp;
        vars[i].size = va_arg(ap, size_t);
    }
    va_end(ap);

    for (i = 0; i < depth; i++)
        ebp = ebp->next;

    for (i = 0; i < n; i++)
        for (j = 0; j < vars[i].size; j++)
            vars[i].addr[j] = (((char *)ebp + vars[i].offset))[j];
}

void
unwind(size_t depth, char *addr, size_t len)
{
    size_t i;
    struct a { struct a *next; } *ebp;
    ptrdiff_t offset;

#if __x86_64__
    asm volatile("movq      %%rbp, %0;" : "=r" (ebp));
#else
    asm volatile("movl      %%ebp, %0;" : "=r" (ebp));
#endif

    /* Enter the first stack frame and get the offset of the variable. */
    ebp = ebp->next;
    offset = addr - (char *)ebp;

    for (i = 0; i < depth; i++)
        ebp = ebp->next;

    for (i = 0; i < len; i++){
        addr[i] = (((char *)ebp + offset))[i];
        printf("%d\n",*addr );

    }
}
void signal_callback_handler(int signum)
{
   sigsegv_show_backtrace();
   exit(signum);
}
