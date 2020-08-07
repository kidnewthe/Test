#define ENABLE_API_PROFILER 1     // Comment this line to disable the profiler

#if ENABLE_API_PROFILER

//------------------------------------------------------------------
// A class for local variables created on the stack by the API_PROFILER macro:
//------------------------------------------------------------------
class APIProfiler
{
public:
    //------------------------------------------------------------------
    // A structure for each thread to store information about an API:
    //------------------------------------------------------------------
    struct ThreadInfo
    {
        INT64 lastReportTime;
        INT64 accumulator;   // total time spent in target module since the last report
        INT64 hitCount;      // number of times the target module was called since last report，析构函数中增加
        const char *name;    // the name of the target module
    };

private:
    INT64 m_start;
    ThreadInfo *m_threadInfo;

    static float s_ooFrequency;      // 1.0 divided by QueryPerformanceFrequency()
    static INT64 s_reportInterval;   // length of time between reports
    void Flush(INT64 end);
    
public:
    __forceinline APIProfiler(ThreadInfo *threadInfo)
    {
        LARGE_INTEGER start;
        QueryPerformanceCounter(&start);
        m_start = start.QuadPart;
        m_threadInfo = threadInfo;
    }

    __forceinline ~APIProfiler()
    {
        LARGE_INTEGER end;
        QueryPerformanceCounter(&end);
        m_threadInfo->accumulator += (end.QuadPart - m_start);
        m_threadInfo->hitCount++;
        if (end.QuadPart - m_threadInfo->lastReportTime > s_reportInterval)
            Flush(end.QuadPart);
    }
};


//thread 用于声明一个线程本地变量. __declspec(thread)的前缀是Microsoft添加给Visual C++编译器的一个修改符。
//它告诉编译器，对应的变量应该放入可执行文件或DLL文件中它的自己的节中。__declspec(thread)后面的变量 必须声明为函数中（或函数外）的一个全局变量或静态变量。不能声明一个类型为__declspec(thread)的局部变量。
//----------------------
// Profiler is enabled
//----------------------
#if defined(__WINDOWS__)
#define DECLARE_API_PROFILER(name) \
    extern __declspec(thread) APIProfiler::ThreadInfo __APIProfiler_##name;

#define DEFINE_API_PROFILER(name) \
    __declspec(thread) APIProfiler::ThreadInfo __APIProfiler_##name = { 0, 0, 0, #name };

#elif defined(__LINUX__)
    //这里我看说linux 的__thread 线程局部标量不支持class的调用构造、析构函数，没试过，所以也不知道这样好不好用
    //补充，不会有影响，因为用的是APIProfiler::ThreadInfo 不需要他的析构函数
    #define DECLARE_API_PROFILER(name) \
        extern __thread APIProfiler::ThreadInfo __APIProfiler_##name;

    #define DEFINE_API_PROFILER(name) \
        __thread APIProfiler::ThreadInfo __APIProfiler_##name = { 0, 0, 0, #name };

#endif

#define TOKENPASTE2(x, y) x ## y
#define TOKENPASTE(x, y) TOKENPASTE2(x, y)										//这里如果不是这样二次展开直接写成TOKENPASTE(x, y) x ## y 这样API_PROFILER(ss)会被展开为__APIProfiler_ss__LINE__不会把line变成行号
#define API_PROFILER(name) \
    APIProfiler TOKENPASTE(__APIProfiler_##name, __LINE__)(&__APIProfiler_##name)   //(&__APIProfiler_##name) 是构造函数的参数，不参与宏展开

#else

//----------------------
// Profiler is disabled
//----------------------
#define DECLARE_API_PROFILER(name)
#define DEFINE_API_PROFILER(name)
#define API_PROFILER(name)

#endif