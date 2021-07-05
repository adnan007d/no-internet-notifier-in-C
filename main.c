#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <unistd.h>

#define BUFFER 15
#define INTERVAL 10

void check_requirements();          // Checking notify-send package exits
void resolve_host(char *, char *);  // Resolving host;
void send_notification(int *, int); // Sending notification

int main(int argc, char *argv[])
{
    check_requirements();

    char *hostname = "www.google.com";
    // max length -> 255.255.255.255 = 15
    char ip[BUFFER] = "";

    // Here we save states to determine that our internet is up or not
    // 1 means up 0 means down
    // default previous state will be 1 as we assume the internet is up and running
    // current state will change after every interval
    int prev_state = 1;
    int state = 0;

    while (1)
    {
        resolve_host(ip, hostname);

        if (strlen(ip) != 0)
        {
            state = 1;
        }
        else
        {
            state = 0;
        }

        memset(ip, 0, BUFFER);
        send_notification(&prev_state, state);
        sleep(INTERVAL);
    }

    return 0;
}

void check_requirements()
{

    // This will return 0 if the notify-send exists
    // And 256 if it doesn't exists
    if (system("which notify-send > /dev/null"))
    {
        printf("Please install notify-send for this program to work\ncommand: sudo apt install notify-send");
        exit(1);
    }
}

void resolve_host(char *ip, char *hostname)
{

    struct hostent *he;
    struct in_addr **ip_addr;

    // If this returns NULL means No Internet
    if ((he = gethostbyname(hostname)) == NULL)
    {
        return;
    }

    // Extracting the array of ip address from hostent struct
    ip_addr = (struct in_addr **)he->h_addr_list;

    // Just a sanity check
    if (ip_addr)
    {
        // Copying the ip address into ip char array
        strcpy(ip, inet_ntoa(*ip_addr[0]));
    }
}

void send_notification(int *prev_state, int state)
{

    // We don't need to do anything if the prev_state and current_state are same
    if (*prev_state == state)
        return;

    // Max Length of command
    char command[151];

    // If they are different we check the current state and send the notification accordingly
    switch (state)
    {
    case 1:
        // Max len of command is 147
        snprintf((char *)command, sizeof(command), "notify-send %s %s %s", "'Internet is Back'", "'Your Internet is back.\n Enjoy :)'", "-i notification-network-wireless");
        system(command);
        *prev_state = 1;
        break;
    case 0:
        snprintf(command, sizeof(command), "notify-send %s %s %s", "'No Internet'", "'Your internet is disconnected.\n You will be notified when its up again :)'", "-i notification-network-wireless-disconnected");
        system(command);
        *prev_state = 0;
        break;
    default:
        printf("Why would you even do this ?");
        break;
    }
}
