#ifndef FW_H
#define FW_H

#include <linux/types.h>
#include <linux/skbuff.h>

typedef u32 fw_code_t;
typedef u32 fw_reg_t;
typedef fw_reg_t fw_addr_t;
typedef u8 fw_byte_t;

#define FW_GENERIC_REGS 32

#define FW_INIT_SR 0x00000000
#define FW_INIT_PC 0x00000000

#define FW_CODE_OP_OFFSET 26
#define FW_CODE_OP_MASK 0xFC000000
#define FW_CODE_IMMEDIATE_OFFSET 10
#define FW_CODE_IMMEDIATE_MASK 0x03FFFC00
#define FW_CODE_RK_OFFSET 10
#define FW_CODE_RK_MASK 0x00007C00
#define FW_CODE_RJ_OFFSET 5
#define FW_CODE_RJ_MASK 0x000003E0
#define FW_CODE_RD_OFFSET 0
#define FW_CODE_RD_MASK 0x0000001F

#define fw_code_op(code) (((code) & FW_CODE_OP_MASK) >> FW_CODE_OP_OFFSET)
#define fw_code_immediate(code) (((code) & FW_CODE_IMMEDIATE_MASK) >> FW_CODE_IMMEDIATE_OFFSET)
#define fw_code_rk(code) (((code) & FW_CODE_RK_MASK) >> FW_CODE_RK_OFFSET)
#define fw_code_rj(code) (((code) & FW_CODE_RJ_MASK) >> FW_CODE_RJ_OFFSET)
#define fw_code_rd(code) (((code) & FW_CODE_RD_MASK) >> FW_CODE_RD_OFFSET)

#define FW_CODE_OP_NOP 0x00
#define FW_CODE_OP_LAND 0x01
#define FW_CODE_OP_LOR 0x02
#define FW_CODE_OP_LNOT 0x03
#define FW_CODE_OP_CMPEQU 0x04
#define FW_CODE_OP_SWAP 0x05
#define FW_CODE_OP_ADDI 0x06
#define FW_CODE_OP_HLT 0x07

#define FW_SR_HLT 0x00000001

typedef struct fw_program
{
    fw_reg_t regs[FW_GENERIC_REGS];
    fw_reg_t sr;
    fw_reg_t pc;
    fw_byte_t *rom;
    fw_addr_t rom_size;
} fw_program_t;

#define E_SUCCESS 0
#define E_MACHINE_HALTED -1
#define E_INVALID_OP -2
#define E_INVALID_PC -3
#define E_ROM_ERROR -4

void fw_program_init(fw_program_t *program, struct sk_buff *skb);
int fw_program_run_single(fw_program_t *program);
int fw_program_run(fw_program_t *program);

#endif