# Traffic Simulation 🚦

A small traffic simulation project built using C, Python, HTML, CSS and JavaScript.

### What it does

The simulation includes:

* Vehicle movement across roads and intersections
* Traffic signal control
* Congestion handling
* Emergency vehicle priority
* Vehicle queues
* Destination-based routing
* Simulation statistics like travel time, waiting time and maximum queue length

### How it works

`frontend.html` → `connector.py` → `backend.c` → `simulation.json` → `frontend.html`

The frontend takes the simulation inputs, Python connects the frontend to the C backend, and the C program runs the simulation and generates the results.

### Running it locally

Compile the C backend:

```bash
gcc backend.c -o traffic_engine.exe
```

Start the Python server:

```bash
python connector.py
```

Then open:

```text
http://localhost:8080/plantain.html
```

### Project Files

* `frontend.html` – frontend and visualization
* `backend.c` – traffic simulation
* `connector.py` – connects the frontend and C backend
* `simulation.json` – simulation output
