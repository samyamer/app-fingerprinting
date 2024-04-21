
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdint.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include <pthread.h>
#include <errno.h>
#include <math.h>




#define PAGE_SIZE (1<<12)

#define ROW_CONFLICT_TH 490

/* Returns Time Stamp Counter (using rdtscp function)*/
extern inline __attribute__((always_inline))
uint64_t rdtscp(void) {
  uint64_t cycles;
  asm volatile ("rdtscp\n"
		"shl $32,%%rdx\n"
		"or %%rdx, %%rax\n"		      
		: /* outputs */ "=a" (cycles));
  return cycles;
}


u_int64_t get_physical_addr(uintptr_t virtual_addr){
	int fd = open("/proc/self/pagemap", O_RDONLY);
    assert(fd >= 0);

    off_t pos = lseek(fd, (virtual_addr / PAGE_SIZE) * 8, SEEK_SET);
    assert(pos >= 0);
    uint64_t value;
    int got = read(fd, &value, 8);
    assert(got == 8);
    int rc = close(fd);
    assert(rc == 0);

    // Check the "page present" flag.
    assert(value & (1ULL << 63));

    uint64_t frame_num = value & ((1ULL << 54) - 1);
    return (frame_num * PAGE_SIZE) | (virtual_addr & (PAGE_SIZE - 1));
}


typedef struct {
    u_int64_t BA0;
    u_int64_t BA1;
    u_int64_t BA2;
    u_int64_t channel;
    u_int64_t rank;
    u_int64_t dimm;
}DramAddr;

#define PAGE_SIZE (1<<12)
#define E_OFFSET 0x01C0

bool check_consecutive(u_int64_t mem, u_int64_t size){
	u_int64_t prev = get_physical_addr(mem);
	u_int64_t phys;
	bool out = true;
	for(u_int64_t i=mem+PAGE_SIZE; i<mem+size; i+= PAGE_SIZE){
		phys = get_physical_addr(i);
        // printf("%lx\n",phys);
		// printf("Vaddr: %lx   Paddr: %lx\n",i, phys);
		if(phys != prev + PAGE_SIZE){
			// printf("NOT consecutive");
			out = false;
		}
		prev  = phys;
	}
	return out;
	
}
inline __attribute__((always_inline))  
int probe(char* addr){
    volatile unsigned int time;
    // read timestamp
    // move timestamp to another reg
    //cpuid
    // mem access
    //cpuid
    // read timestamp
    //cflush

    asm __volatile__(
        "clflush 0(%1)\n\t"
        "mfence\n\t"
        "rdtsc\n\t"
        "lfence\n\t"
        "movl %%eax, %%edi\n\t"
        "movl (%1), %%eax \n\t"
        "lfence\n\t"
        "rdtsc\n\t"
        "subl %%edi, %%eax\n\t"
        :"=a"(time)
        :"c"(addr)
        :"edi","edx"

    );

    return time;
}


// // Haswell 1 channel 1 dimms
//  void dram_address(u_int64_t phys_addr, DramAddr* addr){
//     addr->BA0 = ((phys_addr & 1<<13) >> 13) ^ ((phys_addr & 1<<17) >> 17);
//     addr->BA1 = ((phys_addr & 1<<14) >> 14) ^ ((phys_addr & 1<<18) >> 18);
//     addr->BA2 = ((phys_addr & 1<<16) >> 16) ^ ((phys_addr & 1<<20) >> 20);
//     addr->rank = ((phys_addr & 1<<15) >> 15) ^ ((phys_addr & 1<<19) >> 19);
//     addr->dimm = 0;
//     addr->channel = 0;
    
//     return;
// }

// // // Haswell 1 channel 2 dimms
// //  void dram_address(u_int64_t phys_addr, DramAddr* addr){
// //     addr->BA0 = ((phys_addr & 1<<13) >> 13) ^ ((phys_addr & 1<<18) >> 18);
// //     addr->BA1 = ((phys_addr & 1<<14) >> 14) ^ ((phys_addr & 1<<19) >> 19);
// //     addr->BA2 = ((phys_addr & 1<<17) >> 17) ^ ((phys_addr & 1<<21) >> 21);
// //     addr->rank = ((phys_addr & 1<<16) >> 16) ^ ((phys_addr & 1<<20) >> 20);
// //     addr->dimm = (phys_addr & 1<<15);
// // addr->channel = 0;
    
// //     return;
// // }

// // Haswell 2 channel 1 dimms
//  void dram_address(u_int64_t phys_addr, DramAddr* addr){
//     addr->BA0 = ((phys_addr & 1<<14) >> 14) ^ ((phys_addr & 1<<18) >> 18);
//     addr->BA1 = ((phys_addr & 1<<15) >> 15) ^ ((phys_addr & 1<<19) >> 19);
//     addr->BA2 = ((phys_addr & 1<<17) >> 17) ^ ((phys_addr & 1<<21) >> 21);
//     addr->rank = ((phys_addr & 1<<16) >> 16) ^ ((phys_addr & 1<<20) >> 20);
//     addr->dimm = 0;
//     addr->channel = ((phys_addr & 1<<7) >> 7) ^ ((phys_addr & 1<<8) >> 8) ^ ((phys_addr & 1<<9) >> 9) ^ ((phys_addr & 1<<12) >> 12) ^ ((phys_addr & 1<<13) >> 13) ^ ((phys_addr & 1<<18) >> 18) ^ ((phys_addr & 1<<19) >> 19);
    
//     return;
// }



// // Haswell 2 channel 2 dimms
//  void dram_address(u_int64_t phys_addr, DramAddr* addr){
//     addr->BA0 = ((phys_addr & 1<<14) >> 14) ^ ((phys_addr & 1<<19) >> 19);
//     addr->BA1 = ((phys_addr & 1<<15) >> 15) ^ ((phys_addr & 1<<20) >> 20);
//     addr->BA2 = ((phys_addr & 1<<18) >> 18) ^ ((phys_addr & 1<<22) >> 22);
//     addr->rank = ((phys_addr & 1<<17) >> 17) ^ ((phys_addr & 1<<21) >> 21);
//     addr->dimm = (phys_addr & 1<<16);
//     addr->channel = ((phys_addr & 1<<7) >> 7) ^ ((phys_addr & 1<<8) >> 8) ^ ((phys_addr & 1<<9) >> 9) ^ ((phys_addr & 1<<12) >> 12) ^ ((phys_addr & 1<<13) >> 13) ^ ((phys_addr & 1<<18) >> 18) ^ ((phys_addr & 1<<19) >> 19);
    
//     return;
// }

inline __attribute__((always_inline)) uint64_t 
row_conflict_time(u_int64_t a, u_int64_t b){
    u_int64_t time;

    asm volatile(
        "clflush 0(%1)\n\t"
        "clflush 0(%2)\n\t"
        "lfence\n\t"
        "rdtscp\n\t"
        "lfence\n\t"	
        "mov %%rax, %%rdi\n\t"
        "mov (%1), %%rax\n\t"
        "mov (%2), %%rax\n\t"

        "lfence\n\t"
        "rdtscp\n\t"
        "lfence\n\t"
        "sub %%rdi, %%rax\n\t"

        : "=a"(time)
	    :  "b"(a) , "S"(b)
	    : "rdi", "rdx" ,"rcx"  
    );

    return time;

}


void main(void){
    

    // eat mem
    

    
    int num_pages=100;
    char* mem;
    int offset =0;
    while(1){

        do{
            mem =  (char *) mmap(NULL, num_pages*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE| MAP_POPULATE, -1, 0);
        }while(!check_consecutive(mem,num_pages*PAGE_SIZE));
            
        
        
        // row conflict timing

    

        // DramAddr* dram = (DramAddr*) malloc(sizeof(DramAddr));
        // dram_address(get_physical_addr((uintptr_t) mem), dram);
        // printf("page 0 memory\n");
        // printf("BA0:%lx BA1:%lx BA2:%lx rank:%lx channel:%lx dimm:%lx\n", dram->BA0,dram->BA1, dram->BA2, dram->rank,dram->channel, dram->dimm);
        // printf("---------------------------------------------------------------\n");

        
        uint64_t phys;
        for(int i=2; i<34; i+=2){
            u_int64_t acc_time = 0;
            u_int64_t time;
            int j=0;
            while(j<8000){
                time = row_conflict_time((u_int64_t) (mem), (u_int64_t) (mem + i*PAGE_SIZE));
                if(time > 1000){continue;}
                j++;
                acc_time+=time;
            }
            acc_time = acc_time/8000;
            // printf("Page %d (%lx) Acc Time: %ld\n", i, get_physical_addr((uintptr_t) (mem + i*PAGE_SIZE)),acc_time);
            if(acc_time > ROW_CONFLICT_TH){
                // printf("Found conflict\n");
                offset = i;
                // printf("Offset %d\n", offset);
                break;
            }
            
            // if(acc_time > 300){
            //     printf("%lu\n",acc_time);
            //     phys = get_physical_addr((uintptr_t) (mem+i*PAGE_SIZE));
            //     dram_address(phys, dram);
            //     printf("BA0:%lx BA1:%lx BA2:%lx rank:%lx channel:%lx dimm:%lx\n", dram->BA0,dram->BA1, dram->BA2, dram->rank,dram->channel, dram->dimm);
            //     printf("%lx\n",phys);
            //     printf("--------------------------\n");

            // }
            
        
        }

        if(offset<30){
            munmap(mem,num_pages*PAGE_SIZE);
        }else{
            break;
        }
    }

    // printf("Found offset >= 30\n");
    // all the pages up to offset -1 are in the banks that will be probed

    char* addresses[15];

    for(int i=0; i<15; i++){
        addresses[i] = mem + PAGE_SIZE*(i*2);
    }
    // printf("here");

    // PROBE(F+R)
    // int acc_times[16];
    // for(int i=0; i<16; i++){
    //     acc_times[i] = probe(addresses[i]);
    // }

    // for(int i=0; i<16; i++){
    //     printf("%d\n", acc_times[i]);
    // }

   

    // probe every 2ms cycles
    
    int num =0;

    int num_probes = 25000;
    uint64_t acc_times[num_probes*15];
    uint64_t start = rdtscp();
    while(num < num_probes ){
        while(rdtscp() < start + 0x300000){}
        for(int i=0; i<15; i++){
            acc_times[num*15 + i] = probe(addresses[i]);
        }
        num++;
        start = rdtscp();
    }
    

    for(int j=0; j< num_probes;j++ ){
        for(int i=0; i<15; i++){
            if(i<14){
                if(acc_times[j*15 + i] < ROW_CONFLICT_TH){
                    printf("1,");
                }else{
                    printf("0,");
                }
            }else{
                if(acc_times[j*15 + i] < ROW_CONFLICT_TH){
                    printf("1");
                }else{
                    printf("0");
                }
            }
        }
        printf("\n");
    }





    







   


    
}
