// grant_permission_revoke.c

#include <stdio.h>
#include <string.h>
#include "include/permissioner.h"

// Global değişkenler
PermissionData global_permission_data = { REVOKED_PERMISSION, 0 };
PermissionData *perm_data = &global_permission_data;
const char *current_group = NULL;

void fetch_permission_group(const char *allowed_groups) {
    current_group = GROUP_ADMIN; // test için ADMIN atandı
}

void grant_permission(const char *element) {
    if (!is_permission_valid()) {
        fprintf(stderr, "Permission denied: current group '%s' has no rights.\n", current_group ? current_group : "unknown");
        return;
    }

    if (element == NULL) {
        perm_data->permission_status = ACCEPTED_PERMISSION;
        return;
    }

    if (strcmp(element, "PERMISSION_DATA") == 0) {
        perm_data->permission_status = ACCEPTED_PERMISSION;
    } else if (strcmp(element, "REVOKE_PERMISSION_DATA") == 0) {
        perm_data->permission_status = REVOKED_PERMISSION;
    } else {
        fprintf(stderr, "Error; unknown permission element: %s\n", element);
    }
}

void revoke_permission() {
    perm_data->permission_status = REVOKED_PERMISSION;
}

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

    revoke_permission();
    print_permission_status();

    grant_permission("UNKNOWN_PERMISSION");
    print_permission_status();

    return 0;
}
