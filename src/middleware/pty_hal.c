#include "pty_hal.h"
#include <pty.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>

extern void app_ui_on_pty_output(const char *chunk);

static int       master_fd = -1;
static pthread_t pty_thread;

static void *pty_read_thread(void *arg)
{
    (void)arg;
    char chunk[512];
    fd_set fds;
    struct timeval tv;

    while (1) {
        FD_ZERO(&fds);
        FD_SET(master_fd, &fds);
        tv.tv_sec  = 0;
        tv.tv_usec = 100000;

        int ret = select(master_fd + 1, &fds, NULL, NULL, &tv);
        if (ret > 0) {
            int n = read(master_fd, chunk, sizeof(chunk) - 1);
            if (n > 0) {
                chunk[n] = '\0';
                app_ui_on_pty_output(chunk);
            }
        }
    }
    return NULL;
}

void pty_init(void)
{
    pid_t pid = forkpty(&master_fd, NULL, NULL, NULL);
    if (pid == 0) {
        execl("/bin/bash", "bash", NULL);
        _exit(1);
    }
    pthread_create(&pty_thread, NULL, pty_read_thread, NULL);
}

void pty_write(const char *data)
{
    if (master_fd >= 0)
        write(master_fd, data, strlen(data));
}
