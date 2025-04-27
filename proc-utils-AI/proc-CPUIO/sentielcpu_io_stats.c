#include <asm/init.h>
#include <asm/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/random.h>
#include <linux/seq_file.h>

#define PROC_NAME "sentielcpu_io_stats"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zaman Huseyinli");
MODULE_DESCRIPTION("Virtual IO statistics module for AI-LFS");
MODULE_VERSION("1.0o1-plugin");

static int proc_show(struct seq_file *m, void *v) {
    u32 read_bytes, write_bytes;

    // Rastgele ama makul değerler
    get_random_bytes(&read_bytes, sizeof(read_bytes));
    get_random_bytes(&write_bytes, sizeof(write_bytes));

    read_bytes = read_bytes % 50000;
    write_bytes = write_bytes % 50000;

    seq_printf(m, "read: %u\nwrite: %u\n", read_bytes, write_bytes);
    return 0;
}

static int proc_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_file_ops = {
    .proc_open    = proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int __init sentielcpu_io_init(void) {
    proc_create(PROC_NAME, 0, NULL, &proc_file_ops);
    printk(KERN_INFO "[sentielcpu_io_stats] Yüklendi.\n");
    return 0;
}

static void __exit sentielcpu_io_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "[sentielcpu_io_stats] Kaldırıldı.\n");
}

module_init(sentielcpu_io_init);
module_exit(sentielcpu_io_exit);
