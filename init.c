#include <unistd.h>
#define USERNAME_LENGTH                                                        \
    257 // max size of username in linux + 1 for NULL character

void init_username() {
    // Do not hardcode the prmopt
    char username[USERNAME_LENGTH];
    getlogin_r(username, USERNAME_LENGTH);
}
