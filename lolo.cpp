#include <stdio.h>
#include <unistd.h>
#include <iostream>
int main() {
    const char *folder_path = "pk/lolo";
    int result = remove(folder_path);

    if (result == 0) {
        printf("Folder removed successfully\n");
    } else {
        printf("Failed to remove folder\n");
    }

    return 0;
}