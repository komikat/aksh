#include "headers.h"

void proclore(pid_t pid) {

    // get process information and print it to the console
    struct proc_taskinfo process_info;
    struct proc_bsdinfo bsd_info;

    if (proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &bsd_info, sizeof(bsd_info)) >
        0) {
        printf("\npid: %d\n", pid);
        printf("\nprocess status:%d", bsd_info.pbi_status);
        switch (bsd_info.pbi_status) {
        case 2:
            printf("running...\n");
        }
    } else {
        printf("\nProcess not found!");
    }
}
