#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <sys/resource.h>

#include <darwintest.h>

#define FILE_LIMIT 100

T_DECL(PR_22813396, "STREAM_MAX is affected by changes to RLIMIT_NOFILE")
{
	struct rlimit theLimit;
	getrlimit( RLIMIT_NOFILE, &theLimit );
	theLimit.rlim_cur = FILE_LIMIT;
	setrlimit( RLIMIT_NOFILE, &theLimit );

	long stream_max = sysconf(_SC_STREAM_MAX);
	T_EXPECT_EQ_LONG(stream_max, (long)FILE_LIMIT, "stream_max = FILE_LIMIT");

	FILE *f;
	for(int i = 3; i < stream_max; i++) {
		if((f = fdopen(0, "r")) == NULL) {
			T_FAIL("Failed after %d streams", i);
		}
	}

	f = fdopen(0, "r");
	T_EXPECT_NULL(f, "fdopen fail after stream_max streams");

	theLimit.rlim_cur = FILE_LIMIT + 1;
	setrlimit( RLIMIT_NOFILE, &theLimit );

	f = fdopen(0, "r");
	T_EXPECT_NOTNULL(f, "fdopen succeed after RLIMIT_NOFILE increased");
}
