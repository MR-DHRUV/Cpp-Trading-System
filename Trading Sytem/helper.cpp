#include "helper.h"
#include <iostream>
#include <fstream>
#include <json/json.h>
#include <string>
#include <regex>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#pragma warning(disable : 4996)

using namespace std;

// Callback for cURL to write data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to append response to log file
void appendToLog(const string& response) {
    try
    {
        // append the response to a log file without overwriting
        ofstream logFile("logs.txt", ios::app);

        if (logFile.is_open()) {
            // convert the response to JSON for proper formatting
            nlohmann::json parsedJson = nlohmann::json::parse(response);
            logFile<< parsedJson.dump(4)<<endl;
            logFile.close(); // Close the file
        }
        else {
            cerr << "Unable to open log file." << endl;
        }
    }
    catch (const exception&)
    {
        cerr << "Failed to append to log." << endl;
    }
}

// function to make a cURL request 
bool performCurlRequest(const string& url, string& response,const string& token) {
    try
    {
        // Initialize cURL
        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Failed to initialize cURL." << std::endl;
            return false;
        }

        CURLcode res;
        response.clear(); // Clear previous response

        // Set cURL options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // Set URL

        // Set header for authentication if needed
        struct curl_slist* headers = NULL;
        if (!token.empty()) {
            headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }

        // Set callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // Set response data buffer
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform cURL request
        res = curl_easy_perform(curl);

        // append response to log file
        appendToLog(response);

        // Cleanup
        curl_easy_cleanup(curl);
        if (headers != NULL) {
            curl_slist_free_all(headers);
        }

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        return true;
    }
    catch (const std::exception&)
    {
        cout << "Failed to perform cURL request." << endl;
        return false;
    }
}

// Function to check if instrument is valid
bool checkInstrument(string instrument)
{
    // Perpetual Contract (matches ANY-CHARACTER-PERPETUAL format)
    if (regex_match(instrument, regex("^[A-Z]+-PERPETUAL$"))) {
        return 1;
    }
    // Futures Contract (matches ANY-CHARACTER-25MAR23 format)
    else if (regex_match(instrument, regex("^[A-Z]+-\\d{1,2}[A-Z]{3}\\d{2,4}$"))) {
        return 1;
    }
    // Options Contract (matches ANY-CHARACTER-25MAR23-420-C format)
    else if (regex_match(instrument, regex("^[A-Z]+-\\d{1,2}[A-Z]{3}\\d{2,4}-\\d+(d\\d+)?-[CP]$"))) {
        return 1;
    }

    return 0;
}

void printJson(const string& jsonString) {
    try {
        // Parse the JSON string
        nlohmann::json parsedJson = nlohmann::json::parse(jsonString);

        // Print the formatted JSON
        cout << parsedJson.dump(4) << endl;
    }
    catch (const nlohmann::json::parse_error& e) {
        cerr << "JSON parse error: " << e.what() << endl;
    }
}