#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "InterfaceCore.hpp"
class ClassCore : public InterfaceCore{
   private:
             char        buffer[1024];
      struct sockaddr_in stAddress;
             int         FdSocketServer;
             int         FdSocketClient;
             int         LengthAddress;

   public:
      ~ClassCore(){
         close(FdSocketClient);
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
         stAddress.sin_addr.s_addr = INADDR_ANY;
         if(
               bind(
                     FdSocketServer
                  ,  (struct sockaddr*) &stAddress
                  ,  sizeof(stAddress)
               )
            <  0
         ){
            perror("Bind failed");
            exit(EXIT_FAILURE); // return -1;
         }
         else{
            listen(FdSocketServer, 3);
            LengthAddress = sizeof(stAddress);
            std::cout << "Server listening on port 8080..." << std::endl;
            FdSocketClient = accept(
                                      FdSocketServer
               ,  (struct sockaddr*) &stAddress
               ,  (socklen_t*)       &LengthAddress
            );
         }
      }
      bool MainFunction(void){
         memset(buffer, 0, 1024);
         if(
               read(
                     FdSocketClient
                  ,  buffer
                  ,  1024
               )
            <= 0
         ){
            return false;
         }
         else{
            std::cout << "Client: " << buffer << std::endl;
            send(
                  FdSocketClient
               ,  buffer
               ,  strlen(buffer)
               ,  0
            );
            return true;
         }
      }
};

std::unique_ptr<InterfaceCore> InterfaceCore::Instantiate(void){
   return make_unique<ClassCore>();
}
