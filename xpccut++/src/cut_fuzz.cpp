/**
 * \file          cut_fuzz.cpp
 * \library       libxpccut++
 * \author        Chris Ahlstrom
 * \date          2010-03-21
 * \updates       2015-10-06
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides the implementations for methods of the xpc::Fuzz
 *    class, plus some additional "fuzzy" methods.
 *    Also see the cut_fuzz.h module and the xpc::Fuzz
 *    class.
 */

#include <cstdio>                      /* std::sprintf()                      */
#include <xpc/portable_subset.h>       /* C::xpccut_infoprintf()              */
#include <xpc/cut_fuzz.hpp>            /* class xpc::Fuzz                     */

namespace xpc
{

/**
 * \param tag
 *    Provides the tag to identify the part of the sequence information
 *    being logged.
 *
 * \param doshow
 *    Provides an indicator if the output is to be seen, or not.
 */

RandomNumber::RandomNumber (unsigned int newseed)
 :
    m_original_seed  (0),
    m_last_seed      (0)
{
   if (newseed == XPCCUT_SEED_RANDOMIZE)
   {
      // not yet supported
   }
   else if (newseed != XPCCUT_SEED_SKIP)
   {
      m_original_seed = m_last_seed = xpccut_srandom(newseed);
   }
}

/**
 *    Provides a static sequence number.
 */

int Fuzz::sm_sequence_value = 0;

/**
 * \param tag
 *    Provides the tag to identify the part of the sequence information
 *    being logged.
 *
 * \param doshow
 *    Provides an indicator if the output is to be seen, or not.
 */

Fuzz::Fuzz (const char * tag, bool doshow)
 :
   m_tag_name           (cut_not_nullptr(tag) ? tag : "???"),
   m_do_show            (cut_not_nullptr(tag) ? doshow : true),
   m_original_string    (),
   m_randomized_string  ()
{
   if (m_do_show)
   {
      char temp[64];
      std::sprintf(temp, "%4d:  IN: %s", sm_sequence_value, m_tag_name.c_str());
      xpccut_infoprint(temp);
      sm_sequence_value++;
   }
}

/**
 * \unittests
 *    TBD
 */

Fuzz::~Fuzz ()
{
   if (m_do_show)
   {
      char temp[64];
      std::sprintf(temp, "%4d: OUT: %s", sm_sequence_value, m_tag_name.c_str());
      xpccut_infoprint(temp);
      sm_sequence_value++;
   }
}

/**
 *    Write a string as it is to a simple file.  Absolutely no adornments,
 *    not even a newline.
 *
 *    This function is used to dump data for analysis in unit-tests.
 *
 * @param filename
 *    The name of the file to which to write the data.
 *
 * @param data
 *    The data string to write to the file.
 */

static void
write_to_file
(
   const std::string & filename,
   const std::string & data
)
{
   FILE * f = std::fopen(filename.c_str(), "wa");
   if (f)                                 // not_nullptr(f))
   {
      std::fputs(data.c_str(), f);
      (void) fclose(f);
   }
}

/**
 *    Compares two strings line-by-line, allowing for minor differences.
 *
 *    The strings can differ in as many characters as specified.  If two
 *    characters that are not equal are both digits, they are considered to
 *    match.
 *
 *    This function allows multi-line strings (e.g.  XML strings) to be
 *    tested, even if the boost version number is not the same.
 *
 * @param actual
 *    Provides the first of the strings to be compared.  This is the string
 *    that is to be evaluated for correctness.
 *
 * @param target
 *    Provides the second of the strings to be compared.  This is the string
 *    that the actual strig should match.
 *
 * @param acceptable_threshold
 *    Indicates the number of minor differences that are acceptable.  The
 *    default value is 1 (we figure that's why you called this function in
 *    the first place, otherwise we'd set it to 0).
 *
 * @param ignorestrings
 *    A colon-delimited string. If a substring of this string is found in a
 *    line, that line will be ignored in the comparison.  By default, this
 *    string is empty.
 *
 * @param dumpstrings
 *    If 'true', then the strings are dumped to two files,
 *    "actual_result.tmp" and "target_result.tmp".  By default, this
 *    parameter is 'false'.  This parameter is set to 'true' automatically
 *    if the strings fail to match.
 *
 * @return
 *    Returns 'true' if the number of detected character differences is less
 *    than or equal to \a acceptable_threshold.
 */

bool
fuzzy_line_compare
(
   const std::string & actual,
   const std::string & target,
   int acceptable_threshold,
   const std::string & ignorestrings,
   bool dumpstrings
)
{
   std::size_t sz1 = actual.size();
   std::size_t sz2 = target.size();
   bool result = abs(int(sz1 - sz2)) <= acceptable_threshold;
   bool ignore = ! ignorestrings.empty();
   if (result)
   {
      int line_count = 0;
      int bad_count = 0;
      bool skipline = false;
      std::size_t beg1 = 0;
      std::size_t beg2 = 0;
      std::size_t end1 = actual.find_first_of('\n', 0);     // get first line
      std::size_t end2 = target.find_first_of('\n', 0);     // ditto
      std::string actualline = actual.substr(0, end1);
      std::string targetline = /*actual*/ target.substr(0, end2);
      while (actualline.size() > 0)
      {
         line_count++;
         bad_count = 0;
         skipline = false;
         if (ignore)                         // look for ignore strings in line
         {
            std::string tempstr = ignorestrings;
            while (tempstr.size() > 0)
            {
               std::string ignorethis;
               std::size_t isubbeg = tempstr.find_first_of(":", 0);
               if (isubbeg == std::string::npos)
               {
                  ignorethis = tempstr;
                  tempstr.clear();           // there are no more substrings
               }
               else
               {
                  std::size_t isubend = tempstr.find_first_of(":", isubbeg+1);
                  if (isubend == std::string::npos)
                  {
                     ignorethis = tempstr.substr(1, tempstr.size()-1);
                     tempstr.clear();        // there are no more substrings
                  }
                  else
                  {
                     std::size_t sublen = isubend - isubbeg;
                     ignorethis = tempstr.substr(isubbeg+1, sublen);
                     tempstr = tempstr.substr(isubend);  // next substring
                  }
               }
               if (actualline.find(ignorethis) != std::string::npos)
               {
                  skipline = true;
                  tempstr.clear();           // ignore the rest of the line
               }
            }
         }

         /*
          * If set not to ignore the string, and if the two lines are not
          * equal, then check more carefully.
          */

         if (! skipline && (actualline != targetline))
         {
            std::size_t a_size = actualline.size();
            std::size_t t_size = targetline.size();
            for (std::size_t a = 0, t = 0; a < a_size && t < t_size; a++, t++)
            {
               bool bad = actualline[a] != targetline[t];
               if (bad)
               {
                  bad = ! isdigit(actualline[a]) && ! isdigit(targetline[t]);
                  if (bad)
                  {
                     if (++bad_count > acceptable_threshold)
                     {
                        result = false;

                        /*
                         * errprintf
                         * (
                         *    "%s: bad character at line %d, column %d",
                         *    __func__, line_count, a
                         * );
                         */

                        break;
                     }
                  }
                  else                       // skip past digits in both strings
                  {
                     while (isdigit(actualline[a]))
                        a++;

                     while (isdigit(targetline[t]))
                        t++;

                     /*
                      * Back-track one character to re-evaluate the same
                      * positions again.
                      */

                     a--;                    // compensate for for-loop ++
                     t--;                    // compensate for for-loop ++
                  }
               }
            }
         }
         beg1 = end1 + 1;
         beg2 = end2 + 1;
         if ((beg1 < actual.size()) && (beg2 < target.size()))
         {
            end1 = actual.find_first_of('\n', beg1);
            end2 = target.find_first_of('\n', beg2);
            actualline = actual.substr(beg1, end1-beg1);
            targetline = target.substr(beg2, end2-beg2);
         }
         else
         {
            actualline = "";
            targetline = "";
         }
      }
   }
   if (! result)
   {
      /*
       * errprint
       * (
       *    "Strings differ, see 'unittest_actual.tmp' and 'unittest_target.tmp'"
       * );
       */

      dumpstrings = true;
   }
   if (dumpstrings)
   {
      /*
       * Writing these two files makes it easier to generate results, verify
       * that they make sense, and them make them part of a comparison in
       * the unit-tests.
       */

      write_to_file("actual_result.tmp", actual);
      write_to_file("target_result.tmp", target);
   }
   return result;
}

}           // namespace xpc

/*
 * cut_fuzz.cpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
