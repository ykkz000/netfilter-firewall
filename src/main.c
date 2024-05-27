#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/fs.h>
#include <linux/kernel_stat.h>
#include "fw.h"
#include "uac.h"

const static char COMPILED_FILE_PATH[] = "/etc/nt_fw/nt_fw.fw";

static int __init nf_fw_init();
static void __exit nf_fw_exit();
static unsigned int nf_fw_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *st);

static fw_program_t program;

static struct nf_hook_ops nf_hook_ops_ipv4 = {
    .hook = nf_fw_hook,
    .pf = PF_INET,
    .hooknum = NF_INET_LOCAL_IN,
    .priority = NF_IP_PRI_FIRST};

int __init nf_fw_init()
{
    int ret, pos;
    struct file *compiled_file;
    struct kstat compiled_file_stat;
    if (nf_register_net_hook(&init_net, &nf_hook_ops_ipv4))
    {
        printk(KERN_ERR "Register hook failed\n");
        return -1;
    }
    compiled_file = filp_open(COMPILED_FILE_PATH, O_RDONLY, 0644);
    ret = vfs_stat(COMPILED_FILE_PATH, &compiled_file_stat);
    if (ret < 0)
    {
        printk(KERN_ERR "Open file failed\n");
        return -1;
    }
    program.rom_size = compiled_file_stat.size;
    program.rom = kmalloc(program.rom_size, GFP_KERNEL);
    FS_UACCESS_BEGIN();
    ret = vfs_read(compiled_file, program.rom, program.rom_size, &pos);
    FS_UACCESS_END();
    if (ret < 0)
    {
        printk(KERN_ERR "Read file failed\n");
        return -1;
    }
    filp_close(compiled_file, NULL);
}
void __exit nf_fw_exit()
{
    nf_unregister_net_hook(&init_net, &nf_hook_ops_ipv4);
    kfree(program.rom);
}
unsigned int nf_fw_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *st)
{
    fw_program_init(&program, skb);
    fw_program_run(&program);
    return program.regs[1] ? NF_ACCEPT : NF_DROP;
}

module_init(nf_fw_init);
module_exit(nf_fw_exit);
MODULE_AUTHOR("ykkz000 <ykkz000@163.com>");
MODULE_AUTHOR("niyuta <21371348@buaa.edu.cn>");
MODULE_LICENSE("GPL");