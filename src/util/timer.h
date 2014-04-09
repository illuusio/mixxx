#ifndef TIMER_H
#define TIMER_H

#include "util/stat.h"
#include "util/performancetimer.h"
#include "util/cmdlineargs.h"

const Stat::ComputeFlags kDefaultComputeFlags = Stat::COUNT | Stat::SUM | Stat::AVERAGE |
        Stat::MAX | Stat::MIN | Stat::SAMPLE_VARIANCE;

// A Timer that is instrumented for reporting elapsed times to StatsManager
// under a certain key. Construct with custom compute flags to get custom values
// computed for the times.
class Timer {
  public:
    Timer(const QString& key,
          Stat::ComputeFlags compute = kDefaultComputeFlags);
    void start();

    // Restart the timer returning the nanoseconds since it was last
    // started/restarted. If report is true, reports the elapsed time to the
    // associated Stat key.
    int restart(bool report);

    // Returns nanoseconds since start/restart was called. If report is true,
    // reports the elapsed time to the associated Stat key.
    int elapsed(bool report);

  protected:
    QString m_key;
    Stat::ComputeFlags m_compute;
    bool m_running;
    PerformanceTimer m_time;
};

class TimerAtMem : public Timer {
  public:
    TimerAtMem(const QString& key,
                 Stat::ComputeFlags compute = kDefaultComputeFlags)
            : Timer(key, compute) {
    }
    static inline void* operator new(size_t size, void* ptr) {
        Q_UNUSED(size);
        return ptr;
    } // TimerAtMen's default ctor implicitly called here

    static inline void operator delete(void *p, void* ptr) {
        Q_UNUSED(p);
        Q_UNUSED(ptr);
    } // TimerAtMem's dtor implicitly called at this point
};


class SuspendableTimer : public Timer {
  public:
    SuspendableTimer(const QString& key,
            Stat::ComputeFlags compute = kDefaultComputeFlags);
    void start();
    int suspend();
    void go();
    int elapsed(bool report);

  private:
    int m_leapTime;
};

class ScopedTimer {
  public:
    ScopedTimer(const char* key, int i,
                Stat::ComputeFlags compute = kDefaultComputeFlags)
            : m_pTimer(NULL),
              m_cancel(false) {
        if (CmdlineArgs::Instance().getDeveloper()) {
            initalize(QString(key), QString().number(i), compute);
        }
    }

    ScopedTimer(const char* key, const char *arg = NULL,
                Stat::ComputeFlags compute = kDefaultComputeFlags)
            : m_pTimer(NULL),
              m_cancel(false) {
        if (CmdlineArgs::Instance().getDeveloper()) {
            initalize(QString(key), arg ? QString(arg) : QString(), compute);
        }
    }

    ScopedTimer(const char* key, const QString& arg,
                Stat::ComputeFlags compute = kDefaultComputeFlags)
            : m_pTimer(NULL),
              m_cancel(false) {
        if (CmdlineArgs::Instance().getDeveloper()) {
            initalize(QString(key), arg, compute);
        }
    }

    virtual ~ScopedTimer() {
        if (m_pTimer) {
            if (!m_cancel) {
                m_pTimer->elapsed(true);
            }
            m_pTimer->~Timer();
        }
    }

    inline void initalize(const QString& key, const QString& arg,
                Stat::ComputeFlags compute = kDefaultComputeFlags) {
        QString strKey;
        if (arg.isEmpty()) {
            strKey = key;
        } else {
            strKey = key.arg(arg);
        }
        m_pTimer = new(m_timerMem) Timer(strKey, compute);
        m_pTimer->start();
    }

    void cancel() {
        m_cancel = true;
    }
  private:
    Timer* m_pTimer;
    char m_timerMem[sizeof(Timer)];
    bool m_cancel;
};

#endif /* TIMER_H */
