#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "InterfaceCore.hpp"
class ClassCore : public InterfaceCore{
   private:
             char        buffer[1024];
      struct sockaddr_in stAddress;
             int         FdSocketServer;

   public:
      ~ClassCore(){
         close(FdSocketServer);
      }
      ClassCore(void){
         FdSocketServer = socket(
               AF_INET
            ,  SOCK_STREAM
            ,  0
         );
         stAddress.sin_family      = AF_INET;
         stAddress.sin_port        = htons(8080);
         inet_pton(
               AF_INET
            ,  "172.17.0.2"
            ,  &stAddress.sin_addr
         );
         if(
               connect(
                     FdSocketServer
                  ,  (struct sockaddr*) &stAddress
                  ,  sizeof(stAddress)
               )
            <  0
         ){
            perror("Connection failed");
            exit(EXIT_FAILURE); // return -1;
         }
      }
      bool MainFunction(void){
         std::string input;
         std::cout << "Enter message: ";
         std::getline(std::cin, input);
         send(
               FdSocketServer
            ,  input.c_str()
            ,  input.length()
            ,  0
         );
         memset(buffer, 0, 1024);
         if(
               read(
                     FdSocketServer
                  ,  buffer
                  ,  1024
               )
            <= 0
         ){
            return false;
         }
         else{
            std::cout << "Server echoed: " << buffer << std::endl;
            return true;
         }
      }
};

std::unique_ptr<InterfaceCore> InterfaceCore::Instantiate(void){
   return make_unique<ClassCore>();
}
