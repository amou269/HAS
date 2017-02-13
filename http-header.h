#ifndef HTTP_TS_HEADER_H
#define HTTP_TS_HEADER_H
#include "ns3/header.h"
#include "ns3/nstime.h"

namespace ns3
{
    /**
    * \ingroup dash
    * \class HTTPTHeader
    * \brief Packet header for HTTP layer of the DASH protocol
    */

    #define HTTP_REQUEST  0
    #define HTTP_RESPONSE 1

    class HTTPHeader : public Header
    {
        public:
            HTTPHeader();
            /**
             * \param seq the sequence number
             */
            void SetSeq(uint32_t seq);
            /**
             * \return the sequence number
             */
            uint32_t GetSeq(void) const;
            /**
             * \return the time stamp
             */
            void SetMessageType(uint32_t message_type);
            uint32_t GetMessageType(void) const;
            void SetVideoId(uint32_t video_id);
            uint32_t GetVideoId(void) const;
            void SetResolution(uint32_t resolution);
            uint32_t GetResolution(void) const;
            void SetSegmentId(uint32_t segment_id);
            uint32_t GetSegmentId(void) const;
            Time GetTs(void) const;
            static TypeId GetTypeId(void);
            virtual uint32_t GetSerializedSize(void) const;

        private:
            virtual TypeId GetInstanceTypeId(void) const;
            virtual void Print(std::ostream &os) const;
            virtual void Serialize(Buffer::Iterator start) const;
            virtual uint32_t Deserialize(Buffer::Iterator start);
        
            uint32_t m_seq;
            uint64_t m_ts;
            uint32_t m_message_type;
            uint32_t m_video_id;
            uint32_t m_resolution;
            uint32_t m_segment_id;
    };
}

#endif /* HTTP_TS_HEADER_H */
