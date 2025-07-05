#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <thread>
#include <functional>
#include <nlohmann/json.hpp>
#include <libwebsockets.h>
#include <signal.h>
#include <curl/curl.h>

using namespace std;
using json = nlohmann::json;

static int interrupted = 0;
static struct lws_context *context;

// Map of subscribers for each symbol
unordered_map<string, vector<struct lws*>> subscribers;

// order book data
unordered_map<string, string> orderBooks;

// Set of active Deribit subscriptions
unordered_set<string> active_deribit_subscriptions;

// CURL callback for HTTP requests
size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to broadcast order book updates to all clients subscribed to the symbol
void broadcastOrderBookUpdate(const string& symbol) {
    if (subscribers.find(symbol) != subscribers.end()) {
        for (auto& wsi : subscribers[symbol]) {
            if (wsi && orderBooks.find(symbol) != orderBooks.end()) {
                unsigned char buffer[LWS_PRE + orderBooks[symbol].length()];
                memcpy(&buffer[LWS_PRE], orderBooks[symbol].c_str(), orderBooks[symbol].length());
                lws_write(wsi, &buffer[LWS_PRE], orderBooks[symbol].length(), LWS_WRITE_TEXT);
            }
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

        // For now, we'll simulate the subscription
        // In a real implementation, you'd need to maintain a separate WebSocket connection to Deribit
        active_deribit_subscriptions.insert(symbol);
        cout << "Subscribed to Deribit for symbol: " << symbol << endl;
        
        // Simulate order book data
        json mock_data = {
            {"instrument_name", symbol},
            {"bids", {{"1000.0", "1.0"}, {"999.0", "2.0"}}},
            {"asks", {{"1001.0", "1.5"}, {"1002.0", "2.5"}}}
        };
        orderBooks[symbol] = mock_data.dump();
        broadcastOrderBookUpdate(symbol);
    }
}

// WebSocket server callback
static int callback_server(struct lws *wsi, enum lws_callback_reasons reason,
                          void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            cout << "New client connection opened." << endl;
            break;
            
        case LWS_CALLBACK_RECEIVE:
            {
                string symbol((char*)in, len);
                cout << "Received subscription request for: " << symbol << endl;
                
                // Add client to subscribers list for symbol
                subscribers[symbol].push_back(wsi);
                
                // Subscribe to symbol on Deribit
                subscribe_to_symbol(symbol);
                
                // Send confirmation to client
                string response = "The data stream will start shortly.";
                unsigned char buffer[LWS_PRE + response.length()];
                memcpy(&buffer[LWS_PRE], response.c_str(), response.length());
                lws_write(wsi, &buffer[LWS_PRE], response.length(), LWS_WRITE_TEXT);
            }
            break;
            
        case LWS_CALLBACK_CLOSED:
            {
                cout << "Connection closed." << endl;
                // Remove the closed connection from subscribers
                for (auto& subscriber : subscribers) {
                    auto& connections = subscriber.second;
                    connections.erase(remove(connections.begin(), connections.end(), wsi), connections.end());
                }
            }
            break;
            
        default:
            break;
    }
    
    return 0;
}

// Protocol definition
static struct lws_protocols protocols[] = {
    {
        "websocket",
        callback_server,
        0,
        4096,
    },
    { NULL, NULL, 0, 0 } // terminator
};

// Signal handler for graceful shutdown
static void sigint_handler(int sig) {
    interrupted = 1;
}

int main() {
    struct lws_context_creation_info info;
    
    // Set up signal handler
    signal(SIGINT, sigint_handler);
    
    // Initialize CURL
    curl_global_init(CURL_GLOBAL_ALL);
    
    // Initialize context creation info
    memset(&info, 0, sizeof info);
    info.port = 9002;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;
    
    // Create context
    context = lws_create_context(&info);
    if (!context) {
        cout << "Failed to create context" << endl;
        return -1;
    }
    
    cout << "WebSocket server for clients is listening on port 9002." << endl;
    
    // Main event loop
    while (!interrupted) {
        lws_service(context, 50);
    }
    
    // Cleanup
    lws_context_destroy(context);
    curl_global_cleanup();
    
    return 0;
}