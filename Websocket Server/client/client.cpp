#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

// client and connection handler types
using websocketpp::connection_hdl;
using client = websocketpp::client<websocketpp::config::asio_client>;

// Send symbol as message to subscribe when the connection is opened
void on_open(client* c, websocketpp::connection_hdl hdl, const string& symbol) {
    c->send(hdl, symbol, websocketpp::frame::opcode::text);
    cout << "Subscribed to: " << symbol << endl;
}

// print the received message
void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
    cout << "Received update: "<<endl<< msg->get_payload() << endl<<endl;
}

void on_close(client* c, websocketpp::connection_hdl hdl) {
    cout << "Connection closed." << endl;
}

int main() {

    // client endpoint
    client c;

    c.init_asio(); // support async non-blocking I/O

    string symbol = "BTC-PERPETUAL";

    // Set message handlers
    c.set_open_handler(bind(&on_open, &c, std::placeholders::_1, symbol));
    c.set_message_handler(bind(&on_message, &c, std::placeholders::_1, std::placeholders::_2));
    c.set_close_handler(bind(&on_close, &c, std::placeholders::_1));

    // Create a connection
    string uri = "ws://localhost:9002";
    websocketpp::lib::error_code ec;
    client::connection_ptr con = c.get_connection(uri, ec);

    // connection is not valid
    if (ec) {
        cout << "Could not create connection: " << ec.message() << endl;
        return -1;
    }

    // Start the connection
    c.connect(con);

    // Run the ASIO event loop in a separate thread
    thread client_thread([&c]() { c.run(); });

    // Keep the client running to receive real-time updates
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
    }

    client_thread.join(); // Ensures the main thread waits for the client thread to finish.

    return 0;
}
