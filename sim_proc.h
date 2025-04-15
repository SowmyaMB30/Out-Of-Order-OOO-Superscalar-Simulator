#ifndef SIM_PROC_H
#define SIM_PROC_H

typedef struct proc_params{
    unsigned long int rob_size;
    unsigned long int iq_size;
    unsigned long int width;
}proc_params;

struct ROB_st{
   uint32_t seq_no = 0;
   int rob_index;
   int16_t value = 0;
   bool valid = false;
   int16_t dst = 0;
   bool ready = false;
   bool exp = false;
   bool misp = false;
   ROB_st(uint32_t s, int r, uint16_t v, bool val, uint16_t d, bool rd, bool e, bool m)
        : seq_no(s), rob_index(r), value(v), valid(val), dst(d), ready(rd), exp(e), misp(m) {}
} ;




typedef 
struct {
   bool valid = false;
   int robtag = 0;
} RMT_stage;



struct instruction{
   uint32_t seq_no = 0;
   int32_t rob_tag = -1;
   int32_t addr = -1;
   int16_t opcode = -1;
   int16_t dst = -1;
   int16_t rs1 = -1;
   int16_t rs2 = -1;
   int32_t cycle = 0;
   int FE = -1;
   int DE = -1;
   int RN = -1;
   int RR = -1;
   int DI = -1;
   int IS = -1;
   int EX = -1;
   int WB = -1;
   int RT = -1;
   int timer = -1;

   instruction(uint32_t x,int32_t y,int32_t z, int16_t a, int16_t b, int16_t c, int16_t d, int32_t w, int e, int f, int g, int h, int i, int j, int k, int l, int m, int n)
   :  seq_no(x),rob_tag(y),addr(z), opcode(a),dst(b),rs1(c),rs2(d),cycle(w),FE(e),DE(f),RN(g),RR(h),DI(i),IS(j),EX(k),WB(l),RT(m),timer(n) {}

};

struct DE_pipe{
   uint32_t seq_no = 0;
   bool valid;
   int32_t PC;
   int16_t op_type;
   int16_t dest;
   int16_t rs1;
   int16_t rs2;


   DE_pipe(uint32_t s, bool r, uint32_t pc,uint16_t v,uint16_t u,uint16_t x,uint16_t y)
        : seq_no(s), valid(r), PC(pc), op_type(v), dest(u),rs1(x),rs2(y) {}

};
 
struct RN_pipe{
   uint32_t seq_no = 0;
   bool valid = false;
   int16_t op_type =0;
   int16_t dest = 0;
   bool rob1_rdy = false;
   bool rob1 = false;
   int16_t rs1 = 0;
   bool rob2_rdy = false;
   bool rob2 = false;
   int16_t rs2 = 0;
   int timer = -1;

   RN_pipe(uint32_t s, bool r, uint16_t v,uint16_t u, bool val1,bool val2, uint16_t d, bool val3,bool val4, uint16_t rd, int t)
        : seq_no(s), valid(r), op_type(v), dest(u),rob1_rdy(val1), rob1(val2),rs1(d),rob2_rdy(val3), rob2(val4), rs2(rd), timer(t) {}
};



#endif
