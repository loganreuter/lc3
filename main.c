//Machine Model: von Neumann
//Based around the LC3 machine code instruction set
/*
Following the code from:
    https://www.andreinc.net/2021/12/01/writing-a-simple-vm-in-less-than-125-lines-of-c
    https://www.jmeiners.com/lc3-vm/
*/
#include <stdint.h>

#define MEMORY_MAX (1 << 16)
uint16_t memory[MEMORY_MAX];

uint16_t PC_START = 0x3000;

//Helper function that reads from memory
static inline uint16_t mem_read(uint16_t address){ return memory[address]; }

//Helper function that writes to memory
static inline void mem_write(uint16_t address, uint16_t val){ return memory[address] = val; }

/*
Registers:
    R0-R7: General purpose registers
    RPC: Program counter register
    RCND: Conditional register
*/
enum {
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    RPC,
    RCND,
    RCNT
};

uint16_t reg[RCNT];

enum {
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N*/
};
static inline void uf(enum regist r) {
    if (reg[r]==0) reg[RCND] = FL_ZRO;
    else if(reg[r]>>15) reg[RCND] = FL_NEG;
    else reg[RCND] = FL_POS;
}

//Macro to extract the OpCode
#define OPC(i) ((i) >> 12)

// define the number of operations
#define NOPS (16) 
typedef void (*op_ex_f)(uint16_t instruction);

/*
Macros for the OpCodes
*/
#define SEXTIMM(i) sext(IMM(i), 5)
static inline uint16_t sext(uint16_t n, int b){
    return((n>>(b-1))&1) ? 
        (n|(0xFFFF << b)) : n;
}
#define FIMM(i) ( (i >> 5) & 1)
#define DR(i) (((i) >> 9) & 0x7)
#define SR1(i) (((i) >> 6) & 0x7)
#define SR2(i) ((i) & 0x7)
#define IMM(i) ((i) & 0x1F)

static inline void BR(uint16_t i) {}
static inline void ADD(uint16_t i) {
    reg[DR(i)] = reg[SR1(i)] + (
        FIMM(i) ? 
            SEXTIMM(i) : 
            reg[SR2(i)]
    );

    uf(DR(i)); //Update the conditional register
}
static inline void LD(uint16_t i) {}
static inline void ST(uint16_t i) {}
static inline void JSR(uint16_t i) {}
static inline void AND(uint16_t i) {
    reg[DR(i)] = reg[SR1(i)] & (
        FIMM(i) ?               //If the 5th bit is 1
            SEXTIMM(i) :        //Extend IMM5 and compare it to SR1
            reg[SR2(i)]         //Otherwise we compare SR2 to SR1
    );

    uf(DR(i)); //Update the conditional register
}
static inline void LDR(uint16_t i) {}
static inline void STR(uint16_t i) {}
static inline void RTI(uint16_t i) {}
static inline void NOT(uint16_t i) {}
static inline void LDI(uint16_t i) {}
static inline void STI(uint16_t i) {}
static inline void JMP(uint16_t i) {}
static inline void RES(uint16_t i) {}
static inline void LEA(uint16_t i) {}
static inline void TRAP(uint16_t i) {}


op_ex_f op_ex[NOPS] = {
    BR,
    ADD,
    LD,
    ST,
    JSR,
    AND,
    LDR,
    STR,
    RTI,
    NOT,
    LDI,
    STI,
    JMP,
    RES,
    LEA,
    TRAP
};