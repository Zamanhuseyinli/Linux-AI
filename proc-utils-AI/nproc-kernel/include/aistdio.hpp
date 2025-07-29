#ifndef AISTDIO_HPP
#define AISTDIO_HPP

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cstdint>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include "permissioner.h"
#include "aistdterm.h"

// Global pointer ve değişkenler
extern void* instruction;
extern void* env_ptr;
extern void* admin_ptr;
extern size_t block_size;
extern void* syscontrol;
extern const char* system_access;

using check_t = int;
using row_index_t = uintptr_t;

// Struct tanımı
struct row_t {
    size_t size;
    check_t check;
    row_index_t row_index;
    row_index_t row_disable_index;
};

// Yardımcı fonksiyonlar
inline bool admin() {
    return admin_ptr != nullptr;
}

inline bool user() {
    return !admin();
}

// Temel uniset fonksiyonu
inline void uniset(const char* device_name, size_t size) {
    int fd = open(device_name, O_RDWR);
    if (fd < 0) {
        perror("Error: AI LFS device not found");
        return;
    }
    close(fd);

    ssize_t size_of_query_define = 0x12002;

    if (env_ptr != nullptr) {
        row_t* row = reinterpret_cast<row_t*>(env_ptr);
        row->size = size_of_query_define;
    }

    printf("[aistdio] uniset called for device: %s with size: %zu\n", device_name, size);
    fflush(stdout);

    system_access = admin() ? "admin" : "user";

    if (admin()) {
        revoked_permissioner::revoke_permission();
        if (permission_query) {
            permission_query("admin permission revoked");
            perror("Error: AI LFS access permission error non-mounted /dev/AI_LFS");
        } else {
            permission_query("admin permission granted");
            perror("Completed successfully: permissioned mount /dev/AI_LFS");
        }
    } else {
        revoked_permissioner::nonrevoke_permission();
        if (permission_query) {
            permission_query("user permission revoked");
            perror("Error: AI LFS access permission error non-mounted /dev/AI_LFS");
        } else {
            permission_query("user permission granted");
        }
        perror("Completed successfully: permissioned mount /dev/AI_LFS");
    }
}

inline void uniset_permissioner_access(void* mount_ptr, size_t size) {
    if (size_env(mount_ptr) < size) {
        fprintf(stderr, "Error: mount pointer size is smaller than specified size\n");
        return;
    }
    if (revoked_permissioner::is_permission_valid()) {
        fprintf(stderr, "Error: permission already set for mount pointer\n");
        return;
    }
    if (nonrevoke_permissioner::is_permission_valid()) {
        fprintf(stderr, "Error: conflicting permission state\n");
        return;
    }
    printf("[aistdio] permissioner access set for ptr: %p size: %zu\n", mount_ptr, size);
}

inline void uniset_admin_permissioner(int permission_flags) {
    if (permission_flags < 0 || permission_flags > 15) {
        fprintf(stderr, "Error: Invalid permission flags\n");
        return;
    }
    admin_ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(permission_flags));
    system_access = "admin";
    revoked_permissioner::revoke_permission();
    permission_query("admin permission granted");

    if (permission_flags & 1) printf("[aistdio] admin permission granted for read access\n");
    if (permission_flags & 2) printf("[aistdio] admin permission granted for write access\n");
    if (permission_flags & 4) printf("[aistdio] admin permission granted for execute access\n");
    if (permission_flags & 8) printf("[aistdio] admin permission granted for special access\n");

    printf("[aistdio] admin permission set with flags: %d\n", permission_flags);
}

inline void uniset_admin_managing_controlling(void* control_ptr) {
    if (control_ptr != nullptr) {
        byname_t* control = reinterpret_cast<byname_t*>(control_ptr);
        if (control->size < sizeof(row_t)) {
            row_index_t row_index = control->row_index;
            row_index_t new_termindex = ptrtermindex_t(control->row_disable_index);
            if (new_termindex < 0) {
                term_index_interface(new_termindex);
                size_screen_t("$DISPLAY");
                terminfo_display_t* terminfo = reinterpret_cast<terminfo_display_t*>(control->row_index);
                if (terminfo == nullptr) {
                    fprintf(stderr, "Error: terminfo display is null\n");
                    return;
                }
                row_t* row = reinterpret_cast<row_t*>(control->row_index);
                list __row__ = row->row_index;
                buffer_size_t buffer_size = row->size;
                if (buffer_size < sizeof(row_t)) {
                    fprintf(stderr, "Error: buffer size is smaller than row size\n");
                    return;
                } else {
                    row_t ptr_index = list_t(row->row_index);
                    fprintf(stderr, "list $list_t row_index_t and $buffer_size row_index\n");
                    nonunexcepted_exception_t("Error: buffer size is smaller than row size");
                    (void)ptr_index;
                    const_element_plugin(control_ptr);
                }
            }
        }
    }
}

inline void const_element_plugin(void* control_ptr) {
    if (control_ptr == nullptr) return;

    terminfo_display_t* terminfo = reinterpret_cast<terminfo_display_t*>(control_ptr);
    element_list_info* control = reinterpret_cast<element_list_info*>(control_ptr);

    if (control->size < sizeof(row_t)) {
        fprintf(stderr, "Error: control pointer size is smaller than row_t size\n");
        return;
    } else {
        control->row_index = static_cast<row_index_t>(control->row_index);

        schema_t* control_schema = row_index_t_data_t_size_env_listplugin(control->row_index);

        color_term_rgbindex("#451111");

        printf("[aistdio] control pointer set with row index: %zu\n", control->row_index);

        schema_display_t* termindex_info = display_play;
        (void)termindex_info;
    }

    printf("[aistdio] admin managing controlling set at ptr: %p\n", control_ptr);
}

#endif // AISTDIO_HPP