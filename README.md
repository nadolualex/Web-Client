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
```

## Command Handling

The client supports various commands, each triggering specific actions on the server:

### Register

The `register` command creates a JSON object with user credentials and sends a registration request to the server.

### Login

The `login` command logs in a user, sets the received cookie for subsequent requests, and validates the login.

### Enter Library

The `enter_library` command utilizes a JWT received from the server, parsing it manually, and grants access to the library.

### Logout

The `logout` command logs out a user, displaying a confirmation message and clearing allocated memory for the cookie and JWT.

### Get Books

The `get_books` command retrieves all books from the library, displaying them in JSON format.

### Add Book

The `add_book` command adds a book to the library, ensuring the user is logged in and has the necessary access.

### Delete Book

The `delete_book` command deletes a book from the library, verifying user credentials and handling errors appropriately.

### Get Book

The `get_book` command retrieves information about a specific book from the library.

### Exit

The `exit` command gracefully terminates the program, closing the TCP connection and freeing allocated memory.

## Usage

To run the client-server application, follow these steps:

1. **Clone the Repository:**
  
2. **Compile and Run:**
   - Compile the client and server programs.
   - Execute the compiled client program.

3. **Interaction:**
   - Follow the on-screen instructions to interact with the application.
   - Enter commands, and the client will communicate with the server accordingly.

4. **Exiting the Program:**
   - When prompted to exit, enter "y" to gracefully terminate the program.
   - If you wish to continue using the program, enter any other response.
  
## Usage of Cookies and HTTP

### Cookies:

- **Login Command:**
  - Upon a successful login, the server issues a cookie to the client.
  - The client, in turn, stores this cookie for subsequent requests, allowing the server to recognize and authenticate the user.

- **Logout Command:**
  - When logging out, the client clears the stored cookie, ensuring a clean termination of the user's session.

### HTTP:

- **GET and POST Requests:**
  - User commands, such as registering, logging in, or interacting with the library, are translated into either GET or POST requests, depending on the nature of the action.

- **Headers:**
  - HTTP headers, specifically the `Authorization` header, are utilized to transmit the JWT token securely between the client and server.
  - The token, prefixed with "Bearer," is included in the header for authentication purposes.

- **Response Handling:**
  - The client's `parse_response()` function is responsible for interpreting the HTTP responses received from the server.
  - Successful responses may include data or confirmation messages, while error responses are appropriately handled and presented to the user.

