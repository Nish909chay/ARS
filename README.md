# ARS
Airline Reservation System
The Airline Reservation System project is designed to manage flight bookings, cancellations, and flight data efficiently. It provides functionalities for both users and administrators to interact with the system seamlessly. 

### **User Side**
- **Book Flight**: Users can search for available flights, select a flight, choose a seat, and book the flight by making a payment.
- **View Ticket**: Users can view the details of their bookings using a reference number.
- **Cancel Booking**: Users can request cancellations for their bookings, which are processed through an admin interface.

### **Admin Side**
- **Add Flight**: Administrators can add new flights by entering flight details such as flight ID, date, time, destination, source, and price.
- **Remove Flight**: Administrators can remove existing flights from the system.
- **View Available Flights**: Administrators can view all available flights in the system.

### **Data Storage (CSV Files)**
- **Booking Data**: User bookings are stored in `details.csv`, including reference numbers, passenger details, flight details, and payment information.
- **Flight Data**: Flight details are stored in `flights.csv` for easy access and modification.
- **Cancellation Requests**: Pending cancellation requests are stored in `cancellation_requests.csv` until approved by an admin.

This system utilizes linked lists for efficient data management and file I/O for persistent storage, allowing for streamlined access and update operations. The separation of user and admin interfaces ensures secure access and streamlined management of bookings and flight data.
