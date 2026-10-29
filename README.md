<img width="1382" height="837" alt="Screenshot 2026-10-29 211057" src="https://github.com/user-attachments/assets/9561be47-d76c-447c-9e04-b6531958f3b9" />
# Roti Counter: Producer-Consumer Problem

A wedding tandoori roti counter used to explain the producer-consumer problem.

| Wedding | Concept |
|---|---|
| Chefs making rotis | Producers |
| Guests waiting at the counter | Consumers |
| Plate holding 5 rotis | Bounded buffer |

## Files
- `roti_counter.c`: POSIX threads solution using two semaphores (`empty_slots`, `full_slots`) and a mutex.
- `index.html`: interactive visual simulator (open in any browser, no install needed).

## Run the C program
```
gcc -Wall -o roti_counter roti_counter.c -pthread
./roti_counter
```
Needs Linux, WSL or similar (macOS does not support `sem_init`).

## Run the simulator
Open `index.html` in a browser. Once GitHub Pages is enabled it is also available online.

## How it stays correct
- `empty_slots` (starts at the plate size): a chef waits when the plate is full.
- `full_slots` (starts at 0): a guest waits when no roti is ready.
- `plate_lock`: only one person touches the plate at a time.
