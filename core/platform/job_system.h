#ifndef JOB_SYSTEM_H
#define JOB_SYSTEM_H

#include "defines.h"
#include "ring_queue.h"


typedef bool (job_start)(void*, void*);
typedef void (job_complete)(void*);


typedef enum Job_Type
{
    //can be literally anything
    JOB_TYPE_GENERAL = BITFLAG(0),
    //IO operation
    JOB_TYPE_RESOURCE_LOAD = BITFLAG(1),
    //Renderer
    JOB_TYPE_GPU_RESOURCE = BITFLAG(2),
} Job_Type;

typedef struct Job_Info
{
    Job_Type job_type;

    job_start* job_start;
    job_complete* job_complete;

    void* job_data;


}Job_Info;

#endif //JOB_SYSTEM_H
