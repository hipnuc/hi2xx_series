#ifndef _DECODE_NMEA_H
#define _DECODE_NMEA_H

#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define EN_DECODE_NMEA_GGA  /* enable decode GGA */
//#define EN_DECODE_NMEA_RMC
//#define EN_DECODE_NMEA_GSV
//#define EN_DECODE_NMEA_GSA
//#define EN_DECODE_NMEA_VTG

/* NMEA parse */
#define MAXRAWLEN   (128)

/* solution quality */ 
#define SOLQ_NONE   0                   /* solution status: no solution */
#define SOLQ_FIX    1                   /* solution status: fix */
#define SOLQ_FLOAT  2                   /* solution status: float */
#define SOLQ_SBAS   3                   /* solution status: SBAS */
#define SOLQ_DGPS   4                   /* solution status: DGPS/DGNSS */
#define SOLQ_SINGLE 5                   /* solution status: single */
#define SOLQ_PPP    6                   /* solution status: PPP */
#define SOLQ_DR     7                   /* solution status: dead reconing */



typedef struct
{
    int         nbyte;                  /* number of bytes in message buffer */ 
    int         len;
    int         type;                   /* which type of nmea sentense */
    uint8_t     buf[MAXRAWLEN];         /* message raw buffer */
    double      lat,lon,alt;            /* lat: in RAD, lon: in RAD, alt:m */
    int         solq;                   /* solq: solution quality */
    int         nrcv;                   /* nrcv: num of sat used */
    char        nmea_hdr[3];           /* "GGA", "RMC", etc */
}nmea_raw_t;



int decode_nmea(nmea_raw_t *raw, uint8_t data);


#endif
