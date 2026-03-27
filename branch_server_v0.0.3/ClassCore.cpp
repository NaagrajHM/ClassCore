#include "InterfaceCore.hpp"
#include "InterfaceRingBuffer.hpp"

#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iomanip>

typedef int uint8;
using TypeRingBuffer = InterfaceRingBuffer<Message, SIZE_RING_BUFFER>;

class ClassCore : public InterfaceCore{
   private:
             char        buffer[SIZE_MAX_BUFFER];
             unique_ptr<TypeRingBuffer> ringBuffer = TypeRingBuffer::Instantiate();

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
            FdSocketClient = accept(
                                      FdSocketServer
               ,  (struct sockaddr*) &stAddress
               ,  (socklen_t*)       &LengthAddress
            );
         }
      }
      uint8 Ascii2HexNibble(char chAscii){
         uint8 lu8ValueReturn;
              if(chAscii >= '0' && chAscii <= '9'){lu8ValueReturn = chAscii - '0'     ;}
         else if(chAscii >= 'a' && chAscii <= 'f'){lu8ValueReturn = chAscii - 'a' + 10;}
         else if(chAscii >= 'A' && chAscii <= 'F'){lu8ValueReturn = chAscii - 'A' + 10;}
         else                                     {lu8ValueReturn = 0                 ;}
         return lu8ValueReturn;
      }
      uint8 Ascii2HexByte(char* lptri8Ascii){
         return(
               Ascii2HexNibble(lptri8Ascii[0]) * 16
            +  Ascii2HexNibble(lptri8Ascii[1])
         );
      }
      void Dcm_vExecuteService_TesterPresent(char* ptrchBuffer){
         switch(Ascii2HexByte(ptrchBuffer)){
            case 0x01: memcpy(buffer, "027E01", 7); break;
            default:                                break;
         }
         send(
               FdSocketClient
            ,  buffer
            ,  strlen(buffer)
            ,  0
         );
      }
      void Dcm_vExecuteService(char* ptrchBuffer){
         switch(Ascii2HexByte(&ptrchBuffer[2])){
            case 0x3E: Dcm_vExecuteService_TesterPresent(&ptrchBuffer[4]); break;
            default:                                                       break;
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
            ringBuffer->push(message);
            /* Point of seperation */
            Dcm_vExecuteService(ringBuffer->pop()->data);
            return true;
         }
      }
};

unique_ptr<InterfaceCore> InterfaceCore::Instantiate(void){
   return make_unique<ClassCore>();
}
