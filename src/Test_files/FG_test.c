#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

int main() {
    //Making test data
    FILE *fptr;

    fptr = fopen("Tgrep1.txt", "w");
    fprintf(fptr, "kylling, okse, svin\n");
    fprintf(fptr, "vil gerne have mad\n");
    fprintf(fptr, "mad kunne være godt\n");
    fclose(fptr);
    
    fptr = fopen("Tgrep2.txt", "w");
    fprintf(fptr, "kylling\n");
    fprintf(fptr, "der kun kylling\n");
    fprintf(fptr, "kylling er også godt\n");
    fclose(fptr);

    fptr = fopen("Tgrep3.txt", "w");
    fprintf(fptr, "der ikke mere mad\n");
    fprintf(fptr, "hvordan er alt væk\n");
    fprintf(fptr, "tid til at sulte\n");
    fclose(fptr);

    if (system("./fauxgrep-mt mad Tgrep1.txt") == 0) {
        printf("fauxgrep Single file search passed\n");
    } 
    else {
        printf("fauxgrep Single file search failed\n");
    }

    if (system("./fauxgrep-mt kylling Tgrep1.txt Tgrep2.txt Tgrep3.txt") == 0) {
        printf("fauxgrep Multi file search passed\n");
    } 
    else {
        printf("fauxgrep Multi file search failed\n");
    }

     if (system("./fauxgrep-mt -n 2 kylling Tgrep1.txt Tgrep2.txt Tgrep3.txt") == 0) {
        printf("fauxgrep Multithread search passed\n");
    } 
    else {
        printf("fauxgrep Multithread file search failed\n");
    }

    if (system("./fauxgrep-mt kebab Tgrep1.txt Tgrep2.txt Tgrep3.txt") == 0) {
        printf("fauxgrep No match search passed\n");
    } 
    else {
        printf("fauxgrep No match search failed\n");
    }
    return 0;
}