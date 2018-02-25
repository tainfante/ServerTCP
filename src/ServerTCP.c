/*
 ============================================================================
 Name        : ServerTCP.c
 Author      : Agata Dul
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>


const int buffer_size = 1024;

struct number {
    double currentnumber;
    struct number *nextnumber;
};

double sumation(struct number *el);

double difference(struct number *el);

void sortup(struct number *el);

void sortdown(struct number *el);

char **str_split(char *a_str, const char a_delim);

char *chooseOperation(char **pString, int messageSize);

int getMessageSize(char **pString);

char *splitListAgain(struct number *el);


int main(int argc, char **argv) {
    system("clear");
    if (argc != 2) {
        printf("Invalid number of arguments, define IP and port number\n");
        exit(1);
    }

    int master_socket;
    int new_socket;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t client_addr_length;
    char *buffer;
    int read_bytes;

    buffer = (char *) malloc(buffer_size);

    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Could not create socket: ");
        exit(1);
    }
    else{
    	printf("Created socket number: %d\nWaiting for client to connect...\n", master_socket);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(master_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error in binding: ");
        exit(1);
    }

    if (listen(master_socket, 5) < 0)
        perror("Error in listing operation:  ");
    client_addr_length = sizeof(cli_addr);


    int pid;
    while (1) {
        new_socket = accept(master_socket, (struct sockaddr *) &cli_addr, &client_addr_length);
        if (new_socket < 0) {
            perror("Could not accept connection: ");
            exit(1);
        }

        printf("Connected socket number: %d\nWaiting for instructions...\n", new_socket);


        pid = fork();

        if (pid < 0) {
            perror("Error in fork function, could not create new process: ");
            exit(1);
        } else if (pid == 0) {
            while (1) {
                read_bytes = (int) read(new_socket, buffer, buffer_size);
                if (read_bytes < 0) {
                    perror("Error in receiving a message ");
                    exit(1);
                }

                if (read_bytes == 0) {
                    printf("Socket nr: %d closed\n", new_socket);
                    close(new_socket);
                    break;
                }
                printf("Message from client number %d: %s\n", new_socket, buffer);

                char **messageone = str_split(buffer, '\n');
                char **splittedMessage = str_split(messageone[0], ' ');
                free(messageone);
                int size = getMessageSize(splittedMessage);

                char *preparedMessage = chooseOperation(splittedMessage, size);
                free(splittedMessage);

                printf("Transmitted message: %s\n",preparedMessage);


                if ((read_bytes = (int) write(new_socket, preparedMessage, buffer_size))< 0) {
                    perror("Could not transmit message ");
                    exit(1);
                }
            }
        }

    }

    return 0;
}

char **str_split(char *a_str, const char a_delim) {
    char **result = 0;
    size_t count = 0;
    char *tmp = a_str;
    char *last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    while (*tmp) {
        if (a_delim == *tmp) {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    count += last_comma < (a_str + strlen(a_str) - 1);

    count++;

    result = malloc(sizeof(char *) * count);

    if (result) {
        size_t idx = 0;
        char *token = strtok(a_str, delim);

        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}


char *chooseOperation(char **pString, int messageSize) {
    printf("Chosen operation type: %s\n", *pString);

    int i;
    struct number *nextForOperation = NULL;
    char *bufferToSend = malloc(1024);

    for (i = 1; i < messageSize; i++) {
        struct number *tmpElement = (struct number *) malloc(sizeof(struct number));
        tmpElement->currentnumber = atof(pString[i]);
        tmpElement->nextnumber = nextForOperation;
        nextForOperation = tmpElement;
    }
    if (strcmp(pString[0], "sum") == 0) {
        double result = sumation(nextForOperation);
        printf("Result = %f\n", result);
        sprintf(bufferToSend, "%d", (int) result);
    } else if (strcmp(pString[0], "div") == 0) {
        double result = difference(nextForOperation);
        printf("Result = %f\n", result);
        sprintf(bufferToSend, "%d", (int) result);
    } else if (strcmp(pString[0], "sortup") == 0) {
        sortup(nextForOperation);
        printList(nextForOperation);
        bufferToSend = splitListAgain(nextForOperation);
    } else if (strcmp(pString[0], "sortdown") == 0) {
        sortdown(nextForOperation);
        printList(nextForOperation);
        bufferToSend = splitListAgain(nextForOperation);
    } else {
        bufferToSend = "WRONG INPUT";
    }

    return bufferToSend;
}

int getMessageSize(char **pString) {
    int size = 0;

    if (!pString) return 0;

    while (*(pString + size) != NULL) {
        size++;
    }


    return size;
}


char *splitListAgain(struct number *el) {
    struct number *tmp = el;

    char *output = malloc(1024);

    while (tmp != NULL) {
        char tempBuff[sizeof(double)];
        sprintf(tempBuff, "%.2f", tmp->currentnumber);
        strcat(tempBuff, " ");
        strcat(output, tempBuff);
        tmp = tmp->nextnumber;
    }

    return output;
}

void printList(struct number *start) {
    struct number *temp = start;
    printf("\nList of the elements:\n");
    while (temp != NULL) {
        printf("%0.2f ", temp->currentnumber);
        temp = temp->nextnumber;
    }
}

double sumation(struct number *el) {
    double result = 0.0;
    struct number *tmp = el;

    while (tmp != NULL) {
        result += tmp->currentnumber;
        tmp = tmp->nextnumber;
    }

    return result;
}

double difference(struct number *el) {
    double result;
    struct number *tmp = el;

    result = tmp->currentnumber;
    tmp = tmp->nextnumber;

    while (tmp != NULL) {
        result -= tmp->currentnumber;
        tmp = tmp->nextnumber;
    }

    return result;
}

void sortdown(struct number *el) {
    int swapped, i;
    struct number *ptr1;
    struct number *lptr = NULL;

    ptr1 = el;
    for(; ptr1->nextnumber != NULL; ptr1 = ptr1->nextnumber)
    {
        for(lptr = ptr1->nextnumber; lptr != (struct element*)NULL; lptr = lptr->nextnumber)
        {
            if(ptr1->currentnumber < lptr->currentnumber)
            {
                int temp = (int) ptr1->currentnumber;
                ptr1->currentnumber = lptr->currentnumber;
                lptr->currentnumber = temp;
            }
        }
    }

}

void sortup(struct number *el) {
    int swapped, i;
    struct number *ptr1;
    struct number *lptr = NULL;

    if (ptr1 == NULL)
        return;

    do {
        swapped = 0;
        ptr1 = el;

        while (ptr1->nextnumber != lptr) {
            if (ptr1->currentnumber > ptr1->nextnumber->currentnumber) {
                swap(ptr1, ptr1->nextnumber);
                swapped = 1;
            }
            ptr1 = ptr1->nextnumber;
        }
        lptr = ptr1;
    } while (swapped);


}

void swap(struct number *a, struct number *b) {
    double temp = a->currentnumber;
    a->currentnumber = b->currentnumber;
    b->currentnumber = temp;
}


