#ifndef XPCCUT_FUZZ_H
#define XPCCUT_FUZZ_H

/**
 * \file          fuzz.h
 * \library       xpccut
 * \author        Chris Ahlstrom
 * \date          2010-03-06
 * \updates       2015-10-06
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides a function to generate random strings for testing.
 */

#include <xpc/macros_subset.h>         /* support for special XPC features    */

#if XPC_HAVE_STDLIB_H
#include <stdlib.h>                    /* srand() and rand()                  */
#endif

/**
 *    Provides a way to indicate options for the xpccut_fuzz() function.
 *
 *    This value is a bit-mask.  Use zero if you don't want any of the
 *    options to be in force.  Zero is a suitable default value, too.
 *
 *    If the XPCCUT_FF_ADD_CARRIAGE_RETURNS and XPCCUT_FF_ADD_LINEFEEDS bits
 *    are both set, then CR/LF sequences will be emitted.
 *
 * \var XPCCUT_FF_DEFAULT
 *    Indicates that no options are in force.  Pass this "flag" value in by
 *    default.
 *
 * \var XPCCUT_FF_INCLUDE_NULLS
 *    Add nulls at random throughout.  <b> NOT YET IMPLEMENTED. </b>
 *
 * \var XPCCUT_FF_EXCLUDE_FINAL_NULL
 *    Do not terminate string with a null.  <b> NOT YET IMPLEMENTED. </b>
 *
 * \var XPCCUT_FF_ADD_CARRIAGE_RETURNS
 *    Add carriage returns at random.  <b> NOT YET IMPLEMENTED. </b>
 *
 * \var XPCCUT_FF_ADD_LINEFEEDS
 *    Add linefeeds at random.  <b> NOT YET IMPLEMENTED. </b>
 *
 * \var XPCCUT_FF_LETTERS_ONLY
 *    Use only letters in the character set to be randomized.
 *
 * \var XPCCUT_FF_NUMBERS_ONLY
 *    Use only numbers (and some number-related punctuation) in the
 *    character set to be randomized.
 *
 * \var XPCCUT_FF_RANDOM_SIZE
 *    Provides a way to randomize the number of bytes in a string generated
 *    by xpccut_fuzz().  The size goes from 0 to the \a number_of_bytes
 *    parameter of the xpc_fuzz() function.
 *
 * \var XPCCUT_FF_DUMP_CHARSET
 *     Do not generate a fuzz string.  Instead, copy the final character set
 *     to the destination.  This is a test option for the xpccut_fuzz()
 *     function to make it easy to see if the character set to randomize is
 *     what one truly desired.  It is best to make sure that the destination
 *     buffer and the \a dlength parameter for this buffer is 256 bytes.
 */

typedef enum
{
   XPCCUT_FF_DEFAULT                = 0x0000,
   XPCCUT_FF_INCLUDE_NULLS          = 0x0001,
   XPCCUT_FF_EXCLUDE_FINAL_NULL     = 0x0002,
   XPCCUT_FF_ADD_CARRIAGE_RETURNS   = 0x0004,
   XPCCUT_FF_ADD_LINEFEEDS          = 0x0008,
   XPCCUT_FF_LETTERS_ONLY           = 0x0010,
   XPCCUT_FF_NUMBERS_ONLY           = 0x0020,
   XPCCUT_FF_RANDOM_SIZE            = 0x0040,
   XPCCUT_FF_DUMP_CHARSET           = 0x8000

} xpccut_fuzz_flags_t;

/**
 *    Provides a way to control the seeding of the random sequence.
 *
 *    Any other values are treated as the seed to pass to srand().
 *
 *    Normally, the easiest way to deal with seeding is to call
 *    xpccut_set_seed() first, then pass XPCCUT_SEED_SKIP to all calls to
 *    xpccut_fuzz().
 *
 * \note
 *    Seeds of 0 and 1 are thus not available, although, in GNU, the default
 *    seed value is 1.
 */

typedef enum
{
   XPCCUT_SEED_RANDOMIZE,     /**< Generate and use a random seed.            */
   XPCCUT_SEED_SKIP,          /**< Do not seed the random generator.          */

   /*
    * All other values in between are treat as a seed.
    */

   XPCCUT_SEED_ERROR = 0      /**< Returned as an error by xpccut_fuzz().     */

} xpccut_seed_options_t;

/*
 * Portable C functions.  Global and portable functions for some common, basic
 * tasks.
 */

EXTERN_C_DEC

extern unsigned int xpccut_set_seed (unsigned int seed);
extern unsigned int xpccut_fuzz
(
   char * destination,
   int dlength,
   int number_of_bytes,
   unsigned int seed,
   xpccut_fuzz_flags_t flags,
   const char * allowed_chars,
   const char * excluded_chars,
   const char * prologue,
   const char * epilogue
);

extern void xpccut_dump_string (const char * source, int source_length);
extern unsigned int xpccut_srandom (unsigned int seed);
extern unsigned int xpccut_random (void);
extern unsigned int xpccut_rand (int rangemax);
extern int xpccut_garbled_string
(
   char * source,
   int length
);

EXTERN_C_END

#endif                           /* XPCCUT_FUZZ_H                             */

/*
 * fuzz.h
 *
 * vim: ts=3 sw=3 et ft=c
 */
