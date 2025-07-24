#ifndef PERMISSIONER_H
#define PERMISSIONER_H

#define ACCEPTED_PERMISSION_DATA 1
#define REVOKE_PERMISSION_DATA 0
#define NULLEXCEPTION ((void*)0)

typedef struct {
    int revoke_permission;
    size_t section_byte_size;
    // Diğer alanlar eklenebilir
} PermissionData;

// Dışarıdan erişilecek global değişken
extern PermissionData *perm_data;

// allunkwonlistbyte örnek tanımı
extern size_t *allunkwonlistbyte;

#endif //