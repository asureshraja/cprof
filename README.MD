compile test program


gcc test.c -lpthread -lunwind -g -w


Macros


	_show_line_info_   = shows function name, module name and line number.
        _show_res_usage_info_ = shows resource usages by the current thread.
	_print_virtmem_usage_ = shows virtual memory usage by the process.
        _print_mem_usage_ = shows physical memory usage by the process.

	Shows function trace
		_start_trace_
			//block to show function trace
			//end of block
		_stop_trace_

	Shows time trace
		_start_time_trace_
			//block to show time trace
			//end of block
		_stop_time_trace_

	Shows detailed function trace
		_start_detail_trace_
			//block to show time trace
			//end of block
		_stop_detail_trace_


Test Macro Usage

   	 _start_testing_(2)
        
                 _start_cases_
                         _start_case_(0)
                                 a=1;b=1;
                         _stop_case_
                         _start_case_(1)
                                 a=-1;b=1;
                         _stop_case_
                 _stop_cases_
         ///block we are testing
        
                 c=add(a,b);
         /////end of block
                 _start_verification_
                         _start_case_verify_(0)
                                 if (c==2) {
                                         printf("test case %d passed \n",__ );
                                 }
                         _stop_case_verify
                         _start_case_verify_(1)
                                 if (c==0) {
                                         printf("test case %d passed \n",__ );
                                 }
                         _stop_case_verify
                 _stop_verification_
        
         _stop_testing_
         
         
         
