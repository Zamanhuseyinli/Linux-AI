#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/timekeeping.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define PROC_NAME "gpu_buffer_unit"
#define DEFAULT_ENTRY_LEN 128
#define MAX_ENTRIES 512

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zaman Huseyinli - GPU Version");
MODULE_DESCRIPTION("GPU IO tracking buffer kernel module");
MODULE_VERSION("1.0g1-plugin");

static int write_index = 0;
static int buffer_max = 64;
static DEFINE_SPINLOCK(buffer_lock);

static struct kobject *gpu_buffer_kobj;
#define CONFIG_PATH "/var/AI-stump/GPU-runtime.yml"

struct gpu_entry {
    ktime_t timestamp;
    char data[DEFAULT_ENTRY_LEN];
};

static struct gpu_entry *buffer_entries;

static int load_yaml_config(void) {
    struct file *file;
    mm_segment_t old_fs;
    char *buf;
    loff_t pos = 0;
    ssize_t read_size;

    buf = kzalloc(1024, GFP_KERNEL);
    if (!buf)
        return -ENOMEM;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    file = filp_open(CONFIG_PATH, O_RDONLY, 0);
    if (IS_ERR(file)) {
        set_fs(old_fs);
        kfree(buf);
        return PTR_ERR(file);
    }

    read_size = kernel_read(file, buf, 1024, &pos);
    filp_close(file, NULL);
    set_fs(old_fs);

    if (read_size > 0) {
        char *found = strstr(buf, "buffer_size:");
        if (found) {
            int val;
            sscanf(found, "buffer_size: %d", &val);
            buffer_max = clamp(val, 1, MAX_ENTRIES);
        }
    }

    kfree(buf);
    return 0;
}

// /proc/gpu_buffer_unit gösterimi
static int gpu_buffer_show(struct seq_file *m, void *v) {
    int i;
    spin_lock(&buffer_lock);
    for (i = 0; i < write_index; i++) {
        s64 ms = ktime_to_ms(buffer_entries[i].timestamp);
        seq_printf(m, "[%d] (%lld ms) %s\n", i, ms, buffer_entries[i].data);
    }
    spin_unlock(&buffer_lock);
    return 0;
}

static ssize_t gpu_buffer_write(struct file *file, const char __user *ubuf, size_t len, loff_t *off) {
    char temp[DEFAULT_ENTRY_LEN];
    if (len > DEFAULT_ENTRY_LEN - 1)
        len = DEFAULT_ENTRY_LEN - 1;

    if (copy_from_user(temp, ubuf, len))
        return -EFAULT;

    temp[len] = '\0';

    spin_lock(&buffer_lock);

    if (write_index >= buffer_max) {
        memmove(&buffer_entries[0], &buffer_entries[1], sizeof(struct gpu_entry) * (buffer_max - 1));
        write_index = buffer_max - 1;
    }

    buffer_entries[write_index].timestamp = ktime_get();
    strncpy(buffer_entries[write_index].data, temp, DEFAULT_ENTRY_LEN);
    write_index++;

    spin_unlock(&buffer_lock);
    return len;
}

static int gpu_buffer_open(struct inode *inode, struct file *file) {
    return single_open(file, gpu_buffer_show, NULL);
}

static const struct proc_ops gpu_buffer_ops = {
    .proc_open    = gpu_buffer_open,
    .proc_read    = seq_read,
    .proc_write   = gpu_buffer_write,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

// sysfs: /sys/class/gpu_buffer_unit/buffer_size
static ssize_t buffer_size_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", buffer_max);
}

static ssize_t buffer_size_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int new_size;
    if (kstrtoint(buf, 10, &new_size) < 0 || new_size < 1 || new_size > MAX_ENTRIES)
        return -EINVAL;

    spin_lock(&buffer_lock);
    buffer_max = new_size;
    write_index = 0;
    memset(buffer_entries, 0, sizeof(struct gpu_entry) * MAX_ENTRIES);
    spin_unlock(&buffer_lock);

    return count;
}

static struct kobj_attribute buffer_size_attr = __ATTR(buffer_size, 0664, buffer_size_show, buffer_size_store);

// modül başlat
static int __init gpu_buffer_init(void) {
    buffer_entries = kzalloc(sizeof(struct gpu_entry) * MAX_ENTRIES, GFP_KERNEL);
    if (!buffer_entries)
        return -ENOMEM;

    proc_create(PROC_NAME, 0666, NULL, &gpu_buffer_ops);

    gpu_buffer_kobj = kobject_create_and_add("gpu_buffer_unit", kernel_kobj);
    if (!gpu_buffer_kobj)
        return -ENOMEM;

    sysfs_create_file(gpu_buffer_kobj, &buffer_size_attr.attr);

    printk(KERN_INFO "[gpu-buffer-unit] GPU IO buffer modülü yüklendi.\n");
    return 0;
}

static void __exit gpu_buffer_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    kfree(buffer_entries);
    kobject_put(gpu_buffer_kobj);
    printk(KERN_INFO "[gpu-buffer-unit] GPU buffer modülü kaldırıldı.\n");
}

module_init(gpu_buffer_init);
module_exit(gpu_buffer_exit);
