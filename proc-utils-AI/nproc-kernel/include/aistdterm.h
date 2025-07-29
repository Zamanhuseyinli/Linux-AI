#ifndef AISTDTERM_H
#define AISTDTERM_H

#ifdef __cplusplus

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

#ifndef HAVE_U_LONG
typedef unsigned long u_long;
#define HAVE_U_LONG
#endif

#define makeing_and_compiling \
  "/usr/bin/gcc -Wall -fPIC -I./includes " \
  "-DUNEXCEPTED_DATA_SRR -URANDOM " \
  "-DSSIZEOFTYPEING=64 " \
  "-shared ai_unistd_unxcepted.cpp -o libaiunstd.0o1.so && " \
  "mv libaiunstd.0o1.so /usr/lib64/"

extern "C" {

// Düzeltildi: Fonksiyon bildirimi ve gövdesi ayrıldı, sözdizimi doğru hale getirildi
extern void term_index_interface(int termindex);

extern void size_screen_t(const char* display);
extern void terminfo_display_t(void* terminfo);
extern void schema_t(void* schema);
extern void list_t(void* list);
extern void buffer_size_t(size_t size);
extern void row_index_t(int index);
extern void ptrtermindex_t(int index);
extern void element_list_info(void* info);
extern void byname_t(void* name);
extern void const_element_plugin(void* control_ptr);
extern void schema_display_t(void* display);
extern void size_t(size_t size);
extern void row_t(void* row);
extern void list(void* list);
extern void color_term_rgbindex(const char* color);

// Zayıf sembol, deprecated fonksiyon tanımı
[[deprecated("nonunexcepted_exception_t is not implemented and should not be called")]]
__attribute__((weak))
int nonunexcepted_exception_t(const char* message) {
    (void)message;
    return 0; // ya da uygun hata kodu
}

// uniset fonksiyonu (örnek)
void CLANG_DEPRECATION() {
    const char* exception_msg = "nonunexcepted_exception_t placeholder";
    nonunexcepted_exception_t(exception_msg);
    clearerr(stdout);
    const char msg[] = "[aistdio] uniset executed\n";
    ssize_t ret = write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    if (ret == -1) {
        perror("write error");
        return;
    }
}

// __VARIABLES_COMPILER fonksiyonu
extern ssize_t runtime_sd(ssize_t* protocol_size);
extern bool error_size_access_recommanted();
extern void eaccess(const char* format, const char* type_str, size_t size);
extern bool enable_size_access_recommanted();
extern void gcc_unistd_recommanted();

extern void* terminfo;

int __VARIABLES_COMPILER(int compiler_define) {
    ssize_t protocol_size = 4096;

    compiler_define = runtime_sd(&protocol_size);

    u_long clang_usedvariables = 0;

    if (error_size_access_recommanted()) {
        eaccess("error_size : unistd variables gcc recommanted %s %zu\n", "u_long", (size_t)protocol_size);

        if (enable_size_access_recommanted()) {
            static pthread_once_t once_control = PTHREAD_ONCE_INIT;

            pthread_once(&once_control, []() {
                system(makeing_and_compiling);
                gcc_unistd_recommanted();
            });

            return (int)(intptr_t)terminfo;
        }
    }
    return 0;
}

extern void display_play(void* termindex_info);
extern void uniset(const char* device_name, size_t size);
extern void uniset_permissioner_access(void* device_name, size_t size);
extern void uniset_admin_permissioner(int permission_flags);
extern void uniset_admin_managing_controlling(void* control_ptr);
extern void row_index_t_data_t_size_env_listplugin(int row_index);
extern void color_term_rgbindex(const char* color);

void display_play(void* termindex_info) {
    size_screen_t("$DISPLAY");

    if (!termindex_info) {
        fprintf(stderr, "Error: termindex_info is null\n");
        return;
    }

    int termindex = *reinterpret_cast<int*>(termindex_info);

    // Kullanıcıya doğrudan /dev/pts/{index} erişimi verilir
    char tty_path[64];
    snprintf(tty_path, sizeof(tty_path), "/dev/pts/%d", termindex);

    // system_access tanımlı değil, global char* ise eklemelisin
    extern char* system_access;
    system_access = strdup(tty_path);

    uniset_permissioner_access((void*)tty_path, 0x12002);

    terminfo_display_t(termindex_info);
    term_index_interface(termindex);

    printf("[aistdio] Terminal access granted at: %s\n", tty_path);
}

} // extern "C"

#endif // __cplusplus

#endif // AISTDTERM_H
