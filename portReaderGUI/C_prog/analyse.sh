#!/bin/bash

inputFile="$1"
outputFile="$2" 


if test "$#" -ne 2; then
    echo "Cmmd Line: .\analyse InputFile OutputFile  
    Ex: InputFile=C:\\XilnxISE\\application\\BoardResult\\phase275\\byte0.txt 
        OutputFile=C:\\XilnxISE\\application\\BoardResult\\phase275\\byte0.csv"
fi


./log_parser.o inputFile outputFile
