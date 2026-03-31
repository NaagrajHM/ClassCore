#include "InterfaceCore.hpp"
#include "InterfaceDcmEth.hpp"

#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iomanip>

int FdSocketClient;

class ClassCore : public InterfaceCore{
   private:
      struct sockaddr_in stAddress;
             int         FdSocketServer;
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
            FdSocketClient = accept(
                                      FdSocketServer
               ,  (struct sockaddr*) &stAddress
               ,  (socklen_t*)       &LengthAddress
            );
         }
      }
      bool MainFunction(void){
         Message message;
         memset(message.data, 0, SIZE_MAX_BUFFER);
         if(
               read(
                     FdSocketClient
                  ,  message.data
                  ,  SIZE_MAX_BUFFER
               )
            <= 0
         ){
            return false;
         }
         else{
            InterfaceDcmEth_vRxIndication(message);
            return true;
         }
      }
};

unique_ptr<InterfaceCore> InterfaceCore::Instantiate(void){
   return make_unique<ClassCore>();
}

void InterfaceEthDcm_vTransmit(
   Message& message
){
   send(
         FdSocketClient
      ,  message.data
      ,  strlen(message.data)
      ,  0
   );
}
