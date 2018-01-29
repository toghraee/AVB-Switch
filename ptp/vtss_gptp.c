

/**
 * This file contains an implementation of the 802.AS protocol 
 *
 * It is written generic to be adapted into different operating environment.
 * The file vtss_gptp.h defines the API between this protocol module and
 * the operating environment.
 */



#include "vtss_gptp.h"

#ifdef VTSS_GPTP_NOT_WANTED
#if defined(__CX51__) || defined(__C51__)
const char *vtss_gptp_coseg = "This creates the CO segment of vtss_gptp";
#endif /* __CX51__ || __C51__ */
#else


#include "vtss_gptp_private.h"
/*#include "gptp_bpdu.h"*/
#include "h2io.h"
#include "h2.h"
#include "h2txrx.h"

#if defined(__CX51__) || defined(__C51__)
#include "print.h"
#endif

#ifndef VTSS_GPTP_TRACE
#include <stdio.h>
#pragma warning "Falling back on printf for trace"
#define VTSS_GPTP_TRACE(lvl, fmt, ...)     printf(fmt, ##__VA_ARGS__)
#endif /* VTSS_GPTP_TRACE */



gptpm_t vtss_gptp_vars;

extern vars_ulong32 time_since_boot;



static vars_ulong32 servoCount = 0;

#define GPTP_IS_UNINITED (!GPTP->initialized)
#define GPTP_INIT       do { if (GPTP_IS_UNINITED) gptp_sw_init(); } while (0)

static vtss_gptp_port_vars_t *current_port;

static gptpm_t *current_sm;

/*Packet transmission buffer */

static vars_uchar8 ptp_tx_buf       [1500];






const _UINT64 UINT64_ZERO = Int64Initializer(0, 0);
const _UINT64 UINT64_MAX = Int64Initializer(~0U, ~0U);
const _INT64 INT64_ZERO = Int64Initializer(0, 0);
const _INT64 INT64_MIN  = Int64Initializer(0x80000000,0);
const _INT64 INT64_MAX  = Int64Initializer(0x7fffffff,~0U);
const _INT64 INT64_NEGATIVE_ONE  = Int64Initializer(~0,~0U);
const _UINT64 UINT64_MIN = Int64Initializer(0x80000000,0);


unsigned long long_multiply_high (unsigned long  v1, unsigned long v2)
{
  unsigned long a, b, c, d;
  unsigned long x, y;
  _UINT64 result;

  a = (v1 >> 16) & 0xffff;
  b = v1 & 0xffff;
  c = (v2 >> 16) & 0xffff;
  d = v2 & 0xffff;

  result.LowPart = b * d;                   /* BD */
  x = a * d + c * b;            /* AD + CB */
  y = ((result.LowPart >> 16) & 0xffff) + x;

  result.LowPart = (result.LowPart & 0xffff) | ((y & 0xffff) << 16);
  result.HighPart= (y >> 16) & 0xffff;

  result.HighPart+= a * c;                  /* AC */
  return (result.HighPart);
}

_UINT64 long_multiply(unsigned long  v1, unsigned long v2)
{
  unsigned long a, b, c, d;
  unsigned long x, y;
  _UINT64 result;

  a = (v1 >> 16) & 0xffff;
  b = v1 & 0xffff;
  c = (v2 >> 16) & 0xffff;
  d = v2 & 0xffff;

  result.LowPart = b * d;                   /* BD */
  x = a * d + c * b;            /* AD + CB */
  y = ((result.LowPart >> 16) & 0xffff) + x;

  result.LowPart = (result.LowPart & 0xffff) | ((y & 0xffff) << 16);
  result.HighPart= (y >> 16) & 0xffff;

  result.HighPart+= a * c;                  /* AC */
  return (result);
}






_INT64 Int64TimesInt64(_INT64 x, _INT64 y)
{
    unsigned long x0, x1, y0, y1, z0, z1;
    _INT64 z;
    int sign, xneg, yneg;

    /* Remember sign of result.
     */
    sign = (x.HighPart ^ y.HighPart) >> 31;

    /* Take absolute values of multiplicand x and multiplier y.
     */
    xneg = (x.HighPart < 0);
    x1 = MUX(xneg, ~x.HighPart + !x.LowPart, x.HighPart);
    x0 = MUX(xneg, -x.LowPart, x.LowPart);
    yneg = (y.HighPart < 0);
    y1 = MUX(yneg, ~y.HighPart + !y.LowPart, y.HighPart);
    y0 = MUX(yneg, -y.LowPart, y.LowPart);

    /* Perform the unsigned 32-by-32 multiplications.
     */
    z0 = Uint32TimesUint32(x0,y0);
    z1 = Uint32TimesUint32(x0,y1) + Uint32TimesUint32(x1,y0) + long_multiply_high(x0, y0);

    /* If necessary, negate product.
     */
    z.HighPart = MUX(sign, ~z1 + !z0, z1);
    z.LowPart = MUX(sign, -z0, z0);
    return (z);
}


/*  Multiply a signed,
 * 64-bit integer by a signed, 32-bit integer.  The
 * return value (the product) is a signed 64-bit integer.
 */
_INT64 Int64TimesInt32(_INT64 x, int y)
{
    unsigned long x0, x1, z0, z1;
    _INT64 z;
    int sign, xneg;

    /* Remember sign of result.
     */
    sign = (x.HighPart ^ y) >> 31;

    /* Take absolute values of multiplicand x and multiplier y.
     */
    xneg = (x.HighPart < 0);
    x1 = MUX(xneg, ~x.HighPart + !x.LowPart, x.HighPart);
    x0 = MUX(xneg, -x.LowPart, x.LowPart);
    y = iabs(y);

    /* Perform the unsigned 32-by-32 multiplications.
     */
    z0 = Uint32TimesUint32(x0,y);
    z1 = Uint32TimesUint32(x1,y) + long_multiply_high(x0, y);

    /* If necessary, negate product.
     */
    z.HighPart = MUX(sign, ~z1 + !z0, z1);
    z.LowPart = MUX(sign, -z0, z0);
    return (z);
}

/* Multiply an unsigned,
 * 64-bit integer by an unsigned, 64-bit integer.  The
 * return value (the product) is an unsigned 64-bit integer.
 */
_UINT64 Uint64TimesUint64(_UINT64 x, _UINT64 y)
{
    _UINT64 z;

    /* Perform the unsigned 32-by-32 multiplications.
     */
    z.LowPart = Uint32TimesUint32(x.LowPart,y.LowPart);
    z.HighPart = Uint32TimesUint32(x.LowPart,y.HighPart) +
        Uint32TimesUint32(x.HighPart,y.LowPart) + long_multiply_high(x.LowPart, y.LowPart);

    return (z);
}


/* -- Multiply an unsigned,
 * 64-bit integer by an unsigned, 32-bit integer.  The
 * return value (the product) is an unsigned 64-bit integer.
 */
_UINT64 Uint64TimesUint32(_UINT64 x, int y)
{
    _UINT64 z;

    /* Perform the unsigned 32-by-32 multiplications.
     */
    z.LowPart = Uint32TimesUint32(x.LowPart,y);
    z.HighPart = Uint32TimesUint32(x.HighPart,y) + long_multiply_high(x.LowPart, y);

    return (z);
}


/* 64-bit, add and subtract routines */
#define carry(a,b) (((unsigned long)(a) + (unsigned long)(b)) < (a))
#define borrow(a,b) ((a) < (b))


/* Add one unsigned,
 * 64-bit integer and one unsigned 32-bit integer and
 * return the unsigned, 64-bit sum.
 */
_UINT64 Uint64AddUint32(_UINT64 x, unsigned long y)
{
    x.HighPart += carry(x.LowPart, y);
    x.LowPart += y;
    return (x);
}


_UINT64 UInt64Add(_UINT64 x, _UINT64 y)
{
    x.HighPart += y.HighPart + carry(x.LowPart, y.LowPart);
    x.LowPart += y.LowPart;
    return (x);

}



_INT64 Int64Add(_INT64 x, _INT64 y)
{
    x.HighPart += y.HighPart + carry(x.LowPart, y.LowPart);
    x.LowPart += y.LowPart;
    return (x);
}

_INT64 Int64AddInt32(_INT64 x, int y)
{
    _INT64 z;

    Int32ToInt64(z,y);
    return Int64Add(x,z);
}



_INT64 Int64Subtract(_INT64 x, _INT64 y)
{
    x.HighPart -= y.HighPart + borrow(x.LowPart, y.LowPart);
    x.LowPart -= y.LowPart;
    return (x);
}

/*  -- Subtract an unsigned,
 * 32-bit integer (2nd arg) from an unsigned, 64-bit integer
 * (first arg) and return the unsigned, 64-bit difference.
 */
_UINT64 Uint64SubtractUint32(_UINT64 x, unsigned long y)
{
    x.HighPart -= borrow(x.LowPart, y);
    x.LowPart -= y;
    return (x);
}







_UINT64 Uint64RShift(_UINT64 x, unsigned long n)
{
    /* check bad cases: ((n > 63) || (n < 0)) */
    if ((n & 63) != n)
        return Int64ToUint64(INT64_ZERO);

    if (n > 31) {
        x.LowPart = x.HighPart >> (n-32);
        x.HighPart = 0;
    } else
    if (n) {
        x.LowPart = (x.LowPart >> n) | (x.HighPart << (32-n));
        x.HighPart = x.HighPart >> n;
    }
    return (x);
}


_INT64 Int64RShift(_INT64 x, int n)
{
    /* check bad cases: ((n > 63) || (n < 0)) */
    if ((n & 63) != n)
        return (x.HighPart >= 0) ? INT64_ZERO : INT64_NEGATIVE_ONE;

    if (n > 31) {
        x.LowPart = x.HighPart >> (n-32);
        x.HighPart = x.HighPart >> 31;
    } else
    if (n) {
        x.LowPart = (x.LowPart >> n) | (x.HighPart << (32-n));
        x.HighPart = x.HighPart >> n;
    }
    return (x);
}



/*-- Shift a signed,
 * 64-bit integer left by the specified number of bits.
 * The first argument contains the value to be shifted
 * and the second argument specifies the shift amount.
 * The return value is the shifted version of the first
 * argument.
 */
_INT64 Int64LShift(_INT64 x, int n)
{
    /* check bad cases: ((n > 63) || (n < 0)) */
    if ((n & 63) != n)
        return INT64_ZERO;
    if (n > 31) {
        x.HighPart = x.LowPart << (n-32);
        x.LowPart = 0;
    } else
    if (n) {
        x.HighPart = (x.HighPart << n) | (x.LowPart >> (32-n));
        x.LowPart = x.LowPart << n;
    }
    return (x);
}

/*  Shift an unsigned,
 * 64-bit integer left by the specified number of bits.
 * The first argument contains the value to be shifted
 * and the second argument specifies the shift amount.
 * The return value is the shifted version of the first
 * argument.
 */
_UINT64 Uint64LShift(_UINT64 x, int n)
{
    Uint64ToInt64(x) = Int64LShift(Uint64ToInt64(x),n);
    return (x);
}

/* Worker function for all of the div/mod functions.
 * Computes both the result and the remainder of the
 * division of X/Y, returns them by reference.
 * Funny order of arguments to minimize register swaps,
 * esp in the division case (more popular than modulus).
 */
void Uint64DivMod(_UINT64 *pq, _UINT64 x, _UINT64 y, _UINT64 *pr)
{
    int i, j, n, x1ne0, y1ne0, nlt32, xgey;
    unsigned long x32, y32;
    _UINT64 z;

    /* Check for divide by zero.
     */
    if (!(y.LowPart | y.HighPart)) {
        if (!(x.LowPart | x.HighPart)) {
            z = UINT64_ZERO;
        } else {
            z = UINT64_MIN;
        }
        x = UINT64_ZERO;
        goto out;
    }

    /* Calculate n-bit misalignment of dividend y
     * with respect to divisor x.  Align y to x.
     * z = x1.x0 / y1.y0;
     */

    /* Get the non-zero parts. They both exist, see above */
    x1ne0 = (x.HighPart != 0);
    y1ne0 = (y.HighPart != 0);
    x32 = MUX(x.HighPart, x.HighPart, x.LowPart);
    y32 = MUX(y.HighPart, y.HighPart, y.LowPart);

    /* Compute the index of the leftmost bit, into I for x32
     * and into J for y32.
     */
    i = MUX(x32 >> 16, 24, 8);
    j = MUX(y32 >> 16, 24, 8);
    i = MUX(x32 >> i, i+4, i-4);
    j = MUX(y32 >> j, j+4, j-4);
    i = MUX(x32 >> i, i+2, i-2);
    j = MUX(y32 >> j, j+2, j-2);
    i = MUX(x32 >> i, i+1, i-1);
    j = MUX(y32 >> j, j+1, j-1);
    i -= !(x32 >> i);
    j -= !(y32 >> j);

    /* Compute their disalignment, could be negative.
     */
    n = i - j;

    /* Align y (sort of)
     */
    y.HighPart = (y.HighPart << (n&31)) |
                 izero(i != j, y.LowPart >> ((32-n)&31));
    y.LowPart <<= (n&31);

    /* how many bits will we really get
     */
    n += (x1ne0 - y1ne0) << 5;

    /* really align y, swapping if necessary
     */
    nlt32 = (n < 32);
    y.HighPart = MUX(nlt32, y.HighPart, y.LowPart);
    y.LowPart = izero(nlt32, y.LowPart);

    /* Init result */
    z.LowPart = z.HighPart = 0;

    /* Calculate one bit of quotient z per iteration.
     */
    while (n-- >= 0) {

        xgey = MUX(x.HighPart != y.HighPart, x.HighPart > y.HighPart, x.LowPart >= y.LowPart);
        z.HighPart += z.HighPart + carry(z.LowPart, z.LowPart);
        z.LowPart += z.LowPart + xgey;
        x.HighPart -= izero(xgey, y.HighPart + borrow(x.LowPart, y.LowPart));
        x.LowPart -= izero(xgey, y.LowPart);
        y.LowPart = (y.LowPart >> 1) | (y.HighPart << 31);
        y.HighPart >>= 1;
    }

 out:
    /* Quotient, if desired
     */
    if (pq)
        *pq = z;
    /* Remainder, if desired
     */
    if (pr)
        *pr = x;
}

/* Same as above, only in signed integer terms.
 * At least one compiler uses this directly.
 */
void Int64DivMod(_INT64 *pq, _INT64 x, _INT64 y, _INT64 *pr)
{
    int sign, xneg, yneg;
    _UINT64 ux, uy, uz, ur;
    _INT64 z, r;

    /* Remember sign of result.
     */
    sign = (x.HighPart ^ y.HighPart) >> 31;

    /* Take absolute values of dividend x and divisor y.
     */
    xneg = (x.HighPart < 0);
    ux.HighPart = MUX(xneg, ~x.HighPart + !x.LowPart, x.HighPart);
    ux.LowPart = MUX(xneg, -x.LowPart, x.LowPart);
    yneg = (y.HighPart < 0);
    uy.HighPart = MUX(yneg, ~y.HighPart + !y.LowPart, y.HighPart);
    uy.LowPart = MUX(yneg, -y.LowPart, y.LowPart);

    /* Perform the division, cast the results.
     */
    Uint64DivMod(&uz,ux,uy,&ur);
    z.LowPart = uz.LowPart;
    z.HighPart = (unsigned long) uz.HighPart;
    r.LowPart = ur.LowPart;
    r.HighPart = (unsigned long) ur.HighPart;

    /* If necessary, negate quotient/remainder
     */
    z.HighPart = MUX(sign, ~z.HighPart + !z.LowPart, z.HighPart);
    z.LowPart = MUX(sign, -z.LowPart, z.LowPart);
    r.HighPart = MUX(xneg, ~r.HighPart + !r.LowPart, r.HighPart);
    r.LowPart = MUX(xneg, -r.LowPart, r.LowPart);

    /* Quotient, if desired
     */
    if (pq)
        *pq = z;
    /* Remainder, if desired
     */
    if (pr)
        *pr = r;
}

/*  -- Divide an unsigned, 64-bit
 * integer by another unsigned, 64-bit integer.  The first
 * argument is the dividend and the second is the divisor.
 * The return value (the quotient) is an unsigned 64-bit integer.
 */
_UINT64 Uint64DividedByUint64(_UINT64 x, _UINT64 y)
{
    _UINT64 z;
    Uint64DivMod(&z,x,y,NULL);
    return z;
}

/*  -- Divide an unsigned, 64-bit
 * integer by an unsigned, 32-bit integer.  The first argument
 * is the dividend and the second is the divisor.  The
 * return value (the quotient) is an unsigned 64-bit integer.
 */
_UINT64 Uint64DividedByUint32(_UINT64 x, unsigned long y)
{
    _UINT64 y64;

    y64.LowPart = y;
    y64.HighPart = 0;
    return Uint64DividedByUint64(x, y64);
}


/*  -- Divide a signed, 64-bit
 * integer by another signed, 64-bit integer.  The first
 * argument is the dividend and the second is the divisor.
 * The return value (the quotient) is a signed 64-bit integer.
 */
_INT64 Int64DividedByInt64(_INT64 x, _INT64 y)
{
    _INT64 z;
    Int64DivMod(&z,x,y,NULL);
    return z;
}

/*  Divide a signed, 64-bit
 * integer by a signed, 32-bit integer.  The first argument
 * is the dividend and the second is the divisor.  The
 * return value (the quotient) is a signed 64-bit integer.
 */
_INT64 Int64DividedByInt32(_INT64 x, int y)
{
    _INT64 y64;

    y64.LowPart = y;
    y64.HighPart = y >> 31;
    return Int64DividedByInt64(x, y64);
}



_INT64 Int64Or(_INT64 a, _INT64 b)
{
    a.LowPart = a.LowPart | b.LowPart;
    a.HighPart = a.HighPart | b.HighPart;
    return (a);
}

/*  Or two unsigned,
 * 64-bit integers, return the unsigned 64-bit result.
 */
_UINT64 Uint64Or(_UINT64 a, _UINT64 b)
{
    a.LowPart = a.LowPart | b.LowPart;
    a.HighPart = a.HighPart | b.HighPart;
    return (a);
}

/*  -- Xor two signed,
 * 64-bit integers, return the signed 64-bit result.
 */
_INT64 Int64Xor(_INT64 a, _INT64 b)
{
    a.LowPart = a.LowPart ^ b.LowPart;
    a.HighPart = a.HighPart ^ b.HighPart;
    return (a);
}

/* - Xor two unsigned,
 * 64-bit integers, return the unsigned 64-bit result.
 */
_UINT64 Uint64Xor(_UINT64 a, _UINT64 b)
{
    a.LowPart = a.LowPart ^ b.LowPart;
    a.HighPart = a.HighPart ^ b.HighPart;
    return (a);
}

/*  -- And two signed,
 * 64-bit integers, return the signed 64-bit result.
 */
_INT64 Int64And(_INT64 a, _INT64 b)
{
    a.LowPart = a.LowPart & b.LowPart;
    a.HighPart = a.HighPart & b.HighPart;
    return (a);
}

/*  -- And two unsigned,
 * 64-bit integers, return the unsigned 64-bit result.
 */
_UINT64 Uint64And(_UINT64 a, _UINT64 b)
{
    a.LowPart = a.LowPart & b.LowPart;
    a.HighPart = a.HighPart & b.HighPart;
    return (a);
}




void transmit_announce(struct gptpm *ptp, vars_uchar8 port) {

#define ANNOUNCE_PACKET_SIZE (sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr) + sizeof(AnnounceMessage))

  vars_ulong32 i,j = 0;

  vtss_ptp_time_t presentTime;


  unsigned char *buf = &ptp_tx_buf[0];
  ComMessageHdr *pComMesgHdr = (ComMessageHdr *) &buf[sizeof(ethernet_hdr_t)];
  AnnounceMessage *pAnnounceMesg = (AnnounceMessage *) &buf[sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr)];

  unsigned char *pPathBuff = &buf[sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr) + sizeof(AnnounceMessage)];

  vars_ushort16 msg_len = 0;
  
 

  PtpSystemIdentity *identity = &ptp->gmPriority->rootSystemIdentity;

  print_str("Transmit Announce \r\n");
  
  set_ptp_ethernet_hdr(buf,port);

  // setup the common message header.
  memset(pComMesgHdr, 0, sizeof(ComMessageHdr) + sizeof(AnnounceMessage));

  pComMesgHdr->transportSpecific_messageType = 
    PTP_TRANSPORT_SPECIFIC_HDR | PTP_ANNOUNCE_MESG;
  
  pComMesgHdr->versionPTP = PTP_VERSION_NUMBER;

  /* Add the TLV Length also into announce length */

  msg_len = sizeof(ComMessageHdr) + sizeof(AnnounceMessage)+ (8*(ptp->pathTraceLength));

  VTSS_COMMON_UNALIGNED_PUT_2B(pComMesgHdr->messageLength,msg_len);

  /* Add the flags copy from global system properties */
  pComMesgHdr->flagField[1] = 
     ((ptp->leap_61 & 0x1)) |
     ((ptp->leap_59 & 0x1) << 1) | 
     ((ptp->current_utc_offset_valid & 0x1) << 2) |
     ((PTP_TIMESCALE & 0x1) << 3) |
     ((ptp->time_traceable & 0x1) << 4) |
     ((ptp->frequency_traceable & 0x1) << 5);

   pComMesgHdr->flagField[0]                 = 0x2;   // set two steps flag


   pComMesgHdr->sourcePortIdentity[0] = ptp->properties.grandmasterIdentity[0];
   pComMesgHdr->sourcePortIdentity[1] = ptp->properties.grandmasterIdentity[1];
   pComMesgHdr->sourcePortIdentity[2] = ptp->properties.grandmasterIdentity[2];
   pComMesgHdr->sourcePortIdentity[3] = 0xff;
   pComMesgHdr->sourcePortIdentity[4] = 0xfe;
   pComMesgHdr->sourcePortIdentity[5] = ptp->properties.grandmasterIdentity[3];
   pComMesgHdr->sourcePortIdentity[6] = ptp->properties.grandmasterIdentity[4];
   pComMesgHdr->sourcePortIdentity[7] = ptp->properties.grandmasterIdentity[5];

   /*Set the port ID */

   VTSS_COMMON_UNALIGNED_PUT_2B(&pComMesgHdr->sourcePortIdentity[8], ptp->ports[port].port_properties.us_portNumber);


   /*Increment the sequence ID */
   ptp->ports[port].announceSequenceId++;

   /* Set the sequence id in the packet */
   VTSS_COMMON_UNALIGNED_PUT_2B(pComMesgHdr->sequenceId,ptp->ports[port].announceSequenceId);


   
   pComMesgHdr->controlField = PTP_CTL_FIELD_OTHERS;
   
   pComMesgHdr->logMessageInterval = ptp->ports[port].currentLogAnnounceInterval;


   /* Start the announce message filling */

   // setup the Announce message
   VTSS_COMMON_UNALIGNED_PUT_2B(pAnnounceMesg->currentUtcOffset,ptp->current_utc_offset);
   pAnnounceMesg->grandmasterPriority1 = identity->priority1;
   

   // grandMaster clock quality.
   pAnnounceMesg->clockClass = identity->clockClass;
   pAnnounceMesg->clockAccuracy = identity->clockAccuracy;

   VTSS_COMMON_UNALIGNED_PUT_2B(pAnnounceMesg->clockOffsetScaledLogVariance,UNAL_HOST2NETS(identity->offsetScaledLogVariance));

   
   // grandMaster priority
   pAnnounceMesg->grandmasterPriority2 = identity->priority2;


   /*Grandmaster clock identity */
   for (i=0;i<8;i++)
     pAnnounceMesg->grandmasterIdentity[i] = identity->clockIdentity[i];

   
   
   VTSS_COMMON_UNALIGNED_PUT_2B(pAnnounceMesg->stepsRemoved,ptp->masterStepsRemoved);

   pAnnounceMesg->timeSource = ptp->time_source;

   VTSS_COMMON_UNALIGNED_PUT_2B(pAnnounceMesg->tlvType,HOST2NETS(PTP_ANNOUNCE_TLV_TYPE));
   VTSS_COMMON_UNALIGNED_PUT_2B(pAnnounceMesg->tlvLength,HOST2NETS(8*(ptp->pathTraceLength)));

   i = 0;
   j = 0;

   /* Put in the path trace TLV */
   
   while((i < ptp->pathTraceLength) && (j < (8*(ptp->pathTraceLength))))
   {
        pPathBuff[j++] = ptp->pathTrace[i][0];
        pPathBuff[j++] = ptp->pathTrace[i][1];
        pPathBuff[j++] = ptp->pathTrace[i][2];
        pPathBuff[j++] = ptp->pathTrace[i][3];
        pPathBuff[j++] = ptp->pathTrace[i][4];
        pPathBuff[j++] = ptp->pathTrace[i][5];
        pPathBuff[j++] = ptp->pathTrace[i][6];
        pPathBuff[j++] = ptp->pathTrace[i][7];
        i++;
   } 
   


   get_rtc_time(ptp, &presentTime);

   put_timestamp(ptp, port,pAnnounceMesg->originTimestamp,&presentTime);
   
   ptpex_os_tx_frame( port,ptp_tx_buf,((sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr) + sizeof(AnnounceMessage))+(8*(ptp->pathTraceLength))));

 
}


void transmit_sync(struct gptpm *ptp, vars_uchar8 port)
{

#define SYNC_PACKET_SIZE (sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr) + sizeof(SyncMessage))
#define FOLLOWUP_PACKET_SIZE (sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr) + sizeof(FollowUpMessage))
 
  unsigned char *buf = (unsigned char *) &ptp_tx_buf[0];
  ComMessageHdr *pComMesgHdr = (ComMessageHdr *) &buf[sizeof(ethernet_hdr_t)];
  SyncMessage *pSyncMesg = (SyncMessage *) &buf[sizeof(ethernet_hdr_t) +
                                                sizeof(ComMessageHdr)];
  FollowUpMessage *pFollowUpMesg = (FollowUpMessage *) &buf[sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr)];


  
  vtss_ptp_time_t presentTime;
  vtss_ptp_time_t difference;	  
  vars_uchar8 *pChar = NULL;
  int i = 0;

  set_ptp_ethernet_hdr(buf,port);

  memset(pComMesgHdr, 0, sizeof(ComMessageHdr) + sizeof(FollowUpMessage));

  // 1. Send Sync message.
  
  pComMesgHdr->transportSpecific_messageType =
    PTP_TRANSPORT_SPECIFIC_HDR | PTP_SYNC_MESG;
  
  pComMesgHdr->versionPTP = PTP_VERSION_NUMBER;
  
  VTSS_COMMON_UNALIGNED_PUT_2B(pComMesgHdr->messageLength,HOST2NETS(sizeof(ComMessageHdr) + sizeof(SyncMessage)));

  pComMesgHdr->flagField[0] = 0x2;   // set two steps flag
  
  for(i=0;i<8;i++)
  {
    pComMesgHdr->correctionField[i] = 0;
  }


  if(ptp->grand_master == 1)
  {  

      pComMesgHdr->sourcePortIdentity[0] = ptp->properties.grandmasterIdentity[0];
      pComMesgHdr->sourcePortIdentity[1] = ptp->properties.grandmasterIdentity[1];
      pComMesgHdr->sourcePortIdentity[2] = ptp->properties.grandmasterIdentity[2];
      pComMesgHdr->sourcePortIdentity[3] = 0xff;
      pComMesgHdr->sourcePortIdentity[4] = 0xfe;
      pComMesgHdr->sourcePortIdentity[5] = ptp->properties.grandmasterIdentity[3];
      pComMesgHdr->sourcePortIdentity[6] = ptp->properties.grandmasterIdentity[4];
      pComMesgHdr->sourcePortIdentity[7] = ptp->properties.grandmasterIdentity[5];
  }
  else
  {

      memcpy(pComMesgHdr->sourcePortIdentity,(vars_uchar8*)&ptp->rxSourcePortIdentity,sizeof(PtpPortIdentity));
      /*
      pComMesgHdr->sourcePortIdentity[0] = ptp->rxSourcePortIdentity[0];
      pComMesgHdr->sourcePortIdentity[1] = ptp->rxSourcePortIdentity[1];
      pComMesgHdr->sourcePortIdentity[2] = ptp->rxSourcePortIdentity[2];
      pComMesgHdr->sourcePortIdentity[3] = ptp->rxSourcePortIdentity[3];
      pComMesgHdr->sourcePortIdentity[4] = ptp->rxSourcePortIdentity[4];
      pComMesgHdr->sourcePortIdentity[5] = ptp->rxSourcePortIdentity[5];
      pComMesgHdr->sourcePortIdentity[6] = ptp->rxSourcePortIdentity[6];
      pComMesgHdr->sourcePortIdentity[7] = ptp->rxSourcePortIdentity[7];
      pComMesgHdr->sourcePortIdentity[8] = ptp->rxSourcePortIdentity[8];*/
  }

  /*Increment the sync sequence ID */

  ptp->ports[port].syncSequenceId++;  

  

  
  VTSS_COMMON_UNALIGNED_PUT_2B(pComMesgHdr->sequenceId,HOST2NETS(ptp->ports[port].syncSequenceId));

  pComMesgHdr->controlField = PTP_CTL_FIELD_SYNC;
    




  pComMesgHdr->logMessageInterval = ptp->ports[port].currentLogSyncInterval;


  get_rtc_time(ptp, &presentTime);


  put_timestamp(ptp,port,pSyncMesg->originTimestamp,&presentTime);
 

  ptpex_os_tx_frame(port,ptp_tx_buf,(sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr) + sizeof(SyncMessage)));
  


  /*Increment the time by 1 nanosecond for adding the egress time for followup */
  presentTime.nanoseconds++;
    
  // Send Follow_Up message
  
  pComMesgHdr->transportSpecific_messageType = 
    PTP_TRANSPORT_SPECIFIC_HDR | PTP_FOLLOW_UP_MESG;
  
  pComMesgHdr->controlField = PTP_CTL_FIELD_FOLLOW_UP;

  VTSS_COMMON_UNALIGNED_PUT_2B(pComMesgHdr->messageLength,HOST2NETS(sizeof(ComMessageHdr) + sizeof(FollowUpMessage)));

   /* Set time stamp to received timestamp from master, if GM rxPreciseOriginTimeStamp will be set to rtc nanoseconds */

  if(ptp->grand_master == 1)
  {

    get_rtc_time( ptp,&presentTime);
    presentTime.secondsLower = 0;
    presentTime.secondsUpper = 0;
    timestamp_copy(&ptp->rxPreciseOriginTimeStamp,&presentTime); 
    
  }
   put_timestamp(ptp,port,&pFollowUpMesg->preciseOriginTimestamp,&ptp->rxPreciseOriginTimeStamp);

  /*Fill in the correction field in followup message */

  if (ptp->grand_master == 0)
  {   /* transmit sync send time - UpstreamTxTime */
      timestamp_difference(&presentTime,&ptp->upSTreamTxTime,&difference);
      difference.secondsLower = (vars_ulong32)((difference.secondsLower) * (ptp->rateRatio));
      timestamp_sum(&ptp->rxCorrectionField,&difference,&presentTime);
      put_timestamp(ptp,port,pComMesgHdr->correctionField,&presentTime);
      
  }
  else
  {
      /* put the clockmaster nanoseconds into correction field */
      get_rtc_time(ptp,&presentTime);
      pChar = (vars_uchar8*)&presentTime.nanoseconds;
      pComMesgHdr->correctionField[6] = pChar[2];
      pComMesgHdr->correctionField[7] = pChar[3];
  }

  
  
  
  // Fill in follow up fields as per 802.1as section 11.4.4.2
  VTSS_COMMON_UNALIGNED_PUT_2B(pFollowUpMesg->tlvType,HOST2NETS(0x3));
  VTSS_COMMON_UNALIGNED_PUT_2B(pFollowUpMesg->lengthField,HOST2NETS(28));
  pFollowUpMesg->organizationId[0] = 0x00;
  pFollowUpMesg->organizationId[1] = 0x80;
  pFollowUpMesg->organizationId[2] = 0xc2;
  pFollowUpMesg->organizationSubType[0] = 0;
  pFollowUpMesg->organizationSubType[1] = 0;
  pFollowUpMesg->organizationSubType[2] = 1;

  ptpex_os_tx_frame(port,ptp_tx_buf,FOLLOWUP_PACKET_SIZE);
  
  

}

void ack_grandmaster_change(struct gptpm *ptp) {
 
  
  ptp->rtcChangesAllowed = TRUE;

  
 
  /* Set the RTC back to its nominal increment; even if we're going to
   * operate as a slave, this is the point we wish to start at.
   */
  set_rtc_increment(ptp, &ptp->nominalIncrement);
}



int ptp_events_tx_gm_change(struct gptpm *ptp) {

  ack_grandmaster_change(ptp);

  memset(&ptp->new_master, 0, sizeof(vtss_gptp_system_properties_t));
  ptp->new_master.grandmasterPriority1                  = ptp->gmPriority->rootSystemIdentity.priority1;
  ptp->new_master.grandmasterClockQuality.uc_clockClass    = ptp->gmPriority->rootSystemIdentity.clockClass;
  ptp->new_master.grandmasterClockQuality.uc_clockAccuracy = ptp->gmPriority->rootSystemIdentity.clockAccuracy;
  ptp->new_master.grandmasterClockQuality.us_offsetScaledLogVariance = VTSS_COMMON_UNALIGNED_GET_2B(ptp->gmPriority->rootSystemIdentity.offsetScaledLogVariance);
  ptp->new_master.grandmasterPriority2    = ptp->gmPriority->rootSystemIdentity.priority2;
  memcpy(ptp->new_master.grandmasterIdentity, ptp->gmPriority->rootSystemIdentity.clockIdentity, sizeof(PtpClockIdentity));
  ptp->new_master.masterStepsRemoved = ptp->masterStepsRemoved;
  memcpy(ptp->new_master.pLocalClockIdentity,ptp->properties.grandmasterIdentity,sizeof(PtpClockIdentity));

  return 0;
}

void ptp_print_port_role(vars_ushort16 port_no)
{
   /*
     PTP_MASTER   = 6,
     PTP_SLAVE    = 9,
     PTP_PASSIVE  = 7,
     PTP_DISABLED = 3
   */

   switch(GPTP->ports[port_no].selectedRole)
    {

      case PTP_MASTER:
        print_str("Master");
        break;
      case PTP_SLAVE:
        print_str("Slave");
        break;
      case PTP_PASSIVE:
        print_str("Passive");
        break;
      case PTP_DISABLED:
        print_str("Disabled");
        break;
    }
     
}



/*
 *  Perform the next task needed. But only for one port.
 *   This leaves CPU for all the other protocols and tasks.
 */
void vtss_gptp_more_work(void)
{

    
    unsigned long newMaster;
    vars_uchar8  i;
    signed char  reselect = 0;
    bool localMaster = TRUE;
    unsigned long timerTicks = 0;

	//print_str("GPTP More Work\r\n");

    if (current_sm == NULL)
	{
	    //print_str("Current state Machine is NULL\r\n");
        return;             /* No more work to do */
	}
    if (current_sm->admin_enabled) {
        
        /* Run State Machines for the current port of the state machine */   

          timerTicks = current_sm->timerTicks;
          current_sm->timerTicks = 0;
  
		  //print_str("Running State Machines from More work\r\n");
          /* Update port roles whenever any port is flagged for reselect */
          for (i=0; i<current_sm->numPorts; i++) {
            reselect |= current_sm->ports[i].reselect;
          }
          if (reselect) {
                PortRoleSelection_StateMachine(current_sm);
          }
    
          for (i=0; i<current_sm->numPorts; i++) {
              if (current_sm->ports[i].selectedRole == PTP_SLAVE) {
                  localMaster = FALSE;
                  break;
              }
          }
    
          for (i=0; i<current_sm->numPorts; i++) {
            switch(current_sm->ports[i].selectedRole) {
            case PTP_MASTER:
              /* Send ANNOUNCE and SYNC messages at their rate for a master port */
              current_sm->ports[i].announceCounter += timerTicks;
			  vtss_printf(" GPTP Timer Ticks %d Announce Interval Ticks %d\r\n",current_sm->ports[i].announceCounter,ANNOUNCE_INTERVAL_TICKS(current_sm, i));
              if(current_sm->ports[i].announceCounter >= ANNOUNCE_INTERVAL_TICKS(current_sm, i)) {
                current_sm->ports[i].announceCounter = 0;
                current_sm->ports[i].newInfo = FALSE;
                transmit_announce(current_sm, i);
              }

              current_sm->ports[i].syncCounter += timerTicks;
              if(current_sm->ports[i].syncCounter >= SYNC_INTERVAL_TICKS(current_sm, i)) {
                current_sm->ports[i].syncCounter = 0;
                transmit_sync(current_sm, i);

              }
              break;

            case PTP_SLAVE:
            {


              /* Transmit an ANNOUNCE immediately to speed things along if we've switched our
                           * port to the master state.
                        */
                if(current_sm->ports[i].selectedRole == PTP_MASTER) {

                for (i=0; i<current_sm->numPorts; i++) {
                  current_sm->ports[i].announceCounter    = 0;
                  current_sm->ports[i].announceSequenceId = 0x0000;
                  transmit_announce(current_sm, i);
                }
              } else {
                /* Still a slave; determine whether we are using the end-to-end or peer-to-peer
                             * delay mechanism
                             */
                if(current_sm->properties.delayMechanism == PTP_DELAY_MECHANISM_E2E) {
                 /* Increment the delay request counter and see if it's time to
                               * send one to the master.
                               */
                  if(++current_sm->ports[i].delayReqCounter >= (DELAY_REQ_INTERVAL / VTSS_GPTP_TICKS_PER_SEC)) {
                    current_sm->ports[i].delayReqCounter = 0;
                    //transmit_delay_request(current_sm, i);
                    }
                  }
                } /* if(still a slave) */
            }
            break;

            default:
              /* "Passive"; do nothing */
              break;
            }
         }

          for (i=0; i<current_sm->numPorts; i++)
          {
            LinkDelaySyncIntervalSetting_StateMachine(current_sm, i);

            /* Regardless of whether we are a master or slave, increment the peer delay request
                        * counter and see if it's time to send one to our link peer.
                        */
            current_sm->ports[i].pdelayIntervalTimer++;
            MDPdelayReq_StateMachine(current_sm, i);

            /* Update the PortAnnounceInformation state machine */
            PortAnnounceInformation_StateMachine(current_sm, i);
          }


          newMaster = current_sm->newMaster;
          current_sm->newMaster = FALSE;
  
          if(newMaster) ptp_events_tx_gm_change(current_sm);


    }
	current_sm = NULL;
}

/* Hold on to the CPU and finish up any outstanding work tasks now */
static void finish_gptp_work(void)
{
    //print_str("finish_gptp_work\r\n");
    if(current_sm != NULL)
        vtss_gptp_more_work();
}

/* Start a new slew of tasks */
void
GPTP_machine_update(void)
{
    //print_str("Calling GPTP_Machine_Update\r\n");
    finish_gptp_work();         /* Finish old slew first */    
    current_sm = GPTP;
   
    /* Kick off with the first task */
    vtss_gptp_more_work();
}


void vtss_gptp_set_config(const vtss_gptp_system_config_t VTSS_COMMON_PTR_ATTRIB *system_config)
{
    GPTP_INIT;
    if (memcmp(system_config, &GPTP->system_config, sizeof(*system_config)) == 0)
        return;
    GPTP->system_config = *system_config;
}


static void gptp_hw_init(void)
{
    vars_uchar8 pix;
    vtss_gptp_port_vars_t *pp;
    vtss_common_linkstate_t linkup;

    pp = &GPTP->ports[0];
    for (pix = 1; pix <= VTSS_GPTP_MAX_PORTS ; pix++, pp++) {
        VTSS_GPTP_ASSERT(pp->port_number == pix);
        //linkup = vtss_aggr_get_linkstate(pix);
        if(linkup)
            vtss_os_get_portmac(pix, &pp->port_properties.port_macaddr);

        /* Get port address up front - unconditionally */
        vtss_os_get_portmac(pix, &pp->port_properties.port_macaddr);

        vtss_os_set_fwdstate(pix, VTSS_COMMON_FWDSTATE_ENABLED);
		/* Temprarily Changed
        if (pp->port_config.enable_gptp)
        pp->pttPortEnabled = VTSS_COMMON_BOOL_TRUE;
        else
        pp->pttPortEnabled = VTSS_COMMON_BOOL_FALSE;*/
        
    }
}



void vtss_gptp_init(void)
{
    print_str("Initializing GPTP Software\r\n");
    GPTP_INIT;
    gptp_hw_init();
}

void vtss_gptp_deinit(void)
{
    gptp_sw_init();
}



void get_platform_data(PtpPlatformData *platformData, struct gptpm *pGptp)
{
    vars_ushort16 i = 0;

    platformData->numPorts  = 8;

    pGptp->numPorts = platformData->numPorts;

    /* Let the platform have some delay in nanoseconds */

    platformData->rxPhyMacDelay.nanoseconds++;
    platformData->txPhyMacDelay.nanoseconds++;


    
    pGptp->nominalIncrement.mantissa = platformData->nominalIncrement.mantissa;
    pGptp->nominalIncrement.fraction = platformData->nominalIncrement.fraction;
    
    /* Zero the coefficients initially; they will be inferred from the port mode */
    pGptp->coefficients.P = 0x00000000;
    pGptp->coefficients.I = 0x00000000;
    pGptp->coefficients.D = 0x00000000;
    
    /* Assign the MAC transmit and receive latency */
    for(i=0; i<pGptp->numPorts; i++) {
        pGptp->ports[i].rxPhyMacDelay = platformData->rxPhyMacDelay;
        pGptp->ports[i].txPhyMacDelay = platformData->txPhyMacDelay;
    }

}

int mymul(int x, int y)
{
   int result = 0;

   while(y)
   {
      if(y&1)
	  	result += x;
	  x <<= 1;
	  y >>= 1;
   }
   return result;
}


static void gptp_sw_init(void)
{
    vars_uchar8 pix;
   
    vars_ushort16  i = 0;
    
    vars_ushort16 byteIndex = 0;


    
    vtss_gptp_clock_qual_vars_t *quality;
    

    /* Setup the Platform Specific Data */

    PtpPlatformData platformData;



    get_platform_data(&platformData,GPTP);

   	print_str("gptp_sw_init\r\n Start");

	GPTP->initialized = VTSS_COMMON_BOOL_TRUE;

    /* Set up our own clock  */
    
    quality = &GPTP->properties.grandmasterClockQuality;

    
    for(i=0; i<GPTP->numPorts; i++) {
      GPTP->ports[i].port_properties.us_portNumber = i+1;
    
      for(byteIndex = 0; byteIndex < MAC_ADDRESS_LENGTH ; byteIndex++) {
        GPTP->ports[i].port_properties.port_macaddr.macaddr[byteIndex] = DEFAULT_SOURCE_MAC[byteIndex];
      }
    
      GPTP->ports[i].port_properties.us_stepsRemoved = 0;
    }
    GPTP->properties.domainNumber         = VTSS_GPTP_DEFAULT_DOMAIN_ID;
    GPTP->properties.currentUtcOffset     = VTSS_GPTP_DEFAULT_CURRENT_UTC_OFFSET;
    GPTP->properties.grandmasterPriority1 = GPTP->system_config.prio1;
    quality->uc_clockClass                  = VTSS_GPTP_DEFAULT_CLOCK_CLASS;
    quality->uc_clockAccuracy               = VTSS_GPTP_DEFAULT_CLOCK_ACCURACY;
    quality->us_offsetScaledLogVariance     = VTSS_GPTP_DEFAULT_OFFSET_LOG_VAR;
    GPTP->properties.grandmasterPriority2 = GPTP->system_config.prio2;
    GPTP->properties.timeSource           = VTSS_GPTP_TIME_SOURCE;
    
    GPTP->properties.grandmasterIdentity[0] = DEFAULT_SOURCE_MAC[0];
    GPTP->properties.grandmasterIdentity[1] = DEFAULT_SOURCE_MAC[1];
    GPTP->properties.grandmasterIdentity[2] = DEFAULT_SOURCE_MAC[2];
    GPTP->properties.grandmasterIdentity[3] = 0xFF;
    GPTP->properties.grandmasterIdentity[4] = 0xFE;
    GPTP->properties.grandmasterIdentity[5] = DEFAULT_SOURCE_MAC[3];
    GPTP->properties.grandmasterIdentity[6] = DEFAULT_SOURCE_MAC[4];
    GPTP->properties.grandmasterIdentity[7] = DEFAULT_SOURCE_MAC[5];
    
    GPTP->properties.delayMechanism       = PTP_DELAY_MECHANISM_P2P;
    
    /* Update the system priority vector to match the new properties */
    GPTP->systemPriority.rootSystemIdentity.priority1     = GPTP->properties.grandmasterPriority1;
    GPTP->systemPriority.rootSystemIdentity.clockClass    = GPTP->properties.grandmasterClockQuality.uc_clockClass;
    GPTP->systemPriority.rootSystemIdentity.clockAccuracy = GPTP->properties.grandmasterClockQuality.uc_clockAccuracy;
    VTSS_COMMON_UNALIGNED_PUT_2B(GPTP->systemPriority.rootSystemIdentity.offsetScaledLogVariance,GPTP->properties.grandmasterClockQuality.us_offsetScaledLogVariance);
    
    GPTP->systemPriority.rootSystemIdentity.priority2     = GPTP->properties.grandmasterPriority2;
    memcpy(GPTP->systemPriority.rootSystemIdentity.clockIdentity, GPTP->properties.grandmasterIdentity, sizeof(PtpClockIdentity));
    VTSS_COMMON_UNALIGNED_PUT_2B(GPTP->systemPriority.stepsRemoved,0);
    memcpy(GPTP->systemPriority.sourcePortIdentity.clockIdentity, GPTP->properties.grandmasterIdentity, sizeof(PtpClockIdentity));
    VTSS_COMMON_UNALIGNED_PUT_2B(GPTP->systemPriority.sourcePortIdentity.portNumber,0);
    VTSS_COMMON_UNALIGNED_PUT_2B(GPTP->systemPriority.portNumber,0);
    
    GPTP->pathTraceLength = 1;
    memcpy(&GPTP->pathTrace[0], GPTP->systemPriority.rootSystemIdentity.clockIdentity, sizeof(PtpClockIdentity));
	GPTP->admin_enabled = 1;

    current_port = NULL;        /* Needed if vtss_gptp_deinit() -> vtss_gptp_init() */
    current_sm = NULL;        /* - " - */

    
    /* Initialize all (physical + virtual) ports */
    
    for (pix = 0; pix < VTSS_GPTP_MAX_PORTS; pix++) {

    GPTP->ports[pix].port_number = pix+1;

	vtss_printf("GPTP SW_Init: Port Numbers at initialization are %d\r\n",GPTP->ports[pix].port_number);


    GPTP->ports[pix].announceCounter     = 0;/* log current announce interval in seconds 2^0 */
    GPTP->ports[pix].announceSequenceId  = 0x0000;
    GPTP->ports[pix].syncCounter         = 0; /*log current  sync interval seconds 2^3 */
	GPTP->ports[pix].pdelayIntervalTimer = 0; /* log pdelay interval 2^2 seconds */
    GPTP->ports[pix].syncSequenceId      = 0x0000;
    GPTP->ports[pix].syncSequenceIdValid = 0;
    GPTP->ports[pix].delayReqCounter     = 0;
    GPTP->ports[pix].delayReqSequenceId  = 0x0000;
	

    GPTP->ports[pix].currentLogSyncInterval = 3;
    GPTP->ports[pix].initialLogSyncInterval = GPTP->ports[pix].port_config.initialLogSyncInterval;

    /* TODO: check the ethernet port for link-up here to determine if it should be enabled */
    GPTP->ports[pix].portEnabled = TRUE;
    GPTP->ports[pix].pttPortEnabled = TRUE;
	GPTP->ports[pix].asCapable = 1;

    GPTP->ports[pix].currentLogAnnounceInterval = 0;
    GPTP->ports[pix].initialLogAnnounceInterval = GPTP->ports[pix].port_config.initialLogAnnounceInterval;

    GPTP->ports[pix].syncReceiptTimeout = GPTP->ports[pix].port_config.syncReceiptTimeOut;
    GPTP->ports[pix].announceReceiptTimeout = GPTP->ports[pix].port_config.announceReceiptTimeout;

    /* peer delay request state machine initialization */
    GPTP->ports[pix].mdPdelayReq_State    = MDPdelayReq_NOT_ENABLED;
    GPTP->ports[pix].allowedLostResponses = 3;
    /* Recommended value in 802.1AS/COR1 is 800ns for copper. */
    GPTP->ports[pix].neighborPropDelayThresh = GPTP->ports[pix].port_config.nbrPropDelayThresh;/* Default of 800 ns for copper */

    /* PortAnnounceInformation state machine initialization */
    GPTP->ports[pix].portAnnounceInformation_State = PortAnnounceInformation_BEGIN;

    }


    GPTP->portRoleSelection_State = PortRoleSelection_INIT_BRIDGE;
	vtss_printf("SW_Init: Port Role Selection State Machine\r\n");
    PortRoleSelection_StateMachine(GPTP);
    GPTP->newMaster              = TRUE;

    GPTP->rtcChangesAllowed      = TRUE;

    GPTP->masterRateRatio = 0;
    
    GPTP->masterRateRatioValid = FALSE;


    for(pix = 0; pix < VTSS_GPTP_MAX_PORTS; pix++) {
    
    GPTP->ports[pix].syncTimestampsValid     = 0;
    GPTP->ports[pix].delayReqTimestampsValid = 0;
    GPTP->ports[pix].neighborPropDelay       = 0;
    GPTP->ports[pix].announceTimeoutCounter  = 0;
    GPTP->ports[pix].syncTimeoutCounter      = 0;
  }

   print_str("Setting RTC Clock\r\n");
   set_rtc_increment(GPTP, &GPTP->nominalIncrement);
   print_str("GPTP SW init completed\r\n");
}


static void gptp_set_portconfig(vtss_gptp_port_vars_t *pp, 
                                const vtss_gptp_port_config_t *port_config)
{
    if (memcmp(&pp->port_config, port_config, sizeof(pp->port_config)) == 0)
        return;
    finish_gptp_work();
    VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG, ("Setting port %u config\n",
                    (unsigned)pp->port_number));
    pp->port_config = *port_config;   
}


/*  Begin for 802.1AS  */
void vtss_gptp_set_portconfig(vtss_common_port_t portno,
                              const vtss_gptp_port_config_t VTSS_COMMON_PTR_ATTRIB *port_config)
{
    vtss_gptp_port_vars_t *pp;

    VTSS_GPTP_ASSERT(portno == 0 || VTSS_GPTP_IS_PORT(portno));
    GPTP_INIT;
    finish_gptp_work();
    if (portno == 0) {
        for (pp = &GPTP->ports[VTSS_GPTP_MAX_PORTS]; pp < &GPTP->ports[VTSS_GPTP_MAX_PORTS + VTSS_GPTP_MAX_APORTS]; pp++)
            gptp_set_portconfig(pp, port_config);
    }
    else {
        pp = &GPTP->ports[portno - 1];
        gptp_set_portconfig(pp, port_config);
    }
}

/*  Finish for 802.1AS */


void vtss_gptp_get_config(vtss_gptp_system_config_t VTSS_COMMON_PTR_ATTRIB *system_config)
{
    GPTP_INIT;
    *system_config = GPTP->system_config;
}

void vtss_gptp_get_portconfig(vtss_common_port_t portno,
                              vtss_gptp_port_config_t VTSS_COMMON_PTR_ATTRIB *port_config)
{
    VTSS_GPTP_ASSERT(portno == 0 || VTSS_GPTP_IS_PORT(portno));
    GPTP_INIT;
    *port_config = GPTP->ports[portno ? portno - 1 : VTSS_GPTP_MAX_PORTS].port_config;
}


/* COMEBACK*/
void vtss_gptp_port_timers_tick(void)
{

}




void vtss_gptp_tick(void)
{
    //print_str("Calling finish_gptp_work\r\n");
    finish_gptp_work();
    GPTP->timerTicks++;   
    
    GPTP_machine_update();
}


static void process_rx_announce(gptpm_t *ptp, vars_ulong32 port, vars_uchar8 *rxBuffer) {

  ptp->ports[port].stats.rxAnnounceCount++;

  ptp->ports[port].rcvdAnnouncePtr = rxBuffer;

  PortAnnounceReceive_StateMachine(ptp, port);
}


#define ONE_BILLION  (0x3B9ACA00)

/* Normalizes the sign of the two components of a timestamp */
static void normalize_timestamp(vtss_ptp_time_t *operand) {
  if(operand->secondsUpper < 0) {
    if(operand->nanoseconds > 0) {
      if(operand->secondsLower++ == 0xFFFFFFFF) operand->secondsUpper++;
      operand->nanoseconds -= ONE_BILLION;
    }
  } else if((operand->secondsUpper > 0) || (operand->secondsLower > 0)) {
    if(operand->nanoseconds < 0) {
      if(operand->secondsLower-- == 0x00000000) operand->secondsUpper--;
      operand->nanoseconds += ONE_BILLION;
    }
  }
}

/* Adds two timestamps */
void timestamp_sum(vtss_ptp_time_t *addend, vtss_ptp_time_t *augend, vtss_ptp_time_t *sum) {
  sum->secondsUpper = (addend->secondsUpper + augend->secondsUpper);
  sum->secondsLower = (addend->secondsLower + augend->secondsLower);
  if(sum->secondsLower < addend->secondsLower) sum->secondsUpper++;
  sum->nanoseconds = (addend->nanoseconds + augend->nanoseconds);
  if(sum->nanoseconds >= ONE_BILLION) {
    if(sum->secondsLower == 0xFFFFFFFF) sum->secondsUpper++;
    sum->secondsLower++;
    sum->nanoseconds -= ONE_BILLION;
  } else if(sum->nanoseconds <= -ONE_BILLION) {
    if(sum->secondsLower == 0x00000000) sum->secondsUpper--;
    sum->secondsLower--;
    sum->nanoseconds += ONE_BILLION;
  }
  normalize_timestamp(sum);
}

/* Subtracts two timestamps */
void timestamp_difference(vtss_ptp_time_t *minuend, vtss_ptp_time_t *subtrahend, vtss_ptp_time_t *difference) {
  difference->secondsUpper = (minuend->secondsUpper - subtrahend->secondsUpper);
  difference->secondsLower = (minuend->secondsLower - subtrahend->secondsLower);
  difference->nanoseconds = (minuend->nanoseconds - subtrahend->nanoseconds);
  if(subtrahend->secondsLower > minuend->secondsLower) difference->secondsUpper--;
  if(difference->nanoseconds >= ONE_BILLION) {
    if(difference->secondsLower == 0xFFFFFFFF) difference->secondsUpper++;
    difference->secondsLower++;
    difference->nanoseconds -= ONE_BILLION;
  } else if(difference->nanoseconds <= -ONE_BILLION) {
    if(difference->secondsLower == 0x00000000) difference->secondsUpper--;
    difference->secondsLower--;
    difference->nanoseconds += ONE_BILLION;
  }
  normalize_timestamp(difference);
}

/* Computes the absolute value of a timestamp */
void timestamp_abs(vtss_ptp_time_t *operand, vtss_ptp_time_t *result) {
  /* Copy to the result first */
  timestamp_copy(result, operand);

  /* The value has been normalized; simply do an absolute value on both fields */
  if(result->secondsUpper < 0) {
    /* Negative seconds */
    result->secondsUpper = ~result->secondsUpper;
    result->secondsLower = ~result->secondsLower;
    if(result->secondsLower++ == 0xFFFFFFFF) result->secondsUpper++;
  }
  if(result->nanoseconds < 0) result->nanoseconds = (0 - result->nanoseconds);
}

/* Copies one timestamp to another */
void timestamp_copy(vtss_ptp_time_t *destination, vtss_ptp_time_t *source) {
  destination->secondsUpper = source->secondsUpper;
  destination->secondsLower = source->secondsLower;
  destination->nanoseconds = source->nanoseconds;
}


void get_32_into_timestamp(vars_uchar8 *value,vtss_ptp_time_t *timestamp)
{



    char *sec0_p = (char *) &timestamp->secondsLower;
    char *sec1_p = (char *) &timestamp->secondsUpper;
    char *nsec_p = (char *) &timestamp->nanoseconds;

    char* source_time = (char*)value;

      /*Upper Seconds */
    sec1_p[2] = source_time[0];
    sec1_p[3] = source_time[1];
    /* Lower seconds */                         
    sec0_p[0] = source_time[2];
    sec0_p[1] = source_time[3];
    sec0_p[2] = source_time[4];
    sec0_p[3] = source_time[5];

    nsec_p[0] = source_time[6];
    nsec_p[1] = source_time[7];
    nsec_p[2] = 0;
    nsec_p[3] = 0;

    


}


void get_hardware_timestamp(struct gptpm *ptp, 
                            vars_ulong32 port, 
                            PacketDirection bufferDirection,
                            vars_uchar8 *packetBuffer, 
                            vtss_ptp_time_t *timestamp)
{
    /* We Do not have a nansecond precision clock here, values are counted in seconds.
        * we shall propose a simulated local time,with a random value added for nano seconds.
        */
    vtss_ptp_time_t tempTimestamp;

	char* sec0_p = NULL;
	char* sec1_p = NULL;
	char* nsec_p = NULL;
	char* source_time = NULL;

	packetBuffer = packetBuffer;

    sec0_p = (char *) &tempTimestamp.secondsLower;
    sec1_p = (char *) &tempTimestamp.secondsUpper;
    nsec_p = (char *) &tempTimestamp.nanoseconds;

    source_time = (char*)&time_since_boot;

    /*Upper Seconds */
    sec1_p[2] = source_time[0];
    sec1_p[3] = source_time[1];
    /* Lower seconds */                         
    sec0_p[0] = source_time[0];
    sec0_p[1] = source_time[1];
    sec0_p[2] = source_time[2];
    sec0_p[3] = source_time[3];


    /* Nano seconds is just a fake time */
    nsec_p[0] = source_time[3];
    nsec_p[1] = source_time[2];
    nsec_p[2] = source_time[1];
    nsec_p[3] = source_time[0];

    
    if (bufferDirection == TRANSMITTED_PACKET) {
      /* Add the MAC latency and the PHY latency */
      timestamp_sum(&tempTimestamp, &ptp->ports[port].txPhyMacDelay, timestamp);
    } else {
      /* Subtract the MAC latency and the PHY latency */
      timestamp_difference(&tempTimestamp, &ptp->ports[port].rxPhyMacDelay, timestamp);
    }

    
}




/* Gets the correction field from the passed Rx packet buffer */
void get_correction_field(struct gptpm *ptp, vars_ulong32 port, vars_uchar8 *rxBuffer, vtss_ptp_time_t *correctionField)
{
    ComMessageHdr *pComMesgHdr = (ComMessageHdr *) rxBuffer;

    int i = 0;

	ptp = ptp;

	port = port;

    correctionField->secondsUpper = 0;
    correctionField->secondsLower = 0;


    /* Get the last 16 Bits
        * for fractional nanosecond granularity
        */
    for(i = 6;i < 8 ;i++)
    {
        correctionField->nanoseconds = (correctionField->nanoseconds << 8) + pComMesgHdr->correctionField[i];
    }


}



static void process_rx_sync(struct gptpm *ptp, vars_ulong32 port, vars_uchar8 *rxBuffer) {
    PtpPortIdentity rxIdentity;    

    ComMessageHdr *pComMesgHdr = (ComMessageHdr *) rxBuffer;
    

    ptp->ports[port].stats.rxSyncCount++;
    ptp->ports[port].syncTimeoutCounter = 0;

   /* Only process this packet if we are a slave and it has come from the master
       * we're presently respecting.  If we're the master, spanning tree should prevent
       * us from ever seeing our own SYNC packets, but better safe than sorry.
       */
  get_source_port_id(ptp, port, RECEIVED_PACKET, rxBuffer, (vars_uchar8*)&rxIdentity);
  if((ptp->ports[port].selectedRole == PTP_SLAVE) && 
     (0 == memcmp(&rxIdentity, &ptp->gmPriority->sourcePortIdentity, sizeof(PtpPortIdentity)))) {
    vtss_ptp_time_t tempTimestamp;
    vtss_ptp_time_t correctionField;
    vtss_ptp_time_t correctedTimestamp;

    /* This is indeed a SYNC from the present master.  Capture the hardware timestamp
     * at which we received it, and hang on to its sequence ID for matching to the
     * followup that should follow.
     */

    
    get_hardware_timestamp(ptp, port, RECEIVED_PACKET, rxBuffer, &tempTimestamp);
    get_correction_field(ptp, port, rxBuffer, &correctionField);
    timestamp_difference(&tempTimestamp, &correctionField, &correctedTimestamp);

   
    timestamp_copy(&ptp->ports[port].syncRxTimestampTemp, &correctedTimestamp);
    ptp->ports[port].syncSequenceId = VTSS_COMMON_UNALIGNED_GET_2B(pComMesgHdr->sequenceId);
    ptp->ports[port].syncSequenceIdValid = 1;

    ptp->ports[port].waiting_for_fup = 1;
   
   
  }
}


void get_timestamp(struct gptpm *ptp, vars_ulong32 port,int bufferDirection, vars_uchar8 *rxBuffer,vtss_ptp_time_t *ts)
{
  char *sec1_p = NULL;
  char *sec0_p = NULL;
  char *nsec_p = NULL;
  ptp = ptp;
  port = port;

  bufferDirection = bufferDirection;
  
  sec1_p = (char *)&ts->secondsUpper;
  sec0_p = (char *)&ts->secondsLower;
  nsec_p = (char *)&ts->nanoseconds;

  sec1_p[2] = rxBuffer[0];
  sec1_p[3] = rxBuffer[1];
                             
  sec0_p[0] = rxBuffer[2];
  sec0_p[1] = rxBuffer[3];
  sec0_p[2] = rxBuffer[4];
  sec0_p[3] = rxBuffer[5];
                           
  nsec_p[0] = rxBuffer[6];
  nsec_p[1] = rxBuffer[7];
  nsec_p[2] = rxBuffer[8];
  nsec_p[3] = rxBuffer[9];
  
  return;
}


static void put_timestamp(struct gptpm *ptp, vars_ulong32 port, vars_uchar8 *rxBuffer, vtss_ptp_time_t *ts)
{
    char *sec0_p = (char *) &ts->secondsUpper;
    char *sec1_p = (char *) &ts->secondsLower;
    char *nsec_p = (char *) &ts->nanoseconds;

    ptp = ptp;
	port = port;

	vtss_printf("Putting the time stamp \r\n");

    rxBuffer[0] =  sec0_p[2];
    rxBuffer[1] =  sec0_p[3];

    rxBuffer[2] =  sec1_p[0];
    rxBuffer[3] =  sec1_p[1];
    rxBuffer[4] =  sec1_p[2];
    rxBuffer[5] =  sec1_p[3];
    
    rxBuffer[6] =  nsec_p[0];
    rxBuffer[7] =  nsec_p[1];    
    rxBuffer[8] =  nsec_p[2];
    rxBuffer[9] =  nsec_p[3];



}

void set_rtc_increment(struct gptpm *ptp, RtcIncrement *increment) {

  _UINT64 mantissa;
  _UINT64 fraction;
 

  /* Save the current increment if anyone needs it */
  ptp->currentIncrement = *increment;

  /* Assemble a single value from the increment components */
  

  /* The actual write is already atomic, so no need to ensure mutual exclusion */

  /* Write this to the RTC Clock */

  Uint32ToUint64(mantissa,increment->mantissa);
  mantissa = Uint64LShift(mantissa,27);
  Uint32ToUint64(fraction,increment->fraction);

  ptp->rtc_clock_vars.rtc_adj = Uint64Or(mantissa,fraction);
  
  Uint32ToUint64(mantissa,ptp->nominalIncrement.mantissa);
  mantissa = Uint64LShift(mantissa,27);
  Uint32ToUint64(fraction,ptp->nominalIncrement.fraction);

  ptp->rtc_clock_vars.rtc_nominal = Uint64Or(mantissa,fraction);

  
  
  ptp->rtc_clock_vars.rtc_adj = Uint64LShift(ptp->rtc_clock_vars.rtc_adj,32);


  ptp->rtc_clock_vars.rtc_fraction = Uint64DividedByUint64(ptp->rtc_clock_vars.rtc_adj,ptp->rtc_clock_vars.rtc_nominal);

  ptp->rtc_clock_vars.rtc_fraction = Uint64RShift(ptp->rtc_clock_vars.rtc_fraction ,1);  



}

void get_rtc_time(struct gptpm *ptp,vtss_ptp_time_t *time) {
 
  char *sec0_p = NULL;
  char *sec1_p = NULL;
  char *nsec_p = NULL;
  char* source_time = NULL;

  sec0_p =  (char *)&time->secondsLower;
  sec1_p = (char *)&time->secondsUpper;
  nsec_p = (char *)&time->nanoseconds;

  ptp = ptp;

  vtss_printf("Getting RTC Time \r\n");

  source_time = (char*)&time_since_boot;

  /*Upper Seconds */
  sec1_p[2] = source_time[0];
  sec1_p[3] = source_time[1];
    /* Lower seconds */                         
  sec0_p[0] = source_time[0];
  sec0_p[1] = source_time[1];
  sec0_p[2] = source_time[2];
  sec0_p[3] = source_time[3];


  /* Nano seconds is just a fake time */
  nsec_p[0] = source_time[3];
  nsec_p[1] = source_time[2];
  nsec_p[2] = source_time[1];
  nsec_p[3] = source_time[0];

  /* Write to the capture flag in the upper seconds word to initiate a capture,
   * then poll the same bit to determine when it has completed.  The capture only
   * takes a few RTC clocks, so this busy wait can only consume tens of nanoseconds.
   *
   * This will *not* modify the time, since we don't write the nanoseconds register.
   */
  
}


void set_rtc_time(struct gptpm *ptp, vtss_ptp_time_t *time) {

    char *sec0_p = (char *) &time->secondsLower;
    char *sec1_p = (char *) &time->secondsUpper;
    char *nsec_p = (char *) &time->nanoseconds;

    char* source_time = (char*)&time_since_boot;

	ptp = ptp;

    /*Upper Seconds */
     source_time[0] = sec1_p[2] ;
     source_time[1] = sec1_p[3];
    /* Lower seconds */                         
     source_time[0] = sec0_p[0];
     source_time[1] = sec0_p[1];
     source_time[2] = sec0_p[2];
     source_time[3] = sec0_p[3];
  
}



/* Sets a new RTC time from the passed structure */
void set_rtc_time_adjusted(struct gptpm *ptp, vtss_ptp_time_t *time, vtss_ptp_time_t *entryTime) {
  
  vtss_ptp_time_t timeNow;
  vtss_ptp_time_t timeDifference;
  vtss_ptp_time_t adjustedTime;

  /* Get the current time and make the adjustment with as little jitter as possible */
  

  get_rtc_time(ptp, &timeNow);
  timestamp_difference(&timeNow, entryTime, &timeDifference);
  timestamp_sum(time, &timeDifference, &adjustedTime);
  set_rtc_time(ptp, &adjustedTime);

  
}




/* Calculate the rate ratio from the master. Note that we reuse the neighbor rate ratio 
   fields from PDELAY but it is really the master we are talking to here. */
static void computeDelayRateRatio(struct gptpm *ptp, vars_ulong32 port)
{
  if (ptp->ports[port].initPdelayRespReceived == FALSE)
  {
    /* Capture the initial DELAY response */
    ptp->ports[port].initPdelayRespReceived = TRUE;
    ptp->ports[port].pdelayRespTxTimestampI = ptp->ports[port].delayReqTxLocalTimestamp;
    ptp->ports[port].pdelayRespRxTimestampI = ptp->ports[port].delayReqRxTimestamp;
  }
  else
  {
    vtss_ptp_time_t difference;
    vtss_ptp_time_t difference2;
    _UINT64 nsResponder;
    _UINT64 nsTemp;
    _UINT64 nsTemp2;
    _UINT64 nsRequester;
    _UINT64 rateRatio;
    _UINT64 multiplier;    
    int shift;

    multiplier.HighPart = 10000000;

    timestamp_difference(&ptp->ports[port].delayReqTxLocalTimestamp, &ptp->ports[port].pdelayRespTxTimestampI, &difference2);
    timestamp_difference(&ptp->ports[port].delayReqRxTimestamp, &ptp->ports[port].pdelayRespRxTimestampI, &difference);

    /* The raw differences have been computed; sanity-check the peer delay timestamps; if the 
     * initial Tx or Rx timestamp is later than the present one, the initial ones are bogus and
     * must be replaced.
     */
    if((difference.secondsUpper & 0x80000000) |
       (difference2.secondsUpper & 0x80000000)) {
      ptp->ports[port].initPdelayRespReceived = FALSE;
      ptp->ports[port].neighborRateRatioValid = FALSE;
      ptp->masterRateRatioValid = FALSE;
    } else {
       Uint32ToUint64(nsTemp,difference.secondsLower);
       nsResponder = Uint64AddUint32(Uint64TimesUint32(nsTemp,1000000000),(unsigned long)(difference.nanoseconds));

           
       Uint32ToUint64(nsTemp,difference2.secondsLower);
       nsRequester = Uint64AddUint32(Uint64TimesUint32(nsTemp,1000000000),(unsigned long)difference2.nanoseconds);


      
      for (shift = 0; shift < 31; shift++)
      {
          if (nsResponder.HighPart & (1<<(31-shift))) break;
      }


      nsTemp = Uint64RShift(nsRequester,(31-shift));

      if(!Int64IsZero(nsTemp))
      {

          nsTemp = Uint64LShift(nsResponder,shift);
          nsTemp2 = Uint64RShift(nsRequester,(31-shift));
          rateRatio = Uint64DividedByUint64(nsTemp,nsTemp2);
          
          if ((Uint64ToUint32(rateRatio) < RATE_RATIO_MAX) && (Uint64ToUint32(rateRatio) > RATE_RATIO_MIN)) {
              ptp->ports[port].neighborRateRatio = Uint64ToUint32(rateRatio);
 
              ptp->ports[port].neighborRateRatioValid = TRUE;

             /* Master rate is the same for E2E mode */
              ptp->masterRateRatio = Uint64ToUint32(rateRatio);
              ptp->masterRateRatioValid = TRUE;
        } else {
          /* If we are outside the acceptable range, assume our initial values are bad and grab new ones */
          ptp->ports[port].initPdelayRespReceived = FALSE;
          ptp->ports[port].neighborRateRatioValid = FALSE;
          ptp->masterRateRatioValid = FALSE;
        }



      }


#ifdef PATH_DELAY_DEBUG
      VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Responder delta: %08X%08X.%08X (%llu ns)\n", difference.secondsUpper,
             difference.secondsLower, difference.nanoseconds, nsResponder));
      VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Requester delta: %08X%08X.%08X (%llu ns)\n", difference2.secondsUpper,
             difference2.secondsLower, difference2.nanoseconds, nsRequester));
      VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Rate ratio: %08X (shift %d)\n", ptp->ports[port].neighborRateRatio, shift));
#endif
    } /* if(differences are sane) */
  }
}




void rtc_update_servo(struct gptpm *ptp, vars_ulong32 port) {
  vars_long32 slaveOffset       = 0;
  vars_ulong32 slaveOffsetValid = PTP_RTC_OFFSET_INVALID;
  vtss_ptp_time_t difference;

  /* Update the servo using the appropriate delay mechanism */
  if(ptp->properties.delayMechanism == PTP_DELAY_MECHANISM_E2E) {
    /* Make certain there are both sets of valid timestamps available for a master->
     * slave offset calculation employing the end-to-end mechanism.
     */
    if(ptp->ports[port].syncTimestampsValid && ptp->ports[port].delayReqTimestampsValid) {
      vtss_ptp_time_t difference2;
    
      computeDelayRateRatio(ptp, port);

      /* The core of the algorithm is the calculation of the slave's offset from the
       * master, eliminating the network delay from the equation:
       *
       * [SYNC Rx time - SYNC Tx time] = slave_error + link_delay
       * [DELAY_REQ Rx time - DELAY_REQ Tx time] = -slave_error + link_delay
       *
       * Rearranging terms to get link delay by itself and equate the two expressions
       * gives the following equation for the master-to-slave offset:
       *
       * Offset_m_s = [(SYNC_Rx - SYNC_Tx) + (DELAY_REQ_Tx - DELAY_REQ_Rx)] / 2
       */
      timestamp_difference(&ptp->ports[port].syncRxTimestamp, &ptp->ports[port].syncTxTimestamp, &difference);
      timestamp_difference(&ptp->ports[port].delayReqTxTimestamp, &ptp->ports[port].delayReqRxTimestamp, &difference2);
      
      /* The fact that this is called at all implies there's a < 1 sec slaveOffset; deal
       * strictly with nanoseconds now that the seconds have been normalized.
       */
      slaveOffset = (((int) difference.nanoseconds) + ((int) difference2.nanoseconds));
      slaveOffset >>= 1;
      slaveOffsetValid = PTP_RTC_OFFSET_VALID;

      /* Save the delay in the same spot as P2P mode does for consistency. */
      ptp->ports[port].neighborPropDelay = (-difference2.nanoseconds) + slaveOffset;

      /* Mark the delay timestamps as invalid so we don't keep using them with their old offset */
      ptp->ports[port].delayReqTimestampsValid = 0;
    } 
    else if (ptp->ports[port].syncTimestampsValid)
    {

      /* Cancel out the link delay with the last computed value.
       *
       * [SYNC Rx time - SYNC Tx time] = slave_error + link_delay
       * slaveOffset = slave_error + link_delay - link delay
       */
      timestamp_difference(&ptp->ports[port].syncRxTimestamp, &ptp->ports[port].syncTxTimestamp, &difference);
      slaveOffset = difference.nanoseconds - ptp->ports[port].neighborPropDelay;
      slaveOffsetValid = PTP_RTC_OFFSET_VALID;
    }
  }
  else 
  {
    /* The peer delay mechanism uses the SYNC->FUP messages, but relies upon the
     * messages having had their correction field updated by bridge residence time
     * logic along the way.  Since that is performed, the only remaining correction
     * to be made is to subtract out the path delay to our peer, which is periodically
     * calculated (and should be pretty small.)
     */
    timestamp_difference(&ptp->ports[port].syncRxTimestamp, &ptp->ports[port].syncTxTimestamp, &difference);
      
    /* The fact that this is called at all implies there's a < 1 sec slaveOffset; deal
     * strictly with nanoseconds now that the seconds have been normalized.
     */
    slaveOffset = (((int) difference.nanoseconds) - ptp->ports[port].neighborPropDelay);
    slaveOffsetValid = PTP_RTC_OFFSET_VALID;

    if (ptp->ports[port].neighborRateRatioValid) {
      _INT64 tempRate1;
      _UINT64 tempRate2;
      // Convert from 2^-41 - (1.0) back to something in the 2^-31 range and add the 1.0 back in
      tempRate1 = Int64AddInt32(Uint64ToInt64(tempRate1),(((int)ptp->ports[port].cumulativeScaledRateOffset) >> 10));      
      tempRate1 = Uint64ToInt64(Uint64AddUint32(Int64ToUint64(tempRate1),0x80000000));

      tempRate2 = Int64ToUint64(tempRate1);

      // Get the cumulative rate ratio, including our neighbor

      tempRate2 = Uint64TimesUint32(tempRate2,ptp->ports[port].neighborRateRatio);

      tempRate2 = Uint64RShift(tempRate2,31);
      

      ptp->masterRateRatio = Uint64ToUint32(tempRate2);
      ptp->masterRateRatioValid = TRUE;
    }
    else {
      ptp->masterRateRatioValid = FALSE;
    }
  }

  /* Perform the actual servo update if the slave offset is valid */
  if(slaveOffsetValid == PTP_RTC_OFFSET_VALID) {
    unsigned long newRtcIncrement;
    _INT64 coefficient;
    _INT64 slaveOffsetExtended;
    _INT64 temp;
    _INT64 accumulator;
    _UINT64 accumulator2;
    _INT64 constant;
    _INT64 constant2;
    
    
    int adjustment;

    /* Update the servo with the present value; begin with the master rate ratio
     * if it is available, otherwise start with the nominal increment */
    if (ptp->masterRateRatioValid) {
      newRtcIncrement = ptp->masterRateRatio >> 1;

      /* If we crossed the midpoint, damp the integral */
      if (((slaveOffset < 0) && (ptp->previousOffset > 0)) ||
          ((slaveOffset > 0) && (ptp->previousOffset < 0))) {
        
        ptp->integral = Uint64RShift(ptp->integral,1);
      }
    } else {
      newRtcIncrement = (ptp->nominalIncrement.mantissa & RTC_MANTISSA_MASK);
      newRtcIncrement <<= RTC_MANTISSA_SHIFT;
      newRtcIncrement |= (ptp->nominalIncrement.fraction & RTC_FRACTION_MASK);
    }
    
    /* Operate in two distinct modes; a high-gain, purely-proportional control loop
     * when we're far from the master, and a more complete set of controls once we've
     * narrowed in
     */
    if(ptp->acquiring == PTP_RTC_ACQUIRING) {
      if((slaveOffset > ACQUIRE_THRESHOLD) || (slaveOffset < -ACQUIRE_THRESHOLD)) {
        /* Continue in acquiring mode; accumulate the proportional coefficient's contribution */
        Int32ToInt64(coefficient,ACQUIRE_COEFF_P);

        Int32ToInt64(slaveOffsetExtended,slaveOffset);

        accumulator = Int64RShift(Int64TimesInt64(coefficient,slaveOffsetExtended),COEFF_PRODUCT_SHIFT);
        
        
        if(servoCount >= 10) {
          vars_ulong32  wordChunk;

          accumulator = Int64RShift(accumulator,32);
          accumulator2 = Int64ToUint64(accumulator);
          wordChunk =  Uint64ToUint32(accumulator2);
          VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Acquiring, P contribution = 0x%08X", wordChunk));
          wordChunk = Uint64ToUint32(Int64ToUint64(accumulator));
          VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("%08X\n", wordChunk));
        }

      } else {
        /* Reached the acquisition band */
        ptp->acquiring = PTP_RTC_ACQUIRED;
      }

      /* Also dump the integrator for the integral term */
      Uint32ToUint64(ptp->integral,0);
    }

    /* Now check for "acquired" mode */
    if(ptp->acquiring == PTP_RTC_ACQUIRED) {
      /* We are in the acquisition band; see if we've wandered beyond it badly enough to
       * go back into acquiring mode, producing some hysteresis
       */
      if((slaveOffset > (8 * ACQUIRE_THRESHOLD)) || (slaveOffset < (8 * -ACQUIRE_THRESHOLD))) {
        ptp->acquiring = PTP_RTC_ACQUIRING;
      }

      /* Accumulate the proportional coefficient's contribution */
      Int32ToInt64(slaveOffsetExtended,slaveOffset);
      Int32ToInt64(coefficient,ptp->coefficients.P);
      accumulator = Int64RShift(Int64TimesInt64(coefficient ,slaveOffsetExtended),COEFF_PRODUCT_SHIFT);


   /* Accumulate the integral coefficient's contribution, clamping the integrated
       * error to its bounds.
       */
      Int32ToInt64(coefficient,ptp->coefficients.I);			        

      ptp->integral = UInt64Add(ptp->integral,Int64ToUint64(slaveOffsetExtended)); 

      Uint32ToUint64(constant,INTEGRAL_MAX_ABS);
      Uint32ToUint64(constant2,-INTEGRAL_MAX_ABS);

      if(Int64Less(constant,ptp->integral))
      {    
          Uint32ToUint64(ptp->integral,INTEGRAL_MAX_ABS);
      }else if (Int64Less(ptp->integral,constant2)) {
          Int32ToInt64(ptp->integral,-INTEGRAL_MAX_ABS)
      }


      accumulator = Int64Add(accumulator,(Int64RShift(Int64TimesInt64(coefficient,Uint64ToInt64(ptp->integral)),COEFF_PRODUCT_SHIFT)));

     

      /* Accumulate the derivitave coefficient's contribution */
      Int32ToInt64(coefficient,ptp->coefficients.D);
      
      ptp->derivative += (slaveOffset - ptp->previousOffset); /* TODO: Scale based on the time between syncs? */

      
      Int32ToInt64(temp,ptp->derivative);
      accumulator = Int64Add(accumulator,(Int64RShift(Int64TimesInt64(coefficient,temp),COEFF_PRODUCT_SHIFT)));
      
      ptp->previousOffset = slaveOffset;

    }



    /* Clamp the new increment to within +/- one nanosecond of nominal */

    Int32ToInt64(constant,INCREMENT_DELTA_MAX);
    Int32ToInt64(constant2,INCREMENT_DELTA_MIN);

    if(Int64Less(constant,accumulator))
    {
        adjustment = INCREMENT_DELTA_MAX;
    }
    else if (Int64Less(accumulator,constant2))
    {
        adjustment = INCREMENT_DELTA_MIN;   
    }
    else
    {
        adjustment = Int64ToInt32(accumulator);
    }
   
    newRtcIncrement += adjustment;

    /* Write the new increment out to the hardware, incorporating the enable bit.
     * Suppress the actual write to the RTC increment register if the userspace
     * control has not acknowledged a Grandmaster change.
     */

    if(ptp->rtcChangesAllowed) {
      RtcIncrement newIncrement;
      newIncrement.mantissa = (newRtcIncrement >> RTC_MANTISSA_SHIFT) & RTC_MANTISSA_MASK;
      newIncrement.fraction = (newRtcIncrement & RTC_FRACTION_MASK);
      set_rtc_increment(ptp,&newIncrement);
    }

#ifdef SLAVE_OFFSET_DEBUG
    if(servoCount++ >= 10) {
      VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Slave offset %d\n", slaveOffset));
      VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("  syncRxNS %d, syncTxNS %d (%d), MeanPathNS %d\n", (int)ptp->ports[port].syncRxTimestamp.nanoseconds,
        (int)ptp->ports[port].syncTxTimestamp.nanoseconds, (int)difference.nanoseconds, (int)ptp->ports[port].neighborPropDelay));
      VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("RTC increment 0x%08X", newRtcIncrement));
      if(adjustment == INCREMENT_DELTA_MIN) {
        VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,(" (MIN CLAMP)"));
      } else if(adjustment == INCREMENT_DELTA_MAX) {
        printk(VTSS_GPTP_TRLVL_DEBUG,(" (MAX CLAMP)"));
      }
      VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("\n"));
      servoCount = 0;
    }
#endif
  } /* if(slaveOffsetValid) */

  /* Store the offset and its validity to the device structure for use by
   * the lock detection state machine
   */
  ptp->rtcLastOffsetValid = slaveOffsetValid;
  ptp->rtcLastOffset      = slaveOffset;
}





static void process_rx_fup(struct gptpm *ptp, vars_ulong32 port, vars_uchar8 *rxBuffer) {
  
  unsigned long prop_delay;
  vars_ushort16 i = 0;

  ComMessageHdr *pComMesgHdr = (ComMessageHdr *) rxBuffer;

  FollowUpMessage *pFupMsg = (FollowUpMessage *) ((char *) pComMesgHdr+sizeof(ComMessageHdr));

  ptp->ports[port].stats.rxFollowupCount++;

  /* Make certain of the following:
   * - We are a slave
   * - This is from our master
   * - The sequence ID matches the last valid SYNC message
   */
  get_source_port_id(ptp, port, RECEIVED_PACKET, rxBuffer, (vars_uchar8*)&ptp->rxSourcePortIdentity);
  if((ptp->ports[port].selectedRole == PTP_SLAVE) && 
     (0 == memcmp(&ptp->rxSourcePortIdentity, &ptp->gmPriority->sourcePortIdentity, sizeof(PtpPortIdentity))) &&
     ptp->ports[port].syncSequenceIdValid && 
     (VTSS_COMMON_UNALIGNED_GET_2B(pComMesgHdr->sequenceId) == ptp->ports[port].syncSequenceId) && (ptp->ports[port].waiting_for_fup == 1)) {
     
    vtss_ptp_time_t syncTxTimestamp;
    vtss_ptp_time_t correctionField;
    vtss_ptp_time_t correctedTimestamp;
    vtss_ptp_time_t difference;
    vtss_ptp_time_t absDifference;
    vtss_ptp_time_t prop_delay_time;

 

    /* Everything matches; obtain the preciseOriginTimestamp from the packet.
     * This is the time at which the master captured its transmit of the preceding
     * SYNC, which we also timestamped reception for.
     */


    ptp->ports[port].waiting_for_fup = 0;
    
    get_timestamp(ptp, port, RECEIVED_PACKET, pFupMsg->preciseOriginTimestamp, &syncTxTimestamp);

    /* Correct the Tx timestamp with the received correction field */
    get_correction_field(ptp, port, rxBuffer, &correctionField);
    timestamp_sum(&syncTxTimestamp, &correctionField, &correctedTimestamp);

    /* Set the correction field into the global correction field used for subsequent sync's send */
    timestamp_copy(&ptp->rxCorrectionField,&correctionField);


    /*Set the precise origin time stamp to global precise origintimeStamp, goes unchanged in local sync to peers */
    timestamp_copy(&ptp->rxPreciseOriginTimeStamp,&syncTxTimestamp);




    timestamp_copy(&ptp->ports[port].syncRxTimestamp, &ptp->ports[port].syncRxTimestampTemp);
    timestamp_copy(&ptp->ports[port].syncTxTimestamp, &correctedTimestamp);
    ptp->ports[port].syncTimestampsValid = 1;
    
    /* Retrieve the scaled rate offset */
    ptp->ports[port].cumulativeScaledRateOffset = pFupMsg->cumulativeScaledRateOffset;

    /* Calculate RateRatio from cumulativeScaledRateOffset */
    if(ptp->ports[port].neighborRateRatioValid)
    {  
        /* Calculate RateRatio */
        ptp->rateRatio = (vars_ulong32)((ptp->ports[port].cumulativeScaledRateOffset + 1)+(ptp->ports[port].neighborRateRatio - 1));
         /*Calculate the upStreamTxTime  */
        if(ptp->ports[port].neighborPropDelay != 0)
        {
           /* nbr prop delay/neighbor rate ratio + delayAssmetry(10)/rate-ratio*/
           prop_delay = ((ptp->ports[port].neighborPropDelay)/(ptp->ports[port].neighborRateRatio)) + ((10/ptp->rateRatio));
           get_32_into_timestamp((vars_uchar8*)&prop_delay,&prop_delay_time);
           /*upStreamTxTime = (sync rxtime) - (prop delay relative to local clock) */
           timestamp_difference(&ptp->ports[port].syncRxTimestamp, &prop_delay_time, &difference);
           timestamp_abs(&difference, &absDifference);
           /*Set the Upstream Transmit Time */
           ptp->upSTreamTxTime = absDifference;
      
           
        }
        
    }
   

    /* Compare the timestamps; if the one-way offset plus delay is greater than
     * the reset threshold, we need to reset our RTC before beginning to servo.  Regardless
     * of what we do, we need to invalidate the sync sequence ID, it's been "used up."
     */
    ptp->ports[port].syncSequenceIdValid = 0;
    timestamp_difference(&ptp->ports[port].syncRxTimestampTemp, &correctedTimestamp, &difference);
    timestamp_abs(&difference, &absDifference);
    if((absDifference.secondsUpper > 0) || (absDifference.secondsLower > 0) ||
       (absDifference.nanoseconds > RESET_THRESHOLD_NS)) {
      /* Reset the time using the corrected timestamp adjusted by the time it has been
       * resident locally since it was received; also re-load the nominal
       * RTC increment in advance in order to always have a known starting point
       * for convergence.  Suppress this if the  controller hasn't acknowledged
       * a Grandmaster change yet.
       */
      if(ptp->rtcChangesAllowed) {
        VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Resetting RTC!\n"));
        set_rtc_increment(ptp, &ptp->nominalIncrement);
        set_rtc_time_adjusted(ptp, &correctedTimestamp, &ptp->ports[port].syncRxTimestampTemp);
      }
   } else {
      /* Less than a second, leave these timestamps and update the servo */

      VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Sync Rx: %08X%08X.%08X\n", ptp->ports[port].syncRxTimestampTemp.secondsUpper,
        ptp->ports[port].syncRxTimestampTemp.secondsLower, ptp->ports[port].syncRxTimestampTemp.nanoseconds));
      VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Sync Tx: %08X%08X.%08X (corrected: %08X%08X.%08X\n", syncTxTimestamp.secondsUpper,
        syncTxTimestamp.secondsLower, syncTxTimestamp.nanoseconds, correctedTimestamp.secondsUpper,
        correctedTimestamp.secondsLower, correctedTimestamp.nanoseconds));
      VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Correction: %08X%08X.%08X\n", correctionField.secondsUpper,
        correctionField.secondsLower, correctionField.nanoseconds));

    
      rtc_update_servo(ptp, port);
      
    }

    /* Trigger Sync Send on Ports */



    for (i=0; i<current_sm->numPorts; i++)
    {
        if (current_sm->ports[i].selectedRole == PTP_MASTER) 
        {              
                transmit_sync(current_sm, i);
        }
        
    }
   
  }
}


int compare_clock_identity(const vars_uchar8 *clockIdentityA, const vars_uchar8 *clockIdentityB)
{
  vars_ulong32  byteIndex;
  int comparisonResult = 0;

  for(byteIndex = 0; byteIndex < PTP_CLOCK_IDENTITY_BYTES; byteIndex++) {
    if(clockIdentityA[byteIndex] < clockIdentityB[byteIndex]) {
      comparisonResult = -1;
      break;
    } else if(clockIdentityA[byteIndex] > clockIdentityB[byteIndex]) {
      comparisonResult = 1;
      break;
    }
  }
  return(comparisonResult);
}

static unsigned char src_mac_addr[6];
static unsigned char dest_mac_addr[6] = PTP_DEFAULT_DEST_ADDR;
static unsigned char non_legacy_dest_mac_addr[6] = PTP_8021AS_DEST_ADDR;
static unsigned char legacy_dest_mac_addr[6] = PTP_8021AS_LEGACY_ADDR;



static void set_ptp_ethernet_hdr(unsigned char *buf,vars_uchar8 port_no)
{
  int i = 0;
  ethernet_hdr_t *hdr = (ethernet_hdr_t *) buf;

  vtss_printf("set_ptp_ethernet_hdr on port %d\r\n",port_no);

  /* CODE  REVIEW MODIFICATION*/
  mac_copy(src_mac_addr,&GPTP->ports[port_no].port_properties.port_macaddr.macaddr);

  for (i=0;i<6;i++)  {
    hdr->src_addr[i] = src_mac_addr[i];
    hdr->dest_addr[i] = dest_mac_addr[i];
  }
  
  hdr->ethertype[0] = (PTP_ETHERTYPE >> 8);
  hdr->ethertype[1] = (PTP_ETHERTYPE & 0xff);

  return;
}



void ptpex_os_tx_frame (vars_uchar8  port_no, vars_uchar8 xdata * frame, unsigned short len)
{
    /* insert dummy CRC & reserve bytes*/
    frame[len++] = 0x55;
    frame[len++] = 0x55;
    frame[len++] = 0x55;
    frame[len++] = 0x55;
    vtss_printf("Sending the PTP Frame on port %d with length %d",port_no,len);
    h2_send_frame(port2int(port_no), frame, len);
	vtss_printf("Successfully sent the PTP Frame on port %d with length %d",port_no,len);
}


static void send_ptp_pdelay_resp_msg(struct gptpm *ptp, vars_uchar8 port,vars_uchar8 *rxBuffer)
{
#define PDELAY_RESP_PACKET_SIZE (sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr) + sizeof(PdelayRespMessage))


  unsigned char *buf = &ptp_tx_buf[0];
  unsigned short msglen = 0;
  vars_uchar8 i = 0;
  vtss_ptp_time_t pdelayReqRxTimestamp;
   // received packet pointers.
  ComMessageHdr *pRxMesgHdr = (ComMessageHdr *) rxBuffer;   
   // transmit packet pointers.
  ComMessageHdr *pTxMesgHdr = (ComMessageHdr *) &buf[sizeof(ethernet_hdr_t)]; 
  PdelayRespMessage *pTxRespHdr = 
     (PdelayRespMessage *) &buf[sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr)];
  PdelayRespFollowUpMessage *pTxFollowUpHdr = 
     (PdelayRespFollowUpMessage *) &buf[sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr)];

   
   
   

  set_ptp_ethernet_hdr(buf,port);

  memset(pTxMesgHdr, 0, sizeof(ComMessageHdr) + sizeof(PdelayRespMessage));

  pTxMesgHdr->versionPTP = PTP_VERSION_NUMBER;

  msglen = sizeof(ComMessageHdr) + sizeof(PdelayRespMessage);

  VTSS_COMMON_UNALIGNED_PUT_2B(pTxMesgHdr->messageLength,msglen);

  get_source_port_id(ptp, port, RECEIVED_PACKET, rxBuffer, ptp->ports[port].lastPeerRequestPortId);

  pTxMesgHdr->sourcePortIdentity[0] = ptp->ports[port].port_properties.port_macaddr.macaddr[0];
  pTxMesgHdr->sourcePortIdentity[1] = ptp->ports[port].port_properties.port_macaddr.macaddr[1];  
  pTxMesgHdr->sourcePortIdentity[2] = ptp->ports[port].port_properties.port_macaddr.macaddr[2];
  pTxMesgHdr->sourcePortIdentity[3] = 0xff;
  pTxMesgHdr->sourcePortIdentity[4] = 0xfe;
  pTxMesgHdr->sourcePortIdentity[5] = ptp->ports[port].port_properties.port_macaddr.macaddr[3];
  pTxMesgHdr->sourcePortIdentity[6] = ptp->ports[port].port_properties.port_macaddr.macaddr[4];
  pTxMesgHdr->sourcePortIdentity[7] = ptp->ports[port].port_properties.port_macaddr.macaddr[5];
  //pTxMesgHdr->sourcePortIdentity[8] = 0;
  //pTxMesgHdr->sourcePortIdentity[9] = 1 ;
  VTSS_COMMON_UNALIGNED_PUT_2B(&pTxMesgHdr->sourcePortIdentity[8],ptp->ports[port].port_properties.us_portNumber);

  pTxMesgHdr->controlField = PTP_CTL_FIELD_OTHERS;
  pTxMesgHdr->logMessageInterval = PTP_LOG_MIN_PDELAY_REQ_INTERVAL;

  VTSS_COMMON_UNALIGNED_PUT_2B(pTxMesgHdr->sequenceId,UNAL_HOST2NETS(pRxMesgHdr->sequenceId));

  pTxRespHdr->requestingPortIdentity[0] = pRxMesgHdr->sourcePortIdentity[0];
  pTxRespHdr->requestingPortIdentity[0] = pRxMesgHdr->sourcePortIdentity[0];
  pTxRespHdr->requestingPortIdentity[0] = pRxMesgHdr->sourcePortIdentity[0];
  pTxRespHdr->requestingPortIdentity[0] = pRxMesgHdr->sourcePortIdentity[0];
  pTxRespHdr->requestingPortIdentity[0] = pRxMesgHdr->sourcePortIdentity[0];
  pTxRespHdr->requestingPortIdentity[0] = pRxMesgHdr->sourcePortIdentity[0];
  pTxRespHdr->requestingPortIdentity[0] = pRxMesgHdr->sourcePortIdentity[0];
  pTxRespHdr->requestingPortIdentity[0] = pRxMesgHdr->sourcePortIdentity[0];
  pTxRespHdr->requestingPortIdentity[0] = pRxMesgHdr->sourcePortIdentity[0];
  pTxRespHdr->requestingPortIdentity[0] = pRxMesgHdr->sourcePortIdentity[0];

  pTxMesgHdr->domainNumber = pRxMesgHdr->domainNumber;

  for(i = 0 ; i < 10; i++)
  pTxMesgHdr->correctionField[i] = pRxMesgHdr->correctionField[i];

  /* Send the response message */

  pTxMesgHdr->transportSpecific_messageType = 
    PTP_TRANSPORT_SPECIFIC_HDR | PTP_PDELAY_RESP_MESG;

  /* Add the Request Receipt TimeStamp */

  get_hardware_timestamp( ptp, port,RECEIVED_PACKET,rxBuffer,&pdelayReqRxTimestamp);

  put_timestamp(ptp, port,pTxRespHdr->requestReceiptTimestamp,&pdelayReqRxTimestamp);


  ptpex_os_tx_frame(port,ptp_tx_buf,(sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr) + sizeof(PdelayRespMessage))); 
                       


  /*For additional delay for follow-up, increase the nano seconds */
  pdelayReqRxTimestamp.nanoseconds++;


  pTxMesgHdr->transportSpecific_messageType = 
    PTP_TRANSPORT_SPECIFIC_HDR | PTP_PDELAY_RESP_FOLLOW_UP_MESG;

  put_timestamp(ptp, port,pTxFollowUpHdr->responseOriginTimestamp,&pdelayReqRxTimestamp);
  
  ptpex_os_tx_frame(port,ptp_tx_buf,PDELAY_RESP_PACKET_SIZE); 
                       
    

  return;
}







/* Processes a newly-received PDELAY_REQ packet for the passed instance */
static void process_rx_pdelay_req(struct gptpm *ptp, vars_ulong32 port, vars_uchar8 *rxBuffer) {

  PtpPortIdentity rxIdentity;

  ptp->ports[port].stats.rxPDelayRequestCount++;

  /* React to peer delay requests no matter what, even if we're not using the
   * peer-to-peer delay mechanism or if we're a slave or master.  Transmit
   * a peer delay response back - we will also transmit a peer delay response
   * followup once this message is on the wire. The only exception is if the
   * request came from any port on this system we should discard it.
   */
  get_source_port_id(ptp, port, RECEIVED_PACKET, rxBuffer, (vars_uchar8*)&rxIdentity);
  if (0 != compare_clock_identity(rxIdentity.clockIdentity, ptp->systemPriority.rootSystemIdentity.clockIdentity)) {
    send_ptp_pdelay_resp_msg(ptp, port, rxBuffer);
  } else {
    vars_ushort16 rxPortNumber = get_port_number(rxIdentity.portNumber);
    VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Disabling AS on ports %d and %d due to receipt of our own pdelay.\n", port+1, rxPortNumber));
    ptp->ports[port].portEnabled = FALSE;
    if ((rxPortNumber >= 1) && (rxPortNumber <= ptp->numPorts)) {
      ptp->ports[rxPortNumber-1].portEnabled = FALSE;
    }
  }
}



void transmit_pdelay_request(struct gptpm *ptp, vars_uchar8  port)
{
#define PDELAY_REQ_PACKET_SIZE (sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr) + sizeof(PdelayReqMessage))
    unsigned char *buf = (unsigned char *) &ptp_tx_buf[0];
    ComMessageHdr *pComMesgHdr = (ComMessageHdr *) &buf[sizeof(ethernet_hdr_t)]; 
    PdelayReqMessage *pTxReqHdr = (PdelayReqMessage *) &buf[sizeof(ethernet_hdr_t) + sizeof(pComMesgHdr)];

    unsigned short len = 0;

    vars_uchar8  i = 0;

    vtss_ptp_time_t currTime;

	vtss_printf("tranmist pdelay request in port %d\r\n",(int)port);

    set_ptp_ethernet_hdr(buf,port);   

    // clear the send data first.
    memset(pComMesgHdr, 0, sizeof(ComMessageHdr) + sizeof(PdelayReqMessage));

    // build up the packet as required.
    pComMesgHdr->transportSpecific_messageType = 
    PTP_TRANSPORT_SPECIFIC_HDR | PTP_PDELAY_REQ_MESG;

    pComMesgHdr->versionPTP = PTP_VERSION_NUMBER;

    len = sizeof(ComMessageHdr) +  sizeof(PdelayReqMessage);

    VTSS_COMMON_UNALIGNED_PUT_2B(pComMesgHdr->messageLength,len);

   

    pComMesgHdr->sourcePortIdentity[0] = ptp->ports[port].port_properties.port_macaddr.macaddr[0];
    pComMesgHdr->sourcePortIdentity[1] = ptp->ports[port].port_properties.port_macaddr.macaddr[1];  
    pComMesgHdr->sourcePortIdentity[2] = ptp->ports[port].port_properties.port_macaddr.macaddr[2];
    pComMesgHdr->sourcePortIdentity[3] = 0xff;
    pComMesgHdr->sourcePortIdentity[4] = 0xfe;
    pComMesgHdr->sourcePortIdentity[5] = ptp->ports[port].port_properties.port_macaddr.macaddr[3];
    pComMesgHdr->sourcePortIdentity[6] = ptp->ports[port].port_properties.port_macaddr.macaddr[4];
    pComMesgHdr->sourcePortIdentity[7] = ptp->ports[port].port_properties.port_macaddr.macaddr[5];
    //pComMesgHdr->sourcePortIdentity[8] = 0;
    //pComMesgHdr->sourcePortIdentity[9] = 1 ;

   VTSS_COMMON_UNALIGNED_PUT_2B(&pComMesgHdr->sourcePortIdentity[8],ptp->ports[port].port_properties.us_portNumber);
	

  
    for(i=0;i<8;i++) pComMesgHdr->correctionField[i] = 0;

  // increment the sequence id.

    ptp->ports[port].pdelayReqSequenceId++;

    /* Set the sequence id*/
    VTSS_COMMON_UNALIGNED_PUT_2B(pComMesgHdr->sequenceId,ptp->ports[port].pdelayReqSequenceId);

    get_rtc_time(ptp, &currTime); 

    /*Add the processing time delay for nanoseconds field */

    currTime.nanoseconds++;

    // control field for backward compatiability
    pComMesgHdr->controlField = PTP_CTL_FIELD_OTHERS;
    pComMesgHdr->logMessageInterval = 0x7F;


  // populate the orginTimestamp.
  
   put_timestamp(ptp, port,pTxReqHdr->originTimestamp,&currTime);


   // sent out the data and record the time.


   ptpex_os_tx_frame(port,ptp_tx_buf,(sizeof(ethernet_hdr_t) + sizeof(ComMessageHdr) + sizeof(PdelayReqMessage))); 

 


   ptp->ports[port].stats.txPDelayRequestCount++; 

   // sent out the data and record the actual hw transmission time:
   currTime.nanoseconds++;

   ptp->ports[i].pdelayReqTxTimestamp = currTime;

   ptp->ports[i].rcvdMDTimestampReceive = TRUE;

   /* P Delay Request State Machine */
  // MDPdelayReq_StateMachine(ptp, port);

  return;
}


static void process_rx_pdelay_resp(struct gptpm *ptp, vars_ulong32 port, vars_uchar8 *rxBuffer) {

  ptp->ports[port].stats.rxPDelayResponseCount++;

  ptp->ports[port].rcvdPdelayResp = TRUE;
  ptp->ports[port].rcvdPdelayRespPtr = rxBuffer;

  /* Set the Timestamp of local time when we received this response */

  /* Capture the hardware timestamp at which we received this packet, and hang on to 
       * it for delay and rate calculation. (Trsp4 - our local clock) */

  get_hardware_timestamp( ptp, port,RECEIVED_PACKET,rxBuffer,&ptp->ports[port].pdelayRespRxTimestamp);

  /* AVnu_PTP-5 from AVnu Combined Endpoint PICS D.0.0.1
     Cease pDelay_Req transmissions if more than one
     pDelay_Resp messages have been received for each of
     three successive pDelay_Req messages. */
  ptp->ports[port].pdelayResponses++;

  MDPdelayReq_StateMachine(ptp, port);
}


static void process_rx_pdelay_resp_fup(struct gptpm *ptp, vars_ulong32 port, vars_uchar8 *rxBuffer)
{

  ptp->ports[port].stats.rxPDelayResponseFollowupCount++;

  ptp->ports[port].rcvdPdelayRespFollowUp = TRUE;
  ptp->ports[port].rcvdPdelayRespFollowUpPtr = rxBuffer;

  MDPdelayReq_StateMachine(ptp, port);
}



static void process_rx_signaling(struct gptpm *ptp, vars_ulong32 port, vars_uchar8 *rxBuffer) {

  ptp->ports[port].rcvdSignalingPtr = rxBuffer;
  ptp->ports[port].rcvdSignalingMsg1 = TRUE;

  LinkDelaySyncIntervalSetting_StateMachine(ptp, port);
}


void vtss_gptp_get_new_master(vtss_gptp_system_properties_t *pMasterData)
{

   pMasterData = &GPTP->new_master;

}



void vtss_gptp_receive(vtss_common_port_t from_port,
       const vtss_common_octet_t VTSS_COMMON_BUFMEM_ATTRIB *rxframe,
        vtss_common_framelen_t len)
{

    struct ethernet_hdr_t *ethernet_hdr = (ethernet_hdr_t *) rxframe;
    int has_qtag = ethernet_hdr->ethertype[1]==0x18;
    int ethernet_pkt_size = has_qtag ? 18 : 14;
    ComMessageHdr *msg =  (ComMessageHdr *) rxframe[ethernet_pkt_size];
	len = len;


    
    switch ((msg->transportSpecific_messageType & 0xf)) 
    {
        case PTP_ANNOUNCE_MESG:
        process_rx_announce(GPTP,from_port,(vars_uchar8*)msg);
        break;

        case PTP_SYNC_MESG:
        process_rx_sync(GPTP, from_port, (vars_uchar8*)msg);
        break;

        case PTP_FOLLOW_UP_MESG:
        process_rx_fup(GPTP, from_port, (vars_uchar8*)msg);  
        break;

        case PTP_DELAY_REQ_MESG:
        break;

        case PTP_PDELAY_REQ_MESG:
        process_rx_pdelay_req(GPTP,from_port, (vars_uchar8*)msg);
        break;

        case PTP_PDELAY_RESP_MESG:
        process_rx_pdelay_resp(GPTP, from_port, (vars_uchar8*)msg);
        break;

        case PTP_PDELAY_RESP_FOLLOW_UP_MESG:
        process_rx_pdelay_resp_fup(GPTP, from_port, (vars_uchar8*)msg);
        break;

        case PTP_SIGNALING_MESG:
        process_rx_signaling(GPTP,from_port,(vars_uchar8*)msg);
    }


}


#endif /* VTSS_GPTP_NOT_WANTED */
