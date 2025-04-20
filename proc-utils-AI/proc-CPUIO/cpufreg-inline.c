#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cpufreq.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/platform_device.h>

#define DEVICE_NAME "cpufreq_inline"
#define CLASS_NAME "cpufregAI"
#define BUF_LEN 64

static struct class*  cpufreq_class  = NULL;
static struct device* cpufreq_device = NULL;
static struct task_struct* freq_thread;
static char command_buf[BUF_LEN] = {0};
static int turbo_mode = 0;
static char *current_policy = "ondemand";

static int apply_turbo_mode(int enable) {
    struct file *f;
    mm_segment_t oldfs;
    char *path = "/sys/devices/system/cpu/intel_pstate/no_turbo";
    char val = enable ? '0' : '1';

    oldfs = get_fs();
    set_fs(KERNEL_DS);
    f = filp_open(path, O_WRONLY, 0);
    if (!IS_ERR(f)) {
        kernel_write(f, &val, 1, &f->f_pos);
        filp_close(f, NULL);
    } else {
        printk(KERN_ERR "[cpufreq_inline] Turbo ayarlanamadı.\n");
        set_fs(oldfs);
        return -1;
    }
    set_fs(oldfs);
    turbo_mode = enable;
    printk(KERN_INFO "[cpufreq_inline] Turbo modu: %s\n", enable ? "Açık" : "Kapalı");
    return 0;
}

static int freq_manager_fn(void *data) {
    while (!kthread_should_stop()) {
        // AI profiline göre CPU'yu yönet (basit örnek: policy değiştir)
        if (strcmp(current_policy, "powersave") == 0)
            cpufreq_update_policy(0);
        else if (strcmp(current_policy, "performance") == 0)
            cpufreq_update_policy(0);

        msleep(5000);
    }
    return 0;
}

static ssize_t freqctl_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    if (len > BUF_LEN - 1)
        return -EINVAL;

    if (copy_from_user(command_buf, buf, len))
        return -EFAULT;

    command_buf[len] = '\0';

    printk(KERN_INFO "[cpufreq_inline] Komut alındı: %s\n", command_buf);

    if (strncmp(command_buf, "turbo_on", 8) == 0) {
        apply_turbo_mode(1);
    } else if (strncmp(command_buf, "turbo_off", 9) == 0) {
        apply_turbo_mode(0);
    } else if (strncmp(command_buf, "policy:", 7) == 0) {
        current_policy = kstrdup(command_buf + 7, GFP_KERNEL);
        printk(KERN_INFO "[cpufreq_inline] Politika değiştirildi: %s\n", current_policy);
    } else if (strncmp(command_buf, "reload", 6) == 0) {
        // Kullanıcı değişikliği tetikledi.
        cpufreq_update_policy(0);
        printk(KERN_INFO "[cpufreq_inline] Yapılandırma yeniden yüklendi.\n");
    }

    return len;
}

static struct file_operations fops = {
    .write = freqctl_write
};

static int __init cpufreq_inline_init(void) {
    int ret;
    struct device *dev_ret;

    ret = register_chrdev(0, DEVICE_NAME, &fops);
    if (ret < 0) return ret;

    cpufreq_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(cpufreq_class)) {
        unregister_chrdev(ret, DEVICE_NAME);
        return PTR_ERR(cpufreq_class);
    }

    dev_ret = device_create(cpufreq_class, NULL, MKDEV(ret, 0), NULL, "freqctl");
    if (IS_ERR(dev_ret)) {
        class_destroy(cpufreq_class);
        unregister_chrdev(ret, DEVICE_NAME);
        return PTR_ERR(dev_ret);
    }

    freq_thread = kthread_run(freq_manager_fn, NULL, "ai_freq_thread");

    printk(KERN_INFO "[cpufreq_inline] AI frekans modülü yüklendi.\n");
    return 0;
}

static void __exit cpufreq_inline_exit(void) {
    kthread_stop(freq_thread);
    device_destroy(cpufreq_class, MKDEV(0, 0));
    class_unregister(cpufreq_class);
    class_destroy(cpufreq_class);
    unregister_chrdev(0, DEVICE_NAME);
    printk(KERN_INFO "[cpufreq_inline] AI frekans modülü kaldırıldı.\n");
}

module_init(cpufreq_inline_init);
module_exit(cpufreq_inline_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Polat Alemdar AI");
MODULE_DESCRIPTION("AI destekli CPU frekans yöneticisi");
