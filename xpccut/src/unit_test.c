/**
 * \file          unit_test.c
 * \library       xpccut
 * \author        Chris Ahlstrom
 * \date          2010-03-07
 * \updates       2015-10-10
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This module provides unit test functionality for all internal unit
 *    tests.
 *
 *    The classes in this module support a pretty easy-to-use and orderly
 *    sequence of unit tests, with a lot of useful run-time options.
 *
 * \goals
 *    -# Build a test plan for unit testing (white-box testing), including
 *       decision coverage and condition coverage as appropriate.
 *    -# Build a test driver using C/C++ code and a scripting language as
 *       applicable.
 *    -# Build test cases and a unit-test application to run them.
 *    -# Build benchmarks, as applicable.
 *    -# Automate the construction of at least the unit-test framework, with
 *       functions that the developer can fill in.
 *    -# Provide semi-automatic documentation of traceability between the
 *       tests and the functions that are tested.
 *    -# Incorporate support for test-coverage profiling using GNU gcov.
 *
 * \unittests
 *    A test case is a function that exercises another function or a small
 *    set of functions.  A test case function returns true if the test is
 *    passed, and false if the test is failed.
 *
 *    Please see the marked "Test case:" sections below for complete details
 *    of each test.  Also note that the method is akin to that in the
 *    Halladay and Weibel reference shown below.
 *
 * \references
 *    -# Halladay, S., and Wiebel, M. (1993) "Object-Oriented Software
 *       Engineering:  Examples in C++", R & D Publications, Lawrence, Kansas,
 *       66046.  See Chapter 7 "White-Box Testing".
 *    -# Oram, A. and Wilson, G., editors (2007) "Beautiful Code", O'Reilly
 *       Media, Sebastopol, California 95472.  See Chapter 7, "Beautiful
 *       Tests", by Alberto Savoia.
 */

#include <xpc/unit_test.h>             /* unit_test_t structure & functions   */
#include <xpc/unit_test_status.h>      /* unit_test_status_t functions        */

#if XPC_HAVE_STRING_H
#include <string.h>                    /* char * functions                    */
#endif

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* fprintf() and stdout, stderr        */
#endif

/**
 *    Allocates the help string, and fills it with the provided text.
 *
 * \internal
 *    This function is an internal helper function for the unit_test
 *    utility.
 *
 * \return
 *    Returns 'true' if the parameter was valid and all steps succeeded.
 *    If 'false' is returned, no allocation was made.
 *
 * \unittests
 *    -  Since this is a static (private) function, only indirect tests are
 *       available.  See tests for the unit_test_init() and
 *       unit_test_initialize() functions.
 */

static cbool_t
unit_test_allocate_help
(
   unit_test_t * tests,          /**< The "this" pointer for this function.   */
   const char * helptext         /**< Help text to copy into new buffer.      */
)
{
   /*
    * As much as we'd like to be able to show errors, we cannot later free()
    * m_Additional_Help if it points to one of these items.
    */

   cbool_t result = xpccut_thisptr(tests);
   if (result)
   {
      size_t helpsize = 0;
      result = false;
      if (cut_not_nullptr(helptext))
         helpsize = strlen(helptext);

      if (helpsize > 0)
      {
         char * help;
         if (cut_not_nullptr(tests->m_Additional_Help))
         {
            free(tests->m_Additional_Help);
            set_nullptr(tests->m_Additional_Help);
         }
         help = malloc(helpsize + 1);
         if (cut_not_nullptr(help))
         {
            tests->m_Additional_Help = help;
            strcpy(help, helptext);                   /* DANGER */
            result = true;
         }
         else
         {
            /*
             * No can do: tests->m_Additional_Help = gi_help_failed;
             */

            xpccut_errprint_3_func(_("failed"), "unit_test_allocate_help()");
         }
      }
      else
      {
         /*
          * No can do: tests->m_Additional_Help = gi_help_null;
          */

         xpccut_errprint_3_func(_("empty help"), "unit_test_allocate_help()");
      }
   }
   return result;
}

/**
 *    Allocates or reallocates the case-list array.
 *
 *    The first time it is called, XPCCUT_CASE_ALLOCATION slots for
 *    unit_test_func_t pointers are allocated.
 *    Every time it is called thereafter, the array is reallocated for
 *    another XPCCUT_CASE_ALLOCATION worth of pointers.
 *
 *    This function takes pains to avoid orphaned memory.
 *
 * \internal
 *    This function is an internal helper function for the unit_test
 *    utility.
 *
 * \return
 *    Returns 'true' if the parameter was valid and all steps succeeded.
 *    If 'false' is returned, no allocation or reallocation was made.
 *
 * \unittests
 *    -  Since this is a static (private) function, only indirect tests are
 *       available.  See the tests for the unit_test_initiatilize()
 *       function.
 */

static cbool_t
unit_test_allocate_cases
(
   unit_test_t * tests           /**< The "this" pointer for this function.   */
)
{
   cbool_t result = xpccut_thisptr(tests);
   if (result)
   {
      if (tests->m_Test_Count == 0)             /* no tests yet loaded        */
      {
         result = cut_is_nullptr(tests->m_Test_Cases);
         if (result)
         {
            size_t casesize = 0;
            tests->m_Allocation_Count = XPCCUT_CASE_ALLOCATION;
            casesize = tests->m_Allocation_Count * sizeof(unit_test_func_t);
            tests->m_Test_Cases = malloc(casesize);
            if (cut_is_nullptr(tests->m_Test_Cases))
            {
               xpccut_errprint_func(_("failed"));
               result = false;
            }
         }
         else
            xpccut_errprint_func(_("already allocated"));
      }
      else
      {
         result = cut_not_nullptr(tests->m_Test_Cases);
         if (result)
         {
            size_t casesize = 0;
            tests->m_Allocation_Count += XPCCUT_CASE_ALLOCATION;
            casesize = tests->m_Allocation_Count * sizeof(unit_test_func_t);
            tests->m_Test_Cases = realloc(tests->m_Test_Cases, casesize);
            if (cut_is_nullptr(tests->m_Test_Cases))
            {
               xpccut_errprint_func(_("failed to reallocate case list"));
               result = false;
            }
         }
         else
            xpccut_errprint_func(_("null case list when reallocating"));
      }
   }
   return result;
}

/**
 *    Allocates the case-list array for the first time.
 *    This function first makes sure that any memory allocated to
 *    unit_test_t.m_Test_Cases is freed.  Then it sets the initial values
 *    describing the number of tests.  The it calls
 *    unit_test_allocate_cases() to allocate the memory.
 *
 * \internal
 *    This function is an internal helper function for the unit_test_t
 *    utility.
 *
 * \return
 *    Returns 'true' if the parameter was valid and all steps succeeded.
 *    If the parameter is valid, but 'false' is returned, then the
 *    m_Test_Cases is freed and set to nullptr.
 *
 * \unittests
 *    -  Since this is a static (private) function, only indirect tests are
 *       available.  See the tests for the unit_test_initialize() function.
 */

static cbool_t
unit_test_init_cases
(
   unit_test_t * tests           /**< The "this" pointer for this function.   */
)
{
   cbool_t result = xpccut_thisptr(tests);
   if (result)
   {
      if (cut_not_nullptr(tests->m_Test_Cases))
         free(tests->m_Test_Cases);

      tests->m_Test_Cases           = nullptr;
      tests->m_Total_Errors         =  0;
      tests->m_Current_Test_Number  = XPCCUT_NO_CURRENT_TEST;
      tests->m_Test_Count           =  0;
      tests->m_Subtest_Count        =  0;
      tests->m_Allocation_Count     =  0;
      result = unit_test_allocate_cases(tests);
   }
   return result;
}

/**
 *    Provides a beep/prompt function for unit-testing.
 *
 *    This function implements the basic functionality of the --case-pause
 *    option.  At this point, the caller can Ctrl-C to abort the test, if
 *    desired.
 *
 *    Note that the fprintf() statement does not bother checking the
 *    xpccut_is_silent() option.  We assuming that an interactive test will
 *    not be run with the --silent option.  (grin)
 *
 * \unittests
 *    -  Since this is a static (private) function, only indirect tests are
 *       available.
 */

static void
unit_test_pause
(
   unit_test_t * tests              /**< The "this pointer" for this test.    */
)
{
   if (unit_test_options_do_beep(&tests->m_App_Options))
      unit_test_status_beep();

   fprintf
   (
      stdout, "\n  %s",
      _("press Enter to continue testing or Ctrl-C to end testing")
   );
   (void) xpccut_get_response();
}

/**
 *    Used by unit_test_init() and unit_test_initialize().
 *
 * \return
 *    Returns 'true' if the parameter was valid and all steps succeeded.
 *
 * \unittests
 *    -  Since this is a static (private) function, only indirect tests are
 *       available.  See the unit-tests for the unit_test_init() and
 *       unit_test_initialize() functions.
 */

static cbool_t
unit_test_clear
(
   unit_test_t * tests           /**< The "this" pointer for this function.   */
)
{
   cbool_t result = xpccut_thisptr(tests);
   if (result)
   {
      tests->m_Test_Application_Name[0]      = 0;           /* empty string   */
      tests->m_Test_Application_Version[0]   = 0;           /* empty string   */
      tests->m_Additional_Help               = nullptr;
      tests->m_Test_Count                    = 0;
      tests->m_Subtest_Count                 = 0;
      tests->m_Current_Test_Number           = XPCCUT_NO_CURRENT_TEST;
      tests->m_First_Failed_Test             = 0;
      tests->m_First_Failed_Group            = 0;
      tests->m_First_Failed_Case             = 0;
      tests->m_First_Failed_Subtest          = 0;
      tests->m_Total_Errors                  = 0;
      tests->m_Test_Cases                    = nullptr;
      tests->m_Start_Time_us.tv_sec          = 0;
      tests->m_Start_Time_us.tv_usec         = 0;
      tests->m_End_Time_us.tv_sec            = 0;
      tests->m_End_Time_us.tv_usec           = 0;
   }
   return result;
}

/**
 *    Creates a default test setup with default options.
 *    The name, version, and help-text are bogus, but this constructor might
 *    be useful for quick-and-dirty tests of a consistent format.
 *
 * \steps
 *    -# Check the \a tests parameters.  As with all functions in this
 *       module, the \a tests parameters is effectively a "this" pointer.
 *       The call to xpccut_thisptr() can be disabled by application code
 *       for efficiency's sake, when this parameter can be guaranteed to be
 *       valid.
 *    -# unit_test_options_init() is called to set m_App_Options to its
 *       default values.
 *    -# The m_Test_Application_Name member is set to "No name".
 *    -# The m_Test_Application_Version member is set to "0.1".
 *    -# unit_test_allocate_help() is called to fill in a default help
 *       string.
 *    -# unit_test_init_cases() is called to allocate the initial block for
 *       the case-list array.
 *
 * \return
 *    Returns 'true' if the parameter was valid and all steps succeeded.
 *
 * \unittests
 *    -  unit_unit_test_04_01()
 */

cbool_t
unit_test_init
(
   unit_test_t * tests           /**< The "this" pointer for this function.   */
)
{
   cbool_t result = unit_test_clear(tests);
   if (result)
   {
      result = unit_test_options_init(&tests->m_App_Options);
      if (result)
      {
         strncpy(tests->m_Test_Application_Name, _("No Name"), XPCCUT_NAMELEN);
      }
      if (result)
      {
         strncpy
         (
            tests->m_Test_Application_Version, XPC_PACKAGE_VERSION,
            XPCCUT_VERSIONLEN
         );
      }
      if (result)
         result = unit_test_allocate_help(tests, "-----");

      if (result)
         result = unit_test_init_cases(tests);
   }
   return result;
}

/**
 *    Initializes the unit-test framework based on command-line parameters
 *    and internal application settings.
 *
 * \steps
 *    -# Call xpccut_text_domain() to set up the internationalization
 *       support for the test application.  Note that there could be
 *       two slightly different calls to set up this support, in the
 *       unit-test application for the XPC base library shipped separately
 *       from this unit-test-library.
 *    -# Check the \a tests parameters to make sure it is a good "this"
 *       pointer.
 *    -# unit_test_options_parse() is called to set m_App_Options to
 *       command-line values, if the \a argc and \a argv parameters are
 *       valid.  Otherwise, the options are assumed to initialized
 *       externally.  Existing values of the option are \e not modified.
 *       If you need them to be, call unit_test_options_init() or
 *       unit_test_options_initialize() yourself.
 *    -# The m_Test_Application_Name member is set to the provided name, if
 *       valid.
 *    -# The m_Test_Application_Version member is set to the provided
 *       version string, if valid.
 *    -# unit_test_allocate_help() is called to fill in the provided help
 *       string, if valid.
 *    -# unit_test_init_cases() is called to allocate the initial block for
 *       the case-list array.
 *
 * \return
 *    Returns 'true' if the parameters were valid and all steps succeeded.
 *
 * \unittests
 *    -  unit_unit_test_04_02()
 *    -  unit_unit_test_04_03()
 *    -  unit_unit_test_04_04()
 *
 * \todo
 *    Internationalization setup has been added, so how can we unit-test it?
 */

cbool_t
unit_test_initialize
(
   unit_test_t * tests,          /**< The "this" pointer for this function.   */
   int argc,                     /**< A main()-style argument count.          */
   char * argv [],               /**< A main()-style argument list.           */
   const char * appname,         /**< The short name of the application.      */
   const char * appversion,      /**< Short version string for application.   */
   const char * addedhelp        /**< Additional help for the --help option.  */
)
{
   cbool_t result = unit_test_clear(tests);
   if (result)
   {
      result = unit_test_options_init(&tests->m_App_Options);  /* do defaults */
      if (result)
      {
         result = unit_test_options_parse                      /* cmd line    */
         (
            &tests->m_App_Options, argc, argv, appname, appversion, addedhelp
         );

         /*
          * We want to set NLS up only at the beginning of the application.
          */

#ifdef USE_GETTEXT         /* turned off by "./configure --disable-gettext"   */

         /*
          * \todo
          *    (void) xpccut_text_domain()
 *
          * Causes excess memory leaks, and we need to figure out why.
          */

#endif      // USE_GETTEXT
      }
      if (result)
      {
         if (cut_not_nullptr(appname))
         {
            if (strlen(appname) > 0)
            {
               strncpy(tests->m_Test_Application_Name, appname, XPCCUT_NAMELEN);
               tests->m_Test_Application_Name[XPCCUT_NAMELEN-1] = 0;
            }
            else
               tests->m_Test_Application_Name[0] = 0;
         }
         else
         {
            strncpy
            (
               tests->m_Test_Application_Name, _("Unit Test"), XPCCUT_NAMELEN
            );
         }
      }
      if (result)
      {
         if (cut_not_nullptr(appversion))
         {
            if (strlen(appversion) > 0)
            {
               strncpy
               (
                  tests->m_Test_Application_Version, appversion,
                  XPCCUT_VERSIONLEN
               );
               tests->m_Test_Application_Version[XPCCUT_VERSIONLEN-1] = 0;
            }
            else
               tests->m_Test_Application_Version[0] = 0;
         }
         else
         {
            strncpy
            (
               tests->m_Test_Application_Version, XPC_PACKAGE_VERSION,
               XPCCUT_VERSIONLEN
            );
         }
      }
      if (result)
      {
         if (cut_not_nullptr(addedhelp))
         {
            if (strlen(addedhelp) > 0)
               result = unit_test_allocate_help(tests, addedhelp);
            else
               result = unit_test_allocate_help(tests, _("No help"));
         }
         else
            result = unit_test_allocate_help(tests, _("No help"));
      }
      if (result)
         result = unit_test_init_cases(tests);
   }
   return result;
}

/**
 *    Removes any resources allocated by the "constructors" or other
 *    functions.
 *
 * \todo
 *    Add an atexit() handler to guarantee that this job gets done even if
 *    the programmer is sloppy.
 *
 * \unittests
 *    -  Run "valgrind --leak-check=full ./unit_test_test" to verify this
 *       function.
 *    -  unit_unit_test_04_01()
 *    -  unit_unit_test_04_02()
 *    -  unit_unit_test_04_03()
 *    -  unit_unit_test_04_04()
 */

void
unit_test_destroy
(
   unit_test_t * tests           /**< The "this" pointer for this function.   */
)
{
   cbool_t ok = xpccut_thisptr(tests);
   if (ok)
   {
      if (cut_not_nullptr(tests->m_Additional_Help))
      {
         free(tests->m_Additional_Help);
         tests->m_Additional_Help = nullptr;
      }
      if (cut_not_nullptr(tests->m_Test_Cases))
      {
         free(tests->m_Test_Cases);
         tests->m_Test_Cases = nullptr;
      }
   }
}

/**
 *    Loads a unit-test function pointer in the list of test cases.
 *
 *    This function loads one test function into the case-list.  If it
 *    succeeds, the internal test counter is incremented as an internal
 *    side-effect, and the value "true" is returned.
 *
 * \return
 *    Returns 'true' if the parameter was valid and all steps succeeded.
 *
 * \unittests
 *    -  unit_unit_test_04_05()
 *
 * \todo
 *    Consider creating a function that can copy an array of unit-test
 *    function pointers all at once, reallocating as necessary, instead of
 *    requiring a lot of run-time load calls.
 */

cbool_t
unit_test_load
(
   unit_test_t * tests,          /**< The "this" pointer for this function.   */
   const unit_test_func_t test   /**< The unit-test function to load.         */
)
{
   cbool_t result = xpccut_thisptr(tests);
   if (result)
   {
      result = cut_not_nullptr(test);
      if (result)
      {
         int count = tests->m_Test_Count;
         if ((count + 1) > tests->m_Allocation_Count)
            result = unit_test_allocate_cases(tests);

         if (result)
         {
            tests->m_Test_Cases[count++] = test;
            tests->m_Test_Count = count;
         }
      }
      else
         xpccut_errprint_func(_("null test pointer"));
   }
   return result;
}

/**
 *    Exposes incrementing the test count for use in wrapper libraries.
 *
 * \return
 *    Returns 'true' if the parameter was valid and all steps succeeded.
 *
 * \unittests
 *    -  unit_unit_test_04_06()
 */

cbool_t
unit_test_cpp_load_count
(
   unit_test_t * tests           /**< The "this" pointer for this function.   */
)
{
   cbool_t result = xpccut_thisptr(tests);
   if (result)
      tests->m_Test_Count++;

   return result;
}

/**
 *    This function handles the disposition of a test.
 *
 * \return
 *    Returns true if the user asked to quit.
 *
 * \sideeffect
 *    -  The pass/fail status of the test may be modified as a side-effect.
 *    -  This side-effect also has the side effect of incrementing
 *       unit_test_status_t.m_Subtest_Error_Count, if the fail status is set.
 *       This count could thus end up being a misleading value, so be
 *       careful!
 *
 * \unittests
 *    -  unit_unit_test_04_07()
 */

cbool_t
unit_test_dispose
(
   unit_test_status_t * status         /**< The "this pointer" for status.    */
)
{
   cbool_t result = false;                      /* false means "do not quit"  */
   cbool_t ok = xpccut_thisptr(status);
   if (ok)
   {
      switch (unit_test_status_disposition(status))
      {
      case XPCCUT_DISPOSITION_CONTINUE:

         /*
          * No changes to make, the tests should continue according to plan.
          */

         break;

      case XPCCUT_DISPOSITION_DNT:

         unit_test_status_pass(status, true);   /* set continued testing      */
         break;

      case XPCCUT_DISPOSITION_FAILED:

         unit_test_status_pass(status, false);  /* force the test to fail     */
         break;

      case XPCCUT_DISPOSITION_QUITTED:

         unit_test_status_pass(status, true);   /* guarantee a passed test    */
         result = true;                         /* force the testing to end   */
         break;

      case XPCCUT_DISPOSITION_ABORTED:

         unit_test_status_pass(status, false);  /* force the test to fail     */
         result = true;                         /* force the testing to end   */
         break;
      }
   }
   else
      result = true;                            /* quit, due to error         */

   return result;
}

/**
 *    Provides the number of tests built into the unit-test application.
 *    This value is given by the m_Test_Count field.
 *
 * \note
 *    We've decided not to return -1 in case of an error, for this
 *    particular integer accessor function.  It seems more intuitive to
 *    check for 0, rather than -1, for this function.
 *
 * \return
 *    Returns the number of unit-tests.  If the parameter is invalid, then 0
 *    is returned.  Also, having no test(s) should be treated as an error by
 *    the caller.
 *
 * \unittests
 *    -  unit_unit_test_04_08()
 */

int
unit_test_count
(
   const unit_test_t * tests        /**< The "this pointer" for this test.    */
)
{
   int result = 0;
   if (xpccut_thisptr(tests))
      result = tests->m_Test_Count;

   return result;
}

/**
 *    Provides the current test number.
 *    This value is given by the m_Current_Test_Number field.
 *
 * \return
 *    Returns the number of the current test.  If the parameter is invalid,
 *    then -1 (XPCCUT_NO_CURRENT_TEST) is returned.  If no tests have been
 *    set up yet, then -1 is returned then, too.
 *
 * \unittests
 *    -  unit_unit_test_04_09()
 */

int
unit_test_number
(
   const unit_test_t * tests        /**< The "this pointer" for this test.    */
)
{
   int result = XPCCUT_NO_CURRENT_TEST;
   if (xpccut_thisptr(tests))
      result = tests->m_Current_Test_Number;

   return result;
}

/**
 *    Provides the number of tests built into the unit-test application.
 *
 *    This value is given by the m_Subtest_Count field.
 *
 * \note
 *    We've decided not to return -1 in case of an error, for this
 *    particular integer accessor function.
 *
 * \return
 *    Returns the number of unit-tests.  If the parameter is invalid, then 0
 *    is returned.  Having no subtests should be treated as an error by
 *    the caller.
 *
 * \unittests
 *    -  unit_unit_test_04_11()
 */

int
unit_test_subtest_count
(
   const unit_test_t * tests        /**< The "this pointer" for this test.    */
)
{
   int result = 0;
   if (xpccut_thisptr(tests))
      result = tests->m_Subtest_Count;

   return result;
}

/**
 *    Provides the number of tests that failed.
 *
 *    This value is given by the m_Total_Errors field.
 *
 * \return
 *    Returns the number of tests that failed.  If the parameter is invalid,
 *    then -1 (XPCCUT_INVALID_PARAMETER) is returned.
 *
 * \unittests
 *    -  unit_unit_test_04_12()
 */

int
unit_test_failures
(
   const unit_test_t * tests        /**< The "this pointer" for this test.    */
)
{
   int result = XPCCUT_INVALID_PARAMETER;
   if (xpccut_thisptr(tests))
      result = tests->m_Total_Errors;

   return result;
}

/**
 *    Provides the number of the first test that failed.
 *    This value is given by the m_First_Failed_Test field.  This is an
 *    ordinal number that spans group and case numbers.  It starts at 1.
 *
 * \return
 *    Returns the number of the first test that failed.  If the parameter is
 *    invalid, then -1 (XPCCUT_INVALID_PARAMETER) is returned.  If no test
 *    failed, then 0 is returned.
 *
 * \unittests
 *    -  unit_unit_test_04_13()
 */

int
unit_test_first_failed_test
(
   const unit_test_t * tests        /**< The "this pointer" for this test.    */
)
{
   int result = XPCCUT_INVALID_PARAMETER;
   if (xpccut_thisptr(tests))
      result = tests->m_First_Failed_Test;

   return result;
}

/**
 *    Provides the number of the first test group that failed.
 *    This value is given by the m_First_Failed_Group field.
 *
 * \return
 *    Returns the number of the first test group that failed.  If the
 *    parameter is invalid, then -1 (XPCCUT_INVALID_PARAMETER) is returned.
 *    If no test failed, then 0 is returned.
 *
 * \unittests
 *    -  unit_unit_test_04_14()
 *
 */

int
unit_test_first_failed_group
(
   const unit_test_t * tests        /**< The "this pointer" for this test.    */
)
{
   int result = XPCCUT_INVALID_PARAMETER;
   if (xpccut_thisptr(tests))
      result = tests->m_First_Failed_Group;

   return result;
}

/**
 *    Provides the number of the first test case that failed.
 *    This value is given by the m_First_Failed_Case field.  This is an
 *    ordinal number that spans group and case numbers.  It starts at 1.
 *
 * \return
 *    Returns the number of the first test case that failed.  If the
 *    parameter is invalid, then -1 (XPCCUT_INVALID_PARAMETER) is returned.
 *    If no test failed, then 0 is returned.
 *
 * \unittests
 *    -  unit_unit_test_04_15()
 */

int
unit_test_first_failed_case
(
   const unit_test_t * tests        /**< The "this pointer" for this test.    */
)
{
   int result = XPCCUT_INVALID_PARAMETER;
   if (xpccut_thisptr(tests))
      result = tests->m_First_Failed_Case;

   return result;
}

/**
 *    Provides the number of the first test subtest that failed.
 *    This value is given by the m_First_Failed_Subtest field.  This is an
 *    ordinal number that spans group and case numbers.  It starts at 1.
 *
 * \return
 *    Returns the number of the first test subtest that failed.  If the
 *    parameter is invalid, then -1 (XPCCUT_INVALID_PARAMETER) is returned.
 *    If no test failed, then 0 is returned.
 *
 * \unittests
 *    -  unit_unit_test_04_16()
 */

int
unit_test_first_failed_subtest
(
   const unit_test_t * tests        /**< The "this pointer" for this test.    */
)
{
   int result = XPCCUT_INVALID_PARAMETER;
   if (xpccut_thisptr(tests))
      result = tests->m_First_Failed_Subtest;

   return result;
}

/**
 *    Provides the initialization component of unit_test_run().
 *    This function is a helper function that is exposed so that a C++
 *    wrapper library won't have to reimplement the same functionality.
 *
 * \return
 *    Returns the number of tests that have been loaded.  The caller should
 *    not have to count the number of tests.
 *
 * \unittests
 *    -  unit_unit_test_04_17()
 */

int
unit_test_run_init
(
   unit_test_t * tests  /**< "this pointer" for this function, assumed valid. */
)
{
   int length = 0;
   if (xpccut_thisptr(tests))
   {
      length = tests->m_Test_Count;
      if (length == 0)
         xpccut_errprint_func(_("no unit tests loaded"));
      else
      {
         tests->m_Current_Test_Number  = XPCCUT_NO_CURRENT_TEST;
         tests->m_First_Failed_Test    =  0;
         tests->m_First_Failed_Group   =  0;
         tests->m_First_Failed_Case    =  0;
         tests->m_First_Failed_Subtest =  0;
         tests->m_Total_Errors         =  0;
         if (unit_test_options_show_progress(&tests->m_App_Options))
         {
            fprintf
            (
               stdout,
            "===============================================================\n"
            "%s %s\n"
            "---------------------------------------------------------------\n"
               ,
               tests->m_Test_Application_Name,
               tests->m_Test_Application_Version
            );
         }
         xpccut_get_microseconds(&tests->m_Start_Time_us);
      }
   }
   return length;
}

/**
 *    Provides the show-progress component of unit_test_run().
 *    In addition to showing the provided test number, this function also
 *    sets/increments the unit_test_t.m_Current_Test_Number field.
 *
 *    This function is a helper function that is exposed so that the C++
 *    wrapper library won't have to reimplement the same functionality.
 *
 *    The \a testnum field normally is the integer index of the for-loop
 *    used to iterate over all of the tests that have been loaded.  However,
 *    the caller can also call this function in a loop until it returns a
 *    -1 (XPCCUT_NO_CURRENT_TEST).
 *
 * \return
 *    Returns the current test number, re 0.  If there is a problem, or if
 *    the next test number exceeds the actual number of loaded tests, then
 *    -1 (XPCCUT_NO_CURRENT_TEST) is returned.
 *
 * \unittests
 *    -  unit_unit_test_04_10()
 */

int
unit_test_next_test
(
   unit_test_t * tests  /**< "this pointer" for this function, assumed valid. */
)
{
   int result = XPCCUT_NO_CURRENT_TEST;
   if (xpccut_thisptr(tests))
   {
      /*
       * Since the caller normally provides a for-loop that counts the tests,
       * we could simply load the index number and show it.
       *
       *    tests->m_Current_Test_Number = testnum;
       *
       *    . . .
       *
       *    fprintf(stdout, "\n%s %3d:  \n", _("TEST"), testnum+1);
       *
       * But this puts a burden on the caller and makes the API a tiny bit more
       * complex.
       */

      result = ++tests->m_Current_Test_Number;           /* test number re 0  */
      if (result >= tests->m_Test_Count)                 /* no more tests?    */
         result = XPCCUT_NO_CURRENT_TEST;                /* no more tests!    */
   }
   return result;
}

/**
 *    Provides the post-hoc analysis of the test.
 *
 * \return
 *    Returns 'true' if the tests are supposed to stop immediately.  If
 *    either the \a tests or \a status parameters are null, this is
 *    considered an error, but it will not stop the tests
 *
 * \sideeffect
 *    The result of the test is returned in the \a runresult parameter.
 *    Other unit_test_t fields are modified.
 *
 * \unittests
 *    -  unit_unit_test_04_18()
 */

cbool_t
unit_test_dispose_of_test
(
   unit_test_t * tests,             /**< The "this pointer", assumed valid.   */
   unit_test_status_t * status,     /**< The status object to update.         */
   cbool_t * runresult              /**< Output flag for some failure.        */
)
{
   cbool_t result = false;                      /* if true, caller stops loop */
   cbool_t ok = cut_not_nullptr(status);
   if (! ok)
      xpccut_errprint_func(_("null status pointer"));

   if (xpccut_thisptr(tests) && ok)
   {
      cbool_t quit = unit_test_dispose(status); /* check test result/options  */
      if (cut_not_nullptr(runresult))
         *runresult = unit_test_status_passed(status);
      else
         xpccut_errprint_func(_("PROGRAMMER -- null run-result pointer"));

      if (unit_test_status_passed(status))
      {
         int zzz = unit_test_options_test_sleep_time(&tests->m_App_Options);
         if (zzz > 0)
         {
            if (unit_test_options_show_progress(&tests->m_App_Options))
            {
               fprintf
               (
                  stdout, "  %s %d %s\n",
                  _("Sleeping"), zzz, _("milliseconds")
               );
            }
            xpccut_ms_sleep(zzz);
         }
      }
      else
      {
         tests->m_Total_Errors++;
         if (tests->m_First_Failed_Test == 0)
            tests->m_First_Failed_Test = tests->m_Current_Test_Number;

         if (tests->m_First_Failed_Group == 0)
            tests->m_First_Failed_Group = unit_test_status_group(status);

         if (tests->m_First_Failed_Case == 0)
            tests->m_First_Failed_Case = unit_test_status_case(status);

         if (tests->m_First_Failed_Subtest == 0)
         {
            tests->m_First_Failed_Subtest =
               unit_test_status_failed_subtest(status);
         }
         if (unit_test_options_stop_on_error(&tests->m_App_Options))
         {
            if (! xpccut_is_silent())
            {
               fprintf
               (
                  stdout, "  %s %d\n",
                  _("Stop-on-error enabled; failure in TEST"),
                  unit_test_number(tests)+1
               );
            }
            result = true;
         }
      }

      /*
       * The skip is already noted in the main title, but it makes the
       * output look better, when no "--verbose" option is being used, if
       * we show it here.
       */

      if
      (
         unit_test_options_show_progress(&tests->m_App_Options) &&
            unit_test_options_is_verbose(&tests->m_App_Options)
      )
      {
         if (unit_test_status_is_skipped(status))
            fprintf(stdout, "  %s\n", _("Skipped"));     /* flush cout */
      }
      if (quit)
      {
         if (unit_test_options_show_progress(&tests->m_App_Options))
            fprintf(stdout, "  %s\n", _("User requested an end to testing"));

         result = true;
      }
      if (result && unit_test_options_is_verbose(&tests->m_App_Options))
         fprintf(stdout, "%s\n", _("Quitting the tests early"));
   }
   return result;
}

/**
 *    Provides the post-loop component of unit_test_run().
 *    This function is a helper function that is exposed so that a C++
 *    wrapper library won't have to reimplement the same functionality.
 *
 * \unittests
 *    No unit-test at this time.  This function has no output or
 *    side-effects.  One could redirect stdout for the duration of this
 *    function, but it doesn't strike one as a fruitful option.
 */

void
unit_test_post_loop
(
   unit_test_t * tests,             /**< "this pointer" for this function.    */
   cbool_t testresult               /**< Test result the caller had found.    */
)
{
   if (xpccut_thisptr(tests))
   {
      if (unit_test_options_is_summary(&tests->m_App_Options))
      {
         fprintf
         (
            stdout,
            "\n"
            "%d %s.\n"
            "%s.\n"
            ,
            unit_test_subtest_count(tests), _("sub-tests encountered"),
            _("Tests summarized, not performed")
         );
      }
      else
      {
         double duration_ms;
         xpccut_get_microseconds(&tests->m_End_Time_us);
         duration_ms = 0.001 * xpccut_time_difference_us
         (
            tests->m_Start_Time_us, tests->m_End_Time_us
         );
         if (unit_test_options_show_progress(&tests->m_App_Options))
         {
            fprintf
            (
               stdout,
               "\n------------------------------------------------------------\n"
            );
         }
         if (testresult)
         {
            if (unit_test_options_show_progress(&tests->m_App_Options))
            {
               fprintf
               (
                  stdout, "%d %s.\n%d %s.\n",
                  unit_test_count(tests),
                  _("unit-tests completed; all succeeded or were skipped"),
                  unit_test_subtest_count(tests),
                  _("sub-tests encountered")
               );
            }
         }
         else
         {
            /*
             * Here's a quandary -- we want to see errors noted in the
             * report even if --silent is set, generally.  However, some of
             * the XPCCUT unit-tests of itself simulate errors, which we
             * really don't care to see, because we know they're always
             * failures.
             */

            if (! xpccut_is_silent())
            {
               fprintf
               (
                  stdout,
                  "%d %s (%d %s). %d %s.\n"
                  "  %s: %d (%s %d, %s %d, %s %d)\n"
                  ,
                  unit_test_count(tests), _("tests completed"),
                  unit_test_subtest_count(tests), _("sub-tests encountered"),
                  tests->m_Total_Errors, _("failed"),
                  _("First failed unit-test number"),
                  tests->m_First_Failed_Test+1,
                  _("Group"), tests->m_First_Failed_Group,
                  _("Case"), tests->m_First_Failed_Case,
                  _("Sub-test"), tests->m_First_Failed_Subtest
               );
            }
         }
         if (unit_test_options_show_progress(&tests->m_App_Options))
         {
            fprintf
            (
               stdout, "%s: %4.3f ms\n", _("Full test duration"), duration_ms
            );
            fprintf
            (
               stdout,
               "============================================================\n"
            );
         }
      }
   }
}

/**
 *    Checks the sub-test count for the caller.
 *    This function is a helper function that is exposed so that a C++
 *    wrapper library won't have to reimplement the same functionality.
 *
 * \return
 *    If at least one subtest was counted, this function returns the number
 *    of sub-tests counted (in the current test-case function).  If no
 *    sub-tests were counted, but the --require-sub-test option is not
 *    activated, then 0 is returned.  Otherwise, -1 (XPCCUT_NO_CURRENT_TEST)
 *    is returned, and the caller should exit its test-loop.
 *
 * \sideeffect
 *    The valid sub-test count is added to the current value of
 *    m_Subtest_Count.
 *
 * \unittests
 *    -  unit_unit_test_04_23()
 */

int
unit_test_check_subtests
(
   unit_test_t * tests,                /**< "this pointer" for this function. */
   const unit_test_status_t * status   /**< The ownder of the sub-test count. */
)
{
   int result = XPCCUT_NO_CURRENT_TEST;
   if (xpccut_thisptr(tests))
   {
      /*
       * Keep track of all the sub-test counts encountered.
       */

      int subtest_count = unit_test_status_subtest(status);
      if (subtest_count > 0)
      {
         tests->m_Subtest_Count += subtest_count;
         result = subtest_count;
      }
      else
      {
         if (subtest_count == XPCCUT_INVALID_PARAMETER)
         {
            /* Generally due to a stock null pointer error. */
         }
         else
         {
            /*
             * Not sure we like this option, so it is false by default.
             */

            if (unit_test_options_need_subtests(&tests->m_App_Options))
               xpccut_errprint_func(_("PROGRAMMER--no subtests encountered"));
            else
               result = 0;
         }
      }
   }
   return result;
}

/**
 *    Provides the initialization component of unit_test_run().
 *    This function is a helper function that is exposed so that the C++
 *    wrapper library won't have to reimplement the same functionality.
 *
 * \steps
 *    -# <tt>while ((testnumber = unit_test_next_test(tests)) >= 0) </tt>
 *       This call increments the current unit-test number, then makes sure it
 *       has not exceeded the actual number of unit-tests.
 *    -# <tt>testresult = unit_test_run_a_test() </tt>
 *       This call runs the current unit-test and sets the status based on
 *       the test and run-time settings.
 *    -# <tt>if (unit_test_check_subtests(tests, &testresult) < 0) break </tt>
 *       This call makes sure the number of sub-tests in the current
 *       unit-test is valid, and then adds this count to the running total,
 *       so that the user can see how many sub-tests were run in the whole
 *       application.
 *    -# <tt>if (unit_test_dispose_of_test(tests, &testresult, &result))
 *       break </tt>
 *       This call does the following:
 *       -# Checks if quitting the tests is indicated.
 *       -# Gets the pass/fail status of the current unit-test into \e
 *          result.
 *       -# Increments m_Total_Errors if the unit-test failed.
 *
 * \warning
 *    <b>If you make any changes to this function</b>, you almost certainly
 *    have to make corresponding changes to the xpc::cut::run() function in
 *    the C++ unit-test wrapper library.
 *
 * \return
 *    Returns 'true' if the parameters and setup were all valid, and then
 *    all of the loaded unit-tests succeeded.
 *
 * \unittests
 *    -  unit_unit_test_04_19()
 */

cbool_t
unit_test_run
(
   unit_test_t * tests              /**< The "this pointer" for this test.    */
)
{
   cbool_t result = false;
   if (xpccut_thisptr(tests))
   {
      int length = unit_test_run_init(tests);
      if (length == 0)
         result = false;                  /* no tests found to be loaded      */
      else
      {
         int testnumber;
         result = true;                   /* must set to true for usage below */
         while ((testnumber = unit_test_next_test(tests)) >= 0)
         {
            unit_test_status_t testresult;
            if (unit_test_status_init(&testresult))   /* ca 06/25/2008 new */
            {
               testresult = unit_test_run_a_test
               (
                  tests, tests->m_Test_Cases[testnumber]
               );
               if (unit_test_check_subtests(tests, &testresult) < 0)
                  break;

               if (unit_test_dispose_of_test(tests, &testresult, &result))
                  break;
            }
            else
               break;
         }

         /*
          * What can we do with the status returned above in \e result?
          * The overall result has to be based on the total-error count, not
          * the result of the latest test, which could have passed, while
          * every previous test failed.
          */

         result = tests->m_Total_Errors == 0;
      }
      unit_test_post_loop(tests, result);
   }
   return result;
}

/**
 *    Provides the prelude of unit_test_run_a_test().
 *    This function is a helper function that is exposed so that the C++
 *    wrapper library won't have to reimplement the same functionality.
 *
 * \note
 *    We use intptr_t instead of "void *" because gcc 4.2 warns about
 *    passing a function pointer to a void pointer parameter.
 *
 * \return
 *    Returns 'true' if the setup was valid.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  unit_unit_test_04_20()
 *
 * \todo
 *    There is a flaw in this function; it prints a translated string into a
 *    small buffer, and it is possible to provide a translated string larger
 *    than the buffer -- the string will then be truncated (safely).
 */

cbool_t
unit_test_run_a_test_before
(
   unit_test_t * tests,             /**< The "this pointer" for this test.    */
   intptr_t int_test                /**< Pointer to the current test.         */
)
{
   cbool_t result = xpccut_thisptr(tests);
   if (result)
   {
      result = cut_not_nullptr((void *) int_test);
      if (result)
      {
         tests->m_App_Options.m_Current_Test_Number =
            tests->m_Current_Test_Number;
      }
      else
      {
         char temp[64];

         /**
          * NOT DEFINED IN VISUAL STUDIO.  Also applies to srandom(), random(),
          *
          */

         snprintf
         (
            temp, 63, "%d (%s)",
            tests->m_Current_Test_Number, _("ordinal")
         );
         xpccut_errprint_ex(_("test-function pointer null"), temp);
      }
   }
   return result;
}

/**
 *    Provides a compile-time option.
 *    It selects how to display the test result.
 *    There are no plans as yet to change this value.  In fact, I don't
 *    remember why I changed the format of the output!  Doh!
 */

static cbool_t g_use_one_line = false;

/**
 *    Provides the postlude of unit_test_run_a_test().
 *    This function is a helper function that is exposed so that the C++
 *    wrapper library won't have to reimplement the same functionality.
 *
 * \return
 *    This function returns 'true' if both of the pointer parameters were
 *    valid.
 *
 * \unittests
 *    -  unit_unit_test_04_21()
 */

cbool_t
unit_test_run_a_test_after
(
   unit_test_t * tests,             /**< The "this pointer" for this test.    */
   unit_test_status_t * status      /**< Pointer to the status structure.     */
)
{
   cbool_t result = cut_not_nullptr(status);
   if (result)
      result = xpccut_thisptr(tests);
   else
      xpccut_errprint_func(_("null status pointer"));

   if (result)
   {
      cbool_t show_result;
      (void) unit_test_status_time_delta(status, false);          /* time it  */
      show_result =
      (
         ! unit_test_status_is_skipped(status) &&
         ! unit_test_options_is_summary(&tests->m_App_Options)
      );
      if (show_result)
      {
         if (unit_test_options_show_progress(&tests->m_App_Options))
         {
            if (g_use_one_line)
            {
               fprintf
               (
                  stdout,
                  "  %s %2d '%s'  %s %2d '%s' %s %2d:\n  %s",     /* (no \n)  */
                  _("Group"),
                  unit_test_status_group(status),
                  unit_test_status_group_name(status),
                  _("Case"),
                  unit_test_status_case(status),
                  unit_test_status_case_name(status),
                  _("through Subtest"),
                  unit_test_status_subtest(status),
                  unit_test_status_passed(status) ? _("PASSED") : _("FAILED")
               );
            }
            else
            {
               fprintf
               (
                  stdout,
                  "  %12s %2d '%s'\n  %12s %2d '%s'\n  %12s %2d %s:\n  %s",
                  _("Group"),
                  unit_test_status_group(status),
                  unit_test_status_group_name(status),
                  _("Case"),
                  unit_test_status_case(status),
                  unit_test_status_case_name(status),
                  _("Subtests"),
                  unit_test_status_subtest(status),
                  _("and below"),
                  unit_test_status_passed(status) ? _("PASSED") : _("FAILED")
               );
            }
            if (unit_test_status_duration_ms(status) >= 0.001)
            {
               fprintf
               (
                  stdout, " (%4.3f ms)", unit_test_status_duration_ms(status)
               );
            }
            else
            {
               fprintf(stdout, " (less than 0.001 ms)");
            }
            if (unit_test_status_error_count(status) > 1)
            {
               /*
                * More than one sub-test failed, so show the number of them
                * that failed, as well as the first one that failed.
                */

               fprintf
               (
                  stdout, "\n  %d %s. %s: %d\n",
                  unit_test_status_error_count(status),
                  _("subtests failed"), _("First failed sub-test"),
                  unit_test_status_failed_subtest(status)
               );
            }
            else if (unit_test_status_failed(status))
            {
               /*
                * Only one sub-test failed, so just show that one.
                */

               fprintf
               (
                  stdout, " %s %d\n", _("at sub-test"),
                  unit_test_status_failed_subtest(status)
               );
            }
            else
               fprintf(stdout, "\n");
         }
         if (unit_test_options_is_pause(&tests->m_App_Options))
            unit_test_pause(tests);
      }
      else
      {
         /*
          * This is normally true, but in long tests it shows nothing but
          * the text below, causing any test that did run to scroll off the
          * screen.  Therefore, we make the caller specify it on the command
          * line, using the --verbose option.
          *
          *    unit_test_options_show_progress(&tests->m_App_Options)
          */

         if
         (
            unit_test_status_is_skipped(status) &&
            unit_test_options_is_verbose(&tests->m_App_Options)
         )
         {
            fprintf(stdout, "  %s.\n", _("This test was skipped"));
         }
      }
   }
   return result;
}

/**
 *    This function runs a single unit test.
 *    The result of unit_test_run_a_test_after() is currently just a
 *    redundant pointer check.
 *
 * \return
 *    Returns a status structure reflecting the result of the current unit
 *    test.
 *
 * \unittests
 *    -  unit_unit_test_04_22()
 */

unit_test_status_t
unit_test_run_a_test
(
   unit_test_t * tests,             /**< The "this pointer" for this test.    */
   const unit_test_func_t test      /**< Pointer to the current test.         */
)
{
   unit_test_status_t result;
   cbool_t ok;
   intptr_t int_test = (intptr_t) test;                     /* cast required  */
   unit_test_status_init(&result);
   ok = unit_test_run_a_test_before(tests, int_test);
   if (ok)
   {
      result = (*test)(&tests->m_App_Options);              /* run the test   */
      ok = unit_test_run_a_test_after(tests, &result);
      if (! ok)
         xpccut_errprint_func(_("internal churn"));
   }
   else
   {
      (void) unit_test_status_fail(&result);
      xpccut_errprint_func(_("could not set up pre-test"));
   }
   return result;
}

/**
 *    Generates the final report.
 *
 * \note
 *    The test that determines if output is shown is only a call to
 *    unit_test_options_show_progress().  Currently the code depends on the
 *    --silent option (in unit_test_options.c) setting the
 *    --no-show-progress option, as well as setting the xpccut_is_silent()
 *    option.  At some point, the show-progress function may incorporate
 *    the "is silent" test.
 *
 * \unittests
 *    -  There is no unit-test for unit_test_report() at this time.  It
 *       doesn't affect any states or return any values.  It only shows
 *       output.
 */

void
unit_test_report
(
   const unit_test_t * tests,       /**< The "this pointer" for this test.    */
   cbool_t passed                   /**< The final status of the whole test.  */
)
{
   if (passed)
   {
      if (unit_test_options_show_progress(&tests->m_App_Options))
      {
         fprintf
         (
            stdout, "%s.\n",
            _("All unit-tests succeeded; the test suite passed")
         );
      }
   }
   else if (xpccut_thisptr(tests))
   {
      fprintf
      (
         stderr,
         "? %s -- %d/%d %s.\n",
         _("The unit-test suite did not pass"), unit_test_failures(tests),
         unit_test_count(tests), _("tests failed")
      );
   }
}

/**
 *    Writes the application version information to standard output.
 *    This information is stored in the m_Test_Application_Version array.
 *
 * \unittests
 *    -  There is no unit-test for unit_test_report() at this time.  It
 *       doesn't affect any states or return any values.  It only shows
 *       output.
 */

void
unit_test_version
(
   const unit_test_t * tests        /**< The "this pointer" for this test.    */
)
{
   if (xpccut_thisptr(tests))
   {
      fprintf
      (
         stdout, "%s %s\n",
         _("Version"), tests->m_Test_Application_Version
      );
   }
}

/*
 * unit_test.c
 *
 * vim: ts=3 sw=3 et ft=c
 */
