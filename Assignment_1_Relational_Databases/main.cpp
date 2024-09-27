#include "RandomAccessFile.cpp"
#include "DelimitedFiles(CSV).cpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable: 4996) // required by Visual Studio

int main(void) {
    int choice;

    
    while (1) {
        // Display menu options
        printf("\nMenu:\n");
        printf("1. Delimited Files(CSV)\n");
        printf("2. Random Access Files\n");
        printf("3. Sequential Access File\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            menuDelimitedFiles();
            break;
        case 2:
            menuRandomAccess();
            break;
        case 3:

            break;
        case 4:
            printf("Exiting the program. Goodbye!\n");
            return 0;  // Exit the program
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
