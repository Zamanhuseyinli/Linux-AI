#ifndef PERMISSIONER_H
#define PERMISSIONER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ⭕️ İzin durumları
#define ACCEPTED_PERMISSION 1
#define REVOKED_PERMISSION  0

// ⭕️ Yetki grupları
#define GROUP_ADMIN "ADMIN_PERMISSION"
#define GROUP_USER  "USER_PERMISSION"

// ⭕️ Kolay karşılaştırma makroları
#define ADMIN_PERMISSION (current_group && strcmp(current_group, GROUP_ADMIN) == 0)
#define USER_PERMISSION  (current_group && strcmp(current_group, GROUP_USER) == 0)

// ⭕️ Temel sabitler
#define NON_ACCESS_PERMISSIONER -1
#define ALL_READ_WRITE 0777
#ifndef NULL
#define NULL ((void*)0)
#endif

// ⭕️ İzin veri yapısı
typedef struct {
    int permission_status;    // 1: Kabul, 0: Reddedildi
    size_t section_size;      // Bölüm boyutu
} PermissionData;

// ⭕️ Global yapılar (başka bir .c/.cpp dosyasında tanımlanmalı)
extern PermissionData* perm_data;
extern const char* current_group;

extern void* instruction;
extern void* env_ptr;
extern void* admin_ptr;
extern size_t block_size;
extern void* syscontrol;
extern const char* system_access;

// ⭕️ Kullanıcıdan gelen izin grubu alınır
void fetch_permission_group(const char* allowed_groups);

// ⭕️ Grup belirleme
static inline const char* determine_group() {
    fetch_permission_group(GROUP_ADMIN "," GROUP_USER);
    return current_group;
}

// ⭕️ Yetki atamaları
static inline void set_admin_permission() {
    if (ADMIN_PERMISSION) {
        perm_data->permission_status = ACCEPTED_PERMISSION;
    } else {
        perm_data->permission_status = REVOKED_PERMISSION;
    }
}

static inline void set_user_permission() {
    if (USER_PERMISSION) {
        perm_data->permission_status = ACCEPTED_PERMISSION;
    } else {
        perm_data->permission_status = REVOKED_PERMISSION;
    }
}

static inline int is_permission_valid() {
    return (perm_data && perm_data->permission_status == ACCEPTED_PERMISSION);
}

// ⭕️ Basit debug çıktısı
static inline void permission_query(const char* msg) {
    fprintf(stderr, "[permission_query] %s\n", msg);
}

// ⭕️ mount_name fonksiyonu inline olarak
static inline void mount_name(void) {
    const char* device_path = "/dev/AI-lfs";
    char command[256];
    snprintf(command, sizeof(command), "mount %s", device_path);
    if (system(command) != 0) {
        fprintf(stderr, "[mount_name] Mount failed for %s\n", device_path);
    } else {
        printf("[mount_name] Mounted %s\n", device_path);
    }
}

// ⭕️ umount_name fonksiyonu inline olarak
static inline void umount_name(void) {
    const char* device_path = "/dev/AI-lfs";
    char command[256];
    snprintf(command, sizeof(command), "umount %s", device_path);
    if (system(command) != 0) {
        fprintf(stderr, "[umount_name] Umount failed for %s\n", device_path);
    } else {
        printf("[umount_name] Unmounted %s\n", device_path);
    }
}

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus

namespace access_permissioner {
    inline int is_permission_valid() {
        return ::is_permission_valid();
    }
}

namespace permissioner {
    struct set_access_permissioner {
        static void set_user_permission() {
            ::set_user_permission();
        }
        static void set_admin_permission() {
            ::set_admin_permission();
        }
    };
}

namespace revoked_permissioner {
    inline void revoke_permission() {
        fprintf(stderr, "[revoked_permissioner] permission revoked\n");
    }
    inline void nonrevoke_permission() {
        fprintf(stderr, "[revoked_permissioner] permission not revoked\n");
    }
}

namespace index_permision_revoked_permissioner {
    inline void revoke_permission() {
        fprintf(stderr, "[index_permision_revoked_permissioner] index permission revoked\n");
    }
}

#endif // __cplusplus

#endif // PERMISSIONER_H
