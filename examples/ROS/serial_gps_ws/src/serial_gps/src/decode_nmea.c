#include "decode_nmea.h"

#define MAXFIELD    32                  /* max number of fields in a record */
#define PI          3.1415926535897932	/* pi */
#define D2R         (PI/180.0)          /* deg to rad */

/* sync code */
static int sync_nmea(uint8_t *buf, uint8_t data)
{
    buf[0] = buf[1];
    buf[1] = data;
    return (buf[0] == '$' && buf[1] == 'G');
}

static double dmm2deg(double dmm)
{
    return floor(dmm/100.0)+fmod(dmm,100.0)/60.0;
}

static const int nmea_solq[]={  /* NMEA GPS quality indicator [1] */
    /* 0=Fix not available or invalidi */
    /* 1=GPS SPS Mode, fix valid */
    /* 2=Differential GPS, SPS Mode, fix valid */
    /* 3=GPS PPS Mode, fix valid */
    /* 4=Real Time Kinematic. System used in RTK mode with fixed integers */
    /* 5=Float RTK. Satellite system used in RTK mode, floating integers */
    /* 6=Estimated (dead reckoning) Mode */
    /* 7=Manual Input Mode */
    /* 8=Simulation Mode */
    SOLQ_NONE ,SOLQ_SINGLE, SOLQ_DGPS, SOLQ_PPP , SOLQ_FIX,
    SOLQ_FLOAT,SOLQ_DR    , SOLQ_NONE, SOLQ_NONE, SOLQ_NONE
};

#ifdef EN_DECODE_NMEA_GGA
/* decode NMEA GGA (Global Positioning System Fix Data) sentence -------------*/
static int decode_nmeagga(nmea_raw_t *raw, char **val, int n)
{
    volatile double tod=0.0,lat=0.0,lon=0.0,hdop=0.0,alt=0.0,msl=0.0;
    volatile double age=0.0;
    volatile char ns='N',ew='E',ua=' ',um=' ';
    int i,solq=0,nrcv=0;
    
    for (i=0;i<n;i++) {
        switch (i) {
            case  0: tod =atof(val[i]); break; /* UTC of position (hhmmss) */
            case  1: lat =atof(val[i]); break; /* Latitude (ddmm.mmm) */
            case  2: ns  =*val[i];      break; /* N=north,S=south */
            case  3: lon =atof(val[i]); break; /* Longitude (dddmm.mmm) */
            case  4: ew  =*val[i];      break; /* E=east,W=west */
            case  5: solq=atoi(val[i]); break; /* GPS quality indicator */
            case  6: nrcv=atoi(val[i]); break; /* # of satellites in use */
            case  7: hdop=atof(val[i]); break; /* HDOP */
            case  8: alt =atof(val[i]); break; /* Altitude MSL */
            case  9: ua  =*val[i];      break; /* unit (M) */
            case 10: msl =atof(val[i]); break; /* Geoid separation */
            case 11: um  =*val[i];      break; /* unit (M) */
            case 12: age =atof(val[i]); break; /* Age of differential */
        }
    }
    if ((ns!='N'&&ns!='S')||(ew!='E'&&ew!='W')) {
        //NL_TRACE("invalid nmea gga format\r\n");
        return 0;
    }

    raw->lat=(ns=='N'?1.0:-1.0)*dmm2deg(lat)*D2R;
    raw->lon=(ew=='E'?1.0:-1.0)*dmm2deg(lon)*D2R;
    raw->alt=alt+msl;
    raw->nrcv = nrcv;
    raw->solq = nmea_solq[solq];
    return 1;
}
#endif

#ifdef EN_DECODE_NMEA_RMC
static int decode_nmearmc(nmea_raw_t *raw, char **val, int n)
{
    volatile double tod=0.0,lat=0.0,lon=0.0,vel=0.0,dir=0.0,date=0.0,ang=0.0;
    char act=' ',ns='N',ew='E',mew='E',mode='A';
    int i;
    
    for (i=0;i<n;i++) {
        switch (i) {
            case  0: tod =atof(val[i]); break; /* time in utc (hhmmss) */
            case  1: act =*val[i];      break; /* A=active,V=void */
            case  2: lat =atof(val[i]); break; /* latitude (ddmm.mmm) */
            case  3: ns  =*val[i];      break; /* N=north,S=south */
            case  4: lon =atof(val[i]); break; /* longitude (dddmm.mmm) */
            case  5: ew  =*val[i];      break; /* E=east,W=west */
            case  6: vel =atof(val[i]); break; /* speed (knots) */
            case  7: dir =atof(val[i]); break; /* track angle (deg) */
            case  8: date=atof(val[i]); break; /* date (ddmmyy) */
            case  9: ang =atof(val[i]); break; /* magnetic variation */
            case 10: mew =*val[i];      break; /* E=east,W=west */
            case 11: mode=*val[i];      break; /* mode indicator (>nmea 2) */
                                      /* A=autonomous,D=differential */
                                      /* E=estimated,N=not valid,S=simulator */
        }
    }
    if ((act!='A'&&act!='V')||(ns!='N'&&ns!='S')||(ew!='E'&&ew!='W')) {
        //NL_TRACE("invalid nmea rmc format\n");
        return 0;
    }

    raw->lat=(ns=='N'?1.0:-1.0)*dmm2deg(lat)*D2R;
    raw->lon=(ew=='E'?1.0:-1.0)*dmm2deg(lon)*D2R;
    switch(mode)
    {
        case 'A': raw->solq = SOLQ_SINGLE; break;
        case 'D': raw->solq = SOLQ_DGPS; break;
        case 'R': raw->solq = SOLQ_FIX; break;
        case 'F': raw->solq = SOLQ_FLOAT; break;
        case 'E': raw->solq = SOLQ_DR; break;
        case 'S': raw->solq = SOLQ_SINGLE; break;
        default: raw->solq = SOLQ_NONE;
    }
    
    return 1;
}
#endif

#ifdef EN_DECODE_NMEA_GSA
static int decode_nmeagsa(nmea_raw_t *raw, char **val, int n)
{
    /* TBD */
    return 1;
}
#endif

#ifdef EN_DECODE_NMEA_GSV
static int decode_nmeagsv(nmea_raw_t *raw, char **val, int n)
{
    /* TBD */
    return 1;
}
#endif

#ifdef EN_DECODE_NMEA_VTG
static int decode_nmeavtg(nmea_raw_t *raw, char **val, int n)
{
    /* TBD */
    return 1;
}
#endif


static int parse_nmea(nmea_raw_t *raw)
{
    char *p,*q,*val[MAXFIELD], sum, sum2;
    int n=0;
    
    /* checksum */
    for (q=(char *)raw->buf+1,sum=0;*q && *q != '*';q++) sum^=*q;
    q++; sum2 = (int)strtol(q, NULL, 16);
    
    if(sum != sum2)
    {
        //NL_TRACE("nmea checksum failed %02X %02X\n",sum, sum2);
        return 1;
    }
        
    /* parse fields */
    for (p=(char*)raw->buf; *p && n<MAXFIELD; p=q+1) {
        if ((q=strchr(p,',')) != NULL ||(q=strchr(p,'*')) != NULL) {
            val[n++]=p; *q='\0';
        }
        else break;
    }
    if (n<1) {
        return 0;
    }
    
    /* copy NMEA header */
    memcpy(raw->nmea_hdr, raw->buf+3, 3);
    
#ifdef EN_DECODE_NMEA_GGA
    if (!strncmp(val[0]+3,"GGA", 3)) {return decode_nmeagga(raw, val+1,n-1);}
#endif
    
#ifdef EN_DECODE_NMEA_RMC
    if (!strncmp(val[0]+3,"RMC", 3)) {return decode_nmearmc(raw, val+1,n-1);}
#endif
    
#ifdef EN_DECODE_NMEA_GSV
    if (!strncmp(val[0]+3,"GSV", 3)) {return decode_nmeagsv(raw, val+1,n-1);}
#endif
    
#ifdef EN_DECODE_NMEA_GSA
    if (!strncmp(val[0]+3,"GSA", 3)) {return decode_nmeagsa(raw, val+1,n-1);}
#endif
    
#ifdef EN_DECODE_NMEA_VTG
    if (!strncmp(val[0]+3,"VTG", 3)) {return decode_nmeavtg(raw, val+1,n-1);}
#endif

    return 0;
}

int decode_nmea(nmea_raw_t *raw, uint8_t data)
{
    /* synchronize frame */
    if (raw->nbyte == 0)
    {
        if (!sync_nmea(raw->buf, data)) return 0;
        raw->nbyte = 2;
        return 0;
    }
    
    if(raw->nbyte >= MAXRAWLEN)
    {
//        NL_TRACE("ch length error: len=%d\n",raw->nbyte);
        raw->nbyte = 0;
        return -1;
    }
    
    raw->buf[raw->nbyte++] = data;

    if(data != 0x0A)
    {
        return 0;
    }
    
    raw->len = raw->nbyte;
    raw->nbyte = 0;
    return parse_nmea(raw);
}






