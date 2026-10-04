#include <stdio.h>
#include <stdlib.h>

#define MAX 100
#define MAX_STEPS 500

typedef struct {
    int id;
    int type;
    int entry_time;
    int road_id;
    int destination;
    int active;
    int completed;
    int wait_time;
    int travel_time;
    int pos;
} Vehicle;

typedef struct {
    int id;
    int capacity;
    int threshold;
    int signal;
    int vehicle_count;
    int queue[MAX];
} Road;

typedef struct {
    int id;
    int incoming_count;
    int incoming[MAX];
    int outgoing_count;
    int outgoing[MAX];
} Intersection;

Vehicle vehicles[MAX];
Road roads[MAX];
Intersection intersections[MAX];

int T;
int total_roads;
int total_intersections;
int total_vehicles;

int completed = 0;
int total_waiting = 0;
int total_travel = 0;
int max_queue = 0;

int step_vehicle_ids[MAX][MAX];
int step_vehicle_types[MAX][MAX];
int step_vehicle_pos[MAX][MAX];
int step_vehicle_count[MAX][MAX];
int step_signal[MAX][MAX];

int step_intersection_count[MAX][MAX];
int step_intersection_vehicles[MAX][MAX][MAX];


int findVehicleIndex(int id)
{
    for (int i = 0; i < total_vehicles; i++) {
        if (vehicles[i].id == id)
            return i;
    }

    return -1;
}


int findIntersectionForRoad(int road_id)
{
    for (int i = 0; i < total_intersections; i++) {

        for (int j = 0; j < intersections[i].incoming_count; j++) {

            if (intersections[i].incoming[j] == road_id)
                return i;
        }
    }

    return -1;
}


int findNextRoad(int current_road, int destination)
{
    int ix = findIntersectionForRoad(current_road);

    if (ix < 0)
        return -1;

    /*
       If the destination road is directly connected
       to this intersection, use it.
    */

    for (int i = 0; i < intersections[ix].outgoing_count; i++) {

        if (intersections[ix].outgoing[i] == destination)
            return destination;
    }

    /*
       Otherwise choose the first outgoing road.
    */

    if (intersections[ix].outgoing_count > 0)
        return intersections[ix].outgoing[0];

    return -1;
}


void readInput()
{
    scanf("%d", &T);
    scanf("%d", &total_roads);
    scanf("%d", &total_intersections);

    if (T > MAX_STEPS)
        T = MAX_STEPS;

    if (total_roads > MAX)
        total_roads = MAX;

    if (total_intersections > MAX)
        total_intersections = MAX;


    for (int i = 0; i < total_roads; i++) {

        roads[i].id = i + 1;

        scanf("%d", &roads[i].capacity);
        scanf("%d", &roads[i].threshold);

        if (roads[i].capacity < 1)
            roads[i].capacity = 1;

        if (roads[i].capacity > MAX)
            roads[i].capacity = MAX;

        roads[i].vehicle_count = 0;
        roads[i].signal = 1;
    }


    for (int i = 0; i < total_intersections; i++) {

        intersections[i].id = i + 1;

        scanf("%d", &intersections[i].incoming_count);

        if (intersections[i].incoming_count > MAX)
            intersections[i].incoming_count = MAX;

        for (int j = 0; j < intersections[i].incoming_count; j++) {
            scanf("%d", &intersections[i].incoming[j]);
        }

        scanf("%d", &intersections[i].outgoing_count);

        if (intersections[i].outgoing_count > MAX)
            intersections[i].outgoing_count = MAX;

        for (int j = 0; j < intersections[i].outgoing_count; j++) {
            scanf("%d", &intersections[i].outgoing[j]);
        }
    }


    scanf("%d", &total_vehicles);

    if (total_vehicles > MAX)
        total_vehicles = MAX;


    for (int i = 0; i < total_vehicles; i++) {

        vehicles[i].id = i + 1;

        scanf("%d", &vehicles[i].type);
        scanf("%d", &vehicles[i].entry_time);
        scanf("%d", &vehicles[i].road_id);
        scanf("%d", &vehicles[i].destination);

        vehicles[i].active = 0;
        vehicles[i].completed = 0;
        vehicles[i].wait_time = 0;
        vehicles[i].travel_time = 0;
        vehicles[i].pos = 0;
    }
}


void addVehicles(int t)
{
    for (int i = 0; i < total_vehicles; i++) {

        if (vehicles[i].completed)
            continue;

        if (vehicles[i].active)
            continue;

        if (vehicles[i].entry_time != t)
            continue;

        int r = vehicles[i].road_id - 1;

        if (r < 0 || r >= total_roads)
            continue;

        if (roads[r].vehicle_count >= roads[r].capacity)
            continue;

        roads[r].queue[roads[r].vehicle_count] =
            vehicles[i].id;

        roads[r].vehicle_count++;

        vehicles[i].active = 1;
        vehicles[i].pos = 10;
    }
}


void updateSignals(int t)
{
    for (int r = 0; r < total_roads; r++) {

        /*
           Congested roads stay green longer.
        */

        if (roads[r].vehicle_count > roads[r].threshold) {

            roads[r].signal = 1;

        } else {

            /*
               Simple alternating signal behaviour.
               Every two timesteps the signal changes.
            */

            if ((t / 2) % 2 == 0)
                roads[r].signal = 1;
            else
                roads[r].signal = 0;
        }
    }
}


void removeFromRoad(int r, int queue_position)
{
    for (int i = queue_position;
         i < roads[r].vehicle_count - 1;
         i++) {

        roads[r].queue[i] =
            roads[r].queue[i + 1];
    }

    roads[r].vehicle_count--;
}


void moveVehicles(int t)
{
    for (int r = 0; r < total_roads; r++) {

        if (roads[r].vehicle_count <= 0)
            continue;


        /*
           Move every vehicle forward while the signal is green.
        */

        for (int j = 0; j < roads[r].vehicle_count; j++) {

            int vehicle_id = roads[r].queue[j];

            int v = findVehicleIndex(vehicle_id);

            if (v < 0)
                continue;


            vehicles[v].travel_time++;


            /*
               Red signal = vehicle waits.
            */

            if (roads[r].signal == 0) {

                vehicles[v].wait_time++;

                continue;
            }


            /*
               Green signal = vehicle moves.
            */

            vehicles[v].pos += 25;


            /*
               Vehicle reaches the end of the road.
            */

            if (vehicles[v].pos >= 100) {

                /*
                   If this road is the destination,
                   the vehicle is completed.
                */

                if (roads[r].id == vehicles[v].destination) {

                    vehicles[v].completed = 1;
                    vehicles[v].active = 0;

                    completed++;

                    total_waiting +=
                        vehicles[v].wait_time;

                    total_travel +=
                        vehicles[v].travel_time;

                    removeFromRoad(r, j);

                    j--;

                    continue;
                }


                /*
                   Otherwise try to move the vehicle
                   to the next outgoing road.
                */

                int nextRoad =
                    findNextRoad(
                        roads[r].id,
                        vehicles[v].destination
                    );


                if (nextRoad >= 1 &&
                    nextRoad <= total_roads) {

                    int nr = nextRoad - 1;


                    /*
                       Only move if the next road
                       has space.
                    */

                    if (roads[nr].vehicle_count <
                        roads[nr].capacity) {

                        removeFromRoad(r, j);

                        j--;

                        roads[nr].queue[
                            roads[nr].vehicle_count
                        ] = vehicles[v].id;

                        roads[nr].vehicle_count++;

                        vehicles[v].road_id =
                            nextRoad;

                        vehicles[v].pos = 0;

                        continue;
                    }
                }


                /*
                   If there is no valid next road,
                   keep the vehicle at the end.
                */

                vehicles[v].pos = 95;
            }
        }
    }
}


void updateMaxQueue()
{
    for (int i = 0; i < total_roads; i++) {

        if (roads[i].vehicle_count > max_queue)
            max_queue =
                roads[i].vehicle_count;
    }
}


void handleEmergency()
{
    for (int r = 0; r < total_roads; r++) {

        for (int j = 0; j < roads[r].vehicle_count; j++) {

            int id = roads[r].queue[j];

            int v = findVehicleIndex(id);

            if (v < 0)
                continue;

            if (vehicles[v].type != 1)
                continue;


            /*
               Move emergency vehicle to front.
            */

            int emergency_id =
                roads[r].queue[j];

            for (int k = j; k > 0; k--) {

                roads[r].queue[k] =
                    roads[r].queue[k - 1];
            }

            roads[r].queue[0] =
                emergency_id;


            /*
               Give its road a green signal.
            */

            roads[r].signal = 1;

            return;
        }
    }
}


void saveCurrentState(int step)
{
    if (step >= MAX_STEPS)
        return;


    for (int r = 0; r < total_roads; r++) {

        step_signal[step][r] =
            roads[r].signal;

        step_vehicle_count[step][r] =
            roads[r].vehicle_count;


        for (int j = 0;
             j < roads[r].vehicle_count;
             j++) {

            int id =
                roads[r].queue[j];

            int v =
                findVehicleIndex(id);

            if (v < 0)
                continue;

            step_vehicle_ids[step][r * MAX + j] =
                vehicles[v].id;

            step_vehicle_types[step][r * MAX + j] =
                vehicles[v].type;

            step_vehicle_pos[step][r * MAX + j] =
                vehicles[v].pos;
        }
    }


    /*
       Store vehicles at intersections.
    */

    for (int i = 0;
         i < total_intersections;
         i++) {

        step_intersection_count[step][i] = 0;

        for (int r = 0; r < total_roads; r++) {

            int ix =
                findIntersectionForRoad(
                    roads[r].id
                );

            if (ix != i)
                continue;


            /*
               A vehicle near the end of an incoming
               road is visually considered to be at
               the intersection.
            */

            for (int j = 0;
                 j < roads[r].vehicle_count;
                 j++) {

                int id =
                    roads[r].queue[j];

                int v =
                    findVehicleIndex(id);

                if (v < 0)
                    continue;

                if (vehicles[v].pos >= 90) {

                    int count =
                        step_intersection_count[
                            step][i];

                    if (count < MAX) {

                        step_intersection_vehicles[
                            step][i][count] =
                            vehicles[v].id;

                        step_intersection_count[
                            step][i]++;
                    }
                }
            }
        }
    }
}


void writeRoadJSON(FILE *file, int step, int r)
{
    fprintf(
        file,
        "{"
        "\"id\":%d,"
        "\"vehicleCount\":%d,"
        "\"signal\":%d,"
        "\"vehicles\":[",
        roads[r].id,
        step_vehicle_count[step][r],
        step_signal[step][r]
    );


    for (int j = 0;
         j < step_vehicle_count[step][r];
         j++) {

        int index =
            r * MAX + j;

        fprintf(
            file,
            "{"
            "\"id\":%d,"
            "\"type\":%d,"
            "\"pos\":%d"
            "}",
            step_vehicle_ids[step][index],
            step_vehicle_types[step][index],
            step_vehicle_pos[step][index]
        );

        if (j <
            step_vehicle_count[step][r] - 1)
            fprintf(file, ",");
    }


    fprintf(file, "]}");
}


void writeIntersectionJSON(
    FILE *file,
    int step,
    int i
)
{
    fprintf(
        file,
        "{"
        "\"id\":%d,"
        "\"vehicles\":[",
        intersections[i].id
    );


    for (int j = 0;
         j < step_intersection_count[step][i];
         j++) {

        fprintf(
            file,
            "%d",
            step_intersection_vehicles[
                step][i][j]
        );

        if (j <
            step_intersection_count[step][i] - 1)
            fprintf(file, ",");
    }


    fprintf(file, "]}");
}


void writeJSON()
{
    FILE *file =
        fopen("simulation.json", "w");

    if (!file)
        return;


    float avg_travel = 0;
    float avg_wait = 0;


    if (completed > 0) {

        avg_travel =
            (float)total_travel /
            completed;

        avg_wait =
            (float)total_waiting /
            completed;
    }


    fprintf(file, "{");


    fprintf(
        file,
        "\"metrics\":{"
        "\"total_completed\":%d,"
        "\"avg_travel_time\":%.1f,"
        "\"avg_stopped_time\":%.1f,"
        "\"max_queue_length\":%d"
        "},",
        completed,
        avg_travel,
        avg_wait,
        max_queue
    );


    fprintf(file, "\"steps\":[");


    for (int step = 0;
         step < T;
         step++) {

        fprintf(file, "{");


        fprintf(file, "\"roads\":[");


        for (int r = 0;
             r < total_roads;
             r++) {

            writeRoadJSON(
                file,
                step,
                r
            );

            if (r <
                total_roads - 1)
                fprintf(file, ",");
        }


        fprintf(file, "],");


        fprintf(
            file,
            "\"intersections\":["
        );


        for (int i = 0;
             i < total_intersections;
             i++) {

            writeIntersectionJSON(
                file,
                step,
                i
            );

            if (i <
                total_intersections - 1)
                fprintf(file, ",");
        }


        fprintf(file, "]");


        fprintf(file, "}");


        if (step < T - 1)
            fprintf(file, ",");
    }


    fprintf(file, "]");


    fprintf(file, "}");


    fclose(file);
}


int main()
{
    readInput();


    /*
       Run the simulation for every timestep.
    */

    for (int t = 0; t < T; t++) {

        addVehicles(t);

        handleEmergency();

        updateSignals(t);

        moveVehicles(t);

        updateMaxQueue();

        saveCurrentState(t);
    }


    writeJSON();


    return 0;
}