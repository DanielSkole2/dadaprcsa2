#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

int main() {
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

    int test_status = 0; 

    printf("\n--- fauxgrep-mt Tests ---\n");
    
    printf("Test 1: Single file search (mad in Tgrep1.txt). Output follows:\n");
    fflush(stdout);
    int status1 = system("./fauxgrep-mt mad Tgrep1.txt");
    if (status1 == 0) {
        printf(" -> PASS: fauxgrep-mt Single file search passed (Status 0).\n");
    } else {
        printf(" -> FAIL: fauxgrep-mt Single file search failed (Exit Status: %d).\n", status1);
        test_status = 1;
    }
    fflush(stdout);

    printf("\nTest 2: Multi file search (kylling). Output follows:\n");
    fflush(stdout);
    int status2 = system("./fauxgrep-mt kylling Tgrep1.txt Tgrep2.txt Tgrep3.txt");
    if (status2 == 0) {
        printf(" -> PASS: fauxgrep-mt Multi file search passed (Status 0).\n");
    } else {
        printf(" -> FAIL: fauxgrep-mt Multi file search failed (Exit Status: %d).\n", status2);
        test_status = 1;
    }
    fflush(stdout);

    printf("\nTest 3: Multithread search (-n 2 kylling). Output follows:\n");
    fflush(stdout);
    int status3 = system("./fauxgrep-mt -n 2 kylling Tgrep1.txt Tgrep2.txt Tgrep3.txt");
    if (status3 == 0) {
        printf(" -> PASS: fauxgrep-mt Multithread search passed (Status 0).\n");
    } else {
        printf(" -> FAIL: fauxgrep-mt Multithread file search failed (Exit Status: %d).\n", status3);
        test_status = 1;
    }
    fflush(stdout);

    printf("\nTest 4: No match search (kebab). Output follows:\n");
    fflush(stdout);
    int status4 = system("./fauxgrep-mt kebab Tgrep1.txt Tgrep2.txt Tgrep3.txt");
    if (status4 != 0) {
        printf(" -> PASS: fauxgrep-mt No match search passed (Status != 0).\n");
    } else {
        printf(" -> FAIL: fauxgrep-mt No match search failed (Exit Status: 0).\n");
        test_status = 1;
    }
    fflush(stdout);
    
    printf("\n--- fauxgrep-mt Tests finished ---\n\n"); 
    
    return test_status;
}
