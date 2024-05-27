#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#include "fw.h"

static __always_inline void __fw_program_init_reg(fw_program_t *program, struct sk_buff *skb);

void fw_program_init(fw_program_t *program, struct sk_buff *skb)
{
    struct ethhdr *eth;

    program->sr = FW_INIT_SR;
    program->pc = FW_INIT_PC;
    __fw_program_init_reg(program, skb);
}
int fw_program_run_single(fw_program_t *program)
{
    fw_code_t code;
    u32 op;
    u32 immediate;
    u32 rk, rj, rd;
    ssize_t length;
    int modify_pc;
    mm_segment_t old_fs;

    if (program->sr & FW_SR_HLT)
    {
        return E_MACHINE_HALTED;
    }
    if (program->pc >= program->rom_size)
    {
        return E_INVALID_PC;
    }
    code = *((fw_code_t *)(program->rom + program->pc));
    op = fw_code_op(code);
    immediate = fw_code_immediate(code);
    rk = fw_code_rk(code);
    rj = fw_code_rj(code);
    rd = fw_code_rd(code);
    modify_pc = 0;
    switch (op)
    {
    case FW_CODE_OP_NOP:
        break;
    case FW_CODE_OP_LAND:
        program->regs[rd] = program->regs[rj] && program->regs[rk];
        break;
    case FW_CODE_OP_LOR:
        program->regs[rd] = program->regs[rj] || program->regs[rk];
        break;
    case FW_CODE_OP_LNOT:
        program->regs[rd] = !program->regs[rj];
        break;
    case FW_CODE_OP_CMPEQU:
        program->regs[rd] = program->regs[rj] == program->regs[rk];
        break;
    case FW_CODE_OP_SWAP:
        program->regs[rd] = (program->regs[rd] >> (sizeof(fw_reg_t) / 2)) | (program->regs[rd] << (sizeof(fw_reg_t) / 2));
        break;
    case FW_CODE_OP_ADDI:
        program->regs[rd] = program->regs[rj] + immediate;
        break;
    case FW_CODE_OP_HLT:
        program->sr |= FW_SR_HLT;
        break;
    default:
        return E_INVALID_OP;
    }
    program->regs[0] = 0;
    if (!modify_pc)
    {
        program->pc += sizeof(code);
    }
    return E_SUCCESS;
}
int fw_program_run(fw_program_t *program)
{
    int ret;
    while (!(program->sr & FW_SR_HLT))
    {
        if ((ret = fw_program_run_single(program)) != E_SUCCESS)
        {
            return ret;
        }
    }
    return E_SUCCESS;
}

void __fw_program_init_reg(fw_program_t *program, struct sk_buff *skb)
{
    int i;
    struct ethhdr *eth;
    struct iphdr *ip;
    struct ipv6hdr *ipv6;
    struct tcphdr *tcp;
    struct udphdr *udp;

    for (i = 0; i < FW_GENERIC_REGS; i++)
    {
        program->regs[i] = 0;
    }
    eth = eth_hdr(skb);
    program->regs[30] = ntohs(eth->h_proto);
    if (ntohs(eth->h_proto) == ETH_P_IP)
    {
        ip = ip_hdr(skb);
        program->regs[20] = ntohl(ip->saddr);
        program->regs[24] = ntohl(ip->daddr);
        program->regs[31] = ntohs(ip->protocol);
        if (ntohs(ip->protocol) == IPPROTO_TCP)
        {
            tcp = tcp_hdr(skb);
            program->regs[28] = ntohs(tcp->source);
            program->regs[29] = ntohs(tcp->dest);
        }
        else if (ntohs(ip->protocol) == IPPROTO_UDP)
        {
            udp = udp_hdr(skb);
            program->regs[28] = ntohs(udp->source);
            program->regs[29] = ntohs(udp->dest);
        }
    } else if (ntohs(eth->h_proto) == ETH_P_IPV6)
    {
        ipv6 = ipv6_hdr(skb);
        program->regs[20] = ntohl(ipv6->saddr.s6_addr32[3]);
        program->regs[21] = ntohl(ipv6->saddr.s6_addr32[2]);
        program->regs[22] = ntohl(ipv6->saddr.s6_addr32[1]);
        program->regs[23] = ntohl(ipv6->saddr.s6_addr32[0]);
        program->regs[24] = ntohl(ipv6->daddr.s6_addr32[3]);
        program->regs[25] = ntohl(ipv6->daddr.s6_addr32[2]);
        program->regs[26] = ntohl(ipv6->daddr.s6_addr32[1]);
        program->regs[27] = ntohl(ipv6->daddr.s6_addr32[0]);
        program->regs[31] = ntohs(ipv6->nexthdr);
        if (ntohs(ipv6->nexthdr) == IPPROTO_TCP)
        {
            tcp = tcp_hdr(skb);
            program->regs[28] = ntohs(tcp->source);
            program->regs[29] = ntohs(tcp->dest);
        }
        else if (ntohs(ipv6->nexthdr) == IPPROTO_UDP)
        {
            udp = udp_hdr(skb);
            program->regs[28] = ntohs(udp->source);
            program->regs[29] = ntohs(udp->dest);
        }
    }
}
