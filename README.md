# C++ Trading System

This repository contains two main components: **Trading System** and **WebSocket Server**, both designed to interact with the Deribit API for trading and real-time market updates. These systems are optimized for ultra-low latency and high-frequency trading applications.


## Trading System

The **Trading System** is a C++ application designed to interface with the Deribit API. It provides functionalities for authentication, order placement, order management, and market data retrieval. The system ensures low latency, making it ideal for high-frequency trading.

### Key Features
1. **Authentication**: Authenticate users with the Deribit API and retrieve access tokens.
2. **Order Management**: Place, modify, and cancel orders for various instruments (futures, perpetual contracts, options).
3. **Market Data Retrieval**: Fetch order book data and trading positions.
4. **Instrument Support**:
    - **Futures**: Example: `BTC-25MAR23`
    - **Perpetual Contracts**: Example: `BTC-PERPETUAL`
    - **Options**: Example: `BTC-25MAR23-420-C`

### Project Structure
- **main.cpp**: Contains the main logic for interacting with the trading API.
- **helper.h**: Declares utility functions used in the project.

### Dependencies
- **CURL**: A fast and lightweight library used for making HTTP requests to the trading API. It supports various protocols and allows easy integration of network capabilities.
- **JsonCpp and nlohmann/json**: Libraries for parsing and manipulating JSON data, making it easier to handle responses from the API.
- **Regex**: Used for validating instrument names to ensure they conform to expected formats.

### Running the Trading System
1. Add your Deribit API key and secret to the `main.cpp` file.
2. Install the required dependencies.
3. Compile the project using a C++ compiler.
4. Run the executable file.


## WebSocket Server

The **WebSocket Server** provides real-time updates to clients subscribed to different trading symbols. It broadcasts changes in order book data using WebSocket technology, ensuring faster data delivery and responsiveness to market changes.

### Key Features
1. **Subscription Management**: Efficiently manage client subscriptions for different trading symbols.
2. **Dynamic Order Book Updates**: Listen to the Deribit WebSocket API and update order books in real time.
3. **Targeted Broadcasts**: Push updates to clients subscribed to specific symbols.
4. **High-Frequency Updates**: Designed to handle multiple client connections and high-frequency data updates.

### Main Functions
- **broadcastOrderBookUpdate**: Sends the latest order book data to all clients subscribed to a specific symbol.
- **on_deribit_message**: Handles incoming messages from Deribit’s WebSocket feed and updates order books.
- **on_message**: Manages client subscriptions to trading symbols.
- **on_open** and **on_close**: Handle new connections and clean up closed connections.

### Dependencies
- **CURL**: For making HTTP requests to the trading API.
- **WebSocket++**: For WebSocket communication.
- **JsonCpp and nlohmann/json**: For JSON parsing and manipulation.

### Running the WebSocket Server
1. Install the required dependencies.
2. Compile the project using a C++ compiler.
3. Run the executable file.


## Folder Structure
- **Trading System**: Contains the C++ application for trading functionalities.
- **WebSocket Server**: Includes the server for real-time market updates.
    - **Server**: WebSocket server implementation.
    - **Client**: Sample client for testing WebSocket server functionality.


## References
- [Deribit API Documentation](https://docs.deribit.com/)