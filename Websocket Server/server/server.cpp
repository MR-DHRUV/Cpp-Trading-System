#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <thread>
#include <functional>
#include <nlohmann/json.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/config/asio_client.hpp> 

using namespace std;
using json = nlohmann::json;

// Define server and client handler types
using websocketpp::connection_hdl;
using server = websocketpp::server<websocketpp::config::asio>;
using client = websocketpp::client<websocketpp::config::asio_tls_client>;
using context_ptr = std::shared_ptr<websocketpp::lib::asio::ssl::context>;

// global variables for client and server
client deribit_client;
server ws_server;
connection_hdl deribit_hdl;

// Map of subscribers for each symbol
unordered_map<string, vector<connection_hdl>> subscribers;

// order book data
unordered_map<string, string> orderBooks;

// Set of active Deribit subscriptions
unordered_set<string> active_deribit_subscriptions;

// Function to broadcast order book updates to all clients subscribed to the symbol
void broadcastOrderBookUpdate(const string& symbol) {

    if (subscribers.find(symbol) != subscribers.end()) {
        // send to each subscriber
        for (auto& hdl : subscribers[symbol]) {
            ws_server.send(hdl, orderBooks[symbol], websocketpp::frame::opcode::text);
        }
    }
}

// Send a subscription request to Deribit for a symbol if not already subscribed
void subscribe_to_symbol(const string& symbol) {

    if (active_deribit_subscriptions.find(symbol) == active_deribit_subscriptions.end()) {
        json subscription_msg = {
            {"method", "public/subscribe"},
            {"params", {{"channels", {"book." + symbol + ".none.10.100ms"}}}}
        };

        deribit_client.send(deribit_hdl, subscription_msg.dump(), websocketpp::frame::opcode::text);
        active_deribit_subscriptions.insert(symbol);
        cout << "Subscribed to Deribit for symbol: " << symbol << endl;
    }
}


// Function to handle messages received from Deribit
void on_deribit_message(client* c, connection_hdl hdl, client::message_ptr msg) {
    string payload = msg->get_payload();
    json data = json::parse(payload);

    // Ensure the message format matches the expected structure
    if (data.contains("params") && data["params"].contains("data") && data["params"]["data"].contains("instrument_name")) {
        string symbol = data["params"]["data"]["instrument_name"];

        // Store the entire data object in orderBooks using the symbol as the key
        orderBooks[symbol] = data["params"]["data"].dump();

        // Broadcast to clients subscribed to this symbol
        broadcastOrderBookUpdate(symbol);
    }
}


// Handle new client subscription messages
void on_message(server* s, connection_hdl hdl, server::message_ptr msg) {

    string symbol = msg->get_payload();

    // Add client to subscribers list for symbol
    subscribers[symbol].push_back(hdl);

    // Subscribe to symbol on Deribit
    subscribe_to_symbol(symbol);

    // update the client
    s->send(hdl, "The data stream will start shortly.", websocketpp::frame::opcode::text);
}

// Handle new client connections
void on_open(server* s, connection_hdl hdl) {
    cout << "New client connection opened." << endl;
}

// Handle closed client connections
void on_close(server* s, connection_hdl hdl) {

    // Remove the closed connection from subscribers
    for (auto& subscriber : subscribers) {
        auto& connections = subscriber.second;

        // Remove the connection handle from the vector
        connections.erase(remove_if(connections.begin(), connections.end(), [&hdl](const connection_hdl& existing_hdl) {
            return !existing_hdl.owner_before(hdl) && !hdl.owner_before(existing_hdl);
            }), connections.end());
    }

    cout << "Connection closed." << endl;
}

// TLS initialization callback
context_ptr on_tls_init() {
    context_ptr ctx = make_shared<websocketpp::lib::asio::ssl::context>(websocketpp::lib::asio::ssl::context::tlsv12);
    ctx->set_options(websocketpp::lib::asio::ssl::context::default_workarounds |
        websocketpp::lib::asio::ssl::context::no_sslv2 |
        websocketpp::lib::asio::ssl::context::no_sslv3);
    return ctx;
}

int main() {

    // message handlers
    ws_server.set_open_handler(bind(&on_open, &ws_server, placeholders::_1));
    ws_server.set_close_handler(bind(&on_close, &ws_server, placeholders::_1));
    ws_server.set_message_handler(bind(&on_message, &ws_server, placeholders::_1, placeholders::_2));


    ws_server.init_asio();  // support async non-blocking I/O
    ws_server.listen(9002); // listen on port 9002
    ws_server.start_accept(); // start accepting incoming connections

    // Start WebSocket server in a separate thread
    thread ws_server_thread([&]() { ws_server.run(); });

    // Initialize WebSocket client for Deribit
    deribit_client.init_asio();
    deribit_client.set_message_handler(bind(&on_deribit_message, &deribit_client, placeholders::_1, placeholders::_2));
    deribit_client.set_tls_init_handler(bind(&on_tls_init));

    // Connect to Deribit's  WebSocket endpoint
    string deribit_uri = "wss://test.deribit.com/ws/api/v2";
    websocketpp::lib::error_code ec;
    client::connection_ptr con = deribit_client.get_connection(deribit_uri, ec);

    // if connection fails
    if (ec) {
        cerr << "Could not create connection to Deribit: " << ec.message() << endl;
        return -1;
    }

    deribit_hdl = con->get_handle();
    deribit_client.connect(con);

    // Run Deribit client in the main thread
    cout << "WebSocket server for clients is listening on port 9002." << endl;
    deribit_client.run();

    // Join server thread
    ws_server_thread.join();

    return 0;
}