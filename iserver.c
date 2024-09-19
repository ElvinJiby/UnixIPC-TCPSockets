/*
 * 21/04/2024
 * Elvin Jiby
 * Unix Programming - Assignment 3
 *
 * This is the server program that sends quiz questions & receives responses from the client (named iclient.c)
 * Compile using gcc (i.e. gcc -o iserver.c iserver)
 * Run in the format: ./iserver <IP address of server> <port number>
 *
 * The server program takes two arguments: an IPv4 address and a Port Number
 * The server will bind to the socket address created using the two input arguments
 * The server will print two lines to the standard output:
 *  - Assuming the input arguments are (for example) 127.0.0.1 and 25555, the output is:
 *      <Listening on 127.0.0.1:25555>
 *      <Press ctrl-C to terminate>
 * It will then wait for client connections
 * After successful connection with a client, the server will start the quiz
 * If the client sends Y, the server starts the quiz
 * Otherwise, the server closes the connection & serves the next client
 *
 * In a loop, the server issues the five quiz questions and waits for an answer from
 *  the client. Afterwards, the server will send a response indicating the correctness of the answer given
 * After the quiz, the server sends the quiz score results to the client
 * It then closes the connection and serves the next client
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include "QuizDB.h"

#define BACKLOG 10
#define BUFSIZE 1024

ssize_t sendClientData(int cfd, char *buf);
ssize_t receiveClientData(int cfd, char *buf);

int main(int argc, char *argv[])
{
   if (argc != 3) // check if correct number of arguments are provided
   {
      fprintf(stderr, "Usage: %s <IP address of server> <port number>.\n",
            argv[0]);
      exit(-1);
   }

   srand(time(NULL));

   // Setup of IPv4 socket structure with given address and port
   struct sockaddr_in serverAddress;
   memset(&serverAddress, 0, sizeof(struct sockaddr_in));
   serverAddress.sin_family = AF_INET;
   serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
   serverAddress.sin_port = htons(atoi(argv[2]));

   // define new socket - lfd becomes the file descriptor
   int lfd = socket(AF_INET, SOCK_STREAM, 0);
   if (lfd == -1) {
      fprintf(stderr, "socket() error.\n");
      exit(-1);
   }

   /*
   * This socket option allows you to reuse the server endpoint
   * immediately after you have terminated it.
   */
   int optval = 1;
   if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR,
                  &optval, sizeof(optval)) == -1)
      exit(-1);

   // bind the address to the socket
   int rc = bind(lfd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr));
   if (rc == -1) {
      fprintf(stderr, "bind() error.\n");
      exit(-1);
   }

   if (listen(lfd, BACKLOG) == -1)
      exit(-1);

   fprintf(stdout, "Listening on (%s, %s)\n", argv[1], argv[2]);

   for (;;) /* Handle clients iteratively */
   {
      fprintf(stdout, "<waiting for clients to connect>\n");
      fprintf(stdout, "<ctrl-C to terminate>\n");

      // waits for a client to connect - cfd becomes connection file descriptor
      struct sockaddr_storage claddr;
      socklen_t addrlen = sizeof(struct sockaddr_storage);
      int cfd = accept(lfd, (struct sockaddr *)&claddr, &addrlen);
      if (cfd == -1) {
         fprintf(stderr, "socket() error.\n");
         continue;   /* Print an error message */
      }

      // Given a socket address structure, returns strings containing the
      //  corresponding host and service name, which is printed out
      char host[NI_MAXHOST];
      char service[NI_MAXSERV];
      if (getnameinfo((struct sockaddr *) &claddr, addrlen,
               host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
         fprintf(stdout, "Connection from (%s, %s)\n", host, service);
      else
         fprintf(stderr, "Connection from (?UNKNOWN?)");

      // Send welcome message after connection is established
      char buf[BUFSIZE];
      sprintf(buf, "Welcome to Unix Programming Quiz!\n"
                        "The quiz comprises five questions posed to you one after the other.\n"
                        "You have only one attempt to answer a question.\n"
                        "Your final score will be sent to you after conclusion of the quiz.\n"
                        "To start the quiz, press Y and <enter>.\n"
                        "To quit the quiz, press q and <enter>.\n");

      // Send welcome message to client
      ssize_t totalSent = sendClientData(cfd, buf);
      if(totalSent <= 0) { // error handling - function prints error messages, while here it closes cfd and exits
         close(cfd);
         exit(EXIT_FAILURE);
      }
      bzero(buf, strlen(buf)); // reset all values in buf to 0

      // Get response from client
      ssize_t totalReceived = receiveClientData(cfd, buf);
      if (totalReceived <= 0) { // error handling - function prints error messages, while here it closes cfd and exits
         close(cfd);
         exit(EXIT_FAILURE);
      }

      // Start the Quiz
      if (buf[0] == 'Y') {
         // select 5 random (but distinct) questions from the quiz question list
         int quizArraySize = sizeof(QuizQ) / sizeof(QuizQ[0]);
         int randomIndex; // variable to store the random index number generated
         int selectedAmnt = 0; // variable to store the amount of questions chosen
         int selectedIndex[5]; // array to store the indexes of the 5 questions to be chosen

         // Loop to randomly select questions from QuizDB.h
         for (int i = 0; i < 5; i++) {
            do {
               randomIndex = rand() % quizArraySize; // generate random index number

               for (int j = 0; j < selectedAmnt; j++) { // check if that index was already used before
                  if (selectedIndex[j] == randomIndex) {
                     randomIndex = -1;
                     break;
                  }
               }
            } while (randomIndex == -1); // repeat until the random index number is unique in the selected array

            selectedIndex[selectedAmnt++] = randomIndex; // store the index and increment selectedAmnt counter
         }

         int score = 0; // keeps track of the score / 5
         bzero(buf, strlen(buf)); // reset all values of buf

         // Loop to send questions to client and receive answers from it
         for (int i = 0; i < 5; i++) {
            // Send question
            sprintf(buf, "Q%d: %s\n", i+1, QuizQ[selectedIndex[i]]); // format question into buf
            totalSent = sendClientData(cfd, buf);
            if (totalSent <= 0) {  // error handling
               close(cfd);
               exit(EXIT_FAILURE);
            }

            // Get answer
            bzero(buf, strlen(buf));
            totalReceived = receiveClientData(cfd, buf);
            if (totalReceived <= 0) { // error handling
               close(cfd);
               exit(EXIT_FAILURE);
            }
            buf[strcspn(buf, "\n")] = 0;

            // Check if answer is correct or not, add score if correct
            if (strcmp(buf, QuizA[selectedIndex[i]]) == 0) { // correct answer
               bzero(buf, strlen(buf));
               sprintf(buf, "Right Answer.\n");
               score++;
            } else { // incorrect answer
               bzero(buf, strlen(buf));
               sprintf(buf, "Wrong Answer. Right answer is %s\n", QuizA[selectedIndex[i]]);
            }
            totalSent = sendClientData(cfd, buf); // inform the client on whether their answer is correct or not
            if (totalSent <= 0) {
               close(cfd);
               exit(EXIT_FAILURE);
            }
            bzero(buf, strlen(buf));
         }

         // End of Quiz - return score
         sprintf(buf, "Your quiz score is %d/5. Goodbye!\n", score);
         totalSent = sendClientData(cfd, buf);
         if (totalSent <= 0) {
            close(cfd);
            exit(EXIT_FAILURE);
         }
         fprintf(stdout, "Quitting the quiz.\n");

      }

      // If any option other than 'Y' is entered (e.g. 'q' or any other input), close the connection
      else {
         fprintf(stdout, "Closing connection.\n");
         close(cfd);
         continue; // serve next client
      }

      /* Close connection just incase it gets here somehow */
      if (close(cfd) == -1)
      { // error handling
         fprintf(stderr, "close error.\n");
         exit(EXIT_FAILURE);
      }
   }

   if (close(lfd) == -1) /* Close listening socket */
   {
      fprintf(stderr, "close error.\n");
      exit(EXIT_FAILURE);
   }

   exit(EXIT_SUCCESS);
}

ssize_t sendClientData(int cfd, char *buf) { // Function that sends data to client
   char* bufs = buf; // buffer string to store message
   size_t totalSent;

   // Loop to ensure writing of BUFSIZE bytes
   for (totalSent = 0; totalSent < BUFSIZE; ) {
      ssize_t numWritten = send(cfd, bufs, BUFSIZE - totalSent, 0);
      if (numWritten <= 0) { // Error handling
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

ssize_t receiveClientData(int cfd, char *buf) { // Function that receives data from client
   char* bufr = buf;
   size_t totalReceived;

   // Loop ensures read of BUFSIZE bytes
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