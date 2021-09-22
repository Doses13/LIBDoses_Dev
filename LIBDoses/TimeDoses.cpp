#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

static struct FrameRateInfo
{
private:
    high_resolution_clock::time_point s; //holds start value
    high_resolution_clock::time_point e; //holds end value
    double sleeptime;
public:
    double delta;
    double rate;
    double run_time = 0;
    double totalavg = 0;
    int frames = 0;

    double frameCap = 0;

    int minMaxResetInterval = 5;
    //Returns current run time
    double getRunTime()
    {
        e = high_resolution_clock::now();
        return run_time + duration_cast<duration<double>>(e - s).count();
    }
    //Returns current time in this frame
    double getFrameTime()
    {
        e = high_resolution_clock::now();
        return duration_cast<duration<double>>(e - s).count();
    }
    //Starts timer (Run this command at the beginning of each frame)
    void start()
    {
        s = high_resolution_clock::now();
    }
    //Stops timer, updates data (Run this command at the end of each frame)
    void stop()
    {
        if (frameCap != 0) 
        { 
            e = high_resolution_clock::now();
            sleeptime = (1 / frameCap) - duration_cast<duration<double>>(e - s).count();
            this_thread::sleep_for(chrono::milliseconds((int)(sleeptime*1000)));
        }
        e = high_resolution_clock::now();
        delta = duration_cast<duration<double>>(e - s).count();
        run_time += delta;
        rate = 1 / delta;
        totalavg = (totalavg * frames + rate) / (frames + 1);
        frames++;        
    }
}TimeD;