#include <stdio.h>  // console input/output, perror
#include <stdlib.h> // exit
#include <string.h> // string manipulation
#include <netdb.h>  // getnameinfo

#include <sys/socket.h> // socket APIs
#include <netinet/in.h> // sockaddr_in
#include <unistd.h>     // open, close

#include <signal.h> // signal handling
#include <time.h>   // time

#define SIZE 1024  // buffer size
#define PORT 2728  // port number
#define BACKLOG 10 // number of pending connections queue will hold

void getFileURL(char *route, char *fileURL);

void getMimeType(char *file, char *mime);

void handleSignal(int signal);

void getTimeString(char *buf);

void handleClient(int clientSocket);

int serverSocket;
int clientSocket;

char *request;

int main()
{
    signal(SIGINT, handleSignal);

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        printf("Error: The server is not bound to the address.\n");
        return 1;
    }

    if (listen(serverSocket, BACKLOG) < 0) {
        printf("Error: The server is not listening.\n");
        return 1;
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket < 0) {
            printf("Error: Failed to accept a client.\n");
            continue;
        }

        pid_t pid = fork(); // Create a new process
        if (pid < 0) {
            printf("Error: Failed to fork a process.\n");
            close(clientSocket);
        } else if (pid == 0) {
            // Child process
            close(serverSocket); // Child doesn't need the listening socket
            handleClient(clientSocket);
        } else {
            // Parent process
            close(clientSocket); // Parent doesn't need the connected socket
        }
    }

    close(serverSocket);
    return 0;
}

void getFileURL(char *route, char *fileURL)
{
  // if route has parameters, remove them
  char *question = strrchr(route, '?');
  if (question)
    *question = '\0';

  // if route is empty, set it to index.html
  if (route[strlen(route) - 1] == '/')
  {
    strcat(route, "index.html");
  }

  // get filename from route
  strcpy(fileURL, "htdocs");
  strcat(fileURL, route);

  // if filename does not have an extension, set it to .html
  const char *dot = strrchr(fileURL, '.');
  if (!dot || dot == fileURL)
  {
    strcat(fileURL, ".html");
  }
}

void getMimeType(char *file, char *mime)
{
  // position in string with period character
  const char *dot = strrchr(file, '.');

  // if period not found, set mime type to text/html
  if (dot == NULL)
    strcpy(mime, "text/html");

  else if (strcmp(dot, ".html") == 0)
    strcpy(mime, "text/html");

  else if (strcmp(dot, ".css") == 0)
    strcpy(mime, "text/css");

  else if (strcmp(dot, ".js") == 0)
    strcpy(mime, "application/js");

  else if (strcmp(dot, ".jpg") == 0)
    strcpy(mime, "image/jpeg");

  else if (strcmp(dot, ".png") == 0)
    strcpy(mime, "image/png");

  else if (strcmp(dot, ".gif") == 0)
    strcpy(mime, "image/gif");

  else
    strcpy(mime, "text/html");
}

void handleSignal(int signal)
{
  if (signal == SIGINT)
  {
    printf("\nShutting down server...\n");

    close(clientSocket);
    close(serverSocket);

    if (request != NULL)
      free(request);

    exit(0);
  }
}

void getTimeString(char *buf)
{
  time_t now = time(0);
  struct tm tm = *gmtime(&now);
  strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
}

void handleClient(int clientSocket) {
    char request[SIZE];
    char method[10], route[100];
    read(clientSocket, request, SIZE);
    sscanf(request, "%s %s", method, route);
    printf("%s %s\n", method, route);

    if (strcmp(method, "POST") == 0 && strcmp(route, "/message") == 0) {
        char *body = strstr(request, "\r\n\r\n") + 4;
        printf("Received POST data: %s\n", body);

        const char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nMessage received";
        send(clientSocket, response, sizeof(response) -1, 0);
    } else if (strcmp(method, "GET") == 0) {
        char fileURL[100];
        getFileURL(route, fileURL);

        FILE *file = fopen(fileURL, "r");
        if (!file) {
            const char response[] = "HTTP/1.1 404 Not Found\r\n\n";
            send(clientSocket, response, sizeof(response), 0);
        } else {
            char resHeader[SIZE];
            char timeBuf[100];
            getTimeString(timeBuf);
            char mimeType[32];
            getMimeType(fileURL, mimeType);

            sprintf(resHeader, "HTTP/1.1 200 OK\r\nDate: %s\r\nContent-Type: %s\r\n\n", timeBuf, mimeType);
            int headerSize = strlen(resHeader);

            fseek(file, 0, SEEK_END);
            long fsize = ftell(file);
            fseek(file, 0, SEEK_SET);

            char *resBuffer = (char *)malloc(fsize + headerSize);
            strcpy(resBuffer, resHeader);
            char *fileBuffer = resBuffer + headerSize;
            fread(fileBuffer, fsize, 1, file);

            send(clientSocket, resBuffer, fsize + headerSize, 0);
            free(resBuffer);
            fclose(file);
        }
    } else {
        const char response[] = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        send(clientSocket, response, sizeof(response) - 1, 0);
    }

    close(clientSocket);
    exit(0); // Terminate the child process
}
