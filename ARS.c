#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FLIGHT_FILE "flights.csv"  // File for saving flights
#define DESKTOP_PATH "details.csv" // Path for bookings CSV

// Booking structure
typedef struct Booking {
    char refNo[10];
    char name[30];
    char flightID[10];
    char date[15];
    int seatNumber;  // Added seat number
    float payment;
    int cancelRequested; 
    struct Booking *next;
} Booking;


// Flight structure
typedef struct Flight {
    char flightID[10];
    char date[15];
    char time[10];
    char destination[30];
    char source[30];
    float price;
    struct Flight *next;
} Flight;

typedef struct CancelRequest {
    char refNo[10];
    char name[30];
    char flightID[10];
    char date[15];
    float payment;
    struct CancelRequest *next;
} CancelRequest;

CancelRequest *headCancelRequests = NULL;

// Function prototypes for cancellation requests
void saveCancelRequestToFile(CancelRequest *request);
void loadCancelRequestsFromFile();
void removeCancelRequestFromFile(char *refNo);
void approveCancellationFromRequest(char *refNo);

Booking *head = NULL;
Flight *flightHead = NULL;

// Function prototypes
void trimWhitespace(char *str);
void createCSVIfNotExists();
void removeCancellationRequest(char *refNo) ;
void createCancellationFileIfNotExists();
void saveBookingToFile(Booking *booking);
void updateCSV();
char *generateRefNo();
void viewAvailableFlights();
void bookFlight();
void viewTicket();
void cancelBooking();
void adminMenu();
void viewCancelRequests();
void approveCancelRequest();
void addFlight();
void removeFlight();
void viewTotalPayments();
int verifyPayment(float enteredPayment, float flightPrice);
void adminAuthentication();
void loadFlightsFromFile();
void saveFlightsToFile();


void saveCancelRequestToFile(CancelRequest *request) {
    FILE *file = fopen("cancellation_requests.csv", "a");
    if (!file) {
        printf("Error opening cancellation requests file.\n");
        return;
    }
    fprintf(file, "%s,%s,%s,%s,%.2f\n", request->refNo, request->name, 
            request->flightID, request->date, request->payment);
    fclose(file);
}

void loadCancelRequestsFromFile() {
    FILE *file = fopen("cancellation_requests.csv", "r");
    if (!file) {
        printf("Error opening cancellation requests file.\n");
        return;
    }

    CancelRequest *tail = NULL;
    while (1) {
        CancelRequest *newRequest = (CancelRequest *)malloc(sizeof(CancelRequest));
        if (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%f\n",
                   newRequest->refNo, newRequest->name, newRequest->flightID, 
                   newRequest->date, &newRequest->payment) != 5) {
            free(newRequest);
            break;
        }

        newRequest->next = NULL;
        if (tail) {
            tail->next = newRequest;
        } else {
            headCancelRequests = newRequest;
        }
        tail = newRequest;
    }

    fclose(file);
}

void removeCancelRequestFromFile(char *refNo) {
    FILE *file = fopen("cancellation_requests.csv", "r+");
    if (!file) {
        printf("Error opening cancellation requests file.\n");
        return;
    }

    char tempFilename[] = "temp.csv";
    FILE *tempFile = fopen(tempFilename, "w");
    if (!tempFile) {
        printf("Error creating temporary file.\n");
        fclose(file);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        if (token && strcmp(token, refNo) != 0) {
            fputs(line, tempFile);
        }
    }

    fclose(file);
    fclose(tempFile);
    remove("cancellation_requests.csv");
    rename(tempFilename, "cancellation_requests.csv");
}

void approveCancellationFromRequest(char *refNo) {
    // 1. Find and remove the booking from the details list
    Booking *current = head, *prev = NULL;
    while (current) {
        if (strcmp(current->refNo, refNo) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                head = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }

    // 2. Remove the request from cancellation_requests.csv
    removeCancelRequestFromFile(refNo);

    // 3. Update the details CSV after removal
    updateCSV();

    printf("Cancellation approved and booking removed successfully.\n");
}



// Save the current booking list to the CSV file
void saveDataToCSV() {
    FILE *file = fopen("details.csv", "w");
    if (!file) {
        printf("Error opening file for saving data.\n");
        return;
    }

    Booking *current = head;
    while (current) {
        fprintf(file, "%s,%s,%s,%s,%.2f,%d\n", current->refNo, current->name,
                current->flightID, current->date, current->payment, current->cancelRequested);
        current = current->next;
    }
    
    fclose(file);
    printf("Data saved successfully to details.csv\n");
}

// Helper to find a flight by ID
Flight *findFlight(char *flightID) {
    Flight *current = flightHead;
    while (current) {
        if (strcmp(current->flightID, flightID) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Generate a random reference number for bookings
char *generateRefNo() {
    static char refNo[10];
    srand(time(NULL));
    sprintf(refNo, "R%04d", rand() % 10000);
    return refNo;
}

// Create the CSV file if it doesn't exist
void createCSVIfNotExists() {
    FILE *file = fopen(DESKTOP_PATH, "a");
    if (!file) {
        printf("Error creating or accessing the file.\n");
        exit(1);
    }
    fclose(file);
}

// Save booking data to the CSV file
void saveBookingToFile(Booking *booking) {
    FILE *file = fopen(DESKTOP_PATH, "a");
    if (!file) {
        printf("Error opening the file.\n");
        exit(1);
    }
    fprintf(file, "%s,%s,%s,%s,%d,%.2f,%d\n", booking->refNo, booking->name,
            booking->flightID, booking->date, booking->seatNumber,
            booking->payment, booking->cancelRequested);
    fclose(file);
}


// Update the CSV file with the current booking list
void updateCSV() {
    FILE *file = fopen(DESKTOP_PATH, "w");
    if (!file) {
        printf("Error opening the file.\n");
        exit(1);
    }
    Booking *current = head;
    while (current) {
        fprintf(file, "%s,%s,%s,%s,%.2f,%d\n", current->refNo, current->name,
                current->flightID, current->date, current->payment,
                current->cancelRequested);
        current = current->next;
    }
    fclose(file);
}

// Load the booking data from the CSV file into the linked list
void loadDataFromCSV() {
    FILE *file = fopen("details.csv", "r");
    if (!file) {
        printf("No existing data found. Starting fresh.\n");
        return;  // No data to load
    }

    while (!feof(file)) {
        Booking *newBooking = (Booking *)malloc(sizeof(Booking));
        if (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%lf,%d\n", 
                   newBooking->refNo, newBooking->name, newBooking->flightID,
                   newBooking->date, &newBooking->payment, &newBooking->cancelRequested) == 6) {
            newBooking->next = head;
            head = newBooking;  // Insert at the beginning of the list
        } else {
            free(newBooking);  // Invalid data format, free the allocated memory
        }
    }
    fclose(file);
    printf("Data loaded from details.csv\n");
}


// Load flights from the file into the linked list
void loadFlightsFromFile() {
    FILE *file = fopen(FLIGHT_FILE, "r");
    if (!file) {
        printf("Error opening flight data file.\n");
        return;
    }

    printf("Loading flights from file...\n");

    Flight *tail = flightHead;
    while (tail && tail->next) {
        tail = tail->next;
    }

    while (1) {
        Flight *newFlight = (Flight *)malloc(sizeof(Flight));
        if (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%f\n",
                   newFlight->flightID, newFlight->date, newFlight->time,
                   newFlight->source, newFlight->destination, &newFlight->price) != 6) {
            free(newFlight);
            break;
        }

        newFlight->next = NULL;

        if (tail) {
            tail->next = newFlight;
            tail = newFlight;
        } else {
            flightHead = newFlight;
            tail = newFlight;
        }

        printf("Loaded flight: %s, %s, %s, %s, %s, %.2f\n",
               newFlight->flightID, newFlight->date, newFlight->time,
               newFlight->source, newFlight->destination, newFlight->price);
    }

    fclose(file);
}

// Save flight data to file
void saveFlightsToFile() {
    FILE *file = fopen(FLIGHT_FILE, "w");
    if (!file) {
        printf("Error saving flight data.\n");
        return;
    }

    Flight *current = flightHead;
    while (current) {
        fprintf(file, "%s,%s,%s,%s,%s,%.2f\n", current->flightID, current->date,
                current->time, current->source, current->destination, current->price);
        current = current->next;
    }

    fclose(file);
}

// Display available flights
void viewAvailableFlights() {
    printf("\n=== Available Flights ===\n");
    printf("FlightID  |    Source   |   Destination | Date | Time | Price\n");
    Flight *current = flightHead;
    while (current) {
        printf("%s       |  %s     |     %s     | %s   | %s   |%.2f\n",
               current->flightID, current->source, current->destination,
               current->date, current->time, current->price);
        current = current->next;
    }
}

// Function to book a flight
void bookFlight() {
    char flightID[10];
    printf("\nEnter Flight ID to book: ");
    scanf("%s", flightID);

    Flight *flight = findFlight(flightID);
    if (!flight) {
        printf("Flight not found.\n");
        return;
    }

    // Display seat availability
    char seatFile[50];
    sprintf(seatFile, "%s_seats.csv", flightID);

    FILE *seatFilePtr = fopen(seatFile, "r+");
    if (!seatFilePtr) {
        printf("Error: Seat data for flight %s not found.\n", flightID);
        return;
    }

    char line[100];
    int seatGrid[200] = {0}; // 0 = available, 1 = booked
    int totalSeats = 0;

    // Skip the header line
    fgets(line, sizeof(line), seatFilePtr);

    while (fgets(line, sizeof(line), seatFilePtr)) {
        char *seat = strtok(line, ",");
        char *status = strtok(NULL, ",");
        int seatNum = atoi(seat);
        if (seatNum > 0 && strcmp(status, "Available\n") == 0) {
            seatGrid[seatNum - 1] = 0; // Available
        } else {
            seatGrid[seatNum - 1] = 1; // Booked
        }
        totalSeats++;
    }
    fclose(seatFilePtr);

    printf("\nAvailable Seats (0 = Available, X = Booked):\n\n");
    for (int i = 0; i < totalSeats; i++) {
        if (seatGrid[i] == 0) {
            printf("%10d", i + 1);
        } else {
            printf("%10c", 'X');
        }
        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
    }

    int seatNumber;
    printf("\nEnter seat number to book: ");
    scanf("%d", &seatNumber);

    if (seatNumber <= 0 || seatNumber > totalSeats || seatGrid[seatNumber - 1] == 1) {
        printf("Invalid or already booked seat.\n");
        return;
    }

    // Update seat file
    FILE *tempFile = fopen("temp.csv", "w");
    if (!tempFile) {
        printf("Error creating temporary file.\n");
        return;
    }

    seatFilePtr = fopen(seatFile, "r");
    fgets(line, sizeof(line), seatFilePtr); // Skip header
    fprintf(tempFile, "SeatNumber,Status\n");

    while (fgets(line, sizeof(line), seatFilePtr)) {
        char *seat = strtok(line, ",");
        char *status = strtok(NULL, ",");
        if (atoi(seat) == seatNumber && strcmp(status, "Available\n") == 0) {
            fprintf(tempFile, "%s,Booked\n", seat);
        } else {
            fprintf(tempFile, "%s,%s", seat, status);
        }
    }

    fclose(seatFilePtr);
    fclose(tempFile);

    remove(seatFile);
    rename("temp.csv", seatFile);

    // Save the booking details
    Booking *newBooking = (Booking *)malloc(sizeof(Booking));
    strcpy(newBooking->flightID, flightID);
    printf("Enter your name: ");
    scanf("%s", newBooking->name);

    strcpy(newBooking->date, flight->date); // Auto-fill date from flight info
    newBooking->payment = flight->price;
    newBooking->cancelRequested = 0;
    strcpy(newBooking->refNo, generateRefNo());
    newBooking->next = head;
    head = newBooking;

    // Payment prompt
    char paymentConfirmation[10];
    printf("Pay amount %.2f (type PAY to confirm payment): ", flight->price);
    scanf("%s", paymentConfirmation);

    // Check if the user typed "PAY"
    if (strcasecmp(paymentConfirmation, "PAY") == 0) {
        saveBookingToFile(newBooking);
        printf("Booking successful! Your reference number is: %s\n", newBooking->refNo);
        printf("Seat %d booked successfully on flight %s.\n", seatNumber, flightID);
    } else {
        printf("Payment not confirmed. Booking cancelled.\n");
    }
}



void viewTicket() {
    char refNo[10];
    printf("\nEnter Reference Number: ");
    scanf("%s", refNo);

    Booking *current = head;
    while (current) {
        if (strcmp(current->refNo, refNo) == 0) {
            printf("\n=== Booking Details ===\n");
            printf("Reference Number: %s\n", current->refNo);
            printf("Name: %s\n", current->name);
            printf("Flight ID: %s\n", current->flightID);
            printf("Date: %s\n", current->date);
            printf("Payment: %.2f Rs\n", current->payment);
            return;
        }
        current = current->next;
    }
    printf("No booking found with the given reference number.\n");
}


// Cancel a booking
void trimWhitespace(char *str) {
    char *end;
    // Skip leading whitespace
    while (*str == ' ' || *str == '\t') str++; 
    if (*str == 0)  // If the string is all whitespace
        return;
    
    end = str + strlen(str) - 1;
    
    // Skip trailing whitespace
    while (end > str && (*end == ' ' || *end == '\t')) end--; 
    
    // Null-terminate the string at the last non-whitespace character
    *(end + 1) = '\0';
}


// Function to cancel a booking
// Cancel a booking

void cancelBooking() {
    char refNo[10];
    printf("\nEnter Reference Number to Request Cancellation: ");
    scanf("%s", refNo);

    Booking *current = head;
    while (current) {
        if (strcmp(current->refNo, refNo) == 0) {
            // Move the booking to cancellation requests list
            CancelRequest *newRequest = (CancelRequest *)malloc(sizeof(CancelRequest));
            strcpy(newRequest->refNo, current->refNo);
            strcpy(newRequest->name, current->name);
            strcpy(newRequest->flightID, current->flightID);
            strcpy(newRequest->date, current->date);
            newRequest->payment = current->payment;
            newRequest->next = headCancelRequests;
            headCancelRequests = newRequest;

            // Mark booking for cancellation (or just leave it as is)
            current->cancelRequested = 1;
            updateCSV();

            printf("Cancellation request sent to admin for approval.\n");
            return;
        }
        current = current->next;
    }
    printf("No booking found with the given reference number.\n");
}




void createCancellationFileIfNotExists() {
    FILE *file = fopen("cancellation_requests.csv", "a");
    if (!file) {
        printf("Error creating or accessing the cancellation request file.\n");
        exit(1);
    }
    fclose(file);
}

// Admin menu for managing flights
void adminMenu() {
    int choice;

    while (1) {
        printf("\n=== Admin Menu ===\n");
        printf("1. View Cancellation Requests\n");
        printf("2. Approve Cancellation Request\n");
        printf("3. Add Flight\n");
        printf("4. Remove Flight\n");
        printf("5. View Total Payments\n");
        printf("6. View Available Flights\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                viewCancelRequests();
                break;
            case 2:
                {
                    char refNo[10];
                    printf("\nEnter Reference Number to Approve Cancellation: ");
                    scanf("%s", refNo);
                    approveCancellationFromRequest(refNo);
                }
                break;
            case 3:
                addFlight();
                break;
            case 4:
                removeFlight();
                break;
            case 5:
                viewTotalPayments();
                break;
            case 6:
                viewAvailableFlights();
                break;
            case 7:
                return;
            default:
                printf("Invalid choice.\n");
                break;
        }
    }
}


// View cancel requests
void viewCancelRequests() {
    CancelRequest *current = headCancelRequests;
    int found = 0;

    printf("\n=== Cancellation Requests ===\n");
    while (current) {
        found = 1;
        printf("RefNo: %s | Name: %s | FlightID: %s | Date: %s | Payment: %.2f Rs\n",
               current->refNo, current->name, current->flightID, current->date,
               current->payment);
        current = current->next;
    }

    if (!found) {
        printf("No cancellation requests found.\n");
    }
}


void removeCancellationRequest(char *refNo) {
    FILE *file = fopen("cancellation_requests.csv", "r");
    if (!file) {
        printf("Error opening cancellation request file.\n");
        return;
    }

    FILE *tempFile = fopen("temp_cancellations.csv", "w");
    if (!tempFile) {
        printf("Error opening temporary file.\n");
        fclose(file);
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char tempRefNo[10];
        sscanf(line, "%[^,],", tempRefNo);

        // Copy all lines except the canceled request
        if (strcmp(tempRefNo, refNo) != 0) {
            fputs(line, tempFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    // Replace the original file with the new one without the canceled request
    remove("cancellation_requests.csv");
    rename("temp_cancellations.csv", "cancellation_requests.csv");
}
// Function for admin authentication
void adminAuthentication() {
    char username[20], password[20];
    
    // Hardcoded admin credentials for simplicity
    char adminUsername[] = "admin";
    char adminPassword[] = "admin123";  // Replace with a more secure password in real apps

    printf("\n=== Admin Authentication ===\n");
    printf("Enter Admin Username: ");
    scanf("%s", username);
    printf("Enter Admin Password: ");
    scanf("%s", password);

    // Check if the credentials match
    if (strcmp(username, adminUsername) == 0 && strcmp(password, adminPassword) == 0) {
        printf("Authentication successful. Welcome, Admin!\n");
        adminMenu();  // Grant access to admin menu
    } else {
        printf("Invalid credentials. Access denied.\n");
    }
}


// Approve cancel request
void approveCancelRequest() {
    char refNo[10];
    printf("\nEnter booking reference number to approve cancellation: ");
    scanf("%s", refNo);

    Booking *current = head;
    while (current) {
        if (strcmp(current->refNo, refNo) == 0) {
            // Remove the booking from the linked list (or mark as canceled)
            current->cancelRequested = 0; // Approve cancellation
            updateCSV();  // Update the main booking file
            printf("Booking %s cancellation approved.\n", refNo);

            // Remove the request from the cancellation file
            removeCancellationRequest(refNo);

            return;
        }
        current = current->next;
    }

    printf("Booking not found.\n");
}

// Add a new flight
void addFlight() {
    Flight *newFlight = (Flight *)malloc(sizeof(Flight));

    // Input details for the new flight
    printf("Enter Flight ID: ");
    scanf("%s", newFlight->flightID);
    printf("Enter Date (DD/MM/YYYY): ");
    scanf("%s", newFlight->date);
    printf("Enter Time (HH:MM): ");
    scanf("%s", newFlight->time);
    printf("Enter Source: ");
    scanf("%s", newFlight->source);
    printf("Enter Destination: ");
    scanf("%s", newFlight->destination);
    printf("Enter Price: ");
    scanf("%f", &newFlight->price);

    // Create a unique seat file for the flight
    char seatFileName[50];
    sprintf(seatFileName, "%s_seats.csv", newFlight->flightID);

    FILE *seatFile = fopen(seatFileName, "w");
    if (!seatFile) {
        printf("Error creating seat file for flight.\n");
        free(newFlight);
        return;
    }

    // Initialize the seat file with 200 seats marked as "Available"
    fprintf(seatFile, "SeatNumber,Status\n"); // Header row
    for (int i = 1; i <= 200; i++) {
        fprintf(seatFile, "%d,Available\n", i);
    }
    fclose(seatFile);

    // Add the flight to the linked list
    newFlight->next = flightHead;
    flightHead = newFlight;

    // Save the flight to the flights file
    saveFlightsToFile();

    printf("Flight '%s' added successfully with 200 seats initialized as available.\n", newFlight->flightID);
}


// Function to remove a flight
void removeFlight() {
    char flightID[10];
    printf("Enter the flight ID to remove: ");
    scanf("%s", flightID);

    Flight *current = flightHead, *prev = NULL;
    while (current) {
        if (strcmp(current->flightID, flightID) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                flightHead = current->next;
            }
            free(current);
            saveFlightsToFile();
            printf("Flight removed successfully.\n");
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("Flight not found.\n");
}

// Function to view total payments
void viewTotalPayments() {
    float totalPayments = 0;
    Booking *current = head;
    while (current) {
        totalPayments += current->payment;
        current = current->next;
    }

    printf("Total payments: %.2f\n", totalPayments);
}

// Main function to show menu
int main() {
    loadDataFromCSV();
    createCSVIfNotExists();  // Initialize the booking CSV file if not exist
    loadFlightsFromFile();  // Load available flights from file

    int choice;
    do {
        printf("\n1. View Available Flights\n");
        printf("2. Book Flight\n");
        printf("3. View Ticket\n");
        printf("4. Cancel Booking\n");
        printf("5. Admin Menu\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: viewAvailableFlights(); break;
            case 2: bookFlight(); break;
            case 3: viewTicket(); break;
            case 4: cancelBooking(); break;
            case 5: adminAuthentication(); break;
            case 6: printf("Exiting...\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 6);

    saveDataToCSV();
    return 0;
}