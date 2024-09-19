/*
 * 21/04/2024
 * Elvin Jiby
 * Unix Programming - Assignment 3
 *
 * This is the client program that receives quiz questions & receives answers from the server program (named iserver.c)
 * Compile using gcc (i.e. gcc -o iclient.c iclient)
 * Run in the format: ./iclient <IP address of server> <port number>
 *
 * Client Program takes two arguments: an IPv4 address and a Port Number
 * Connects to the server using these two input arguments
 * It then sends Y or q to the server to start the quiz or quit
 * For each question sent by the server:
 *      - The client must accept the answer from standard input
 *      - The answer is sent to the server
 * After the conclusion of the quiz, the client will close the connection to the server
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

#define BUFSIZE 1024

ssize_t sendServerData(int cfd, char *buf);
ssize_t receiveServerData(int cfd, char *buf);

int main(int argc, char *argv[]) 
{
   if (argc != 3) // check for valid number of arguments
   {
      fprintf(stderr, "Usage: %s <IP address of server> <port number>.\n",
            argv[0]);
      exit(-1);
   }

   struct sockaddr_in serverAddress;
   
   memset(&serverAddress, 0, sizeof(struct sockaddr_in));
   serverAddress.sin_family = AF_INET;
   serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
   serverAddress.sin_port = htons(atoi(argv[2]));

   // socket setup
   int cfd = socket(AF_INET, SOCK_STREAM, 0);
   if (cfd == -1) {
      fprintf(stderr, "socket() error.\n");
      exit(-1);
   }

   // connect to socket
   int rc = connect(cfd, (struct sockaddr *)&serverAddress,
                  sizeof(struct sockaddr));
   if (rc == -1) {
      fprintf(stderr, "connect() error, errno %d.\n", errno);
      exit(-1);
   }

   // receive welcome message from server
   char buf[BUFSIZE];
   ssize_t totalReceived = receiveServerData(cfd, buf);
   if (totalReceived <= 0) { // error handling - function prints error messages, while here it closes cfd and exits
      close(cfd);
      exit(EXIT_FAILURE);
   }
   fprintf(stdout, "%s\n", buf); // print welcome message

   // Input response through stdin
   bzero(buf, BUFSIZE);
   fprintf(stdout, "Enter Y to start the quiz or q to quit: ");
   if (fgets(buf, BUFSIZE, stdin) == NULL) { // error handling
      fprintf(stdout, "Input error.\n");
      close(cfd);
      exit(EXIT_FAILURE);
   }
   ssize_t totalSent = sendServerData(cfd, buf);
   if (totalSent <= 0) { // error handling - function prints error messages, while here it closes cfd and exits
      close(cfd);
      exit(EXIT_FAILURE);
   }

   bzero(buf, BUFSIZE); // reset buf

   // Loop to receive questions and input responses to quiz
   for (int i = 0; i < 5; i++) {
      // Get question from server
      totalReceived = receiveServerData(cfd, buf);
      if (totalReceived <= 0) {
         close(cfd);
         exit(EXIT_FAILURE);
      }
      fprintf(stdout, "%s", buf);
      bzero(buf, BUFSIZE);

      // Send answer to server
      fprintf(stdout, "Enter your answer: ");
      if (fgets(buf, BUFSIZE, stdin) == NULL) { // error handling
         fprintf(stdout, "Input error.\n");
         close(cfd);
         exit(EXIT_FAILURE);
      }
      totalSent = sendServerData(cfd, buf);
      if (totalSent <= 0) {
         close(cfd);
         exit(EXIT_FAILURE);
      }
      bzero(buf, BUFSIZE);

      // Receive response from server on correctness of the answer
      totalReceived = receiveServerData(cfd, buf);
      if (totalReceived <= 0) {
         close(cfd);
         exit(EXIT_FAILURE);
      }
      fprintf(stdout, "%s", buf);
      bzero(buf, BUFSIZE);
   }

   // Receive quiz score
   totalReceived = receiveServerData(cfd, buf);
   if (totalReceived <= 0) {
      close(cfd);
      exit(EXIT_FAILURE);
   }
   fprintf(stdout, "%s", buf); // print the score received from server
   bzero(buf, BUFSIZE);


   /* Close connection */
   if (close(cfd) == -1)
   {
      fprintf(stderr, "close error.\n");
      exit(EXIT_FAILURE);
   }

   exit(EXIT_SUCCESS);
}

ssize_t sendServerData(int cfd, char *buf) { // Function that sends data to server
   char* bufs = buf;
   size_t totalSent;

   // Loop to ensure write of BUFSIZE bytes
   for (totalSent = 0; totalSent < BUFSIZE; ) {
      ssize_t numWritten = send(cfd, bufs, BUFSIZE - totalSent, 0);
      if (numWritten <= 0) { // Error Handling
         if (numWritten == -1 && errno == EINTR)
            continue;
         else {
            fprintf(stderr, "Write error.\n");
            exit(EXIT_FAILURE);
         }
      }
      totalSent += numWritten;
      bufs += numWritten; // increment buffer position
   }
   return totalSent;
}

ssize_t receiveServerData(int cfd, char *buf) { // Function that receives data from server
   char* bufr = buf;
   size_t totalReceived;

   // Loop to ensure read of BUFSIZE bytes
   for (totalReceived = 0; totalReceived < BUFSIZE; ) {
      ssize_t numRead = recv(cfd, bufr, BUFSIZE - totalReceived, 0);
      if (numRead == 0)
         break;
      if (numRead == -1) { // Error Handling
         if (errno == EINTR)
            continue;
         else {
            fprintf(stderr, "Read error.\n");
         }
      }
      totalReceived += numRead;
      bufr += numRead; // increment buffer position
   }
   return totalReceived;
}
