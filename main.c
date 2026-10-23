#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <unistd.h>

#define MAX_MOVIE 100
#define ROWS 5
#define COLS 4

typedef struct Movie{
    char movieID[100];
    char title[100];
    char time[30];
    int availableSeat;
    float price;
    char update[100];
    int available;
    char targetID[100];
} Movie;

typedef struct ticketSale {
    char movieTitle[50];
    char time[30];
    int  seat[100];
    char ticketReceipts[100];
} Ticket;


Movie library[MAX_MOVIE];
int movieCount = 0;


void clearInputBuffer() { 
    int c;
    while ((c = getchar()) != '\n' && c != EOF);   
}

void getMoviesfile() {
    FILE* getMoviefile = fopen("Add-movie.txt", "r");   //
    if (getMoviefile == NULL) {
        printf("No movie file found.\n");
        return;
    }
    movieCount = 0;
    Movie movie;
    while (
        fscanf(getMoviefile, "\nMovie Title : %[^\n]\n", movie.title) == 1 &&
        fscanf(getMoviefile, "Movie ID : %s\n", movie.movieID) == 1 &&
        fscanf(getMoviefile, "Time : %[^\n]\n", movie.time) == 1 &&
        fscanf(getMoviefile, "Total Seatings : %d\n", &movie.availableSeat) == 1 &&
        fscanf(getMoviefile, "Price : $%f\n", &movie.price) == 1 &&
        fscanf(getMoviefile, "--------------------------------------\n") == 0 // consume separator
    ) {
        movie.available = 1;
        library[movieCount++] = movie;
    }
    fclose(getMoviefile);
}

void displaySeating(char seats[ROWS][COLS]) {
    printf("\n     ");
    for (int col = 0; col < COLS; col++) {
        printf(" %d ", col + 1);
    }
    printf("\n");

    for (int row = 0; row < ROWS; row++) {
        printf(" %c  ", 'A' + row);
        for (int col = 0; col < COLS; col++) {
            printf("[%c]", seats[row][col] == 'O' ? ' ' : 'X');
        }
        printf("\n");
    }

    printf("\n([ ] = Available, [X] = Booked)\n");
}

void loadSeats(const char *movieID, char seats[ROWS][COLS]) {
    char filename[50];
    sprintf(filename, "%s_seat.txt", movieID);
    FILE *file = fopen(filename, "r");

    if (!file) {
        // If file doesn't exist, initialize all to 'O'
        for (int i = 0; i < ROWS; i++)
            for (int j = 0; j < COLS; j++)
                seats[i][j] = 'O';
        return;
    }

    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            fscanf(file, " %c", &seats[i][j]);

    fclose(file);
}

void saveSeats(const char *movieID, char seats[ROWS][COLS]) {
    char filename[50];
    sprintf(filename, "%s_seat.txt", movieID);
    FILE *file = fopen(filename, "w");

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++)
            fprintf(file, "%c ", seats[i][j]);
        fprintf(file, "\n");
    }

    fclose(file);
}

void welcome() {
    system("cls");
    printf("===============================================================\n");
    printf("||                                                           ||\n");
    printf("||               WELCOME TO DIAMOON THEATER SYSTEM           ||\n");
    printf("||                                                           ||\n");
    printf("||      Manage Movies   |   Buy Tickets   |   View Sales     ||\n");
    printf("||                                                           ||\n");
    printf("===============================================================\n\n");
}

int generateReceiptNumber() {
    FILE *file = fopen("receipt_counter.txt", "r+");
    int number = 1;
    if (file) {
        fscanf(file, "%d", &number);
        fseek(file, 0, SEEK_SET);
    } else {
        file = fopen("receipt_counter.txt", "w");
    }
    fprintf(file, "%d", number + 1);
    fclose(file);
    return number;
}

void menu(){
    welcome();
    printf("\n------------------------\n");
    printf("\n*** Choose A Feature ***\n");
    printf("1. Add Movie\n");
    printf("2. View Movie Info\n");
    printf("3. Update Movie\n");
    printf("4. Delete Movie\n");
    printf("5. Buy Movie Ticket\n");
    printf("6. Ticket Sale Record\n");
    printf("7. Exit\n");
}

void addMovie() {
    Movie newMovie;
    char buf[1024];
    int checkID = 1;

    FILE *input_new_movie = fopen("Add-movie.txt", "r");printf("\n-----Adding Movie-----\n");
    printf("\nEnter the title of the Movie: ");
    fgets(newMovie.title, 100, stdin);
    newMovie.title[strlen(newMovie.title) - 1] = '\0';

    printf("\nEnter Movie ID: ");
    scanf("%s", newMovie.movieID);
    clearInputBuffer();



    if (input_new_movie != NULL) {
        while (fgets(buf, sizeof(buf), input_new_movie)) {
            if (strncmp(buf, "Movie ID :", 10) == 0) {
                char existingID[100];
                sscanf(buf, "Movie ID : %s", existingID);
                if (strcmp(existingID, newMovie.movieID) == 0) {
                    checkID = 0;
                    break;
                }
            }
        }
        fclose(input_new_movie);    // you open the input new movie but you never close it.
    }

    if (checkID == 0) {
        printf("\nMovie already exists.\n");
        return;
    } else {
        printf("\nEnter time: ");
        fgets(newMovie.time, 100, stdin);
        newMovie.time[strcspn(newMovie.time, "\n")] = '\0';

        printf("\nEnter available seat: ");
        scanf("%d", &newMovie.availableSeat);
        clearInputBuffer();

// Limit available seat based on ROWS and COLS
        if (newMovie.availableSeat > ROWS * COLS) {
        printf("Maximum seats allowed is %d. Setting available seats to %d.\n", ROWS * COLS, ROWS * COLS);
        newMovie.availableSeat = ROWS * COLS;
        }

        if (newMovie.availableSeat <= 0) {
        printf("Available seats must be at least 1. Setting to default value 1.\n");
        newMovie.availableSeat = 1;
        }

        printf("\nEnter price: ");
        scanf("%f", &newMovie.price);
        newMovie.available = 1;
        library[movieCount++] = newMovie;

        printf("\n----Movie has been added to the library----\n");

        FILE *add_movie_info = fopen("Add-movie.txt", "a");
        if (add_movie_info == NULL) {
            printf("Error opening file for appending.\n");
            return;
        }

        fprintf(add_movie_info, "\nMovie Title : %s", newMovie.title);
        fprintf(add_movie_info, "\nMovie ID : %s", newMovie.movieID);
        fprintf(add_movie_info, "\nTime : %s", newMovie.time);
        fprintf(add_movie_info, "\nTotal Seatings : %d", newMovie.availableSeat);
        fprintf(add_movie_info, "\nPrice : $%.2f", newMovie.price);
        fprintf(add_movie_info, "\n--------------------------------------\n");

        fclose(add_movie_info);
    }
}

void displayMovie() {
    int found = 0;
    Movie movie;
    int choose;

    while (1) {
        printf("1. Search movie by ID\n");
        printf("2. View all movies\n");
        printf("3. Exit\n");
        printf("\nChoose: ");
        scanf("%d", &choose);
        clearInputBuffer();
        system("cls");

        switch (choose) {
            case 1:
                printf("Enter the movie ID to view: ");
                scanf("%s", movie.targetID);
                clearInputBuffer();

                found = 0;
                for (int i = 0; i < movieCount; i++) {
                    if (strcmp(library[i].movieID, movie.targetID) == 0 && library[i].available) {
                        printf("\nMovie Title : %s", library[i].title);
                        printf("\nMovie ID : %s", library[i].movieID);
                        printf("\nTime : %s", library[i].time);
                        printf("\nTotal Seatings : %d", library[i].availableSeat);
                        printf("\nPrice : $%.2f", library[i].price);
                        printf("\n--------------------------------------\n");
                        found = 1; 
                        break;
                    }
                }
                if (!found) {
                    printf("Movie ID not found.\n");
                }
                break;
                case 2:
                if (movieCount == 0) {
                    printf("No movies found in the library.\n");
                } else {
                    printf("---- Listed All Movies ----\n");
                    for (int i = 0; i < movieCount; i++) {
                        if (library[i].available) {
                            printf("\nMovie Title : %s", library[i].title);
                            printf("\nMovie ID : %s", library[i].movieID);
                            printf("\nTime : %s", library[i].time);
                            printf("\nTotal Seatings : %d", library[i].availableSeat);
                            printf("\nPrice : $%.2f", library[i].price);
                            printf("\n--------------------------------------\n");
                        }
                    }
                }
                break;

            case 3:
                printf("Returning to main menu...\n");
                return;

            default:
                printf("Invalid option!\n");
                break;
        }

        printf("\n");
    }
}

void updateMovie() {
    Movie movie;

    printf("Enter the Movie ID to update: ");
    scanf("%s", movie.targetID);
    clearInputBuffer();

    int found = 0;

    for (int i = 0; i < movieCount; i++) {
        if (strcmp(library[i].movieID, movie.targetID) == 0 && library[i].available) {
            found = 1;
            char input[100];

            printf("Which part do you want to change? (title, time, seat, or price): ");
            scanf("%s", input);
            clearInputBuffer();  // clear after reading string

            if (strcmp(input, "title") == 0) {
                printf("Enter new title: ");
                fgets(library[i].title, sizeof(library[i].title), stdin);
                library[i].title[strcspn(library[i].title, "\n")] = '\0'; // remove newline
            }
            else if (strcmp(input, "time") == 0) {
                printf("Enter new time: ");
                fgets(library[i].time, sizeof(library[i].time), stdin);
                library[i].time[strcspn(library[i].time, "\n")] = '\0';
            }
            else if (strcmp(input, "seat") == 0) {
                printf("Enter new available seat: ");
                scanf("%d", &library[i].availableSeat);
                clearInputBuffer();
            }
            else if (strcmp(input, "price") == 0) {
                printf("Enter new price: ");
                scanf("%f", &library[i].price);
                clearInputBuffer();
            } else {
                printf("Invalid choice.\n");
                return;  // or consider letting user retry
            }

            printf("\n----Movie has been updated!----\n");

            FILE* update = fopen("temp.txt", "w");
            if (update == NULL) {
                printf("Error opening file for updating.\n");
                return;
            }

            // Rewrite all movies except unavailable ones
            for (int j = 0; j < movieCount; j++) {
                if (library[j].available) {
                    fprintf(update, "\nMovie Title : %s", library[j].title);
                    fprintf(update, "\nMovie ID : %s", library[j].movieID);
                    fprintf(update, "\nTime : %s", library[j].time);
                    fprintf(update, "\nTotal Seatings : %d", library[j].availableSeat);
                    fprintf(update, "\nPrice : $%.2f", library[j].price);
                    fprintf(update, "\n--------------------------------------\n");
                }
            }

            fclose(update);
            remove("Add-movie.txt");
            rename("temp.txt", "Add-movie.txt");
            break;
        }
    }

    if (!found) {
        printf("Movie ID not found\n");
    }
}

void deleteMovie() {
    Movie movie;
    // Don't call getMoviesfile() here; load once at program start

    printf("Enter the Movie ID to delete: ");
    scanf("%s", movie.targetID);
    clearInputBuffer();

    int found = 0;FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error opening file for deleting.\n");
        return;
    }

    for (int i = 0; i < movieCount; i++) {
        if (strcmp(library[i].movieID, movie.targetID) == 0 && library[i].available) {
            found = 1;
            printf("\n----Movie has been deleted!----\n");
            // Mark the movie as unavailable in memory
            library[i].available = 0;
            continue;  // skip writing this movie to the file
        }

        if (library[i].available) {
            fprintf(tempFile, "\nMovie Title : %s", library[i].title);
            fprintf(tempFile, "\nMovie ID : %s", library[i].movieID);
            fprintf(tempFile, "\nTime : %s", library[i].time);
            fprintf(tempFile, "\nTotal Seatings : %d", library[i].availableSeat);
            fprintf(tempFile, "\nPrice : $%.2f", library[i].price);
            fprintf(tempFile, "\n--------------------------------------\n");
        }
    }

    fclose(tempFile);

    if (found) {
        remove("Add-movie.txt");
        rename("temp.txt", "Add-movie.txt");
    } else {
        printf("Movie ID not found.\n");
        remove("temp.txt"); // remove temp file since nothing changed
    }
}

void buyTicket() {
    char inputID[100];
    int found = 0;
       
    printf("Enter Movie ID to buy ticket: ");
    scanf("%s", inputID);
    clearInputBuffer();

    int index = -1;
    for (int i = 0; i < movieCount; i++) {
        if (strcmp(inputID, library[i].movieID) == 0) {
            index = i;
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Movie ID not found.\n");
        return;
    }

    if (!library[index].available) {
        printf("This movie is no longer available.\n");
        return;
    }

    if (library[index].availableSeat <= 0) {
        printf("No seats available for this movie.\n");
        return;
    }

    int numTickets;
    printf("Enter number of tickets to buy: ");
    scanf("%d", &numTickets);
    clearInputBuffer();

    if (numTickets <= 0) {
        printf("Invalid number of tickets.\n");
        return;
    }

    if (numTickets > library[index].availableSeat) {
        printf("Not enough seats. Only %d available.\n", library[index].availableSeat);
        return;
    }

    // Load and show seats
    char seats[ROWS][COLS];
    loadSeats(library[index].movieID, seats);
    displaySeating(seats);

    int chosenSeats[numTickets][2];
    for (int t = 0; t < numTickets; t++) {
        char seatCode[4];  // e.g., A1, B3
        int row, col;

        while (1) {
            printf("Enter seat (e.g., A1): ");
            fgets(seatCode, sizeof(seatCode), stdin);
            seatCode[strcspn(seatCode, "\n")] = '\0';  // remove newline

            if (strlen(seatCode) < 2) {
                printf("Invalid format. Try again.\n");
                continue;
            }

            row = seatCode[0] - 'A';      // Row as letter A-E
            col = atoi(&seatCode[1]) - 1; // Column as number 1–4

            if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
                if (seats[row][col] == 'O') {
                    seats[row][col] = 'X';
                    chosenSeats[t][0] = row;
                    chosenSeats[t][1] = col;
                    printf("Seat %s booked.\n", seatCode);
                    break;
                } else {
                    printf("Seat %s already booked. Try again.\n", seatCode);
                }
            } else {
                printf("Invalid seat code. Try again.\n");
            }
        }
    }

    // Update and save seat file
    saveSeats(library[index].movieID, seats);
    library[index].availableSeat -= numTickets;

    // Receipt generation
    // Receipt generation
for (int i = 0; i < numTickets; i++) {
    int receiptNum = generateReceiptNumber();
    char rowLetter = 'A' + chosenSeats[i][0];  // Row as letter
    int seatNumber = chosenSeats[i][1] + 1;   // Seat number

    char trimmedTitle[33]; // 32 max + '\0'
    strncpy(trimmedTitle, library[index].title, 32);
    trimmedTitle[32] = '\0';

    printf("\n\n\n\n");
    printf("       *****************************************************-*******************************\n");
    printf("      )      RECEIPT NO: %06d          TIME: %-2s                     %-5s\n",
           receiptNum, library[index].time, trimmedTitle);
    printf("     |                                                      |                                 |\n");
    printf("      )                    DIAMOON THEATER                  |                                (\n");
    printf("     |                       MOVIE TICKET                   |            ID: %-10s       |\n",
           library[index].movieID);
    printf("      )                                                     |                                (\n");
    printf("     |                                                      |                                 |\n");
    printf("      )     ----------------------------------------------- |                                (\n");
    printf("     |        ROW                           SEAT            |                                 |\n");
    printf("      )        %-2c                            %-2d                           $%.2f             \n",
           rowLetter, seatNumber, library[index].price);
    printf("       *****************************************************-*******************************\n");
    printf("\n\n\n\n\n");
}
}







void viewTicketSale() {
    printf("\nTicket Sale Summary:\n");
    printf("-------------------------------------------------\n");
    printf("%-12s %-25s %-5s\n", "Movie ID", "Title", "Sold");
    printf("-------------------------------------------------\n");

    for (int i = 0; i < movieCount; i++) {
        // Skip unavailable movies if needed
        // if (!library[i].available) continue;

        char filename[128];
        snprintf(filename, sizeof(filename), "%s_seat.txt", library[i].movieID);

        FILE *file = fopen(filename, "r");
        if (!file) {
            // No seat file = no sales
            printf("%-12s %-25s %-5d\n", library[i].movieID, library[i].title, 0);
            continue;
        }

        int sold = 0;
        char ch;
        while ((ch = fgetc(file)) != EOF) {
            if (ch == 'X') sold++;
        }
        fclose(file);

        printf("%-12s %-25s %-5d\n", library[i].movieID, library[i].title, sold);
    }

    printf("-------------------------------------------------\n\n");
}

 
int main() {
    getMoviesfile();  
    int choice;

    do {
        system("cls");

        menu();
        printf("\nChoose a feature: ");
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("\nInvalid input! Please enter a number.\n");
            printf("\nPress any key to continue...");
            getch();
            continue;
        }
        clearInputBuffer();
        system("cls");

        switch (choice) {
            case 1:
                addMovie();
                getMoviesfile();
                break;
            case 2:
                displayMovie();
                break;
            case 3:
                updateMovie();
                getMoviesfile();
                break;
            case 4:
                deleteMovie();
                getMoviesfile();
                break;
            case 5:
                buyTicket();
                getMoviesfile();  // to refresh data after ticket purchase
                break;
            case 6:
                viewTicketSale();
                break;
            case 7:
                printf("\n------Exit the program------\n");
                exit(0);  // normal exit
                break;
            default:
                printf("\nInvalid option!\n");
                printf("\nPress any key to continue...");
                getch();
                break;
        }

        if (choice != 7) {
            printf("\nPress any key to continue...");
            getch();
        }

    } while (choice != 7);

    return 0;
}