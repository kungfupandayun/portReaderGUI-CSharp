#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aes.h"



// Alternative solution for S-Box
unsigned int Alt_IsSet = 0;
unsigned char Alt_Faults_Value[17]; // Bit-flip value
unsigned int Alt_Fault_Position;
unsigned char Alt_Fault_Value;
unsigned int Alt_Num_of_BitFlips;

const char* fault_time[]= { "NO_FAULT",
                     "R1_SB", "R1_SR", "R1_MC", "R1_AK",
                     "R2_SB", "R2_SR", "R2_MC", "R2_AK",
                     "R3_SB", "R3_SR", "R3_MC", "R3_AK",
                     "R4_SB", "R4_SR", "R4_MC", "R4_AK",
                     "R5_SB", "R5_SR", "R5_MC", "R5_AK",
                     "R6_SB", "R6_SR", "R6_MC", "R6_AK",
                     "R7_SB", "R7_SR", "R7_MC", "R7_AK",
                     "R8_SB", "R8_SR", "R8_MC", "R8_AK",
                    "R9_SB", "R9_SR", "R9_MC", "R9_AK",
                    "R10_SB", "R10_SR", "R10_AK", "R10_OUT" };


int log_parser( int argc, char* argv[] )
{
   uchar fltdin[16], okdin[16], key[16];
   //unsigned int key_schedule[60];
   TFaultStructure fault;

   FILE *in_file, *out_file;
   char *REF_TEXT = "69c4e0d86a7b0430d8cdb78070b4c55a";

   int i,j;
   int iteration = 0;
   char flt_out[33];
   char spare_out[7];
   char code_out[5];
   char line[1000];

   unsigned int debugcounter = 0;

   if ( argc<2)
   {
      printf( "Syntax:\n log_parser <input_file> <output_file> [power=<Watt>] [delay=<ns>]\n\r" );
      printf( "\t NOTE : overwrites output file!\n\n\r" );
      exit( 0 );
   }

   // Check existance and open input file
   if ( (in_file=fopen(argv[1],"r")) == NULL )
   {
      printf( "Input file does not exist. Exiting.\n\n\r");
      exit( 1 );
   }


   Hex2Char( "000102030405060708090A0B0C0D0E0F", key );
   //KeyExpansion(key,key_schedule,128);
   Hex2Char( REF_TEXT, okdin );


   // Initialize all variables:
   iteration = 0;
   memset( flt_out, 0, 33 );
   memset( spare_out, 0, 7 );
   memset( code_out, 0, 5 );
   memset( line, 0, 1000 );
   Alt_IsSet = 0;

   // Open output file for writing
   out_file = fopen( argv[2], "wb" );
   fprintf( out_file, "Iteration;");
   fprintf( out_file, "spare_out;code_out;AES Output;" );
   fprintf( out_file, "OUT flt BYTES;OUT Flt Bits;START Flt Matrix;" );
   fprintf( out_file, "Num Faulty BYTES;Num Faulty bits;Fault timing;Faults Values;");
   fprintf( out_file, "1st Flt Pos;1st Flt Value;#BitFlips;Flt Mask;# Bit Resets;Reset Flt Mask;# Bit Sets;Set Flt Mask\n" );
   

   while (fgets(line,1000, in_file)!=NULL)
   {
      if ( !strncmp( line, "aes : ", strlen("aes : ") ) )
         strncpy( flt_out, line+strlen("aes : "), 32 );
      if ( !strncmp( line, "Iteration", strlen("Iteration") ) )
         sscanf( line, "Iteration #%d", &iteration );
      if ( !strncmp( line, "anr_spareout : ", strlen("anr_spareout : ") ) )
      {
         strncpy( spare_out, line+strlen("anr_spareout : "), 6 );
         //spare_out[6] = '\0';
         strncpy( code_out, &spare_out[1], 4 );
         //code_out[5] = '\0';

         // FAULT TEXT ANALYSIS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         Hex2Char( flt_out, fltdin );
         // aes_fltdecrypt( okdin, fltdin, key_schedule, 128 );
         // try to infer original fault value from error pattern
         // Analyze_Fault_DFA( &fault, okdin, fltdin, key_schedule );

         /***** DEBUG CODE ******************************************************************
         debugcounter++;
         if ( debugcounter==243 )
         {
            printf( "%d\n", debugcounter );
         }
         // ********************************************************************************/

         Analyze_Fault( &fault, okdin, fltdin, key, code_out );
         /***** DEBUG CODE ******************************************************************
         if ( fault.Num_of_Byte_Faults>1 && fault.Num_of_Byte_Faults<99 )
         {
            printf( "%d * %d . %f\n", debugcounter, REF_DELAY, REF_POWER );
         }
         // ********************************************************************************/

         fprintf( out_file,
                 "%d;=\"%s\";=\"%s\";=\"%s\";",
                 iteration,
                 spare_out, code_out, flt_out );
         // fprintf( out_file, "OUT flt BYTES;OUT Flt Bits;START Flt Matrix;" );
         fprintf( out_file, "%d;%d;\"", fault.Start_Num_ByteFaults, fault.Start_Num_BitFaults );
         for ( i=0; i<4; i++ )
         {
            for ( j=0; j<4; j++ )
               fprintf( out_file, "%2x ", fault.Start_Fault_Matrix[i+4*j] );
            if ( i<3 ) fprintf( out_file, "\n" );
         }
         fprintf( out_file, "\";" );
         // fprintf( out_file, "Num Faulty BYTES;Num Faulty BITS;Fault timing;Faults Values;");
         fprintf( out_file, "%d;%d;", fault.Num_of_Byte_Faults, fault.Num_of_Bit_Faults );
         fprintf( out_file, "%s;\"", fault_time[fault.Fault_Occurrence] ); // Timing
         for ( i=0; i<4; i++ )
         {
            for ( j=0; j<4; j++ )
               fprintf( out_file, "%2x ", fault.Faults_Value[i+4*j] );
            if ( i<3 ) fprintf( out_file, "\n" );
         }
         fprintf( out_file, "\";" );
         // fprintf( out_file, "1st Flt Pos;1st Flt Value;#BitFlips;Flt Mask;# Bit Resets;Reset Flt Mask;# Bit Sets;Set Flt Mask\n" );
         fprintf( out_file, "%d;%2x;%d;%s;%d;%s;%d;%s",
                 fault.Fault_Position, fault.Fault_Value, fault.Num_of_BitFlips, fault.Fault_Mask,
                 fault.Num_of_BitResets, fault.Fault_Reset_Mask, fault.Num_of_BitSets, fault.Fault_Set_Mask );
         if ( Alt_IsSet )
         {
            Alt_IsSet = 0;
            fprintf( out_file, ";\"" ); // Timing
            for ( i=0; i<4; i++ )
            {
               for ( j=0; j<4; j++ )
                  fprintf( out_file, "%2x ", Alt_Faults_Value[i+4*j] );
               if ( i<3 ) fprintf( out_file, "\n" );
            }
            fprintf( out_file, "\";" );
            fprintf( out_file, "%d;%2x;%d",
                 Alt_Fault_Position, Alt_Fault_Value, Alt_Num_of_BitFlips );
         }
         fprintf( out_file, "\n" );
      }
   }

   fclose( in_file );
   fclose( out_file );
   exit( 0 );
/* */
}
