//
// Created by prema on 2/5/2024.
//

#ifndef HBENCH_H
#define HBENCH_H

#include <time.h>

struct Duration {
    struct timespec startTime;
    struct timespec endTime;
};

#endif
