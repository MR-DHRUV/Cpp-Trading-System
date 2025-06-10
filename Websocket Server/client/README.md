# Sample Client
This WebSocket client connects to a server running at `ws://localhost:9002` and subscribes to updates for a given symbol, here `"BTC-PERPETUAL"`. When the connection opens, the client sends the symbol as a message to the server to subscribe to updates for that symbol. The client then remains open, receiving and printing updates as they come from the server in real time.

The `on_open` function sends the subscription message to the server when the connection is first established, while `on_message` handles incoming messages, outputting the data to the console. The `on_close` function logs when the connection is closed. The ASIO event loop runs in a separate thread to keep the client responsive, and the main thread waits indefinitely to keep the client open for continuous updates. This setup enables efficient real-time updates with minimal latency, leveraging WebSocket's persistent connection.

# Running the project
1. Install the required dependencies
2. Compile the project using a C++ compiler
3. Run the executable file

