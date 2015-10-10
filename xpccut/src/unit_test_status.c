/**
 * \file          unit_test_status.c
 * \library       xpccut
 * \author        Chris Ahlstrom
 * \date          2008-03-07
 * \updates       2015-10-10
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides functions to manage a unit_test_status_t structure.
 *
 *    The unit_test_status_t structure holds a number of variables that
 *    describe the settings and result-variables for a single unit test.
 *    This structure is initialized and populated by a unit test, and then
 *    returned to the unit_test_run() loop to determine, based on the
 *    results and the options, the future course of the testing.
 *
 *    Also see the unit_test_status.h module and the unit_test_status_t
 *    structure typedef for more details.
 */

#include <xpc/unit_test_status.h>      /* unit_test_status_t functions        */
#include <xpc/portable_subset.h>       /* small set of portable functions     */

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* fprintf() and stdout                */
#endif

/**
 *    Provides a way to bypass the call to xpccut_get_response() in
 *    unit_test_status_prompt_before().
 *
 *    When debugging the --interactive option under GNU gdb, it can be
 *    difficult to get the prompt from the user.  Furthermore, it would be
 *    nice to be able to have an interactive test run automatically.
 */

static char gs_status_prompt_before  = 0;

/**
 *    Provides a way to bypass the call to xpccut_get_response() in
 *    unit_test_status_prompt_after().
 *    See gs_status_prompt_before for more details.
 */

static char gs_status_prompt_after   = 0;

/**
 *    Sets the default values of the given unit_test_status_t object.
 *    This constructor sets everything to default values.  If this version
 *    is used, then the unit_test_status_initialize() function must next be
 *    called, before this structure can be used.
 *
 *    Conversely, it is generally required to call the
 *    unit_test_status_init() function first, so that all values are
 *    guaranteed to have a reasonable value even if neglected by later
 *    operations.  Usually, the writer of the unit-test application doesn't
 *    have to worry about this detail, as it is handled internally.
 *
 * \return
 *    Returns 'true' if the function succeeded.  It will fail if the status
 *    pointer (our so-called "this" pointer) is null.
 *
 * \unittests
 *    -  Group 1 unit tests provide a quick functionality check of this
 *       function and many other functions, and also provide a good, if
 *       incomplete, example of how to write a unit-test.
 *    -  unit_unit_test_02_01()
 */

cbool_t
unit_test_status_init
(
   unit_test_status_t * status   /**< The "this" pointer for this function.   */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
   {
      status->m_Test_Options          = nullptr;
      status->m_Group_Name[0]         = 0;
      status->m_Case_Description[0]   = 0;
      status->m_Subtest_Name[0]       = 0;
      status->m_Test_Group            = 0;
      status->m_Test_Case             = 0;
      status->m_Subtest               = 0;
      status->m_Test_Result           = true;        /* for easy skipping    */
      status->m_Subtest_Error_Count   = 0;
      status->m_Failed_Subtest        = 0;
      status->m_Test_Disposition      = XPCCUT_DISPOSITION_ABORTED;
      status->m_Start_Time_us.tv_sec  = 0;
      status->m_Start_Time_us.tv_usec = 0;
      status->m_End_Time_us.tv_sec    = 0;
      status->m_End_Time_us.tv_usec   = 0;
      status->m_Test_Duration_ms      = 0.0;
   }
   return result;
}

/**
 *    Initializes and fills in a unit_test_status_t structure.
 *    This "constructor" lets a unit_test_options_t structure be wired in as
 *    a pointer.  It also lets the caller set the group number and name for
 *    the test, and the case number and name for the test.
 *
 *    The group number provides a way of grouping unit-tests into a
 *    selectable bundle of related tests.  It must be greater than 0, or
 *    this function will fail.
 *
 *    The case number provides a way of selecting only one test case to be
 *    run.  It must be greater than 0, or this function will fail.
 *
 *    The group name and case name provide brief descriptions of these
 *    items, so that the user can understand the tests being done by
 *    supplying the --verbose command-line option.  If these names are null
 *    pointers, then this function will fail.  However, they can be empty
 *    strings.
 *
 *    This "constructor" also calls the unit_test_status_init() function to
 *    guarantee that all fields in unit_test_status_t have sane values.
 *
 *    The caller can then call the unit_test_status_can_proceed() accessor
 *    function to see if the current test group and test case are properly
 *    initialized and can be run.  Of course, simply checking the return
 *    value is the easiest way to make this check.
 *
 *    The last thing done is to call xpccut_get_microseconds() to log the
 *    start-time of the test.
 *
 * \note
 *    This function calls unit_test_status_init(), and lets that function
 *    check the "this" pointer for this function.
 *
 * \return
 *    Returns 'true' if the function succeeded.  It will fail if the status
 *    pointer (also known as the "this" pointer) is null, among other
 *    things.  However, a 'false' result can also be returned if the test
 *    group and case numbers do not match the --group and --case options.
 *    The caller should check the return value to see if the current test
 *    group and test case should be run.  The caller should not emit any
 *    messages based on the result.
 *
 * \unittests
 *    -  unit_unit_test_02_02()
 */

cbool_t
unit_test_status_initialize
(
   unit_test_status_t * status,     /**< The "this" pointer for this call.    */
   const unit_test_options_t * opt, /**< Global options for the unit-tests.   */
   int testgroup,                   /**< The test's group number, > 0.        */
   int testcase,                    /**< The test's case number, > 0.         */
   const char * groupname,          /**< The test's group name, not nullptr.  */
   const char * casename            /**< The test's case name, not nullptr.   */
)
{
   cbool_t result = unit_test_status_init(status);       /* set sane defaults */
   if (result)
      result = (testgroup > 0) && (testcase > 0);        /* need real values  */

   if (result)
   {
      result = cut_not_nullptr_3(opt, groupname, casename);
      if (result)
      {
         gs_status_prompt_before = unit_test_options_prompt_before(opt);
         gs_status_prompt_after = unit_test_options_prompt_after(opt);
      }
      else
         xpccut_errprint_func(_("null options, or group/case names"));
   }
   else
      xpccut_errprint_func(_("invalid test group or case number"));

   if (result)
   {
      /*
       * This section sets any unit_test_status_t fields that need to be
       * overridden from the values set in the unit_test_status_init() call.
       *
       * Note the xpccut_stringcopy() calls, which take care to prevent any
       * potential buffer overflows, and also allow empty strings to be
       * copied.
       *
       * Setting the disposition to XPCCUT_DISPOSITION_CONTINUE overrides
       * the initial setting of XPCCUT_DISPOSITION_ABORTED.  This value is
       * one that is checked by the unit_test_status_can_proceed() function.
       */

      int optiongroup = unit_test_options_test_group(opt);
      xpccut_stringcopy(status->m_Group_Name, groupname);
      xpccut_stringcopy(status->m_Case_Description, casename);
      status->m_Test_Group          = testgroup;
      status->m_Test_Case           = testcase;
      status->m_Test_Disposition    = XPCCUT_DISPOSITION_CONTINUE;
      status->m_Test_Options        = opt;
      if (optiongroup != 0)
      {
         /*
          * A non-zero integer --group option was specified.  Only this
          * unit-test group will be allowed to run.
          */

         result = testgroup == optiongroup;
      }
      else
      {
         /*
          * If a valid group-name was specified by the --group option, then
          * only a matching unit-test group will be allowed to run.
          */

         const char * named_group = unit_test_options_named_group
         (
            status->m_Test_Options
         );
         if (cut_not_nullptr(named_group))
            result = strcmp(named_group, groupname) == 0;
      }
      if (result)                   /* this unit-test group is allowed to run */
      {
         /*
          * Now it is time to see if the current case in the group is
          * allowed to run.  As with the --group option, the --case option
          * is checked for valid integer or string values.
          */

         int optioncase = unit_test_options_test_case(status->m_Test_Options);
         if (optioncase != 0)
            result = testcase == optioncase;
         else
         {
            const char * named_case = unit_test_options_named_case
            (
               status->m_Test_Options
            );
            if (cut_not_nullptr(named_case))
               result = strcmp(named_case, casename) == 0;
         }
      }
      if (result)                                           /* test can run   */
      {
         if (unit_test_options_show_progress(status->m_Test_Options))
         {
            const char * tag = _("TEST");
            int testnum =
               unit_test_options_current_test(status->m_Test_Options) + 1;

            if (unit_test_options_is_simulated(status->m_Test_Options))
               tag = _("Simulated TEST");

            fprintf(stdout, "\n%s %3d:  \n", tag, testnum);
         }
         if (unit_test_options_is_summary(status->m_Test_Options))
         {
            fprintf
            (
               stdout, "  %s %d '%s', %s %d '%s'\n",
               _("Group"), testgroup, groupname, _("Case"), testcase, casename
            );
         }
         else
            (void) unit_test_status_show_title(status);     /* show test info */
      }
      else
      {
         status->m_Test_Disposition = XPCCUT_DISPOSITION_DNT;
         status->m_Test_Result = true;
         if (unit_test_options_is_verbose(status->m_Test_Options))
         {
            fprintf
            (
               stdout, "  %s %d '%s', %s %d '%s' %s\n",
               _("Group"), testgroup, groupname,
               _("Case"), testcase, casename, _("skipped")
            );
         }
      }
      xpccut_get_microseconds(&status->m_Start_Time_us);   /* always log time */
   }
   return result;
}

/**
 *    Set the disposition of the test to "continue".
 *    This function is used only for unit-testing.  It resets the
 *    disposition of a unit_test_status_t object, so that testing the
 *    disposition 'abort' and 'quit' values can be done without prematurely
 *    terminating the test.
 *
 * \return
 *    Returns 'true' if the function succeeded.  It will fail if the status
 *    pointer is null.
 *
 * \unittests
 *    -  unit_unit_test_02_03()
 */

cbool_t
unit_test_status_reset
(
   unit_test_status_t * status   /**< The "this" pointer for this function.   */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
      status->m_Test_Disposition = XPCCUT_DISPOSITION_CONTINUE;

   return result;
}

/**
 *    This function sets the pass/fail flag for this status object.  It also
 *    increments the error count.
 *
 *    It also provides some indication of where the error occured, if the
 *    --verbose option was set.
 *
 *    The m_Subtest_Error_Count item is incremented, if the status flag is
 *    false.
 *
 *    If the sub-test was the first to fail, it's index is logged so that
 *    the user can find the first error more easily.
 *
 * \return
 *    Returns 'true' if the function succeeded.  It will fail if the status
 *    pointer is null.  Callers should always check the return value.
 *
 * \unittests
 *    -  unit_unit_test_02_04()
 */

cbool_t
unit_test_status_pass
(
   unit_test_status_t * status,  /**< The "this" pointer for this function.   */
   cbool_t flag                  /**< Sets the test's pass/fail status.       */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
   {
      status->m_Test_Result = flag;
      if (! flag)
      {
         status->m_Subtest_Error_Count++;          /* count the error         */
         if (status->m_Failed_Subtest == 0)        /* only log the first one  */
            status->m_Failed_Subtest = status->m_Subtest;

         if (unit_test_options_show_progress(status->m_Test_Options))
         {
            /*
             * In some of the XPCCUT library's own unit tests, the
             * m_Test_Options field is set to nullptr, so that
             * unit_test_options_show_progress() returns the default value
             * of the show-progress flag, which is 'true'.  But, if --silent
             * is in force, we don't want to see it even then.
             */

            if (! xpccut_is_silent())
            {
               const char * tag = _("FAILURE in sub-test");
               if (unit_test_options_is_simulated(status->m_Test_Options))
                  tag = _("FAILURE in simulated sub-test");

               fprintf
               (
                  stdout, "  %s %d ['%s']\n",
                  tag, status->m_Subtest, status->m_Subtest_Name
               );
            }
         }
      }
   }
   return result;
}

/**
 *    This function unsets the pass/fail flag for this status object.
 *
 * \return
 *    Returns 'true' if the function succeeded.  It will return 'false' if
 *    the status pointer is null.
 *
 * \unittests
 *    -  unit_unit_test_02_04()
 */

cbool_t
unit_test_status_fail
(
   unit_test_status_t * status   /**< The "this" pointer for this function.   */
)
{
   return unit_test_status_pass(status, false);
}

/**
 *    This function unsets the pass/fail flag for this status object.
 *    It is a shortcut for "unit_test_status_pass(false)", except that it
 *    also might emit a message about the failure being deliberate, as a
 *    small semantic difference bewteen unit_test_status_fail() and
 *    unit_test_status_fail_deliberately.
 *
 * \return
 *    Returns 'true' if the function succeeded.  It will fail if the status
 *    pointer is null.
 *
 * \unittests
 *    -  unit_unit_test_02_04()
 */

cbool_t
unit_test_status_fail_deliberately
(
   unit_test_status_t * status   /**< The "this" pointer for this function.   */
)
{
   cbool_t result = unit_test_status_fail(status);
   if (result)
   {
      if (unit_test_options_show_progress(status->m_Test_Options))
      {
         /*
          * Some tests of the XPCCUT library itself set m_Test_Options to
          * nullptr, so that the default value of the --show-progress
          * option is returned.  When --silent is in force, we don't want to
          * see \e such messages, ever.
          */

         if (! xpccut_is_silent())
            fprintf(stdout, "! %s.\n", _("This FAILURE is deliberate"));
      }
   }
   return result;
}

/**
 *    Loads the current system time.
 *    The current system time is loaded into status->m_Start_Time_us.  The
 *    status->m_End_Time_us value is set to 0.0 so that forgetting to set the
 *    end time is easier to detect.
 *
 * \note
 *    This is more of value to the caller who wants supplemental time
 *    measurements.  Internally, the unit_test_status_initialize() function
 *    makes both settings itself.
 *
 * \return
 *    Returns 'true' if the "this" parameter is valid.
 *
 * \unittests
 *    -  unit_unit_test_02_05()
 */

cbool_t
unit_test_status_start_timer
(
   unit_test_status_t * status   /**< The "this" pointer for this function.   */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
   {
      status->m_End_Time_us.tv_sec = 0;                    /* nullify end time  */
      status->m_End_Time_us.tv_usec = 0;
      xpccut_get_microseconds(&status->m_Start_Time_us);   /* set start time    */
   }
   return result;
}

/**
 *    This function assumes that the start-time member has been set.  It
 *    first loads the end-time member, and then calculates the time
 *    difference in seconds.
 *
 *    If the reset parameter is true, then the start-time is modified to the
 *    current time, so that the next call to unit_test_status_time_delta()
 *    can yield a reasonable value.
 *
 * \sideeffect
 *    status->m_Test_Duration_ms is set to the result.
 *
 * \return
 *    If the function has valid parameters, then the time difference is
 *    returned.  Otherwise, -1.0 is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_05()
 */

double
unit_test_status_time_delta
(
   unit_test_status_t * status,  /**< The "this" pointer for this function.   */
   cbool_t startreset            /**< If true, reset time to current time.    */
)
{
   double result = -1.0;
   if (xpccut_thisptr(status))
   {
      if
      (
         status->m_Start_Time_us.tv_sec == 0 &&
         status->m_Start_Time_us.tv_usec == 0
      )
      {
         xpccut_errprint("logged unit-test start time was 0");
      }
      else
      {
         xpccut_get_microseconds(&status->m_End_Time_us);
         result = xpccut_time_difference_ms
         (
            status->m_Start_Time_us, status->m_End_Time_us
         );
         if (result >= 0.0)
            status->m_Test_Duration_ms = result;            /* log the result */
         else
            xpccut_errprint_func(_("time-difference < 0.0, left unassigned"));

         if (startreset)
         {
            xpccut_get_microseconds(&status->m_Start_Time_us);    /* new time */
            xpccut_infoprint("unit-test start time reset!");
         }
      }
   }
   return result;
}

/**
 *    This functions shows the group and case information, if the options
 *    have been set to "--verbose".
 *
 * \return
 *    Returns 'true' if the parameter was valid.
 *
 * \unittests
 *    -  unit_unit_test_02_06()
 */

cbool_t
unit_test_status_show_title
(
   unit_test_status_t * status   /**< The "this" pointer for this function.   */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
   {
      if (unit_test_options_is_verbose(status->m_Test_Options))
      {
         const char * groupname = cut_not_nullptr(status->m_Group_Name) ?
               status->m_Group_Name : _("unnamed") ;

         const char * casename = cut_not_nullptr(status->m_Case_Description) ?
               status->m_Case_Description : _("none given") ;

         fprintf
         (
            stdout, "\n  %s(%d, %d) [%s (%s)]\n",
            _("Unit test"),
            status->m_Test_Group, status->m_Test_Case, groupname, casename
         );
      }
   }
   return result;
}

/**
 *    This function returns false if the disposition of the test was
 *    "aborted" or "did not test".  The function is equivalent to
 *
 *       return = ! unit_test_status_is_aborted(status) &&
 *          ! unit_test_status_is_skipped(status)
 *
 *    but those function are meant for external usage.
 *
 * \return
 *    Returns 'true' if the parameter is valid and the test was not aborted
 *    or skipped.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_07()
 */

cbool_t
unit_test_status_can_proceed
(
   unit_test_status_t * status   /**< The "this" pointer for this function.   */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
   {
      result =
      (
         status->m_Test_Disposition != XPCCUT_DISPOSITION_ABORTED &&
         status->m_Test_Disposition != XPCCUT_DISPOSITION_DNT
      );
      if (! result)
         xpccut_infoprint(_("Test is aborted or is to be skipped"));
   }

   return result;
}

/**
 *    This function determines if the test is allowed to pass or fail
 *    without being executed.
 *
 *    At the same time, it has the side effect of passing the test if it is
 *    to be skipped or quit, and of failing the test if it is to be aborted.
 *
 *    Compare and contrast this function with the
 *    unit_test_options_is_interactive() function.
 *
 * \return
 *    Returns 'true' if the parameter is valid, and the test status
 *    indicates that it is skipped, quitted, or aborted.
 *
 * \unittests
 *    -  unit_unit_test_02_08()
 */

cbool_t
unit_test_status_ignore
(
   unit_test_status_t * status   /**< The "this" pointer for this function.   */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
   {
      result = unit_test_status_is_skipped(status);
      if (! result)
         result = unit_test_status_is_quitted(status);

      if (result)
      {
         unit_test_status_pass(status, true);
      }
      else if (unit_test_status_is_aborted(status))
      {
         result = true;                               /* skip test            */
         unit_test_status_fail_deliberately(status);  /* guarantee failure    */
      }
   }
   return result;
}

/**
 *    Sets up for the next unit-test sub-test.
 *    This function loads the name of the next sub-test, and compares the
 *    pending sub-test number to the single-sub-test number.  If there is a
 *    match, or if the single-sub-test number is zero, then the test can be
 *    run, and 'true' is returned.
 *
 *    If the --summarize option is on, this function lists the subtest
 *    features to standard output, and returns 'false'.  The caller has to
 *    cooperate by calling this function (and only this function), and
 *    obeying its return value, and by not nesting subtests.
 *
 * \return
 *    Returns true if the parameters are valid and the sub-test is able and
 *    authorized to run.
 *
 * \unittests
 *    This function is obviously tested indirectly in all of the unit tests,
 *    but this test detects meaningful regressions.
 *    -  unit_unit_test_02_09()
 */

cbool_t
unit_test_status_next_subtest
(
   unit_test_status_t * status,  /**< The "this" pointer for this function.   */
   const char * tag              /**< Describes the sub-test briefly.         */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
   {
      if (cut_is_nullptr(tag) || strlen(tag) < 1)
      {
         tag = _("unnamed");
         if (unit_test_options_show_progress(status->m_Test_Options))
         {
            fprintf
            (
               stdout, "! %s: unit_test_status_next_subtest()\n\n",
               _("empty tag")
            );
         }
      }
      if (unit_test_options_is_summary(status->m_Test_Options))
      {
         status->m_Subtest++;
         xpccut_stringcopy(status->m_Subtest_Name, tag);
         if (! xpccut_is_silent())
         {
            fprintf
            (
               stdout, "  %s %d: '%s'\n", _("Sub-test"), status->m_Subtest, tag
            );
         }
         result = false;
      }
      else
      {
         /**
          * Check to see if the --sub-test option was provided.  If it was,
          * and the next sub-test number will match the provided --sub-test
          * number, then set \e result to true to allow the test to run.
          *
          * \warning
          *    The unit-test developer must make sure that there are no
          *    important test setup actions that are done in sub-tests
          *    previous to the one being selected for execution.  Otherwise,
          *    there's probably no way the single sub-test can ever succeed.
          */

         int singlesubtest = unit_test_options_single_subtest
         (
            status->m_Test_Options
         );
         if (singlesubtest != 0)
         {
            result = (status->m_Subtest + 1) == singlesubtest;
         }
         else
         {
            /**
             * Also check to see if there is a named subtest.  If so, then
             * check that name against the current name.  If they don't
             * match, then do not allow this sub-test to be executed.
             */

            const char * named_subtest = unit_test_options_named_subtest
            (
               status->m_Test_Options
            );
            if (cut_not_nullptr(named_subtest))
               result = strcmp(named_subtest, tag) == 0;
         }
         status->m_Subtest++;
         xpccut_stringcopy(status->m_Subtest_Name, tag);
         if (result)
         {
            if (unit_test_options_show_step_numbers(status->m_Test_Options))
            {
               if (status->m_Subtest == 1)
                  fprintf(stdout, "\n");

               fprintf
               (
                  stdout, "  %s %d: %s\n",
                  _("Sub-test"), status->m_Subtest, status->m_Subtest_Name
               );
            }
         }
         else if (unit_test_options_is_verbose(status->m_Test_Options))
         {
            fprintf
            (
               stdout, "  %s %d (%s) %s\n",
               _("Sub-test"), status->m_Subtest, tag, _("skipped")
            );
         }
      }
   }
   return result;
}

/**
 *    This function implements the basic beep.
 *
 * \win32
 *    A MessageBeep(-1) call is used.
 *
 * \posix
 *    A  Ctrl-G character ("\a") is sent to stdout.
 *
 * \unittests
 *    -  unit_unit_test_02_10()
 */

void
unit_test_status_beep (void)
{
#ifdef WIN32
      MessageBeep((UINT) -1);                /* play the current beep sound   */
#else
      fprintf(stdout, "\a");                 /* play the console alert (BEL)  */
#endif
}

/**
 *    Provides an internal function to access the --verbose option.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the verbose option is
 *    on.  Otherwise, returns 'false'.
 */

static cbool_t
unit_test_status_is_verbose
(
   unit_test_status_t * status   /**< The "this" pointer for this function.   */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
      result = cut_not_nullptr(status->m_Test_Options);

   if (result)
      result = unit_test_options_is_verbose(status->m_Test_Options);

   return result;
}

/**
 *    This function shows a string on standard output and then accepts a
 *    response from the user.  The following response are supported:
 *
 *    -  (c)ontinue     Continue with the unit-test.
 *    -  (s)kip         Skip this unit-test.  Do not run it.
 *    -  (a)bort        Stop the unit-test application, indicating failure.
 *    -  (q)uit         Stop the unit-test application, indicating success.
 *
 *    If --silent is in force, the prompt is skipped, and the response is
 *    as if the user passed the test;  [hence, at present, --silent is
 *    equivalent to --batch.]
 *
 * \note
 *    -# On Windows, the first two don't return until a non-Enter key is
 *       struck.  The third one, getchar() will accept an Enter keystroke by
 *       itself, but it also requires an Enter keystroke if another character
 *       is entered.  So we loop to ignore newline characters.
 *       TODO:  see if we can unbuffer the console input here.
 *    -# If the tests are run non-interactively, then basically we simply
 *       want to skip the test.  The prompt() function returns the "did not
 *       test" disposition, which should be tested for.  The
 *       unit_test_status_interactive() function provides an easy way to
 *       make this test.
 *    -# The disposition is also copied into the member variable, so that
 *       this status object can be analyzed later by the dispose() function.
 *       This side-effect is very important.  See the unit_test_t::run()
 *       function for the usage of this side-effect.
 *
 * \return
 *    If the parameters are valid, the disposition selected by the user is
 *    returned.  Otherwise, the value XPCCUT_DISPOSITION_ABORTED is
 *    returned.
 *
 * \unittests
 *    -  Indirectly tested by unit_unit_test_02_10()
 */

static unit_test_disposition_t
unit_test_status_prompt_before
(
   unit_test_status_t * status,  /**< The "this" pointer for this function.   */
   const char * message          /**< Optional message to use for the prompt. */
)
{
   unit_test_disposition_t result = XPCCUT_DISPOSITION_ABORTED;
   cbool_t ok = xpccut_thisptr(status);
   if (ok)
   {
      cbool_t no_response;
      const char * prompt_string =
      _(
         "For this test [(c)ontinue, (s)kip, (q)uit, (a)bort, (h)elp]"
       );
      if (unit_test_options_do_beep(status->m_Test_Options))
         unit_test_status_beep();

      do
      {
         char r;
         no_response = false;
         if (! unit_test_options_batch_mode(status->m_Test_Options))
         {
            if (cut_is_nullptr(message) || strlen(message) == 0)
               fprintf(stdout, "\n%s ", prompt_string);
            else
               fprintf(stdout, "\n%s:\n%s ", message, prompt_string);
         }
         if (gs_status_prompt_before == 0)
         {
            r = xpccut_get_response();
         }
         else
         {
            r = gs_status_prompt_before;  /* use --response-before value      */
            if (! unit_test_options_batch_mode(status->m_Test_Options))
            {
               fprintf
               (
                  stdout, "\n(%s %c)\n", _("Responding automatically with"), r
               );
            }
         }
         switch (r)
         {
            case 'c': case 'C': case '\x00':

               result = XPCCUT_DISPOSITION_CONTINUE;
               if (unit_test_status_is_verbose(status))
                  fprintf(stdout, "%s...\n", _("Continuing"));
               break;

            case 's': case 'S':

               result = XPCCUT_DISPOSITION_DNT;
               if (unit_test_status_is_verbose(status))
                  fprintf(stdout, "%s...\n", _("Skipping"));
               break;

            case 'q': case 'Q':

               result = XPCCUT_DISPOSITION_QUITTED;
               if (unit_test_status_is_verbose(status))
                  fprintf(stdout, "%s...\n", _("Quitting"));
               break;

            case 'a': case 'A':

               result = XPCCUT_DISPOSITION_ABORTED;
               if (unit_test_status_is_verbose(status))
                  fprintf(stdout, "%s...\n", _("Aborting"));
               break;

            case 'h': case 'H': case '?':


               fprintf
               (
                  stdout,
                  "\n%s\n%s\n%s\n%s\n",
               /* TRANSLATORS: line up the 2nd column of the next 4 strings. */
   _("Continue:  Go ahead and perform the upcoming test."),
   _("Skip:      Do not perform the test.  Treat it as passed."),
   _("Quit:      Do not perform any more tests.  Treat this test as passed."),
   _("Abort:     Do not perform any more tests.  Treat this test as failed.")
               );
               no_response = true;
               break;

            default:

               /*
                * This case should just be ignored.
                *
                * fprintf(stdout, "=== CHAR 0x%x ===\n", (int) r);
                */

               no_response = true;
               break;
         }
         if (r == '\x00')
            break;

      } while (no_response);
   }
   return result;
}

/**
 *    This function shows a string on standard output and then accepts a
 *    response from the user.  The following response are supported:
 *
 *    -  (p)ass         Mark the just-performed unit-test as passed, then
 *                      continue with the next unit-test.
 *    -  (f)ail         Mark the just-performed unit-test as failed, then
 *                      continue with the next unit-test.
 *    -  (a)bort        Stop the unit-test application, indicating failure.
 *    -  (q)uit         Stop the unit-test application, indicating success.
 *
 * \return
 *    If the parameters are valid, the disposition selected by the user is
 *    returned.  Otherwise, the value XPCCUT_DISPOSITION_ABORTED is
 *    returned.
 *
 * \unittests
 *    -  Indirectly tested by unit_unit_test_02_10()
 */

static unit_test_disposition_t
unit_test_status_prompt_after
(
   unit_test_status_t * status,  /**< The "this" pointer for this function.   */
   const char * message          /**< Optional message to use for the prompt. */
)
{
   unit_test_disposition_t result = XPCCUT_DISPOSITION_ABORTED;
   cbool_t ok = xpccut_thisptr(status);
   if (ok)
   {
      cbool_t no_response;
      const char * prompt_string =
            _("Disposition [(p)ass, (f)ail, (q)uit, (a)bort, (h)elp]");

      if (unit_test_options_do_beep(status->m_Test_Options))
      {
         if (! xpccut_is_silent())
            unit_test_status_beep();
      }
      do
      {
         char r;
         no_response = false;
         if (! unit_test_options_batch_mode(status->m_Test_Options))
         {
            if (cut_is_nullptr(message) || strlen(message) == 0)
               fprintf(stdout, "\n%s ", prompt_string);
            else
               fprintf(stdout, "\n%s:\n%s ", message, prompt_string);
         }
         if (gs_status_prompt_after == 0)
         {
            r = xpccut_get_response();
         }
         else
         {
            r = gs_status_prompt_after;   /* use --response-after value       */
            if (! unit_test_options_batch_mode(status->m_Test_Options))
            {
               fprintf
               (
                  stdout, "\n(%s %c)\n", _("Responding automatically with"), r
               );
            }
         }
         switch (r)
         {
            case 'p': case 'P': case '\x00':

               result = XPCCUT_DISPOSITION_CONTINUE;
               if (unit_test_status_is_verbose(status))
                  fprintf(stdout, "%s.\n", _("Passed"));
               break;

            case 'f': case 'F':

               result = XPCCUT_DISPOSITION_FAILED;
               if (unit_test_status_is_verbose(status))
                  fprintf(stdout, "%s.\n", _("Failed"));
               break;

            case 'q': case 'Q':

               result = XPCCUT_DISPOSITION_QUITTED;
               if (unit_test_status_is_verbose(status))
                  fprintf(stdout, "%s...\n", _("Quitting"));
               break;

            case 'a': case 'A':

               result = XPCCUT_DISPOSITION_ABORTED;
               if (unit_test_status_is_verbose(status))
                  fprintf(stdout, "%s...\n", _("Aborting"));
               break;

            case 'h': case 'H': case '?':

               fprintf
               (
                  stdout,
                  "\n%s\n%s\n%s\n%s\n",
               /* TRANSLATORS: line up the 2nd column of the next 4 strings. */
   _("Pass:      Indicate that the test has passed."),
   _("Fail:      Indicate that the test has failed."),
   _("Quit:      Treat this test as passed, and end the unit-testing."),
   _("Abort:     Treat this test as failed, and end the unit-testing.")
               );
               no_response = true;
               break;

            default:

               /*
                * This case should never happen.
                *
                * fprintf(stdout, "=== CHAR 0x%x ===\n", (int) r);
                */

               no_response = true;
               break;
         }
      } while (no_response);
   }
   return result;
}

/**
 *    Prints a message, and waits for a user's response before performing the
 *    next unit-test.  Call this function only for unit-tests that require
 *    some kind of user interaction \e before they can execute.
 *
 *    This function and user_test_status_response() provides responses based
 *    on the settings of unit_test_options_batch_mode() and
 *    unit_test_status_is_interactive(), as well as the responses of the
 *    user, if those two functions allow user interaction.
 *
 *    If interactivity is not allowed, unit_test_status_prompt()
 *    returns false, meaning the test should not be run.  In this case, the
 *    user never even sees the prompt, and the test is skipped.
 *
 *    If interactivity is allowed, then the user is given the prompt and
 *    various options.  If the user decides to continue or pass the test,
 *    then unit_test_status_prompt() returns true.
 *
 *    If the test is skipped, then the disposition of the the test is also
 *    checked.  If the test was not marked as failed or aborted, then the
 *    test is marked as passed.  The philosophy is that tests that are
 *    skipped due to some kind of silent/non-interactive/batch mode are
 *    considered to be passed.
 *
 *    We may need to consider if that is true.  Is it wrong to run a
 *    partly-interactive unit-test in batch mode?
 *
 *    Compare and contrast this function with the unit_test_status_ignore()
 *    function.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the test is able and
 *    authorized to be performed.
 *
 * \sideeffect
 *    The pass/fail status of the test is evaluated and modified in the \a
 *    status parameter's object.
 *
 * \unittests
 *    -  unit_unit_test_02_10()
 */

cbool_t
unit_test_status_prompt
(
   unit_test_status_t * status,  /**< The "this" pointer for this function.   */
   const char * message          /**< Optional message to use for the prompt. */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
   {
      unit_test_disposition_t disposition;
      result = unit_test_options_is_interactive(status->m_Test_Options);
      if (result)
      {
         disposition = unit_test_status_prompt_before(status, message);
         if (disposition != XPCCUT_DISPOSITION_CONTINUE)
            result = false;
      }
      else
         disposition = XPCCUT_DISPOSITION_DNT;

      status->m_Test_Disposition = disposition;
      if (! result)
      {
         unit_test_status_pass
         (
            status,
            ! unit_test_status_is_failed(status) &&
            ! unit_test_status_is_aborted(status)
         );
      }
   }
   return result;
}

/**
 *    Prints a message, and waits for a user's response before evaluating
 *    the current unit-test.
 *
 *    Call this function only after unit-tests that require some kind of user
 *    interaction \e before they can be determined to be successful or not.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the response indicated
 *    some kind of success.
 *
 * \sideeffect
 *    The pass/fail status of the test is evaluated and modified in the \a
 *    status parameter's object.
 *
 * \unittests
 *    -  unit_unit_test_02_10()
 */

cbool_t
unit_test_status_response
(
   unit_test_status_t * status,  /**< The "this" pointer for this function.   */
   const char * message          /**< Optional message to use for the prompt. */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
   {
      unit_test_disposition_t disposition;
      result = unit_test_options_is_interactive(status->m_Test_Options);
      if (result)
      {
         disposition = unit_test_status_prompt_after(status, message);
         if (disposition != XPCCUT_DISPOSITION_CONTINUE)
            result = false;
      }
      else
         disposition = XPCCUT_DISPOSITION_DNT;

      status->m_Test_Disposition = disposition;
      if (result)
      {
         xpccut_infoprint(_("User indicates test succeeded"));
      }
      else
      {
         cbool_t ok = ! unit_test_status_is_failed(status);
         if (ok)
         {
            ok = ! unit_test_status_is_aborted(status);
            if (ok)
               xpccut_infoprint(_("User quits, but passes this test"));
            else
               xpccut_infoprint(_("User indicates test aborted"));
         }
         else
            xpccut_infoprint(_("User indicates test failed"));

         unit_test_status_pass(status, ok);
      }
   }
   return result;
}

/**
 *    Returns the value of the m_Group_Name field.
 *
 * \note
 *    There is no "setter" function for this field.  Instead, the
 *    unit_test_status_initialize() function allows this field (and others)
 *    to be set at the beginning of a unit-test function.
 *
 * \return
 *    Returns the status->m_Group_Name field if the \a status pointer
 *    is valid.  Otherwise, a null pointer value (nullptr) is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_11()
 */

const char *
unit_test_status_group_name
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   const char * result = nullptr;
   if (xpccut_thisptr(status))
      result = status->m_Group_Name;

   return result;
}

/**
 *    Returns the value of the m_Case_Description field.
 *
 * \note
 *    There is no "setter" function for this field.  Instead, the
 *    unit_test_status_initialize() function allows this field (and others)
 *    to be set at the beginning of a unit-test function.
 *
 * \return
 *    Returns the status->m_Case_Description field if the \a status pointer
 *    is valid.  Otherwise, a null pointer value (nullptr) is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_12()
 */

const char *
unit_test_status_case_name
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   const char * result = nullptr;
   if (xpccut_thisptr(status))
      result = status->m_Case_Description;

   return result;
}

/**
 *    Returns the value of the m_Subtest_Name field.
 *
 * \note
 *    There is no "setter" function for this field.  Instead, the
 *    unit_test_status_initialize() function allows this field (and others)
 *    to be set at the beginning of a unit-test function.
 *
 * \return
 *    Returns the status->m_Subtest_Name field if the \a status pointer
 *    is valid.  Otherwise, a null pointer value (nullptr) is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_13()
 */

const char *
unit_test_status_subtest_name
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   const char * result = nullptr;
   if (xpccut_thisptr(status))
      result = status->m_Subtest_Name;

   return result;
}

/**
 *    Returns the value of the m_Test_Group index.
 *
 *    This value is 0 if there is no test-group in force.  Otherwise, group
 *    numbers start from 1 and go upward for as many test groups as the
 *    developer defines for the unit-test application.
 *
 *    Conceptually, a "group" is nothing more that a set of related
 *    unit-test functions.  Each function in this group is a test \e case.
 *    The functions can have any name, but, for clarity, a good convention
 *    is indicated by the following examples:
 *
 *       -  my_unit_test_01_01():  Group 1, Case 1.
 *       -  my_unit_test_01_02():  Group 1, Case 2.
 *       -  my_unit_test_02_01():  Group 2, Case 1.
 *       -  my_unit_test_02_02():  Group 2, Case 2.
 *       -  my_unit_test_02_03():  Group 2, Case 3.
 *       -  sqrt_unit_test_02_03():  Group 2, Case 3.
 *
 *    Also see the unit_test_status_subtest() function for a description of
 *    the concept of the sub-test.
 *
 * \note
 *    There is no "setter" function for this field.  Instead, the
 *    unit_test_status_initialize() function allows this field (and others)
 *    to be set at the beginning of a unit-test function.
 *
 * \return
 *    Returns the value of status->m_Test_Group if the \a status pointer
 *    is valid.  Otherwise, -1 (XPCCUT_INVALID_PARAMETER) is returned, which
 *    indicates an invalid pointer parameter or an invalid group.
 *
 * \unittests
 *    -  unit_unit_test_02_14()
 */

int
unit_test_status_group
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   int result = XPCCUT_INVALID_PARAMETER;    /* an invalid index number       */
   if (xpccut_thisptr(status))
      result = status->m_Test_Group;

   return result;
}

/**
 *    Returns the value of the m_Test_Case field.
 *    See the unit_test_status_group() function for more information about
 *    test groups and test cases.
 *
 * \note
 *    There is no "setter" function for this field.  Instead, the
 *    unit_test_status_initialize() function allows this field (and others)
 *    to be set at the beginning of a unit-test function.
 *
 * \return
 *    Returns the value of status->m_Test_Case if the \a status pointer
 *    is valid.  Otherwise, -1 (XPCCUT_INVALID_PARAMETER) is returned, which
 *    indicates an invalid pointer parameter or an invalid case.
 *
 * \unittests
 *    -  unit_unit_test_02_15()
 */

int
unit_test_status_case
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   int result = XPCCUT_INVALID_PARAMETER;    /* an invalid index number       */
   if (xpccut_thisptr(status))
      result = status->m_Test_Case;

   return result;
}

/**
 *    Returns the value of the m_Subtest field.
 *    See the unit_test_status_group() function for more information about
 *    test groups and test cases.
 *
 *    In addition, each function can be divided into sub-tests.  Sub-tests
 *    are minor variations on the tests for a given application or library
 *    function.  For example, the unit-test for a square-root function would
 *    include subtests for zero values, negative values, well-known
 *    difficult-to-calculate values, or even for +INF, -INF, and NaN.
 *
 *    Each subtest should be introduced by a test of the return value of
 *    unit_test_status_next_subtest().  That function allows the processing
 *    of options that could cause the unit-test application to terminate
 *    early (for example).
 *
 * \note
 *    There is no "setter" function for this field.  Instead, the
 *    unit_test_status_initialize() function allows this field (and others)
 *    to be set at the beginning of a unit-test function.
 *
 * \return
 *    Returns the value of status->m_Subtest if the \a status pointer
 *    is valid.  Otherwise, -1 is (XPCCUT_INVALID_PARAMETER) returned, which
 *    indicates an invalid pointer parameter or an invalid case.
 *
 * \unittests
 *    -  unit_unit_test_02_16()
 */

int
unit_test_status_subtest
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   int result = XPCCUT_INVALID_PARAMETER;    /* an invalid index number       */
   if (xpccut_thisptr(status))
      result = status->m_Subtest;

   return result;
}

/**
 *    Returns the value of the m_Failed_Subtest field, which indicates the
 *    value of the first sub-test that failed.
 *
 * \note
 *    There is no "setter" function for this field.  Instead, it is set
 *    directly during the course of analyzing test results.
 *
 * \return
 *    Returns the value of status->m_Failed_Subtest if the \a status pointer
 *    is valid.  Otherwise, -1 is (XPCCUT_INVALID_PARAMETER) returned, which
 *    indicates an invalid pointer parameter or an invalid sub-test.
 *
 * \unittests
 *    -  unit_unit_test_02_17()
 */

int
unit_test_status_failed_subtest
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   int result = XPCCUT_INVALID_PARAMETER;       /* an invalid index number    */
   if (xpccut_thisptr(status))
      result = status->m_Failed_Subtest;

   return result;
}

/**
 *    Returns the value of the m_Subtest_Error_Count field.
 *
 * \note
 *    There is no "setter" function for this field.  Instead, it is set
 *    directly during the course of analyzing test results.
 *
 * \return
 *    Returns the status->m_Subtest_Error_Count field if the "this"
 *    parameter is valid.  Otherwise, -1 is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_18()
 */

int
unit_test_status_error_count
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   int result = XPCCUT_INVALID_PARAMETER;       /* an invalid error count     */
   if (xpccut_thisptr(status))
      result = status->m_Subtest_Error_Count;

   return result;
}

/**
 *    Returns the result of the current unit-test function.
 *
 * \note
 *    There is no "setter" function for this field.  Instead, the caller
 *    uses the unit_test_status_pass() or unit_test_status_fail() functions
 *    to affect this field.
 *
 * \return
 *    Returns 'true' if the \a status parameter is valid and the
 *    status->m_Subtest_Error_Count value is 0.  Otherwise, 'false' is
 *    returned.
 *
 * \unittests
 *    -  unit_unit_test_02_19()
 */

cbool_t
unit_test_status_passed
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   cbool_t result = false;
   if (xpccut_thisptr(status))
   {
      /*
       * This was the old code.  However, m_Test_Result is really just
       * the result of the last sub-test.  The true status of the current
       * unit-test function is actually given by m_Subtest_Error_Count.
       *
       * result = status->m_Test_Result;
       * if (result)
       * {
       *    result = status->m_Subtest_Error_Count == 0;
       *    if (! result)
       *       xpccut_errprint_func(_("test passed, but error count > 0"));
       * }
       * else
       * {
       *    if (status->m_Subtest_Error_Count == 0)
       *       xpccut_errprint_func(_("test failed, but error count == 0"));
       * }
       *
       */

      result = status->m_Subtest_Error_Count == 0;
      if (status->m_Subtest_Error_Count < 0)
         xpccut_errprint_func(_("sub-test error count < 0"));
   }
   return result;
}

/**
 *    Returns the failure-status of the current unit-test function.
 *
 *    This function simply returns the negation of
 *    unit_test_status_passed().
 *
 * \return
 *    Returns 'true' if the "this" parameter is invalid, or if the unit-test
 *    experienced some errors in sub-tests.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_20()
 */

cbool_t
unit_test_status_failed
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   return ! unit_test_status_passed(status);
}

/**
 *    Returns the value of the m_Test_Disposition field.
 *    This indicates if the test was quit, aborted with failure, or allowed
 *    to continue.
 *
 * \return
 *    Return's the value of status->m_Test_Disposition if the "this"
 *    parameter is valid.  Otherwise, XPCCUT_DISPOSITION_ABORTED is
 *    returned.
 *
 * \unittests
 *    -  unit_unit_test_02_21()
 */

unit_test_disposition_t
unit_test_status_disposition
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   unit_test_disposition_t result = XPCCUT_DISPOSITION_ABORTED;
   if (xpccut_thisptr(status))
      result = status->m_Test_Disposition;

   return result;
}

/**
 *    Returns the status of the ability to continue with the testing.
 *
 * \return
 *    Returns 'true'  if the parameter is valid, and the
 *    status->m_Test_Disposition field is XPCCUT_DISPOSITION_CONTINUE.
 *    Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_22()
 */

cbool_t
unit_test_status_is_continue
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   cbool_t result = false;
   if (xpccut_thisptr(status))
      result = status->m_Test_Disposition == XPCCUT_DISPOSITION_CONTINUE;

   return result;
}

/**
 *    Returns the status of the skipping of the current unit-test function.
 *
 * \return
 *    Returns 'true'  if the parameter is valid, and the
 *    status->m_Test_Disposition field is XPCCUT_DISPOSITION_DNT.
 *    Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_23()
 */

cbool_t
unit_test_status_is_skipped
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   cbool_t result = false;
   if (xpccut_thisptr(status))
      result = status->m_Test_Disposition == XPCCUT_DISPOSITION_DNT;

   return result;
}

/**
 *    Returns the status of the deliberate failing of the current unit-test
 *    function.
 *
 * \return
 *    Returns 'true'  if the parameter is valid, and the
 *    status->m_Test_Disposition field is XPCCUT_DISPOSITION_FAILED.
 *    Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_24()
 */

cbool_t
unit_test_status_is_failed
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   cbool_t result = false;
   if (xpccut_thisptr(status))
      result = status->m_Test_Disposition == XPCCUT_DISPOSITION_FAILED;

   return result;
}

/**
 *    Returns the status of the deliberate quitting of the current unit-test
 *    function.
 *
 *    Quitting a unit-test means that the unit-test application will halt,
 *    but the current unit-test passes the test anyway.
 *
 * \return
 *    Returns 'true'  if the parameter is valid, and the
 *    status->m_Test_Disposition field is XPCCUT_DISPOSITION_QUITTED.
 *    Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_25()
 */

cbool_t
unit_test_status_is_quitted
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   cbool_t result = false;
   if (xpccut_thisptr(status))
      result = status->m_Test_Disposition == XPCCUT_DISPOSITION_QUITTED;

   return result;
}

/**
 *    Returns the status of the deliberate aborting of the current unit-test
 *    function.
 *
 *    Aborting a unit-test is the same as quitting a unit-test, except that
 *    the result of the test is failure.
 *
 * \return
 *    Returns 'true'  if the parameter is valid, and the
 *    status->m_Test_Disposition field is XPCCUT_DISPOSITION_ABORTED.
 *    Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_26()
 */

cbool_t
unit_test_status_is_aborted
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   cbool_t result = false;
   if (xpccut_thisptr(status))
      result = status->m_Test_Disposition == XPCCUT_DISPOSITION_ABORTED;

   return result;
}

/**
 *    Returns the "okay" status of the current unit-test, which means that
 *    the unit-test is allowed to continue, or is merely skipped.
 *
 *    Does XPCCUT_DISPOSITION_QUITTED need to be included?
 *
 * \return
 *    Returns 'true'  if the parameter is valid, and the
 *    status->m_Test_Disposition field is either XPCCUT_DISPOSITION_CONTINUE
 *    or XPCCUT_DISPOSITION_DNT.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_27()
 */

cbool_t
unit_test_status_is_okay
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   cbool_t result = false;
   if (xpccut_thisptr(status))
      result = (status->m_Test_Disposition == XPCCUT_DISPOSITION_CONTINUE) ||
         (status->m_Test_Disposition == XPCCUT_DISPOSITION_DNT);

   return result;
}

/**
 *    Returns the value of the m_Test_Duration_ms field.
 *
 * \return
 *    Returns the duration value, status->m_Test_Duration_ms if the "this"
 *    parameter is valid.  Otherwise, -1.0 is returned.
 *
 * \unittests
 *    -  unit_unit_test_02_28()
 */

double
unit_test_status_duration_ms
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   double result = -1.0;
   if (xpccut_thisptr(status))
      result = status->m_Test_Duration_ms;

   return result;
}

/**
 *    This function sets the pass/fail flag for this status object based on
 *    the comparison of two integer parameters.
 *
 *    It compares the two integers, emits an error message, and then passes
 *    the result to unit_test_status_pass().
 *
 *    Thus, this function can be used in lieu of unit_test_status_pass().
 *
 * \note
 *    -  The caller has the option of handling the checks without this
 *       function, and then calling unit_test_status_pass() at then end of
 *       all of the comparisons.  The benefit is a little extra speed, at
 *       the expense of losing some informative error messages.
 *    -  The comparison is made before the status pointer is checked; we
 *       allow unit_test_status_pass() to check this pointer.
 *
 * \return
 *    Returns 'true' if the comparison succeeded.  It will fail if the
 *    status pointer is null.
 *
 * \unittests
 *    -  unit_unit_test_02_29()
 */

cbool_t
unit_test_status_int_check
(
   unit_test_status_t * status,  /**< The "this" pointer for this function.   */
   int expected_value,           /**< The target value for the comparison.    */
   int actual_value              /**< The result obtained during testing.     */
)
{
   cbool_t flag = (actual_value == expected_value);
   cbool_t result = unit_test_status_pass(status, flag);
   if (! flag)
   {
      if (! xpccut_is_silent())
      {
         fprintf
         (
            stdout, "? %d %s, %d %s\n",
            expected_value, _("expected"), actual_value, _("actual")
         );
      }
      result = false;            /* whether 'status' valid or not, must fail  */
   }
   return result;
}

/**
 *    This function sets the pass/fail flag for this status object based on
 *    the comparison of two character-pointer parameters.
 *    It compares the two strings, emits an error message, and then passes
 *    the result to unit_test_status_pass().
 *
 *    Thus, this function can be used in lieu of unit_test_status_pass().
 *
 * \note
 *    -  If both pointers are null, then they are considered a good match.
 *       This behavior may be tricky, but allows testing for null-pointer
 *       results.
 *    -  The caller has the option of handling the checks without this
 *       function, and then calling unit_test_status_pass() at then end of
 *       all of the comparisons.  The benefit is a little extra speed, at
 *       the expense of losing some informative error messages.
 *    -  The comparison is made before the status pointer is checked; we
 *       allow unit_test_status_pass() to check this pointer.
 *
 * \return
 *    Returns 'true' if the comparison succeeded.  It will fail if the
 *    status pointer is null, or if only one of the value pointers is null.
 *
 * \unittests
 *    -  unit_unit_test_02_30()
 */

cbool_t
unit_test_status_string_check
(
   unit_test_status_t * status,  /**< The "this" pointer for this function.   */
   const char * expected_value,  /**< The target value for the comparison.    */
   const char * actual_value     /**< The result obtained during testing.     */
)
{
   cbool_t flag;
   cbool_t result;
   if (cut_is_nullptr(expected_value))
      flag = cut_is_nullptr(actual_value);
   else
   {
      if (cut_is_nullptr(actual_value))
         flag = false;
      else
         flag = strcmp(actual_value, expected_value) == 0;
   }
   result = unit_test_status_pass(status, flag);
   if (! flag)
   {
      if (! xpccut_is_silent())
      {
         if (cut_is_nullptr(expected_value))
            expected_value = _("null pointer");

         if (cut_is_nullptr(actual_value))
            actual_value = _("null pointer");

         fprintf
         (
            stdout, "? '%s' %s, '%s' %s\n",
            expected_value, _("expected"), actual_value, _("actual")
         );
      }
      result = false;            /* whether 'status' valid or not, must fail  */
   }
   return result;
}

/**
 *    This function sets the pass/fail flag for this status object based on
 *    the comparison of two boolean parameters.
 *
 *    It compares the two boolean, emits an error message, and then passes
 *    the result to unit_test_status_pass().
 *
 *    Thus, this function can be used in lieu of unit_test_status_pass().
 *
 * \return
 *    Returns 'true' if the comparison succeeded.  It will fail if the
 *    status pointer is null.
 *
 * \unittests
 *    -  unit_unit_test_02_31()
 */

cbool_t
unit_test_status_bool_check
(
   unit_test_status_t * status,  /**< The "this" pointer for this function.   */
   cbool_t expected_value,       /**< The target value for the comparison.    */
   cbool_t actual_value          /**< The result obtained during testing.     */
)
{
   cbool_t flag = (actual_value == expected_value);
   cbool_t result = unit_test_status_pass(status, flag);
   if (! flag)
   {
      if (! xpccut_is_silent())
      {
         fprintf
         (
            stdout, "? %s %s, %s %s\n",
            expected_value ? _("true") : _("false"), _("expected"),
            actual_value ? _("true") : _("false"), _("actual")
         );
      }
      result = false;            /* whether 'status' valid or not, must fail  */
   }
   return result;
}

/**
 *    This function allows for internal testing of the error-count value.
 *
 *    Although the C version of the unit-test can access this member
 *    directly, we don't want the C++ code to do this directly.  Instead, a
 *    function is provided that makes it obvious that we're using a back
 *    door here.
 *
 *    Direct access of m_Subtest_Error_Count in C++ is possible:
 *
 *       xpc::cut_status mystatus;
 *       mystatus.status().m_Status.m_Subtest_Error_Count--;   // naughty!
 *
 *    But this is better:
 *
 *       mystatus.self_test_error_count_decrement();
 *
 *    In C:
 *
 *       mystatus.m_Subtest_Error_Count--
 *
 *    Versus:
 *
 *       (void) status_self_test_error_count_decrement(&mystatus);
 *
 * \return
 *    Returns 'true' if the parameter was valid.  Otherwise, 'false' is
 *    returned.
 *
 * \unittests
 *    -  N/A.  This function is not meant to be used anywhere except in the
 *
 */

cbool_t
status_self_test_error_count_decrement
(
   unit_test_status_t * status   /**< The "this" pointer for this function.   */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
      status->m_Subtest_Error_Count--;

   return result;
}

/**
 *    This function allows for internal testing of the failed-subtest value.
 *    See status_self_test_decrement_error_count() for the rational of this
 *    function.
 *
 * \return
 *    Returns 'true' if the parameter was valid.  Otherwise, 'false' is
 *    returned.
 *
 * \unittests
 *    -  N/A.  This function is not meant to be used anywhere except in the
 *       unit-test of the XPCCUT library itself.
 */

cbool_t
status_self_test_failed_subtest_set
(
   unit_test_status_t * status,  /**< The "this" pointer for this function.   */
   int value                     /**< Value to set m_Failed_Subtest to.       */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result)
      status->m_Failed_Subtest = value;

   return result;
}

/**
 *    Shows the values of the given unit_test_status_t object.
 *    See unit_test_status_trace() for a deeper discussion.
 *
 * \return
 *    Returns 'true' if the function succeeded.  It will fail if the "this"
 *    pointer is null.
 *
 * \unittests
 *    -  None.  This function is actually used mainly for debugging.
 */

cbool_t
unit_test_status_show
(
   const unit_test_status_t * status   /**< "this" pointer for this function. */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result && ! xpccut_is_silent())
   {
      fprintf
      (
         stdout,
         "- unit_test_status_t:\n"
         "-    m_Test_Options:           %p\n"
         "-    m_Group_Name:             %s\n"
         "-    m_Case_Description:       %s\n"
         "-    m_Subtest_Name:           %s\n"
         "-    m_Test_Group:             %d\n"
         "-    m_Test_Case:              %d\n"
         "-    m_Subtest:                %d\n"
         "-    m_Test_Result:            %s\n"
         "-    m_Subtest_Error_Count:    %d\n"
         "-    m_Failed_Subtest:         %d\n"
         "-    m_Test_Disposition:       %d\n"
         "-    m_Start_Time_us.tv_sec:   %d\n"
         "-    m_Start_Time_us.tv_usec:  %d\n"
         "-    m_End_Time_us.tv_sec:     %d\n"
         "-    m_End_Time_us.tv_usec:    %d\n"
         "-    m_Test_Duration_ms:       %g\n"
         ,
         (void *) status->m_Test_Options,
         status->m_Group_Name,
         status->m_Case_Description,
         status->m_Subtest_Name,
         status->m_Test_Group,
         status->m_Test_Case,
         status->m_Subtest,
         status->m_Test_Result ? _("true") : _("false"),
         status->m_Subtest_Error_Count,
         status->m_Failed_Subtest,
         status->m_Test_Disposition,
         (int) status->m_Start_Time_us.tv_sec,
         (int) status->m_Start_Time_us.tv_usec,
         (int) status->m_End_Time_us.tv_sec,
         (int) status->m_End_Time_us.tv_usec,
         status->m_Test_Duration_ms
      );
   }
   return result;
}

/**
 *    Shows some the values of the given unit_test_status_t object.
 *    This function is an alternative to unit_test_status_show().  It shows
 *    much less output, plus the 'this' pointer, and thus is more suitable
 *    for usage in a console-based debugger.
 *
 * \note
 *    This function was created while trying to get an --interactive test to
 *    work in the C++ wrapper for this library.  We would deliberately
 *    create an error, it would be found, and yet the unit-test application
 *    passed.  While debugging, we were seeing some pretty strange effects
 *    under the gdb debugger.  Therefore, we needed a non-invasive (and
 *    fast) way to see what was happening to the status values.
 *
 * \return
 *    Returns 'true' if the function succeeded.  It will fail if the "this"
 *    pointer is null.
 *
 * \unittests
 *    -  None.  This function is actually used mainly for debugging.
 */

cbool_t
unit_test_status_trace
(
   const unit_test_status_t * status,  /**< "this" pointer for this function. */
   const char * context                /**< Says where the call was made.     */
)
{
   cbool_t result = xpccut_thisptr(status);
   if (result && ! xpccut_is_silent())
   {
      if (cut_not_nullptr(context))
         fprintf(stdout, "- %s: %s\n", _("Context"), context);

      fprintf
      (
         stdout,
         "- unit_test_status_t partial settings:\n"
         "-    'this' pointer:           %p\n"
         "-    'options' pointer:        %p\n"
         "-    Group, case, & sub-test:  '%s', '%s', & '%s'\n"
         "-    Subtest error count:      %d\n"
         ,
         (void *) status,
         (void *) status->m_Test_Options,
         status->m_Group_Name,
         status->m_Case_Description,
         status->m_Subtest_Name,
         status->m_Subtest_Error_Count
      );
   }
   return result;
}

/*
 * unit_test_status.c
 *
 * vim: ts=3 sw=3 et ft=c
 */
