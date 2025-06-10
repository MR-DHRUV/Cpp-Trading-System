#ifndef HELPER_H
#define HELPER_H


#include <string>

using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
void appendToLog(const string& response);
bool checkInstrument(string instrument);
bool performCurlRequest(const string& url, string& response, const string& token = "");
void printJson(const string& jsonString);

#endif // HELPER_H
