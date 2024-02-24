#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libudev.h>
#include <time.h>
#include <sys/select.h>

char* getCurrentDateTime();

int main(int argc, char* argv[]){
    FILE *file;
    file = fopen(argv[1], "a");
    if (file == NULL) {
        printf("Error opening log file.\n");
        return 1;
    }

    struct udev *udev;
    struct udev_device *dev;
    struct udev_monitor *mon;
    int fd;
    int status;
    udev = udev_new();
    if (!udev) {
        fprintf(stderr, "Can't create udev\n");
        return 1;
    }
    mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "block", "disk");
    udev_monitor_enable_receiving(mon);
    fd = udev_monitor_get_fd(mon);
    
    while (1) {
        fd_set fds;
        struct timeval tv;
        int ret;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        ret = select(fd + 1, &fds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(fd, &fds)) {
            dev = udev_monitor_receive_device(mon);
            if (dev) {
                const char *action = udev_device_get_action(dev);
                const char *devname = udev_device_get_sysname(dev);

                if (action && devname && strcmp(action, "add") == 0) {
                    fprintf(file, "%s - flash drive connected: %s\n", getCurrentDateTime(), devname);
                    struct udev_list_entry *properties_list = udev_device_get_properties_list_entry(dev);
                    struct udev_list_entry *entry;
                    udev_list_entry_foreach(entry, properties_list) {
                        const char *key = udev_list_entry_get_name(entry);
                        const char *value = udev_list_entry_get_value(entry);
                        fprintf(file, "  %s: %s\n", key, value);
                    }
                    fflush(file);
                }
                else if (action && devname && strcmp(action, "remove") == 0) {
                    fprintf(file, "%s - flash drive disconnected: %s\n", getCurrentDateTime(), devname);
                    fflush(file);
                }

                udev_device_unref(dev);
            }
        }
        usleep(500 * 1000);
    }
    
    udev_unref(udev);
    fclose(file);
    return 0;
}

char* getCurrentDateTime() {
    time_t now = time(0);
    struct tm* tstruct;
    char* buf = (char*)malloc(80 * sizeof(char));
    tstruct = localtime(&now);
    strftime(buf, 80, "%Y-%m-%d %X", tstruct);
    return buf;
}
