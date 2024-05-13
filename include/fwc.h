#ifndef _FWC_H_
#define _FWC_H_

#include <linux/ip.h>
#include <linux/netfilter.h>

enum fwc_operation
{
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_SADDR,
    OP_SPORT,
    OP_PROTOCOL
#ifdef __FWC_IMPLEMENTATION
    ,
    OP_ERROR
#endif
};
struct fwc_node
{
    struct fwc_node *left;
    struct fwc_node *right;
    enum fwc_operation op;
    union
    {
        __be32 saddr;
        __be16 sport;
        __u8 protocol;
    } value;
};

int fwc_parse(struct file *fwc_file, struct fwc_node **p_root);
int fwc_exec(struct fwc_node *root, struct sk_buff *skb);
void fwc_free(struct fwc_node *root);

#ifdef __FWC_IMPLEMENTATION
enum fwc_token_type
{
    TYPE_SYM_AND,
    TYPE_SYM_OR,
    TYPE_SYM_NOT,
    TYPE_SYM_LPARENT,
    TYPE_SYM_RPARENT,
    TYPE_KEY_SADDR,
    TYPE_KEY_SPORT,
    TYPE_KEY_PROTOCOL,
    TYPE_IP_IP,
    TYPE_INT_PORT,
    TYPE_STR_TCP,
    TYPE_STR_UDP,
    TYPE_STR_ICMP,
    TYPE_END,
    TYPE_ERROR
};
enum fwc_symbol_type
{
    SYMBOL_EXP_OR,
    SYMBOL_EXP_AND,
    SYMBOL_EXP_NOT,
    SYMBOL_SADDR,
    SYMBOL_SPORT,
    SYMBOL_PROTOCOL,
    SYMBOL_ERROR
};
#endif

#endif
