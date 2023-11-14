
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define HOST "34.254.242.81"
#define PORT 8080

char *error_output(char *response) {
	char *error = (char*)calloc(100, sizeof(char));
	strcpy(error, strstr(response, "\"error\":"));
	error += 9;
	error[strlen(error) - 2] = '\0';
	return error;
}

char *get_credentials (char *username, char *password)  {
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);

	json_object_set_string(root_object, "username", username);
	json_object_set_string(root_object, "password", password);

	return json_serialize_to_string_pretty(root_value);
}

char *get_book(char *title, char *author, char *genre, char *publisher, char *page_count) {
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);

	json_object_set_string(root_object, "title", title);
	json_object_set_string(root_object, "author", author);
	json_object_set_string(root_object, "genre", genre);
	json_object_set_string(root_object, "publisher", publisher);
	json_object_set_string(root_object, "page_count", page_count);

	return json_serialize_to_string_pretty(root_value);
}

int main(int argc, char *argv[]) 
{
    char buffer[LINELEN];
    char *message, *response;
    char username[LINELEN], password[LINELEN];
    char *payload=(char*)calloc(2000, sizeof(char*));
	int cookie_count = 0;;
	char *cookie = NULL, *JWT = NULL;
	char *ID_ROUTE = (char*)calloc(100, sizeof(char));

    setvbuf(stdout, NULL, _IONBF, 0);

    int running = 1;

    while (running) {
		scanf("%s", buffer);
		int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

		if(!sockfd){
			printf("Error opening connection!\n");
			continue;
		}

		if (strcmp(buffer, "register") == 0) {

			printf("username=");
			scanf("%s", username);

			printf("password=");
			scanf("%s", password);
			
			payload = get_credentials(username, password);
			
			message = compute_post_request(HOST, REGISTER_ROUTE, JSON, &payload, 1, NULL, 0, NULL);

			send_to_server(sockfd, message);

			free(message);

			response = receive_from_server(sockfd);

			if (strstr(response, "\"error\":") != NULL) {
				char* error = error_output(response);
				printf("%s\n", error);
				free(response);
			} else {
				printf("200 - OK - \"User authenticated successfully!\"\n");
			}

			free(response);

			close(sockfd);
		}
		else 
			if (strcmp(buffer, "login") == 0) {

				if(cookie_count == 1) {
					printf("Already authenticated!\n");
					continue;
				}

				printf("username=");
				scanf("%s", username);

				printf("password=");
				scanf("%s", password);
				
				payload = get_credentials(username, password);

				message = compute_post_request(HOST, LOGIN_ROUTE, JSON, &payload, 1, NULL, 0, NULL);

				send_to_server(sockfd, message);

				free(message);
				
				response = receive_from_server(sockfd);

				if (strstr(response, "\"error\":") != NULL) {
					char* error = error_output(response);
					printf("%s\n", error);
					free(response);
					continue;
				}

				char* token_start = strstr(response, "connect.sid=");
				char* token_end = strstr(token_start, ";");
				cookie = (char*)calloc(token_end - token_start + 1, sizeof(char));
				strncpy(cookie, token_start, token_end - token_start);

				if(cookie != NULL) 
					cookie_count = 1;

				if (strstr(response, "\"error\":") == NULL) {
					printf("200 - OK - \"User logged successfully!\"\n");
					cookie_count = 1;
				}

				close(sockfd);
			}
			else 
				if (strcmp(buffer, "enter_library") == 0) {
					
					if(JWT) {
						printf("Already in the library!\n");
						continue;
					}
					message = compute_get_request(HOST, ACCESS_ROUTE, NULL, &cookie, 1, NULL);

					send_to_server(sockfd, message);

					free(message);

					response = receive_from_server(sockfd);

					if (strstr(response, "\"error\":") != NULL) {
						char* error = error_output(response);
						printf("%s\n", error);
						free(response);
						continue;
					}
					
					char* token_start = strstr(response, "{\"token\":\"");
					token_start += 10;
					char* token_end = strstr(token_start, "\"}");
					JWT = (char*)calloc(token_end - token_start + 1, sizeof(char));
					strncat(JWT, token_start, token_end - token_start);
					JWT[strlen(JWT)] = '\0';
					
					printf("200 - OK - \"User entered library successfully!\"\n");
					
					close(sockfd);
				}
				else 
					if (strcmp(buffer, "logout") == 0) {

						message = compute_get_request(HOST, LOGOUT_ROUTE, NULL, &cookie, 1, NULL);
						send_to_server(sockfd, message);
						free(message);

						response = receive_from_server(sockfd);

						if (strstr(response, "\"error\":") != NULL) {
							char* error = error_output(response);
							printf("%s\n", error);
							free(response);
						} else {
							printf("200 - OK - \"User logged out successfully!\"\n");
							cookie_count = 0;
							free(cookie);
							JWT = NULL;
						}
						close(sockfd);
					}
					else 
						if (strcmp(buffer, "get_books") == 0) {

							if(!cookie_count) {
								printf("You are not logged in!\n");
								continue;
							}

							if(!JWT) {
								printf("You are not in any library!\n");
								continue;
							}

							message = compute_get_request(HOST, BOOKS_ROUTE, NULL, &cookie, cookie_count, JWT);
							send_to_server(sockfd, message);
							free(message);

							response = receive_from_server(sockfd);
							if (strstr(response, "\"error\":") != NULL) {
								char* error = error_output(response);
								printf("%s\n", error);
								free(response);
							} else {
								printf("200 - OK - \"Books received successfully!\"\n");
								printf("%s\n", strstr(response, "["));
							}
							close(sockfd);
						}
						else 
							if(strcmp(buffer, "add_book") == 0) {

								if(!cookie_count) {
									printf("You are not logged in!\n");
									continue;
								}

								if(!JWT) {
									printf("You are not in any library!\n");
									continue;
								}

								char* title = (char*)calloc(100, sizeof(char));
								char* author = (char*)calloc(100, sizeof(char));
								char* genre = (char*)calloc(100, sizeof(char));
								char* publisher = (char*)calloc(100, sizeof(char));
								char* page_count = (char*)calloc(100, sizeof(char));

								getchar();

								printf ("title=");
								fgets(title, 100, stdin);
								title[strlen(title) - 1] = '\0';

								printf("author=");
								fgets(author, 100, stdin);
								author[strlen(author) - 1] = '\0';

								printf("genre=");
								fgets(genre, 100, stdin);
								genre[strlen(genre) - 1] = '\0';

								printf("publisher=");
								fgets(publisher, 100, stdin);
								publisher[strlen(publisher) - 1] = '\0';

								printf("page_count=");
								fgets(page_count, 100, stdin);
								page_count[strlen(page_count) - 1] = '\0';

								payload = get_book(title, author, genre, publisher, page_count);

								message = compute_post_request(HOST, BOOKS_ROUTE, JSON, &payload, 1, NULL, 0, JWT);

								send_to_server(sockfd, message);

								free(message);
								
								response = receive_from_server(sockfd);

								if (strstr(response, "\"error\":") != NULL) {
									char* error = error_output(response);
									printf("%s\n", error);
									free(response);
									continue;
								}

								printf("200 - OK - \"Book added successfully!\"\n");
								close(sockfd);
							}
							else 
								if(strcmp(buffer, "delete_book") == 0) {

									if(!cookie_count) {
										printf("You are not logged in!\n");
										continue;
									}

									if(!JWT) {
										printf("You are not in any library!\n");
										continue;
									}


									char *id = (char*)calloc(100, sizeof(char));

									printf("id=");
									scanf("%s", id);

									strcpy(ID_ROUTE, BOOKS_ROUTE);
									strcat(ID_ROUTE, "/");
									strcat(ID_ROUTE, id);

									message = compute_delete_request(HOST, ID_ROUTE, JSON, &cookie, cookie_count, JWT);

									send_to_server(sockfd, message);

									free(message);

									response = receive_from_server(sockfd);
										if (strstr(response, "\"error\":") != NULL) {
										char* error = error_output(response);
										printf("%s\n", error);
										free(response);
										continue;
									}

									printf("200 - OK - \"Book deleted successfully!\"\n");

								}
								else 
									if (strcmp(buffer, "get_book") == 0) {
										
										if(!cookie_count) {
											printf("You are not logged in!\n");
											continue;
										}

										if(!JWT) {
											printf("You are not in any library!\n");
											continue;
										}

										char *id = (char*)calloc(100, sizeof(char));
										printf("id=");
										scanf("%s", id);

										strcpy(ID_ROUTE, BOOKS_ROUTE);
										strcat(ID_ROUTE, "/");
										strcat(ID_ROUTE, id);

										message = compute_get_request(HOST, ID_ROUTE, NULL, &cookie, cookie_count, JWT);

										send_to_server(sockfd, message);

										free(message);

										response = receive_from_server(sockfd);

										if (strstr(response, "\"error\":") != NULL) {
											char* error = error_output(response);
											printf("%s\n", error);
											free(response);
										}
										else {
											printf("200 - OK - \"Book received successfully!\"\n");
											printf("%s\n", strstr(response, "{"));
										}
										close(sockfd);
									}
									else 
										if (strcmp(buffer, "exit") == 0) {
											printf("Are you sure you want to exit? (y/n)\n");
											char *exit = (char*)calloc(100, sizeof(char));
											scanf("%s", exit);
											if(strcmp(exit, "y") == 0) {
												running = 0;
											}

											free(exit);

											if(cookie_count) {
												free(cookie);
											}

											if(JWT) {
												free(JWT);
											}
										}
    }

    return 0;
}
