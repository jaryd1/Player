//
// Created by Jarryd on 2019/10/21.
//

#ifndef PLAYER_MEDIACLOCK_H
#define PLAYER_MEDIACLOCK_H
#include <math.h>

#define AV_NOSYNC_THRESHOLD 10.0

extern "C" {
#include <libavutil/time.h>
};

class MediaClock {

public:
    MediaClock();

    virtual ~MediaClock();

    // 初始化
    void init();

    // 获取时钟
    double getClock();

    // 设置时钟
    void setClock(double pts, double time);

    // 设置时钟
    void setClock(double pts);

    // 设置速度
    void setSpeed(double speed);

    // 同步到从属时钟
    void syncToSlave(MediaClock *slave);

    // 获取时钟速度
    double getSpeed() const;

private:
    double pts;
    double pts_drift;
    double last_updated;
    double speed;
    int paused;

    double getLastClock();
};
#endif //PLAYER_MEDIACLOCK_H
