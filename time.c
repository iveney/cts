#include<time.h>
void gettime(usertime,systemtime,total)
double * usertime;
double * systemtime;
double * total;
{
	struct tms timesbuffer;
	time_t totaltime;
	time_t utime;
	time_t stime;

	time(&timesbuffer);
	utime = timesbuffer.tms_utime;
	stime = timesbuffer.tms_stime;
	totaltime = timesbuffer.tms_utime + timesbuffer.tms_stime; /* In 60th seconds */
	*usertime = (double)utime/60.0;
	*systemtime = (double)stime/60.0;
	*total = (double)totaltime/60.0;
}


