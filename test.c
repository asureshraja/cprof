#include "cprof.h"
int meow(int a,int b);

int meow(int a,int b){
        return 2;
}

void foo(){
        _show_line_info_
        meow(3,4);
}
int main(int argc, char const *argv[]) {



        _show_line_info_
        _show_res_usage_info_

        foo();

        _print_virtmem_usage_
        _print_mem_usage_

        _start_trace_
        printf("%d\n", meow(1,2));
        while (1) {
                printf("%d\n", meow(1,2));
                sleep(1);
                break;
        }
        meow(1,2);
        meow(1,2);
        _stop_trace_


        printf("%s\n","exiting" );

        return 0;
}
