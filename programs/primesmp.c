// Prime SMP Test Program (v1.4, May 14 2013)
// Written by Ian Seyler @ Return Infinity
//
// This program checks all odd numbers between 3 and 'maxn' and determines if they are prime.
// On exit the program will display the execution time and how many prime numbers were found.
// Useful for testing runtime performance between Linux/BSD and BareMetal OS.
//
// BareMetal compile using GCC (Tested with 4.7) with Newlib 2.0.0
// gcc -I newlib-2.0.0/newlib/libc/include/ -c primesmp.c -o primesmp.o -DBAREMETAL
// gcc -c libBareMetal.c -o libBareMetal.o
// ld -T app.ld -o primesmp.app primesmp.o libBareMetal.o libc.a
//
// Linux/BSD compile using GCC (Tested with 4.7)
// gcc -pthread primesmp.c -o primesmp
//
// maxn = 500000	primes = 41538
// maxn = 1000000	primes = 78498
// maxn = 5000000	primes = 348513
// maxn = 10000000	primes = 664579

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef BAREMETAL
#include "libBareMetal.h"
#else
#include <pthread.h>
#include <time.h>
#endif

void *prime_process(void *param);

// primes is set to 1 since we don't calculate for '2' as it is a known prime number
unsigned long max_number=0, primes=1, local=0, lock=0, process_stage=0, processes=0, max_processes=0, singletime=0;
float speedup;

#ifndef BAREMETAL
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
#endif


int main(int argc, char *argv[])
{
	if ((argc == 1) || (argc >= 4))
	{
		printf("usage: %s max_processes max_number\n", argv[0]);
		exit(1);
	}
	else
	{
		max_processes = atoi(argv[1]);
		max_number = atoi(argv[2]);
	}

	if ((max_processes == 0) || (max_number == 0))
	{
		printf("Invalid argument(s).\n");
		printf("usage: %s max_processes max_number\n", argv[0]);
		exit(1);
	}

	printf("PrimeSMP v1.4 - Using a maximum of %ld process(es). Searching up to %ld.\n", max_processes, max_number);

	unsigned long k;

	for (processes=1; processes <= max_processes; processes*=2)
	{
		primes = 1;
		process_stage = processes;

#ifdef BAREMETAL
		unsigned long start, finish;
		start = b_get_timercounter();		// Grab the starting time
#else
		time_t start, finish;
		time(&start);				// Grab the starting time
		pthread_t worker[processes];
#endif

		// Spawn the worker processes
		for (k=0; k<processes; k++)
		{
#ifdef BAREMETAL
			b_smp_enqueue(&prime_process);
#else
			pthread_create(&worker[k], NULL, prime_process, NULL);
#endif
		}

		printf("Processing with %ld process(es)...\n", processes);

#ifdef BAREMETAL
		// Attempt to run a process on this CPU Core
		while (b_smp_queuelen() != 0)		// Check the length of the queue. If greater than 0 then try to run a queued job.
		{
			local = b_smp_dequeue();	// Grab a job from the queue. b_smp_dequeue returns the memory address of the code
			if (local != 0)			// If it was set to 0 then the queue was empty
				b_smp_run(local);	// Run the code
		}
		b_smp_wait();				// Wait for all CPU cores to finish
#else
		for (k=0; k<processes; k++)
		{
			pthread_join(worker[k], NULL);
		}
#endif

		// Print the results
#ifdef BAREMETAL
		finish = b_get_timercounter();
#else
		time(&finish);
		if (processes == 1)
		{
			singletime = difftime(finish, start);
			speedup = 1;
		}
		else
		{
			speedup = singletime / difftime(finish, start);
		}
		printf("%ld in %.0lf seconds. Speedup over 1 process: %.2lfX of maximum %ld.00X\n", primes, difftime(finish, start), speedup, processes);
#endif

	}

	return 0;
}


// prime_process() only works on odd numbers.
// The only even prime number is 2. All other even numbers can be divided by 2.
// 1 process	1: 3 5 7 ...
// 2 processes	1: 3 7 11 ...	2: 5 9 13 ...
// 3 processes	1: 3 9 15 ...	2: 5 11 17 ...	3: 7 13 19 ...
// 4 processes	1: 3 11 19 ...	2: 5 13 21 ...	3: 7 15 23 ...	4: 9 17 25...
// And so on.

void *prime_process(void *param)
{
	register unsigned long h, i, j, tprimes=0;

	// Lock process_stage, copy it to local var, subtract 1 from process_stage, unlock it.
#ifdef BAREMETAL
	b_smp_lock(lock);
#else
	pthread_mutex_lock( &mutex1 );
#endif

	i = (process_stage * 2) + 1;
	process_stage--;

#ifdef BAREMETAL
	b_smp_unlock(lock);
#else
	pthread_mutex_unlock( &mutex1 );
#endif

	h = processes * 2;

	// Process
	for(; i<=max_number; i+=h)
	{
		for(j=2; j<=i-1; j++)
		{
			if(i%j==0) break; // Number is divisible by some other number. So break out
		}
		if(i==j)
		{
			tprimes = tprimes + 1;
		}
	} // Continue loop up to max number

	// Add tprimes to primes.
#ifdef BAREMETAL
	b_smp_lock(lock);
#else
	pthread_mutex_lock( &mutex1 );
#endif

	primes = primes + tprimes;

#ifdef BAREMETAL
	b_smp_unlock(lock);
#else
	pthread_mutex_unlock( &mutex1 );
#endif
}

// EOF
