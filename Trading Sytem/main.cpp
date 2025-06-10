#include <iostream>
#include <fstream>
#include <json/json.h>
#include <string>
#include "helper.h"
#pragma warning(disable : 4996)

using namespace std;

// const string API_KEY = "API_KEY";
// const string API_SECRET = "API_SECRET";
// const string BASE_URL = "https://test.deribit.com/api/v2";

const string API_KEY = "rb7PTnhs";
const string API_SECRET = "jNJEKuu6IaTRSWmqEibgxb3aUJpUrTBDRi-hrI29o9U";
const string BASE_URL = "https://test.deribit.com/api/v2";

// Function to authenticate and get access token
string authenticate() {
    try
    {
        string readBuffer, token, errs;
        string url = BASE_URL + "/public/auth?client_id=" + API_KEY + "&client_secret=" + API_SECRET + "&grant_type=client_credentials";

        // Make the request
        if (!performCurlRequest(url, readBuffer,"")) {
			cerr << "Failed to authenticate." << endl;
			return "";
		}

        // Parse JSON response to get access token
        Json::CharReaderBuilder reader;
        Json::Value obj;

        istringstream s(readBuffer);
        if (Json::parseFromStream(reader, s, &obj, &errs)) {
            token = obj["result"]["access_token"].asString();
            cout << "Authenticated. Access Token: " << token << endl;
        }
        else {
            cerr << "Failed to parse authentication response json." << endl;
        }

        // token or empty string if authentication failed
        return token; 
    }
    catch (const exception&e)
    {
        cerr << "Failed to authenticate." << endl;
        return "";
    }
}

string getOrderStatus(const string& token, const string& orderId) {
    try {
        // Validate orderId
        if (orderId.empty()) {
            cerr << "Invalid order ID." << endl;
            return "";
        }
 
        string readBuffer, state, errs;
        string url = BASE_URL + "/private/get_order_state?order_id=" + orderId;

        // Make the request
        if (!performCurlRequest(url, readBuffer, token)) {
			cerr << "Failed to get order status." << endl;
			return "";
		}

        // Parse JSON response to get order state
        Json::CharReaderBuilder reader;
        Json::Value obj;

        istringstream s(readBuffer);
        if (Json::parseFromStream(reader, s, &obj, &errs)) {
            state = obj["result"]["order_state"].asString();
            cout<<"Order Id: "<<orderId<<" State: "<<state<<endl;
        }
        else {
            cerr << "Failed to parse order state." << endl;
        }

        return state;
    }
    catch (const exception&) {
        cerr << "Failed to get order status." << endl;
        return "";
    }
}

void placeOrder(const string& token, const string& instrument, double amount, double price, string type) {

    try
    {
        // Validate amount and price
        if (amount <= 0 || price <= 0) {
            cerr << "Invalid amount or price." << endl;
            return;
        }

        // Validate instrument using regex
        if (!checkInstrument(instrument)) {
            cerr << "Invalid instrument." << endl;
            return;
        }

        // validate type
        if (type != "buy" && type != "sell") {
			cerr << "Invalid type." << endl;
			return;
		}
       
        string readBuffer;
        string url = BASE_URL + "/private/"+type+"?instrument_name=" + instrument + "&amount=" + to_string(amount) + "&price=" + to_string(price);

        // Make the request
        if (!performCurlRequest(url, readBuffer, token)) {
            cerr << "Failed to place order." << endl;
            return;
        }

        // print the response
        printJson(readBuffer);
    }
    catch (const exception&)
    {
		cerr << "Failed to buy." << endl;
    }
}

void getPositions(const string& token, const string& currency = "BTC") {

    try {
        string readBuffer;
        string url = BASE_URL + "/private/get_positions";

        // Append currency to URL if provided
        if (!currency.empty()) {
			url += "?currency=" + currency;
		}

        // Make the request
        if (!performCurlRequest(url, readBuffer, token)) {
			cerr << "Failed to get positions." << endl;
			return;
		}

        // print the response
        printJson(readBuffer);
    }
    catch (const exception&) {
        cerr << "Failed to get positions." << endl;
    }
}

void getOrderBook(const string& instrument) {
    try {

        // Validate instrument
        if (instrument.empty() || !checkInstrument(instrument)) {
			cerr << "Invalid instrument." << endl;
			return;
		}

        string readBuffer, errors;
        string url = BASE_URL + "/public/get_order_book?instrument_name=" + instrument;

        // Make the request
        if (!performCurlRequest(url, readBuffer, "")) {
			cerr << "Failed to get order book." << endl;
			return;
		}
        
        // Print the positions
        Json::Value jsonData;
        Json::CharReaderBuilder reader;

        // Parse the JSON response
        istringstream s(readBuffer);
        if (Json::parseFromStream(reader, s, &jsonData, &errors)) {
            cout << "Order Book for " << instrument << ":" << endl;
            
            // Print asks
            cout << "Asks:" << endl;
            for (const auto& ask : jsonData["result"]["asks"]) {
                cout << "Price: " << ask[0].asDouble() << ", Amount: " << ask[1].asDouble() << endl;
            }

            // Print bids
            cout << "Bids:" << endl;
            for (const auto& bid : jsonData["result"]["bids"]) {
                cout << "Price: " << bid[0].asDouble() << ", Amount: " << bid[1].asDouble() << endl;
            }
        }
        else {
            cerr << "Failed to parse JSON: " << errors << endl;
        }
    }
    catch (const exception&) {
        cerr << "Failed to get order book." << endl;
    }
}

void cancelOrder(const string& token, const string& orderId) {
    try {
        string readBuffer;
        string url = BASE_URL + "/private/cancel?order_id=" + orderId;

        // Make the request
        if (!performCurlRequest(url, readBuffer, token)) {
			cerr << "Failed to cancel order." << endl;
			return;
		}

        // print the response
		printJson(readBuffer);
    }
    catch (const exception&) {
        cerr << "Failed to cancel order." << endl;
    }
}

void modifyOrder(const string& token, const string& orderId, double newAmount = -1, double newPrice = -1) {
    try {
        string url = BASE_URL + "/private/edit?order_id=" + orderId;
        string readBuffer;

        // Append new amount if provided
        if (newAmount >= 0) {
            url += "&amount=" + to_string(newAmount);
        }

        // Append new price if provided
        if (newPrice >= 0) {
            url += "&price=" + to_string(newPrice);
        }

        // Ensure at least one parameter is being modified
        if (newAmount < 0 && newPrice < 0) {
            cerr << "At least one of amount or price must be specified for modification." << endl;
            return;
        }

        // Make the request
        if (!performCurlRequest(url, readBuffer, token)) {
            cerr << "Failed to modify order." << endl;
        }

        // print the response
        printJson(readBuffer);
    }
    catch (const exception&) {
        cerr << "Failed to modify order." << endl;
    }
}

// function to display the menu
void displayMenu() {
    cout << "=== Trading System Menu ===" << endl;
    cout << "1. Place Order" << endl;
    cout << "2. Get Order Status" << endl;
    cout << "3. Get Current Positions" << endl;
    cout << "4. Get Order Book" << endl;
    cout << "5. Cancel Order" << endl;
    cout << "6. Modify Order" << endl;
    cout << "0. Exit" << endl;
}

// function to drive the application
void driver() {
    string token = authenticate(); // get the access token
    bool running = true;

    while (running) {
        displayMenu();
        cout << "Select an option: ";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {  // Place Order
            if (token.empty()) {
                cout << "Please authenticate first." << endl;
                break;
            }
            string instrument, type;
            double amount, price;
            cout << "Enter the instrument name: ";
            cin >> instrument;
            cout << "Enter the order amount: ";
            cin >> amount;
            cout << "Enter the order price: ";
            cin >> price;
            cout << "Enter the order type (buy/sell): ";
            cin >> type;
            placeOrder(token, instrument, amount, price, type);
            break;
        }

        case 2: {  // Get Order Status
            if (token.empty()) {
                cout << "Please authenticate first." << endl;
                break;
            }
            string orderId;
            cout << "Enter the order ID to check status: ";
            cin >> orderId;
            getOrderStatus(token, orderId);
            break;
        }

        case 3: {  // Get Current Positions
            if (token.empty()) {
                cout << "Please authenticate first." << endl;
                break;
            }
            getPositions(token);
            break;
        }

        case 4: {  // Get Order Book
            string instrument;
            cout << "Enter the instrument name to get the order book: ";
            cin >> instrument;
            getOrderBook(instrument);
            break;
        }

        case 5: {  // Cancel Order
            if (token.empty()) {
                cout << "Please authenticate first." << endl;
                break;
            }
            string orderId;
            cout << "Enter the order ID to cancel: ";
            cin >> orderId;
            cancelOrder(token, orderId);
            break;
        }

        case 6: {  // Modify Order
            if (token.empty()) {
                cout << "Please authenticate first." << endl;
                break;
            }
            string orderId;
            double newAmount, newPrice;
            cout << "Enter the order ID to modify: ";
            cin >> orderId;
            cout << "Enter new amount (-1 to skip): ";
            cin >> newAmount;
            cout << "Enter new price (-1 to skip): ";
            cin >> newPrice;
            modifyOrder(token, orderId, newAmount, newPrice);
            break;
        }

        case 0:  // Exit
            running = false;
            cout << "Exiting the application." << endl;
            break;

        default:
            cout << "Invalid option. Please try again." << endl;
            break;
        }
    }
}

int main() {
    driver();
	return 0;
}
