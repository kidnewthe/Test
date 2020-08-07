#if ENABLE_API_PROFILER

static const float APIProfiler_ReportIntervalSecs = 1.0f;

float APIProfiler::s_ooFrequency = 0;
INT64 APIProfiler::s_reportInterval = 0;

//------------------------------------------------------------------
// Flush is called at the rate determined by APIProfiler_ReportIntervalSecs
//------------------------------------------------------------------

//QueryPerformanceFrequency 硬件时钟频率，在固定机器上是定制
//配合QueryPerformanceCounter 来计算出精确时间
//一般的
/*
		QueryPerformanceFrequency(&nFreq);

		QueryPerformanceCounter(&nBeginTime);

		Sleep(1000);

		QueryPerformanceCounter(&nEndTime);

		time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;

  


*/

void APIProfiler::Flush(INT64 end)
{
    // Auto-initialize globals based on timer frequency:
    if (s_reportInterval == 0)
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);           

        // s_ooFrequency = 1.0f / freq.QuadPart;   就相当于先获取除数部分，这部分也是定值。
        // 后续可以看到都是*s_ooFrequency
        s_ooFrequency = 1.0f / freq.QuadPart;


        MemoryBarrier();

        //s_reportInterval,APIProfiler_ReportIntervalSecs是1，就是1s内的时钟频率用来计算时间间隔以调用flush
        s_reportInterval = (INT64) (freq.QuadPart * APIProfiler_ReportIntervalSecs);


    }

    // Avoid garbage timing on first call by initializing a new interval:
    if (m_threadInfo->lastReportTime == 0)
    {
        m_threadInfo->lastReportTime = m_start;
        return;
    }

    // Enough time has elapsed. Print statistics to console:
    float interval = (end - m_threadInfo->lastReportTime) * s_ooFrequency;
    float measured = m_threadInfo->accumulator * s_ooFrequency;
    printf("TID 0x%x time spent in \"%s\": %.0f/%.0f ms %.1f%% %dx\n",
        GetCurrentThreadId(),
        m_threadInfo->name,
        measured * 1000,
        interval * 1000,
        100.f * measured / interval,
        m_threadInfo->hitCount);

    // Reset statistics and begin next timing interval:
    m_threadInfo->lastReportTime = end;
    m_threadInfo->accumulator = 0;
    m_threadInfo->hitCount = 0;
}

#endif