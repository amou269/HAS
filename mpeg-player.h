#ifndef MPEG_PLAYER_H_
#define MPEG_PLAYER_H_

#include <queue>
#include <map>
#include "ns3/ptr.h"
#include "ns3/packet.h"

namespace ns3
{
    enum
    {
        MPEG_PLAYER_PAUSED, MPEG_PLAYER_PLAYING, MPEG_PLAYER_NOT_STARTED, MPEG_PLAYER_DONE
    };
    class EvalvidClient;
    class MpegPlayer
    {
        public:
            MpegPlayer();
            virtual ~MpegPlayer();
            void ReceiveFrame(Ptr<Packet> message);
            int GetQueueSize();
            void Start();
            Time GetRealPlayTime(Time playTime);
            void inline SchduleBufferWakeup(const Time t, EvalvidClient * client)
            {
                m_bufferDelay = t;
                m_dashClient = client;
            }
            int m_state;
            Time m_interruption_time;
            int m_interrruptions;
            Time m_start_time;
            uint64_t m_totalRate;
            uint32_t m_minRate;
            uint32_t m_framesPlayed;

        private:
            void PlayFrame();
            Time m_lastpaused;
            std::queue<Ptr<Packet> > m_queue;
            Time m_bufferDelay;
            EvalvidClient * m_dashClient;
    };
}
#endif /* MPEG_PLAYER_H_ */
