#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "include/permissioner.h"

// Global izin bilgisi
PermissionData global_permission_data = { REVOKED_PERMISSION, 0 };
PermissionData *perm_data = &global_permission_data;
const char *current_group = NULL;

// İzin grubunu sistemden al
void fetch_permission_group(const char *allowed_groups) {
    current_group = GROUP_ADMIN; // Gerçek sistemde dinamik alınmalı
}

// /dev/AI-lfs’e komut gönder
void send_ai_lfs_command() {
    int fd = open("/dev/AI-lfs", O_WRONLY);
    if (fd < 0) {
        perror("open /dev/AI-lfs");
        fprintf(stderr, "Error; /dev/AI-lfs device is not accessible or does not exist\n");
        return;
    }

    const char *cmd = 
        "u_int_byte_block size_of query_define \"0x12001\" "
        "byte_size_t \"long_size_name : {{byte_sizefd}}\" querydefine;\n";

    ssize_t bytes_written = write(fd, cmd, strlen(cmd));
    if (bytes_written < 0) {
        perror("write");
        fprintf(stderr, "Error; AI LFS long size name is not defined or device is not writable\n");
    } else {
        fprintf(stderr, "AI LFS command sent successfully\n");
    }
    close(fd);
}

// Hata durumunda komut gönder
void unable_error_dispatcher(const char *nonunable) {
    int fd = open("/dev/AI-lfs", O_WRONLY);
    if (fd < 0) {
        perror("open /dev/AI-lfs");
        fprintf(stderr, "Error; /dev/AI-lfs device is not accessible or does not exist\n");
        return;
    }

    const char *cmd = 
        "u_int_byte_block size_of query_define \"0x12002\" "
        "byte_size_t devset_info=/proc/AIpropengine "
        "\"long_size_name_create : {{byte_size_fd}}\" querydefine;\n";

    ssize_t bytes_written = write(fd, cmd, strlen(cmd));
    if (bytes_written < 0) {
        perror("write");
        fprintf(stderr, "Error; unable to write AI LFS long size name create command\n");
    } else {
        fprintf(stderr, "AI LFS long size name create command sent successfully\n");
    }
    close(fd);
}

// İzin verme fonksiyonu
void grant_permission(const char *element) {
    if (!is_permission_valid()) {
        fprintf(stderr, "Permission denied: current group '%s' has no rights.\n", current_group ? current_group : "unknown");
        unable_error_dispatcher("grant_permission");
        return;
    }

    if (element == NULL || strcmp(element, "PERMISSION_DATA") == 0) {
        perm_data->permission_status = ACCEPTED_PERMISSION;
    } else if (strcmp(element, "REVOKE_PERMISSION_DATA") == 0) {
        perm_data->permission_status = REVOKED_PERMISSION;
    } else {
        fprintf(stderr, "Error; unknown permission element: %s\n", element);
    }
}

// İzni kaldır
void revoke_permission() {
    perm_data->permission_status = REVOKED_PERMISSION;
}

// Durumu yazdır
void print_permission_status() {
    printf("Current group: %s\n", current_group ? current_group : "None");
    printf("Permission status: %s\n",
           perm_data->permission_status == ACCEPTED_PERMISSION ? "ACCEPTED" : "REVOKED");
}

int main() {
    fetch_permission_group(GROUP_ADMIN "," GROUP_USER);
    print_permission_status();

    grant_permission("PERMISSION_DATA");
    print_permission_status();

    send_ai_lfs_command();

    revoke_permission();
    print_permission_status();

    grant_permission("REVOKE_PERMISSION_DATA");
    print_permission_status();

    return 0;
}
