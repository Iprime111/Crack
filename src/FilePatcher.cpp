#include <cassert>
#include <cstddef> 
#include <cstdio>
#include <gtk/gtk.h>

#include "FilePatcher.h"

char *ComFilename = NULL;

const size_t CALL_ADDRESS         = 0x03;
const size_t CONTROL_BYTE_ADDRESS = 0xa2;

const char MOV_INSTRUCTION [] = {0xb9, 0x44, 0x00};
const char CONTROL_BYTE    [] = {0x01};

void PatchFile () {
    assert (ComFilename);

    FILE *comFile = fopen (ComFilename, "r+b");

    fseek  (comFile, CALL_ADDRESS, SEEK_SET);
    fwrite (MOV_INSTRUCTION, sizeof (char), 3, comFile);

    fseek  (comFile, CONTROL_BYTE_ADDRESS, SEEK_SET);
    fwrite (CONTROL_BYTE,    sizeof (char), 1, comFile);

    fclose (comFile);
}
