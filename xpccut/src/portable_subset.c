/**
 * \file          portable_subset.c
 * \library       xpccut
 * \author        Chris Ahlstrom
 * \date          2010-03-07
 * \update        2015-10-10
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides some miscellaneous portable functions necessary to simplify
 *    the writing of the unit-test library modules.
 *
 *    These functions are mostly modified or simplified versions of the XPC
 *    library's "portable" module's functions, provided so that the XPC C
 *    Unit Test library doesn't have to depend on that library, and can
 *    stand alone.
 *
 *    For that reason, and because these functions are for logging
 *    human-readable output, unit-tests have not been written for the
 *    functions in this module.
 *
 *    Note that no internationalization support is supplied in this module.
 *    Not sure if that is truly a defect, though -- most of it is sprintf()
 *    formatting and function names.
 */

#include <xpc/portable_subset.h>       /* functions, macros, and headers      */

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* fprintf() and friends               */
#endif

#if XPC_HAVE_LIMITS_H
#include <limits.h>                    /* declares ULONG_MAX, INT_MAX, etc.   */
#endif

#if XPC_HAVE_LIBINTL_H
#include <libintl.h>                   /* declares gettext()                  */
#endif

#if XPC_HAVE_LOCALE_H
#include <locale.h>                    /* declares setlocale()                */
#endif

/**
 * \doxygen
 *    This module has a function we would like to be documented, even if not
 *    enabled in normal compilation.
 */

#ifdef DOXYGEN
#define USE_GETTEXT                    /* make definitions visible            */
#endif

/**
 *    Provides for visibility of print messages.
 *
 *    When set to 'false', no messages are shown.  This setting is useful in
 *    unit-testing where all you want is a result value, and don't want to
 *    see \e any output.
 */

static cbool_t gs_Allow_Output = true;

/**
 *    Sets the static gs_Allow_Output flag to 'false'.
 *
 * \unittests
 *    -  unit_unit_test_03_05() [actually tests unit_test_options_parse()]
 */

void
xpccut_silence_printing (void)
{
   gs_Allow_Output = false;
}

/**
 *    Sets the static gs_Allow_Output flag to 'true'.
 *
 * \unittests
 *    -  unit_unit_test_03_05() [actually tests unit_test_options_parse()]
 */

void
xpccut_allow_printing (void)
{
   gs_Allow_Output = true;
}

/**
 *    Returns the negative of the gs_Allow_Output flag.
 *
 *    This function exposes this value to callers who may want to tailor
 *    their own output based on command-line options.
 *
 * \unittests
 *    -  unit_unit_test_03_05() [actually tests unit_test_options_parse()]
 */

cbool_t
xpccut_is_silent (void)
{
   return ! gs_Allow_Output;
}

/**
 *    Provides for writing an error message to stderr.
 *
 *    This function is similar to the xpc_errprint() function, but its
 *    behavior is not alterable from the command-line.  Nor does it attempt
 *    to support all the features of that function.  Obviously, a unit-test
 *    framework just needs to emit enough information to help the developer.
 *
 * \unittests
 *    -  None.
 */

void
xpccut_errprint
(
   const char * message    /**< The error message to be displayed to stderr.  */
)
{
   if (gs_Allow_Output)
   {
      if (cut_is_nullptr(message))
         fprintf(stderr, "? xpccut_errprint(): %s\n", _("null pointer"));
      else
         fprintf(stderr, "? %s\n", message);    /* one uniform format   */
   }
}

/**
 *    Provides for writing an error message to stderr, with an additional
 *    tag for labelling the message.
 *
 *    The tag appears first in the message, followed by a colon.
 *
 *    This function is similar to the xpc_errprint_ex() function, though
 *    severely crippled.
 *
 * \unittests
 *    -  None.
 */

void
xpccut_errprint_ex
(
   const char * message,   /**< The main part of the message to be printed.   */
   const char * extra      /**< The tag for the message to be printed.        */
)
{
   if (gs_Allow_Output)
   {
      if (cut_not_nullptr_2(message, extra))
         fprintf(stderr, "? %s: %s\n", extra, message);
      else
         fprintf(stderr, "? xpccut_errprint_ex(): %s\n", _("null pointer(s)"));
   }
}

/**
 *    Provides for writing an error message to stderr, with an additional
 *    tag for labelling the message, plus one more argument that is usually
 *    a function name.
 *
 *    The tag appears first in the message, followed by a colon.
 *
 *    This function is similar to the xpc_errprint_ex() function, though
 *    severely crippled.
 *
 * \unittests
 *    -  None.
 */

void
xpccut_errprint_3
(
   const char * message,   /**< The main part of the message to be printed.   */
   const char * funcname,  /**< The name of the failing function.             */
   const char * extra      /**< The tag for the message to be printed.        */
)
{
   if (gs_Allow_Output)
   {
      if (cut_not_nullptr_3(message, funcname, extra))
         fprintf(stderr, "? %s: %s, %s\n", extra, message, funcname);
      else
         fprintf(stderr, "? xpccut_errprint_ex(): %s\n", _("null pointer(s)"));
   }
}

/**
 *    Provides for writing an informational message to stdout.
 *
 *    This function is similar to the xpc_infoprint() function, but its
 *    behavior is not alterable from the command-line.  Nor does it attempt
 *    to support all the features of that function.  Obviously, a unit-test
 *    framework just needs to emit enough information to help the developer.
 *
 *    Output is made only if gs_Allow_Output is true.
 *
 * \unittests
 *    -  None.
 */

void
xpccut_infoprint
(
   const char * message    /**< The message to be printed.                    */
)
{
   if (gs_Allow_Output)
   {
      if (cut_not_nullptr(message))
         fprintf(stdout, "  %s\n", message);
      else
         fprintf(stderr, "? xpccut_infoprint(): %s\n", _("null pointer(s)"));
   }
}

/**
 *    Provides for writing an information message to stdout, with an
 *    additional tag for labelling the message.
 *
 *    This function is similar to the xpc_infoprint_ex() function, though
 *    severely crippled.  For example, no color :-(.
 *
 *    Output is made only if gs_Allow_Output is true.
 *
 * \unittests
 *    -  None.
 */

void
xpccut_infoprint_ex
(
   const char * message,   /**< The main part of the message to be printed.   */
   const char * extra      /**< The tag for the message to be printed.        */
)
{
   if (gs_Allow_Output)
   {
      if (cut_not_nullptr_2(message, extra))
         fprintf(stdout, "  %s: %s\n", extra, message);
      else
         fprintf(stderr, "? xpccut_infoprint_ex(): %s\n", _("null pointer(s)"));
   }
}

/**
 *    Combines the cut_is_nullptr() macro and the xpccut_errprint() function.
 *
 *    This function is provided for convenience.
 *
 * \return
 *    Returns 'true' if the \a pointer parameter is valid.
 *
 * \unittests
 *    -  None.
 */

cbool_t
xpccut_nullptr
(
   const char * pointer,   /**< The pointer to be checked for validity.       */
   const char * message,   /**< The main part of the message to be printed.   */
   const char * extra      /**< The tag for the message to be printed.        */
)
{
   cbool_t result = true;
   if (cut_is_nullptr(pointer))
   {
      result = false;
      xpccut_errprint_ex(message, extra);
   }
   return result;
}

/**
 *    Combines the cut_is_thisptr() macro and the xpccut_errprint() function.
 *
 *    This function is provided for convenience.  It is used in the
 *    xpccut_thisptr() macro, which is much easier to use if you want the
 *    stock message and __func__ parameter to be passed in.  Normally, you
 *    don't need to call xpccut_thisptr_impl() directly.
 *
 * \param pointer
 *    The pointer to be checked for validity.  The pointer is valid if it is
 *    not null.  If the caller defined XPC_NO_THISPTR before including the
 *    macros_subset.h header file, then the cut_not_thisptr() macro always
 *    returns 'false', so that xpccut_thisptr_impl() function always returns
 *    'true', avoiding the check for the null pointer (very useful in C++
 *    code).
 *
 * \param extra
 *    The tag for the message to be printed.  Normally, this should be the
 *    __func__ macro.
 *
 * \return
 *    Returns 'true' if the \a pointer parameter is valid, or if
 *    XPC_NO_THISPTR is defined.
 *
 * \unittests
 *    -  unit_unit_test_07_03() [a test of the xpccut_thisptr() macro]
 */

cbool_t
xpccut_thisptr_impl
(
   const void * pointer,   /**< The pointer to be checked for validity.       */
   const char * extra      /**< The tag for the message to be printed.        */
)
{
   cbool_t result = true;
   if (cut_not_thisptr(pointer))
   {
      result = false;
      xpccut_errprint_ex(_("null 'this' pointer"), extra);
   }
   else
   {
      if (cut_is_nullptr(pointer))
         xpccut_infoprint(_("XPC_NO_THISPTR in force"));
   }
   return result;
}

/**
 *    Provides a way to safely copy the strings used in the
 *    unit_test_status_t module.
 *
 *    All of these strings are the same length, XPCCUT_STRLEN, which makes
 *    the task even easier.
 *
 * \unittests
 *    -  None.
 */

void
xpccut_stringcopy
(
   char * destination,     /**< The destination for the string-copy.          */
   const char * source     /**< The source string for the string-copy.        */
)
{
   if (cut_not_nullptr_2(destination, source))
   {
      strncpy(destination, source, XPCCUT_STRLEN);
      destination[XPCCUT_STRLEN-1] = 0;
   }
}

/**
 *    This function implements the basic functionality of waiting for a
 *    keystroke/Enter combination.  The character (if any) is returned.
 *    An empty or null response is not accepted.  However, if a
 *    carriage-return or line feed is all that is found, a null is returned.
 *
 * \warning
 *    This function is easy to break by tinkering with the code.
 *
 * \return
 *    Returns the first valid character entered by the user.  If the user
 *    simply hit Enter, a null character is returned.
 *
 * \unittests
 *    -  None.  Like most of the routines in this module, this is a helper
 *       function that gets indirect testing in the unit_test_status_t
 *       unit-tests.
 *
 */

char
xpccut_get_response (void)
{
   char result = '\x00';
   int r;
   do
   {
      r = getchar();
      if ((result == '\x00') && (r > ' '))
         result = (char) r;

   } while (r > ' ');
   return result;
}

/**
 *    Sleeps for roughly the given number of milliseconds.
 *
 *    It is meant for sleeping longer than 10 msec.
 *
 * \posix
 *    The select() function is used, as described in the xpccut_us_sleep()
 *    function.
 *
 * \win32
 *    The Sleep() function is used.  Thus, a parameter of \a ms == 0 can be
 *    used to yield the processor.  But the POSIX version does not support
 *    this concept, so don't rely on it, even though we allow it.
 *
 * \unittests
 *    -  unit_unit_test_02_05().  The testing of this function is indirect
 *       and a little coarse due to operating-system timing variability.
 */

void
xpccut_ms_sleep
(
   unsigned long ms     /**< The approximate number of milliseconds to sleep. */
)
{
#ifdef WIN32                              /* Win32                            */

   Sleep((DWORD) ms);

#else                                     /* POSIX                            */

   struct timeval tv;
   struct timeval * tvptr = &tv;
   tv.tv_usec = (ms % 1000) * 1000;
   tv.tv_sec = ms / 1000;
   (void) select(0, nullptr, nullptr, nullptr, tvptr);

#endif                                    /* POSIX/Win32                      */
}

/**
 *    Obtains the current "time-of-day" in microseconds.
 *
 *    If you really want to understand this function, see
 *    xpccut_get_microseconds() in the XPC library project, libxpc.
 *
 * \return
 *    Returns true if the function was able to obtain the time.
 *
 * \unittests
 *    -  None.  Like most of the routines in this module, this is a helper
 *       function that may get indirect testing in the unit_test_status_t
 *       unit-tests.
 */

#ifdef WIN32                                             /* Win32             */

cbool_t
xpccut_get_microseconds
(
   struct timeval * c      /**< The structure in which to insert the time     */
)
{
   cbool_t result = false;
   if (cut_not_nullptr(c))
   {
      static cbool_t gm_is_ready = false;

      /*
       * okay with VS?
       *
       *    static LARGE_INTEGER gm_frequency   = { 0 }; // tick frequency    //
       *    static LARGE_INTEGER gm_t0  = { 0 };         // starting ticks    //
       *
       * needed for mingw?
       */

      /*
       * According to http://publib.boulder.ibm.com/infocenter/lnxpcomp/v8v101/
       *    index.jsp?topic=%2Fcom.ibm.xlcpp8l.doc%2Flanguage%2Fref%2Fstrin.htm,
       * only the first part of a union can be initialized.
       */

      static LARGE_INTEGER gm_frequency =                /* tick frequency    */
      {
         .u = { 0, 0 }
      };
      static LARGE_INTEGER gm_t0  =                      /* starting ticks    */
      {
         .u = { 0, 0 }
      };
      if (! gm_is_ready)                                 /* first call?       */
      {
         BOOL okay = QueryPerformanceFrequency(&gm_frequency);
         if (okay)
         {

#if USE_QPC_BASE                                         /* not recommended   */

            /*
             * Not sure we really need to use a baseline time.  It could
             * screw up any test that checks to see if the seconds field is
             * non-zero.  So, for now, USE_QPC_BASE should not be defined.
             */

            okay = QueryPerformanceCounter(&gm_t0);      /* ticks now         */
            if (okay)
               gm_is_ready = true;                       /* calibrated!       */
#else
            gm_is_ready = true;                          /* calibrated!       */
#endif
         }
         else
         {
            gm_frequency.QuadPart = 0;
            xpccut_errprint_3_func
            (
               _("calibration failed"), "QueryPerformanceCounter()"
            );
         }
      }
      if (gm_is_ready)                                   /* calibrated?       */
      {
         LARGE_INTEGER t;
         BOOL okay = QueryPerformanceCounter(&t);        /* time in ticks     */
         if (okay)
         {
            /**
             * -  ticks:      Difference between the first call and current call.
             * -  seconds:    The number of seconds, truncated.
             * -  secticks:   The truncated seconds, in tick units.
             * -  us:         Starts as the ticks with the secticks deducted.
             *
             * The \e us value is the number of ticks left over after
             * deducting seconds.  It gets scaled to seconds, in units of
             * ticks, by multiplying this difference by 1000000.  Then we
             * convert it to a seconds value by dividing by the frequency, but
             * this seconds value is actually microseconds.
             */

            unsigned __int64 ticks = t.QuadPart - gm_t0.QuadPart;
            unsigned __int64 seconds = ticks / gm_frequency.QuadPart;
            unsigned __int64 secticks = seconds * gm_frequency.QuadPart;
            unsigned __int64 us = (ticks - secticks) * 1000000;
            us /= gm_frequency.QuadPart;
            c->tv_sec = (long) seconds;
            c->tv_usec = (long) us;
         }
         else
         {
            c->tv_sec = (long) 0;
            c->tv_usec = (long) 0;
            xpccut_errprint_3_func(_("failed"), "QueryPerformanceCounter()");
         }
      }
   }
   else
      xpccut_errprint_func("null pointer");

   return result;
}

#else                                                    /* POSIX             */

cbool_t
xpccut_get_microseconds
(
   struct timeval * c      /**< The structure in which to insert the time     */
)
{
   cbool_t result = false;
   if (cut_not_nullptr(c))
   {
      int errcode = gettimeofday(c, nullptr);            /* fill in c         */
      if (is_posix_error(errcode))
      {
         c->tv_sec = c->tv_usec = 0;
         xpccut_errprint_func(_("failed"));
      }
      else
         result = true;
   }
   else
      xpccut_errprint_func(_("null pointer"));

   return result;
}

#endif                                                   /* POSIX vs Win32    */

/**
 *    Normalizes a timeval structure, if necessary.
 *
 *    The \a tv_usec value must range only from 0 to 999999.  As noted
 *    below, it is a signed value.  It can then be less than zero, or
 *    greater than 999999.
 *
 *    If it is less than 0, then it should be increased until it is not, and
 *    the corresponding number of seconds should be subtracted from the \a
 *    tv_sec field.
 *
 *    If it is greater than 999999, then it should be reduced until it is,
 *    and the corresponding number of seconds be added to the \a tv_sec
 *    field.
 *
 * \return
 *    Returns 'true' if an adjustment had to be made.  Otherwise, 'false' is
 *    returned.  This function also returns 'false' if the pointer was
 *    invalid (null).
 *
 * \unittests
 *    -  None.
 */

cbool_t
xpccut_time_fix
(
   struct timeval * c1     /**< Structure for the time-stamp to be adjusted.  */
)
{
   cbool_t result = false;
   if (cut_is_nullptr(c1))
      xpccut_errprint_func(_("null pointer"));
   else
   {
      if (c1->tv_usec > 999999)
      {
         c1->tv_sec += c1->tv_usec / 1000000;
         c1->tv_usec %= 1000000;
         result = true;
         xpccut_infoprint("deducted from tv_usec");
      }
      else if (c1->tv_usec < 0)
      {
         int delta_sec = c1->tv_usec / 1000000 + 1;
         c1->tv_sec -= delta_sec;
         c1->tv_usec += delta_sec * 1000000;
         result = true;
         xpccut_infoprint("added to tv_usec");
      }
   }
   return result;
}

/**
 *    Provides the time difference between two timeval structures in
 *    milliseconds.
 *
 *    The times are fixed, if bad, but an unnormalized timeval structure
 *    should never happen.
 *
 *    This function is meant only for unit testing, for intervals no greater
 *    than a few minutes.  It doesn't attempt to cover the case where c2 is
 *    earlier than c1.
 *
 * \change ca 2015-10-10
 *    Found some major errors in this function.  What a dope!
 *
 * \return
 *    Returns the difference between the two times ("c2 - c1") in
 *    milliseconds.  If the time difference was detected to be too long to
 *    fit in the return value, then 0 is returned.  The caller should avoid
 *    using the result of this function if it is 0..
 *
 * \unittests
 *    -  None.
 */

unsigned long
xpccut_time_difference_ms
(
   struct timeval c1,      /**< Structure for the earlier time-stamp.         */
   struct timeval c2       /**< Structure for the later time-stamp.           */
)
{
   unsigned long result;
   (void) xpccut_time_fix(&c1);
   (void) xpccut_time_fix(&c2);
   int secdifference = c2.tv_sec - c1.tv_sec;
   if (secdifference < 0)
   {
      xpccut_errprint("backwards seconds in xpccut_time_difference_ms");
      result = 0;
   }
   else
   {
      int usdifference = c2.tv_usec - c1.tv_usec;
      if (usdifference < 0)
      {
         secdifference -= 1;
         usdifference += 1000000;
      }
      result = (unsigned long) secdifference;
      result *= 1000000L;
      result += (unsigned long) usdifference;
      result /= 1000;                                 /* convert usec to msec */
   }
   return result;
}

/**
 *    Used by xpccut_stopwatch_start() and other stopwatch functions.
 */

static cbool_t g_xpccut_stopwatch_started = false;

/**
 *    Used by xpccut_stopwatch_start() and other stopwatch functions.
 */

static struct timeval g_xpccut_stopwatch_start_time;

/**
 *    Used by xpccut_stopwatch_start() and other stopwatch functions.
 */

static struct timeval g_xpccut_stopwatch_lap_time;

/**
 *    Starts a single-threaded stopwatch with microsecond resolution.
 *    This function first gets that start time using xpccut_get_microseconds().
 *    Then it sets the lap time to this time.  Finally, a "stopwatch
 *    started" flag is set to true.
 *
 *    The xpccut_stopwatch_start(), xpccut_stopwatch_duration(), and
 *    xpccut_stopwatch_lap() functions provide a global timer so that the call
 *    doesn't even have to know about struct timeval.  The "duration"
 *    function gets the current time and returns the difference between it
 *    and the start time, in seconds.  The "lap" function returns the time
 *    between the previous lap time and now.
 *
 *    Please note that these functions are not thread-safe, since the start
 *    and lap times are stored in a static variable.  Access them only with
 *    a single thread.  They are meant mostly for testing and for the
 *    evaluation of algorithms.
 *
 *    We could provide an optional structure parameter to make this function
 *    thread-safe, if the need becomes evident.
 *
 * \unittests
 *    -  None.
 */

void
xpccut_stopwatch_start (void)
{
   xpccut_get_microseconds(&g_xpccut_stopwatch_start_time);
   g_xpccut_stopwatch_lap_time = g_xpccut_stopwatch_start_time;
   g_xpccut_stopwatch_started = true;
}

/**
 *    Provides the total time elapsed since the start time.
 *
 *    This function gets the current time with xpccut_get_microseconds(),
 *    and then it returns the difference between the end time (current time)
 *    and the start time using the xpccut_time_difference_ms() function.
 *
 * \return
 *    The duration since the start time of the stopwatch is returned, in
 *    units of milliseconds.
 *
 * \unittests
 *    -  None.
 */

double
xpccut_stopwatch_duration (void)
{
   double result = 0.0;
   if (g_xpccut_stopwatch_started)
   {
      struct timeval end_time;
      xpccut_get_microseconds(&end_time);
      result = xpccut_time_difference_ms
      (
         g_xpccut_stopwatch_start_time, end_time
      );
   }
   return result;
}

/**
 *    Returns the time difference between the current call to
 *    xpccut_stopwatch_lap() and the previous call to it.
 *
 *    This function gets the current time with xpccut_get_microseconds(),
 *    and then it returns the difference between the end time (current time)
 *    and the last lap time using the xpccut_time_difference() function.
 *
 *    Then the old lap time is updated with the current time in anticipation
 *    of the next call to xpccut_stopwatch_lap().  To increase the accuracy
 *    a little bit, the new lap time is obtained by another call to
 *    xpccut_get_microseconds().
 *
 * \return
 *    Returns the difference in microseconds between the current call to
 *    xpccut_stopwatch_lap(), and the last call [or to
 *    xpccut_stopwatch_start() if there was no previous call to
 *    xpccut_stopwatch_lap().
 *
 * \unittests
 *    -  None.
 */

double
xpccut_stopwatch_lap (void)
{
   double result = 0.0;
   if (g_xpccut_stopwatch_started)
   {
      struct timeval end_time;
      xpccut_get_microseconds(&end_time);
      result = xpccut_time_difference_ms(g_xpccut_stopwatch_lap_time, end_time);
      xpccut_get_microseconds(&g_xpccut_stopwatch_lap_time);
   }
   return result;
}

/**
 *    Sets up the gettext() support in a very rudimentary way.
 *
 *    There's a lot of checks and things that could be done to make this
 *    setup robust, but they are not done here.  [See the
 *    init_gettext_support() function in the gettext_support.c module in the
 *    xpc library if you want to see some serious error-checking.
 *
 *    This function calls setlocale(), bindtextdomain(), and textdomain().
 *
 * \win32
 *    Not yet available in Windows in the XPC CUT library.
 *
 * \usage
 *    -# Make sure that LC_ALL or LANG are defined in your environment.
 *
 * \return
 *    Returns 'true' if the setting of the locale succeeded.
 *
 * \unittests
 *    -  None.
 */

#ifdef USE_GETTEXT         /* turned off by "./configure --disable-gettext"   */

cbool_t
xpccut_text_domain (void)
{
   cbool_t result = false;

   /*
    * ca 2010-07-17
    * The second argument should be NULL or a valid locale name.
    *
    *    char * locale_set = setlocale(LC_ALL, "");
    */

   char * locale_set = setlocale(LC_ALL, NULL);
   if (cut_not_nullptr(locale_set))
   {
      xpccut_infoprint_ex(locale_set, _("LC_ALL locale setting"));
      locale_set = bindtextdomain("xpccut", nullptr);
      if (cut_not_nullptr(locale_set))
      {
         /*
          * We want to set the message domain, but we're EXPERIMENTING
          * to see what can cause valgrind to complain.  We believe
          * that leaks occur if the "xpccut" messages cannot be loaded.
          *
          * locale_set = textdomain(NULL);
          */

         locale_set = textdomain("xpccut");
         if (cut_not_nullptr(locale_set))
         {
            xpccut_infoprint_ex(locale_set, _("current text domain"));
            result = true;
         }
         else
            xpccut_errprint_3_func(_("failed"), "textdomain()");
      }
      else
         xpccut_errprint_3_func(_("failed"), "bindtextdomain()");
   }
   else
      xpccut_errprint_3_func(_("invalid locale"), "LC_ALL");

   return result;
}

#else

cbool_t
xpccut_text_domain (void)
{
   xpccut_errprint_func
   (
      "gettext() not available; turn on USE_GETTEXT and rebuild"
   );
   return false;
}

#endif      // USE_GETTEXT

/*
 * portable_subset.c
 *
 * vim: ts=3 sw=3 et ft=c
 */
