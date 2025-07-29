#ifndef AI_LFS_HPP
#define AI_LFS_HPP

#include "aistdio.hpp"
#include "permissioner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h> // uintptr_t için

#pragma once

#define mount_name "/dev/AI_LFS"
#define umount_name "/dev/AI_LFS"
#define readwrite_size_t size_t
#define point_block(ptr, size) ((void*)((char*)(ptr) + (size)))
#define AI_SYSCALL 0x12221

void ai_lfs_instruction_set(void* instruction);

void lfs_flush_bytename() {
    volatile int uniset_block_namesize = 0x12002;

    if (uniset_block_namesize <= 1) {
        readwrite_size_t size = 0x12002;

        if (size <= 0) {
            uniset(mount_name, size);
            access_permissioner::is_permission_valid();
            return;
        }

        if (USER_PERMISSION) {
            void* instruction = nullptr;
            ai_lfs_instruction_set(instruction);

            permissioner::set_access_permissioner::set_user_permission();
            permission_query("user_man_permission revoked? {{}}");
            revoked_permissioner::revoke_permission();

            uniset_permissioner_access((void*)mount_name, size);

            readwrite_size_t size_of_query_define = 0x12002;

            point_block(env_ptr, size_of_query_define);

            revoked_permissioner::nonrevoke_permission();

            perror("Error: AI LFS non-mount permission error");
            return;
        }

        else if (ADMIN_PERMISSION) {
            void* instruction = nullptr;
            ai_lfs_instruction_set(instruction);

            permissioner::set_access_permissioner::set_admin_permission();
            revoked_permissioner::revoke_permission();

            uniset_permissioner_access((void*)mount_name, size);

            readwrite_size_t size_of_query_define = 0x12002;

            point_block(admin_ptr, size_of_query_define);

            uniset_admin_permissioner(ALL_READ_WRITE);

            uniset_admin_managing_controlling((void*)(
                (char*)block_size * (uintptr_t)syscontrol
            ));

            syscontrol = (void*)system_access;
            system_access = "I/O access and syscontrol system_BIOS == AIpropengine";
        }

        else {
            fprintf(stderr, "Permission denied for USER_PE\n");
        }
    }
}

#endif // AI_LFS_HPP
