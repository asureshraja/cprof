#include "cprof.h"

int meow(int a,int b);

int meow(int a,int b){
        return 2;
}

void foo(){
        _show_line_info_
        meow(3,4);
}
// void start_test_cases(int no_of_test_cases){
//     for (int __ = 0; __ < n; ++__){switch (__){
//
//         case 0:{}break;
//
//         default:printf("%s\n","No test case executed. Wrong test case number." );break;}}}
int add(int a,int b){
        return a+b;
}
int tee=4;
void name() {
        unwind(1,(char *)&tee,sizeof(tee));
        printf("%d\n",tee );
}

int main(int argc, char const *argv[]) {
        _init_cprof_
        char *aa;
        // printf("%s\n",aa);
        int a,b,c;
        tee=5;
        name();
        _show_backtrace_
        // _start_testing_(2)
        //
        //         _start_cases_
        //                 _start_case_(0)
        //                         a=1;b=1;
        //                 _stop_case_
        //                 _start_case_(1)
        //                         a=-1;b=1;
        //                 _stop_case_
        //         _stop_cases_
        // ///block we are testing
        //
        //         c=add(a,b);
        // /////end of block
        //         _start_verification_
        //                 _start_case_verify_(0)
        //                         if (c==2) {
        //                                 printf("test case %d passed \n",__ );
        //                         }
        //                 _stop_case_verify
        //                 _start_case_verify_(1)
        //                         if (c==0) {
        //                                 printf("test case %d passed \n",__ );
        //                         }
        //                 _stop_case_verify
        //         _stop_verification_
        //
        // _stop_testing_
        //
        // show_backtrace();
        // _show_line_info_
        // _show_res_usage_info_
        //
        // foo();
        //
        // _print_virtmem_usage_
        // _print_mem_usage_
        //
        // _start_trace_
        // printf("%d\n", meow(1,2));
        // while (1) {
        //         printf("%d\n", meow(1,2));
        //         sleep(1);
        //         break;
        // }
        // meow(1,2);
        // meow(1,2);
        // _stop_trace_
        //

        printf("%s\n","exiting" );

        return 0;
}
