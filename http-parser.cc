#include "http-parser.h"
#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/socket.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "http-header.h"
#include "mpeg-header.h"
#include "ns3/seq-ts-header.h"
#include "evalvid-client.h"

NS_LOG_COMPONENT_DEFINE("HttpParser");

namespace ns3
{

    HttpParser::HttpParser() :
        m_bytes(0), m_app(NULL), m_lastmeasurement("0s")
    {
        NS_LOG_FUNCTION(this);
        packetcount=0;
        message_size=0;
    }

    HttpParser::~HttpParser()
    {
        NS_LOG_FUNCTION(this);
    }

    void
    HttpParser::SetApp(EvalvidClient *app)
    {
        NS_LOG_FUNCTION(this << app);
        m_app = app;
    }
    void
    HttpParser::ReadSocket(Ptr<Socket> socket)
    {
        NS_LOG_FUNCTION(this << socket);
        Address from;
        int bytes = socket->RecvFrom(&m_buffer[m_bytes], MPEG_MAX_MESSAGE - m_bytes, 0, from);
        if (bytes<=0) return;

        /* bytes > 0 */
        packetcount++;
        m_bytes += bytes;
        std::cout <<"Client: bytes received->" <<bytes <<" packetcount:" <<packetcount <<" m_bytes->"
                  <<m_bytes <<" " <<Simulator::Now().ToDouble(ns3::Time::S) <<std::endl;

        MPEGHeader mpeg_header;
        HTTPHeader http_header;
        SeqTsHeader seqTs;
        uint32_t headersize = mpeg_header.GetSerializedSize() + http_header.GetSerializedSize() + seqTs.GetSerializedSize();

        if (m_lastmeasurement > Time("0s"))
        {
            NS_LOG_INFO(Simulator::Now().GetSeconds() << " bytes: " << bytes << " dt: "
                        << (Simulator::Now() - m_lastmeasurement).GetSeconds() << " bitrate: "
                        << (8 * (bytes + headersize)/ (Simulator::Now() - m_lastmeasurement).GetSeconds()));
        }
        m_lastmeasurement = Simulator::Now();
        NS_LOG_INFO("### Buffer space: " << m_bytes << " Queue length " << m_app->GetPlayer().GetQueueSize());
        if (m_bytes < headersize)
        {
            return;
        }
        if (m_bytes<536+headersize){ //536 is TCP MSS
            Packet headerPacket(m_buffer, headersize);
            headerPacket.RemoveHeader(mpeg_header);
            message_size = mpeg_header.GetSize() + (mpeg_header.GetSize()/m_app->m_packetPayload)*headersize;
            if (mpeg_header.GetSize()%m_app->m_packetPayload > 0)
            {
                message_size+=headersize;
            }
            std::cout <<"Client: message size(wH)->" <<message_size <<" packetcount->" <<packetcount <<std::endl;
        }

        while (m_bytes >= message_size)
        {
            Packet message(m_buffer, message_size);
            memmove(m_buffer, &m_buffer[message_size], m_bytes - message_size);
            m_bytes -= message_size;
            m_app->MessageReceived(message);
            if (m_bytes >headersize && m_bytes<=536){ //536 is TCP MSS
                Packet headerPacket(m_buffer, headersize);
                headerPacket.RemoveHeader(mpeg_header);
                message_size = mpeg_header.GetSize() + (mpeg_header.GetSize()/m_app->m_packetPayload)*headersize;
                if (mpeg_header.GetSize()%m_app->m_packetPayload > 0)
                {
                    message_size+=headersize;
                }
                std::cout <<"Client: message size(wH)->" <<message_size <<" packetcount->" <<packetcount <<std::endl;
            }
        }
        ReadSocket(socket);
    }
}
