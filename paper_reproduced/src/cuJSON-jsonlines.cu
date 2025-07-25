#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <cuda_runtime.h>
#include <math.h>
#include <chrono>
#include <thread>
#include <x86intrin.h>
#include <string.h>
#include <bitset>
#include <thrust/sort.h>
#include <thrust/device_ptr.h>
#include <thrust/binary_search.h>
#include <thrust/device_vector.h>
#include <thrust/copy.h>
#include <thrust/scan.h>
#include <thrust/transform.h>
#include <thrust/gather.h>
#include <thrust/extrema.h>
#include <thrust/partition.h>
#include <thrust/execution_policy.h>
#include <inttypes.h>
#include "./query/query_iterator.cpp"
#include <thrust/host_vector.h>
#include <device_launch_parameters.h>
#include <vector>
#include <cstring>



// #include "./12-GJSON-Class.cuh"

#define         MAXLINELENGTH     1073741824   //4194304 8388608 33554432 67108864 134217728 201326592 268435456 536870912 805306368 1073741824// Max record size
                                              //4MB       8MB     32BM    64MB      128MB    192MB     256MB     512MB     768MB       1GB
#define         BUFSIZE           1073741824   //4194304 8388608 33554432 67108864 134217728 201326592 268435456 536870912 805306368 1073741824

#define BLOCKSIZE 256

#define OPENBRACKET 91
#define CLOSEBRACKET 93
#define OPENBRACE 123
#define CLOSEBRACE 125
#define I 73

#define ROW1 1
#define ROW2 2
#define ROW3 3
#define ROW4 4
#define ROW5 5

using namespace std;
using namespace std::chrono;


// This structure is used to encapsulate input (output information) data and metadata for processing through functions.
struct inputStartStruct {
    uint32_t size;               // The total size of the input data (in bytes).
    int result_size;             // The size of the result array or buffer to store processing output.
    uint8_t* block;              // Pointer to the input data block to be processed.
    int32_t* res;                // Pointer to the result array where processed output will be stored.
    int lastChunkIndex;          // Index of the last processed character in the chunk of block.
    int lastStructuralIndex;     // Index of the last processed structural element in the json data.
};

// This structure is used to record timing data for various stages of processing.
struct time_cost_EE {
    float EE_t;                // End-to-end timing for processing each chunk.
    float EE_t_val;            // End-to-end timing for the validation phase.
    float EE_t_tok;            // End-to-end timing for the tokenization phase.
    float EE_t_pars;           // End-to-end timing for the parsing phase.
    float copy_start;          // Time taken to copy a chunk of data from host to device.
    float copy_start_total;    // Total time taken to copy all data from host to device.
    float copy_end;            // Time taken to copy a chunk of data from device to host.
    float copy_end_total;      // Total time taken to copy all data from device to host.
    float EE_total;            // Total end-to-end processing time, including all phases.
};

// Initialize the timing structure with zero values.
struct time_cost_EE time_EE = {0, 0, 0, 0, 0, 0, 0, 0, 0};



// _______________________Helpful__Functions_______________________

// Struct to check if a given integer is equal to 1.
struct is_one {
    __host__ __device__ // Can be called from both host (CPU) and device (GPU) code.
    bool operator()(const int x) { 
        return (x == 1); // Returns true if x is 1.
    }
};

// Struct to check if a character is an opening brace or bracket.
struct is_opening {
    __host__ __device__ // Can be called from both host (CPU) and device (GPU) code.
    bool operator()(char x) {
        return (x == OPENBRACE) || (x == OPENBRACKET); // Returns true for '{' or '['.
    }
};

// Struct to check if a character is a closing brace or bracket.
struct is_closing {
    __host__ __device__ // Can be called from both host (CPU) and device (GPU) code.
    bool operator()(char x) {
        return (x == CLOSEBRACE) || (x == CLOSEBRACKET); // Returns true for '}' or ']'.
    }
};

// Struct to decrease an integer by 1.
struct decrease {
    __host__ __device__ // Can be called from both host (CPU) and device (GPU) code.
    int operator()(int x) {
        return x - 1; // Decreases the input integer by 1.
    }
};

// Struct to increase an integer by 1.
struct increase {
    __host__ __device__ // Can be called from both host (CPU) and device (GPU) code.
    int operator()(int x) {
        return x + 1; // Increases the input integer by 1.
    }
};

// Inline device function to compute the prefix XOR for a 32-bit integer.
// This function performs XOR-based prefix computations for efficiency.
__device__ __forceinline__
uint32_t prefix_xor(uint32_t x) {
    x ^= (x << 1);   // XOR with left-shifted version by 1 bit.
    x ^= (x << 2);   // XOR with left-shifted version by 2 bits.
    x ^= (x << 4);   // XOR with left-shifted version by 4 bits.
    x ^= (x << 8);   // XOR with left-shifted version by 8 bits.
    x ^= (x << 16);  // XOR with left-shifted version by 16 bits.
    return x;        // Returns the resulting XOR value.
}

// Inline device function to compute the prefix XOR for a 64-bit integer.
// This function performs XOR-based prefix computations for efficiency.
__device__ __forceinline__
uint64_t prefix_xor64(uint64_t x) {
    x ^= (x << 1);   // XOR with left-shifted version by 1 bit.
    x ^= (x << 2);   // XOR with left-shifted version by 2 bits.
    x ^= (x << 4);   // XOR with left-shifted version by 4 bits.
    x ^= (x << 8);   // XOR with left-shifted version by 8 bits.
    x ^= (x << 16);  // XOR with left-shifted version by 16 bits.
    x ^= (x << 32);  // XOR with left-shifted version by 32 bits.
    return x;        // Returns the resulting XOR value.
}


// _______________________Debug__Functions_______________________

// _______________________Device_Functions_______________________
// Converts a single byte (8 bits) into its binary string representation.
// Returns a pointer to a shared memory string containing the binary representation.
__device__
const char* byteToBinary(uint8_t byte) {
    __shared__ char binary[9]; // Shared memory buffer for binary string (ensure no race conditions).
    binary[8] = '\0'; // Null terminator for the binary string.

    for (int i = 7; i >= 0; --i) {
        binary[i] = (byte & 0x01) ? '1' : '0'; // Extract the least significant bit.
        byte >>= 1; // Shift right to process the next bit.
    }

    return binary;
}

// Converts a 32-bit unsigned integer into its binary string representation.
// The output buffer `out` must have at least 33 characters (32 bits + null terminator).
__device__
void u32ToBinary(uint32_t num, char* out) {
    out[32] = '\0'; // Null terminator for the binary string.
    for (int i = 31; i >= 0; --i) {
        out[i] = (num & 0x01) ? '1' : '0'; // Extract the least significant bit.
        num >>= 1; // Shift right to process the next bit.
    }
}


// _______________________Host_Functions_for_Debugging_GPU_Data_______________________

// Prints a 2D array of 32-bit unsigned integers stored on the GPU.
// Converts each value to its binary representation and outputs row by row.
void print_d32(uint32_t* d_data, int total_padded_32, int rows) {
    uint32_t* h_data = (uint32_t*)malloc(total_padded_32 * rows * sizeof(uint32_t)); // Host buffer.
    if (!h_data) {
        std::cerr << "Failed to allocate host memory!" << std::endl;
        return;
    }

    cudaMemcpy(h_data, d_data, total_padded_32 * rows * sizeof(uint32_t), cudaMemcpyDeviceToHost); // Copy from device to host.

    for (int i = 0; i < total_padded_32 * rows; ++i) {
        uint32_t value = h_data[i];
        for (int j = 0; j < 32; ++j) { // Print each bit of the value.
            std::cout << ((value >> j) & 1);
        }
        std::cout << std::endl;
    }

    free(h_data); // Free host memory.
}

// Prints selected portions of a 2D array of 32-bit unsigned integers stored on the GPU.
int print_d(uint32_t* input_GPU, int length, int rows) {
    uint32_t* input = (uint32_t*)malloc(sizeof(uint32_t) * length * rows); // Host buffer.
    cudaMemcpyAsync(input, input_GPU, sizeof(uint32_t) * length * rows, cudaMemcpyDeviceToHost); // Async copy to host.

    for (long i = 0; i < rows; i++) {
        for (long j = 401; j < 470 && j < length; j++) { // Print a specific range of columns.
            std::bitset<32> y(*(input + j + (i * length))); // Convert to binary using std::bitset.
            if (j == 129) printf("----129----");
            std::cout << y << ' ';
        }
        std::cout << "\n";
    }

    free(input); // Free host memory.
    return 1;
}

// Prints a 2D array of 8-bit unsigned integers stored on the host.
int print8(uint8_t* input, int length, int rows) {
    for (long i = 0; i < rows; i++) {
        for (long j = 0; j < length && j < 200; j++) { // Print up to 200 values per row.
            std::cout << *(input + j + (i * length)) << ' ';
        }
        std::cout << std::endl;
    }
    return 1;
}

// Prints a 2D array of 32-bit integers stored on the host.
int print32(int32_t* input, int length, int rows) {
    for (long i = 0; i < rows; i++) {
        for (long j = 0; j < length && j < 200; j++) { // Print up to 200 values per row.
            std::cout << *(input + j + (i * length)) << ' ';
        }
        std::cout << std::endl;
    }
    return 1;
}

// Template function to print a 2D array of 8-bit integers from the GPU.
// The array is transferred to the host before printing.
template<typename T>
int print8_d(uint8_t* input_GPU, int length, int rows) {
    uint8_t* input = (uint8_t*)malloc(sizeof(uint8_t) * length); // Host buffer.
    cudaMemcpyAsync(input, input_GPU, sizeof(uint8_t) * length, cudaMemcpyDeviceToHost); // Async copy to host.

    for (long i = 0; i < rows; i++) {
        for (long j = 0; j < 300 && j < length; j++) { // Print up to 300 values per row.
            std::cout << (T)*(input + j + (i * length)) << ' ';
        }
        std::cout << std::endl;
    }

    free(input); // Free host memory.
    return 1;
}


void printByteByByte(int32_t* data, int length) {
    for (int i = 0; i < length; ++i) {
        unsigned char* bytePointer = (unsigned char*)&data[i];
        for (int j = 0; j < sizeof(int32_t); ++j) {
            printf("%02x ", bytePointer[j]);
        }
        printf("\n");
    }
}



// ______________________check_CUDA_______________________

// Function to check the status of a CUDA API call and handle errors if any.
// If the CUDA call fails, the function prints the error message and terminates the program.
void checkCuda(cudaError_t result) {
    if (result != cudaSuccess) { // Check if the CUDA call did not succeed.
        // Print the error message associated with the CUDA error.
        fprintf(stderr, "CUDA Runtime Error: %s\n", cudaGetErrorString(result));
        
        // Exit the program with a non-zero status to indicate an error.
        exit(1);
    }
}

// __________________________Start___________________________


// _______________________Device_Codes_______________________



// prev1            --> 4 character
// result           --> source
// size             --> total size of array
// total_padded_32  --> based on size howmany thread work on that
__device__ __forceinline__
void vectorizedClassification(uint32_t block_compressed, uint32_t prev1, uint32_t& result, uint64_t size, int total_padded_32){
    constexpr const uint8_t TOO_SHORT   = 1<<0; // 00000001
                                                // The leading byte must be followed by N-1 continuation bytes, 
                                                // where N is the UTF-8 character length.
                                                // 11______ 0_______
                                                // 11______ 11______

    constexpr const uint8_t TOO_LONG    = 1<<1; // The leading byte must not be a continuation byte.
                                                // 0_______ 10______

    constexpr const uint8_t OVERLONG_2  = 1<<5; // Above U+7F for two-byte characters,
                                                // 1100000_ 10______
    constexpr const uint8_t OVERLONG_3  = 1<<2; // Above U+7FF for three-byte characters,
                                                // 11100000 100_____
    constexpr const uint8_t OVERLONG_4  = 1<<6; // Above U+7FFF for three-byte characters,
                                                // 11110000 1000____

    constexpr const uint8_t SURROGATE   = 1<<4; // The decoded character must be not be in U+D800...DFFF
                                                // 11101101 101_____

    constexpr const uint8_t TWO_CONTS   = 1<<7; // Two continious bit after each other
                                                // 10______ 10______

    constexpr const uint8_t TOO_LARGE   = 1<<3; // The decoded character must be less than or equal to U+10FFFF
                                                // 11110100 1001____
                                                // 11110100 101_____
                                                // 11110101 1001____
                                                // 11110101 101_____
                                                // 1111011_ 1001____
                                                // 1111011_ 101_____
                                                // 11111___ 1001____
                                                // 11111___ 101_____

    constexpr const uint8_t TOO_LARGE_1000 = 1<<6;
                                                // Out of the range, it must be maximum 100 if you see 0101, 011_, or 1___
                                                // 11110101 1000____
                                                // 1111011_ 1000____
                                                // 11111___ 1000____


    constexpr const uint8_t CARRY = TOO_SHORT | TOO_LONG | TWO_CONTS; 
                                                // These all have ____ in byte 1 . 10000011


    
    // SIMDJSON use table in CPU, but in GPU Table is very slow
    // we check 4 character in a single time by this:
    constexpr const uint32_t TOO_SHORT_32 = (
        ((uint32_t)TOO_SHORT)       | 
        ((uint32_t)TOO_SHORT) << 8  | 
        ((uint32_t)TOO_SHORT) << 16 | 
        ((uint32_t)TOO_SHORT) << 24
    );
    constexpr const uint32_t TOO_LONG_32 = (
        ((uint32_t)TOO_LONG)        | 
        ((uint32_t)TOO_LONG) << 8   |
        ((uint32_t)TOO_LONG) << 16  |
        ((uint32_t)TOO_LONG) << 24
    );
    constexpr const uint32_t OVERLONG_2_32 = (
        ((uint32_t)OVERLONG_2)       | 
        ((uint32_t)OVERLONG_2) << 8  | 
        ((uint32_t)OVERLONG_2) << 16 | 
        ((uint32_t)OVERLONG_2) << 24
    );
    constexpr const uint32_t OVERLONG_3_32 = (
        ((uint32_t)OVERLONG_3)       | 
        ((uint32_t)OVERLONG_3) << 8  | 
        ((uint32_t)OVERLONG_3) << 16 | 
        ((uint32_t)OVERLONG_3) << 24
    );
    constexpr const uint32_t OVERLONG_4_32 = (
        ((uint32_t)OVERLONG_4)       | 
        ((uint32_t)OVERLONG_4) << 8  | 
        ((uint32_t)OVERLONG_4) << 16 | 
        ((uint32_t)OVERLONG_4) << 24
    );
    constexpr const uint32_t SURROGATE_32 = (
        ((uint32_t)SURROGATE)       | 
        ((uint32_t)SURROGATE) << 8  | 
        ((uint32_t)SURROGATE) << 16 | 
        ((uint32_t)SURROGATE) << 24
    );
    constexpr const uint32_t TWO_CONTS_32 = (
        ((uint32_t)TWO_CONTS)       | 
        ((uint32_t)TWO_CONTS) << 8  | 
        ((uint32_t)TWO_CONTS) << 16 | 
        ((uint32_t)TWO_CONTS) << 24
    );
    constexpr const uint32_t TOO_LARGE_32 = (
        ((uint32_t)TOO_LARGE)       | 
        ((uint32_t)TOO_LARGE) << 8  | 
        ((uint32_t)TOO_LARGE) << 16 | 
        ((uint32_t)TOO_LARGE) << 24
    );
    constexpr const uint32_t TOO_LARGE_1000_32 = (
        ((uint32_t)TOO_LARGE_1000)       | 
        ((uint32_t)TOO_LARGE_1000) << 8  | 
        ((uint32_t)TOO_LARGE_1000) << 16 | 
        ((uint32_t)TOO_LARGE_1000) << 24
    );
    constexpr const uint32_t CARRY_32 = (
        ((uint32_t)CARRY)       | 
        ((uint32_t)CARRY) << 8  | 
        ((uint32_t)CARRY) << 16 |
        ((uint32_t)CARRY) << 24
    );
    


    uint32_t prev1_current = prev1;
    uint32_t byte_1 = 
        (__vcmpltu4(prev1_current, 0x80808080) & TOO_LONG_32) |
        (__vcmpgeu4(prev1_current, 0xC0C0C0C0) & TOO_SHORT_32) | 
        ( (__vcmpeq4(prev1_current, 0xC0C0C0C0) | __vcmpeq4(prev1_current, 0xC1C1C1C1)) & OVERLONG_2_32) | 
        (__vcmpeq4(prev1_current, 0xEDEDEDED) & (SURROGATE_32)) | 
        (__vcmpeq4(prev1_current, 0xE0E0E0E0) & (OVERLONG_3_32)) | 
        (__vcmpeq4(prev1_current, 0xF0F0F0F0) & (OVERLONG_4_32)) | 
        (__vcmpgtu4(prev1_current, 0xF4F4F4F4) & TOO_LARGE_1000_32) | 
        (__vcmpgtu4(prev1_current, 0xF3F3F3F3) & TOO_LARGE_32);

    byte_1 = (__vcmpeq4(byte_1, 0x00000000) & TWO_CONTS_32);
    

    uint32_t block_compressed_high = (block_compressed >> 4) & 0x0F0F0F0F; 
    // 4 khune bala ro brdshti 
    // baraye moqaysee adadi bordim daste rast k rahat tr bashe

    // to make it more easier than before, save it and use it multiple time
    uint32_t less_than_12 = __vcmpltu4(block_compressed_high, 0x0C0C0C0C);
    uint32_t byte_2_high = 
        ((__vcmpltu4(block_compressed_high, 0x08080808) | __vcmpgtu4(block_compressed_high, 0x0B0B0B0B)) & TOO_SHORT_32) |
        (less_than_12 & __vcmpgeu4(block_compressed_high, 0x08080808) & (TOO_LONG_32 | OVERLONG_2_32 | TWO_CONTS_32)) | 
        (less_than_12 & __vcmpgtu4(block_compressed_high, 0x08080808) & TOO_LARGE_32) | 
        (__vcmpeq4(block_compressed_high, 0x08080808) & (TOO_LARGE_1000_32 | OVERLONG_4_32)) | 
        (__vcmpgtu4(block_compressed_high, 0x09090909) & less_than_12 & SURROGATE_32); 


    result =   (byte_1 & byte_2_high);  
    // 0 --> okay and return secussfuly
}

// make sure it has 2 or 3 continuation
// for 3,4 Byte
__device__ __forceinline__
void continuationBytes(uint32_t prev2, uint32_t prev3, uint32_t sc, uint32_t& must32Upper_sc, uint64_t size, int total_padded_32){
    static const uint32_t third_subtract_byte =  
    // 11100000 - 1 --> 11011111 --> This is the maximum of 2 Byte, So if it’s more than this, we have 3 
        (0b11100000u-1)       | 
        (0b11100000u-1) << 8  | 
        (0b11100000u-1) << 16 | 
        (0b11100000u-1) << 24;

    static const uint32_t fourth_subtract_byte = 
        (0b11110000u-1)       | 
        (0b11110000u-1) << 8  |
        (0b11110000u-1) << 16 | 
        (0b11110000u-1) << 24;


    int index = blockIdx.x * blockDim.x + threadIdx.x;

    // the latest byte in our UTF8Bytes (character) is third or fourth
    // subtract prev2 and prev3 from third_subtract_byte and fourth_subtract_byte
    // must be 0 
    // unsign saturated subtraction 4 Byte --> 4 Byte ro parallel az ham kam mikone ya 0 mishe ya 1 
    // ma mikhaym prev2 az third_subtract_byte va prev3 az forth_subtract_byte kochak tr bashe k javab 0 bashe
    uint32_t is_third_byte  = __vsubus4(prev2, third_subtract_byte);
    uint32_t is_fourth_byte = __vsubus4(prev3, fourth_subtract_byte);


    uint32_t gt = ( __vsubss4((int32_t)(is_third_byte | is_fourth_byte), int32_t(0)) ) & 0xFFFFFFFF; 
    
    // because we are working in 32 bit, we need do this for all 4 characters
    uint32_t must32 = __vcmpgtu4(gt, 0); // gt --> hamin must32 hast o mitonim hazfesh knim

    must32Upper_sc = (must32 & 0x80808080) ^ sc;            //  sc --> output of 32 bit check
    // upper bit of each 4 character
} 

__global__ 
void checkAscii(uint32_t* blockCompressed_GPU, uint64_t size, int total_padded_32, bool* hastUTF8, int WORDS){
    int threadId = threadIdx.x;
    __shared__ uint32_t shared_flag;
    
    if(threadId == 0) shared_flag = 0;
    __syncthreads();

    int index = blockIdx.x * blockDim.x + threadId;
    int stride = blockDim.x * gridDim.x;

    for(long i = index; i< total_padded_32; i+=stride){
        int start = i*WORDS;
        #pragma unroll
        for(int j=start; j<size && j<start+WORDS; j++){
            if((blockCompressed_GPU[j] & 0x80808080) != 0) atomicOr(&shared_flag, 1); 
            // check the upper bit
            // atomic or because it works in parallel
        }
        __syncthreads();
    }
    if(threadId == 0 && shared_flag) *hastUTF8 = true;
}

__global__
void checkUTF8(uint32_t* blockCompressed_GPU, uint32_t* error_GPU, uint64_t size, int total_padded_32, int WORDS){
    /*
    - blockCompressed_GPU is a pointer to the compressed data block in GPU memory, 
    - error_GPU is a pointer to a location in GPU memory where the function will store an error code if it detects invalid UTF-8, 
    - size is the size of the data block, 
    - total_padded_32 is the total number of 32-bit words in the padded data block, and 
    - WORDS is the number of words processed by each thread in each iteration of the loop
    */
    static const uint32_t max_val = 
        (uint32_t)(0b11000000u-1 << 24) | 
        (uint32_t)(0b11100000u-1 << 16) | 
        (uint32_t)(0b11110000u-1 << 8)  | 
        (uint32_t)(255); 

    int threadId = threadIdx.x;
    __shared__ uint32_t shared_error;
    if(threadId == 0) shared_error = 0;

    __syncthreads();
    int index = blockIdx.x * blockDim.x + threadId;
    int stride = blockDim.x * gridDim.x;

    for(long i = index; i< total_padded_32; i+=stride){
        int start = i*WORDS;
        #pragma unroll
        for(int j=start; j<size && j<start+WORDS; j++){
            uint32_t current = blockCompressed_GPU[j];
            uint32_t previous = j>0 ? blockCompressed_GPU[j-1] : 0;
            uint32_t prev_incomplete = __vsubus4(previous, max_val);
            
            if((current & 0x80808080) == 0) {
                atomicExch(&shared_error, prev_incomplete);
            }else{
                uint32_t prev1, prev2, prev3;
                uint32_t sc;
                uint32_t must32Upper_sc;

                uint64_t dist = ( ((uint64_t)current) << 32) | (uint64_t) previous;
                prev1 = (uint32_t)(dist >> 3*8); // shifted by 3 byte (3 * 8 bits)
                prev2 = (uint32_t)(dist >> 2*8); // shifted by 2 byte (2 * 8 bits)
                prev3 = (uint32_t)(dist >> 1*8); // shifted by 1 byte (1 * 8 bits)

                vectorizedClassification(current, prev1, sc, size, total_padded_32); // check 1,2 Byte 
                continuationBytes(prev2, prev3, sc, must32Upper_sc, size, total_padded_32); // Check 3,4 byte

                atomicExch(&shared_error, must32Upper_sc); // return error
            }
        }
    }
    __syncthreads();
    if(threadId==0 && shared_error) *error_GPU = shared_error;
}

inline bool stage1_UTF8Validator(uint32_t * block_GPU, uint64_t size){
    // _________________INIT_________________________
    int total_padded_32 = size;

    uint32_t* general_ptr;
    cudaMallocAsync(&general_ptr, sizeof(uint32_t), 0);
    uint32_t* error_GPU = general_ptr;
    cudaMemsetAsync(error_GPU, 0, sizeof(uint32_t), 0);

  
    int total_padded_16B = (size+3)/4;
    int WORDS = 4;
    int numBlock_16B = (total_padded_16B+BLOCKSIZE-1) / BLOCKSIZE;


    bool hastUTF8 = false;
    bool* hastUTF8_GPU;
    cudaMallocAsync(&hastUTF8_GPU, sizeof(bool), 0);                  //  Allocates Memory on the Device and Returns a Pointer to the Allocated Memory.
    cudaMemsetAsync(hastUTF8_GPU, 0, sizeof(bool), 0);                //  Initializes a Block of Memory on the Device with a Specified Value


    // _________________PART_1_______________________
    checkAscii<<<numBlock_16B, BLOCKSIZE>>>(block_GPU, size, total_padded_16B, hastUTF8_GPU, WORDS);
    cudaStreamSynchronize(0);
    
    cudaMemcpyAsync(&hastUTF8, hastUTF8_GPU, sizeof(bool), cudaMemcpyDeviceToHost, 0);
    //cudaFreeAsync(hastUTF8_GPU, 0);

    if(!hastUTF8){ 
        //printf("There is no utf8 charachter\n");
        cudaFreeAsync(general_ptr, 0);
        //cudaFreeAsync(hastUTF8_GPU, 0);
        return true;
    }


    // _________________PART_2_______________________
    checkUTF8<<<numBlock_16B, BLOCKSIZE>>>(block_GPU, error_GPU, size, total_padded_16B, WORDS);
    cudaStreamSynchronize(0);

    // _________________RESULT_______________________
    // Stop event
    //t cudaEventRecord(stop, 0);
    //t cudaEventSynchronize(stop);
    //t float elapsedTime;
    //t cudaEventElapsedTime(&elapsedTime, start, stop); // that's our time!
    // Clean up:
    //t cudaEventDestroy(start);
    //t cudaEventDestroy(stop);
    //t time_cal.validation_t += elapsedTime;

    uint32_t error = 0;
    cudaMemcpyAsync(&error, error_GPU, sizeof(uint32_t), cudaMemcpyDeviceToHost, 0);
    cudaFreeAsync(general_ptr, 0);
    if(error != 0){ 
        printf("Incomplete ASCII!\n"); 
        //cudaFreeAsync(error_GPU, 0);
        //cudaFreeAsync(hastUTF8_GPU, 0);
        return false;
    }
            
    //udaFreeAsync(error_GPU, 0);
    //cudaFreeAsync(hastUTF8_GPU, 0);
    return true;

}

__global__
void bitMapCreator(uint8_t* block_GPU, uint32_t* outputSlash, uint32_t* outputQuote, uint32_t* op_GPU, uint32_t* newLine_GPU, uint64_t size, int total_padded_32){
    /*
    The purpose of this function is to create bitmaps for different characters in the data block. 
    Each bitmap represents the presence or absence of a specific character at a particular position in the data block.

    For Example:
    Let's say the data block is {"name": "John\/Doe"}. 
        {"name":"John\/Doe"}
    \:  00000000000000100000
    ":  01000010100000000010
    op: 10000001000000000001

    
    Also, let's assume that the data block has a total of 64 characters (so size = 64) and 
    total_padded_32 = 2 (assuming we have 2 32-bit elements after padding).
    The resulting bitmap will be stored in the outputSlash array. 
    The value of output1[0] will represent the bitmap for the first 32 characters, 
    and output1[1] will represent the bitmap for the remaining 32 characters.
    */


    /*
    - block_GPU: A pointer to an array of uint8_t (unsigned 8-bit integers), representing a block of data.
    - outputSlash, outputQuote, op_GPU, and newLine_GPU: Pointers to arrays of uint32_t (unsigned 32-bit integers), which will store the output results.
    - size: A uint64_t (unsigned 64-bit integer) representing the size of the data block.
    - total_padded_32: An int representing the total number of 32-bit elements after padding.
    */


    /*
    The variables blockIdx.x, blockDim.x, and threadIdx.x are used to calculate the current thread's index
    and the stride value (stride) that determines the loop iterations for each thread.
    */
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;


    for(long i = index; i< total_padded_32; i+=stride){
        // loop inside data among blocks
        // The for loop iterates over the range of total_padded_32 with a step size of stride. 
        // This loop distributes the work among different threads to process different parts of the data block.
        int start_position = i*32;
        // the variable start_position is calculated based on the current index (i) to 
        // determine the starting position of the 32-bit segment being processed.

        // These variables will store the bitmaps for different characters:    
        uint32_t res_slash = 0;     //  " / "
        uint32_t res_quote = 0;     //  " " "
        uint32_t res_op = 0;        //  " { } [ ] : ,"
        uint32_t res_newline = 0;   //  " \n "

        for (int j = start_position; j<start_position+32 && j<size; j++){
            // Loop inside blocks between threads
            // Another nested loop iterates over the range from start to start+32 and 
            // ensures that the loop doesn't go beyond the size of the data block (size).
            uint8_t block = block_GPU[j];
            // uint8_t block_low = block & 0x08; // This operation isolates the 4th bit of the block value.

            // |= : bitwise OR operation 
            // << :bitwise SHIF operation
            block == '\\' ? res_slash |= 1 << (j-start_position) : NULL; // or-shif: first or then shift 
            block == '\"' ? res_quote |= 1 << (j-start_position) : NULL;
            block == '\n' ? res_newline |= 1 << (j-start_position): NULL;
            res_op |= ((( //operands
                    block == '{' ||
                    block == '[' ||
                    block == '}' ||
                    block == ']' ||
                    block == ':' ||
                    block == ','
                    ) ? 1 : 0) << (j-start_position)) ;
        }

        // creating bit-map for this 4 results-->
        outputSlash[i] = res_slash;      // " \ "
        outputQuote[i] = res_quote;      // " " "
        op_GPU[i] = res_op;           // operands
        newLine_GPU[i] = res_newline;   // \n
    }

    /*
    Example of how it works:
    Suppose we have the following data block: 
        "Hello\\World".
    We want to create a bitmap to represent the backslash character '\\' in the data block.
    Let's assume that we are currently processing the 10th character in the data block, and (j-start_position) is 10.

    Now, when we reach the 10th character, which is the backslash '\\', the condition block == '\\' will evaluate to true.
    So, the expression res1 |= 1 << (j-start_position) will be executed. 
    Since j-start is 10 in this example, 
        the bitwise left shift operation 1 << (j-start) will shift the number 1 by 10 positions to the left, 
        resulting in the binary number 0b00000000001.

    Then, the bitwise OR operation res_slash |= ... will be performed. 
    Suppose the initial value of res_slash is 0. The operation res1 |= 0b00000000001 will update res1 by setting the 10th bit to 1.
    
    After processing the entire data block, 
    the resulting res_slash bitmap will represent the presence or absence of the backslash character '\\' at each position.

    In the following character if we found any other "\\", 
        the result of this approach again would be the seeting the (j-size_position)th bit to 1.  
    */
}

__global__
void bitMapCreatorSimd(uint32_t* block_GPU, uint8_t* outputSlash, uint8_t* outputQuote, uint8_t* op_GPU, uint8_t* open_close_bitmap, uint64_t size, int total_padded_8){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    for (int i = index; i < total_padded_8 && i < size; i += stride) {
        
        int start = i*2;
        
        uint8_t res_slash = 0;     //  " / "
        uint8_t res_quote = 0;     //  " " "
        uint8_t res_op = 0;        //  " { } [ ] : ,"
        uint8_t res_open_close = 0;   //  " \n "

        uint32_t temp_res_slash = 0;
        uint32_t temp_res_quote = 0;
        uint32_t temp_res_op = 0;
        uint32_t temp_colon_comma_newline = 0;
        uint32_t temp_open_close = 0;


        uint32_t block = block_GPU[start];

        temp_res_slash = (__vcmpeq4(block, 0x5C5C5C5C) & 0x01010101); // 00000000 00000001 00000001 00000001
        temp_res_quote = (__vcmpeq4(block, 0x22222222) & 0x01010101);
        temp_open_close = ((
                    __vcmpeq4(block, 0x5B5B5B5B) |
                    __vcmpeq4(block, 0x5D5D5D5D) |
                    __vcmpeq4(block, 0x7B7B7B7B) |
                    __vcmpeq4(block, 0x7D7D7D7D) ) & 0x01010101);

        temp_colon_comma_newline = ((
                    __vcmpeq4(block, 0x3A3A3A3A) |
                    __vcmpeq4(block, 0x2C2C2C2C) |
                    __vcmpeq4(block, 0x0A0A0A0A)) & 0x01010101);

        temp_res_op = temp_colon_comma_newline | temp_open_close;
    
        
        if(i == total_padded_8 - 1 && 4*(start + 1)  >= size ){
            for(int j = 0; j < 4; j++){
                res_slash   |= (uint8_t) (temp_res_slash >> j*7 & 0x0F) ;
                res_quote   |= (uint8_t) (temp_res_quote >> j*7 & 0x0F);
                res_op      |= (uint8_t) (temp_res_op >> j*7 & 0x0F   );
                res_open_close |= (uint8_t) (temp_open_close >> j*7 & 0x0F );
            }
            outputSlash[i] = res_slash;      // " \ "
            outputQuote[i] = res_quote;      // " " "
            op_GPU[i] = res_op;              // operands
            open_close_bitmap[i] = res_open_close;    // \n
            continue;
        }

        uint32_t temp2_res_slash = 0;
        uint32_t temp2_res_quote = 0;
        uint32_t temp2_res_op = 0;
        uint32_t temp2_colon_comma_newline = 0;
        uint32_t temp2_open_close= 0;


      

        uint32_t block_2 = block_GPU[start+1];
        // printf("index-%d -2-> %d\n", i, block_2);
        temp2_res_slash = (__vcmpeq4(block_2, 0x5C5C5C5C) & 0x01010101); // 00000001 00000001 00000001 00000001
        temp2_res_quote = (__vcmpeq4(block_2, 0x22222222) & 0x01010101);

        temp2_open_close = (
                    __vcmpeq4(block_2, 0x5B5B5B5B) |
                    __vcmpeq4(block_2, 0x5D5D5D5D) |
                    __vcmpeq4(block_2, 0x7B7B7B7B) |
                    __vcmpeq4(block_2, 0x7D7D7D7D) ) & 0x01010101;

        temp2_colon_comma_newline = (
                    __vcmpeq4(block_2, 0x3A3A3A3A) |
                    __vcmpeq4(block_2, 0x2C2C2C2C) |
                    __vcmpeq4(block_2, 0x0A0A0A0A)) & 0x01010101;

        temp2_res_op = temp2_colon_comma_newline | temp2_open_close;


        for(int j = 0; j < 4; j++){
            //   j=0     00000001               | 00000001 << 3 = 00010000
            //   j=1     00000010               | 00000010 << 3
            //   j=2     00000100               | 00000100 << 3
            //   j=3     ...
            res_slash   |= (uint8_t) ((temp_res_slash >> j*7) | ((temp2_res_slash >> j*7) << 4) );
            res_quote   |= (uint8_t) (temp_res_quote >> j*7 | ((temp2_res_quote >> j*7) << 4) );
            res_op      |= (uint8_t) (temp_res_op >> j*7    | ((temp2_res_op >> j*7) << 4) );
            res_open_close |= (uint8_t) (temp_open_close >> j*7 | ((temp2_open_close >> j*7) << 4) );
        }


        // if(index == 15) {
        //     char binary1[33], binary2[33];
        //     u32ToBinary(temp_res_quote, binary1);
        //     u32ToBinary(temp2_res_quote, binary2);
        //     printf("temp1: %s\n", binary1);
        //     printf("temp2: %s\n", binary2);
        //     printf("merge: %s\n", byteToBinary(res_quote));
        // }
        // 0-1: 0
        // 2-3: 1
        // 4-5: 2
        // 6-7: 3
        // cout << "index-" << i << "-->" << res_quote << endl;
        // printf("index-%d --> %d\n", i, res_quote);
        // storing results in the larger arrays
        outputSlash[i] = res_slash;      // " \ "
        outputQuote[i] = res_quote;      // " " "
        op_GPU[i] = res_op;              // operands
        open_close_bitmap[i] = res_open_close;    // \n
    }
}


// fusedStep2_3(): checkOverflow() + buildQuoteBitmap() + countQuotePerWord();
__global__
void fusedStep2_3(uint32_t* backslashes_bitmap, uint32_t* quote_bitmap, uint32_t* structural_quote_bitmap, int size, int total_padded_32, int WORDS){
    /*
        The findEscapedQuote function analyzes the input data block and identifies the escaped characters. 
        It processes the data in parallel, utilizing bitwise operations to detect escape sequences 
        and mark the positions of non-escaped characters. 
        The resulting information is stored in the structural_quote_bitmap array for further processing or analysis.
    */

    // odd-length sequences of backslashes means we have escape character

    // OVERFLOW IS CAME FROM HIGH OF PREVIOUS WORD TO LOW BIT OF CURRENT WORD
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for(long i = index; i< total_padded_32; i+=stride){
        int start = i*WORDS;

        // Parallel-For in GPU: 
        #pragma unroll
        for(int k=start; k<size && k<start+WORDS; k++){
            // _______________buildQuoteBitmap()_______________ : start
            uint32_t overflow = 2;
            // It is used in combination with bitwise operations to detect --> 2 mean maybe overflow maybe not

            uint32_t evenBits = 0x55555555UL; // 5 --> 0101
            uint32_t oddBits = ~evenBits;
            // This is a uint32_t constant with a value of 0x55555555UL. 
            // It represents a bitmask with 1s in all even bit positions. 

            long j=k-1;
            if(k == 0) overflow = 0;
            uint32_t current_word_quote = quote_bitmap[k];
            uint32_t backslashes = backslashes_bitmap[k];                          

            uint32_t possible_escaped_quote =  current_word_quote & (backslashes << 1 | 1);  
            // this one is for finding possible escape double qutoes that we have to check
            if(possible_escaped_quote == 0){
                structural_quote_bitmap[k] = current_word_quote;
                quote_bitmap[k] = (uint32_t) __popc(structural_quote_bitmap[k]);  // quote is total_one, we will rename it
                continue;
            }

            // _______________checkOverflow()_______________ : start
            while(overflow == 2){
                uint32_t backslash_j = backslashes_bitmap[j];                             
                // This is a uint32_t variable that stores the value of backslashes_bitmap[j]. It represents the backslashes at position j in the input data.
                uint8_t following_backslash_counts = __clz(~backslash_j); // Convert to 0-based index
                overflow = (following_backslash_counts == 32) ? 2 : following_backslash_counts & 1; 
                j--; // previous word  
            }
            // _______________checkOverflow()_______________ : end

            // has overflow at this step: 0 or 1
            // as same as SIMDJSON
            backslashes = backslashes & (~overflow);                            
            uint32_t applyEscapedChar = (backslashes << 1) | overflow;            

            // All BACKSLASHES that are at ODD LOCATION and not ESCAPED
            uint32_t oddSequence = backslashes & oddBits & ~applyEscapedChar;      
            uint32_t sequenceStartatEven = oddSequence + backslashes;           
            uint32_t invert_mask = sequenceStartatEven << 1;            
            uint32_t escaped = (evenBits ^ invert_mask) & applyEscapedChar;
            
            
            structural_quote_bitmap[k] = (~escaped) & current_word_quote;    // structural quote
            // _______________buildQuoteBitmap()_______________ : end

            // _______________countQuotePerWord()_______________: start
            quote_bitmap[k] = (uint32_t) __popc(structural_quote_bitmap[k]);    // quote is total_one, we will rename it,
            // _______________countQuotePerWord()_______________: end

        }
    }

}

// 1 WORD - Step 3:
// 1 Word + popc - scatter
__global__
void countQuotePerWord(uint32_t* real_quote_bitmap, uint32_t* prediction_GPU, int total_padded_32){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    for(long i = index; i < total_padded_32; i+=stride){
        prediction_GPU[i] = (uint32_t) __popc(real_quote_bitmap[i]);
    }
}

// 2 WORD - Step 3:
// 2 Word + popc - scatter
__global__
void countQuotePerWord64(uint64_t* real_quote_bitmap, uint64_t* prediction_GPU, int total_padded_64){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    for(long i = index; i < total_padded_64; i+=stride){
        prediction_GPU[i] = (uint64_t) __popcll(real_quote_bitmap[i]);
    }
}

// CUDA kernel where prefix_xor is called
__global__
void buildStringMask(uint32_t* quote_bitmap, uint32_t* acc_quote_cnt, uint32_t* str_mask, int total_padded_32){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for(int i = index; i < total_padded_32; i += stride){
        // Check if the prefix sum is odd (parity)
        bool parity = acc_quote_cnt[i] & 1;
        // Apply prefix_xor to quote_bitmap
        str_mask[i] = prefix_xor(quote_bitmap[i]);
        // Update str_mask[i] based on the parity
        str_mask[i] = parity ? ~str_mask[i] : str_mask[i];
    }
}

__global__
void buildStringMask64(uint64_t* quote_bitmap, uint64_t* acc_quote_cnt, uint64_t* str_mask, int total_padded_64){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for(int i = index; i < total_padded_64; i += stride){
        // Check if the prefix sum is odd (overflow)
        bool parity = acc_quote_cnt[i] & 1;

        // Apply prefix_xor64 to quote_bitmap
        str_mask[i] = prefix_xor64(quote_bitmap[i]);

        // Update str_mask[i] based on the parity
        str_mask[i] = parity ? ~str_mask[i] : str_mask[i];
    }
}

__global__
void findOutUsefulString(uint32_t* op_GPU, uint32_t* newLine_GPU, uint32_t* str_mask, uint64_t size, int total_padded_32, int WORDS){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // find all useful character such as { } [ ] : , \n out of sring.
    for(long i = index; i< total_padded_32; i+=stride){
        int start = i*WORDS;
        #pragma unroll
        for(int k=start; k<size && k<start+WORDS; k++){
            uint32_t op = op_GPU[k];                    // { } [ ] : ,
            uint32_t newLine = newLine_GPU[k];    // \n
            uint32_t in_string = str_mask[k];

            uint32_t usefulCharacter = op | newLine;
            str_mask[k] = ~in_string & usefulCharacter;
        }
    }
}


// fusedStep3_4():  buildStringMask() + removeCharacterInString() + countStructuralPerWord()
__global__
void fusedStep3_4(uint32_t* op_GPU, uint32_t* open_close_bitmap, uint32_t* str_mask, uint64_t size, int total_padded_32, int WORDS, uint32_t* structural_cnt){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // find all useful character such as { } [ ] : , \n out of sring.
    for(long i = index; i< total_padded_32; i+=stride){
        int start = i*WORDS;
        #pragma unroll
        for(int k=start; k<size && k<start+WORDS; k++){
            uint32_t all_structural = op_GPU[k];                    // { } [ ] : ,
            uint32_t open_close = open_close_bitmap[k];             // \n
            uint32_t curr_str_mask = str_mask[k];

            // ___________________removeCharacterInString()__________________ : start
            str_mask[k] = ~curr_str_mask & all_structural;              // all structural that are out string
            open_close_bitmap[k] = ~curr_str_mask & open_close;         // all open close that are out string
            // ___________________removeCharacterInString()__________________ : end

            // ___________________countStructuralPerWord()___________________ : start
            structural_cnt[k] = (uint32_t) __popc(str_mask[k]);         // total_bits is total_one, we will rename it
            // ___________________countStructuralPerWord()___________________ : end

            // ___________________countOpenClosePerWord()___________________ : start
            op_GPU[k] = (uint32_t) __popc(open_close_bitmap[k]);    // total_bits of open_close is total_one, we will rename it, we put it in op_GPU to prevenet new allocation
            // ___________________countOpenClosePerWord()___________________ : end

        }
    }
}

__global__
void count_set_bits(uint32_t* input, uint32_t* total_bits, int size, uint32_t total_padded_32, int WORDS){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for(uint32_t i = index; i< total_padded_32; i+=stride){
        //UPdate
        int start = i*WORDS;
        #pragma unroll
        for(int k=start; k<size && k<start+WORDS; k++){
            total_bits[k] = (uint32_t) __popc(input[k]);
        }
    }
}

__global__
void extractStructuralIdx( uint32_t* structural_cnt,
                 uint32_t* open_close_cnt,
                 uint32_t* out_string, 
                 uint32_t* open_close_bitmap, 
                 uint8_t* block_GPU, 
                 uint32_t* out_string_8_index_GPU, 
                 uint8_t* out_string_open_close_8_GPU, 
                 uint32_t* out_string_open_close_8_index_GPU, 
                 uint32_t size, 
                 uint32_t size_structural, 
                 uint32_t size_open_close, 
                 uint32_t total_padded_32,
                 uint64_t lastStructuralIndex,
                 uint64_t lastChunkIndex){

    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // each thread working on 32 bit, and each bit has 32 bits.
    // each bit is a single character
    // we want to convert it from bit to char
    for(uint32_t i = index; i< total_padded_32; i+=stride){
        uint32_t local_out_string = out_string[i];
        if (local_out_string == 0) continue; 

        uint32_t local_out_string_open_close = open_close_bitmap[i];


        uint32_t total_before = i > 0 ? structural_cnt[i-1] : 0;
        uint32_t total_before_open_close = i > 0 ? open_close_cnt[i-1] : 0;

        // uint32_t current_total = 0;
        // uint32_t current_total_open_close = 0;
        
        uint32_t current_total = 0;
        uint32_t current_total_open_close = 0;
        
        uint32_t k = i*32; 

        // Calculate first and last set bit positions
        uint8_t first_set_bit_pos = __ffs(local_out_string) - 1; // Convert to 0-based index
        uint8_t last_set_bit_pos = 32 - __clz(local_out_string) - 1; // Convert to 0-based index

        for (int j = first_set_bit_pos; j <= last_set_bit_pos && k + j < size; j++) {
            uint32_t adjusted_index = total_before + current_total;
            uint32_t adjusted_index_open_close = total_before_open_close + current_total_open_close;

            uint8_t current_bit = (local_out_string >> j) & 1;      
            if (current_bit == 1){
                uint8_t currentChar = block_GPU[k + j];
                // uint8_t replacementChar = (currentChar == 0x0A) ? ',' : currentChar; // check if its new line put comma instead, Check for newline character (0x0A) and set a comma in out_string_8_GPU
                // out_string_8_GPU[adjusted_index] = replacementChar; // Set the value in out_string_8_GPU
                out_string_8_index_GPU[adjusted_index] = k + j + 1 + lastChunkIndex;

                uint8_t current_bit_open_close = (local_out_string_open_close >> j) & 1;      
                if(current_bit_open_close == 1){
                    out_string_open_close_8_GPU[adjusted_index_open_close] = currentChar; // Set the value in out_string_8_GPU
                    out_string_open_close_8_index_GPU[adjusted_index_open_close] = adjusted_index;
                }
                current_total_open_close += current_bit_open_close;       // prefix_sum inside count_set_bits_open_close

            }
            current_total += current_bit;       // prefix_sum inside count_set_bits

        }  
    }
}

inline uint8_t * stage2_tokenizer(  uint8_t* block_GPU, 
                            uint64_t size, 
                            int &ret_size, 
                            uint32_t  &last_index_tokens, 
                            uint32_t  &last_index_tokens_open_close, 
                            uint32_t* &in_string_out_index_d,
                            uint32_t* &open_close_index_d,
                            uint64_t lastStructuralIndex,               // last structural index from previous chunk
                            uint64_t lastChunkIndex                     // last real json index from previous chunk
                            ){

    int total_padded_32 = (size+31)/32 ; // size be byte eshe totall padded be bit eshe
    uint8_t*  open_close_d;
    // +31 vase ine k 0 be ma nade o min 1 bde bema
    // va msln size=6 bashe --> 
    //int numBlockBySize = (size + BLOCKSIZE - 1) / BLOCKSIZE;

    // ____________________Initialize________________________
    uint32_t* general_ptr;
    cudaMallocAsync(&general_ptr, total_padded_32 * sizeof(uint32_t) * ROW5, 0);
    for (int i = 0; i < ROW5; ++i) {
        uint32_t* row_end_ptr = general_ptr + i * total_padded_32 + (total_padded_32 - 1);
        cudaMemsetAsync(row_end_ptr, 0, sizeof(uint32_t), 0);
    }
    cudaStreamSynchronize(0);

    uint32_t* quote_bitmap         = general_ptr;
    uint32_t* backslashes_bitmap   = general_ptr + total_padded_32;         
    uint32_t* open_close_bitmap    = general_ptr + total_padded_32 * ROW2;  //  { } [ ]
    uint32_t* op_GPU               = general_ptr + total_padded_32 * ROW3;  //  { } [ ] : ,

    int WORDS = 2;

    int total_padded_8B         = (total_padded_32 + 1) / 2;
    int total_padded_16B        = (total_padded_32 + 3) / 4;
    int total_padded_32_div_8   = (total_padded_32 + 7) / 8;
    int total_padded_32_div_32  = (total_padded_32 + 31) / 32;

    int total_padded_8 = (size + 7) / 8;
    int total_padded_32B = (size + 7) / 8;
    int total_padded_64 = (size + 63) / 64;

    int smallNumBlock   = (total_padded_32_div_32 + BLOCKSIZE - 1) / BLOCKSIZE;
    int smallNumBlock_8 = (total_padded_32_div_8 + BLOCKSIZE - 1) / BLOCKSIZE;

    int numBlock        = (total_padded_32 + BLOCKSIZE - 1) / BLOCKSIZE;
    int numBlock_8      = (total_padded_8 + BLOCKSIZE - 1) / BLOCKSIZE;
    int numBlock_8B     = (total_padded_8B + BLOCKSIZE - 1) / BLOCKSIZE;
    int numBlock_16B    = (total_padded_16B + BLOCKSIZE - 1) / BLOCKSIZE;
    int numBlock_32B    = (total_padded_32B + BLOCKSIZE - 1) / BLOCKSIZE;
    int numBlock_64     = (total_padded_64 + BLOCKSIZE - 1) / BLOCKSIZE;



    // Step 1: Build Character Bitmaps
    bitMapCreatorSimd<<<numBlock_8, BLOCKSIZE>>>( (uint32_t*) block_GPU, (uint8_t*) backslashes_bitmap, (uint8_t*) quote_bitmap, (uint8_t*) op_GPU, (uint8_t*) open_close_bitmap, size, total_padded_8);
    cudaStreamSynchronize(0);


    // Step 2: Build Structural Quote Bitmap + Step 3: Build String Mask Bitmap
    uint32_t* real_quote_bitmap = general_ptr + total_padded_32 * ROW4;

    // fusedStep2_3(): checkOverflow() + buildQuoteBitmap() + countQuotePerWord();
    fusedStep2_3<<<numBlock_8B, BLOCKSIZE>>>(backslashes_bitmap, quote_bitmap, real_quote_bitmap, total_padded_32, total_padded_8B, WORDS);
    cudaStreamSynchronize(0);

    // Step 3: Build String Mask Bitmap
    // Step 3a: countQuotePerWord() - handled in fusedStep2_3()
    uint32_t* quote_cnt = general_ptr;

    // Step 3b:
    thrust::exclusive_scan(thrust::cuda::par, quote_cnt, quote_cnt + (total_padded_32), quote_cnt);
    uint32_t* acc_quote_cnt = quote_cnt;

    // Step 3c:
    uint32_t* str_mask = general_ptr;
    buildStringMask<<<numBlock, BLOCKSIZE>>>(real_quote_bitmap, acc_quote_cnt, str_mask, total_padded_32);
    cudaStreamSynchronize(0);
  
    // Step 4: Generate Tokenization Outputs
    // fusedStep3_4():  [buildStringMask()] + removeCharacterInString() + countStructuralPerWord()
    uint32_t* structural_cnt = general_ptr + total_padded_32;
    fusedStep3_4<<<numBlock_8B, BLOCKSIZE>>>(op_GPU, open_close_bitmap, str_mask, total_padded_32, total_padded_8B, WORDS, structural_cnt);
    cudaStreamSynchronize(0);


    uint32_t* open_close_cnt = op_GPU;          // lets rename it for easy understanding, as same as 'structural_cnt' for open_close bitmap
    uint32_t* structural_bitmap = str_mask;     // lets rename it for easy understanding

 
    // Step 4b:
    thrust::inclusive_scan(thrust::cuda::par, structural_cnt, structural_cnt + total_padded_32, structural_cnt);
    cudaMemcpyAsync(&last_index_tokens, structural_cnt + total_padded_32 - 1, sizeof(uint32_t), cudaMemcpyDeviceToHost);
    uint32_t* acc_structural_cnt = structural_cnt;

    // Step 4c:
    thrust::inclusive_scan(thrust::cuda::par, open_close_cnt, open_close_cnt + total_padded_32, open_close_cnt);
    cudaMemcpyAsync(&last_index_tokens_open_close, open_close_cnt + total_padded_32 - 1, sizeof(uint32_t), cudaMemcpyDeviceToHost);
    uint32_t* acc_open_close_cnt = open_close_cnt;



    // Step 4d:
    int reminder = last_index_tokens % 4;    
    int padding = (4-reminder) & 3; 
    // It will always return a number between 0 and 3, 
    // which represents the number of padding bytes needed to align the size to the next multiple of 4.
    // uint64_t last_index_tokens_padded = (last_index_tokens + padding)/4;


    uint32_t* out_string_8_index_GPU; // it's going to store real index.
    cudaMallocAsync(&out_string_8_index_GPU, last_index_tokens * sizeof(uint32_t) * ROW2,0); // Row 1 for structural index, Row 2 for ending pos which will calculated in parsr
    int reminder2 = last_index_tokens_open_close % 4;    
    int padding2 = (4-reminder2) & 3; 
    // It will always return a number between 0 and 3, 
    // which represents the number of padding bytes needed to align the size to the next multiple of 4.
    // uint64_t last_index_tokens_open_close_padded = (last_index_tokens_open_close + padding2)/4;



    uint8_t* out_string_open_close_8_GPU;
    uint32_t* out_string_open_close_8_index_GPU; // it's going to store structural index, not real index
    cudaMallocAsync(&out_string_open_close_8_GPU, (last_index_tokens_open_close + padding2)  * sizeof(uint8_t),0);
    cudaMallocAsync(&out_string_open_close_8_index_GPU, last_index_tokens_open_close * sizeof(uint32_t),0);

    // extractStructuralIdx(): extractStructuralIdx() + extractOpenCloseIdx()
    extractStructuralIdx<<<numBlock, BLOCKSIZE>>>(acc_structural_cnt,       // prefix sum set bits until each word of structural
                                        acc_open_close_cnt,                 // prefix sum set bits until each word of open close
                                        structural_bitmap,                  // structural bitmap out string
                                        open_close_bitmap,                  // open close bitmap out string
                                        block_GPU,                          // real json block
                                        out_string_8_index_GPU,             // structural real index in real json file
                                        out_string_open_close_8_GPU,        // open_close byte
                                        out_string_open_close_8_index_GPU,  // structural index for each open close (not real json file)
                                        size, 
                                        last_index_tokens,                  // structural size
                                        last_index_tokens_open_close,       // open close size
                                        total_padded_32,
                                        lastStructuralIndex,                // last structural index from previous chunk
                                        lastChunkIndex);                    // last real json index from previous chunk
    cudaStreamSynchronize(0);
    cudaFreeAsync(general_ptr,0);


    in_string_out_index_d = out_string_8_index_GPU;
    ret_size = last_index_tokens; // latest index in chunk based on real json file

    open_close_d = out_string_open_close_8_GPU;
    open_close_index_d = out_string_open_close_8_index_GPU;

    return open_close_d;
}

__global__
void map_open_close(uint32_t* open_close_bitmap, uint32_t* oc_1, int oc_cnt_32, int oc_cnt){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for(int32_t i = index; i < oc_cnt_32 && i < oc_cnt ; i+=stride){
        uint32_t idx = i*4;
        uint32_t current_4_bytes = open_close_bitmap[i];

        uint32_t isOpen = (__vcmpeq4(current_4_bytes, 0x5B5B5B5B) | __vcmpeq4(current_4_bytes, 0x7B7B7B7B) ) & 0x01010101; // 01
        uint32_t isClose = (__vcmpeq4(current_4_bytes, 0x5D5D5D5D) |  __vcmpeq4(current_4_bytes, 0x7D7D7D7D) );            // FF

        oc_1[i] = (isOpen |  isClose); 
    }
}

__global__
void validate_expand(char* pair_oc, uint32_t* index_arr, uint32_t* endIdx, int oc_cnt_32, int oc_cnt, bool* error, uint64_t lastStructuralIndex){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x; 

    __shared__ uint32_t shared_error;
    if(threadIdx.x == 0) shared_error = 0;
    __syncthreads();

    for(int32_t i = index; i < oc_cnt_32; i+=stride){
        int k = i*4; 

        if( i == oc_cnt_32 - 1){
            if(k+1 >= oc_cnt){
                shared_error |= 1; 
            }else if(k+2 >= oc_cnt){
                uint32_t two_chars = pair_oc[k] | pair_oc[k+1] << 8;
                uint32_t shifted_two_char = two_chars << 8; 
                uint32_t xor_chars =  (two_chars ^ shifted_two_char) & 0x0000FF00;        
                uint32_t error_local = __vcmpeq4(xor_chars, 0x00000600);
                shared_error |= (~error_local) > 0; 
                endIdx[index_arr[k]] = index_arr[k+1] + lastStructuralIndex + 1;
            }else if(k+3 >= oc_cnt){ 
                shared_error |= 1; 
            }else{
                uint32_t four_chars = pair_oc[k] | pair_oc[k+1] << 8 | pair_oc[k+2] << 16 | pair_oc[k+3] << 24;
                uint32_t shifted_four_chars = four_chars << 8;
                uint32_t xor_chars =  (four_chars ^ shifted_four_chars) & 0xFF00FF00;        
                uint32_t error_local = __vcmpeq4(xor_chars, 0x06000600) & 0xFFFFFFFF;
                shared_error |= (~error_local) > 0; 

                endIdx[index_arr[k]] = index_arr[k+1] + lastStructuralIndex + 1;
                endIdx[index_arr[k+2]] = index_arr[k+3] + lastStructuralIndex + 1;
            }   
        }else{    
            uint32_t order_err = pair_oc[k] > pair_oc[k+1] | pair_oc[k+2] > pair_oc[k+3];
            uint32_t four_chars = pair_oc[k] | pair_oc[k+1] << 8 | pair_oc[k+2] << 16 | pair_oc[k+3] << 24;
            uint32_t shifted_four_chars = four_chars << 8;
            uint32_t xor_chars =  (four_chars ^ shifted_four_chars) & 0xFF00FF00;        
            uint32_t error_local = __vcmpeq4(xor_chars, 0x06000600) & 0xFFFFFFFF;

            shared_error |= (~error_local | order_err) > 0; 
            endIdx[index_arr[k]] = index_arr[k+1] + lastStructuralIndex + 1;
            endIdx[index_arr[k+2]] = index_arr[k+3] + lastStructuralIndex + 1;
        }
        __syncthreads();
        if (threadIdx.x == 0 && shared_error) *error = true;
    }

}

int32_t* stage3_parser(uint8_t* open_close_bitmap, int32_t** open_close_index_d,  int32_t** real_input_index_d, int oc_cnt, int structural_cnt, int & result_size, uint64_t lastStructuralIndex) {
    uint32_t* oc_idx = reinterpret_cast<uint32_t*>(*open_close_index_d);        // open_close index from structural array
    uint32_t* parsed_oc = reinterpret_cast<uint32_t*>(*real_input_index_d);     // contains two rows--> 1. structural     2. pair_pos 

    int numBlock = (structural_cnt + BLOCKSIZE - 1) / BLOCKSIZE;
    int numBlock_open_close = (oc_cnt + BLOCKSIZE - 1) / BLOCKSIZE;

    int WORDS = 4;
    int structural_cnt_32 = (structural_cnt + WORDS - 1) / WORDS;               // for times that we are working on 4 bytes instead of 1 bytes in a thread
    int numBlock_32 = (structural_cnt_32 + BLOCKSIZE - 1) / BLOCKSIZE;

    int oc_cnt_32 = (oc_cnt + WORDS - 1) / WORDS;
    int numBlock_open_close_32 = (oc_cnt_32 + BLOCKSIZE - 1) / BLOCKSIZE;


    // _______________STEP_1__(a)_________________    
    int32_t* res; // temporary result that will use in following
    uint32_t* oc_1; // output 
    cudaMallocAsync(&oc_1, oc_cnt_32*sizeof(uint32_t), 0); 
    
    map_open_close<<<numBlock_open_close_32, BLOCKSIZE>>>( (uint32_t*) open_close_bitmap, oc_1, oc_cnt_32, oc_cnt);
    cudaStreamSynchronize(0);


    uint32_t* depth = oc_1; // output 
    // // _______________STEP_1__(b)_________________
    thrust::inclusive_scan(thrust::cuda::par,  (uint8_t*) depth,  ((uint8_t*) depth) + oc_cnt,  (uint8_t*) depth); // on depth

    // // _______________STEP_2__(a)_________________
    thrust::transform_if(thrust::cuda::par, (uint8_t*) depth, ((uint8_t*) depth) + oc_cnt, open_close_bitmap, (uint8_t*) depth, decrease(), is_opening());

    // // _______________STEP_3__(b)_________________
    // Use zip iterator to combine oc_idx and open_close_bitmap
    auto zipped_begin = thrust::make_zip_iterator(thrust::make_tuple(oc_idx, open_close_bitmap));
    // Sorting based on depth using a single stable_sort_by_key
    thrust::stable_sort_by_key(thrust::cuda::par, (uint8_t*)depth, ((uint8_t*)depth) + oc_cnt, zipped_begin);

    char* pair_oc = (char *) open_close_bitmap;
    uint32_t* pair_idx = oc_idx;

    // _______________STEP_4__(a)_________________
    bool pairError = false;
    bool* pairError_GPU;
    cudaMallocAsync(&pairError_GPU, sizeof(bool), 0);                  //  Allocates Memory on the Device and Returns a Pointer to the Allocated Memory.
    cudaMemsetAsync(pairError_GPU, 0, sizeof(bool), 0);                //  Initializes a Block of Memory on the Device with a Specified Value
  
    uint32_t* pair_pos = parsed_oc + structural_cnt;
    validate_expand<<<numBlock_open_close_32, BLOCKSIZE>>>(pair_oc, pair_idx, pair_pos, oc_cnt_32, oc_cnt, pairError_GPU, lastStructuralIndex); 

    cudaStreamSynchronize(0);
    cudaMemcpyAsync(&pairError, pairError_GPU, sizeof(bool), cudaMemcpyDeviceToHost, 0);

    if(pairError){  // 0 no error, 1 error
        printf("error found!");
        exit(0);
    }

    result_size = structural_cnt;

    cudaFreeAsync(open_close_bitmap, 0);
    cudaFreeAsync(depth, 0);

    return (int32_t*) parsed_oc;
    //arr(output): ROW 1 depth (not anymore) | ROW1 Real Character Index | ROW2 End Index (for each opening)
}

// This function implements the main steps for processing a chunk of JSON data.
// It includes memory allocation, validation, tokenization, and parsing.
inline void *stages_implementation(void* inputStart) {
    // _________________INIT_________________________
    // Extract input data and metadata from the inputStartStruct.
    uint8_t* currentChunk = ((inputStartStruct *)inputStart)->block;          // Pointer to the input buffer.
    uint64_t size = ((inputStartStruct *)inputStart)->size;            // Size of the input buffer.
    uint64_t lastStructuralIndex = ((inputStartStruct *)inputStart)->lastStructuralIndex; // Last processed structural index.
    uint64_t lastChunkIndex = ((inputStartStruct *)inputStart)->lastChunkIndex;           // Last processed chunk index.

    uint8_t* block_GPU;            // GPU memory to hold the input buffer (our current chunk).
    uint8_t* open_close_bitmap;       // GPU memory for bitmaps of opening and closing characters (e.g., `{`, `}`, `[`, `]`).
    uint64_t *parse_tree;          // Placeholder for future usage of parse tree representation.

    // Calculate padding to align the buffer size to the nearest multiple of 4 bytes for optimal GPU performance.
    int reminder = size % 4;    
    int padding = (4 - reminder) & 3; // Padding bytes needed.
    uint64_t size_32 = (size + padding) / 4; // Aligned size in 32-bit units.

    // Allocate GPU memory for the input buffer with padding and initialize it to 0.
    cudaMallocAsync(&block_GPU, (size + padding) * sizeof(uint8_t), 0);
    cudaMemsetAsync(block_GPU, 0, (size + padding) * sizeof(uint8_t), 0);

    // ________________Host-to-Device_________________
    cudaEvent_t startHD, stopHD;
    cudaEventCreate(&startHD);
    cudaEventCreate(&stopHD);
    cudaEventRecord(startHD, 0); // Start timing.

    // Copy input buffer from host to GPU.
    cudaMemcpyAsync(block_GPU, currentChunk, sizeof(uint8_t) * size, cudaMemcpyHostToDevice, 0);

    cudaEventRecord(stopHD, 0); // Stop timing.
    cudaEventSynchronize(stopHD);

    // Calculate elapsed time for the transfer and add it to the total.
    float elapsedTimeHD;
    cudaEventElapsedTime(&elapsedTimeHD, startHD, stopHD);
    time_EE.copy_start += elapsedTimeHD;

    // _________________Validation___________________
    cudaEvent_t startValEE, stopValEE;
    cudaEventCreate(&startValEE);
    cudaEventCreate(&stopValEE);
    cudaEventRecord(startValEE, 0); // Start timing validation.

    // Validate the input buffer to ensure it contains valid UTF-8 encoded data.
    bool isValidUTF8 = stage1_UTF8Validator(reinterpret_cast<uint32_t *>(block_GPU), size_32);
    cudaStreamSynchronize(0); // Ensure validation is complete before proceeding.

    if (!isValidUTF8) {
        // If validation fails, terminate the program.
        exit(0);
    }

    cudaEventRecord(stopValEE, 0); // Stop timing validation.
    cudaEventSynchronize(stopValEE);

    // Calculate elapsed time for validation and add it to the total.
    float elapsedTimeVal;
    cudaEventElapsedTime(&elapsedTimeVal, startValEE, stopValEE);
    time_EE.EE_t_val += elapsedTimeVal;
    time_EE.EE_t += elapsedTimeVal;

    // __________________Tokenizer___________________
    cudaEvent_t startTokEE, stopTokEE;
    cudaEventCreate(&startTokEE);
    cudaEventCreate(&stopTokEE);
    cudaEventRecord(startTokEE, 0); // Start timing tokenization.

    // Perform tokenization on the input buffer.
    uint32_t last_index_tokens;
    uint32_t last_index_tokens_open_close;
    int ret_size = 0;
    uint32_t* tokens_index_GPU;
    uint32_t* open_close_index_GPU;

    open_close_bitmap = stage2_tokenizer(block_GPU, size, ret_size, last_index_tokens, last_index_tokens_open_close, tokens_index_GPU, open_close_index_GPU, lastStructuralIndex, lastChunkIndex);

    cudaEventRecord(stopTokEE, 0); // Stop timing tokenization.
    cudaEventSynchronize(stopTokEE);

    // Calculate elapsed time for tokenization and add it to the total.
    float elapsedTimeTok;
    cudaEventElapsedTime(&elapsedTimeTok, startTokEE, stopTokEE);
    time_EE.EE_t_tok += elapsedTimeTok;
    time_EE.EE_t += elapsedTimeTok;

    // __________________Parsing_____________________
    cudaEvent_t startParseEE, stopParseEE;
    cudaEventCreate(&startParseEE);
    cudaEventCreate(&stopParseEE);
    cudaEventRecord(startParseEE, 0); // Start timing parsing.

    // Perform parsing using the tokenized data.
    int32_t* result_GPU;
    int result_size;

    result_GPU = stage3_parser(open_close_bitmap, 
                        (int32_t **)(&open_close_index_GPU), 
                        (int32_t **)(&tokens_index_GPU), 
                        last_index_tokens_open_close, 
                        last_index_tokens, 
                        result_size,
                        lastStructuralIndex);

    // Store the size of the parsed result in the input structure.
    ((inputStartStruct *)inputStart)->result_size = result_size;

    cudaEventRecord(stopParseEE, 0); // Stop timing parsing.
    cudaEventSynchronize(stopParseEE);

    // Calculate elapsed time for parsing and add it to the total.
    float elapsedTimeParse;
    cudaEventElapsedTime(&elapsedTimeParse, startParseEE, stopParseEE);
    time_EE.EE_t_pars += elapsedTimeParse;
    time_EE.EE_t += elapsedTimeParse;

    // Clean up GPU memory allocated for the input buffer.
    cudaFreeAsync(block_GPU, 0); 

    // Return the parsed result (on GPU) as output.
    return (void *)result_GPU;
}


int32_t *mergeChunks(int32_t* res_buf_arrays[], resultStructGJSON* resultStruct, int chunkCounts){
    int32_t* resultBuffer; // cpu
    cudaMallocHost(&resultBuffer, sizeof(uint32_t)*(resultStruct->resultSizesPrefix[chunkCounts])*ROW2 + 3);   
    for(int i = 0; i <= chunkCounts; i++){
        int start_pos = 0;
        if(i > 0){
            start_pos = resultStruct->resultSizesPrefix[i-1];
        }
        // structurual
        memcpy(resultBuffer + 1 + start_pos ,                                                 res_buf_arrays[i], sizeof(int32_t)*resultStruct->resultSizes[i]);
        // pair_pos
        memcpy(resultBuffer + 1 + start_pos + resultStruct->resultSizesPrefix[chunkCounts] + 1,  res_buf_arrays[i] + resultStruct->resultSizes[i], sizeof(int32_t)*resultStruct->resultSizes[i]);
    }
    return resultBuffer;
}

inline int32_t *cuJSON(char *file,int n, resultStructGJSON* resultStruct){
    // _________________Input_________________________
    static uint8_t*  inputBuffer;    // input json buffer 

    // _________________OPEN_FILE____________________
    FILE * handle;
    if (!(handle = fopen(file,"rb"))){
        printf("file not found!\n");
        return 0;
    }

    // _________________FILE_SIZE____________________
    fseek(handle, 0, SEEK_END);   // Move to the end of the file
    long fileSize = ftell(handle); // Get the current byte offset in the file
    fseek(handle, 0, SEEK_SET);   // Move back to the beginning of the file

    // _________________CHUNK_COUNT____________________
    // compute chunk count for json lines according to BUFSIZE
    // int chunks_count = (fileSize + BUFSIZE -1) / BUFSIZE;            // best case
    int chunks_count = (fileSize / ((BUFSIZE/2) + 1));                  // worst case
    cudaMallocHost(&inputBuffer, sizeof(uint8_t)*BUFSIZE);                      // input (each chunk)
    resultStruct->chunkCount = chunks_count;


    // _________________READ_FILE_init____________________
    ssize_t  currentLineSize;
    uint8_t  *line = NULL;
    size_t   len = 0;
    uint32_t accumulatedSize = 0;
    uint32_t lines = 0;
    uint32_t lineLengths[1<<20]; //the maximum size of the array // we can convert it to 1 instead of array or remove it

    // read start of file
    int i = 0;
    int current_chunk_num = 0;
    int total_result_size = 0;          // latest index structural
    int latest_index_realJSON = 0;      // latest index realJSON


    // static int32_t* resultBuffer;           
    int32_t* res_buf_arrays[chunks_count];  // output json buffer for each chunk
    int32_t *resultBuffer;           // output json buffer in total



    // Read the JSON file line by line and add each line to the input buffer.
    // If the size of the input buffer exceeds `BUFSIZE`, process the accumulated chunk 
    // using the cuJSON algorithm and prepare for the next chunk.
    while ((currentLineSize = getline((char **)&line, &len, handle)) != -1) {        
        int potentialTotalSize = accumulatedSize + currentLineSize; // Calculate the potential total size if the current line is added.

        if (potentialTotalSize > BUFSIZE) { // Check if adding the current line exceeds the buffer size.

            // Prepare the input structure for processing the current chunk.
            inputStartStruct inputStart;
            inputStart.block = inputBuffer;                         // Assign the input buffer to the block field.
            inputStart.size = lineLengths[i-1];                     // Size of the last line added to the buffer.
            inputStart.lastChunkIndex = latest_index_realJSON;      // Last processed chunk index.    (last chunk index from previous chunk)
            inputStart.lastStructuralIndex = total_result_size;     // Accumulated result size so far (last structural index from real input from previous chunk).

            // Call the function to process the current chunk.
            resultBuffer= (int32_t*) stages_implementation((void*)&inputStart);

            
            // Allocate pinned memory on the host for storing results of the current chunk.
            cudaMallocHost(&res_buf_arrays[current_chunk_num], sizeof(int32_t) * inputStart.result_size * ROW2);

            // Measure the device-to-host memory transfer time.
            cudaEvent_t startDtoH, stopDtoH;
            cudaEventCreate(&startDtoH); 
            cudaEventCreate(&stopDtoH); 
            cudaEventRecord(startDtoH, 0);

            // Copy the processed results from the device to the host memory.

            // 'structural' array of current chunk
            cudaMemcpy(res_buf_arrays[current_chunk_num], 
                    resultBuffer, 
                    sizeof(int32_t) * (inputStart.result_size), 
                    cudaMemcpyDeviceToHost);

            // 'pair_pos' array of current chunk
            cudaMemcpy(res_buf_arrays[current_chunk_num] + inputStart.result_size, 
                    resultBuffer+ inputStart.result_size, 
                    sizeof(int32_t) * (inputStart.result_size), 
                    cudaMemcpyDeviceToHost);

            // Update the total accumulated result size and store the result sizes.
            // it will use for next chunk
            total_result_size += inputStart.result_size;
            (resultStruct->resultSizesPrefix).push_back(total_result_size); // Prefix sum of result sizes.
            (resultStruct->resultSizes).push_back(inputStart.result_size);  // Result size for this chunk.

            // Stop the timing event and calculate elapsed time for the transfer.
            cudaEventRecord(stopDtoH, 0);
            cudaEventSynchronize(stopDtoH);
            float elapsedTime;
            cudaEventElapsedTime(&elapsedTime, startDtoH, stopDtoH);
            time_EE.copy_end += elapsedTime; // Add the elapsed time to the total copy time.

            // Free device memory and synchronize the device to ensure completion for current chunk.
            cudaFree(resultBuffer);
            cudaDeviceSynchronize();

            // Update the index for the next chunk and reset the buffer state.
            latest_index_realJSON += accumulatedSize; // Update the last processed index in the JSON.
            accumulatedSize = 0;                      // Reset the buffer size.
            i = 0;                          // Reset line index.
            memcpy(inputBuffer + accumulatedSize, line, sizeof(uint8_t) * currentLineSize); // Copy the latest line to the buffer for next chunk.
            accumulatedSize = currentLineSize;                   // Set the current accumulatedSize to the size of the last line.
            lineLengths[i] = accumulatedSize;         // Record the length of the line.
            current_chunk_num++;            // Increment the chunk counter.
        } else {
            // Add the current line to the input buffer without exceeding BUFSIZE.
            memcpy(inputBuffer + accumulatedSize , line, sizeof(uint8_t) * currentLineSize);
            accumulatedSize += currentLineSize;                  // Update the total size of the buffer.
            lineLengths[i] = accumulatedSize;         // Record the length of the line.
        }
        i++; // Increment the line index.
    }


    // remaining parts that are very small (smaller than our BUFSIZE)
    if(accumulatedSize > 0){

        inputStartStruct inputStart;
        inputStart.block = inputBuffer;
        inputStart.size = lineLengths[i-1];
        inputStart.lastChunkIndex = latest_index_realJSON;
        inputStart.lastStructuralIndex = total_result_size;

        resultBuffer= (int32_t*) stages_implementation( (void*) &inputStart);

            
        cudaMallocHost(&res_buf_arrays[current_chunk_num], sizeof(int32_t)*inputStart.result_size * ROW2);   // output(all chunks together)

        cudaEvent_t startDtoH, stopDtoH;
        cudaEventCreate(&startDtoH);
        cudaEventCreate(&stopDtoH);
        cudaEventRecord(startDtoH, 0);


        cudaMemcpy(res_buf_arrays[current_chunk_num],                            resultBuffer,                          sizeof(int32_t)*(inputStart.result_size), cudaMemcpyDeviceToHost); // first and last is for [ and ]
        cudaMemcpy(res_buf_arrays[current_chunk_num] + inputStart.result_size,   resultBuffer+ inputStart.result_size, sizeof(int32_t)*(inputStart.result_size), cudaMemcpyDeviceToHost); // first and last is for [ and ]
        total_result_size += inputStart.result_size;
        (resultStruct->resultSizesPrefix).push_back(total_result_size);
        (resultStruct->resultSizes).push_back(inputStart.result_size);

        cudaEventRecord(stopDtoH, 0);
        cudaEventSynchronize(stopDtoH);
        float elapsedTime;
        cudaEventElapsedTime(&elapsedTime, startDtoH, stopDtoH);
        time_EE.copy_end += elapsedTime;

        cudaFree(resultBuffer);
        latest_index_realJSON += accumulatedSize;
        cudaDeviceSynchronize();
        
    }
    accumulatedSize = 0;


    cudaFreeHost(inputBuffer);
    fclose(handle);


    // merge all chunks together as a single output
    resultBuffer= mergeChunks(res_buf_arrays, resultStruct, current_chunk_num);


    resultStruct->totalResultSize = total_result_size + 2;
    resultStruct->fileSize = latest_index_realJSON + 2;
    resultStruct->structural = resultBuffer;
    resultStruct->pair_pos = resultBuffer+ total_result_size + 1;


    if(n == 6){
        cout << "Warmup HtoD Time:" << time_EE.copy_start << endl;
        cout << "Warmup Start Running: " << time_EE.EE_t <<endl;
        cout << "Warmup DtoH Time:" << time_EE.copy_end << endl;
    }else{
        cout << "1. H2D: \t\t" << time_EE.copy_start<<endl;
        cout << "2. Validation:\t\t" << time_EE.EE_t_val <<endl;
        cout << "3. Tokenization:\t" <<time_EE.EE_t_tok <<endl;
        cout << "4. Parser: \t\t" << time_EE.EE_t_pars <<endl;
        cout << "5. D2H: \t\t" << time_EE.copy_end<<endl;

        cout << "\nTOTAL (ms):\t\t" << time_EE.copy_start + time_EE.EE_t_val + time_EE.EE_t_tok + time_EE.EE_t_pars + time_EE.copy_end << endl;

        time_EE.EE_total += time_EE.EE_t;
        time_EE.copy_end_total += time_EE.copy_end;
        time_EE.copy_start_total += time_EE.copy_start;

    }
    time_EE.EE_t = 0;
    time_EE.copy_end = 0;
    time_EE.copy_start = 0;

    cout << "\nParsed Tree Size:\t" <<  ( resultStruct->totalResultSize);
    cout << "\nParser's Output Size:\t" <<  ( resultStruct->totalResultSize * 8 ) / 1024 / 1024 << "MB" << endl << endl;


    return resultBuffer;
}

// User side main function
int main(int argc, char **argv) {
    int32_t* result; // Pointer to store the result of the cuJSON function.

    // Check if there are any command-line arguments provided.
    if (argv[1] != NULL) {
        // Check if the user wants to run in batch mode and has provided a file path.
        if (strcmp(argv[1], "-b") == 0 && argv[2] != NULL) {
            std::cout << "Batch mode running..." << std::endl;

            // Initialize the output structure to store the parsed JSON tree data.
            resultStructGJSON parsed_tree;
            parsed_tree.bufferSize = BUFSIZE;       // Set the buffer size for the JSON data.
            parsed_tree.chunkCount = 0;            // Initialize the chunk count.
            parsed_tree.totalResultSize = 0;       // Initialize the total size of results.
            // parsed_tree.resultSizes = nullptr;     // Initialize result sizes pointer to null.
            // parsed_tree.resultSizesPrefix = nullptr; // Initialize prefix sum of result sizes to null.
            parsed_tree.structural = NULL;         // Initialize structural data pointer to null.               // output 
            parsed_tree.pair_pos = NULL;           // Initialize position of key-value pairs to null.           // output

            // Call the primary parsing function.
            // Inputs:
            // 1. File path (argv[2]): The path of the JSON file to parse.
            // 2. Number of repeats (1): Specifies how many times to repeat the parsing.
            // 3. Output structure (parsed_tree): A predefined structure to store the output of parsing.
            result = cuJSON(argv[2], 1, &parsed_tree);

            // Free pinned memory allocated for the `structural` field in the host.
            cudaFreeHost(parsed_tree.structural);
        } 
        else {
            // Inform the user about the correct command format if the input is invalid.
            std::cout << "Command should be like '-b [file path]'" << std::endl;
        }
    } else {
        // Inform the user about the required input if no arguments are provided.
        std::cout << "Please select (batch: -b): " << std::endl;
    }

    // Reset the CUDA device to ensure proper cleanup and prevent memory leaks.
    cudaDeviceReset();

    return 0; // Exit the program successfully.
}
