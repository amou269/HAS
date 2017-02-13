#ifndef HTTP_PARSER_H_
#define HTTP_PARSER_H_

#include <ns3/ptr.h>
#include "mpeg-header.h"

namespace ns3
{
    class Socket;
    class EvalvidClient;
    class HttpParser
    {
        public:
            HttpParser();
            virtual ~HttpParser();
            void ReadSocket(Ptr<Socket> socket);
            void SetApp(EvalvidClient *app);

        private:
            uint8_t m_buffer[MPEG_MAX_MESSAGE];
            uint32_t m_bytes;
            EvalvidClient *m_app;
            Time m_lastmeasurement;
            int packetcount;
            uint32_t message_size; //size of MPEG message currently being transmitted
    };
}
#endif /* HTTP_PARSER_H_ */
