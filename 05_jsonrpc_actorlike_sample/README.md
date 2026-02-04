# Real-Life Example: Thread-Safe JSON-RPC Client & Server

This directory contains a practical example of a multi-threaded C++ application implementing a JSON-RPC 2.0 client-server architecture. The project demonstrates how to handle shared resources (sockets), thread synchronization, and modern C++ concurrency primitives in a real-world scenario.

## Architecture Overview

### 1. The Client (`01_jsonrpc_client.cpp`)
**Goal:** Create a client that can be safely used by multiple threads simultaneously, despite the underlying network socket being a non-thread-safe resource that must be accessed sequentially.

**Key Design Choices:**

*   **Worker Thread & Message Queue:**
    *   To prevent race conditions on the socket, we use the **Actor-like pattern**. A single internal background thread (`processRequests`) owns the socket.
    *   All external threads calling `request()` push their data into a thread-safe `std::queue` protected by a `std::mutex` and `std::condition_variable`.
    *   This effectively **serializes** all network traffic, ensuring that requests and responses do not interleave corruptly on the wire.

*   **Synchronization via `std::promise` and `std::future`:**
    *   The `request()` function is designed to be **synchronous/blocking** for the caller.
    *   We use a `std::promise<std::optional<int>>` inside the `Request` structure.
    *   **Flow:**
        1.  Caller thread creates a promise.
        2.  Caller pushes request (data + promise) to queue.
        3.  Caller blocks on `future.get()`.
        4.  Worker thread processes request, gets response from server.
        5.  Worker thread sets the value on the promise, waking up the caller.
    *   **Why?** This decouples the processing logic from the waiting logic and avoids complex manual condition variable management for every single request.

*   **JSON-RPC 2.0:**
    *   We use the standard JSON-RPC 2.0 protocol format (`jsonrpc`, `method`, `params`, `id`).
    *   Library: `nlohmann/json` is used for robust parsing and serialization.

### 2. The Server (`01_jsonrpc_server.cpp`)
**Goal:** A simple multithreaded server capable of handling multiple concurrent client connections.

**Key Design Choices:**

*   **Thread-Per-Client Model:**
    *   For simplicity and clarity, the server spawns a new `std::thread` for every incoming connection (`accept`).
    *   `std::thread(handleClient, clientSocket).detach();`
    *   *Note: In a massive-scale production environment, an Async I/O (epoll/IO_uring/ASIO) model would be preferred, but thread-per-client is perfectly adequate for this CPU-bound example.*

*   **Logic (Mandelbrot Set):**
    *   The server performs a CPU-intensive task: calculating iterations for the Mandelbrot set based on coordinates provided by the client.
    *   This simulates a "compute service" where offloading work from client to server is beneficial.

## Dependencies

*   **nlohmann/json**: Used for JSON manipulation. The CMake configuration automatically fetches this dependency from GitHub if not found locally.
*   **pthreads**: Standard threading support.

## How to Build & Run

1.  **Build:**
    ```bash
    mkdir -p build && cd build
    cmake ..
    make 01_jsonrpc_server 01_jsonrpc_client
    ```

2.  **Run Server:**
    ```bash
    ./05_jsonrpc_actorlike_sample/01_jsonrpc_server
    ```

3.  **Run Client:**
    ```bash
    ./05_jsonrpc_actorlike_sample/01_jsonrpc_client
    ```
