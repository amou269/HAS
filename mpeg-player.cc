#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "http-header.h"
#include "mpeg-header.h"
#include "mpeg-player.h"
#include "evalvid-client.h"
#include <cmath>

NS_LOG_COMPONENT_DEFINE("MpegPlayer");
namespace ns3
{

    MpegPlayer::MpegPlayer():   m_state(MPEG_PLAYER_NOT_STARTED), m_interrruptions(0),m_totalRate(0),
                                m_minRate(100000000), m_framesPlayed(0), m_bufferDelay("0s")
    {
        NS_LOG_FUNCTION(this);
    }

    MpegPlayer::~MpegPlayer()
    {
        NS_LOG_FUNCTION(this);
    }

    int
    MpegPlayer::GetQueueSize()
    {
        return m_queue.size();
    }

    Time
    MpegPlayer::GetRealPlayTime(Time playTime)
    {
        NS_LOG_INFO(" Start: " << m_start_time.GetSeconds() << " Inter: " << m_interruption_time.GetSeconds()
                    << " playtime: " << playTime.GetSeconds() << " now: " << Simulator::Now().GetSeconds()
                    << " actual: " << (m_start_time + m_interruption_time + playTime).GetSeconds());

        return  m_start_time + m_interruption_time + (m_state == MPEG_PLAYER_PAUSED ?
                (Simulator::Now() - m_lastpaused) : Seconds(0)) + playTime - Simulator::Now();
    }

    void
    MpegPlayer::ReceiveFrame(Ptr<Packet> message)
    {
        NS_LOG_FUNCTION(this << message);
        NS_LOG_INFO("Received Frame " << m_state);

        Ptr<Packet> msg = message->Copy();
        m_queue.push(msg);
        if (m_state == MPEG_PLAYER_PAUSED)
        {
            NS_LOG_INFO("Play resumed");
            m_state = MPEG_PLAYER_PLAYING;
            m_interruption_time += (Simulator::Now() - m_lastpaused);
            PlayFrame();
        }
        else if (m_state == MPEG_PLAYER_NOT_STARTED)
        {
            NS_LOG_INFO("Play started");
            m_state = MPEG_PLAYER_PLAYING;
            m_start_time = Simulator::Now();
            Simulator::Schedule(Simulator::Now(), &MpegPlayer::PlayFrame, this);
        }
    }

    void
    MpegPlayer::Start(void)
    {
        NS_LOG_FUNCTION(this);
        m_state = MPEG_PLAYER_PLAYING;
        m_interruption_time = Seconds(0);
    }

    void
    MpegPlayer::PlayFrame(void)
    {
        NS_LOG_FUNCTION(this);
        if (m_state == MPEG_PLAYER_DONE)//should not happen
        {
            return;
        }
        if (m_queue.empty())
        {
            NS_LOG_INFO(Simulator::Now().GetSeconds() << " No frames to play");
            m_state = MPEG_PLAYER_PAUSED;
            m_lastpaused = Simulator::Now();
            m_interrruptions++;
            return;
        }
        Ptr<Packet> message = m_queue.front();
        m_queue.pop();

        MPEGHeader mpeg_header;
        HTTPHeader http_header;

        message->RemoveHeader(mpeg_header);
        message->RemoveHeader(http_header);

        m_totalRate += http_header.GetResolution();
        if (http_header.GetSegmentId() > 0) // Discard the first segment for the minRate
        {                                 // calculation, as it is always the minimum rate
            m_minRate =
                http_header.GetResolution() < m_minRate ?
                    http_header.GetResolution() : m_minRate;
        }
        m_framesPlayed++;
        MPEGHeader mpegHeader;
        m_queue.back()->Copy()->RemoveHeader(mpegHeader);
        Time b_t = GetRealPlayTime(mpegHeader.GetPlaybackTime()); //How much time the last frame in the queue will be bufferred

        if (m_bufferDelay > Time("0s") && b_t < m_bufferDelay && m_dashClient)
        {
            m_dashClient->Send();
            m_bufferDelay = Seconds(0);
            m_dashClient = NULL;
        }

        NS_LOG_INFO(Simulator::Now().GetSeconds() << " PLAYING FRAME: "
                    << " VidId: " << http_header.GetVideoId() << " SegId: " << http_header.GetSegmentId()
                    << " Res: " << http_header.GetResolution() << " FrameId: " << mpeg_header.GetFrameId()
                    << " PlayTime: " << mpeg_header.GetPlaybackTime().GetSeconds() << " Type: " << mpeg_header.GetType()
                    << " interTime: " << m_interruption_time.GetSeconds() << " queueLength: " << m_queue.size());
        
        Simulator::Schedule(MilliSeconds(20), &MpegPlayer::PlayFrame, this);
    }

}
