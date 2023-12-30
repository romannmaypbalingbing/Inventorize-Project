/*FINAL PROJECT IN COMPUTER PROGRAMMING 2 (CS103)
PROJECT TITLE >> INVENTORIZE: STOCK-LEVEL MANAGEMENT AND REPORTING SYSTEM

MEMBERS:
ROM-ANN MAY BALINGBING
AIILA MARIE BELLEN
BEA MARIE CERILLO
RHAIN GOMEZ

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 100
#define RESET "\x1B[0m"
#define BOLD "\x1B[1m"

typedef struct list {
    int code;
    char name[MAX];
    char type[MAX];
    int numstocks;
    int solditems;
    int remqty;
    char status[MAX];
    struct list* next;
} list;

list* head = NULL;
 
// Prototype declarations
void title();
void get_currenttime(char* filedate, char* datetime);
void menu(list** head, list* newItem, list* current, int items, FILE *ofp2, char *datetime);
void displaytable(list** head, list* current);
void add_product(list** head, list* newItem, list* current, int items);
void delete_product(list** head, list* current, int* choicePtr);
void selection(list **head, list *current, int *choicePtr);
void get_remqty(list **head, list *current);
void sorting(list **head, list *current, int *choicePtr);
void selection2(list **head, list *current, list *newItem, int *choicePtr, int items, FILE *ofp2, char *datetime);
void print_output(list** head, list *current, FILE* ofp2, char* datetime);
int save_data(list* head, list* current);

int main() {
    title();

    char filedate[MAX] = "\0";
    char datetime[MAX] = "\0";
    get_currenttime(filedate, datetime);

    FILE* ifp = fopen("list.txt", "rt");
    if (ifp == NULL) {
        printf("Failed to open input file.");
        return 1;
    }

    FILE *ofp2 = fopen(filedate, "wt");
    if (ofp2 == NULL) {
        printf("Failed to open output file %s\n", filedate);
        return 1;
    }

    int items = 0, i = 0;
    int choice, *choicePtr = &choice;

    // Insertion of data
    while (!feof(ifp)) {
        list* newItem = malloc(sizeof(list));
        fscanf(ifp, "%s %s %d", newItem->name, newItem->type, &newItem->numstocks);
        newItem->next = head;
        head = newItem;
        items++;
    }

    printf("Date and Time of Inventory Report: %s\n\n", datetime);
    printf("PRODUCT CODE    PRODUCT NAME    PRODUCT TYPE  TOTAL PRODUCT\n");

    // Print initial data on the table
    list* current = head;
    list* newItem = NULL;
    int code = 1;
    while (current != NULL) {
        current->code = code;
        printf("    %03d \t %-10s \t %-10s   \t   %-10d\n", current->code, current->name, current->type, current->numstocks);
        current = current->next;
        code++;
    }
    printf("\n\n");

    menu(&head, newItem, current, items, ofp2, datetime);

    // Clean up memory
    current = head;
    while (current != NULL) {
        list* temp = current->next;
        free(current);
        current = temp;
    }
    save_data(head, current);
    fclose(ifp);
    fclose(ofp2);

    return 0;
}

void title() {
    for (int i = 0; i < 80; i++) // Fits the interface (used WSL on windows)
        printf("*");
    printf("\n");

    printf("\n\t  %sINVENTORIZE: Stock Level Management and Reporting Software%s\n\n", BOLD, RESET);

    for (int i = 0; i < 80; i++)
        printf("*");
    printf("\n");
}

//this function gets the current time to be used as output file and to be displayed on the terminal and output file
void get_currenttime(char* filedate, char* datetime) {
    time_t current_time = time(NULL);
    struct tm* tm_ptr = localtime(&current_time);
    strftime(filedate, MAX, "inv_report_%Y%m%d_%H%M%S.out", tm_ptr);
    strftime(datetime, MAX, "%B %d,%Y %I:%M:%S %p", tm_ptr);
}

//main selection of functions
void menu(list** head, list* newItem, list* current, int items, FILE *ofp2, char *datetime) {
    int choice;
    int* choicePtr = &choice;

    printf("OPTIONS:\n");
    printf("\t1 | ADD PRODUCT\n \t2 | REMOVE PRODUCT\n \t3 | UPDATE SALES\n \t4 | SORT\n \t5 | PRINT OUTPUT\n \t6 | EXIT\n\n");
    printf("Enter choice here: ");
    while (scanf(" %d", choicePtr) != EOF) {
        getchar();
        switch (*choicePtr) {
            case 1:
                add_product(head, newItem, current, items);
                printf("Add another product?\n");
                printf("1 | Yes\n2 | No\n");
                printf("Enter choice here: ");
                while (scanf(" %d", choicePtr) != 2) {
                    if (choice == 1)
                        add_product(head, newItem, current, items);
                    else if (choice == 2) {
                        // Print initial data on the table
                        printf("\nPRODUCT CODE    PRODUCT NAME    PRODUCT TYPE  TOTAL PRODUCT\n");
                        current = *head;
                        newItem = NULL;
                        int code = 1;
                        while (current != NULL) {
                            current->code = code;
                            printf("    %03d \t %-10s \t %-10s   \t   %-10d\n", current->code, current->name, current->type, current->numstocks);
                            current = current->next;
                            code++;
                        }
                        printf("\n");
                        menu(head, newItem, current, items, ofp2, datetime);
                    }
                    printf("Add another product?\n");
                    printf("1 | Yes\n2 | No\n");
                    printf("Enter choice here: ");
                }
                break;
            case 2:
                delete_product(head, current, choicePtr);
                while (getchar() != '\n');
                printf("Remove another product?\n");
                printf("1 | Yes\n2 | No\n");
                printf("Enter choice here: ");
                while (scanf(" %d", choicePtr) != 2) {
                printf("\n");
                getchar();
                    
                    if (choice == 1)
                        delete_product(head, current, choicePtr);
                    else if (choice == 2) {
                          menu(head, newItem, current, items, ofp2, datetime);
                    }
                    printf("Enter the code of the product to delete: ");
                }
            break;
            case 3: selection(head, current, choicePtr);
                    get_remqty(head, current);
                    displaytable(head, current);
                    printf("\n");
                    menu(head, newItem, current, items, ofp2, datetime);
            break;
            case 4: selection2(head, current, newItem, choicePtr, items, ofp2, datetime);
            break;
            case 5: printf("Data has been printed in the output file.");
                    print_output(head, current, ofp2, datetime);
            break;
            case 6:
                exit(0);
                break;
            default:
                printf("Please enter a valid option:\n");
                break;
        }
    }
}

void displaytable(list** head, list* current) {
    printf("PRODUCT CODE    PRODUCT NAME    PRODUCT TYPE  TOTAL PRODUCT    REMAINING QUANTITY          STATUS\n");
    current = *head;
    while (current != NULL) {
        printf(" %03d \t\t %-11s \t %-11s \t   %-14d \t%-17d %-s\n", current->code, current->name, current->type, current->numstocks, current->remqty, current->status);
        current = current->next;
    }
}

void add_product(list** head, list* newItem, list* current, int items) {
    newItem = malloc(sizeof(list));
    newItem->code = items + 1;
    items++;
    getchar();

    printf("\nEnter product name: ");
    fgets(newItem->name, sizeof(newItem->name), stdin);
    newItem->name[strcspn(newItem->name, "\n")] = '\0';

    printf("Enter product type: ");
    fgets(newItem->type, sizeof(newItem->type), stdin);
    newItem->type[strcspn(newItem->type, "\n")] = '\0';

    printf("Enter number of stocks: ");
    scanf("%d", &newItem->numstocks);
    getchar();
    printf("\n");
    newItem->solditems = 0;
    newItem->remqty = 0;
    strcpy(newItem->status, "");

    newItem->next = NULL; 

    
    if (*head == NULL) {
        *head = newItem;
    } 
    else {
        list* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newItem;
    }
}

void delete_product(list** head, list* current, int* choicePtr) {
    printf("Enter the code of the product to delete: ");
    scanf(" %d", choicePtr);
    getchar();
    
    if (*head == NULL) {
        printf("Product not found.\n");
        return;
    }

    list* previous = NULL;
    list *temp = *head;

    while (temp != NULL && temp->code != *choicePtr){
        previous = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Product not found.\n");
        return;
    }

    if (previous == NULL) {
      
        *head = temp->next;
    } else {
       
        previous->next = temp->next;
    }

    printf("Product deleted successfully.\n");

    printf("PRODUCT CODE    PRODUCT NAME    PRODUCT TYPE  TOTAL PRODUCT\n");
    current = *head;
    int code = 1;
    while (current != NULL) {
        printf("    %03d \t %-10s \t %-10s   \t   %-10d\n", current->code, current->name, current->type, current->numstocks);
        current = current->next;
    }
    free(temp);
    printf("Press Enter to continue...");
}

//this function continuously asks the user the product code and the corresponding number of sold items until EndofFile
void selection(list **head, list *current, int *choicePtr) {
    printf("Product Code Selection (Enter Ctrl-Z to exit)\n\n");
    printf("Select product code: ");
    
    while (scanf(" %d", choicePtr) != EOF) {
        getchar();
        
        // Check if the user entered a specific code or value to exit the loop
        if (*choicePtr == EOF) {
            break;
        }
        
        int found = 0;
        current = *head;
        
        while (current != NULL) {
            if (current->code == *choicePtr) {
                found = 1;
                break;
            }
            current = current->next;
        }

        if (found == 1) {
            printf("Enter number of items sold for %s: ", current->name);
            scanf("%d", &current->solditems);
            getchar();
            
            while (current->solditems > current->numstocks) {
                printf("Record cannot be made. Entered sales is more than the number of stocks.\n\n");
                printf("Enter number of items sold for %s: ", current->name);
                scanf("%d", &current->solditems);
                getchar();
            }
            
            printf("\n");
            printf("Select product code: ");
        } else {
            printf("Product not found.\n\n");
            printf("Select product code: ");
        }
    }
    
    printf("(Exited)\n\n");
}


void get_remqty(list **head, list *current){
    current = *head;
    while (current != NULL) {
        current->remqty = current->numstocks - current->solditems;

        if (current->remqty == current->numstocks)
            strcpy(current->status, "FULL");
        else if (current->remqty < (current->numstocks / 2))
            strcpy(current->status, "NEED_TO_RESTOCK");
        else
            strcpy(current->status, "SUFFICIENT");

        current = current->next;
    }
}

//this function asks the user if they prefer to sort the data according to category they prefer
void selection2(list **head, list *current, list *newItem, int *choicePtr, int items, FILE *ofp2, char *datetime){
    int i;
    printf("Would you like to sort the data?\n ");
    printf("1 | Yes\n 2 | No\n\n");

    printf("Enter input here: ");
    scanf(" %d", choicePtr);

    switch (*choicePtr) {
        case 1:
            printf("\nChoose category to sort data: \n1 | Product Name\n2 | Product Type\n3 | Total Products\n4 | Remaining Product Quantity\n5 | Status\n");
            printf("\nEnter input here: ");
            while (scanf("%1d", choicePtr) != EOF) {
                switch (*choicePtr) {
                    case 1:
                        printf("\nSorting by Product Name:\n");
                        sorting(head, current, choicePtr);
                        displaytable(head, current);
                        printf("\n\n");
                        printf("Select another category to sort data:\n (Ctrl-Z to Exit)\n Enter input here: ");
                        break;
                    case 2:
                        printf("\nSorting by Product Type:\n");
                        sorting(head, current, choicePtr);
                        displaytable(head, current);
                        printf("\n\n");
                        printf("Select another category to sort data:\n (Ctrl-Z to Exit)\n Enter input here: ");
                        break;
                    case 3:
                        printf("\nSorting by Total Products:\n");
                        sorting(head, current, choicePtr);
                        displaytable(head, current);
                        printf("\n\n");
                        printf("Select another category to sort data:\n (Ctrl-Z to Exit)\n Enter input here: ");
                        break;
                    case 4:
                        printf("\nSorting by Remaining Product Quantity:\n");
                        sorting(head, current, choicePtr);
                        displaytable(head, current);
                        printf("\n\n");
                        printf("Select another category to sort data:\n (Ctrl-Z to Exit)\n Enter input here: ");
                        break;
                    case 5:
                        printf("\nSorting by Status:\n");
                        sorting(head, current, choicePtr);
                        displaytable(head, current);
                        printf("\n\n");
                        printf("Select another category to sort data: (Ctrl-Z to Exit)\n Enter input here: ");
                        break;
                }
            }
            menu(head, newItem, current, items, ofp2, datetime);
            break;
        case 2:
            menu(head, newItem, current, items, ofp2, datetime); 
            break;
    }
}

void sorting(list **head, list *current, int *choicePtr) {
    list* sorted = NULL;
    list* temp = NULL;
    current = *head;

    while (current != NULL) {
        list* nextNode = current->next;

        if (sorted == NULL || (*choicePtr == 1 && strcmp(current->name, sorted->name) <= 0) ||
            (*choicePtr == 2 && strcmp(current->type, sorted->type) <= 0) ||
            (*choicePtr == 3 && current->numstocks <= sorted->numstocks) ||
            (*choicePtr == 4 && current->remqty <= sorted->remqty) ||
            (*choicePtr == 5 && strcmp(current->status, sorted->status) <= 0)) {
            
            current->next = sorted;
            sorted = current;
        } else {
            temp = sorted;

            while (temp->next != NULL &&
                ((*choicePtr == 1 && strcmp(current->name, temp->next->name) > 0) ||
                 (*choicePtr == 2 && strcmp(current->type, temp->next->type) > 0) ||
                 (*choicePtr == 3 && current->numstocks > temp->next->numstocks) ||
                 (*choicePtr == 4 && current->remqty > temp->next->remqty) ||
                 (*choicePtr == 5 && strcmp(current->status, temp->next->status) > 0))) {

                temp = temp->next;
            }

            current->next = temp->next;
            temp->next = current;
        }

        current = nextNode;
    }

    *head = sorted;
}

void print_output(list** head, list *current, FILE* ofp2, char* datetime) {
    fprintf(ofp2, "Date and Time of Inventory Report: %s\n\n", datetime);
    fprintf(ofp2, "PRODUCT CODE    PRODUCT NAME    PRODUCT TYPE  TOTAL PRODUCT  REMAINING QUANTITY  STATUS\n");

    current = *head;
    while (current != NULL) {
        fprintf(ofp2, "%-15d %-15s %-13s %-15d %-20d %s\n", current->code, current->name, current->type, current->numstocks, current->remqty, current->status);
        current = current->next;
    }

    fprintf(ofp2, "\n\n");
}

int save_data(list* head, list* current) {
    FILE *ofp1 = fopen("list.txt", "wt");
    if (ofp1 == NULL) {
        printf("Failed to open output file %s\n", "list.txt");
        return 1;
    }
     current = head;
    list* newItem = NULL;
    while (current != NULL) {
        printf("    %03d \t %-10s \t %-10s   \t   %-10d\n", current->code, current->name, current->type, current->numstocks);
        current = current->next;
    }
    printf("\n\n");
}
