#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define PROC_NAME "sentielcpu_io_stats"
#define DEV_NAME "ai_lfs"
#define BUFFER_SIZE 128

static char kernel_buffer[BUFFER_SIZE] = "Initial IO Stats: \n";
static char ai_buffer[BUFFER_SIZE] = "AI Decision: none\n";
static int major;
static struct class* ai_class = NULL;
static struct device* ai_device = NULL;

/* /proc interface */
static ssize_t proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    return simple_read_from_buffer(buf, count, ppos, kernel_buffer, strlen(kernel_buffer));
}

static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
};

/* char device read */
static ssize_t ai_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    return simple_read_from_buffer(buf, len, off, ai_buffer, strlen(ai_buffer));
}

/* char device write */
static ssize_t ai_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
    if (len > BUFFER_SIZE - 1)
        len = BUFFER_SIZE - 1;

    if (copy_from_user(ai_buffer, buf, len) == 0) {
        ai_buffer[len] = '\0';
        printk(KERN_INFO "[react_new_t] AI tahmini alındı: %s\n", ai_buffer);

        // Kernel tarafında kullanıcı alanına sinyal gönderme ya da AI-GBinder IPC tetikleme noktası
        // Kullanıcı alanı daemon: /dev/ai_lfs üzerinden bu yazılanı okur ve işlem yapar.
    }

    return len;
}

static struct file_operations ai_fops = {
    .owner = THIS_MODULE,
    .read = ai_read,
    .write = ai_write,
};

/* init */
static int __init sentiel_init(void) {
    /* /proc kaydı */
    proc_create(PROC_NAME, 0666, NULL, &proc_fops);

    /* Karakter aygıtı oluştur */
    major = register_chrdev(0, DEV_NAME, &ai_fops);
    ai_class = class_create(THIS_MODULE, "ai_class");
    ai_device = device_create(ai_class, NULL, MKDEV(major, 0), NULL, DEV_NAME);

    printk(KERN_INFO "[sentielcpuiostringmod] Başlatıldı. [react_new_t AI bağlantısı aktif]\n");

    return 0;
}

/* çıkış */
static void __exit sentiel_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    device_destroy(ai_class, MKDEV(major, 0));
    class_unregister(ai_class);
    class_destroy(ai_class);
    unregister_chrdev(major, DEV_NAME);

    printk(KERN_INFO "[sentielcpuiostringmod] Kaldırıldı.\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zaman huseyinli");
MODULE_DESCRIPTION("The AI-based kernel module is integrated into the gbinder structure via cbinder and uses binderfs for the kernel artificial intelligence automation system.");
MODULE_VERSION("1.0.o1");

module_init(sentiel_init);
module_exit(sentiel_exit);
