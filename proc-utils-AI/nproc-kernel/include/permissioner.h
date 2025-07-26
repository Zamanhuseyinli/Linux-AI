#ifndef PERMISSIONER_H
#define PERMISSIONER_H

#include <stddef.h>
#include <string.h>

// İzin durumları
#define ACCEPTED_PERMISSION 1
#define REVOKED_PERMISSION  0

// Yetki grupları
#define GROUP_ADMIN "ADMIN_PERMISSION"
#define GROUP_USER  "USER_PERMISSION"

// NULL tanımı
#ifndef NULL
#define NULL ((void*)0)
#endif

typedef struct {
    int permission_status;    // 1: Kabul, 0: Reddedildi
    size_t section_size;      // İlgili izin bölümü boyutu
} PermissionData;

// Global değişkenler (tanımı başka bir .c dosyasında yapılacak)
extern PermissionData *perm_data;
extern const char *current_group;

// Sistemden izin gruplarını alacak fonksiyon prototipi (kullanıcı tanımlı)
void fetch_permission_group(const char *allowed_groups);

// Yetki grubunu belirle ve döndür
static inline const char* determine_group() {
    fetch_permission_group(GROUP_ADMIN "," GROUP_USER); // Sadece ADMIN ve USER
    return current_group;
}

// İzin kontrolü: Yalnızca ADMIN izin verilir
static inline void set_admin_permission() {
    if (current_group && strcmp(current_group, GROUP_ADMIN) == 0) {
        perm_data->permission_status = ACCEPTED_PERMISSION;
    } else {
        perm_data->permission_status = REVOKED_PERMISSION;
    }
}

// İzin kontrolü: Yalnızca USER izin verilir
static inline void set_user_permission() {
    if (current_group && strcmp(current_group, GROUP_USER) == 0) {
        perm_data->permission_status = ACCEPTED_PERMISSION;
    } else {
        perm_data->permission_status = REVOKED_PERMISSION;
    }
}

// İzin dışı işlemleri kesinlikle engelle
static inline int is_permission_valid() {
    return (perm_data->permission_status == ACCEPTED_PERMISSION);
}

#endif // PERMISSIONER_H
