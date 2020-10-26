/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: misc.c
*
* Description: misc.c
*
**************************************************************/

#include "mfs.h"

char *inttostr(int value, char *string, int base) // For convert Integer to string
{
    char *pointer = string, *pointer1 = string, tmp_char;
    int tmp_value;

    if (base < 2 || base > 36)
    {
        *string = '\0';
        return (string);
    }

    do
    {
        tmp_value = value;
        value /= base;
        *pointer++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
    } while (value);

    if (tmp_value < 0)
    {
        *pointer++ = '-';
    }
    *pointer-- = '\0';
    while (pointer1 < pointer)
    {
        tmp_char = *pointer;
        *pointer-- = *pointer1;
        *pointer1++ = tmp_char;
    }

    return (string);
}