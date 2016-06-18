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
#include <dlfcn.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/vtimes.h>
#include <execinfo.h>
#include <mcheck.h>
#define READ 0
#define WRITE 1
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
