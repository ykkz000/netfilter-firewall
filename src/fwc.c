#define __FWC_IMPLEMENTATION

#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include "fwc.h"

enum __fwc_parse_state {
    STATE_START,
    STATE_AND,
    STATE_OR,
    STATE_NOT,
    STATE_LPARENT,
    STATE_RPARENT,
    STATE_S,
    STATE_SA,
    STATE_SAD,
    STATE_SADD,
    STATE_SADDR,
    STATE_SP,
    STATE_SPO,
    STATE_SPOR,
    STATE_SPORT,
    STATE_P,
    STATE_PR,
    STATE_PRO,
    STATE_PROT,
    STATE_PROTO,
    STATE_PROTOC,
    STATE_PROTOCO,
    STATE_PROTOCOL,
    STATE_ERROR
};

int fwc_parse(struct file *fwc_file, struct fwc_node **p_root);
int fwc_exec(struct fwc_node *root, struct sk_buff *skb)
{
    struct iphdr *iph;
    struct tcphdr *tcph;
    struct udphdr *udph;

    if (root == NULL || skb == NULL)
    {
        return 0;
    }
    iph = ip_hdr(skb);
    switch (root->op)
    {
    case OP_AND:
        return fwc_exec(root->left, skb) && fwc_exec(root->right, skb);
    case OP_OR:
        return fwc_exec(root->left, skb) || fwc_exec(root->right, skb);
    case OP_NOT:
        return !fwc_exec(root->left, skb);
    case OP_SADDR:
        return root->value.saddr == iph->saddr;
    case OP_SPORT:
        switch (iph->protocol)
        {
        case IPPROTO_TCP:
            tcph = tcp_hdr(skb);
            return tcph != NULL && root->value.sport == tcph->source;
        case IPPROTO_UDP:
            udph = udp_hdr(skb);
            return udph != NULL && root->value.sport == udph->source;
        default:
            return 0;
        }
    case OP_PROTOCOL:
        return root->value.protocol == iph->protocol;
    default:
        break;
    }
}
void fwc_free(struct fwc_node *root)
{
    if (root->left != NULL) {
        fwc_free(root->left);
    }
    if (root->right != NULL) {
        fwc_free(root->right);
    }
    kfree(root);
}