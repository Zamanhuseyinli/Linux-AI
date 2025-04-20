#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/cpumask.h>
#include <linux/smp.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define PROC_NAME "cpu_sensivity_frogline"
#define CHECK_INTERVAL_MS 2000
#define CPU_SENSIVITY_THRESHOLD 70

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zaman Huseyinli");
MODULE_DESCRIPTION("AI response module based on CPU sensitivity");
MODULE_VERSION("1.0o1-plugin");

static struct task_struct *monitor_thread;
static int cpu_over_threshold = 0;

// CPU kullanımını basitçe kontrol eden fonksiyon
static int check_cpu_usage(void) {
    unsigned int i, cpu;
    int total_usage = 0;

    for_each_online_cpu(cpu) {
        struct kernel_cpustat kcs;
        cputime64_t user, nice, system, idle;
        u64 total, busy;

        user = kcpustat_cpu(cpu).cpustat[CPUTIME_USER];
        nice = kcpustat_cpu(cpu).cpustat[CPUTIME_NICE];
        system = kcpustat_cpu(cpu).cpustat[CPUTIME_SYSTEM];
        idle = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];

        busy = user + nice + system;
        total = busy + idle;

        if (total == 0) continue;

        total_usage += (busy * 100) / total;
    }

    return total_usage / num_online_cpus();
}

static int monitor_fn(void *data) {
    while (!kthread_should_stop()) {
        int usage = check_cpu_usage();
        cpu_over_threshold = (usage > CPU_SENSIVITY_THRESHOLD) ? 1 : 0;
        ssleep(CHECK_INTERVAL_MS / 1000);
    }
    return 0;
}

static int proc_show(struct seq_file *m, void *v) {
    if (cpu_over_threshold) {
        seq_puts(m, "ALERT: CPU threshold exceeded. AI optimization required.\n");
    } else {
        seq_puts(m, "NORMAL: CPU usage is within acceptable limits.\n");
    }
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

static int __init frogline_init(void) {
    proc_create(PROC_NAME, 0, NULL, &proc_file_ops);
    monitor_thread = kthread_run(monitor_fn, NULL, "frogline_monitor");
    printk(KERN_INFO "[frogline] CPU Sensivity Plugin yüklendi.\n");
    return 0;
}

static void __exit frogline_exit(void) {
    if (monitor_thread) {
        kthread_stop(monitor_thread);
    }
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "[frogline] CPU Sensivity Plugin kaldırıldı.\n");
}

module_init(frogline_init);
module_exit(frogline_exit);
