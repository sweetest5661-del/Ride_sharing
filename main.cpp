/**
 * ============================================================================
 * FILE: main.cpp
 * LANGUAGE: C++ (Object-Oriented Programming, Zero STL)
 *
 * PURPOSE:
 * Implements the SmartRide Intelligent Route-Pooling System using:
 * 1. Classes and Objects for Domain Data Representation
 * 2. Inheritance & Polymorphism (Person -> Rider/Driver, Fare -> Solo/Shared, Ride -> Solo/Shared)
 * 3. Pure C-style arrays and strings (No std::string, std::vector, or std::map)
 * 4. File I/O using standard C file functions (fopen, fprintf, fgets, fclose)
 * 5. Calls pure C QuickSort and string routines from algorithms.c
 * ============================================================================
 */

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <sys/stat.h>

using namespace std;

/* ============================================================================
 * SECTION: C Algorithm Linkage
 * WORK:
 * Includes the pure C algorithms from algorithms.c so that compiling
 * simply with 'g++ main.cpp' works immediately in any terminal.
 * ============================================================================ */
#include "algorithms.c"


/* ============================================================================
 * SECTION: System Constants and Terminal Colors
 * WORK:
 * Defines fixed sizes and ANSI color codes for rich, readable console output.
 * ============================================================================ */
const int MAX_RIDERS = 50;
const int MAX_DRIVERS = 30;
const int MAX_RIDES = 100;
const int MAX_LOCATIONS = 8;
const int MAX_HISTORY = 20;

namespace Colors {
    const char* const RESET   = "\033[0m";
    const char* const BOLD    = "\033[1m";
    const char* const RED     = "\033[31m";
    const char* const GREEN   = "\033[32m";
    const char* const YELLOW  = "\033[33m";
    const char* const BLUE    = "\033[34m";
    const char* const MAGENTA = "\033[35m";
    const char* const CYAN    = "\033[36m";
}

/* ============================================================================
 * ENUMS: RideStatus & DriverStatus
 * WORK:
 * Tracks lifecycle states of rides and driver availability.
 * ============================================================================ */
enum class RideStatus { REQUESTED, CONFIRMED, COMPLETED, CANCELLED };
enum class DriverStatus { AVAILABLE, BUSY, OFFLINE };
enum class RideType { SOLO, SHARED };

/* ============================================================================
 * HELPER FUNCTIONS: Status Conversions
 * WORK:
 * Converts enum status values to printable C-strings and vice versa.
 * ============================================================================ */
const char* rideStatusToString(RideStatus st) {
    switch (st) {
        case RideStatus::REQUESTED: return "REQUESTED";
        case RideStatus::CONFIRMED: return "CONFIRMED";
        case RideStatus::COMPLETED: return "COMPLETED";
        case RideStatus::CANCELLED: return "CANCELLED";
        default:                    return "UNKNOWN";
    }
}

RideStatus stringToRideStatus(const char* s) {
    if (!s) return RideStatus::REQUESTED;
    if (strcmp(s, "CONFIRMED") == 0) return RideStatus::CONFIRMED;
    if (strcmp(s, "COMPLETED") == 0) return RideStatus::COMPLETED;
    if (strcmp(s, "CANCELLED") == 0) return RideStatus::CANCELLED;
    return RideStatus::REQUESTED;
}

const char* driverStatusToString(DriverStatus st) {
    switch (st) {
        case DriverStatus::AVAILABLE: return "Available";
        case DriverStatus::BUSY:      return "Busy";
        case DriverStatus::OFFLINE:   return "Offline";
        default:                      return "Unknown";
    }
}

DriverStatus stringToDriverStatus(const char* s) {
    if (!s) return DriverStatus::AVAILABLE;
    if (strcmp(s, "Busy") == 0 || strcmp(s, "BUSY") == 0) return DriverStatus::BUSY;
    if (strcmp(s, "Offline") == 0 || strcmp(s, "OFFLINE") == 0) return DriverStatus::OFFLINE;
    return DriverStatus::AVAILABLE;
}

/* ============================================================================
 * CLASS: Person (Abstract Base Class)
 * WORK:
 * Encapsulates shared attributes (ID, Name, Phone) for all human participants.
 * Demonstrates Abstraction with pure virtual displayProfile().
 * ============================================================================ */
class Person {
protected:
    char id[32];
    char name[64];
    char phone[32];

public:
    Person(const char* i = "", const char* n = "", const char* p = "") {
        strncpy(id, i ? i : "", sizeof(id) - 1); id[sizeof(id) - 1] = '\0';
        strncpy(name, n ? n : "", sizeof(name) - 1); name[sizeof(name) - 1] = '\0';
        strncpy(phone, p ? p : "", sizeof(phone) - 1); phone[sizeof(phone) - 1] = '\0';
    }
    virtual ~Person() {}

    /* Pure Virtual Function: Implemented by Rider and Driver */
    virtual void displayProfile() const = 0;

    const char* getId() const { return id; }
    const char* getName() const { return name; }
    const char* getPhone() const { return phone; }
};

/* ============================================================================
 * CLASS: Rider (Inherits from Person)
 * WORK:
 * Stores rider-specific travel information: pickup, destination, requested
 * seats, and trip history array.
 * ============================================================================ */
class Rider : public Person {
private:
    char currentSource[64];
    char currentDestination[64];
    int requestedSeats;
    char rideHistory[MAX_HISTORY][32];
    int historyCount;

public:
    Rider(const char* i = "", const char* n = "", const char* p = "",
          const char* src = "", const char* dest = "", int seats = 1)
        : Person(i, n, p), requestedSeats(seats > 0 ? seats : 1), historyCount(0) {
        strncpy(currentSource, src ? src : "", sizeof(currentSource) - 1);
        currentSource[sizeof(currentSource) - 1] = '\0';
        strncpy(currentDestination, dest ? dest : "", sizeof(currentDestination) - 1);
        currentDestination[sizeof(currentDestination) - 1] = '\0';
    }

    void displayProfile() const override {
        cout << Colors::CYAN << "----------------------------------------\n" << Colors::RESET;
        cout << Colors::BOLD << "           RIDER PROFILE                \n" << Colors::RESET;
        cout << Colors::CYAN << "----------------------------------------\n" << Colors::RESET;
        cout << " Rider ID     : " << Colors::YELLOW << id << Colors::RESET << "\n";
        cout << " Name         : " << name << "\n";
        cout << " Phone        : " << phone << "\n";
        cout << " Source       : " << (currentSource[0] ? currentSource : "Not Set") << "\n";
        cout << " Destination  : " << (currentDestination[0] ? currentDestination : "Not Set") << "\n";
        cout << " Seats        : " << requestedSeats << "\n";
        cout << " Total Rides  : " << historyCount << "\n";
        if (historyCount > 0) {
            cout << " Ride History : ";
            for (int j = 0; j < historyCount; j++) {
                cout << rideHistory[j] << (j + 1 < historyCount ? ", " : "");
            }
            cout << "\n";
        }
        cout << Colors::CYAN << "----------------------------------------\n" << Colors::RESET;
    }

    const char* getSource() const { return currentSource; }
    const char* getDestination() const { return currentDestination; }
    int getSeats() const { return requestedSeats; }
    int getHistoryCount() const { return historyCount; }
    const char* getRideHistoryAt(int idx) const {
        return (idx >= 0 && idx < historyCount) ? rideHistory[idx] : "";
    }

    void setSource(const char* s) {
        strncpy(currentSource, s ? s : "", sizeof(currentSource) - 1);
        currentSource[sizeof(currentSource) - 1] = '\0';
    }
    void setDestination(const char* d) {
        strncpy(currentDestination, d ? d : "", sizeof(currentDestination) - 1);
        currentDestination[sizeof(currentDestination) - 1] = '\0';
    }
    void setSeats(int s) { requestedSeats = (s > 0 ? s : 1); }

    void addRideToHistory(const char* rideId) {
        if (rideId && historyCount < MAX_HISTORY) {
            strncpy(rideHistory[historyCount], rideId, 31);
            rideHistory[historyCount][31] = '\0';
            historyCount++;
        }
    }
};

/* ============================================================================
 * CLASS: Driver (Inherits from Person)
 * WORK:
 * Represents a cab driver: vehicle details, capacity, and current status.
 * ============================================================================ */
class Driver : public Person {
private:
    char vehicleNumber[32];
    char vehicleType[32];
    int availableSeats;
    DriverStatus status;
    char assignedRides[MAX_HISTORY][32];
    int assignedCount;

public:
    Driver(const char* i = "", const char* n = "", const char* p = "",
           const char* vNum = "", const char* vType = "Sedan", int seats = 4,
           DriverStatus st = DriverStatus::AVAILABLE)
        : Person(i, n, p), availableSeats(seats), status(st), assignedCount(0) {
        strncpy(vehicleNumber, vNum ? vNum : "", sizeof(vehicleNumber) - 1);
        vehicleNumber[sizeof(vehicleNumber) - 1] = '\0';
        strncpy(vehicleType, vType ? vType : "Sedan", sizeof(vehicleType) - 1);
        vehicleType[sizeof(vehicleType) - 1] = '\0';
    }

    void displayProfile() const override {
        cout << Colors::CYAN << "----------------------------------------\n" << Colors::RESET;
        cout << Colors::BOLD << "           DRIVER PROFILE               \n" << Colors::RESET;
        cout << Colors::CYAN << "----------------------------------------\n" << Colors::RESET;
        cout << " Driver ID       : " << Colors::YELLOW << id << Colors::RESET << "\n";
        cout << " Name            : " << name << "\n";
        cout << " Phone           : " << phone << "\n";
        cout << " Vehicle No      : " << vehicleNumber << "\n";
        cout << " Vehicle Type    : " << vehicleType << "\n";
        cout << " Available Seats : " << availableSeats << "\n";
        cout << " Status          : " << driverStatusToString(status) << "\n";
        cout << " Total Trips     : " << assignedCount << "\n";
        cout << Colors::CYAN << "----------------------------------------\n" << Colors::RESET;
    }

    const char* getVehicleNumber() const { return vehicleNumber; }
    const char* getVehicleType() const { return vehicleType; }
    int getAvailableSeats() const { return availableSeats; }
    DriverStatus getStatus() const { return status; }
    int getAssignedCount() const { return assignedCount; }
    const char* getAssignedRideAt(int idx) const {
        return (idx >= 0 && idx < assignedCount) ? assignedRides[idx] : "";
    }

    void setStatus(DriverStatus st) { status = st; }
    void assignRide(const char* rideId) {
        if (rideId && assignedCount < MAX_HISTORY) {
            strncpy(assignedRides[assignedCount], rideId, 31);
            assignedRides[assignedCount][31] = '\0';
            assignedCount++;
            status = DriverStatus::BUSY;
        }
    }
    void completeRide() { status = DriverStatus::AVAILABLE; }
};

/* ============================================================================
 * CLASS: Fare (Abstract Base Class for Pricing)
 * WORK:
 * Encapsulates base pricing parameters (Base fare: ₹50, Rate/KM: ₹10).
 * Demonstrates Runtime Polymorphism with virtual calculateFare().
 * ============================================================================ */
class Fare {
protected:
    double baseFare;
    double ratePerKm;
    double distanceKm;

public:
    Fare(double base = 50.0, double rate = 10.0, double dist = 0.0)
        : baseFare(base), ratePerKm(rate), distanceKm(dist >= 0 ? dist : 0) {}
    virtual ~Fare() {}

    /* Pure virtual method */
    virtual double calculateFare() const = 0;

    double getDistanceKm() const { return distanceKm; }
    void setDistanceKm(double d) { distanceKm = (d >= 0 ? d : 0); }
};

/* ============================================================================
 * CLASS: SoloFare (Inherits from Fare)
 * WORK:
 * Computes standard single-rider metered fare:
 * Total Fare = ₹50 Base + (Distance * ₹10)
 * ============================================================================ */
class SoloFare : public Fare {
public:
    SoloFare(double dist = 0.0) : Fare(50.0, 10.0, dist) {}
    double calculateFare() const override {
        return baseFare + (distanceKm * ratePerKm);
    }
};

/* ============================================================================
 * CLASS: SharedFare (Inherits from Fare)
 * WORK:
 * Computes dynamically split carpool fare:
 * Per-Person Fare = (₹50 Base + Distance * ₹10) / Total Riders
 * Savings = Solo Fare - Per-Person Fare
 * ============================================================================ */
class SharedFare : public Fare {
private:
    int totalRiders;

public:
    SharedFare(double dist = 0.0, int riders = 2)
        : Fare(50.0, 10.0, dist), totalRiders(riders > 0 ? riders : 1) {}

    double calculateTotalFare() const {
        return baseFare + (distanceKm * ratePerKm);
    }

    double calculateFare() const override {
        return calculateTotalFare() / static_cast<double>(totalRiders);
    }

    double calculateSavings() const {
        return calculateTotalFare() - calculateFare();
    }

    void setTotalRiders(int r) { totalRiders = (r > 0 ? r : 1); }
    int getTotalRiders() const { return totalRiders; }
};

/* ============================================================================
 * CLASS: Ride (Abstract Base Class for Trips)
 * WORK:
 * Encapsulates trip metadata: pickup, drop-off, distance, status, assigned driver.
 * ============================================================================ */
class Ride {
protected:
    char rideId[32];
    char source[64];
    char destination[64];
    double distanceKm;
    RideStatus status;
    char driverId[32];

public:
    Ride(const char* id, const char* src, const char* dest, double dist,
         RideStatus st = RideStatus::REQUESTED, const char* dId = "None")
        : distanceKm(dist >= 0 ? dist : 0), status(st) {
        strncpy(rideId, id ? id : "", sizeof(rideId) - 1); rideId[sizeof(rideId) - 1] = '\0';
        strncpy(source, src ? src : "", sizeof(source) - 1); source[sizeof(source) - 1] = '\0';
        strncpy(destination, dest ? dest : "", sizeof(destination) - 1); destination[sizeof(destination) - 1] = '\0';
        strncpy(driverId, dId ? dId : "None", sizeof(driverId) - 1); driverId[sizeof(driverId) - 1] = '\0';
    }
    virtual ~Ride() {}

    /* Polymorphic Interface */
    virtual void displayReceipt() const = 0;
    virtual void displayDetails() const = 0;
    virtual RideType getRideType() const = 0;
    virtual double getFarePerPerson() const = 0;
    virtual double getTotalFare() const = 0;
    virtual int getRiderCount() const = 0;
    virtual const char* getRiderIdAt(int idx) const = 0;
    virtual bool hasRider(const char* rId) const = 0;

    const char* getRideId() const { return rideId; }
    const char* getSource() const { return source; }
    const char* getDestination() const { return destination; }
    double getDistanceKm() const { return distanceKm; }
    RideStatus getStatus() const { return status; }
    const char* getDriverId() const { return driverId; }

    void setStatus(RideStatus s) { status = s; }
    void setDriverId(const char* d) {
        strncpy(driverId, d ? d : "None", sizeof(driverId) - 1);
        driverId[sizeof(driverId) - 1] = '\0';
    }
};

/* ============================================================================
 * CLASS: SoloRide (Inherits from Ride)
 * WORK:
 * Represents a private ride booked by a single passenger.
 * ============================================================================ */
class SoloRide : public Ride {
private:
    char riderId[32];
    SoloFare fare;

public:
    SoloRide(const char* id, const char* rId, const char* src, const char* dest,
             double dist, RideStatus st = RideStatus::REQUESTED, const char* dId = "None")
        : Ride(id, src, dest, dist, st, dId), fare(dist) {
        strncpy(riderId, rId ? rId : "", sizeof(riderId) - 1);
        riderId[sizeof(riderId) - 1] = '\0';
    }

    RideType getRideType() const override { return RideType::SOLO; }
    double getFarePerPerson() const override { return fare.calculateFare(); }
    double getTotalFare() const override { return fare.calculateFare(); }
    int getRiderCount() const override { return 1; }
    const char* getRiderIdAt(int) const override { return riderId; }
    bool hasRider(const char* rId) const override {
        return (rId && strcmp(riderId, rId) == 0);
    }

    void displayReceipt() const override {
        cout << "\n" << Colors::CYAN << "========================================\n" << Colors::RESET;
        cout << Colors::BOLD << "           SMARTRIDE RECEIPT            \n" << Colors::RESET;
        cout << Colors::CYAN << "========================================\n" << Colors::RESET;
        cout << " Ride ID     : " << Colors::YELLOW << rideId << Colors::RESET << "\n";
        cout << " Rider ID    : " << riderId << "\n";
        cout << " Driver ID   : " << driverId << "\n";
        cout << " Route       : " << source << " -> " << destination << "\n";
        cout << " Distance    : " << distanceKm << " KM\n";
        cout << " Ride Type   : " << Colors::BLUE << "SOLO" << Colors::RESET << "\n";
        cout << " Total Fare  : " << Colors::GREEN << "₹" << fixed << setprecision(0) << getTotalFare() << Colors::RESET << "\n";
        cout << " Status      : " << rideStatusToString(status) << "\n";
        cout << Colors::CYAN << "========================================\n" << Colors::RESET;
    }

    void displayDetails() const override {
        cout << "[SOLO] Ride #" << rideId << " | Rider: " << riderId
             << " | Driver: " << driverId << " | " << source << " -> " << destination
             << " (" << distanceKm << " KM) | Fare: ₹" << fixed << setprecision(0) << getTotalFare()
             << " | Status: " << rideStatusToString(status) << "\n";
    }
};

/* ============================================================================
 * CLASS: SharedRide (Inherits from Ride)
 * WORK:
 * Represents a pooled ride shared by up to 4 passengers traveling identical
 * or overlapping routes, splitting the fare among them.
 * ============================================================================ */
class SharedRide : public Ride {
public:
    static const int MAX_RIDERS_PER_CAB = 4;

private:
    char riderIds[MAX_RIDERS_PER_CAB][32];
    int riderCount;
    SharedFare fare;

public:
    SharedRide(const char* id, const char rIds[][32], int count, const char* src,
               const char* dest, double dist, RideStatus st = RideStatus::REQUESTED,
               const char* dId = "None")
        : Ride(id, src, dest, dist, st, dId), riderCount(0), fare(dist, count > 0 ? count : 1) {
        if (rIds) {
            for (int i = 0; i < count && i < MAX_RIDERS_PER_CAB; i++) {
                strncpy(riderIds[i], rIds[i], 31);
                riderIds[i][31] = '\0';
                riderCount++;
            }
        }
        fare.setTotalRiders(riderCount > 0 ? riderCount : 1);
    }

    RideType getRideType() const override { return RideType::SHARED; }
    double getFarePerPerson() const override { return fare.calculateFare(); }
    double getTotalFare() const override { return fare.calculateTotalFare(); }
    double getSavings() const { return fare.calculateSavings(); }
    int getRiderCount() const override { return riderCount; }
    const char* getRiderIdAt(int idx) const override {
        return (idx >= 0 && idx < riderCount) ? riderIds[idx] : "";
    }

    bool hasRider(const char* rId) const override {
        if (!rId) return false;
        for (int i = 0; i < riderCount; i++) {
            if (strcmp(riderIds[i], rId) == 0) return true;
        }
        return false;
    }

    bool addRider(const char* rId) {
        if (!rId || riderCount >= MAX_RIDERS_PER_CAB || hasRider(rId)) return false;
        strncpy(riderIds[riderCount], rId, 31);
        riderIds[riderCount][31] = '\0';
        riderCount++;
        fare.setTotalRiders(riderCount);
        return true;
    }

    int getAvailableSeats() const { return MAX_RIDERS_PER_CAB - riderCount; }

    void displayReceipt() const override {
        cout << "\n" << Colors::CYAN << "========================================\n" << Colors::RESET;
        cout << Colors::BOLD << "           SMARTRIDE RECEIPT            \n" << Colors::RESET;
        cout << Colors::CYAN << "========================================\n" << Colors::RESET;
        cout << " Ride ID      : " << Colors::YELLOW << rideId << Colors::RESET << "\n";
        cout << " Passengers   : ";
        for (int i = 0; i < riderCount; i++) cout << riderIds[i] << (i + 1 < riderCount ? ", " : "");
        cout << "\n Driver ID    : " << driverId << "\n";
        cout << " Route        : " << source << " -> " << destination << "\n";
        cout << " Distance     : " << distanceKm << " KM\n";
        cout << " Ride Type    : " << Colors::MAGENTA << "SHARED (POOLED)" << Colors::RESET << "\n";
        cout << " Full Fare    : ₹" << fixed << setprecision(0) << getTotalFare() << "\n";
        cout << " Fare / Person: " << Colors::GREEN << "₹" << fixed << setprecision(0) << getFarePerPerson() << Colors::RESET << "\n";
        cout << " You Saved    : " << Colors::YELLOW << "₹" << fixed << setprecision(0) << getSavings() << Colors::RESET << "\n";
        cout << " Status       : " << rideStatusToString(status) << "\n";
        cout << Colors::CYAN << "========================================\n" << Colors::RESET;
    }

    void displayDetails() const override {
        cout << "[SHARED] Ride #" << rideId << " | Riders (" << riderCount << "/" << MAX_RIDERS_PER_CAB << "): ";
        for (int i = 0; i < riderCount; i++) cout << riderIds[i] << (i + 1 < riderCount ? ", " : "");
        cout << " | Driver: " << driverId << " | " << source << " -> " << destination
             << " (" << distanceKm << " KM) | ₹" << fixed << setprecision(0) << getFarePerPerson() << "/person"
             << " | Status: " << rideStatusToString(status) << "\n";
    }
};

/* ============================================================================
 * CLASS: LocationManager
 * WORK:
 * Manages regional transit network nodes and a 2D distance matrix lookup table.
 * Zero map/vector: Uses fixed 2D array distanceMatrix[8][8].
 * ============================================================================ */
class LocationManager {
private:
    char locations[MAX_LOCATIONS][64];
    int locationCount;
    double distanceMatrix[MAX_LOCATIONS][MAX_LOCATIONS];

    void addDistance(const char* l1, const char* l2, double dist) {
        int i1 = findIndex(l1);
        int i2 = findIndex(l2);
        if (i1 != -1 && i2 != -1) {
            distanceMatrix[i1][i2] = dist;
            distanceMatrix[i2][i1] = dist;
        }
    }

public:
    LocationManager() : locationCount(0) {
        const char* list[MAX_LOCATIONS] = {
            "Clock Tower", "ISBT", "Rajpur Road", "Clement Town",
            "Jakhan", "Prem Nagar", "Haridwar", "Rishikesh"
        };
        for (int i = 0; i < MAX_LOCATIONS; i++) {
            strncpy(locations[i], list[i], 63);
            locations[i][63] = '\0';
            locationCount++;
            for (int j = 0; j < MAX_LOCATIONS; j++) {
                distanceMatrix[i][j] = (i == j) ? 0.0 : 15.0;
            }
        }

        /* Known Regional Distances */
        addDistance("Clock Tower", "ISBT", 8.0);
        addDistance("Clock Tower", "Rajpur Road", 4.0);
        addDistance("Clock Tower", "Jakhan", 6.0);
        addDistance("Clock Tower", "Clement Town", 11.5);
        addDistance("Clock Tower", "Prem Nagar", 12.0);
        addDistance("Clock Tower", "Haridwar", 54.0);
        addDistance("Clock Tower", "Rishikesh", 45.0);
        addDistance("ISBT", "Rajpur Road", 12.0);
        addDistance("ISBT", "Clement Town", 4.5);
        addDistance("ISBT", "Jakhan", 14.0);
        addDistance("ISBT", "Prem Nagar", 9.5);
        addDistance("ISBT", "Haridwar", 50.0);
        addDistance("ISBT", "Rishikesh", 43.0);
        addDistance("Rajpur Road", "Jakhan", 3.0);
        addDistance("Rajpur Road", "Clement Town", 15.0);
        addDistance("Rajpur Road", "Prem Nagar", 16.0);
        addDistance("Rajpur Road", "Haridwar", 58.0);
        addDistance("Rajpur Road", "Rishikesh", 48.0);
        addDistance("Jakhan", "Clement Town", 17.0);
        addDistance("Jakhan", "Prem Nagar", 18.0);
        addDistance("Jakhan", "Haridwar", 60.0);
        addDistance("Jakhan", "Rishikesh", 49.0);
        addDistance("Clement Town", "Prem Nagar", 14.0);
        addDistance("Clement Town", "Haridwar", 48.0);
        addDistance("Clement Town", "Rishikesh", 46.0);
        addDistance("Prem Nagar", "Haridwar", 62.0);
        addDistance("Prem Nagar", "Rishikesh", 55.0);
        addDistance("Haridwar", "Rishikesh", 24.0);
    }

    int findIndex(const char* name) const {
        if (!name) return -1;
        for (int i = 0; i < locationCount; i++) {
            if (c_str_case_cmp(locations[i], name) == 0) return i;
        }
        return -1;
    }

    const char* resolveLocation(const char* input) const {
        int idx = findIndex(input);
        return (idx != -1) ? locations[idx] : nullptr;
    }

    double getDistance(const char* src, const char* dest) const {
        int i1 = findIndex(src);
        int i2 = findIndex(dest);
        if (i1 != -1 && i2 != -1) return distanceMatrix[i1][i2];
        return 12.0; /* Default estimate if unknown */
    }

    bool areNearby(const char* l1, const char* l2) const {
        double d = getDistance(l1, l2);
        return (d > 0.0 && d <= 6.0);
    }

    int getCount() const { return locationCount; }
    const char* getLocationAt(int i) const {
        return (i >= 0 && i < locationCount) ? locations[i] : "";
    }

    void displayLocations() const {
        cout << Colors::CYAN << "Available Route Nodes in SmartRide Transit Network:\n" << Colors::RESET;
        for (int i = 0; i < locationCount; i++) {
            cout << "  " << (i + 1) << ". " << locations[i];
            if ((i + 1) % 4 == 0) cout << "\n";
            else cout << "\t\t";
        }
        if (locationCount % 4 != 0) cout << "\n";
    }
};

/* ============================================================================
 * CLASS: RideSharingSystem
 * WORK:
 * Main system engine that stores arrays of Riders, Drivers, and Rides.
 * Handles menu navigation, booking, QuickSort matching, and C file I/O.
 * ============================================================================ */
class RideSharingSystem {
private:
    Rider* riders[MAX_RIDERS];
    int riderCount;

    Driver* drivers[MAX_DRIVERS];
    int driverCount;

    Ride* rides[MAX_RIDES];
    int rideCount;

    LocationManager locMgr;

    int nextRiderIdNum;
    int nextDriverIdNum;
    int nextRideIdNum;

    /* Helper: Splits pipe-delimited line from text files */
    int splitLine(const char* str, char delim, char tokens[][64], int maxTokens) {
        int count = 0, charIdx = 0;
        while (*str && count < maxTokens) {
            if (*str == delim) {
                tokens[count][charIdx] = '\0';
                count++;
                charIdx = 0;
            } else {
                if (charIdx < 63) tokens[count][charIdx++] = *str;
            }
            str++;
        }
        if (count < maxTokens) {
            tokens[count][charIdx] = '\0';
            count++;
        }
        return count;
    }

public:
    RideSharingSystem()
        : riderCount(0), driverCount(0), rideCount(0),
          nextRiderIdNum(101), nextDriverIdNum(201), nextRideIdNum(1001) {
        for (int i = 0; i < MAX_RIDERS; i++) riders[i] = nullptr;
        for (int i = 0; i < MAX_DRIVERS; i++) drivers[i] = nullptr;
        for (int i = 0; i < MAX_RIDES; i++) rides[i] = nullptr;

        mkdir("data", 0777);
        loadData();
        seedIfEmpty();
    }

    ~RideSharingSystem() {
        saveData();
        for (int i = 0; i < riderCount; i++) delete riders[i];
        for (int i = 0; i < driverCount; i++) delete drivers[i];
        for (int i = 0; i < rideCount; i++) delete rides[i];
    }

    /* ========================================================================
     * METHOD: loadData
     * WORK:
     * Reads riders.txt, drivers.txt, and rides.txt using standard C fopen/fgets.
     * ======================================================================== */
    void loadData() {
        /* 1. Load Riders */
        FILE* fp = fopen("data/riders.txt", "r");
        if (fp) {
            char line[256];
            while (fgets(line, sizeof(line), fp) && riderCount < MAX_RIDERS) {
                c_str_trim(line);
                if (line[0] == '\0' || line[0] == '#') continue;
                char t[10][64];
                int n = splitLine(line, '|', t, 10);
                if (n >= 6) {
                    riders[riderCount] = new Rider(t[0], t[1], t[2], t[3], t[4], atoi(t[5]));
                    if (n >= 7 && t[6][0] != '\0') {
                        char h[10][64];
                        int hz = splitLine(t[6], ',', h, 10);
                        for (int k = 0; k < hz; k++) riders[riderCount]->addRideToHistory(h[k]);
                    }
                    int num = atoi(t[0] + 1);
                    if (num >= nextRiderIdNum) nextRiderIdNum = num + 1;
                    riderCount++;
                }
            }
            fclose(fp);
        }

        /* 2. Load Drivers */
        fp = fopen("data/drivers.txt", "r");
        if (fp) {
            char line[256];
            while (fgets(line, sizeof(line), fp) && driverCount < MAX_DRIVERS) {
                c_str_trim(line);
                if (line[0] == '\0' || line[0] == '#') continue;
                char t[10][64];
                int n = splitLine(line, '|', t, 10);
                if (n >= 7) {
                    DriverStatus st = stringToDriverStatus(t[6]);
                    drivers[driverCount] = new Driver(t[0], t[1], t[2], t[3], t[4], atoi(t[5]), st);
                    int num = atoi(t[0] + 1);
                    if (num >= nextDriverIdNum) nextDriverIdNum = num + 1;
                    driverCount++;
                }
            }
            fclose(fp);
        }

        /* 3. Load Rides */
        fp = fopen("data/rides.txt", "r");
        if (fp) {
            char line[256];
            while (fgets(line, sizeof(line), fp) && rideCount < MAX_RIDES) {
                c_str_trim(line);
                if (line[0] == '\0' || line[0] == '#') continue;
                char t[10][64];
                int n = splitLine(line, '|', t, 10);
                if (n >= 8) {
                    double dist = atof(t[6]);
                    RideStatus st = stringToRideStatus(t[7]);
                    char rArr[4][64];
                    int rz = splitLine(t[2], ',', rArr, 4);

                    if (strcmp(t[1], "SOLO") == 0) {
                        rides[rideCount] = new SoloRide(t[0], (rz > 0 ? rArr[0] : ""), t[4], t[5], dist, st, t[3]);
                    } else {
                        char passArr[4][32];
                        for (int p = 0; p < rz && p < 4; p++) {
                            strncpy(passArr[p], rArr[p], 31);
                            passArr[p][31] = '\0';
                        }
                        rides[rideCount] = new SharedRide(t[0], passArr, rz, t[4], t[5], dist, st, t[3]);
                    }
                    int num = atoi(t[0] + 1);
                    if (num >= nextRideIdNum) nextRideIdNum = num + 1;
                    rideCount++;
                }
            }
            fclose(fp);
        }
    }

    /* ========================================================================
     * METHOD: saveData
     * WORK:
     * Saves riders, drivers, and rides to text files using standard C fprintf.
     * ======================================================================== */
    void saveData() {
        FILE* fp = fopen("data/riders.txt", "w");
        if (fp) {
            for (int i = 0; i < riderCount; i++) {
                Rider* r = riders[i];
                if (!r) continue;
                fprintf(fp, "%s|%s|%s|%s|%s|%d|",
                        r->getId(), r->getName(), r->getPhone(),
                        r->getSource(), r->getDestination(), r->getSeats());
                for (int j = 0; j < r->getHistoryCount(); j++) {
                    fprintf(fp, "%s%s", r->getRideHistoryAt(j), (j + 1 < r->getHistoryCount() ? "," : ""));
                }
                fprintf(fp, "\n");
            }
            fclose(fp);
        }

        fp = fopen("data/drivers.txt", "w");
        if (fp) {
            for (int i = 0; i < driverCount; i++) {
                Driver* d = drivers[i];
                if (!d) continue;
                fprintf(fp, "%s|%s|%s|%s|%s|%d|%s|\n",
                        d->getId(), d->getName(), d->getPhone(),
                        d->getVehicleNumber(), d->getVehicleType(),
                        d->getAvailableSeats(), driverStatusToString(d->getStatus()));
            }
            fclose(fp);
        }

        fp = fopen("data/rides.txt", "w");
        if (fp) {
            for (int i = 0; i < rideCount; i++) {
                Ride* rd = rides[i];
                if (!rd) continue;
                fprintf(fp, "%s|%s|", rd->getRideId(), (rd->getRideType() == RideType::SOLO ? "SOLO" : "SHARED"));
                for (int j = 0; j < rd->getRiderCount(); j++) {
                    fprintf(fp, "%s%s", rd->getRiderIdAt(j), (j + 1 < rd->getRiderCount() ? "," : ""));
                }
                fprintf(fp, "|%s|%s|%s|%.1f|%s\n",
                        rd->getDriverId(), rd->getSource(), rd->getDestination(),
                        rd->getDistanceKm(), rideStatusToString(rd->getStatus()));
            }
            fclose(fp);
        }
    }

    /* Seed default records if system is brand new */
    void seedIfEmpty() {
        if (riderCount == 0) {
            riders[riderCount++] = new Rider("R101", "Rahul", "9876543210", "Clock Tower", "ISBT", 1);
            riders[riderCount++] = new Rider("R102", "Priya", "9876543211", "Clock Tower", "ISBT", 1);
            riders[riderCount++] = new Rider("R103", "Rohan", "9876543212", "Clock Tower", "Haridwar", 1);
            riders[riderCount++] = new Rider("R104", "Ananya", "9876543213", "Rajpur Road", "ISBT", 1);
            saveData();
        }
        if (driverCount == 0) {
            drivers[driverCount++] = new Driver("D201", "Amit", "9123456780", "UK07AB1234", "Sedan", 4, DriverStatus::AVAILABLE);
            drivers[driverCount++] = new Driver("D202", "Vikram", "9123456781", "UK07CD5678", "SUV", 6, DriverStatus::AVAILABLE);
            drivers[driverCount++] = new Driver("D203", "Neha", "9123456782", "UK07EF9012", "Hatchback", 4, DriverStatus::AVAILABLE);
            saveData();
        }
    }

    Rider* findRider(const char* id) {
        if (!id) return nullptr;
        for (int i = 0; i < riderCount; i++) {
            if (riders[i] && strcmp(riders[i]->getId(), id) == 0) return riders[i];
        }
        return nullptr;
    }

    Driver* findAvailableDriver() {
        for (int i = 0; i < driverCount; i++) {
            if (drivers[i] && drivers[i]->getStatus() == DriverStatus::AVAILABLE) return drivers[i];
        }
        return nullptr;
    }

    /* ========================================================================
     * METHOD: registerRider
     * WORK:
     * Takes rider input from console and adds a new Rider object to the array.
     * ======================================================================== */
    void registerRider() {
        if (riderCount >= MAX_RIDERS) {
            cout << Colors::RED << "Maximum riders reached!\n" << Colors::RESET;
            return;
        }
        char name[64], phone[32];
        cout << "\nEnter Rider Name: ";
        cin.getline(name, sizeof(name));
        cout << "Enter Phone Number: ";
        cin.getline(phone, sizeof(phone));

        locMgr.displayLocations();
        int srcChoice, destChoice;
        cout << "Select Pickup Location (1-" << locMgr.getCount() << "): ";
        cin >> srcChoice;
        cout << "Select Destination (1-" << locMgr.getCount() << "): ";
        cin >> destChoice;
        cin.ignore(1000, '\n');

        const char* src = locMgr.getLocationAt(srcChoice - 1);
        const char* dest = locMgr.getLocationAt(destChoice - 1);

        char id[32];
        snprintf(id, sizeof(id), "R%d", nextRiderIdNum++);
        riders[riderCount++] = new Rider(id, name, phone, src, dest, 1);
        saveData();

        cout << Colors::GREEN << "\n✓ Rider registered successfully with ID: "
             << Colors::YELLOW << id << Colors::RESET << "\n";
    }

    /* ========================================================================
     * METHOD: registerDriver
     * WORK:
     * Registers a new Driver object into the drivers array.
     * ======================================================================== */
    void registerDriver() {
        if (driverCount >= MAX_DRIVERS) {
            cout << Colors::RED << "Maximum drivers reached!\n" << Colors::RESET;
            return;
        }
        char name[64], phone[32], vNum[32], vType[32];
        int seats;

        cout << "\nEnter Driver Name: ";
        cin.getline(name, sizeof(name));
        cout << "Enter Phone: ";
        cin.getline(phone, sizeof(phone));
        cout << "Enter Vehicle Number: ";
        cin.getline(vNum, sizeof(vNum));
        cout << "Enter Vehicle Type (Sedan/SUV/Hatchback): ";
        cin.getline(vType, sizeof(vType));
        cout << "Enter Available Seats: ";
        cin >> seats;
        cin.ignore(1000, '\n');

        char id[32];
        snprintf(id, sizeof(id), "D%d", nextDriverIdNum++);
        drivers[driverCount++] = new Driver(id, name, phone, vNum, vType, seats, DriverStatus::AVAILABLE);
        saveData();

        cout << Colors::GREEN << "\n✓ Driver registered successfully with ID: "
             << Colors::YELLOW << id << Colors::RESET << "\n";
    }

    /* ========================================================================
     * METHOD: bookSoloRide
     * WORK:
     * Creates a single-passenger private ride and assigns an available driver.
     * ======================================================================== */
    void bookSoloRide() {
        char riderId[32];
        cout << "\nEnter Rider ID (e.g. R101): ";
        cin.getline(riderId, sizeof(riderId));

        Rider* r = findRider(riderId);
        if (!r) {
            cout << Colors::RED << "Rider not found!\n" << Colors::RESET;
            return;
        }

        locMgr.displayLocations();
        int sIdx, dIdx;
        cout << "Select Pickup (1-" << locMgr.getCount() << "): "; cin >> sIdx;
        cout << "Select Destination (1-" << locMgr.getCount() << "): "; cin >> dIdx;
        cin.ignore(1000, '\n');

        const char* src = locMgr.getLocationAt(sIdx - 1);
        const char* dest = locMgr.getLocationAt(dIdx - 1);
        double dist = locMgr.getDistance(src, dest);

        Driver* drv = findAvailableDriver();
        const char* dId = drv ? drv->getId() : "None";

        char rideId[32];
        snprintf(rideId, sizeof(rideId), "R%d", nextRideIdNum++);

        SoloRide* ride = new SoloRide(rideId, r->getId(), src, dest, dist,
                                      drv ? RideStatus::CONFIRMED : RideStatus::REQUESTED, dId);
        rides[rideCount++] = ride;
        r->addRideToHistory(rideId);
        if (drv) drv->assignRide(rideId);
        saveData();

        ride->displayReceipt();
    }

    /* ========================================================================
     * METHOD: findSharedRide
     * WORK:
     * Uses pure C algorithms:
     * 1. Evaluates route scores (Source match = 40, Dest match = 40, Nearby = 20)
     * 2. Ranks candidates using pure C QuickSort (c_quicksort_candidates)
     * 3. Pools riders and applies 50% fare splitting
     * ======================================================================== */
    void findSharedRide() {
        char riderId[32];
        cout << "\nEnter Your Rider ID: ";
        cin.getline(riderId, sizeof(riderId));

        Rider* currentRider = findRider(riderId);
        if (!currentRider) {
            cout << Colors::RED << "Rider not found!\n" << Colors::RESET;
            return;
        }

        locMgr.displayLocations();
        int sIdx, dIdx;
        cout << "Select Pickup (1-" << locMgr.getCount() << "): "; cin >> sIdx;
        cout << "Select Destination (1-" << locMgr.getCount() << "): "; cin >> dIdx;
        cin.ignore(1000, '\n');

        const char* src = locMgr.getLocationAt(sIdx - 1);
        const char* dest = locMgr.getLocationAt(dIdx - 1);
        double dist = locMgr.getDistance(src, dest);

        /* Evaluate candidate matches using C algorithm */
        struct CMatchCandidate candidates[MAX_RIDERS];
        int cCount = 0;

        SoloFare solo(dist);
        double fullFare = solo.calculateFare();
        SharedFare prospective(dist, 2);

        for (int i = 0; i < riderCount; i++) {
            Rider* other = riders[i];
            if (!other || strcmp(other->getId(), currentRider->getId()) == 0) continue;

            int score = 0;
            if (strcmp(src, other->getSource()) == 0) score += 40;
            else if (locMgr.areNearby(src, other->getSource())) score += 20;

            if (strcmp(dest, other->getDestination()) == 0) score += 40;
            else if (locMgr.areNearby(dest, other->getDestination())) score += 20;

            if (score >= 40) {
                struct CMatchCandidate& cand = candidates[cCount++];
                strncpy(cand.riderId, other->getId(), 31);
                cand.riderId[31] = '\0';
                strncpy(cand.source, other->getSource(), 63);
                cand.source[63] = '\0';
                strncpy(cand.destination, other->getDestination(), 63);
                cand.destination[63] = '\0';
                cand.score = score;
                cand.distanceKm = dist;
                cand.originalFare = fullFare;
                cand.sharedFare = prospective.calculateFare();
                cand.savings = prospective.calculateSavings();
            }
        }

        if (cCount == 0) {
            cout << Colors::YELLOW << "\nNo compatible riders found for pooling right now.\n" << Colors::RESET;
            cout << "Would you like to book a Solo Ride instead? (y/n): ";
            char ans; cin >> ans; cin.ignore(1000, '\n');
            if (ans == 'y' || ans == 'Y') {
                Driver* drv = findAvailableDriver();
                char rId[32];
                snprintf(rId, sizeof(rId), "R%d", nextRideIdNum++);
                SoloRide* ride = new SoloRide(rId, currentRider->getId(), src, dest, dist,
                                              drv ? RideStatus::CONFIRMED : RideStatus::REQUESTED,
                                              drv ? drv->getId() : "None");
                rides[rideCount++] = ride;
                currentRider->addRideToHistory(rId);
                if (drv) drv->assignRide(rId);
                saveData();
                ride->displayReceipt();
            }
            return;
        }

        /* Pure C Divide-and-Conquer QuickSort to rank matches by score descending */
        c_quicksort_candidates(candidates, 0, cCount - 1);

        cout << "\n" << Colors::BOLD << "Top Compatible Carpool Matches Found:" << Colors::RESET << "\n";
        cout << "--------------------------------------------------------------------------------\n";
        for (int i = 0; i < cCount; i++) {
            cout << " [" << (i + 1) << "] Rider: " << Colors::YELLOW << candidates[i].riderId << Colors::RESET
                 << " | Route: " << candidates[i].source << " -> " << candidates[i].destination
                 << " | Match Score: " << Colors::GREEN << candidates[i].score << "/80" << Colors::RESET
                 << " | Shared Fare: " << Colors::CYAN << "₹" << candidates[i].sharedFare << Colors::RESET
                 << " (Save ₹" << candidates[i].savings << ")\n";
        }
        cout << "--------------------------------------------------------------------------------\n";

        cout << "Select match number to confirm shared ride (0 to cancel): ";
        int choice; cin >> choice; cin.ignore(1000, '\n');
        if (choice < 1 || choice > cCount) return;

        char rIds[2][32];
        strncpy(rIds[0], currentRider->getId(), 31);
        strncpy(rIds[1], candidates[choice - 1].riderId, 31);

        Driver* drv = findAvailableDriver();
        char newRideId[32];
        snprintf(newRideId, sizeof(newRideId), "R%d", nextRideIdNum++);

        SharedRide* pooledRide = new SharedRide(newRideId, rIds, 2, src, dest, dist,
                                                drv ? RideStatus::CONFIRMED : RideStatus::REQUESTED,
                                                drv ? drv->getId() : "None");
        rides[rideCount++] = pooledRide;
        currentRider->addRideToHistory(newRideId);
        Rider* matchedRider = findRider(candidates[choice - 1].riderId);
        if (matchedRider) matchedRider->addRideToHistory(newRideId);
        if (drv) drv->assignRide(newRideId);
        saveData();

        pooledRide->displayReceipt();
    }

    /* Display All Rides */
    void displayAllRides() const {
        cout << "\n" << Colors::CYAN << "================================================================================\n" << Colors::RESET;
        cout << Colors::BOLD << "                             ALL ACTIVE & LOGGED RIDES                          \n" << Colors::RESET;
        cout << Colors::CYAN << "================================================================================\n" << Colors::RESET;
        if (rideCount == 0) {
            cout << " No rides booked yet.\n";
        } else {
            for (int i = 0; i < rideCount; i++) {
                if (rides[i]) rides[i]->displayDetails();
            }
        }
        cout << Colors::CYAN << "================================================================================\n" << Colors::RESET;
    }

    /* Complete a Ride */
    void completeRide() {
        char rId[32];
        cout << "\nEnter Ride ID to Complete: ";
        cin.getline(rId, sizeof(rId));
        for (int i = 0; i < rideCount; i++) {
            if (rides[i] && strcmp(rides[i]->getRideId(), rId) == 0) {
                rides[i]->setStatus(RideStatus::COMPLETED);
                for (int d = 0; d < driverCount; d++) {
                    if (drivers[d] && strcmp(drivers[d]->getId(), rides[i]->getDriverId()) == 0) {
                        drivers[d]->completeRide();
                    }
                }
                saveData();
                cout << Colors::GREEN << "✓ Ride " << rId << " marked as COMPLETED!\n" << Colors::RESET;
                return;
            }
        }
        cout << Colors::RED << "Ride ID not found!\n" << Colors::RESET;
    }

    /* View Profiles */
    void viewRider() {
        char id[32];
        cout << "\nEnter Rider ID: ";
        cin.getline(id, sizeof(id));
        Rider* r = findRider(id);
        if (r) r->displayProfile();
        else cout << Colors::RED << "Rider not found!\n" << Colors::RESET;
    }

    void viewDriver() {
        char id[32];
        cout << "\nEnter Driver ID: ";
        cin.getline(id, sizeof(id));
        for (int i = 0; i < driverCount; i++) {
            if (drivers[i] && strcmp(drivers[i]->getId(), id) == 0) {
                drivers[i]->displayProfile();
                return;
            }
        }
        cout << Colors::RED << "Driver not found!\n" << Colors::RESET;
    }

    /* Interactive Menu Loop */
    void run() {
        int choice = 0;
        while (choice != 9) {
            cout << "\n" << Colors::CYAN << "╔════════════════════════════════════════════════════════════╗\n" << Colors::RESET;
            cout << Colors::BOLD << "║        SMARTRIDE: INTELLIGENT ROUTE-POOLING SYSTEM         ║\n" << Colors::RESET;
            cout << Colors::CYAN << "╠════════════════════════════════════════════════════════════╣\n" << Colors::RESET;
            cout << "║  1. Register New Rider                                     ║\n";
            cout << "║  2. Register New Driver                                    ║\n";
            cout << "║  3. Book Solo Ride                                         ║\n";
            cout << "║  4. Find & Book Shared Ride (Route-Pooling & QuickSort)    ║\n";
            cout << "║  5. View Rider Profile                                     ║\n";
            cout << "║  6. View Driver Profile                                    ║\n";
            cout << "║  7. Display All Rides                                      ║\n";
            cout << "║  8. Complete a Ride                                        ║\n";
            cout << "║  9. Save & Exit                                            ║\n";
            cout << Colors::CYAN << "╚════════════════════════════════════════════════════════════╝\n" << Colors::RESET;
            cout << "Enter your choice (1-9): ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(1000, '\n');
                continue;
            }
            cin.ignore(1000, '\n');

            switch (choice) {
                case 1: registerRider(); break;
                case 2: registerDriver(); break;
                case 3: bookSoloRide(); break;
                case 4: findSharedRide(); break;
                case 5: viewRider(); break;
                case 6: viewDriver(); break;
                case 7: displayAllRides(); break;
                case 8: completeRide(); break;
                case 9:
                    saveData();
                    cout << Colors::GREEN << "\nAll data persisted to text files. Exiting SmartRide. Goodbye!\n" << Colors::RESET;
                    break;
                default:
                    cout << Colors::RED << "Invalid option! Try again.\n" << Colors::RESET;
            }
        }
    }
};

/* ============================================================================
 * FUNCTION: main
 * WORK:
 * Program entry point. Instantiates the RideSharingSystem and starts execution.
 * ============================================================================ */
int main() {
    RideSharingSystem system;
    system.run();
    return 0;
}
