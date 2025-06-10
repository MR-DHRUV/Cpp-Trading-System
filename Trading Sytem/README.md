# Trading System
Trading System is a C++ application designed to interface with the deribit API, ensuring ultra low latency while enabling users to authenticate, place and manage orders, and fetch market data. 

The system provides functions for authentication, order placement, order status retrieval, position fetching, order book display, order cancellation, and order modification. 

## Project Structure
- main.cpp: The main application file containing the logic for interacting with the trading API.
- helper.h: A header file that declares utility functions used in the project.

## Key Functions
1. **string authenticate()**
    - Authenticates the user with the trading API and retrieves an access token.
    - Constructs a URL using the API key and secret, sends an authentication request, and parses the JSON response to extract the access token.
    - Returns the access token or an empty string if authentication fails.
2. **string getOrderStatus(const string& token, const string& orderId)**
    - Retrieves the status of a specific order using the provided order ID.
    - Validates the order ID, constructs the appropriate URL, and sends a request to fetch the order state.
    - Parses the JSON response to obtain and return the order state.
3. **void placeOrder(const string& token, const string& instrument, double amount, double price, string type)**
    - Places a buy/sell order for a specified instrument, amount, and price.
    - type is an enumeration of either "buy" or "sell".
    - The instument can be used to represent a spot/option/future instrument.
    
        | Kind | Examples | Template | Comments |
        | --- | --- | --- | --- |
        | Future | `BTC-25MAR23`, `BTC-5AUG23` | `BTC-DMMMYY` | `BTC` is currency, `DMMMYY` is expiration date, `D` stands for day of month (1 or 2         digits), `MMM` - month (3 first letters in English), `YY` stands for year. |
        | Perpetual | `BTC-PERPETUAL` |  | Perpetual contract for currency `BTC`. |
        | Option | `BTC-25MAR23-420-C`, `BTC-5AUG23-580-P` | `BTC-DMMMYY-STRIKE-K` | `STRIKE` is option strike price in USD. Template `K` is option         kind: `C` for call options or `P` for put options.

        ref:(https://docs.deribit.com/?shell#overview)
    
    - Validates the amount and price, checks the instrument's validity, and constructs the order request URL.
    - Sends the request and prints the JSON response.

4. **void getPositions(const string& token, const string& currency = "BTC")**
    - Retrieves the current trading positions for a specified currency (default is BTC).
    - Constructs the URL for fetching positions and sends the request.
    - Prints the JSON response containing the positions.
5. **void getOrderBook(const string& instrument)**
    - Fetches the order book for a specified instrument.
    - Validates the instrument, constructs the order book request URL, and sends the request.
    - Parses the response to display the current asks and bids in the order book.
6. **void cancelOrder(const string& token, const string& orderId)**
    - Cancels an existing order using the provided order ID and prints the response.
7. **void modifyOrder(const string& token, const string& orderId, double newAmount = -1, double newPrice = -1)**
    - Modifies an existing order's amount or price based on the provided parameters.
    - Validates the input and constructs the modify order URL.
    - Sends the request and prints the response.

## Running the Project
1. Add your deribit API key and secret to the `main.cpp` file.
2. Install the required dependencies
3. Compile the project using a C++ compiler
4. Run the executable file
