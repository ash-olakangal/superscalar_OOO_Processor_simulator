#ifndef PROC_H
#define PROC_H

#include <iostream>
#include <vector>
#include <deque>
#include <cstdint>
#include <algorithm>
#include <iomanip>

class Processor{

    private:
        unsigned long int rob_size;
        unsigned long int iq_size;
        unsigned long int width;

        unsigned long int cycle_count = 0;

        bool trace_depleted = false;

        unsigned int current_rob_tail = 0; 

        unsigned long int total_instr_read = 0;

        struct Instruction {
            uint64_t instr_pc;
            int op_type, dest, src1, src2;
            bool src1_rdy, src2_rdy;
            uint64_t age;
            uint64_t exec_time;
            bool write_back;
            bool retired;
            int og_dest, og_src1, og_src2;
            
            unsigned int rob_tag; 

            unsigned long int begin_cycle = 0;
            int FE_time=0;
            int DE_time=0;
            int RN_time=0;
            int RR_time=0;
            int DI_time=0;
            int IS_time=0;
            int EX_time=0;
            int WB_time=0;
            int RT_time=0;
        };

        struct ROB_entry {
            uint64_t pc;
            bool ready;
            int dest;
            unsigned int tag;
        };

        struct RMT_entry {
            bool valid;
            unsigned int rob_tag;
        };

        std::deque<Instruction> DE_reg;
        std::deque<Instruction> RN_reg;
        std::deque<Instruction> RR_reg;
        std::deque<Instruction> DI_reg;
        std::deque<Instruction> IQ_reg;
        std::deque<Instruction> execute_list;
        std::deque<Instruction> WB_reg;
        std::deque<Instruction> RT_reg;
        std::deque<ROB_entry> ROB;
        std::vector<RMT_entry> RMT;

    public:
        // Helper functions
        int read_instruction_from_file(FILE *file_ptr, uint64_t *pc, int *op_type, int *dest, int *src1, int *src2);
        void print_instruction(const Instruction&);
        void print_instruction_vector();
        void printRMTVector();
        void print_timing(const Instruction&);
        void output_statistics();

        // Main functions
        void config_proc(unsigned long int, unsigned long int, unsigned long int);
        void retire();
        void write_back();
        void execute();
        void issue();
        void dispatch();
        void register_read();
        void rename();
        void decode();
        void fetch(FILE *FP);
        bool advance_cycle();
};

#endif
