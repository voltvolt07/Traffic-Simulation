#include <stdio.h>

#define MAX 100

// -------- STRUCTS --------
struct Vehicle {
    int id;
    int entry_time;
    int road_id;
    int type; //1-emergency 0-regular
};

struct Road {
    int id;
    int capacity;
    int vehicle_count;
    int queue[MAX];
    int signal; // 0 = RED, 1 = GREEN
    int congestion_threshold;
    int signal_timer;
    int green_duration;
    int red_duration;
};

// -------- GLOBAL VARIABLES --------
struct Vehicle vehicles[MAX];
struct Road roads[MAX];

int total_vehicles;
int total_roads;
int T;
int total_completed = 0;
int total_waiting_time = 0;
int max_queue = 0;

// -------- INITIALIZATION --------
void initializeSimulation() {

    printf("Enter total time steps: ");
    scanf("%d", &T);

    printf("Enter number of roads: ");
    scanf("%d", &total_roads);

    // Initialize roads
    for (int i = 0; i < total_roads; i++) {
        roads[i].id = i + 1;
        roads[i].vehicle_count = 0;
        roads[i].signal_timer=0;

        printf("Enter capacity of Road %d: ", i + 1);
        scanf("%d", &roads[i].capacity);
        printf("Enter congestion threshold for Road %d: ", i + 1);
        scanf("%d", &roads[i].congestion_threshold);
        roads[i].signal = 1; // start with GREEN
        roads[i].green_duration = 1;
        roads[i].red_duration = 1;
    }

    printf("Enter number of vehicles: ");
    scanf("%d", &total_vehicles);

    // Initialize vehicles
    for (int i = 0; i < total_vehicles; i++) {
        vehicles[i].id = i + 1;

        printf("Enter id for Vehicle %d: ", i + 1);
        scanf("%d", &vehicles[i].id);

        printf("Enter type of Vehicle %d: ", i + 1);
        scanf("%d", &vehicles[i].type);

        printf("Enter entry time for Vehicle %d: ", i + 1);
        scanf("%d", &vehicles[i].entry_time);

        printf("Enter road number for Vehicle %d: ", i + 1);
        scanf("%d", &vehicles[i].road_id);




    }
}

// -------- ADD VEHICLES --------
void addVehicles(int t) {

    for (int i = 0; i < total_vehicles; i++) {

        if (vehicles[i].entry_time == t) {

            int r = vehicles[i].road_id - 1;

            if (roads[r].vehicle_count < roads[r].capacity) {

                roads[r].queue[roads[r].vehicle_count] = vehicles[i].id;
                roads[r].vehicle_count++;

                printf("Vehicle %d entered Road %d\n",
                       vehicles[i].id, roads[r].id);

            } else {
                printf("Road %d full! Vehicle %d waiting\n",
                       roads[r].id, vehicles[i].id);
            }
        }
    }
}

// -------- MOVE VEHICLES (FIFO) --------
void moveVehicles(int t) {

    for (int r = 0; r < total_roads; r++) {

        if (t > 0 && roads[r].vehicle_count > 0 && roads[r].signal == 1) {

            int leaving = roads[r].queue[0];

            printf("Vehicle %d left Road %d\n", leaving, roads[r].id);
            total_completed++;

            // find vehicle and calculate waiting time
            for (int v = 0; v < total_vehicles; v++) {
            if (vehicles[v].id == leaving) {
                total_waiting_time += (t - vehicles[v].entry_time);
                break;
    }
}

            // Shift queue
            for (int j = 0; j < roads[r].vehicle_count - 1; j++) {
                roads[r].queue[j] = roads[r].queue[j + 1];
            }

            roads[r].vehicle_count--;
        }

        if (roads[r].vehicle_count > max_queue) {
            max_queue = roads[r].vehicle_count;
        }
    }
}

void updateSignals(int t) {

    for (int r = 0; r < total_roads; r++) {

        // Step 1: Set durations based on congestion
        if (roads[r].vehicle_count > roads[r].congestion_threshold) {
            roads[r].green_duration = 3;
            roads[r].red_duration = 1;
        } else {
            roads[r].green_duration = 1;
            roads[r].red_duration = 2;
        }

        int total_cycle = roads[r].green_duration + roads[r].red_duration;

        // Step 2: Increment timer
        roads[r].signal_timer++;

        // Step 3: Reset timer if cycle completes
        if (roads[r].signal_timer >= total_cycle) {
            roads[r].signal_timer = 0;
        }

        // Step 4: Decide signal
        if (roads[r].signal_timer < roads[r].green_duration) {
            roads[r].signal = 1; // GREEN
        } else {
            roads[r].signal = 0; // RED
        }

        // Print signal
        printf("Road %d Signal: %s\n",
               roads[r].id,
               roads[r].signal ? "GREEN" : "RED");
    }
}

void updateCongestion() {

    for (int r = 0; r < total_roads; r++) {

        if (roads[r].vehicle_count > roads[r].congestion_threshold) {
            printf("Congestion on Road %d\n", roads[r].id);
        }
    }
}

void handleEmergency()
{
    for (int i = 0; i < total_roads; i++)
    {
        for (int j = 0; j < roads[i].vehicle_count; j++)
        {
            int vid = roads[i].queue[j];

            // Find this vehicle in vehicles array
            for (int v = 0; v < total_vehicles; v++)
            {
                if (vehicles[v].id == vid && vehicles[v].type == 1)
                {
                    int temp = roads[i].queue[j];

                    // Move emergency vehicle to front
                    for (int k = j; k > 0; k--)
                    {
                        roads[i].queue[k] = roads[i].queue[k - 1];
                    }

                    roads[i].queue[0] = temp;

                    // Set all roads RED
                    for (int r = 0; r < total_roads; r++)
                    {
                        roads[r].signal = 0;
                    }

                    // Set this road GREEN
                    roads[i].signal = 1;

                    printf(" Emergency vehicle prioritized on Road %d\n", roads[i].id);

                    return; // handle only one per timestep
                }
            }
        }
    }
}

void recordMetrics()
{
    printf("\n--- Final Metrics ---\n");

    printf("Total Vehicles Completed: %d\n", total_completed);

    if (total_completed > 0)
    {
        printf("Average Waiting Time: %.2f\n",
               (float)total_waiting_time / total_completed);
    }
    else
    {
        printf("Average Waiting Time: 0\n");
    }

    printf("Max Queue Length: %d\n", max_queue);
}

// -------- PRINT STATE --------
void printRoads() {

    for (int r = 0; r < total_roads; r++) {

        printf("Road %d: ", roads[r].id);

        for (int i = 0; i < roads[r].vehicle_count; i++) {
            printf("%d ", roads[r].queue[i]);
        }

        printf("\n");
    }
}

// -------- MAIN --------
int main() {

    initializeSimulation();

    for (int t = 0; t < T; t++) {

        printf("\n===== Time %d =====\n", t);
        updateSignals(t);
        addVehicles(t);
        handleEmergency();
        moveVehicles(t);
        updateCongestion();
        printRoads();
    }
    recordMetrics();

    return 0;
}