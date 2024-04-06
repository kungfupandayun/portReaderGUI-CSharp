/******************************************
** Advanced Encryption Standard
** Author: B-Con (b-con@b-con.us)
** Copyright/Restrictions: GNU GPL
** Disclaimer: This code is presented "as is" without any garuentees; said author holds
               liability for no problems rendered by the use of this code.
** Details: This code is the implementation of the AES algorithm, as specified by the
            NIST in in publication FIPS PUB 197, availible on the NIST website at
            http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf .
******************************************/

#define uchar unsigned char // 8-bit byte
#define uint unsigned long // 32-bit word

// This is the specified AES SBox. To look up a substitution value, put the first
// nibble in the first index (row) and the second nibble in the second index (column).

enum Fault_Timing { NO_FAULT=0, R1_SB=1, R1_SR, R1_MC, R1_AK,
                    R2_SB, R2_SR, R2_MC, R2_AK,
                    R3_SB, R3_SR, R3_MC, R3_AK,
                    R4_SB, R4_SR, R4_MC, R4_AK,
                    R5_SB, R5_SR, R5_MC, R5_AK,
                    R6_SB, R6_SR, R6_MC, R6_AK,
                    R7_SB, R7_SR, R7_MC, R7_AK,
                    R8_SB, R8_SR, R8_MC, R8_AK,
                    R9_SB, R9_SR, R9_MC, R9_AK,
                    R10_SB, R10_SR, R10_AK, R10_OUT }; // corresponding at function input
enum K_Fault_Timing { R9_RK8=101, R9_KSB, R9_RK9, R10_RK9, R10_KSB, R10_RK10 };

typedef struct{
   unsigned int Start_Num_ByteFaults;
   unsigned int Start_Num_BitFaults;
   unsigned char Start_Fault_Matrix[17];
   unsigned int Num_of_Byte_Faults;
   unsigned int Num_of_Bit_Faults;
   int Fault_Occurrence; // Corresponding to minimum umber of faulty bytes
   unsigned char Faults_Value[17]; // Bit-flip value
   // Next variables only for first identified fault
   unsigned int Fault_Position;
   unsigned char Fault_Value;
   unsigned int Num_of_BitFlips;
   unsigned char Fault_Mask[9]; // Bit-flip string mask
   unsigned int Num_of_BitResets;
   unsigned char Fault_Reset_Mask[9]; // Reset string mask (only reset bit set to 0, others to .)
   unsigned int Num_of_BitSets;
   unsigned char Fault_Set_Mask[9]; // Set string mask (only set bit are set to 1, others to .)
   } TFaultStructure;



char* trimspaces( char* str_out, const char* str_in );


/********************
** ADD ROUND KEY
********************/

// Performs the AddRoundKey step. Each round has its own pre-generated 16-byte key in the
// form of 4 integers (the "w" array). Each integer is XOR'd by one column of the state.
// Also performs the job of InvAddRoundKey(); since the function is a simple XOR process,
// it is its own inverse.
void AddRoundKey(uchar state[][4], uint w[]);


/********************
** (Inv)SubBytes
********************/

// Performs the SubBytes step. All bytes in the state are substituted with a
// pre-calculated value from a lookup table.
void SubBytes(uchar state[][4]);


void InvSubBytes(uchar state[][4]);


/********************
** (Inv)ShiftRows
********************/

// Performs the ShiftRows step. All rows are shifted cylindrically to the left.
void ShiftRows(uchar state[][4]);


// All rows are shifted cylindrically to the right.
void InvShiftRows(uchar state[][4]);

/********************
** (Inv)MixColumns
********************/

// Performs the MixColums step. The state is multiplied by itself using matrix
// multiplication in a Galios Field 2^8. All multiplication is pre-computed in a table.
// Addition is equivilent to XOR. (Must always make a copy of the column as the original
// values will be destoyed.)
void MixColumns(uchar state[][4]);

void InvMixColumns(uchar state[][4]);

/********************
** DEBUGGING FUNCTIONS
********************/

// This prints the "state" grid as a linear hex string
void printstate(uchar state[][4]);

// This prints the key (4 consecutive ints) used for a given round as a linear hex string.
void print_rnd_key(uint key[]);


/********************
** AES (En/De)Crypt
********************/

// "in" is the block of 16 sequencial bytes that is to be encrypted. "out" is the encrypted
// sequencial output. "key" is an array consisting of the KEY value that was generated
// using KeySchedule() previously. "keysize" MUST be 128, 192, 256 in size.
void aes_encrypt(uchar in[], uchar out[], uchar key[]);

void aes_decrypt(uchar in[], uchar out[], uchar key[]);


void aes_fltdecrypt(uchar in[], uchar fin[], uchar key[]);


void Hex2Char(char hex[33], uchar Char[16] );

int IsFaultHorizontal( uchar in[], uchar fin[] );

void Analyze_Fault( TFaultStructure *fault_data, uchar golden[], uchar faulty[], uchar key[], uchar errorcode[] );

void Analyze_Fault_DFA( TFaultStructure *fault_data, uchar in[], uchar fin[], uchar secretkey[], uchar errorcode[] );

int log_parser( int argc, char* argv[] );

