#include "proc.h"

// Helper function implementation
int Processor::read_instruction_from_file(FILE *file_ptr, uint64_t *pc, int *op_type, int *dest, int *src1, int *src2) {
    if (file_ptr == NULL) {
        return 0; 
    }
    int scan_result = fscanf(file_ptr, "%lx %d %d %d %d", pc, op_type, dest, src1, src2);
    return scan_result; 
}

void Processor::print_instruction(const Instruction& instr) {
    std::cout << "  [PC: 0x" << std::hex << instr.instr_pc << std::dec 
              << ", Op: " << instr.op_type 
              << ", Dest: " << instr.dest 
              << ", Src1: " << instr.src1 
              << ", Src2: " << instr.src2 
              << ", Src1.ready: " << instr.src1_rdy
              << ", Src2.ready: " << instr.src2_rdy
              << ", age: " << instr.age
              << ", execution time: " << instr.exec_time
              << ", write_back: " << instr.write_back << "]" << std::endl;
}

void Processor::print_instruction_vector() {
    std::cout << "Contents of DE vector" << std::endl;
    if (DE_reg.empty()) std::cout << "  (Empty)" << std::endl;
    for (const auto& instr : DE_reg) print_instruction(instr);
    std::cout << "---------------------------------------" << std::endl;

    std::cout << "Contents of RN vector" << std::endl;
    if (RN_reg.empty()) std::cout << "  (Empty)" << std::endl;
    for (const auto& instr : RN_reg) print_instruction(instr);
    std::cout << "---------------------------------------" << std::endl;

    std::cout << "Contents of RR vector" << std::endl;
    if (RR_reg.empty()) std::cout << "  (Empty)" << std::endl;
    for (const auto& instr : RR_reg) print_instruction(instr);
    std::cout << "---------------------------------------" << std::endl;

    std::cout << "Contents of DI vector" << std::endl;
    if (DI_reg.empty()) std::cout << "  (Empty)" << std::endl;
    for (const auto& instr : DI_reg) print_instruction(instr);
    std::cout << "---------------------------------------" << std::endl;

    std::cout << "Contents of IQ vector" << std::endl;
    if (IQ_reg.empty()) std::cout << "  (Empty)" << std::endl;
    for (const auto& instr : IQ_reg) print_instruction(instr);
    std::cout << "---------------------------------------" << std::endl;

    std::cout << "Contents of execute_list vector" << std::endl;
    if (execute_list.empty()) std::cout << "  (Empty)" << std::endl;
    for (const auto& instr : execute_list) print_instruction(instr);
    std::cout << "---------------------------------------" << std::endl;

    std::cout << "Contents of WB_reg vector" << std::endl;
    if (WB_reg.empty()) std::cout << "  (Empty)" << std::endl;
    for (const auto& instr : WB_reg) print_instruction(instr);
    std::cout << "---------------------------------------" << std::endl;

    std::cout << "Contents of RT_reg vector" << std::endl;
    if (RT_reg.empty()) std::cout << "  (Empty)" << std::endl;
    for (const auto& instr : RT_reg) print_instruction(instr);
    std::cout << "---------------------------------------" << std::endl;

    std::cout << "Contents of ROB" << std::endl;
    if (ROB.empty()) std::cout << "  (Empty)" << std::endl;
    for (const auto& instr : ROB) {
        std::cout << "  [PC: 0x" << std::hex << instr.pc << std::dec 
              << ", Ready: " << instr.ready 
              << ", Dest: " << instr.dest
              << ", Tag: " << instr.tag << "]" << std::endl;
    }
    std::cout << "---------------------------------------" << std::endl;
}

void Processor::printRMTVector() {
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "RMT Vector Contents:" << std::endl;
    if (RMT.empty()) {
        std::cout << "  (Vector is empty)" << std::endl;
        return;
    }
    for (size_t i = 0; i < RMT.size(); ++i) {
        std::cout << "  Entry " << i << ": "
                  << "valid = " << std::boolalpha << RMT[i].valid
                  << ", rob_tag = " << RMT[i].rob_tag << std::endl;
    }
    std::cout << "---------------------------------------" << std::endl;
}


void Processor::print_timing(const Instruction& instr){
    unsigned long int temp = instr.begin_cycle + instr.FE_time + instr.DE_time + instr.RN_time + instr.RR_time;
    std::cout << instr.age
              << " fu{" << instr.op_type << "}"
              << " src{" << instr.og_src1 << "," << instr.og_src2 << "}"
              << " dst{" << instr.og_dest << "}"
              << " FE{" << instr.begin_cycle << "," << instr.FE_time << "}"
              << " DE{" << instr.begin_cycle + instr.FE_time << "," << instr.DE_time << "}"
              << " RN{" << instr.begin_cycle + instr.FE_time + instr.DE_time << "," << instr.RN_time << "}"
              << " RR{" << instr.begin_cycle + instr.FE_time + instr.DE_time + instr.RN_time << "," << instr.RR_time << "}"
              << " DI{" << temp << "," << instr.DI_time << "}"
              << " IS{" << temp + instr.DI_time << "," << instr.IS_time << "}"
              << " EX{" << temp + instr.DI_time + instr.IS_time << "," << instr.EX_time << "}"
              << " WB{" << temp + instr.DI_time + instr.IS_time + instr.EX_time << "," << instr.WB_time << "}"
              << " RT{" << temp + instr.DI_time + instr.IS_time + instr.EX_time + instr.WB_time<< "," << instr.RT_time << "}"
              << std::endl;
}

void Processor::output_statistics() {
    std::cout << "# === Simulation Results ========" << std::endl;
    std::cout << "# Dynamic Instruction Count    = " << total_instr_read << std::endl;
    std::cout << "# Cycles                       = " << cycle_count << std::endl;
    std::cout << "# Instructions Per Cycle (IPC) = " << std::fixed << std::setprecision(2) << ((double)total_instr_read / cycle_count) << std::endl;
}

// Main functions
void Processor::config_proc(unsigned long int ip_rob_size, unsigned long int ip_iq_size, unsigned long int ip_width){

    // set processor configurations
    rob_size = ip_rob_size;
    iq_size = ip_iq_size;
    width = ip_width;

    RMT_entry init_entry;
    // Set all rmt registers to 0 initially
    init_entry.valid = 0;
    init_entry.rob_tag = 0;
    for(int i=0; i<67; i++){
        RMT.push_back(init_entry);
    }
}

void Processor::retire(){
    for(unsigned long int i=0; i< RT_reg.size(); i++){
        RT_reg[i].RT_time++;
    }

    for(unsigned long int i=0; i<width; i++){
        if(ROB.empty()) break; // do nothing if ROB is empty 

        ROB_entry current_rob_entry = ROB.front(); 

        if(current_rob_entry.ready == 1){
            
            // Search for the specific instruction in RT_reg using the Unique ROB Tag
            auto it = RT_reg.begin();
            while(it != RT_reg.end()){
                if(it->rob_tag == current_rob_entry.tag){
                    print_timing(*it);
                    
                    // Remove it from the list
                    RT_reg.erase(it);
                    break; 
                }
                it++;
            }

            // update RMT (Free the register mapping if it's still the latest)
            if(current_rob_entry.dest != -1 ){
                RMT_entry current_rmt_entry = RMT[current_rob_entry.dest];
                if(current_rmt_entry.valid && current_rob_entry.tag == current_rmt_entry.rob_tag){
                   RMT[current_rob_entry.dest].valid = 0;          
                }
            }

            // retire head of ROB
            ROB.pop_front();
        }
        else{
            break;
        }
    }
}

void Processor::write_back(){
    for(unsigned long int i=0; i < WB_reg.size(); i++){
        WB_reg[i].WB_time++;
    }
    
    unsigned long int temp = WB_reg.size();
    for(unsigned long int i=0; i<temp; i++){
        Instruction current_instr = WB_reg.front();

        for(unsigned long int j=0; j<ROB.size(); j++){
            if(ROB[j].tag == current_instr.rob_tag){ 
                 ROB[j].ready = true; // mark instr ready in rob
                 break;
            }
        }

        RT_reg.push_back(current_instr);
        WB_reg.pop_front();
    }
}

void Processor::execute(){
    for(unsigned long int i=0; i < execute_list.size(); i++){
        execute_list[i].EX_time++;
    }

    for(unsigned long int i = 0; i<execute_list.size(); i++){
        execute_list[i].exec_time += 1;
        
        bool completed = false;
        //check if any instr is ready to move to wb stage this cycle
        if(execute_list[i].op_type == 0 && execute_list[i].exec_time >= 1) {
            completed = true;
        }
        else if(execute_list[i].op_type == 1 && execute_list[i].exec_time >= 2){
            completed = true;
        }
        else if(execute_list[i].op_type == 2 && execute_list[i].exec_time >= 5){
            completed = true;
        }

        if(completed){ // move to wb stage
            execute_list[i].write_back = true;
            WB_reg.push_back(execute_list[i]);
            
            if(execute_list[i].dest != -1){
                int producer_tag = execute_list[i].dest; 

                //mark the sources ready for younger instructions in previous stages
                for(unsigned long int k=0; k<IQ_reg.size(); k++){
                    if(IQ_reg[k].src1 == producer_tag) IQ_reg[k].src1_rdy = true; 
                    if(IQ_reg[k].src2 == producer_tag) IQ_reg[k].src2_rdy = true; 
                }
                for(unsigned long int k=0; k<DI_reg.size(); k++){
                    if(DI_reg[k].src1 == producer_tag) DI_reg[k].src1_rdy = true; 
                    if(DI_reg[k].src2 == producer_tag) DI_reg[k].src2_rdy = true; 
                }
                for(unsigned long int k=0; k<RR_reg.size(); k++){
                    if(RR_reg[k].src1 == producer_tag) RR_reg[k].src1_rdy = true; 
                    if(RR_reg[k].src2 == producer_tag) RR_reg[k].src2_rdy = true; 
                }
            }
        }
    }

    for(unsigned long int i = 0; i < execute_list.size(); i++){
        if(execute_list[i].write_back == true){
            execute_list.erase(execute_list.begin() + i); // remove from exectue_list
            i--;
        }
    }
}

void Processor::issue(){
    for(unsigned long int i=0; i < IQ_reg.size(); i++){
        IQ_reg[i].IS_time++;
    }

    if(IQ_reg.empty()) return;

    for(unsigned int j = 0; j < width; j++){
        bool oldest_ready_found = false;
        int oldest_index = -1;
        
        for(unsigned int i=0; i < IQ_reg.size(); i ++){
            if(IQ_reg[i].src1_rdy && IQ_reg[i].src2_rdy){
                if(oldest_index == -1 || IQ_reg[i].age < IQ_reg[oldest_index].age){
                    oldest_index = i;
                    oldest_ready_found = true;
                }
            }
        }

        if(oldest_ready_found){
            // send oldest AND READY instr to execute list and remove from IQ
            execute_list.push_back(IQ_reg[oldest_index]);
            IQ_reg.erase(IQ_reg.begin() + oldest_index);
        } 
    }
}

void Processor::dispatch(){
    for(unsigned long int i=0; i < DI_reg.size(); i++){
        DI_reg[i].DI_time++;
    }

    // check avaialable IQ space
    if(iq_size - IQ_reg.size() < DI_reg.size()){
        return;
    }
    else{
        while(!DI_reg.empty()){
            Instruction current_instr = DI_reg.front();
            IQ_reg.push_back(current_instr);
            DI_reg.pop_front();
        }
    }
}

void Processor::register_read(){
    for(unsigned long int i=0; i < RR_reg.size(); i++){
        RR_reg[i].RR_time++;
    }
    
    if(DI_reg.size() != 0 || RR_reg.size() == 0){
        return;
    }
    else{
        while(!RR_reg.empty()){
            Instruction current_instr = RR_reg.front();
            
            // Handle Src1
            if(!current_instr.src1_rdy){
                bool tag_found = false;
                bool tag_ready = false;
                
                for(const auto& entry : ROB){
                    if(entry.tag == (unsigned int)current_instr.src1){
                        tag_found = true;
                        tag_ready = entry.ready;
                        break;
                    }
                }
                if(tag_found){
                    if(tag_ready) current_instr.src1_rdy = true;
                } 
                else {
                    current_instr.src1_rdy = true;
                }
            }

            // Handle Src2
            if(!current_instr.src2_rdy){
                bool tag_found = false;
                bool tag_ready = false;
                
                for(const auto& entry : ROB){
                    if(entry.tag == (unsigned int)current_instr.src2){
                        tag_found = true;
                        tag_ready = entry.ready;
                        break;
                    }
                }
                
                if(tag_found){
                    if(tag_ready) current_instr.src2_rdy = true;
                } 
                else {
                    current_instr.src2_rdy = true;
                }
            }

            DI_reg.push_back(current_instr);
            RR_reg.pop_front();
        }
    }
}

void Processor::rename(){
    for(unsigned long int i=0; i < RN_reg.size(); i++){
        RN_reg[i].RN_time++;
    }

    if(RR_reg.size() != 0 || rob_size-ROB.size() < width || RN_reg.size() == 0)
        return;
    
    unsigned long int count = 0;
    while(!RN_reg.empty() && count < width){
        Instruction current_instr = RN_reg.front();
        RN_reg.pop_front();

        current_instr.rob_tag = current_rob_tail;

        ROB_entry current_rob_entry;
        current_rob_entry.pc = current_instr.instr_pc;
        current_rob_entry.dest = current_instr.dest; 
        current_rob_entry.ready = false;
        current_rob_entry.tag = current_rob_tail;
        ROB.push_back(current_rob_entry);

        // Rename Src1
        if(current_instr.src1 != -1){
            if(RMT[current_instr.src1].valid){
                current_instr.src1 = RMT[current_instr.src1].rob_tag; 
                current_instr.src1_rdy = false; 

                // Check readiness of this tag in ROB
                bool tag_found = false;
                bool tag_ready = false;
                for(const auto& entry : ROB){
                    if(entry.tag == (unsigned int)current_instr.src1){
                        tag_found = true;
                        tag_ready = entry.ready;
                        break;
                    }
                }
                if(tag_found && tag_ready) current_instr.src1_rdy = true;
                else if(!tag_found) current_instr.src1_rdy = true; // Retired

            } 
            else {
                current_instr.src1_rdy = true; 
            }
        } 
        else {
            current_instr.src1_rdy = true; 
        }

        // Rename Src2
        if(current_instr.src2 != -1){
            if(RMT[current_instr.src2].valid){
                current_instr.src2 = RMT[current_instr.src2].rob_tag; 
                current_instr.src2_rdy = false;

                // Check readiness of this tag in ROB
                bool tag_found = false;
                bool tag_ready = false;
                for(const auto& entry : ROB){
                    if(entry.tag == (unsigned int)current_instr.src2){
                        tag_found = true;
                        tag_ready = entry.ready;
                        break;
                    }
                }
            if(tag_found && tag_ready) current_instr.src2_rdy = true;
            else if(!tag_found) current_instr.src2_rdy = true; // Retired

            } 
            else {
                current_instr.src2_rdy = true;
            }
        } 
        else {
            current_instr.src2_rdy = true;
        }

        if(current_instr.dest != -1){
            RMT[current_instr.dest].valid = 1;
            RMT[current_instr.dest].rob_tag = current_rob_tail;
            current_instr.dest = current_rob_tail; 
        }

        RR_reg.push_back(current_instr);
        current_rob_tail++; 
        count++;
    }
}

void Processor::decode(){
    for(unsigned long int i=0; i < DE_reg.size(); i++){
        DE_reg[i].DE_time++;
    }

    // check if enough space in RN_reg
    if(RN_reg.size() != 0 || DE_reg.size() == 0){
        return;
    }
    else{
        while(!DE_reg.empty()){
             RN_reg.push_back(DE_reg.front()); // push all instr to next reg stage
             DE_reg.pop_front();
        }
    }
}

void Processor::fetch(FILE *FP) {
    if(DE_reg.size() != 0) return;

    if (FP == NULL) {
        return; 
    }

    uint64_t pc;
    int op_type, dest, src1, src2;

    for (unsigned long int i = 0; i < width; i++) {
        int read_status = read_instruction_from_file(FP, &pc, &op_type, &dest, &src1, &src2);

        if (read_status != EOF && read_status == 5) { 
            Instruction current_instr;
            current_instr.instr_pc = pc;
            current_instr.op_type = op_type;
            current_instr.dest = dest;
            current_instr.src1 = src1;
            current_instr.src2 = src2;
            current_instr.og_dest = dest;
            current_instr.og_src1 = src1;
            current_instr.og_src2 = src2;
            current_instr.src1_rdy = false; 
            current_instr.src2_rdy = false;
            current_instr.age = total_instr_read;
            current_instr.exec_time = 0;
            current_instr.write_back = false;
            current_instr.retired = false;
            current_instr.begin_cycle = cycle_count;
            current_instr.FE_time++; 
            
            DE_reg.push_back(current_instr);
            total_instr_read++;
        } 
        else {
            trace_depleted = true;
            break; 
        }
    }
}

bool Processor::advance_cycle() {
    cycle_count++;
    
    // terminate simulation if all pipeline is empty and end of trace file is ereached
    if(trace_depleted && DE_reg.empty() && RN_reg.empty() && RR_reg.empty() && DI_reg.empty() && IQ_reg.empty() && execute_list.empty() && WB_reg.empty() && ROB.empty()){
        return false;
    }

    return true;
}
