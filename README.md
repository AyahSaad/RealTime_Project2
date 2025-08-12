# 🛒 Supermarket Simulation System
This project simulates the operations of a supermarket using multi-processing, shared memory, message queues, POSIX threads, signals, and OpenGL for visualization. The system manages products, customers, shelving teams, and inventory control in real-time.

# 📦 Overview
The system consists of multiple cooperating processes:

Shelving Teams: Restock products from the queue.

Customer Simulator: Simulates customer arrivals who purchase items.

Main Process: Initializes shared resources, creates processes, and runs the OpenGL visualization.

Inventory Manager: Controls stock and handles termination on events like out-of-stock or time expiration.

# 🧠 Features
🧾 Reads Configuration and Product Files

🧍‍♂️ Simulates Customer Arrivals

🔁 Handles Interprocess Communication (IPC) using:

Shared Memory

Message Queues

Signals

# 📦 Tracks Stock & Replenishment

❗ Handles Critical Events:

Time expiration (SIGALRM)

Out-of-stock (SIGUSR1)

🧵 Uses POSIX Threads & Mutexes for shared memory synchronization

🖥️ Renders Real-time Visualization using OpenGL

# 🛠️ Technologies Used
C (POSIX-compliant)

IPC: Shared Memory, Message Queues, Signals

PThreads

OpenGL (GLUT)

Linux System Programming APIs

# ⚙️ How It Works
# 🧾 Initialization
Reads configuration from arguments.txt

Reads product inventory from products.txt

Initializes shared memory segments and message queues

Sets alarm for time-limited simulation (e.g., 60 seconds)

# 👷 Process Creation
Main parent forks:

Multiple Shelving Team processes

One Customer Arrival process

Launches OpenGL GUI for visualization

# 🔁 Communication & Control
Customers send product requests via message queue

Teams restock products using shared memory

Inventory is tracked, and termination occurs on:

Out-of-stock event → SIGUSR1

Timeout → SIGALRM

# ▶️ How to Run
🧰 Prerequisites
Linux OS
GCC Compiler

OpenGL & GLUT libraries installed:
sudo apt-get install freeglut3-dev

🏗️ Build & Run

gcc main.c -o supermarket -lGL -lGLU -lglut -lpthread
./supermarket
