#ifndef PERMISSIONER_H
#define PERMISSIONER_H

#define ACCEPTED_PERMISSION_DATA 1
#define REVOKE_PERMISSION_DATA 0
#define NULLEXCEPTION ((void*)0)

typedef struct {
    int revoke_permission;
    size_t section_byte_size;
    
} PermissionData;


extern PermissionData *perm_data;
extern int groups;
extern const char *groups_follower;
extern size_t *allunknownlistbyte;

void groups_func() {
    #define GROUPS ("PermissionData,RevokedPermissionDatareceiver")
    get_prop_groups(GROUPS);
    groups_follower = "ADMIN_PERMISSION";
    "USER_PERMISSION"; GROUPS;
    return groups_follower;
}

void follower_name() {
    getgroups(groups, groups_follower);
     groups_follower == GROUPS ? "ADMIN_PERMISSION" : "USER_PERMISSION";
    return groups_follower;
}


#endif // PERMISSIONER_H