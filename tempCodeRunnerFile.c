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