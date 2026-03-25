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
      ClassCore(
         string stringAddressIP
      ){
         FdSocketServer = socket(
               AF_INET
            ,  SOCK_STREAM
            ,  0
         );
         stAddress.sin_family = AF_INET;
         stAddress.sin_port   = htons(8080);
         inet_pton(
               AF_INET
            ,  stringAddressIP.c_str()
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
         string input;
         cout << "Enter message: ";
         getline(cin, input);
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
            cout << "Server echoed: " << buffer << endl;
            return true;
         }
      }
};

std::unique_ptr<InterfaceCore> InterfaceCore::Instantiate(
   string stringAddressIP
){
   return make_unique<ClassCore>(
      stringAddressIP
   );
}
