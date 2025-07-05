#include <libwebsockets.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <signal.h>

using namespace std;

static int interrupted = 0;

// WebSocket client context
static struct lws_context *context;
static struct lws *wsi;

// Callback function for WebSocket events
static int callback_client(struct lws *wsi, enum lws_callback_reasons reason,
                          void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            cout << "Connected to WebSocket server" << endl;
            // Send subscription message
            {
                string symbol = "BTC-PERPETUAL";
                unsigned char buffer[LWS_PRE + symbol.length()];
                memcpy(&buffer[LWS_PRE], symbol.c_str(), symbol.length());
                lws_write(wsi, &buffer[LWS_PRE], symbol.length(), LWS_WRITE_TEXT);
                cout << "Subscribed to: " << symbol << endl;
            }
            break;
            
        case LWS_CALLBACK_CLIENT_RECEIVE:
            cout << "Received update: " << endl << (char*)in << endl << endl;
            break;
            
        case LWS_CALLBACK_CLIENT_CLOSED:
            cout << "Connection closed." << endl;
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
        callback_client,
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
    struct lws_client_connect_info conn_info;
    
    // Set up signal handler
    signal(SIGINT, sigint_handler);
    
    // Initialize context creation info
    memset(&info, 0, sizeof info);
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;
    
    // Create context
    context = lws_create_context(&info);
    if (!context) {
        cout << "Failed to create context" << endl;
        return -1;
    }
    
    // Set up connection info
    memset(&conn_info, 0, sizeof(conn_info));
    conn_info.context = context;
    conn_info.address = "localhost";
    conn_info.port = 9002;
    conn_info.path = "/";
    conn_info.host = conn_info.address;
    conn_info.origin = conn_info.address;
    conn_info.protocol = protocols[0].name;
    conn_info.ssl_connection = 0;
    
    // Connect to server
    wsi = lws_client_connect_via_info(&conn_info);
    if (wsi == NULL) {
        cout << "Failed to connect to server" << endl;
        return -1;
    }
    
    // Main event loop
    while (!interrupted) {
        lws_service(context, 50);
    }
    
    // Cleanup
    lws_context_destroy(context);
    
    return 0;
}
