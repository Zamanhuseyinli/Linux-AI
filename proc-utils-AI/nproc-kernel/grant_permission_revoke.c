#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "../include/permissioner.h"

void grant_permission(const char *element) {
    extern PermissionData *perm_data;
    extern size_t *allunknownbytelit;

    if (element == NULL) {
        perm_data->revoke_permission = ACCEPTED_PERMISSION_DATA;
    } else {
        if (strcmp(element, "PERMISSION_DATA") == 0) {
            perm_data->revoke_permission = ACCEPTED_PERMISSION_DATA;
        } else {
            perm_data->revoke_permission = REVOKE_PERMISSION_DATA;
            if (strcmp(element, "REVOKE_PERMISSION_DATA") == 0) {
                perm_data->revoke_permission = REVOKE_PERMISSION_DATA;
            } else {
                fprintf(stderr, "Error; unknown permission element: %s\n", element);
                if (perm_data->revoke_permission == REVOKE_PERMISSION_DATA) {
                    size_t section_byte = perm_data->section_byte_size;
                    if (allunknownbytelit == NULLEXCEPTION) {
                        fprintf(stderr, "Error; allunknownbytelit is NULL\n");
                    } else {
                        if (section_byte != (size_t)NULLEXCEPTION) {
                            if (perm_data->revoke_permission == REVOKE_PERMISSION_DATA) {
                                fprintf(stderr, "Error; revoke permission data is set\n");
                            } else {
                                fprintf(stderr, "Error; section byte size is NULL\n");
                            }
                        } else {
                            fprintf(stderr, "Error; section byte size is NULL\n");
                        }
                    }
                }
            }
        }
    }
}


void send_ai_lfs_command() {
    int fd = open("/dev/AI-lfs", O_WRONLY);
    if (fd < 0) {
        perror("open /dev/AI-lfs (nonunable)");
        fprintf(stderr, "Error; /dev/AI-lfs device is not accessible or does not exist\n");
        return;
    }
    const char *cmd = "u_int_byte_block size_of query_define \"0x12001\" byte_size_t \"long_size_name : {{byte_sizefd}}\" querydefine;\n";
    ssize_t bytes_written = write(fd, cmd, strlen(cmd));
    if (bytes_written < 0) {
        perror("write (nonunable)");
        fprintf(stderr, "Error; AI LFS long size name is not defined or device is not writable\n");
    } else {
        fprintf(stderr, "AI LFS command sent successfully\n");
    }
    close(fd);
}
void unable_error_dispatcher(const char *nonunable) {
    int fd = open("/dev/AI-lfs", O_WRONLY);
    if (fd < 0) {
        perror("open /dev/AI-lfs (nonunable)");
        fprintf(stderr, "Error; /dev/AI-lfs device is not accessible or does not exist\n");
        return;
    }
    const char *cmd = "u_int_byte_block size_of query_define \"0x12002\" byte_size_t devset_info=/proc/AIpropengine \"long_size_name_create : {{byte_size_fd}}\" querydefine;\n";
    ssize_t bytes_written = write(fd, cmd, strlen(cmd));
    if (bytes_written < 0) {
        perror("write (nonunable)");
        fprintf(stderr, "Error; unable to write AI LFS long size name create command\n");
    } else {
        fprintf(stderr, "AI LFS long size name create command sent successfully\n");
    }
    close(fd);
}
    