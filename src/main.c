#include <linux/types.h>
#include <linux/netfilter.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/inet.h>
#include "fwc.h"

static int __init nf_fw_init();
static void __exit nf_fw_exit();
static unsigned int nf_fw_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *st);

static struct nf_hook_ops nf_hook_ops_ipv4 = {
    .hook = nf_fw_hook,
    .pf = PF_INET,
    .hooknum = NF_INET_LOCAL_IN,
    .priority = NF_IP_PRI_FIRST};
static struct file *config_file;
static struct fwc_node *fwc_root;

int __init nf_fw_init()
{
    if (nf_register_net_hook(&init_net, &nf_hook_ops_ipv4))
    {
        printk(KERN_ERR "Register hook failed\n");
        return -1;
    }
    config_file = filp_open("/etc/nt_fw/nt_fw.cfg", O_RDONLY, 0644);
    return fwc_parse(config_file, &fwc_root);
}
void __exit nf_fw_exit()
{
    nf_unregister_net_hook(&init_net, &nf_hook_ops_ipv4);
    fwc_free(fwc_root);
    filp_close(config_file, NULL);
}
unsigned int nf_fw_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *st)
{
    return fwc_exec(fwc_root, skb) ? NF_ACCEPT : NF_DROP;
}

module_init(nf_fw_init);
module_exit(nf_fw_exit);
MODULE_AUTHOR("ykkz000 <ykkz000@163.com>");
MODULE_LICENSE("GPL");