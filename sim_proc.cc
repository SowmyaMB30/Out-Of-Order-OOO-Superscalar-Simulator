#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sim_proc.h"
#include <cmath>
#include <iostream>
#include <cstdio>
#include <vector>
#include <iomanip>
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    proc_params params;       // look at sim_bp.h header file for the the definition of struct proc_params
    int op_type, dest, src1, src2;  // Variables are read from trace file
    uint64_t pc; // Variable holds the pc read from input file
    
    if (argc != 5)
    {
        printf("Error: Wrong number of inputs:%d", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.rob_size     = strtoul(argv[1], NULL, 10);
    params.iq_size      = strtoul(argv[2], NULL, 10);
    params.width        = strtoul(argv[3], NULL, 10);
    trace_file          = argv[4];

    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s", trace_file);
        exit(EXIT_FAILURE);
    }
    
     // Initialize structures and variables for simulation
    uint32_t seq_no = 0;                   // Sequence number of instructions
     RMT_stage* RMT = new RMT_stage[67];   // Rename Map Table
    uint32_t WB_size = params.width*5 ;
    std::vector<DE_pipe> Decode_arr;      // Decode pipeline register
    std::vector<DE_pipe> Fetch_arr;       // Fetch pipeline register
    int lastflag = 0;                     // Flag indicating end of trace
    std::vector<RN_pipe> Rename_arr;     // Rename pipeline register
    std::vector<RN_pipe>RegRd_arr;       // Register Read pipeline register
    std::vector<RN_pipe>Dispatch_arr;    // Dispatch pipeline register
    std::vector<instruction> inst;      // List of instructions
    std::vector<ROB_st> ROB;             // Reorder Buffer
    std::vector<RN_pipe> IQ;            // Issue Queue
    std::vector<RN_pipe> WB;              // Writeback buffer
    std::vector<RN_pipe> RT;               // Retire buffer
    std::vector<int> pipe_rdy;              // List of ready dependencies
    std::vector<RN_pipe> execute_list;        // Execution list
    int rob_index = 0;
    uint32_t cycle = 0;

     // Variables for instruction timing
    int fetch;
    int decode;
    int rename;
    int regrd;
    int dispatch;
    int issue;
    int execute;
    int writeback;
    int retire;
    int end = 0;
    int retire_cnt;

   // Main simulation loop
    do
    {
        retire_cnt=params.width;
// === Retire Stage ===

        {for (std::vector<ROB_st>::iterator itt = ROB.begin(); itt != ROB.end(); ++itt) 
        {
            if(itt->ready==true)
            {
                if(retire_cnt>0)
                {
                    // for (auto& ele : inst) 
                    for (std::vector<instruction>::iterator it = inst.begin(); it != inst.end(); ++it) 
                    {
                        if(it->seq_no == itt->seq_no)
                        {
                            fetch=it->cycle; 
                            decode=fetch+it->FE; 
                            rename=decode+it->DE;
                            regrd=rename+it->RN; 
                            dispatch=regrd+it->RR; 
                            issue=dispatch+it->DI; 
                            execute=issue+it->IS; 
                            writeback=execute+it->EX;
                            retire=writeback+it->WB; 
                            printf("%d fu{%d} src{%d,%d} dst{%d} FE{%d,%d} DE{%d,%d} RN{%d,%d} RR{%d,%d} DI{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,%d} RT{%d,%d}\n"
                            ,it->seq_no,it->opcode,it->rs1,it->rs2,it->dst,fetch,it->FE,decode,it->DE,rename,it->RN,regrd,it->RR,dispatch,it->DI,issue,it->IS,
                            execute,it->EX,writeback,it->WB,retire,it->RT);
                            end = retire+it->RT;

                            it = inst.erase(it);
                            --it;
                            retire_cnt--;
                            break;
                            
                            }
                            
                            }
                            itt = ROB.erase(itt);
                            --itt;
                        }
                        

                    }  
                    else if(itt->ready==false) break;  
                }}

        
 // === Writeback Stage ===
        for (std::vector<RN_pipe>::iterator it =WB.begin(); it != WB.end(); ++it)
        {

            {for (std::vector<ROB_st>::iterator ele =ROB.begin(); ele != ROB.end(); ++ele)
           
            {
                if(it->dest == ele->rob_index)
                {
                    ele->ready = true;
                    for (std::vector<instruction>::iterator e =inst.begin(); e != inst.end(); ++e)
                   
                    {
                        if(e->seq_no == it->seq_no)
                        {e->RT++;
                        break;} 
                    }
                    break;
                }                
            }
            }         
        }

        WB.clear();
        for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element)
        
        {
        if (element->RT > -1) {
            element->RT++; 
        }}
        

            {
                for (std::vector<RN_pipe>::iterator it = execute_list.begin(); it != execute_list.end(); ++it) {
                if(((it->timer)<=0)&&(WB.size()<(params.width*5))) {
            
                WB.push_back(*it);

            for (std::vector<instruction>::iterator element = inst.begin(); element != inst.end(); ++element)
             
            {
                if (element->seq_no == it->seq_no) 
                {
                    element->WB++; 
                }
            }
            it = execute_list.erase(it);
            --it;
            }}}       
        
        for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element) {
        if ((element->RT == -1)&&(element->WB > -1)) {
            element->WB++; 
        }} 
        
// === Execute Stage ===

        int final = execute_list.size() + params.width;

            {for (std::vector<RN_pipe>::iterator it = IQ.begin(); it != IQ.end(); ++it) {
            if((execute_list.size()<final)&&(execute_list.size()<(params.width*5)))    
            if(((!(it->rob1))|(it->rob1_rdy))&((!(it->rob2))|(it->rob2_rdy))) {

            execute_list.push_back(*it);
            it = IQ.erase(it);
            --it;
            }} }           

        for (std::vector<RN_pipe>::iterator element = execute_list.begin(); element != execute_list.end(); ++element)
        
            {
                element->timer--;
                for (std::vector<instruction>::iterator ele =inst.begin(); ele != inst.end(); ++ele)  {
                if (ele->seq_no == element->seq_no) 
                {
                    if(ele->EX==-1) ele->EX++;
                    ele->EX++;
                }}
                if(element->timer == 0) pipe_rdy.push_back(element->dest);
            }

        if((Dispatch_arr.size()==params.width)||(lastflag==1))
        while(((params.iq_size-IQ.size())>=Dispatch_arr.size())&&Dispatch_arr.size())
        {
            const RN_pipe& old =Dispatch_arr.front();            
            IQ.push_back(old);
            RN_pipe& iq_new = IQ.back();
            
            for (std::vector<ROB_st>::iterator element = ROB.begin(); element != ROB.end(); ++element)
          
            {                
                if((element->rob_index == old.rs1)&&(iq_new.rob1==true)) 
                {
                    if(element->ready==true) iq_new.rob1_rdy=true;
                }
                if((element->rob_index == old.rs2)&&(iq_new.rob2==true)) 
                {
                    if(element->ready==true) iq_new.rob2_rdy=true;
                }
            }

            for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element)    
            
            {
                if (element->seq_no == old.seq_no) 
                {
                    element->IS++; 
                }
            }
           Dispatch_arr.erase(Dispatch_arr.begin());
        }

        for (std::vector<RN_pipe>::iterator element =IQ.begin(); element != IQ.end(); ++element)
      
        {
            if(element->rob1==true)
            {
                for (std::vector<int>::iterator it = pipe_rdy.begin(); it != pipe_rdy.end(); ++it)
              
                {
                    int value = *it;
                    if(value == element->rs1) element->rob1_rdy = true;
                }
            }
            if(element->rob2==true)
            {
                for (std::vector<int>::iterator it = pipe_rdy.begin(); it != pipe_rdy.end(); ++it)
                
                {
                    int value = *it;
                    if(value == element->rs2) element->rob2_rdy = true;
                }
            }
            if(element->timer == -1)
            {if(element->op_type == 0) element->timer=1;
            else if(element->op_type == 1) element->timer=2;
            else if(element->op_type == 2) element->timer=5;}
        }

        for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element) {
       
        if ((element->EX == -1)&&(element->IS > -1)) {
            element->IS++; 
        }}

// === Dispatch Stage ===
        if((Dispatch_arr.size()==0)&&((RegRd_arr.size()==params.width)||(lastflag==1)))
        while((Dispatch_arr.size()<params.width)&&(RegRd_arr.size()))
        {
            const RN_pipe& old =RegRd_arr.front();

           Dispatch_arr.push_back(old);
            for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element) {
            
            if (element->seq_no == old.seq_no) {
            element->DI++; 
            }}
           RegRd_arr.erase(RegRd_arr.begin());
        }
        for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element) {
        
        if ((element->IS == -1)&&(element->DI > -1)) {
            element->DI++; 
        }}
        
// === Register Read Stage ===
        if((RegRd_arr.size()==0)&&((params.rob_size-ROB.size())>=Rename_arr.size())&&((Rename_arr.size()==params.width)||(lastflag==1)))
        while(((RegRd_arr.size()<params.width)&&(ROB.size()<params.rob_size))&&(Rename_arr.size()))
        {
            const RN_pipe& old = Rename_arr.front();

           RegRd_arr.push_back(old);
            RN_pipe& latest =RegRd_arr.back();

            if(latest.rs1 >= 0)
            if(RMT[latest.rs1].valid)
            {
                latest.rs1 = RMT[latest.rs1].robtag;
                latest.rob1 = true;
            }
            if(latest.rs2 >= 0)
            if(RMT[latest.rs2].valid)
            {
                latest.rs2 = RMT[latest.rs2].robtag;
                latest.rob2 = true;
            }

            ROB.push_back({latest.seq_no,rob_index,static_cast<int16_t>(0),true,latest.dest,false,false,false});

            if(latest.dest>0)
            {
                RMT[latest.dest].valid = true;
                RMT[latest.dest].robtag = rob_index;
            }     

            latest.dest = rob_index;

            rob_index++; 

            for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element) {
            
            if (element->seq_no == old.seq_no) {
            element->RR++; 
            }}
            Rename_arr.erase(Rename_arr.begin());
        }
        for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element) {
        if ((element->DI == -1)&&(element->RR > -1)) {
            element->RR++; 
        }}

// === Rename Stage ===
        if((Rename_arr.size()==0)&&((Decode_arr.size()==params.width)||(lastflag==1)))

        while((Rename_arr.size()<params.width)&&(Decode_arr.size()))
        {
            const DE_pipe& old = Decode_arr.front();
 
            Rename_arr.push_back({old.seq_no,true,old.op_type,old.dest,false,false,old.rs1,false,false,old.rs2,-1});
            
            for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element) {
           
            if (element->seq_no == old.seq_no) {
            element->RN++; 
            }}
            Decode_arr.erase(Decode_arr.begin());
        }
        for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element) {
        if ((element->RR == -1)&&(element->RN > -1)) {
            element->RN++; 
        }}

        if((Decode_arr.size()==0)&&((Fetch_arr.size()==params.width)||(lastflag==1)))
        while((Decode_arr.size()<params.width)&&(Fetch_arr.size()))
        {
            const DE_pipe& old = Fetch_arr.front();
    
            Decode_arr.push_back(old);
            for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element) {
           
            if (element->seq_no == old.seq_no) {
            element->DE++; 
            }}
            Fetch_arr.erase(Fetch_arr.begin());
           
        }
        for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element) {
        if ((element->RN == -1)&&(element->DE > -1)) {
            element->DE++; 
        }}
           
           int in =1;
// === Decode Stage ===

        while((Fetch_arr.size()<params.width)&&(in==1))
        {

            if(fscanf(FP, "%lx %d %d %d %d", &pc, &op_type, &dest, &src1, &src2) != EOF)
            {
            in = 1;

            Fetch_arr.push_back({static_cast<uint32_t>(seq_no),true,static_cast<int32_t>(pc), static_cast<int16_t>(op_type), static_cast<int16_t>(dest), static_cast<int16_t>(src1), static_cast<int16_t>(src2)});

            inst.push_back({static_cast<uint32_t>(seq_no),static_cast<int32_t>(-1),static_cast<int32_t>(pc),static_cast<int16_t>(op_type), static_cast<int16_t>(dest), static_cast<int16_t>(src1),static_cast<int16_t>(src2),static_cast<int32_t>(cycle),0,-1, -1, -1, -1, -1, -1, -1, -1, -1});
            seq_no++;
        }
        else {in = 0;lastflag = 1;}
        
        }
       
        for (std::vector<instruction>::iterator element =inst.begin(); element != inst.end(); ++element) {
        if(element->DE == -1) 
        {
            if(element->FE >=0)
            element->FE++; 
            if(element->FE>1)
            {element->FE--;element->cycle++;}
        }}
      
        cycle++;   // Advance simulation cycle

    }while (inst.size());  // Loop until all instructions are retired

 // Output final simulation results
std::cout << "# === Simulator Command =========" << std::endl;
std::cout << "# ./sim " << params.rob_size << " " << params.iq_size << " " << params.width << " " << trace_file << std::endl;
std::cout << "# === Processor Configuration ===" << std::endl;
std::cout << "# ROB_SIZE = " << params.rob_size << std::endl;
std::cout << "# IQ_SIZE  = " << params.iq_size << std::endl;
std::cout << "# WIDTH    = " << params.width << std::endl;
std::cout << "# === Simulation Results ========" << std::endl;
std::cout << "# Dynamic Instruction Count    = " << seq_no << std::endl;
std::cout << "# Cycles                       = " << end << std::endl;
std::cout << "# Instructions Per Cycle (IPC) = " 
          << std::fixed << std::setprecision(2) // Format IPC to 2 decimal places
          << static_cast<float>(seq_no) / static_cast<float>(end) << std::endl;
return 0;

}
