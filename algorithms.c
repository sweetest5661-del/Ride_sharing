/**
 * ============================================================================
 * FILE: algorithms.c
 * LANGUAGE: Pure C (C99/C11 Standard)
 *
 * PURPOSE:
 * This file contains low-level algorithmic operations written in pure C:
 * 1. String Sanitization & Case-Insensitive Comparison (Pointer manipulation)
 * 2. Search Algorithms (Linear Search & Binary Search)
 * 3. Divide-and-Conquer QuickSort (Ranking ride-share match candidates)
 * 4. Candidate Array Filtering (Selecting eligible route-sharing candidates)
 * ============================================================================
 */

#include <ctype.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * DATA STRUCTURE: CMatchCandidate
 * WORK:
 * Stores candidate match data during route-pooling evaluation so that
 * pure C sorting and filtering algorithms can process them efficiently.
 * ============================================================================ */
struct CMatchCandidate {
    char rideId[32];        /* Unique ID of the existing ride */
    char riderId[32];       /* Unique ID of the passenger */
    char source[64];        /* Pickup location */
    char destination[64];   /* Drop-off destination */
    int score;              /* Route compatibility score (0 to 80 points) */
    double distanceKm;      /* Trip distance in kilometers */
    double originalFare;    /* Full solo fare for this distance */
    double sharedFare;      /* Discounted split fare per rider */
    double savings;         /* Amount of money saved by sharing */
};

/* ============================================================================
 * FUNCTION: c_str_trim
 * WORK:
 * Removes leading and trailing whitespace characters (spaces, tabs, newlines)
 * directly in-place within the string using pointer movement.
 * ============================================================================ */
void c_str_trim(char* str) {
    if (!str) return;

    /* Step 1: Advance pointer past any leading whitespace */
    char* start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    /* Step 2: Shift characters back to the beginning of the string buffer */
    if (start != str) {
        char* dst = str;
        while (*start) {
            *dst++ = *start++;
        }
        *dst = '\0';
    }

    /* Step 3: Walk backwards from the end and remove trailing whitespace */
    int len = (int)strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
}

/* ============================================================================
 * FUNCTION: c_str_to_upper
 * WORK:
 * Converts every character in a null-terminated string to uppercase.
 * Useful for case-insensitive matching of commands, statuses, or IDs.
 * ============================================================================ */
void c_str_to_upper(char* str) {
    if (!str) return;
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = (char)toupper((unsigned char)str[i]);
    }
}

/* ============================================================================
 * FUNCTION: c_str_case_cmp
 * WORK:
 * Compares two C-strings character by character without case sensitivity.
 * Returns:
 *   0  if both strings are identical (ignoring case)
 *  < 0 if s1 is alphabetically before s2
 *  > 0 if s1 is alphabetically after s2
 * ============================================================================ */
int c_str_case_cmp(const char* s1, const char* s2) {
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;

    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

/* ============================================================================
 * FUNCTION: c_linear_search_int
 * WORK:
 * Iterates sequentially through an array of integers looking for 'target'.
 * Returns the index if found, or -1 if not found.
 * ============================================================================ */
int c_linear_search_int(const int arr[], int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) {
            return i;
        }
    }
    return -1;
}

/* ============================================================================
 * FUNCTION: c_binary_search_int
 * WORK:
 * Performs divide-and-conquer binary search on an already sorted integer array.
 * Halves the search space in each step. Returns index if found, or -1 if not.
 * ============================================================================ */
int c_binary_search_int(const int arr[], int size, int target) {
    int low = 0;
    int high = size - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

/* ============================================================================
 * HELPER FUNCTION: swap_candidates
 * WORK:
 * Swaps two CMatchCandidate records in memory during QuickSort partitioning.
 * ============================================================================ */
static void swap_candidates(struct CMatchCandidate* a, struct CMatchCandidate* b) {
    struct CMatchCandidate temp = *a;
    *a = *b;
    *b = temp;
}

/* ============================================================================
 * HELPER FUNCTION: partition
 * WORK:
 * QuickSort partition step configured for DESCENDING order (highest match
 * score and highest savings placed at the beginning of the array).
 * ============================================================================ */
static int partition(struct CMatchCandidate arr[], int low, int high) {
    int pivotScore = arr[high].score;
    double pivotSavings = arr[high].savings;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        /* Condition: Higher score comes first. If scores are tied, higher savings comes first. */
        if (arr[j].score > pivotScore || 
           (arr[j].score == pivotScore && arr[j].savings > pivotSavings)) {
            i++;
            swap_candidates(&arr[i], &arr[j]);
        }
    }
    swap_candidates(&arr[i + 1], &arr[high]);
    return i + 1;
}

/* ============================================================================
 * FUNCTION: c_quicksort_candidates
 * WORK:
 * Classic Divide-and-Conquer QuickSort algorithm. Recursively sorts candidate
 * records by score in descending order so the best ride-share match is shown first.
 * ============================================================================ */
void c_quicksort_candidates(struct CMatchCandidate arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        c_quicksort_candidates(arr, low, pi - 1);
        c_quicksort_candidates(arr, pi + 1, high);
    }
}

/* ============================================================================
 * FUNCTION: c_filter_candidates
 * WORK:
 * Iterates through all potential candidates and copies only those whose score
 * meets or exceeds 'minScore' into the output array. Returns filtered count.
 * ============================================================================ */
int c_filter_candidates(const struct CMatchCandidate inArr[], int inCount, 
                         struct CMatchCandidate outArr[], int minScore) {
    int count = 0;
    for (int i = 0; i < inCount; i++) {
        if (inArr[i].score >= minScore) {
            outArr[count++] = inArr[i];
        }
    }
    return count;
}

#ifdef __cplusplus
}
#endif

#ifndef __cplusplus
#include <stdio.h>

/* ============================================================================
 * FUNCTION: main (Standalone C Algorithms Test Runner)
 * WORK:
 * Allows algorithms.c to be compiled and run directly in the terminal
 * (e.g. gcc algorithms.c && ./a.out). It demonstrates and tests all C algorithms:
 * 1. String trimming
 * 2. Case-insensitive comparison
 * 3. Linear & Binary search
 * 4. QuickSort candidate ranking
 * ============================================================================ */
int main(void) {
    printf("\n========================================================\n");
    printf("        SMARTRIDE PURE C ALGORITHMS TEST SUITE          \n");
    printf("========================================================\n\n");

    /* 1. Test In-Place String Trim */
    char testStr[64] = "   Clock Tower Dehradun   ";
    printf("[1] String Trim Algorithm:\n");
    printf("    Before: '%s'\n", testStr);
    c_str_trim(testStr);
    printf("    After : '%s'\n\n", testStr);

    /* 2. Test Case-Insensitive String Comparison */
    const char* city1 = "ISBT";
    const char* city2 = "isbt";
    printf("[2] Case-Insensitive String Comparison:\n");
    printf("    Comparing '%s' and '%s' -> %s\n\n",
           city1, city2,
           c_str_case_cmp(city1, city2) == 0 ? "MATCH (Identical)" : "NO MATCH");

    /* 3. Test Searching Algorithms */
    int sampleArr[] = {10, 20, 30, 40, 50, 60};
    int n = sizeof(sampleArr) / sizeof(sampleArr[0]);
    printf("[3] Search Algorithms:\n");
    printf("    Linear Search for 30 in array -> Found at index %d\n", c_linear_search_int(sampleArr, n, 30));
    printf("    Binary Search for 50 in array -> Found at index %d\n\n", c_binary_search_int(sampleArr, n, 50));

    /* 4. Test QuickSort Candidate Ranking (Descending order by score and savings) */
    printf("[4] Divide-and-Conquer QuickSort Candidate Ranking:\n");
    struct CMatchCandidate candidates[3] = {
        {"R101", "Rider_A", "Rajpur Road", "ISBT", 60, 12.0, 170.0, 85.0, 85.0},
        {"R102", "Rider_B", "Clock Tower", "ISBT", 80, 8.0, 130.0, 65.0, 65.0},
        {"R103", "Rider_C", "Prem Nagar", "ISBT", 40, 9.5, 145.0, 72.5, 72.5}
    };

    printf("    Before QuickSort:\n");
    for (int i = 0; i < 3; i++) {
        printf("      - %s: Score %d/80 (Savings: Rs.%.0f)\n", candidates[i].riderId, candidates[i].score, candidates[i].savings);
    }

    c_quicksort_candidates(candidates, 0, 2);

    printf("    After QuickSort (Highest compatibility first):\n");
    for (int i = 0; i < 3; i++) {
        printf("      [%d] %s: Score %d/80 (Savings: Rs.%.0f)\n", i + 1, candidates[i].riderId, candidates[i].score, candidates[i].savings);
    }

    printf("\n[SUCCESS] All pure C algorithms executed successfully!\n\n");
    return 0;
}
#endif


