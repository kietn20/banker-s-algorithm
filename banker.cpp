// Compile with "g++ banker.cpp -o banker"
// Execute program with "./banker inputfile.txt"

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

using namespace ::std;

// The available amount of each resource
int available[NUMBER_OF_RESOURCES];

// The maximum demand of each customer
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// The amount currently allocated to each customer
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// The remaining need of each customer
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// Function Declarations
int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);
bool checking_safe_state();
void print_all_data_structures();

int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        cout << "Please provide an input.txt file that contain 5 rows of 4 numbers separated by commas.\n";
        exit(1);
    }

    ifstream inputFile(argv[1]);
    if (!inputFile)
    {
        cerr << "Failed to open the input file.";
        exit(1);
    }

    string line;
    int row = 0;
    while (getline(inputFile, line) && row < 5)
    {
        stringstream ss(line);
        string cell;
        int col = 0;

        // Split the line by commas and parse each number
        while (getline(ss, cell, ',') && col < 4)
        {
            maximum[row][col] = stoi(cell);
            col++;
        }
        row++;
    }

    // Intializing Available resources
    cout << "Enter the avaiable resources: ";
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        scanf("%d", &available[i]);
    }

    // Initializing allocation and need matrices
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
        {
            allocation[i][j] = 0;
            need[i][j] = maximum[i][j];
        }
    }

    int request[NUMBER_OF_RESOURCES];
    int release[NUMBER_OF_RESOURCES];
    string choice;
    int customer;

    while (1)
    {
        cout << "Enter a choice (RQ, RL, *): ";
        cin >> choice;

        if (choice == "RQ")
        {
            cout << ("Enter the customer number (0, 1, 2, 3, 4): ");
            scanf("%d", &customer);
            cout << ("Enter the resource request: ");
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
                scanf("%d", &request[i]);

            bool is_satisfied = request_resources(customer, request);

            if (is_satisfied)
                cout << ("Request granted.\n");
            else
                cout << ("Request denied.\n");
        }
        else if (choice == "RL")
        {
            cout << ("Enter the customer number: ");
            scanf("%d", &customer);
            cout << ("Enter the resource release: ");
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
                scanf("%d", &release[i]);

            release_resources(customer, release);
            cout << ("Resources released.\n");
        }
        else if (choice == "*")
        {
            print_all_data_structures();
        }
        else
        {
            cout << ("Please only enter valid choices (RQ, RL, *). Try Again.\n");
        }
    }

    return 0;
}

int request_resources(int customer_num, int request[])
{
    // Checking if request is exceedding the need or avaiable resouces
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        if (request[i] > need[customer_num][i] || request[i] > available[i])
        {
            cout << "Request sequence is larger than customer's needs or availability. ";
            return false;
        }
    }

    // Allocating tempororay resources for testing using Banker's Algorithm
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    // Checking if state if safe after allocating
    bool is_safe = checking_safe_state();

    // Resetting allocation if the state is not safe
    if (!is_safe)
    {
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        {
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        cout << "Fails Banker's Algorithm. Not Safe.\n";
    }

    return is_safe;
}

void release_resources(int customer_num, int release[])
{
    // Release the allocated resources
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        available[i] += release[i];
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i];
    }
}

void print_all_data_structures()
{
    cout << ("\nAvailable resources: [");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        printf("%d ", available[i]);
    cout << "\b]\n";

    printf("\nMaximum resources:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        cout << "[";
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
            printf("%d ", maximum[i][j]);
        cout << "\b]\n";
    }

    printf("\nAllocation:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        cout << "[";
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
            printf("%d ", allocation[i][j]);
        cout << "\b]\n";
    }

    printf("\nNeed:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        cout << "[";
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
            printf("%d ", need[i][j]);
        cout << "\b]\n";
    }
}

bool checking_safe_state()
{
    bool customer_satisfied[NUMBER_OF_CUSTOMERS] = {false};
    int tempAvailable[NUMBER_OF_RESOURCES];

    // Initialize temporary array with available resources
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        tempAvailable[i] = available[i];

    // Find an unfinished customer with need <= work
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        if (!customer_satisfied[i])
        {
            bool allocated = true;
            for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
            {
                if (need[i][j] > tempAvailable[j])
                {
                    allocated = false;
                    break;
                }
            }

            if (allocated)
            {
                // allocate resources to customer i
                for (int k = 0; k < NUMBER_OF_RESOURCES; k++)
                {
                    tempAvailable[k] += allocation[i][k];
                }

                customer_satisfied[i] = true;
                i = -1; // reset the loop to check on if request can satisfy any previous customers
            }
        }
    }

    // Check if all customers finished
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        if (!customer_satisfied[i])
            return false;
    }

    return true;
}