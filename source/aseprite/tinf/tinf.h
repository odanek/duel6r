/*
 * tinf  -  tiny inflate library (inflate, gzip, zlib)
 *
 * version 1.00
 *
 * Copyright (c) 2003 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 *
 * Changed by Frantisek Veverka 2018
 *  - keeping only tinf_uncompress
 */

#ifndef TINF_H_INCLUDED
#define TINF_H_INCLUDED

#define TINF_OK             0
#define TINF_DATA_ERROR    (-3)

void tinf_init();

int tinf_uncompress(void *dest, unsigned int *destLen,
                           const void *source, unsigned int sourceLen);

#endif
