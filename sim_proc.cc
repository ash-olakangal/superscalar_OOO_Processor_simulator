#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sim_proc.h"
#include "proc.cc"    
#include <iostream>

/* argc holds the number of command line arguments
    argv[] holds the commands themselves
*/
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    proc_params params;     // look at sim_bp.h header file for the definition of struct proc_params
    
    if (argc != 5)
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.rob_size     = strtoul(argv[1], NULL, 10);
    params.iq_size      = strtoul(argv[2], NULL, 10);
    params.width        = strtoul(argv[3], NULL, 10);
    trace_file          = argv[4];
    
    

    // Open trace_file
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    
    // Creating processor instance
    Processor proc_instance;
    proc_instance.config_proc(params.rob_size, params.iq_size, params.width);

    do{
        proc_instance.retire();
        proc_instance.write_back();
        proc_instance.execute();
        proc_instance.issue();
        proc_instance.dispatch();
        proc_instance.register_read();
        proc_instance.rename();
        proc_instance.decode();
        proc_instance.fetch(FP);
        //proc_instance.print_instruction_vector();
        //proc_instance.printRMTVector();
    }
    while(proc_instance.advance_cycle());

    // printing simulation command
    std::cout << "# === Simulator Command =========" << std::endl;
    std::cout << "#";
    for(int i=0; i<argc; i++){
        std::cout << " " << argv[i];
    }
    std::cout << std::endl;

    std::cout << "# === Processor Configuration ===" << std::endl;
    std::cout << "# ROB_SIZE = " << params.rob_size << std::endl;
    std::cout << "# IQ_SIZE  = " << params.iq_size << std::endl;
    std::cout << "# WIDTH    = " << params.width << std::endl;
    // output final statistics
    proc_instance.output_statistics();

    fclose(FP);
    return 0;
}
