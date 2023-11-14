# Client-Web Communication with REST API

## Overview

This project is a client-server application written in C that communicates over a REST API. The server simulates an online library, exposing a set of HTTP routes for various actions. The client, functioning as a command-line interface (CLI), accepts user commands, sends corresponding requests to the server, and displays the server's responses.
## Communication Flow

The communication between the client and server involves the following steps:

1. **Establishing Connection:**
   - The client opens a TCP connection with the server.

2. **Sending Requests:**
   - In a continuous loop, the client reads user commands from the keyboard and sends GET or POST requests to the server based on the command.

3. **Request Handling:**
   - The `send_to_server()` function is used to transmit requests, and the server's responses are received using the `receive_from_server()` function.

4. **Parsing Responses:**
   - The `parse_response()` function is employed to parse the server's responses.

## Parsing Data

To parse data, the [Parson](https://github.com/kgabis/parson) library is used, with two functions created for handling credentials (`get_credentials()`) and book information (`get_book()`).

```c
// Example code snippet
#include "parson.h"

// Function to get user credentials in JSON format
JSON_Object* get_credentials(const char* username, const char* password) {
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    // Add username and password to the JSON object
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);

    // Return the JSON object
    return root_object;
}
