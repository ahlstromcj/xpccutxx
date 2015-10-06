#if ! defined XPC_CUT_FUZZ_HPP
#define XPC_CUT_FUZZ_HPP

/**
 * \file          cut_fuzz.hpp
 * \library       libxpccut++
 * \author        Chris Ahlstrom
 * \date          2010-03-21
 * \updates       2015-10-06
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Also see the fuzz.cpp module and the xpc::Fuzz class.
 *
 *    The xpc::RandomNumber class provides an easy object for returning
 *    randomized strings.  The xpc::Fuzz class is meant to provide a way of
 *    generating fuzzed-up strings.
 */

#include <xpc/fuzz.h>                  /* external C fuzz functions           */
#include <string>                      /* std::string                         */

namespace xpc
{

/**
 *    Provides a wrapper for GNU-style random-number generation.
 *
 *    It is written as a base class, so that you can override the exact
 *    means of generating the random numbers.
 */

class RandomNumber
{

private:

   /**
    *    Stores the seed value that was used to seed the generator.
    *    The values of 0 and 1 are special, and not used as seeds.  See the
    *    XPCCUT_SEED_RANDOMIZE and XPCCUT_SEED_SKIP values.  The value of 0
    *    is used as an error code or as a indicator that seeding has not
    *    been done.
    */

   unsigned int m_original_seed;

   /**
    *    Stores the latest seed.
    */

   unsigned int m_last_seed;

public:

   RandomNumber (unsigned int newseed = 2);

   /**
    *    Seeds (or re-seeds) the random number generator.
    */

   virtual unsigned int seed (unsigned int newseed)
   {
      m_last_seed = xpccut_srandom(newseed);
      return m_last_seed;
   }

   /**
    *    Obtains the next random number in the desired range.
    *
    * \param range
    *    Provides the number representing the range, plus one.
    *
    * \return
    *    Returns a value from 0 to rangemax-1, inclusive.
    */

   virtual unsigned int generate (int rangemax = 0)
   {
      if (rangemax == 0)
         return xpccut_random();
      else
         return xpccut_rand(rangemax);
   }

   /**
    *    Returns the range of the random-number generator.
    *
    *    The lower end of the range is always 0.
    */

   virtual unsigned int maximum ()
   {
      return RAND_MAX;
   }

protected:

   /**
    * \setter m_original_seed
    */

   void original_seed (unsigned int newseed)
   {
      m_original_seed = newseed;
   }

   /**
    * \setter m_last_seed
    */

   void last_seed (unsigned int newseed)
   {
      m_last_seed = newseed;
   }

};             // RandomNumber

/**
 *
 *    Provides a wrapper for the C fuzz functions.
 *
 *    It is meant to be even easier to use.
 *
 */

class Fuzz
{

private:

   /**
    *    Provides a "global" sequence number.
    */

   static int sm_sequence_value;

   /**
    *    Provides a tag name for making the output more comprehensible.
    */

   std::string m_tag_name;

   /**
    *    Indicates if there should be verbose output.
    */

   bool m_do_show;

   /**
    *    Preserves a copy of the original string.
    */

   std::string m_original_string;

   /**
    *    The most recent randomized string.
    */

   std::string m_randomized_string;

private:

   Fuzz (const Fuzz &);
   Fuzz & operator = (const Fuzz &);

public:

   Fuzz
   (
      const char * tag        = nullptr,
      bool doshow             = false
   );

   virtual ~Fuzz ();

   /*
    * \setter gm_Sequence_Value
    *
    *    We can rely on the compiler/linker to have this value
    *    initialized to zero in a timely fashion, so this function really is
    *    not needed.
    *
    *       static void initialize () { gm_Sequence_Value = 0; }
    */

};             // Fuzz

extern bool fuzzy_line_compare
(
   const std::string & actual,
   const std::string & target,
   int acceptable_threshold            = 1,
   const std::string & ignorestrings   = "",
   bool dumpstrings                    = false
);

}              // namespace xpc

#endif         // XPC_CUT_FUZZ_HPP

/*
 * cut_fuzz.hpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
