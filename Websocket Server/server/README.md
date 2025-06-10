# WebSocket Server 
This server provides real-time updates to clients subscribed to different trading symbols, specifically broadcasting changes in order book data. Using WebSocket technology, the server ensures faster data delivery and responsiveness to market changes. It leverages the `websocketpp` library for WebSocket communication and `nlohmann/json` for JSON parsing.

# Approach
The server is designed to handle high-frequency updates and efficiently manage multiple client connections across different insturments. Key components include:

- **Subscription Management**: An `unordered_map` data structure (`subscribers`) stores clients subscribed to each trading symbol, using the symbol as a key and associating it with a list of clients (or `connection_hdl` handles).
- **Dynamic Order Book Updates**: The server listens to the Deribit WebSocket API and updates order books whenever new data is available, enhancing speed, reliability and accuracy.
- **Targeted Broadcasts**: When an order book update arrives from deribit, it is immediately sent to all clients subscribed to the corresponding symbol, ensuring they receive the latest data instantly.

Using WebSockets enables a persistent, bidirectional connection between the server and clients, reducing the latency of receiving order book data. This is ideal for financial applications where market changes need to be reflected in real time. By directly subscribing to Deribit’s WebSocket API, we bypass the delay of repeated HTTP requests and instead receive data as soon as it is available.

# Main Functions

### `broadcastOrderBookUpdate`
Broadcasts the latest order book data to all clients subscribed to a specific symbol. This function loops through each connection handle in `subscribers[symbol]` and sends the current order book data stored in `orderBooks[symbol]`.

### `on_deribit_message`
This function handles messages received from Deribit’s WebSocket feed. When new data arrives, the function extracts the symbol and relevant order book details, updates `orderBooks`, and triggers `broadcastOrderBookUpdate` to push the update to all subscribers.

### `on_message`
Handles incoming messages from clients. When a client subscribes to a symbol, their connection handle is added to the subscriber list for that symbol, allowing the server to target updates.

### `on_open` and `on_close`
Manage new connections and clean up closed connections, respectively. These functions ensure that resources are allocated efficiently and that closed connections do not receive updates.

# Dependencies
- **CURL**: A fast and lightweight library used for making HTTP requests to the trading API. It supports various protocols and allows easy integration of network capabilities.
- **WebSocket++**: A C++ library that provides support for WebSocket connections. It simplifies the process of setting up a WebSocket server and managing connections.
- **JsonCpp and nlohmann/json**: Libraries for parsing and manipulating JSON data, making it easier to handle responses from the API.

# Running the project
1. Install the required dependencies
2. Compile the project using a C++ compiler
3. Run the executable file

