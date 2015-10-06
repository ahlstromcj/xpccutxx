/**
 * \file          unit_test_test.c
 * \library       libxpccut
 * \author        Chris Ahlstrom
 * \date          2008-03-14
 * \updates       2015-10-05
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a few unit tests of the xpccut library
 *    module.  It provides a demonstration of how to use the XPCCUT facility.
 *
 *    This module provides a small amount of testing to demonstrate and
 *    prove the very flexible unit-test functionality of the unit-test class
 *    and its related classes.
 *
 * Groups:
 *
 *    -# Overall high-level integrative test
 *    -# unit_test_status_t unit-tests
 *    -# unit_test_options_t unit-tests
 *    -# unit_test_t unit-tests
 *    -# Miscellaneous tests.
 *    -# Recapitulation of the first group's first unit test.
 *
 *    To get a complete list of all of the tests, run this unit-test
 *    application with the --summarize option.
 *
 * \note
 *    -  One interesting thing to note about each unit-test functionis that
 *       each is \e static.  By declaring each unit-test to be a static
 *       function, this allows the compiler to flag a warning if the test is
 *       not used.  This normally means that they developer forgot to add it
 *       to the list of tests using the unit_test_load() function.
 *    -  Be sure to make sure that this test is valgrind-clean by running
 *       the following command:
\verbatim
         valgrind -v --leak-check=full ./unit_test_test
\endverbatim
 *    -  If you want normal running, but don't care to see any error
 *       messages (because you already know they are a normal part of the
 *       current tests), then divert stderr to /dev/null:
\verbatim
         ./unit_test_test 2> /dev/null
\endverbatim
 *    -  If you want to double-check the total duration of the unit-test
 *       (and get an idea of the setup overhead), run the test using the
 *       'time' command:
\verbatim
         time ./unit_test_test
\endverbatim
 *       There should be only a few milliseconds of difference in what the
 *       unit-test says, and what 'time' says.
 *
 * \todo
 *    -  Create a test that demonstrates how failures do not stop the test
 *       unless --stop-on-error is supplied.
 */

#include <xpc/fuzz.h>                  /* functions for creating fuzz strings */
#include <xpc/unit_test.h>             /* unit_test_t structure               */

#ifdef USE_XPCCUT_NULLPTR_TEST
#include <xpc/portable_subset.h>       /* Other testing functions and macros  */
#endif

#if XPC_HAVE_LIMITS_H
#include <limits.h>                    /* UINT_MAX                            */
#endif

/*
 *    This function is deprecated, but we'll keep it around for awhile
 *    and give it its due testing.
 */

extern cbool_t unit_test_options_initialize
(
   unit_test_options_t * options,
   cbool_t verbose,
   cbool_t showvalues,
   cbool_t usetextsynch,
   cbool_t showstepnumbers,
   cbool_t showprogress,
   cbool_t stoponerror,
   cbool_t batchmode,
   cbool_t interactive,
   cbool_t beeps,
   cbool_t summarize,
   cbool_t casepause,
   int singlegroup,
   int singlecase,
   int singlesubtest,
   int testsleeptime
);

/**
 *    Provides a way to disable some "destructor" calls, in order to test
 *    leakage mechanisms.
 *
 *    This variable is normally false.  It can be set to true using the
 *    --leak-check option (which should not be confused with valgrind's
 *    option of the same name).
 *
 * \note
 *    Might not be implemented in the current unit test application.
 */

static cbool_t g_do_leak_check = false;

/**
 *    Provides a way to enable dumping certain results to standard output.
 *
 *    For example, test 09.03 will use this option to dump a list of random
 *    integers to standard ouput.
 *
 *    This variable is normally false.  It can be set to true using the
 *    --dump-text option.
 */

static cbool_t g_do_dump_text = false;

/**
 *    Provides a way to set the option structure's test number to a
 *    recognizably bogus value, for less confusing output.
 *
 *    When output, this value is incremented internally, and will be
 *    displayed as "1000".
 */

static int g_BOGUS_TEST_NUMBER = 999;

/**
 *    Provides an internal helper function to show a message indicating that
 *    the failure was deliberate.
 *
 *    This output helps reassure the user that all is really well.  It
 *    appears only if the --show-progress option is in effect.
 */

static void
show_deliberate_failure
(
   const unit_test_options_t * options    /**< The options to check.          */
)
{
   if (unit_test_options_show_progress(options) && ! xpccut_is_silent())
      fprintf(stdout, "! %s\n", _("This FAILURE is deliberate."));
}

/**
 *    This first test demonstrates the basics of the unit-test functions.
 *
 *    This functions is of the type unit_test_func_t; see that typedef for
 *    more information.
 *
 * \group
 *    1. Integration and smoke tests.
 *
 * \case
 *    1. Smoke test.
 *
 *    This first test is like an overview.  The detailed tests start in
 *    group 2.
 *
 * \test
 *    This unit-test function perforce tests more than one unit from the
 *    unit-test library (libxpccut):
 *    -  unit_test_status_initialize()
 *    -  unit_test_status_pass()
 *    -  unit_test_status_fail()
 *    -  unit_test_status_is_verbose() [by user visualization only]
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_01_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, "XPCCUT", _("Smoke Test")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         if (unit_test_options_show_values(options))
            fprintf(stdout, "  %s\n", _("No values to show in this test"));

         /*  1 */

         if (unit_test_status_next_subtest(&status, "unit_test_status_pass()"))
         {
            unit_test_status_pass(&status, true);
            ok =
            (
               (status.m_Subtest_Error_Count == 0) &&
               (status.m_Failed_Subtest == 0)
            );
            if (! ok)
            {
               (void) unit_test_status_fail(&status);    /* create a failure  */
               if (unit_test_options_is_verbose(options))
               {
                  fprintf
                  (
                     stderr, "%s %s\n",
                     "unit_test_status_pass()", _("internal failure")
                  );
               }
            }
            unit_test_status_pass(&status, ok);
         }

         /*  2 */

         if
         (
            ok &&
            unit_test_status_next_subtest(&status, "unit_test_status_fail()")
         )
         {
            /**
             * \tricky
             *    The following two calls create a failure and then converts
             *    it to a pass.  However, the unit_test_status_pass()
             *    function increments unit_test_status_t.m_Subtest_Error_Count,
             *    and there is no way to decrement it.  The function also
             *    sets unit_test_status_t.m_Failed_Subtest, with no way to
             *    unset it via a function call.
             *
             *    Therefore, this unit-test directly accesses the structure.
             *    <i> Never </i> do that in your applications, please!
             */

            ok = unit_test_status_fail_deliberately(&status);   /* make failure */

            /*
             * Now part of unit_test_status_failure()
             *
             *   if (unit_test_options_is_verbose(options))
             *      fprintf(stdout, "! %s\n", _("This FAILURE is deliberate."));
             */

            if (ok)
            {
               ok =
               (
                  (status.m_Subtest_Error_Count == 1) &&
                  (status.m_Failed_Subtest == 2)
               );
               unit_test_status_pass(&status, ok); /* convert it to a "pass"  */
               if (ok)
               {
                  /*
                   * This direct access has been replaced by the usage of
                   * some special self-test functions meant only for
                   * internal usage in testing this unit-test framework.
                   *
                   * status.m_Subtest_Error_Count--;  // direct access (bad)!!!
                   * status.m_Failed_Subtest = 0;     // direct access (bad)!!!
                   *
                   */

                  ok = status_self_test_error_count_decrement(&status);
                  if (ok)
                     ok = status_self_test_failed_subtest_set(&status, 0);
               }

               /*
                * The final result of this sub-test.
                */

               unit_test_status_pass(&status, ok);
            }
         }

         /*
          * This duration check is no longer easy to note in this large
          * unit-test application, so don't bother with it any more.
          *
          * if (ok)
          *    xpccut_ms_sleep(500);               // a visual duration check
          */
      }
   }
   return status;
}

/**
 *    Provides a way to verify the default options
 *
 * \return
 *    Return's true if all the default are matched.  There is no check of
 *    the op parameter in this internal helper function.
 */

static cbool_t
default_options_check
(
   const unit_test_options_t * op,  /**< The options structure to be checked. */
   cbool_t test_show_progress       /**< If true, test this special case.     */
)
{
   cbool_t result;
   const char * itemname = "m_Is_Verbose";
   result = op->m_Is_Verbose              == XPCCUT_IS_VERBOSE;
   if (result)
   {
      itemname = "m_Show_Values";
      result = op->m_Show_Values          == XPCCUT_SHOW_VALUES;
   }
   if (result)
   {
      itemname = "m_Text_Synch";
      result = op->m_Text_Synch           == XPCCUT_TEXT_SYNCH;
   }
   if (result)
   {
      itemname = "m_Show_Step_Numbers";
      result = op->m_Show_Step_Numbers    == XPCCUT_SHOW_STEP_NUMBERS;
   }
   if (result)
   {
      /*
       * We sometimes pass along the --no-show-progress option from the
       * normal options structure to the test options structure.  (See the
       * references to "x_options_x" in the code below).  Therefore, we
       * don't want to regression test for this time.  We'll add special
       * cases in the tests to make sure we don't miss it.
       */

      if (test_show_progress)
      {
         itemname = "m_Show_Progress";
         result = op->m_Show_Progress     == XPCCUT_SHOW_PROGRESS;
      }
   }
   if (result)
   {
      itemname = "m_Stop_On_Error";
      result = op->m_Stop_On_Error        == XPCCUT_STOP_ON_ERROR;
   }
   if (result)
   {
      itemname = "m_Batch_Mode";
      result = op->m_Batch_Mode           == XPCCUT_BATCH_MODE;
   }
   if (result)
   {
      itemname = "m_Interactive";
      result = op->m_Interactive          == XPCCUT_INTERACTIVE;
   }
   if (result)
   {
      itemname = "m_Beep_Prompt";
      result = op->m_Beep_Prompt          == XPCCUT_BEEP;
   }
   if (result)
   {
      itemname = "m_Summarize";
      result = op->m_Summarize            == XPCCUT_SUMMARIZE;
   }
   if (result)
   {
      itemname = "m_Need_Subtests";
      result = op->m_Need_Subtests        == XPCCUT_NEED_SUBTESTS;
   }
   if (result)
   {
      itemname = "m_Force_Failure";
      result = op->m_Force_Failure        == XPCCUT_FORCE_FAILURE;
   }
   if (result)
   {
      itemname = "m_Case_Pause";
      result = op->m_Case_Pause           == XPCCUT_CASE_PAUSE;
   }
   if (result)
   {
      itemname = "m_Single_Test_Group";
      result = op->m_Single_Test_Group    == XPCCUT_NO_SINGLE_GROUP;
   }
   if (result)
   {
      itemname = "m_Single_Test_Case";
      result = op->m_Single_Test_Case     == XPCCUT_NO_SINGLE_CASE;
   }
   if (result)
   {
      itemname = "m_Single_Sub_Test";
      result = op->m_Single_Sub_Test      == XPCCUT_NO_SINGLE_SUB_TEST;
   }
   if (result)
   {
      itemname = "m_Test_Sleep_Time";
      result = op->m_Test_Sleep_Time      == XPCCUT_TEST_SLEEP_TIME;
   }
   if (result)
   {
      itemname = "m_Current_Test_Number";
      result = op->m_Current_Test_Number  == XPCCUT_NO_CURRENT_TEST;
   }
   if (result)
   {
      itemname = "m_Response_Before";
      result = op->m_Response_Before      == 0;
   }
   if (result)
   {
      itemname = "m_Response_After";
      result = op->m_Response_After       == 0;
   }

   /*
    * We cannot check this option, since, in tests of the
    * unit_test_options_t
    *
    * if (result)
    * {
    *    itemname = "m_Is_Simulated";
    *    result = op->m_Is_Simulated      == XPCCUT_IS_SIMULATED;
    * }
    */

   if (! result)
      xpccut_errprint_ex(_("bad default option value"), itemname);

   return result;
}

/**
 *    Provides a unit/regression test to verify that the basic
 *    initialization function properly zeros or clears all fields to their
 *    "default" values.
 *
 *    The function tested is like a "C" version of a "C++" default
 *    constructor.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *    1. unit_test_status_init()
 *
 * \test
 *    -  unit_test_status_init()
 *
 * \todo
 *    It would be nice if we could verify the duration of the previous test
 *    to be correct to within plus-or-minus 10 ms.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 1, "unit_test_status_t", "unit_test_status_init()"
   );
   if (ok)
   {
      unit_test_status_t x_status_x;
      (void) unit_test_status_init(&x_status_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_init(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Good 'this'"))
      {
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "m_Test_Options"))
      {
         ok = x_status_x.m_Test_Options == nullptr;
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "m_Group_Name"))
      {
         ok = strlen(x_status_x.m_Group_Name) == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "m_Case_Description"))
      {
         ok = strlen(x_status_x.m_Case_Description) == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "m_Subtest_Name"))
      {
         ok = strlen(x_status_x.m_Subtest_Name) == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "m_Test_Group"))
      {
         /*
          * Here we need to verify that we can get stop-on-error to cause
          * the rest the subtests to automatically be skipped without being
          * executed.  Done.
          *
          * ok = x_status_x.m_Test_Group == 1;     // erroneous value
          */

         ok = x_status_x.m_Test_Group == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "m_Test_Case"))
      {
         ok = x_status_x.m_Test_Case == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "m_Subtest"))
      {
         ok = x_status_x.m_Subtest == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "m_Test_Result"))
      {
         ok = x_status_x.m_Test_Result == true;
         unit_test_status_pass(&status, ok);
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "m_Subtest_Error_Count"))
      {
         ok = x_status_x.m_Subtest_Error_Count == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "m_Failed_Subtest"))
      {
         ok = x_status_x.m_Failed_Subtest == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 13 */

      if (unit_test_status_next_subtest(&status, "m_Test_Disposition"))
      {
         ok = x_status_x.m_Test_Disposition == XPCCUT_DISPOSITION_ABORTED;
         unit_test_status_pass(&status, ok);
      }

      /* 14 */

      if (unit_test_status_next_subtest(&status, "m_Start_Time_us"))
      {
         ok = x_status_x.m_Start_Time_us.tv_sec == 0;
         if (ok)
            ok = x_status_x.m_Start_Time_us.tv_usec == 0;

         unit_test_status_pass(&status, ok);
      }

      /* 15 */

      if (unit_test_status_next_subtest(&status, "m_End_Time_us"))
      {
         ok = x_status_x.m_End_Time_us.tv_sec == 0;
         if (ok)
            ok = x_status_x.m_End_Time_us.tv_sec == 0;

         unit_test_status_pass(&status, ok);
      }

      /* 16 */

      if (unit_test_status_next_subtest(&status, "m_Test_Duration_ms"))
      {
         ok = x_status_x.m_Test_Duration_ms == 0.0;
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the extended
 *    initialization function properly sets all fields to their
 *    parameterized values.
 *
 *    The function tested is like a "C" version of a "C++" principal
 *    constructor.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *    2. unit_test_status_initialize()
 *
 * \test
 *    -  unit_test_status_initialize()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_02 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 2,
      "unit_test_status_t", "unit_test_status_initialize()"
   );
   if (ok)
   {
      unit_test_status_t x_status_x;
      unit_test_options_t x_options_x;
      cbool_t null_ok = false;
      ok = unit_test_status_init(&x_status_x);
      if (ok)
         ok = unit_test_options_init(&x_options_x);

      /*
       * This next statement causes the "TEST" banner to change to
       * "Simulated TEST".
       */

      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      if (ok)
      {
         null_ok = ! unit_test_status_initialize
         (
            nullptr, options, 0, 0, "xxx", "yyy"
         );
         if (unit_test_options_is_verbose(options))
            fprintf(stdout, _("This test number is simulated"));

         ok = unit_test_status_initialize
         (
            &x_status_x, &x_options_x, 33, 44, _("Group 33"), _("Case 44")
         );
         if (ok)
         {
            /*
             * Pass along this option to guarantee hiding output even from
             * the secondary test options function calls.
             */

            ok = unit_test_options_show_progress_set
            (
               &x_options_x,
               unit_test_options_show_progress(options)
            );
         }
      }
      else
      {
         xpccut_errprint(_("internal failure, could not set up options"));

         /*
          * This call will work, but is meant to indicate that the failure
          * may be deliberate.
          *
          * unit_test_status_fail(&status);
          */

         unit_test_status_pass(&status, false);
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null options-pointer"))
      {
         unit_test_status_t y_status_y;
         null_ok = ! unit_test_status_initialize
         (
            &y_status_y, nullptr, 99, 99, "xxx", "yyy"
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Zero group-number"))
      {
         unit_test_status_t y_status_y;
         null_ok = ! unit_test_status_initialize
         (
            &y_status_y, options, 0, 99, "xxx", "yyy"
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Zero case-number"))
      {
         unit_test_status_t y_status_y;
         null_ok = ! unit_test_status_initialize
         (
            &y_status_y, options, 99, 0, "xxx", "yyy"
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Null group-name"))
      {
         unit_test_status_t y_status_y;
         null_ok = ! unit_test_status_initialize
         (
            &y_status_y, options, 99, 99, nullptr, "yyy"
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Null case-name"))
      {
         unit_test_status_t y_status_y;
         null_ok = ! unit_test_status_initialize
         (
            &y_status_y, options, 99, 99, "xxx", nullptr
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "Good 'this'"))
      {
         /*
          * Here, the value of "ok" set way above is what is tested.
          */

         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "m_Test_Options"))
      {
         ok = x_status_x.m_Test_Options == &x_options_x;
         if (ok)
         {
            /*
             * This subtest is actually kind of redundent relative to the
             * test of unit_test_options_init().  Not totally redundant,
             * though.
             */

            ok = default_options_check(x_status_x.m_Test_Options, false);

            /*
             * Do this afterward so the check above can succeed.
             */

            if (ok)
            {
               ok = unit_test_options_current_test_set
               (
                  &x_options_x, g_BOGUS_TEST_NUMBER
               );
            }
         }
         else
            xpccut_errprint(_("internal failure, options address bad"));

         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "m_Group_Name"))
      {
         ok = strcmp(x_status_x.m_Group_Name, _("Group 33")) == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "m_Case_Description"))
      {
         ok = strcmp(x_status_x.m_Case_Description, _("Case 44")) == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "m_Subtest_Name"))
      {
         /*
          * Currently, the subtest-name is left at it default value from
          * unit_test_status_init(), an empty string.
          */

         ok = strlen(x_status_x.m_Subtest_Name) == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "m_Test_Group"))
      {
         ok = x_status_x.m_Test_Group == 33;
         unit_test_status_pass(&status, ok);
      }

      /* 13 */

      if (unit_test_status_next_subtest(&status, "m_Test_Case"))
      {
         ok = x_status_x.m_Test_Case == 44;
         unit_test_status_pass(&status, ok);
      }

      /* 14 */

      if (unit_test_status_next_subtest(&status, "m_Subtest"))
      {
         /*
          * Currently, the subtest-number is left at it default value from
          * unit_test_status_init(), zero.
          */

         ok = x_status_x.m_Subtest == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 15 */

      if (unit_test_status_next_subtest(&status, "m_Test_Result"))
      {
         ok = x_status_x.m_Test_Result == true;
         unit_test_status_pass(&status, ok);
      }

      /* 16 */

      if (unit_test_status_next_subtest(&status, "m_Subtest_Error_Count"))
      {
         ok = x_status_x.m_Subtest_Error_Count == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 17 */

      if (unit_test_status_next_subtest(&status, "m_Failed_Subtest"))
      {
         ok = x_status_x.m_Failed_Subtest == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 18 */

      if (unit_test_status_next_subtest(&status, "m_Test_Disposition"))
      {
         ok = x_status_x.m_Test_Disposition == XPCCUT_DISPOSITION_CONTINUE;
         unit_test_status_pass(&status, ok);
      }

      /* 19 */

      if (unit_test_status_next_subtest(&status, "m_Start_Time_us"))
      {
         ok = x_status_x.m_Start_Time_us.tv_sec > 0;
         if (ok)
            ok = x_status_x.m_Start_Time_us.tv_usec >= 0;

         unit_test_status_pass(&status, ok);
      }

      /* 20 */

      if (unit_test_status_next_subtest(&status, "m_End_Time_us"))
      {
         ok = x_status_x.m_End_Time_us.tv_sec == 0;
         if (ok)
            ok = x_status_x.m_End_Time_us.tv_sec == 0;

         unit_test_status_pass(&status, ok);
      }

      /* 21 */

      if (unit_test_status_next_subtest(&status, "m_Test_Duration_ms"))
      {
         ok = x_status_x.m_Test_Duration_ms == 0.0;
         if (! ok)
         {
            fprintf
            (
               stdout, "  m_Test_Duration = %f\n", x_status_x.m_Test_Duration_ms
            );
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "reset"
 *    functionality works.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *    3. unit_test_status_reset()
 *
 * \test
 *    -  unit_test_status_reset()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_03 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 3,
      "unit_test_status_t", "unit_test_status_reset()"
   );
   if (ok)
   {
      unit_test_status_t x_status_x;
      (void) unit_test_status_init(&x_status_x);   /* set up for sane usage   */

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_reset(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Good initialization"))
      {
         ok = x_status_x.m_Test_Disposition == XPCCUT_DISPOSITION_ABORTED;
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Good reset of disposition"))
      {
         ok = unit_test_status_reset(&x_status_x);
         if (ok)
            ok = x_status_x.m_Test_Disposition == XPCCUT_DISPOSITION_CONTINUE;

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "pass/fail`"
 *    functionality works.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *    4. Pass and fail checks
 *
 * \test
 *    -  unit_test_status_pass()
 *    -  unit_test_status_fail()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_04 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 4,
      "unit_test_status_t", "unit_test_status_pass/fail()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      unit_test_status_t x_status_x;
      (void) unit_test_status_init(&x_status_x);   /* set up for sane usage   */
      (void) unit_test_options_init(&x_options_x); /* ditto, for options      */
      (void) unit_test_options_is_simulated_set(&x_options_x, true);
      if (ok)
      {
         /*
          * Pass along this option to guarantee hiding output even from
          * the secondary test options function calls.
          */

         ok = unit_test_options_show_progress_set
         (
            &x_options_x, unit_test_options_show_progress(options)
         );
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', pass()"))
      {
         cbool_t null_ok = ! unit_test_status_pass(nullptr, true);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Good initialization"))
      {
         ok = x_status_x.m_Test_Result == true;
         if (ok)
            ok = x_status_x.m_Subtest_Error_Count == 0;

         if (ok)
            ok = x_status_x.m_Failed_Subtest == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Good failure with pass()"))
      {
         /*
          * Wire in a sub-test number by accessing the structure directly (a
          * big no-no, except in our unit test.  Also, since we're now
          * making a legitimate call to unit_test_status_pass() for our
          * fake status structure, x_status_x, we have to hard-wired in the
          * fake options structure, too.  Otherwise, a meaningless (in the
          * context of this unit-test) and confusing error message occurs.
          */

         x_status_x.m_Subtest = 99;
         x_status_x.m_Test_Options = &x_options_x;
         ok = unit_test_status_fail_deliberately(&x_status_x);
         if (ok)
            ok = x_status_x.m_Test_Result == false;

         if (ok)
            ok = x_status_x.m_Subtest_Error_Count == 1;

         if (ok)
            ok = x_status_x.m_Failed_Subtest == 99;

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Good success"))
      {
         /*
          * Wire in a sub-test number by accessing the structure directly (a
          * big no-no, except in our unit test.  Also, this sub-test depends
          * on settings made in the previous sub-test.
          */

         x_status_x.m_Subtest = 100;
         ok = unit_test_status_pass(&x_status_x, true);
         if (ok)
            ok = x_status_x.m_Test_Result == true;

         if (ok)
            ok = x_status_x.m_Subtest_Error_Count == 1;

         if (ok)
            ok = x_status_x.m_Failed_Subtest == 99;

         unit_test_status_pass(&status, ok);
      }

      /*
       * The same tests, through the unit_test_status_fail() interface.
       * Reset the structures again.
       */

      (void) unit_test_status_init(&x_status_x);   /* set up for sane usage   */
      (void) unit_test_options_init(&x_options_x); /* ditto, for options      */
      (void) unit_test_options_is_simulated_set(&x_options_x, true);
      if (ok)
      {
         /*
          * Pass along this option to guarantee hiding output even from
          * the secondary test options function calls.
          */

         ok = unit_test_options_show_progress_set
         (
            &x_options_x, unit_test_options_show_progress(options)
         );
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Null 'this', fail()"))
      {
         cbool_t null_ok = ! unit_test_status_fail(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Good initialization"))
      {
         ok = x_status_x.m_Test_Result == true;
         if (ok)
            ok = x_status_x.m_Subtest_Error_Count == 0;

         if (ok)
            ok = x_status_x.m_Failed_Subtest == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "Good failure with fail()"))
      {
         /*
          * Same comments as for subtest 3 above.
          */

         x_status_x.m_Subtest = 98;
         x_status_x.m_Test_Options = &x_options_x;
         ok = unit_test_status_fail_deliberately(&x_status_x);
         if (ok)
            ok = x_status_x.m_Test_Result == false;

         if (ok)
            ok = x_status_x.m_Subtest_Error_Count == 1;

         if (ok)
            ok = x_status_x.m_Failed_Subtest == 98;

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a kludge to notify the user that only a duration check
 *    failed.
 *
 *    This number is set to the value of the last sub-test of
 *    unit_unit_test_02_05() that failed the duration check.  This number is
 *    reported just before this unit-test application exits.
 *
 *    Sometimes just rerunning the test will help.  Sometime, the duration
 *    has to be adjust upwards to avoid false positives.
 */

static int g_duration_out_of_range = 0;

/**
 *    Provides a unit/regression test to somewhat verify the timing
 *    functionality.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *    5. Timing functions.
 *
 * \test
 *    -  unit_test_status_start_timer()
 *    -  unit_test_status_time_delta()
 *    -  xpccut_ms_sleep()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_05 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 5,
      "unit_test_status_t", "unit_test_status_start_timer/time_delta()"
   );
   if (ok)
   {
      unit_test_status_t x_status_x;
      (void) unit_test_status_init(&x_status_x);   /* set up for sane usage   */

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', time()"))
      {
         cbool_t null_ok = ! unit_test_status_start_timer(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Basic time-getting"))
      {
         ok = unit_test_status_start_timer(&x_status_x);
         if (ok)
            ok = x_status_x.m_End_Time_us.tv_sec == 0;

         if (ok)
            ok = x_status_x.m_End_Time_us.tv_usec == 0;

         if (ok)
            ok = x_status_x.m_Start_Time_us.tv_sec > 0;

         if (ok)
            ok = x_status_x.m_Start_Time_us.tv_usec >= 0;

         if (! ok && unit_test_options_is_verbose(options))
         {
            fprintf
            (
               stdout,
               "\n"
               "? Bad times:\n"
               "\n"
               "  Start time: %d sec %d msec (should be > 0, >= 0)\n"
               "    End time: %d sec %d msec (should be > 0, >= 0)\n"
               "\n",
               (int) x_status_x.m_Start_Time_us.tv_sec,
               (int) x_status_x.m_Start_Time_us.tv_usec,
               (int) x_status_x.m_End_Time_us.tv_sec,
               (int) x_status_x.m_End_Time_us.tv_usec
            );
         }
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Null 'this', time_delta() 1"))
      {
         cbool_t null_ok = unit_test_status_time_delta(nullptr, true) == -1.0;
         unit_test_status_pass(&status, null_ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Null 'this', time_delta() 2"))
      {
         cbool_t null_ok = unit_test_status_time_delta(nullptr, false) == -1.0;
         unit_test_status_pass(&status, null_ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Time-difference, 50 ms"))
      {
         ok = unit_test_status_start_timer(&x_status_x);
         if (ok)
            ok = x_status_x.m_End_Time_us.tv_sec == 0;

         if (ok)
            ok = x_status_x.m_End_Time_us.tv_usec == 0;

         if (ok)
            ok = x_status_x.m_Start_Time_us.tv_sec > 0;

         if (ok)
            ok = x_status_x.m_Start_Time_us.tv_usec >= 0;

         if (ok)
         {
            double d;
            xpccut_ms_sleep(50);
            d = unit_test_status_time_delta(&x_status_x, false);  /* no reset */
            if (ok)
               ok = x_status_x.m_Test_Duration_ms == d;

            if (ok)
            {
#ifdef WIN32
               /**
                * At least under Win 2000 in a VM, there seems to be more
                * variance in the timing.  We ran the following command a
                * number of times in Win 2000 and increased the ranges until
                * a few runs in a row turned up no errors:
                *
                *    unit_test_test --verbose --group 2 --case 5
                *
                * This same note applies to many more tests below.
                *
                * \todo
                *    Parameterize these range-checks in a function that can
                *    report the actual value and where it falls within the
                *    range.
                */

               ok = (d > 40.0) && (d < 70.0);
#else
               ok = (d > 40.0) && (d < 60.0);
#endif
            }
            else
               xpccut_errprint_ex(_("Subtest 5, not set"), "m_Test_Duration_ms");

            if (! ok)
            {
               xpccut_errprint(_("Subtest 5, duration out of range"));
               g_duration_out_of_range = 5;
            }
            if (unit_test_options_is_verbose(options))
            {
               fprintf
               (
                  stdout, "  Duration: nominal = 50 ms, actual = %f ms\n", d
               );
            }
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Time-difference, 25 ms"))
      {
         ok = unit_test_status_start_timer(&x_status_x);
         if (ok)
         {
            double d;
            xpccut_ms_sleep(25);
            d = unit_test_status_time_delta(&x_status_x, false);  /* no reset */
#ifdef WIN32
            /*
             * At least under Win 2000 in a VM, there seems to be more
             * variance in the timing.
             */

            ok = (d > 15.0) && (d < 40.0);
#else
            /*
             * On an old Pentium III laptop this sometimes fails.
             *
             *    ok = (d > 15.0) && (d < 35.0);
             */

            ok = (d > 15.0) && (d < 40.0);
#endif
            if (! ok)
            {
               xpccut_errprint(_("Subtest 6, duration out of range"));
               g_duration_out_of_range = 6;
            }

            if (unit_test_options_is_verbose(options))
            {
               fprintf
               (
                  stdout, "  Duration: nominal = 25 ms, actual = %f ms\n", d
               );
            }
         }
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "Time-difference, 10 ms"))
      {
         ok = unit_test_status_start_timer(&x_status_x);
         if (ok)
         {
            double d;
            xpccut_ms_sleep(10);
            d = unit_test_status_time_delta(&x_status_x, false);  /* no reset */
#ifdef WIN32
            /*
             * At least under Win 2000 in a VM, there seems to be more
             * variance in the timing.
             */

            ok = (d > 1.0) && (d < 30.0);
#else
            ok = (d > 1.0) && (d < 20.0);
#endif
            if (! ok)
            {
               xpccut_errprint(_("Subtest 7, duration out of range"));
               g_duration_out_of_range = 7;
            }

            if (unit_test_options_is_verbose(options))
            {
               fprintf
               (
                  stdout, "  Duration: nominal = 10 ms, actual = %f ms\n", d
               );
            }
         }
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "Time-difference, 5 ms"))
      {
         ok = unit_test_status_start_timer(&x_status_x);
         if (ok)
         {
            double d;
            xpccut_ms_sleep(5);
            d = unit_test_status_time_delta(&x_status_x, false);  /* no reset */
#ifdef WIN32
               /*
                * At least under Win 2000 in a VM, there seems to be more
                * variance in the timing.
                */

            ok = (d > 1.0) && (d < 25.0);
#else
            ok = (d > 1.0) && (d < 15.0);                   /* up from 10.0   */
#endif
            if (! ok)
            {
               xpccut_errprint(_("Subtest 8, duration out of range"));
               g_duration_out_of_range = 8;
            }

            if (unit_test_options_is_verbose(options))
            {
               fprintf
               (
                  stdout, "  Duration: nominal = 5 ms, actual = %f ms\n", d
               );
            }
         }
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "Time-difference, 1 ms"))
      {
         ok = unit_test_status_start_timer(&x_status_x);
         if (ok)
         {
            double d;
            xpccut_ms_sleep(1);
            d = unit_test_status_time_delta(&x_status_x, false);  /* no reset */
#ifdef WIN32
               /*
                * At least under Win 2000 in a VM, there seems to be more
                * variance (way more!) in the timing.
                */

            ok = (d > 0.5) && (d < 25.0);
#else
            ok = (d > 0.5) && (d < 10.0);
#endif
            if (! ok)
            {
               xpccut_errprint(_("Subtest 9, duration out of range"));
               g_duration_out_of_range = 9;
            }

            if (unit_test_options_is_verbose(options))
            {
               fprintf
               (
                  stdout, "  Duration: nominal = 1 ms, actual = %f ms\n", d
               );
            }
         }
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "Time-difference with reset"))
      {
         ok = unit_test_status_start_timer(&x_status_x);
         if (ok)
         {
            double d;
            xpccut_ms_sleep(5);
            d = unit_test_status_time_delta(&x_status_x, true);   /* w/reset  */
            ok = x_status_x.m_Start_Time_us.tv_sec > 0;
            if (ok)
               ok = x_status_x.m_Start_Time_us.tv_usec >= 0;
            else
               xpccut_errprint(_("Subtest 9, m_Start_Time_us.tv_sec == 0"));

            if (ok)
               ok = x_status_x.m_Test_Duration_ms == d;
            else
               xpccut_errprint(_("Subtest 9, m_Start_Time_us.tv_usec < 0"));

            /*
             * Don't think this output is helpful here.
             *
             * if (unit_test_options_is_verbose(options))
             * {
             *    fprintf
             *    (
             *       stdout, "  Duration: nominal = 5 ms, actual = %f ms\n", d
             *    );
             * }
             */
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the title
 *    functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *    6. unit_test_status_show_title()
 *
 * \test
 *    -  unit_test_status_show_title()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_06 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 6,
      "unit_test_status_t", "unit_test_status_show_title()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_show_title(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         ok = unit_test_status_show_title(&status);
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "can-proceed"
 *    functionality works.
 *
 *    This is, of necessity, somewhat of a regression test.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *    7. unit_test_status_can_proceed()
 *
 * \test
 *    -  unit_test_status_can_proceed()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_07 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 7,
      "unit_test_status_t", "unit_test_status_can_proceed()"
   );
   if (ok)
   {
      unit_test_status_t x_status_x;
      (void) unit_test_status_init(&x_status_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_can_proceed(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "CONTINUE"))
      {
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_CONTINUE;
         ok = unit_test_status_can_proceed(&x_status_x);
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "DNT (Did Not Test)"))
      {
         cbool_t not_ok;
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_DNT;
         not_ok = ! unit_test_status_can_proceed(&x_status_x);
         unit_test_status_pass(&status, not_ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "FAILED"))
      {
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_FAILED;
         ok = unit_test_status_can_proceed(&x_status_x);
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "QUITTED"))
      {
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_QUITTED;
         ok = unit_test_status_can_proceed(&x_status_x);
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "ABORTED"))
      {
         cbool_t not_ok;
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_ABORTED;
         not_ok = ! unit_test_status_can_proceed(&x_status_x);
         unit_test_status_pass(&status, not_ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "ignore"
 *    functionality works.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *    8. unit_test_status_ignore()
 *
 * \test
 *    -  unit_test_status_ignore()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_08 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 8, "unit_test_status_t", "unit_test_status_ignore()"
   );
   if (ok)
   {
      unit_test_status_t x_status_x;
      (void) unit_test_status_init(&x_status_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_ignore(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "CONTINUE"))
      {
         cbool_t not_ok;
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_CONTINUE;
         not_ok = ! unit_test_status_ignore(&x_status_x);
         if (not_ok)
            ok = x_status_x.m_Test_Result == true;       /* from init() only  */
         else
            ok = false;

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "DNT (Did Not Test)"))
      {
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_DNT;
         ok = unit_test_status_ignore(&x_status_x);
         if (ok)
            ok = x_status_x.m_Test_Result == true;       /* from ignore()     */

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "FAILED"))
      {
         cbool_t not_ok;
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_FAILED;
         not_ok = ! unit_test_status_ignore(&x_status_x);
         if (not_ok)
            ok = x_status_x.m_Test_Result == true;       /* from init() only  */
         else
            ok = false;

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "QUITTED"))
      {
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_QUITTED;

         /*
          * Note that this call checks the disposition, and sets the
          * test-result status to 'true'.  In other words, the user wants to
          * quit, but pass the current test.
          */

         ok = unit_test_status_ignore(&x_status_x);
         if (ok)
            ok = x_status_x.m_Test_Result == true;       /* from ignore()     */

         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "ABORTED"))
      {
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_ABORTED;

         /*
          * Note that this call checks the disposition, and sets the
          * test-result status to 'false'.  In other words, the user wants to
          * abort, and fail the current test.  That is, aborting is quitting
          * where a failure of the current test is forced.
          */

         ok = unit_test_status_ignore(&x_status_x);
         if (ok)
            ok = x_status_x.m_Test_Result == false;      /* from ignore()     */

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "next sub-test"
 *    functionality works.
 *
 *    Because of the heavy testing of this function throughout the unit-test
 *    application, this test doesn't bother testing scenarios that merely
 *    result in different output to the screen.
 *
 *    Only scenarios that might result in different internal status
 *    assignments are tested.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *    9. unit_test_status_next_subtest()
 *
 * \test
 *    -  unit_test_status_next_subtest()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_09 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 9,
      "unit_test_status_t", "unit_test_status_next_subtest()"
   );
   if (ok)
   {
      unit_test_status_t x_status_x;
      unit_test_options_t x_options_x;
      ok = unit_test_status_init(&x_status_x);
      if (ok)
         ok = unit_test_options_init(&x_options_x);

      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      if (ok)
      {
         ok = unit_test_options_current_test_set
         (
            &x_options_x, g_BOGUS_TEST_NUMBER
         );
      }
      if (ok)
      {
         ok = unit_test_status_initialize
         (
            &x_status_x, &x_options_x, 97, 97,
            _("Internal Status"), "unit_test_status_next_subtest()"
         );
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_next_subtest(nullptr, "?");
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Basic initialization"))
      {
         if (ok)                                   /* the result from above   */
            ok = unit_test_status_next_subtest(&x_status_x, "mxlptlx");

         if (ok)
         {
            ok = x_status_x.m_Test_Options == &x_options_x;
            if (ok)
               ok = x_status_x.m_Subtest == 1;

            if (ok)
               ok = strcmp(x_status_x.m_Subtest_Name, "mxlptlx") == 0;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Basic incrementing"))
      {
         ok = unit_test_status_next_subtest(&x_status_x, "?");
         if (ok)
         {
            ok = x_status_x.m_Test_Options == &x_options_x;
            if (ok)
               ok = x_status_x.m_Subtest == 2;

            if (ok)
               ok = strcmp(x_status_x.m_Subtest_Name, "?") == 0;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Null tagname"))
      {
         ok = unit_test_status_next_subtest(&x_status_x, nullptr);
         if (ok)
         {
            ok = x_status_x.m_Test_Options == &x_options_x;
            if (ok)
               ok = x_status_x.m_Subtest == 3;

            if (ok)
               ok = strcmp(x_status_x.m_Subtest_Name, _("unnamed")) == 0;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "--summarize behavior"))
      {
         /**
          * Obtain the silence-setting that was specified on the command
          * line, so that it can be restored at the end of some of the
          * sub-tests.  If the user didn't specify --summarize, this value
          * should be false by default.
          *
          * Also save the --summarize setting for the same treatment.
          */

         cbool_t original_silence_setting = xpccut_is_silent();
         cbool_t original_summary_setting = unit_test_options_is_summary
         (
            &x_options_x
         );

         /**
          * Turn on the --summarize option.  This option causes
          * unit_test_status_next_subtest() to return 'false', so that in no
          * case will a sub-test actually run.
          */

         cbool_t not_ok = false;
         ok = unit_test_options_is_summary_set(&x_options_x, true);
         if (ok)
            not_ok = ! unit_test_status_next_subtest(&x_status_x, "summarize");

         if (not_ok)
         {
            ok = x_status_x.m_Test_Options == &x_options_x;
            if (ok)
               ok = x_status_x.m_Subtest == 4;

            if (ok)
               ok = strcmp(x_status_x.m_Subtest_Name, "summarize") == 0;
         }
         else
            xpccut_errprint(_("next_subtest() returned true in subtest 5"));

         unit_test_status_pass(&status, ok);

         /*
          * Restore the original setting of the silence flag, so that
          * the output of the unit-test will have the appearance expected
          * by the dude who ran this test.  Also restore the summarize
          * setting.
          */

         (void) unit_test_options_is_summary_set
         (
            &x_options_x, original_summary_setting
         );
         if (original_silence_setting)
            xpccut_silence_printing();
         else
            xpccut_allow_printing();
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "--sub-test skip behavior"))
      {
         /*
          * Save the original sub-test value for restoration at the end of
          * this sub-test.
          */

         int original_sub_test = unit_test_options_single_subtest(&x_options_x);

         /*
          * Turn on the --sub-test option.
          */

         ok = unit_test_options_single_subtest_set(&x_options_x, 5);
         if (ok)
            ok = unit_test_status_next_subtest(&x_status_x, "skip sub-test");

         if (ok)
         {
            ok = x_status_x.m_Test_Options == &x_options_x;
            if (ok)
               ok = x_status_x.m_Subtest == 5;

            if (ok)
               ok = strcmp(x_status_x.m_Subtest_Name, "skip sub-test") == 0;
         }
         unit_test_status_pass(&status, ok);

         /*
          * Restore the --sub-test option.
          */

         (void) unit_test_options_single_subtest_set
         (
            &x_options_x, original_sub_test
         );
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "xxx"))
      {
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    This test demonstrates the usage of the beep facility.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   10. unit_test_status_beep()
 *
 * \test
 *    -  unit_test_status_beep()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_10 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 10,
      "unit_test_status_t", "unit_test_status_beep()"
   );
   if (ok)
   {
      if (unit_test_options_show_progress(options))
         fprintf(stdout, "  %s\n", _("This test plays a beep if interactive."));

      /*  1 */

      if (unit_test_status_next_subtest(&status, _("beep test")))
      {
         cbool_t do_it = unit_test_status_prompt
         (
            &status, _("Decide if you want to test the beep functionality...")
         );
         if (do_it)
         {
            unit_test_status_beep();
            (void) unit_test_status_response
            (
               &status, _("Pass this test if you heard the beep")
            );
         }

         /*
          * This is done inside the unit_test_prompt() function. It is
          * not needed here.
          *
          *    (void) unit_test_status_pass
          *    (
          *       &status,
          *       ! unit_test_status_is_aborted(&status) &&
          *       ! unit_test_status_is_failed(&status)
          *    );
          */
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "group-name"
 *    functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   11. unit_test_status_name()
 *
 * \test
 *    -  unit_test_status_group_name()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_11 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 11,
      "unit_test_status_t", "unit_test_status_group_name()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = cut_is_nullptr(unit_test_status_group_name(nullptr));
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         const char * group = unit_test_status_group_name(&status);
         ok = strcmp(group, "unit_test_status_t") == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Reassignment test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            /*
             * \todo
             *    Instead of using strcpy(), we could use
             *    xpccut_stringcopy() and also test that function.
             */

            const char * group;
            (void) strcpy(x_status_x.m_Group_Name, "locacion privato");
            group = unit_test_status_group_name(&x_status_x);
            ok = cut_not_nullptr(group);
            if (ok)
               ok = strcmp(group, "locacion privato") == 0;
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "case-name"
 *    functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   12. unit_test_status_case_name()
 *
 * \test
 *    -  unit_test_status_case_name()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_12 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 12,
      "unit_test_status_t", "unit_test_status_case_name()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = cut_is_nullptr(unit_test_status_case_name(nullptr));
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         const char * kase = unit_test_status_case_name(&status);
         ok = strcmp(kase, "unit_test_status_case_name()") == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Reassignment test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            const char * kase;
            (void) strcpy(x_status_x.m_Case_Description, "locacion privato");
            kase = unit_test_status_case_name(&x_status_x);
            ok = cut_not_nullptr(kase);
            if (ok)
               ok = strcmp(kase, "locacion privato") == 0;
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "sub-test-name"
 *    functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   13. unit_test_status_subtest_name()
 *
 * \test
 *    -  unit_test_status_subtest_name()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_13 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 13,
      "unit_test_status_t", "unit_test_status_subtest_name()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = cut_is_nullptr(unit_test_status_subtest_name(nullptr));
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         const char * subtest = unit_test_status_subtest_name(&status);
         ok = strcmp(subtest, "Normal test") == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Reassignment test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            const char * subtest;
            (void) strcpy(x_status_x.m_Subtest_Name, "locacion privato");
            subtest = unit_test_status_subtest_name(&x_status_x);
            ok = cut_not_nullptr(subtest);
            if (ok)
               ok = strcmp(subtest, "locacion privato") == 0;
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "group-number"
 *    functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   14. unit_test_status_group()
 *
 * \test
 *    -  unit_test_status_group()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_14 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 14,
      "unit_test_status_t", "unit_test_status_group()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_status_group(nullptr) ==
            XPCCUT_NO_CURRENT_GROUP;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         int group = unit_test_status_group(&status);
         ok = group == 2;
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Reassignment test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            int group;
            x_status_x.m_Test_Group = 96;
            group = unit_test_status_group(&x_status_x);
            ok = group == 96;
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "case-number"
 *    functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   15. unit_test_status_case()
 *
 * \test
 *    -  unit_test_status_case()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_15 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 15, "unit_test_status_t", "unit_test_status_case()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_status_case(nullptr) ==
            XPCCUT_NO_CURRENT_CASE;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         int kase = unit_test_status_case(&status);
         ok = kase == 15;
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Reassignment test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            int kase;
            x_status_x.m_Test_Case = 95;
            kase = unit_test_status_case(&x_status_x);
            ok = kase == 95;
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "sub-test-number"
 *    functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   16. unit_test_status_subtest()
 *
 * \test
 *    -  unit_test_status_subtest()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_16 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 16,
      "unit_test_status_t", "unit_test_status_subtest()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_status_subtest(nullptr) ==
            XPCCUT_NO_CURRENT_TEST;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         int subtest = unit_test_status_subtest(&status);
         ok = subtest == 2;
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Reassignment test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            int subtest;
            x_status_x.m_Subtest = 94;
            subtest = unit_test_status_subtest(&x_status_x);
            ok = subtest == 94;
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the
 *    "failed-sub-test-number" functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   17. unit_test_status_failed_subtest()
 *
 * \test
 *    -  unit_test_status_failed_subtest()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_17 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 17,
      "unit_test_status_t", "unit_test_status_failed_subtest()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_status_failed_subtest(nullptr) ==
            XPCCUT_NO_CURRENT_TEST;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         int subtest = unit_test_status_failed_subtest(&status);
         ok = subtest == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Reassignment test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            int subtest;
            x_status_x.m_Failed_Subtest = 93;
            subtest = unit_test_status_failed_subtest(&x_status_x);
            ok = subtest == 93;
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the
 *    "error-count-for-sub-tests" functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   18. unit_test_status_error_count()
 *
 * \test
 *    -  unit_test_status_error_count()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_18 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 18,
      "unit_test_status_t", "unit_test_status_error_count()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_status_error_count(nullptr) ==
            XPCCUT_INVALID_PARAMETER;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         int errcount = unit_test_status_error_count(&status);
         ok = errcount == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Reassignment test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            int errcount;
            x_status_x.m_Subtest_Error_Count = 92;
            errcount = unit_test_status_error_count(&x_status_x);
            ok = errcount == 92;
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "passed"
 *    functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   19. unit_test_status_passed()
 *
 * \test
 *    -  unit_test_status_passed()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_19 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 19,
      "unit_test_status_t", "unit_test_status_passed()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_passed(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         ok = unit_test_status_passed(&status);       /* incestuous, no?      */
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Zeroing test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Subtest_Error_Count = 0;
            ok = unit_test_status_passed(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Reassignment test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Subtest_Error_Count = 1;
            ok = ! unit_test_status_passed(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Negation test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Subtest_Error_Count = XPCCUT_INVALID_PARAMETER;
            ok = ! unit_test_status_passed(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "failed"
 *    functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   20. unit_test_status_failed()
 *
 * \test
 *    -  unit_test_status_failed()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_20 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 20,
      "unit_test_status_t", "unit_test_status_failed()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_status_failed(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         ok = ! unit_test_status_failed(&status);        /* incestuous, no?   */
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Zeroing test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Subtest_Error_Count = 0;
            ok = ! unit_test_status_failed(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Reassignment test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Subtest_Error_Count = 1;
            ok = unit_test_status_failed(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Negation test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Subtest_Error_Count = XPCCUT_INVALID_PARAMETER;
            ok = unit_test_status_failed(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   21. unit_test_status_disposition()
 *
 * \test
 *    -  unit_test_status_disposition()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_21 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 21,
      "unit_test_status_t", "unit_test_status_disposition()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         unit_test_disposition_t d = unit_test_status_disposition(nullptr);
         cbool_t null_ok = d == XPCCUT_DISPOSITION_ABORTED;
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         int d = unit_test_status_disposition(&status);
         ok = d == XPCCUT_DISPOSITION_CONTINUE;
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Reassignment test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            int d;
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_DNT;
            d = unit_test_status_disposition(&x_status_x);
            ok = d == XPCCUT_DISPOSITION_DNT;
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor for "continue" functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   22. unit_test_status_is_continue()
 *
 * \test
 *    -  unit_test_status_is_continue()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_22 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 22,
      "unit_test_status_t", "unit_test_status_is_continue()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_is_continue(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         ok = unit_test_status_is_continue(&status);
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "False test DNT"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_DNT;
            ok = ! unit_test_status_is_continue(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "False test FAILED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_FAILED;
            ok = ! unit_test_status_is_continue(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "False test QUITTED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_QUITTED;
            ok = ! unit_test_status_is_continue(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "False test ABORTED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_ABORTED;
            ok = ! unit_test_status_is_continue(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "False test integer"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = 92;
            ok = ! unit_test_status_is_continue(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "True test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_CONTINUE;
            ok = unit_test_status_is_continue(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor for "skipped" functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   23. unit_test_status_is_skipped()
 *
 * \test
 *    -  unit_test_status_is_skipped()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_23 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 23,
      "unit_test_status_t", "unit_test_status_is_skipped()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_is_skipped(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         ok = ! unit_test_status_is_skipped(&status);
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "True test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_DNT;
            ok = unit_test_status_is_skipped(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "False test FAILED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_FAILED;
            ok = ! unit_test_status_is_skipped(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "False test QUITTED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_QUITTED;
            ok = ! unit_test_status_is_skipped(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "False test ABORTED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_ABORTED;
            ok = ! unit_test_status_is_skipped(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "False test integer"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = 92;
            ok = ! unit_test_status_is_skipped(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "False test CONTINUE"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_CONTINUE;
            ok = ! unit_test_status_is_skipped(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor for "failed" functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   24. unit_test_status_is_failed()
 *
 * \test
 *    -  unit_test_status_is_failed()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_24 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 24,
      "unit_test_status_t", "unit_test_status_is_failed()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_is_failed(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         ok = ! unit_test_status_is_failed(&status);
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "False test DNT"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_DNT;
            ok = ! unit_test_status_is_failed(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "True test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_FAILED;
            ok = unit_test_status_is_failed(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "False test QUITTED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_QUITTED;
            ok = ! unit_test_status_is_failed(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "False test ABORTED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_ABORTED;
            ok = ! unit_test_status_is_failed(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "False test integer"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = 92;
            ok = ! unit_test_status_is_failed(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "False test CONTINUE"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_CONTINUE;
            ok = ! unit_test_status_is_failed(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor for "quitted" functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   25. unit_test_status_is_quitted()
 *
 * \test
 *    -  unit_test_status_is_quitted()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_25 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 25,
      "unit_test_status_t", "unit_test_status_is_quitted()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_is_quitted(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         ok = ! unit_test_status_is_quitted(&status);
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "False test DNT"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_DNT;
            ok = ! unit_test_status_is_quitted(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "False test FAILED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_FAILED;
            ok = ! unit_test_status_is_quitted(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "True test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_QUITTED;
            ok = unit_test_status_is_quitted(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "False test ABORTED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_ABORTED;
            ok = ! unit_test_status_is_quitted(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "False test integer"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = 92;
            ok = ! unit_test_status_is_quitted(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "False test CONTINUE"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_CONTINUE;
            ok = ! unit_test_status_is_quitted(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor for "aborted" functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   26. unit_test_status_is_aborted()
 *
 * \test
 *    -  unit_test_status_is_aborted()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_26 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 26,
      "unit_test_status_t", "unit_test_status_is_aborted()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_is_aborted(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         ok = ! unit_test_status_is_aborted(&status);
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "False test DNT"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_DNT;
            ok = ! unit_test_status_is_aborted(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "False test FAILED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_FAILED;
            ok = ! unit_test_status_is_aborted(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "False test QUITTED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_QUITTED;
            ok = ! unit_test_status_is_aborted(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "True test"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_ABORTED;
            ok = unit_test_status_is_aborted(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "False test integer"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = 92;
            ok = ! unit_test_status_is_aborted(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "False test CONTINUE"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_CONTINUE;
            ok = ! unit_test_status_is_aborted(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor for "okay" functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   27. unit_test_status_is_okay()
 *
 * \test
 *    -  unit_test_status_is_okay()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_27 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 27,
      "unit_test_status_t", "unit_test_status_is_okay()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_status_is_okay(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         ok = unit_test_status_is_okay(&status);
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "True test DNT"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_DNT;
            ok = unit_test_status_is_okay(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "False test FAILED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_FAILED;
            ok = ! unit_test_status_is_okay(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      /**
       * \todo
       *    Still need to determine if QUITTED is an appropriate "okay"
       *    status.
       */

      if (unit_test_status_next_subtest(&status, "False test QUITTED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_QUITTED;
            ok = ! unit_test_status_is_okay(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "False test ABORTED"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_ABORTED;
            ok = ! unit_test_status_is_okay(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "False test integer"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = 92;
            ok = ! unit_test_status_is_okay(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "True test CONTINUE"))
      {
         unit_test_status_t x_status_x;
         ok = unit_test_status_init(&x_status_x);
         if (ok)
         {
            x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_CONTINUE;
            ok = unit_test_status_is_okay(&x_status_x);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the duration accessor
 *    functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   28. unit_test_status_duration_ms()
 *
 * \test
 *    -  unit_test_status_duration_ms()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_28 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 28,
      "unit_test_status_t", "unit_test_status_duration_ms()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         double d = unit_test_status_duration_ms(nullptr);
         unit_test_status_pass(&status, (d == -1.0));
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Normal test"))
      {
         double d = unit_test_status_duration_ms(&status);
         unit_test_status_pass(&status, (d == 0.0));
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Reassignment test"))
      {
         double d;
         unit_test_status_t x_status_x;
         unit_test_status_init(&x_status_x);
         d = unit_test_status_duration_ms(&x_status_x);
         ok = (d == 0.0);
         if (ok)
         {
            x_status_x.m_Test_Duration_ms = 90.01;
            d = unit_test_status_duration_ms(&x_status_x);
            ok = (d == 90.01);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the integer-checking
 *    functionality works.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   29. unit_test_status_int_check()
 *
 * \test
 *    -  unit_test_status_int_check()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_29 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 29,
      "unit_test_status_t", "unit_test_status_int_check()"
   );
   if (ok)
   {
      /**
       *    The first sub-test sets a test group/case of 99/99.  It copies
       *    the provided \e options parameter, and then modified it slightly
       *    so that the output is less confusing to a user.  The output
       *    value is incremented internally, so "TEST 1000" is shown.
       */

      unit_test_status_t x_status_x;
      unit_test_options_t x_options_x = *options;  /* copy existing options   */
      ok = unit_test_options_current_test_set(&x_options_x, g_BOGUS_TEST_NUMBER);
      if (ok)
      {
         ok = unit_test_status_initialize
         (
            &x_status_x, &x_options_x, 99, 99,
            "x_status_x", _("internal test")
         );
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this' part 1"))
      {
         cbool_t null_ok = ! unit_test_status_int_check(nullptr, 0, 0);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this' part 2"))
      {
         cbool_t null_ok = ! unit_test_status_int_check(nullptr, 0, 1);
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Normal test of match"))
      {
         if (ok)
            ok = unit_test_status_int_check(&x_status_x, 0, 0);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Normal test of mis-match"))
      {
         if (ok)
         {
            ok = ! unit_test_status_int_check(&x_status_x, 0, 1);
            show_deliberate_failure(options);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the integer-checking
 *    functionality works.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   30. unit_test_status_string_check()
 *
 * \test
 *    -  unit_test_status_string_check()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_30 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 30,
      "unit_test_status_t", "unit_test_status_string_check()"
   );
   if (ok)
   {
      const char * s1 = "123";
      const char * s2 = "321";
      unit_test_status_t x_status_x;
      unit_test_options_t x_options_x = *options;  /* copy existing options   */
      ok = unit_test_options_current_test_set(&x_options_x, g_BOGUS_TEST_NUMBER);
      if (ok)
      {
         ok = unit_test_status_initialize
         (
            &x_status_x, &x_options_x, 99, 99,
            "x_status_x", _("internal test")
         );
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this' part 1"))
      {
         cbool_t null_ok = ! unit_test_status_string_check(nullptr, s1, s1);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this' part 2"))
      {
         cbool_t null_ok = ! unit_test_status_string_check(nullptr, s1, s2);
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Null 'this' part 3"))
      {
         cbool_t null_ok = ! unit_test_status_string_check(nullptr, nullptr, s2);
         unit_test_status_pass(&status, null_ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Null 'this' part 4"))
      {
         cbool_t null_ok = ! unit_test_status_string_check(nullptr, s1, nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Null 'this' part 5"))
      {
         cbool_t null_ok = ! unit_test_status_string_check
         (
            nullptr, nullptr, nullptr
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Normal test of match"))
      {
         if (ok)
            ok = unit_test_status_string_check(&x_status_x, s1, s1);

         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "Normal test of mis-match"))
      {
         if (ok)
         {
            ok = ! unit_test_status_string_check(&x_status_x, s1, s2);
            show_deliberate_failure(options);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "Normal null match"))
      {
         if (ok)
            ok = unit_test_status_string_check(&x_status_x, nullptr, nullptr);

         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "Normal null mis-match, part 1"))
      {
         if (ok)
         {
            ok = ! unit_test_status_string_check(&x_status_x, nullptr, s2);
            show_deliberate_failure(options);
         }
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "Normal null mis-match, part 2"))
      {
         if (ok)
         {
            ok = ! unit_test_status_string_check(&x_status_x, s1, nullptr);
            show_deliberate_failure(options);
         }
         unit_test_status_pass(&status, ok);
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "Normal empty-string match"))
      {
         if (ok)
            ok = unit_test_status_string_check(&x_status_x, "", "");

         unit_test_status_pass(&status, ok);
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "Null+empty-string mismatch 1"))
      {
         if (ok)
         {
            ok = ! unit_test_status_string_check(&x_status_x, nullptr, "");
            show_deliberate_failure(options);
         }
         unit_test_status_pass(&status, ok);
      }

      /* 13 */

      if (unit_test_status_next_subtest(&status, "Null+empty-string mismatch 2"))
      {
         if (ok)
         {
            ok = ! unit_test_status_string_check(&x_status_x, "", nullptr);
            show_deliberate_failure(options);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the boolean-checking
 *    functionality works.
 *
 * \group
 *    2. unit_test_status_t functions.
 *
 * \case
 *   31. unit_test_status_bool_check()
 *
 * \test
 *    -  unit_test_status_bool_check()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_02_31 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 31,
      "unit_test_status_t", "unit_test_status_bool_check()"
   );
   if (ok)
   {
      unit_test_status_t x_status_x;
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      if (ok)
      {
         ok = unit_test_options_current_test_set
         (
            &x_options_x, g_BOGUS_TEST_NUMBER
         );
      }
      if (ok)
      {
         ok = unit_test_status_initialize
         (
            &x_status_x, options, 99, 99, "x_status_x", _("internal test")
         );
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this' part 1"))
      {
         cbool_t null_ok = ! unit_test_status_bool_check(nullptr, false, false);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this' part 2"))
      {
         cbool_t null_ok = ! unit_test_status_bool_check(nullptr, false, true);
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Null 'this' part 3"))
      {
         cbool_t null_ok = ! unit_test_status_bool_check(nullptr, true, false);
         unit_test_status_pass(&status, null_ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Normal test of match"))
      {
         if (ok)
            ok = unit_test_status_bool_check(&x_status_x, false, false);

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Normal test of mis-match"))
      {
         if (ok)
         {
            ok = ! unit_test_status_bool_check(&x_status_x, false, true);
            show_deliberate_failure(options);
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Normal test of mis-match"))
      {
         if (ok)
         {
            ok = ! unit_test_status_bool_check(&x_status_x, true, false);
            show_deliberate_failure(options);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the basic
 *    initialization function properly zeros or clears all fields to their
 *    "default" values.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *    1. unit_test_options_init()
 *
 * \test
 *    -  unit_test_options_init()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 1,
      "unit_test_options_t", "unit_test_options_init()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);

      /*
       * This setting ensures that the user will see "Simulated TEST",
       * rather than be confused by seeing bogus-looking "TEST" numbers.
       */

      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_options_init(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Good 'this'"))
      {
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "m_Is_Verbose"))
      {
         /*
          * Let's use the recent addition to our unit-test armaments:
          *
          * ok = x_options_x.m_Is_Verbose == XPCCUT_IS_VERBOSE;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Is_Verbose, XPCCUT_IS_VERBOSE
         );
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "m_Show_Values"))
      {
         /*
          * ok = x_options_x.m_Show_Values == XPCCUT_SHOW_VALUES;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Show_Values, XPCCUT_SHOW_VALUES
         );
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "m_Text_Synch"))
      {
         /*
          * ok = x_options_x.m_Text_Synch == XPCCUT_TEXT_SYNCH;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Text_Synch, XPCCUT_TEXT_SYNCH
         );
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "m_Show_Step_Numbers"))
      {
         /*
          * ok = x_options_x.m_Show_Step_Numbers == XPCCUT_SHOW_STEP_NUMBERS;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Show_Step_Numbers, XPCCUT_SHOW_STEP_NUMBERS
         );
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "m_Show_Progress"))
      {
         /*
          * ok = x_options_x.m_Show_Progress == XPCCUT_SHOW_PROGRESS;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Show_Progress, XPCCUT_SHOW_PROGRESS
         );
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "m_Stop_On_Error"))
      {
         /*
          * ok = x_options_x.m_Stop_On_Error == XPCCUT_STOP_ON_ERROR;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Stop_On_Error, XPCCUT_STOP_ON_ERROR
         );
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "m_Batch_Mode"))
      {
         /*
          * ok = x_options_x.m_Batch_Mode == XPCCUT_BATCH_MODE;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Batch_Mode, XPCCUT_BATCH_MODE
         );
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "m_Interactive"))
      {
         /*
          * ok = x_options_x.m_Interactive == XPCCUT_INTERACTIVE;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Interactive, XPCCUT_INTERACTIVE
         );
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "m_Beep_Prompt"))
      {
         /*
          * ok = x_options_x.m_Beep_Prompt == XPCCUT_BEEP;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Beep_Prompt, XPCCUT_BEEP
         );
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "m_Summarize"))
      {
         /*
          * ok = x_options_x.m_Summarize == XPCCUT_SUMMARIZE;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Summarize, XPCCUT_SUMMARIZE
         );
      }

      /* 13 */

      if (unit_test_status_next_subtest(&status, "m_Case_Pause"))
      {
         /*
          * ok = x_options_x.m_Case_Pause == XPCCUT_CASE_PAUSE;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Case_Pause, XPCCUT_CASE_PAUSE
         );
      }

      /* 14 */

      if (unit_test_status_next_subtest(&status, "m_Single_Test_Group"))
      {
         /*
          * ok = x_options_x.m_Single_Test_Group == XPCCUT_NO_SINGLE_GROUP;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Single_Test_Group, XPCCUT_NO_SINGLE_GROUP
         );
      }

      /* 15 */

      if (unit_test_status_next_subtest(&status, "m_Single_Test_Case"))
      {
         /*
          * ok = x_options_x.m_Single_Test_Case == XPCCUT_NO_SINGLE_CASE;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Single_Test_Case, XPCCUT_NO_SINGLE_CASE
         );
      }

      /* 16 */

      if (unit_test_status_next_subtest(&status, "m_Single_Sub_Test"))
      {
         /*
          * ok = x_options_x.m_Single_Sub_Test == XPCCUT_NO_SINGLE_SUB_TEST;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Single_Sub_Test, XPCCUT_NO_SINGLE_SUB_TEST
         );
      }

      /* 17 */

      if (unit_test_status_next_subtest(&status, "m_Test_Sleep_Time"))
      {
         /*
          * ok = x_options_x.m_Test_Sleep_Time == XPCCUT_TEST_SLEEP_TIME;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Test_Sleep_Time, XPCCUT_TEST_SLEEP_TIME
         );
         }

      /* 18 */

      if (unit_test_status_next_subtest(&status, "m_Need_Subtests"))
      {
         /*
          * ok = x_options_x.m_Need_Subtests == XPCCUT_NEED_SUBTESTS;
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_bool_check
         (
            &status, x_options_x.m_Need_Subtests, XPCCUT_NEED_SUBTESTS
         );
      }

      /* 19 */

      if (unit_test_status_next_subtest(&status, "m_Current_Test_Number"))
      {
         ok = unit_test_status_int_check
         (
            &status, x_options_x.m_Current_Test_Number,
            XPCCUT_NO_CURRENT_TEST
         );
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the basic
 *    initialization function properly zeros or clears all fields to their
 *    "default" values.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *    2. unit_test_options_initialize()
 *
 * \test
 *    -  unit_test_options_initialize()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_02 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 2,
      "unit_test_options_t", "unit_test_options_initialize()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;

      /*
       * Start out with default values so that we can take the opposite tack
       * with the full initialization function.
       */

      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      if (ok)
      {
         /**
          * This test also modifies the options structure with the opposite
          * values (or at least different values) for every accessible
          * field, so that the resulting test is a bizarro-clone of test
          * 03_01.
          *
          * \note
          *    -  The unit_test_options_initialize() function does not
          *       provide parameters for the single test group, case, and
          *       sub-test names, the need-subtests option, the
          *       force-failure option, the current-test number, the
          *       response-before and response-after values, and the
          *       is-simulated value.  It does set them all in a hard-wired
          *       fashion, though.
          *    -  It is important to realize that the settings made by
          *       unit_test_options_initialize() can be a bit different
          *       from the settings made by calling each of the setter
          *       functions individually with the same values.  The reason
          *       is that the following setters have side-effects on other
          *       fields in the unit_test_options_t structure:
          *       -  unit_test_options_batch_mode_set()
          *       -  unit_test_options_show_progress_set()
          *       -  unit_test_options_summary_set()
          */

         ok = unit_test_options_initialize
         (
            &x_options_x,
            ! XPCCUT_IS_VERBOSE,          /* verbose           */
            ! XPCCUT_SHOW_VALUES,         /* showvalues        */
            ! XPCCUT_TEXT_SYNCH,          /* usetextsynch      */
            ! XPCCUT_SHOW_STEP_NUMBERS,   /* showstepnumbers   */
            ! XPCCUT_SHOW_PROGRESS,       /* showprogress      */
            ! XPCCUT_STOP_ON_ERROR,       /* stoponerror       */
            ! XPCCUT_BATCH_MODE,          /* batchmode         */
            ! XPCCUT_INTERACTIVE,         /* interactive       */
            ! XPCCUT_BEEP,                /* beeps             */
            ! XPCCUT_SUMMARIZE,           /* summarize         */
            ! XPCCUT_CASE_PAUSE,          /* casepause         */
            XPCCUT_NO_SINGLE_GROUP+1,     /* singlegroup       */
            XPCCUT_NO_SINGLE_CASE+2,      /* singlecase        */
            XPCCUT_NO_SINGLE_SUB_TEST+3,  /* singlesubtest     */
            XPCCUT_TEST_SLEEP_TIME+4      /* testsleeptime     */
         );
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_options_initialize
         (
            nullptr,
            false, false, false, false, false, false,
            false, false, false, false, false,
            0, 0, 0, 0
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Good 'this'"))
      {
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "m_Is_Verbose"))
      {
         /*
          * Rather than use the recent unit_test_status_bool_check()
          * function, like we did in 03_01 above, we'll leave these tests as
          * they are.  This shows another way to do the checks, for more
          * general comparisons or tests.
          */

         ok = x_options_x.m_Is_Verbose != XPCCUT_IS_VERBOSE;
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "m_Show_Values"))
      {
         ok = x_options_x.m_Show_Values != XPCCUT_SHOW_VALUES;
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "m_Text_Synch"))
      {
         ok = x_options_x.m_Text_Synch != XPCCUT_TEXT_SYNCH;
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "m_Show_Step_Numbers"))
      {
         ok = x_options_x.m_Show_Step_Numbers != XPCCUT_SHOW_STEP_NUMBERS;
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "m_Show_Progress"))
      {
         ok = x_options_x.m_Show_Progress != XPCCUT_SHOW_PROGRESS;
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "m_Stop_On_Error"))
      {
         ok = x_options_x.m_Stop_On_Error != XPCCUT_STOP_ON_ERROR;
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "m_Batch_Mode"))
      {
         ok = x_options_x.m_Batch_Mode != XPCCUT_BATCH_MODE;
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "m_Interactive"))
      {
         ok = x_options_x.m_Interactive != XPCCUT_INTERACTIVE;
         unit_test_status_pass(&status, ok);
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "m_Beep_Prompt"))
      {
         ok = x_options_x.m_Beep_Prompt != XPCCUT_BEEP;
         unit_test_status_pass(&status, ok);
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "m_Summarize"))
      {
         ok = x_options_x.m_Summarize != XPCCUT_SUMMARIZE;
         unit_test_status_pass(&status, ok);
      }

      /* 13 */

      if (unit_test_status_next_subtest(&status, "m_Case_Pause"))
      {
         ok = x_options_x.m_Case_Pause != XPCCUT_CASE_PAUSE;
         unit_test_status_pass(&status, ok);
      }

      /* 14 */

      if (unit_test_status_next_subtest(&status, "m_Single_Test_Group"))
      {
         /*
          * Let's use the recent addition to our unit-test armaments for
          * these integer checks.
          *
          * ok = x_options_x.m_Single_Test_Group == (XPCCUT_NO_SINGLE_GROUP+1);
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_int_check
         (
            &status, x_options_x.m_Single_Test_Group, XPCCUT_NO_SINGLE_GROUP+1
         );
      }

      /* 15 */

      if (unit_test_status_next_subtest(&status, "m_Single_Test_Case"))
      {
         /*
          * ok = x_options_x.m_Single_Test_Case == (XPCCUT_NO_SINGLE_CASE+2);
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_int_check
         (
            &status, x_options_x.m_Single_Test_Case, XPCCUT_NO_SINGLE_CASE+2
         );
      }

      /* 16 */

      if (unit_test_status_next_subtest(&status, "m_Single_Sub_Test"))
      {
         /*
          * ok = x_options_x.m_Single_Sub_Test == (XPCCUT_NO_SINGLE_SUB_TEST+3);
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_int_check
         (
            &status, x_options_x.m_Single_Sub_Test, XPCCUT_NO_SINGLE_SUB_TEST+3
         );
      }

      /* 17 */

      if (unit_test_status_next_subtest(&status, "m_Test_Sleep_Time"))
      {
         /*
          * ok = x_options_x.m_Test_Sleep_Time == (XPCCUT_TEST_SLEEP_TIME+4);
          * unit_test_status_pass(&status, ok);
          */

         ok = unit_test_status_int_check
         (
            &status, x_options_x.m_Test_Sleep_Time, XPCCUT_TEST_SLEEP_TIME+4
         );
      }

      /* 18 */

      if (unit_test_status_next_subtest(&status, "m_Need_Subtests"))
      {
         ok = x_options_x.m_Need_Subtests == XPCCUT_NEED_SUBTESTS;
         unit_test_status_pass(&status, ok);
      }

      /* 19 */

      if (unit_test_status_next_subtest(&status, "m_Current_Test_Number"))
      {
         ok = unit_test_status_int_check
         (
            &status, x_options_x.m_Current_Test_Number,
            XPCCUT_NO_CURRENT_TEST
         );
      }
   }
   return status;
}

/**
 *    Provides a constant for allocation simulated command-line arguments.
 */

#define FULL_ARG_COUNT           24

/**
 *    Provides a unit/regression test to verify that the parse functionality
 *    handles turning off the options.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *    3. unit_test_options_parse(), part 1.
 *
 * \test
 *    -  unit_test_options_parse()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_03 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 3,
      "unit_test_options_t", "unit_test_options_parse() part 1"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 1;                                /* 1 more than last index  */
      argv[0] = "<unknown>";                       /* guarantee some value    */

      /*
       * Start out with default values so that we can take the opposite tack
       * with the full initialization function.
       */

      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      if (ok)
      {
         /*
          * Now modify the options structure with the opposite values (or at
          * least different values) for every accessible field, so that the
          * resulting test is a bizarro-clone of test 03_01.
          */

         argv[0]  = "unit_test_test";              /* application name        */
         argv[1]  = "--no-verbose";
         argv[2]  = "--no-show-progress";          /* "--no-silent"           */
         argv[3]  = "--no-show-values";
         argv[4]  = "--no-show-step-numbers";
         argv[5]  = "--no-text-synch";
         argv[6]  = "--no-stop-on-error";
         argv[7]  = "--no-batch-mode";
         argv[8]  = "--no-interactive";
         argv[9]  = "--no-beeps";
         argv[10] = "--no-case-pause";
         argv[11] = "--no-summarize";
         argv[12] = "--group";
         argv[13] = "1";
         argv[14] = "--case";
         argv[15] = "2";
         argv[16] = "--sub-test";
         argv[17] = "3";
         argv[18] = "--sleep-time";
         argv[19] = "4";
         argv[20] = "--no-verbose";
         argv[21] = "--no-verbose";
         argc = 22;
         ok = unit_test_options_parse
         (
            &x_options_x, argc, argv,
            "Test 03.03", "version", "additionalhelp"
         );
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_options_parse
         (
            nullptr, argc, argv,
            "Test 03.03", "version", "additionalhelp"
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Good 'this'"))
      {
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "m_Is_Verbose"))
      {
         ok = ! x_options_x.m_Is_Verbose;
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "m_Show_Values"))
      {
         ok = ! x_options_x.m_Show_Values;
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "m_Text_Synch"))
      {
         ok = ! x_options_x.m_Text_Synch;
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "m_Show_Step_Numbers"))
      {
         ok = ! x_options_x.m_Show_Step_Numbers;
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "m_Show_Progress"))
      {
         ok = ! x_options_x.m_Show_Progress;
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "m_Stop_On_Error"))
      {
         ok = ! x_options_x.m_Stop_On_Error;
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "m_Batch_Mode"))
      {
         ok = ! x_options_x.m_Batch_Mode;
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "m_Interactive"))
      {
         ok = ! x_options_x.m_Interactive;
         unit_test_status_pass(&status, ok);
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "m_Beep_Prompt"))
      {
         ok = ! x_options_x.m_Beep_Prompt;
         unit_test_status_pass(&status, ok);
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "m_Summarize"))
      {
         ok = ! x_options_x.m_Summarize;
         unit_test_status_pass(&status, ok);
      }

      /* 13 */

      if (unit_test_status_next_subtest(&status, "m_Case_Pause"))
      {
         ok = ! x_options_x.m_Case_Pause;
         unit_test_status_pass(&status, ok);
      }

      /* 14 */

      if (unit_test_status_next_subtest(&status, "m_Single_Test_Group"))
      {
         ok = x_options_x.m_Single_Test_Group == (XPCCUT_NO_SINGLE_GROUP+1);
         unit_test_status_pass(&status, ok);
      }

      /* 15 */

      if (unit_test_status_next_subtest(&status, "m_Single_Test_Case"))
      {
         ok = x_options_x.m_Single_Test_Case == (XPCCUT_NO_SINGLE_CASE+2);
         unit_test_status_pass(&status, ok);
      }

      /* 16 */

      if (unit_test_status_next_subtest(&status, "m_Single_Sub_Test"))
      {
         ok = x_options_x.m_Single_Sub_Test == (XPCCUT_NO_SINGLE_SUB_TEST+3);
         unit_test_status_pass(&status, ok);
      }

      /* 17 */

      if (unit_test_status_next_subtest(&status, "m_Test_Sleep_Time"))
      {
         ok = x_options_x.m_Test_Sleep_Time == (XPCCUT_TEST_SLEEP_TIME+4);
         unit_test_status_pass(&status, ok);
      }

      /* 18 */

      if (unit_test_status_next_subtest(&status, "m_Need_Subtests"))
      {
         ok = ! x_options_x.m_Need_Subtests;
         unit_test_status_pass(&status, ok);
      }

      /* 19 */

      if (unit_test_status_next_subtest(&status, "m_Current_Test_Number"))
      {
         ok = x_options_x.m_Current_Test_Number == XPCCUT_NO_CURRENT_TEST;
         unit_test_status_pass(&status, ok);
      }

#ifdef THIS_IS_POSSIBLE

      /* 20 */

      if (unit_test_status_next_subtest(&status, "Help text"))
      {
         /*
          *    "testname", "version", "additionalhelp"
          *
          * We want to test the three additional parameters.  However, there's
          * no easy way to see that the test-name got packed into the help text
          * and know that we are correct.  And we can't even verify that the
          * testname is \e somewhere in the help text, since the output
          * buffer is freed as soon as it is printed.
          *
          *    char * substring = strstr
          *    (
          *       x_options_x.m_HELPTEXT, "testname"
          *    );
          *    ok = cut_not_nullptr(substring);
          */

         unit_test_status_pass(&status, ok);
      }

#endif      /* THIS_IS_POSSIBLE */

   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the parse functionality
 *    handles turning on the options fields.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *    4. unit_test_options_parse(), part 2.
 *
 * \test
 *    -  unit_test_options_parse()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_04 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 4,
      "unit_test_options_t", "unit_test_options_parse() part 2"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 1;                                /* guarantee initialized   */
      argv[0] = "<unknown>";                       /* guarantee some value    */

      /*
       * Start out with default values so that we can take the opposite tack
       * with the full initialization function.
       */

      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok;
         argv[0] = "N/A";
         argc = 1;
         null_ok = ! unit_test_options_parse
         (
            nullptr, argc, argv,
            "Test 03.04", "version", "additionalhelp"
         );
         if (null_ok)
         {
            null_ok = ! unit_test_options_parse
            (
               &x_options_x, 0, argv,
               "Test 03.04", "version", "additionalhelp"
            );
         }
         if (null_ok)
         {
            null_ok = ! unit_test_options_parse
            (
               &x_options_x, argc, nullptr,
               "Test 03.04", "version", "additionalhelp"
            );
         }
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      if (ok)
      {
         /*
          * Now modify the options structure with the opposite values (or at
          * least different values) for every accessible field, so that the
          * resulting test is a bizarro-clone of test 03_01.
          *
          * Also note that some options turn off the global output option or
          * alter other options.  We disable them by tacking on "---ignore".
          *
          * The --summarize option does this, and the output looks weird.
          */

         argv[0]  = "unit_test_test";
         argv[1]  = "--verbose";
         argv[2]  = "--show-progress";
         argv[3]  = "--show-values";
         argv[4]  = "--show-step-numbers";
         argv[5]  = "--text-synch";
         argv[6]  = "--stop-on-error";
         argv[7]  = "--batch-mode---ignore";
         argv[8]  = "--interactive---ignore";
         argv[9]  = "--beeps";
         argv[10] = "--case-pause";
         argv[11] = "--summarize---ignore";
         argv[12] = "--group";
         argv[13] = "1";
         argv[14] = "--case";
         argv[15] = "2";
         argv[16] = "--sub-test";
         argv[17] = "3";
         argv[18] = "--sleep-time";
         argv[19] = "4";
         argc = 20;
         ok = unit_test_options_parse
         (
            &x_options_x, argc, argv,
            "Test 03.04", "version", "additionalhelp"
         );
      }
      if (unit_test_status_next_subtest(&status, "Good 'this'"))
      {
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "m_Is_Verbose"))
      {
         ok = x_options_x.m_Is_Verbose;
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "m_Show_Values"))
      {
         ok = x_options_x.m_Show_Values;
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "m_Text_Synch"))
      {
         ok = x_options_x.m_Text_Synch;
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "m_Show_Step_Numbers"))
      {
         ok = x_options_x.m_Show_Step_Numbers;
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "m_Show_Progress"))
      {
         ok = x_options_x.m_Show_Progress;
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "m_Stop_On_Error"))
      {
         ok = x_options_x.m_Stop_On_Error;
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "m_Batch_Mode"))
      {
         ok = ! x_options_x.m_Batch_Mode;
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "m_Interactive"))
      {
         ok = ! x_options_x.m_Interactive;
         unit_test_status_pass(&status, ok);
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "m_Beep_Prompt"))
      {
         ok = x_options_x.m_Beep_Prompt;
         unit_test_status_pass(&status, ok);
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "m_Summarize"))
      {
         ok = ! x_options_x.m_Summarize;
         unit_test_status_pass(&status, ok);
      }

      /* 13 */

      if (unit_test_status_next_subtest(&status, "m_Case_Pause"))
      {
         ok = x_options_x.m_Case_Pause;
         unit_test_status_pass(&status, ok);
      }

      /* 14 */

      if (unit_test_status_next_subtest(&status, "m_Single_Test_Group"))
      {
         ok = x_options_x.m_Single_Test_Group == (XPCCUT_NO_SINGLE_GROUP+1);
         unit_test_status_pass(&status, ok);
      }

      /* 15 */

      if (unit_test_status_next_subtest(&status, "m_Single_Test_Case"))
      {
         ok = x_options_x.m_Single_Test_Case == (XPCCUT_NO_SINGLE_CASE+2);
         unit_test_status_pass(&status, ok);
      }

      /* 16 */

      if (unit_test_status_next_subtest(&status, "m_Single_Sub_Test"))
      {
         ok = x_options_x.m_Single_Sub_Test == (XPCCUT_NO_SINGLE_SUB_TEST+3);
         unit_test_status_pass(&status, ok);
      }

      /* 17 */

      if (unit_test_status_next_subtest(&status, "m_Test_Sleep_Time"))
      {
         ok = x_options_x.m_Test_Sleep_Time == (XPCCUT_TEST_SLEEP_TIME+4);
         unit_test_status_pass(&status, ok);
      }

      /* 18 */

      if (unit_test_status_next_subtest(&status, "m_Need_Subtests"))
      {
         ok = ! x_options_x.m_Need_Subtests;
         unit_test_status_pass(&status, ok);
      }

      /* 18 */

      if (unit_test_status_next_subtest(&status, "m_Current_Test_Number"))
      {
         ok = x_options_x.m_Current_Test_Number == XPCCUT_NO_CURRENT_TEST;
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the batch-mode,
 *    interactive-mode, summarize, and other tests that affect more than one
 *    flag at a time.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *    5. unit_test_options_parse(), part 3, plus some output options.
 *
 * \test
 *    -  unit_test_options_parse()
 *    -  xpccut_is_silent() [indirectly]
 *    -  xpccut_silence_printing() [indirectly]
 *    -  xpccut_allow_printing() [indirectly]
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_05 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 5,
      "unit_test_options_t", "unit_test_options_parse() part 3"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      cbool_t original_silence_setting;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 1;                                /* guarantee initialized   */
      argv[0] = "<unknown>";                       /* guarantee some value    */

      /**
       * Obtain the silence-setting that was specified on the command
       * line, so that it can be restored at the end of some of the
       * sub-tests.  If the user didn't specify --summarize, this value
       * should be false by default.
       */

      original_silence_setting = xpccut_is_silent();

      /*  1 */

      /*
       * Start out with default values so that we can take the opposite tack
       * with the full initialization function.
       */

      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      if (unit_test_status_next_subtest(&status, "--batch-mode test"))
      {
         if (ok)
         {
            /*
             * Force some options to be the opposite of what the test should
             * result in.  Not completely airtight, but good enough.
             *
             * Here, we set only batch-mode, which also sets the
             * show-progress mode off.
             */

            x_options_x.m_Interactive        = true;
            x_options_x.m_Show_Step_Numbers  = true;
            x_options_x.m_Show_Values        = true;
            x_options_x.m_Is_Verbose         = true;
            x_options_x.m_Summarize          = false;
            x_options_x.m_Need_Subtests      = false;

            /*
             * Now run a simulated command-line.
             */

            argv[0]  = "unit_test_test";
            argv[1]  = "--batch-mode";
            argv[2]  = "--interactive---ignore";
            argv[3] = "--summarize---ignore";
            argc = 4;
            ok = unit_test_options_parse
            (
               &x_options_x, argc, argv,
               "03.05", "version", "additionalhelp"
            );
         }

         /*
          * This stilted style makes it easy to catch error under a
          * debugger.
          */

         if (ok)
            ok = x_options_x.m_Interactive;              /* new */

         if (ok)
            ok = x_options_x.m_Show_Step_Numbers;        /* new */

         if (ok)
            ok = ! x_options_x.m_Show_Values;

         if (ok)
            ok = ! x_options_x.m_Is_Verbose;

         if (ok)
            ok = ! x_options_x.m_Summarize;

         if (ok)
            ok = ! x_options_x.m_Need_Subtests;

         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (ok)
         ok = unit_test_options_init(&x_options_x);

      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      if (unit_test_status_next_subtest(&status, "--interactive re-test"))
      {
         if (ok)
         {
            x_options_x.m_Interactive = false;
            argv[0]  = "unit_test_test";
            argv[1]  = "--batch-mode--ignore";     /* affects other options   */
            argv[2]  = "--interactive";
            argv[3]  = "--summarize---ignore";     /* affects other options   */
            argc = 4;
            ok = unit_test_options_parse
            (
               &x_options_x, argc, argv,
               "03.05", "version", "additionalhelp"
            );
         }
         if (ok)
            ok = x_options_x.m_Interactive;

         if (ok)
            ok = ! x_options_x.m_Show_Step_Numbers;

         if (ok)
            ok = ! x_options_x.m_Show_Values;

         if (ok)
            ok = ! x_options_x.m_Is_Verbose;

         if (ok)
            ok = ! x_options_x.m_Summarize;

         if (ok)
            ok = ! x_options_x.m_Need_Subtests;

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (ok)
         ok = unit_test_options_init(&x_options_x);

      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      if (unit_test_status_next_subtest(&status, "--summarize test"))
      {
         if (ok)
         {
            x_options_x.m_Interactive        = true;
            x_options_x.m_Summarize          = true;
            x_options_x.m_Case_Pause         = true;
            argv[0]  = "unit_test_test";
            argv[1]  = "--batch-mode---ignore";
            argv[2]  = "--interactive---ignore";
            argv[3]  = "--summarize";              /* affects other options   */
            argc = 4;
            ok = unit_test_options_parse
            (
               &x_options_x, argc, argv,
               "03.05", "version", "additionalhelp"
            );
         }
         if (ok)
         {
            /*
             * --summarize no longer sets this variable.
             *
             * ok = xpccut_is_silent();            // did --summarize work?
             */

            /*
             * Now restore the original setting of the silence flag, so that
             * the output of the unit-test will have the appearance expected
             * by the dude who ran this test.
             */

            if (original_silence_setting)
               xpccut_silence_printing();
            else
               xpccut_allow_printing();
         }

         /*
          * We check more than we really need to, here.
          */

         if (ok)
            ok = ! x_options_x.m_Interactive;

         if (ok)
            ok = ! x_options_x.m_Show_Step_Numbers;

         if (ok)
            ok = ! x_options_x.m_Show_Values;

         if (ok)
            ok = ! x_options_x.m_Is_Verbose;

         if (ok)
            ok = ! x_options_x.m_Case_Pause;

         if (ok)
            ok = x_options_x.m_Summarize;

         if (ok)
            ok = ! x_options_x.m_Need_Subtests;

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (ok)
         ok = unit_test_options_init(&x_options_x);

      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      if (unit_test_status_next_subtest(&status, "--version test"))
      {
         cbool_t null_ok;
         xpccut_silence_printing();
         argv[0]  = "unit_test_test";
         argv[1]  = "--version";
         argc = 2;
         null_ok = ! unit_test_options_parse
         (
            &x_options_x, argc, argv,
            "03.05", nullptr, "additionalhelp"
         );
         if (null_ok)
         {
            null_ok = ! unit_test_options_parse
            (
               &x_options_x, argc, argv,
               "03.05", "version", "additionalhelp"
            );
         }

         /*
          * Restore the original setting of the silence flag.
          */

         if (original_silence_setting)
            xpccut_silence_printing();
         else
            xpccut_allow_printing();

         unit_test_status_pass(&status, null_ok);
      }

      /*  4 */

      if (ok)
         ok = unit_test_options_init(&x_options_x);

      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      if (unit_test_status_next_subtest(&status, "--help test"))
      {
         cbool_t null_ok;
         xpccut_silence_printing();
         argv[0]  = "unit_test_test";
         argv[1]  = "--help";
         argc = 2;
         null_ok = ! unit_test_options_parse
         (
            &x_options_x, argc, argv, "03.05", "version", nullptr
         );
         if (null_ok)
         {
            null_ok = ! unit_test_options_parse
            (
               &x_options_x, argc, argv,
               nullptr, "version", "additionalhelp"
            );
         }
         if (null_ok)
         {
            null_ok = ! unit_test_options_parse
            (
               &x_options_x, argc, argv,
               "03.05", "version", "additionalhelp"
            );
         }

         /*
          * Restore the original setting of the silence flag.
          */

         if (original_silence_setting)
            xpccut_silence_printing();
         else
            xpccut_allow_printing();

         unit_test_status_pass(&status, null_ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "is-verbose"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *    6. Accessors for the --verbose option.
 *
 * \test
 *    -  unit_test_options_is_verbose_set()
 *    -  unit_test_options_is_verbose()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_06 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 6,
      "unit_test_options_t", "unit_test_options_is_verbose...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_is_verbose_set(nullptr, false);
         if (null_ok)
            null_ok = ! unit_test_options_is_verbose_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_is_verbose(nullptr) == XPCCUT_IS_VERBOSE
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_verbose_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Is_Verbose;

         if (ok)
            ok = ! unit_test_options_is_verbose(&x_options_x);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_verbose_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Is_Verbose;

         if (ok)
            ok = unit_test_options_is_verbose(&x_options_x);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "show-values"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *    7. Accessors for the --show-values option.
 *
 * \test
 *    -  unit_test_options_show_values_set()
 *    -  unit_test_options_show_values()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_07 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 7,
      "unit_test_options_t", "unit_test_options_is_show_values...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_show_values_set(nullptr, false);
         if (null_ok)
            null_ok = ! unit_test_options_show_values_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_show_values(nullptr) == XPCCUT_SHOW_VALUES
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_show_values_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Show_Values;

         if (ok)
            ok = ! unit_test_options_show_values(&x_options_x);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_show_values_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Show_Values;

         if (ok)
            ok = unit_test_options_show_values(&x_options_x);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "text-synch"
 *    functionality.
 *
 * \note
 *    Although the unit_test_options_t.m_Text_Synch variable is supported,
 *    the actual usage of a synchronization object has not yet been added.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *    8. Accessors for the --text-synch option.
 *
 * \test
 *    -  unit_test_options_use_text_synch_set()
 *    -  unit_test_options_use_text_synch()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_08 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 8,
      "unit_test_options_t", "unit_test_options_use_text_synch...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_use_text_synch_set(nullptr, false);
         if (null_ok)
            null_ok = ! unit_test_options_use_text_synch_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_use_text_synch(nullptr) == XPCCUT_TEXT_SYNCH
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_use_text_synch_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Text_Synch;

         if (ok)
            ok = ! unit_test_options_use_text_synch(&x_options_x);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_use_text_synch_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Text_Synch;

         if (ok)
            ok = unit_test_options_use_text_synch(&x_options_x);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "step-numbers"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *    9. Accessors for the --show-step-numbers option.
 *
 * \test
 *    -  unit_test_options_show_step_numbers_set()
 *    -  unit_test_options_show_step_numbers()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_09 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 9,
      "unit_test_options_t", "unit_test_options_show_step_numbers...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_show_step_numbers_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_show_step_numbers_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         /*
          * Let's do this a different way, just to show another way to do
          * it.
          *
          * cbool_t null_ok =
          * (
          *    unit_test_options_show_step_numbers(nullptr) ==
          *       XPCCUT_SHOW_STEP_NUMBERS
          * );
          * unit_test_status_pass(&status, null_ok);
          */

         (void) unit_test_status_bool_check
         (
            &status, unit_test_options_show_step_numbers(nullptr),
            XPCCUT_SHOW_STEP_NUMBERS
         );
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_show_step_numbers_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Show_Step_Numbers;

         if (ok)
            ok = ! unit_test_options_show_step_numbers(&x_options_x);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_show_step_numbers_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Show_Step_Numbers;

         if (ok)
            ok = unit_test_options_show_step_numbers(&x_options_x);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "show-progress"
 *    functionality.
 *
 * \note
 *    This function affects more than one field if the flag is set to false.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   10. Accessors for the --show-progress option.
 *
 * \test
 *    -  unit_test_options_show_progress_set()
 *    -  unit_test_options_show_progress()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_10 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 10,
      "unit_test_options_t", "unit_test_options_show_progress...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_show_progress_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_show_progress_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok;
         cbool_t match = XPCCUT_SHOW_PROGRESS;

         /*
          * This makes no sense.  Besides, --hide-errors affects this
          * function, and thus using it can make the unit-test fail.
          * Useless!
          *
          * if (xpccut_is_silent())                   // --silent in force?
          *    match = false;
          *
          * null_ok = unit_test_options_show_progress(nullptr) == match;
          */

         null_ok = unit_test_options_show_progress(nullptr) == match;
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_show_progress_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Show_Progress;

         if (ok)
            ok = ! unit_test_options_show_progress(&x_options_x);

         /*
          * These settings are side-effects.
          */

         if (ok)
            ok = ! x_options_x.m_Show_Step_Numbers;

         if (ok)
            ok = ! x_options_x.m_Show_Values;

         if (ok)
            ok = ! x_options_x.m_Is_Verbose;

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_show_progress_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Show_Progress;

         /*
          * These settings are side-effects, but the values are not changed
          * when setting the flag to true.
          */

         if (ok)
            ok = ! x_options_x.m_Show_Step_Numbers;

         if (ok)
            ok = ! x_options_x.m_Show_Values;

         if (ok)
            ok = ! x_options_x.m_Is_Verbose;

         if (ok)
         {
            /*
             * This may be unwise, but the show-progress function also
             * checks the --silent option internally.  At any rate, we have
             * to account for that.
             **
             * \change ca 04/23/2008
             *    No, this is no longer true.  We have to ignore this
             *    sub-sub-test.
             **
             * if (xpccut_is_silent())
             *    ok = ! unit_test_options_show_progress(&x_options_x);
             * else
             *    ok = unit_test_options_show_progress(&x_options_x);
             */
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "stop-on-error"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   11. Accessors for the --stop-on-error option.
 *
 * \test
 *    -  unit_test_options_stop_on_error_set()
 *    -  unit_test_options_stop_on_error()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_11 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 11,
      "unit_test_options_t", "unit_test_options_stop_on_error...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_stop_on_error_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_stop_on_error_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_stop_on_error(nullptr) == XPCCUT_STOP_ON_ERROR
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_stop_on_error_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Stop_On_Error;

         if (ok)
            ok = ! unit_test_options_stop_on_error(&x_options_x);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_stop_on_error_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Stop_On_Error;

         if (ok)
            ok = unit_test_options_stop_on_error(&x_options_x);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "batch-mode"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   12. Accessors for the --batch-mode option.
 *
 * \test
 *    -  unit_test_options_batch_mode_set()
 *    -  unit_test_options_batch_mode()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_12 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 12,
      "unit_test_options_t", "unit_test_options_batch_mode...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_batch_mode_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_batch_mode_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_batch_mode(nullptr) == XPCCUT_BATCH_MODE
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_batch_mode_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Batch_Mode;

         if (ok)
            ok = ! unit_test_options_batch_mode(&x_options_x);

         /*
          * These settings are side-effects.
          */

         if (ok)
            ok = ! x_options_x.m_Show_Step_Numbers;

         if (ok)
            ok = ! x_options_x.m_Show_Values;

         if (ok)
            ok = ! x_options_x.m_Is_Verbose;

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_batch_mode_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Batch_Mode;

         if (ok)
            ok = unit_test_options_batch_mode(&x_options_x);

         /*
          * The unit_test_options_batch_mode_set() function also sets these
          * variables, but only if the boolean parameter is true.
          */

         if (ok)
            ok = x_options_x.m_Interactive;

         if (ok)
            ok = x_options_x.m_Response_Before = 'c';

         if (ok)
            ok = x_options_x.m_Response_After = 'p';

         /*
          * These settings are side-effects, but the values are not changed
          * when setting the flag to true or false.
          */

         if (ok)
            ok = ! x_options_x.m_Show_Step_Numbers;

         if (ok)
            ok = ! x_options_x.m_Show_Values;

         if (ok)
            ok = ! x_options_x.m_Is_Verbose;

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "interactive"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   13. Accessors for the --interactive option.
 *
 * \test
 *    -  unit_test_options_is_interactive_set()
 *    -  unit_test_options_is_interactive()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_13 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 13,
      "unit_test_options_t", "unit_test_options_is_interactive...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_is_interactive_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_is_interactive_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_is_interactive(nullptr) == XPCCUT_INTERACTIVE
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_interactive_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Interactive;

         if (ok)
            ok = ! unit_test_options_is_interactive(&x_options_x);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_interactive_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Interactive;

         if (ok)
            ok = unit_test_options_is_interactive(&x_options_x);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "beep"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   14. Accessors for the --beeps option.
 *
 * \test
 *    -  unit_test_options_do_beep_set()
 *    -  unit_test_options_do_beep()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_14 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 14,
      "unit_test_options_t", "unit_test_options_do_beep...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_do_beep_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_do_beep_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_do_beep(nullptr) == XPCCUT_BEEP
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_do_beep_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Beep_Prompt;

         if (ok)
            ok = ! unit_test_options_do_beep(&x_options_x);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_do_beep_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Beep_Prompt;

         if (ok)
            ok = unit_test_options_do_beep(&x_options_x);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "summarize"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   15. Accessors for the --summarize option.
 *
 * \test
 *    -  unit_test_options_is_summary_set()
 *    -  unit_test_options_is_summary()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_15 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 15,
      "unit_test_options_t", "unit_test_options_is_summary...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_is_summary_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_is_summary_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_is_summary(nullptr) == XPCCUT_SUMMARIZE
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_summary_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Summarize;

         if (ok)
            ok = ! unit_test_options_is_summary(&x_options_x);

         /*
          * The following settings are side-effects.
          */

         if (ok)
            ok = ! x_options_x.m_Interactive;

         if (ok)
            ok = ! x_options_x.m_Case_Pause;

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_summary_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Summarize;

         if (ok)
            ok = unit_test_options_is_summary(&x_options_x);

         /*
          * The following settings are side-effects that do not get undone
          * by setting the flag to true.
          */

         if (ok)
            ok = ! x_options_x.m_Interactive;

         if (ok)
            ok = ! x_options_x.m_Case_Pause;

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "pause"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   16. Accessors for the --case-pause option.
 *
 * \test
 *    -  unit_test_options_is_pause_set()
 *    -  unit_test_options_is_pause()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_16 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 16,
      "unit_test_options_t", "unit_test_options_is_pause...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_is_pause_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_is_pause_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_is_pause(nullptr) == XPCCUT_CASE_PAUSE
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_pause_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Case_Pause;

         if (ok)
            ok = ! unit_test_options_is_pause(&x_options_x);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_pause_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Case_Pause;

         if (ok)
            ok = unit_test_options_is_pause(&x_options_x);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "test group"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   17. Accessors for the --group option.
 *
 * \test
 *    -  unit_test_options_test_group_set()
 *    -  unit_test_options_test_group()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_17 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 17,
      "unit_test_options_t", "unit_test_options_test_group...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_test_group_set(nullptr, 0);
         if (null_ok)
            null_ok = ! unit_test_options_test_group_set(nullptr, 1);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         /*
          * Let's do this another way, just to show it.
          *
          * cbool_t null_ok =
          * (
          *    unit_test_options_test_group(nullptr) == XPCCUT_NO_SINGLE_GROUP
          * );
          * unit_test_status_pass(&status, null_ok);
          */

         (void) unit_test_status_int_check
         (
            &status, unit_test_options_test_group(nullptr),
            XPCCUT_NO_SINGLE_GROUP
         );
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting negative, set/get"))
      {
         /*
          * Set explicitly to make sure the correct value replaces this one.
          */

         x_options_x.m_Single_Test_Group = 5;
         ok = ! unit_test_options_test_group_set(&x_options_x, -1);

         /*
          * Let's do this another way, just to see how it looks.
          *
          * if (ok)
          *    ok = x_options_x.m_Single_Test_Group == 0;
          *
          * if (ok)
          *    ok = unit_test_options_test_group(&x_options_x) == 0;
          */

         if (ok)
         {
            ok = unit_test_status_int_check
            (
               &status, x_options_x.m_Single_Test_Group, 0
            );
         }
         if (ok)
         {
            ok = unit_test_status_int_check
            (
               &status, unit_test_options_test_group(&x_options_x), 0
            );
         }

         /*
          * Not needed, since we're using unit_test_status_int_check(),
          * which calls this function under the covers:
          *
          * unit_test_status_pass(&status, ok);
          */
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting to 0, set/get"))
      {
         x_options_x.m_Single_Test_Group = 5;
         if (ok)
            ok = unit_test_options_test_group_set(&x_options_x, 0);

         if (ok)
            ok = x_options_x.m_Single_Test_Group == 0;

         if (ok)
            ok = unit_test_options_test_group(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Setting to 1, set/get"))
      {
         x_options_x.m_Single_Test_Group = 5;
         if (ok)
            ok = unit_test_options_test_group_set(&x_options_x, 1);

         if (ok)
            ok = x_options_x.m_Single_Test_Group == 1;

         if (ok)
            ok = unit_test_options_test_group(&x_options_x) == 1;

         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Setting to 101, set/get"))
      {
         x_options_x.m_Single_Test_Group = 5;
         if (ok)
            ok = ! unit_test_options_test_group_set(&x_options_x, 101);

         if (ok)
            ok = x_options_x.m_Single_Test_Group == 0;

         if (ok)
            ok = unit_test_options_test_group(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "test case"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   18. Accessors for the --case option.
 *
 * \test
 *    -  unit_test_options_test_case_set()
 *    -  unit_test_options_test_case()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_18 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 18,
      "unit_test_options_t", "unit_test_options_test_case...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_test_case_set(nullptr, 0);
         if (null_ok)
            null_ok = ! unit_test_options_test_case_set(nullptr, 1);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_test_case(nullptr) == XPCCUT_NO_SINGLE_CASE
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting negative, set/get"))
      {
         /*
          * Set explicitly to make sure the correct value replaces this one.
          */

         x_options_x.m_Single_Test_Case = 5;
         ok = ! unit_test_options_test_case_set(&x_options_x, -1);
         if (ok)
            ok = x_options_x.m_Single_Test_Case == 0;

         if (ok)
            ok = unit_test_options_test_case(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting to 0, set/get"))
      {
         x_options_x.m_Single_Test_Case = 5;
         if (ok)
            ok = unit_test_options_test_case_set(&x_options_x, 0);

         if (ok)
            ok = x_options_x.m_Single_Test_Case == 0;

         if (ok)
            ok = unit_test_options_test_case(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Setting to 1, set/get"))
      {
         x_options_x.m_Single_Test_Case = 5;
         if (ok)
            ok = unit_test_options_test_case_set(&x_options_x, 1);

         /*
          * This is the first way to do this check:
          */

         if (ok)
            ok = x_options_x.m_Single_Test_Case == 1;

         /*
          * This is another way to do this check:
          */

         if (ok)
         {
            ok = unit_test_status_int_check
            (
               &status, x_options_x.m_Single_Test_Case, 1
            );
         }

         /*
          * This is yet another way to do this check.
          */

         if (ok)
            ok = unit_test_options_test_case(&x_options_x) == 1;

         /*
          * This is yet another way to do this check.  Four different ways
          * to do the same check!
          */

         if (ok)
         {
            ok = unit_test_status_int_check
            (
               &status, unit_test_options_test_case(&x_options_x), 1
            );
         }

         /*
          * If all the checks above used unit_test_status_int_check(), this
          * line of code would be redundant.
          */

         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Setting to 101, set/get"))
      {
         x_options_x.m_Single_Test_Case = 5;
         if (ok)
            ok = ! unit_test_options_test_case_set(&x_options_x, 101);

         if (ok)
            ok = x_options_x.m_Single_Test_Case == 0;

         if (ok)
            ok = unit_test_options_test_case(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "subtest"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   19. Accessors for the --subtest or --sub-test option.
 *
 * \test
 *    -  unit_test_options_single_subtest_set()
 *    -  unit_test_options_single_subtest()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_19 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 19,
      "unit_test_options_t", "unit_test_options_single_subtest...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_single_subtest_set(nullptr, 0);
         if (null_ok)
            null_ok = ! unit_test_options_single_subtest_set(nullptr, 1);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_single_subtest(nullptr) ==
               XPCCUT_NO_SINGLE_SUB_TEST
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting negative, set/get"))
      {
         /*
          * Set explicitly to make sure the correct value replaces this one.
          */

         x_options_x.m_Single_Sub_Test = 5;
         ok = ! unit_test_options_single_subtest_set(&x_options_x, -1);
         if (ok)
            ok = x_options_x.m_Single_Sub_Test == 0;

         if (ok)
            ok = unit_test_options_single_subtest(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting to 0, set/get"))
      {
         x_options_x.m_Single_Sub_Test = 5;
         if (ok)
            ok = unit_test_options_single_subtest_set(&x_options_x, 0);

         if (ok)
            ok = x_options_x.m_Single_Sub_Test == 0;

         if (ok)
            ok = unit_test_options_single_subtest(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Setting to 1, set/get"))
      {
         x_options_x.m_Single_Sub_Test = 5;
         if (ok)
            ok = unit_test_options_single_subtest_set(&x_options_x, 1);

         if (ok)
            ok = x_options_x.m_Single_Sub_Test == 1;

         if (ok)
            ok = unit_test_options_single_subtest(&x_options_x) == 1;

         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Setting to 1001, set/get"))
      {
         x_options_x.m_Single_Sub_Test = 5;
         if (ok)
            ok = ! unit_test_options_single_subtest_set(&x_options_x, 1001);

         if (ok)
            ok = x_options_x.m_Single_Sub_Test == 0;

         if (ok)
            ok = unit_test_options_single_subtest(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "test sleep-time"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   20. Accessors for the --sleep-time option.
 *
 * \test
 *    -  unit_test_options_test_sleep_time_set()
 *    -  unit_test_options_test_sleep_time()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_20 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 20,
      "unit_test_options_t", "unit_test_options_test_sleep_time...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_test_sleep_time_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_test_sleep_time_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_test_sleep_time(nullptr) == XPCCUT_TEST_SLEEP_TIME
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting negative, set/get"))
      {
         /*
          * Set explicitly to make sure the correct value replaces this one.
          */

         x_options_x.m_Test_Sleep_Time = 5;
         ok = ! unit_test_options_test_sleep_time_set(&x_options_x, -1);
         if (ok)
            ok = x_options_x.m_Test_Sleep_Time == 0;

         if (ok)
            ok = unit_test_options_test_sleep_time(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting to 0, set/get"))
      {
         x_options_x.m_Test_Sleep_Time = 5;
         if (ok)
            ok = unit_test_options_test_sleep_time_set(&x_options_x, 0);

         if (ok)
            ok = x_options_x.m_Test_Sleep_Time == 0;

         if (ok)
            ok = unit_test_options_test_sleep_time(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Setting to 1, set/get"))
      {
         x_options_x.m_Test_Sleep_Time = 5;
         if (ok)
            ok = unit_test_options_test_sleep_time_set(&x_options_x, 1);

         if (ok)
            ok = x_options_x.m_Test_Sleep_Time == 1;

         if (ok)
            ok = unit_test_options_test_sleep_time(&x_options_x) == 1;

         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Setting to 3600001, set/get"))
      {
         x_options_x.m_Test_Sleep_Time = 5;
         if (ok)
            ok = ! unit_test_options_test_sleep_time_set(&x_options_x, 3600001);

         if (ok)
            ok = x_options_x.m_Test_Sleep_Time == 0;

         if (ok)
            ok = unit_test_options_test_sleep_time(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "named test group"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   21. Accessors for the --group option using the name of the test instead
 *       of the number of the test.
 *
 * \test
 *    -  unit_test_options_named_group_set()
 *    -  unit_test_options_named_group()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_21 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 21,
      "unit_test_options_t", "unit_test_options_named_group...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_named_group_set(nullptr, nullptr);
         if (null_ok)
            null_ok = ! unit_test_options_named_group_set(nullptr, "!");

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok = cut_is_nullptr(unit_test_options_named_group(nullptr));
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting to null, set/get"))
      {
         /*
          * Set explicitly to make sure the value is not modified.  We don't
          * bother making sure a 3-character string can be copied here.
          */

         strcpy(x_options_x.m_Single_Test_Group_Name, "!!!");
         ok = ! unit_test_options_named_group_set(&x_options_x, nullptr);
         if (ok)
            ok = strcmp(x_options_x.m_Single_Test_Group_Name, "!!!") == 0;

         if (ok)
            ok = strcmp(unit_test_options_named_group(&x_options_x), "!!!") == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting to empty, set/get"))
      {
         strcpy(x_options_x.m_Single_Test_Group_Name, "!!!");
         if (ok)
            ok = unit_test_options_named_group_set(&x_options_x, "");

         if (ok)
            ok = strlen(x_options_x.m_Single_Test_Group_Name) == 0;

         if (ok)
            ok = cut_is_nullptr(unit_test_options_named_group(&x_options_x));

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Valid setting, set/get"))
      {
         strcpy(x_options_x.m_Single_Test_Group_Name, "!!!!!");
         if (ok)
            ok = unit_test_options_named_group_set(&x_options_x, "0xDEADBEEF");

         if (ok)
            ok = strcmp(x_options_x.m_Single_Test_Group_Name, "0xDEADBEEF") == 0;

         if (ok)
         {
            ok = 0 == strcmp
            (
               unit_test_options_named_group(&x_options_x), "0xDEADBEEF"
            );
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "named test case"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   22. Accessors for the --case option using the named of the test case
 *   instead of the test-case number.
 *
 * \test
 *    -  unit_test_options_named_case_set()
 *    -  unit_test_options_named_case()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_22 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 22,
      "unit_test_options_t", "unit_test_options_named_case...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_named_case_set(nullptr, nullptr);
         if (null_ok)
            null_ok = ! unit_test_options_named_case_set(nullptr, "!");

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok = cut_is_nullptr(unit_test_options_named_case(nullptr));
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting to null, set/get"))
      {
         /*
          * Set explicitly to make sure the value is not modified.  We don't
          * bother making sure a 3-character string can be copied here.
          */

         strcpy(x_options_x.m_Single_Test_Case_Name, "!!!");
         ok = ! unit_test_options_named_case_set(&x_options_x, nullptr);
         if (ok)
            ok = strcmp(x_options_x.m_Single_Test_Case_Name, "!!!") == 0;

         if (ok)
            ok = strcmp(unit_test_options_named_case(&x_options_x), "!!!") == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting to empty, set/get"))
      {
         strcpy(x_options_x.m_Single_Test_Case_Name, "!!!");
         if (ok)
            ok = unit_test_options_named_case_set(&x_options_x, "");

         if (ok)
            ok = strlen(x_options_x.m_Single_Test_Case_Name) == 0;

         if (ok)
            ok = cut_is_nullptr(unit_test_options_named_case(&x_options_x));

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Valid setting, set/get"))
      {
         strcpy(x_options_x.m_Single_Test_Case_Name, "!!!!!");
         if (ok)
            ok = unit_test_options_named_case_set(&x_options_x, "0xDEADBEEF");

         if (ok)
            ok = strcmp(x_options_x.m_Single_Test_Case_Name, "0xDEADBEEF") == 0;

         if (ok)
         {
            ok = 0 == strcmp
            (
               unit_test_options_named_case(&x_options_x), "0xDEADBEEF"
            );
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "named test sub-test"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   23. Accessors for the --subtest option using the name of the sub-test
 *       instead of the sub-test number.
 *
 * \test
 *    -  unit_test_options_named_subtest_set()
 *    -  unit_test_options_named_subtest()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_23 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 23,
      "unit_test_options_t", "unit_test_options_named_subtest...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_named_subtest_set
         (
            nullptr, nullptr
         );
         if (null_ok)
            null_ok = ! unit_test_options_named_subtest_set(nullptr, "!");

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
            cut_is_nullptr(unit_test_options_named_subtest(nullptr));

         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting to null, set/get"))
      {
         /*
          * Set explicitly to make sure the value is not modified.  We don't
          * bother making sure a 3-character string can be copied here.
          */

         strcpy(x_options_x.m_Single_Sub_Test_Name, "!!!");
         ok = ! unit_test_options_named_subtest_set(&x_options_x, nullptr);
         if (ok)
            ok = strcmp(x_options_x.m_Single_Sub_Test_Name, "!!!") == 0;

         if (ok)
            ok = strcmp(unit_test_options_named_subtest(&x_options_x), "!!!")
                     == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting to empty, set/get"))
      {
         strcpy(x_options_x.m_Single_Sub_Test_Name, "!!!");
         if (ok)
            ok = unit_test_options_named_subtest_set(&x_options_x, "");

         if (ok)
            ok = strlen(x_options_x.m_Single_Sub_Test_Name) == 0;

         if (ok)
            ok = cut_is_nullptr(unit_test_options_named_subtest(&x_options_x));

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Valid setting, set/get"))
      {
         strcpy(x_options_x.m_Single_Sub_Test_Name, "!!!!!");
         if (ok)
            ok = unit_test_options_named_subtest_set(&x_options_x, "0xDEADBEEF");

         if (ok)
            ok = strcmp(x_options_x.m_Single_Sub_Test_Name, "0xDEADBEEF") == 0;

         if (ok)
         {
            ok = 0 == strcmp
            (
               unit_test_options_named_subtest(&x_options_x), "0xDEADBEEF"
            );
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "require-sub-tests"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   24. Accessors for the --require-subtests option.
 *
 * \test
 *    -  unit_test_options_need_subtests_set()
 *    -  unit_test_options_need_subtests()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_24 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 24,
      "unit_test_options_t", "unit_test_options_need_subtests...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_need_subtests_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_need_subtests_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_need_subtests(nullptr) == XPCCUT_NEED_SUBTESTS
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_need_subtests_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Need_Subtests;

         if (ok)
            ok = ! unit_test_options_need_subtests(&x_options_x);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_need_subtests_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Need_Subtests;

         if (ok)
            ok = unit_test_options_need_subtests(&x_options_x);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "force-failure"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   25. Accessors for the --force-failure option.
 *
 * \test
 *    -  unit_test_options_force_failure_set()
 *    -  unit_test_options_force_failure()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_25 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 25,
      "unit_test_options_t", "unit_test_options_force_failure...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_force_failure_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_force_failure_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_force_failure(nullptr) == XPCCUT_FORCE_FAILURE
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_force_failure_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Force_Failure;

         if (ok)
            ok = ! unit_test_options_force_failure(&x_options_x);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_force_failure_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Force_Failure;

         if (ok)
            ok = unit_test_options_force_failure(&x_options_x);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the
 *    "get-current-test-number" functionality.
 *
 *    There's probably more that can be done with this test.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   26. unit_test_options_current_test()
 *
 * \test
 *    -  unit_test_options_current_test()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_26 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 26,
      "unit_test_options_t", "unit_test_options_current_test()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_options_current_test(nullptr) ==
            XPCCUT_NO_CURRENT_TEST;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "get"))
      {
         ok =  unit_test_options_current_test(&x_options_x) ==
            XPCCUT_NO_CURRENT_TEST;

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the
 *    "prompt-before/response-before" functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   27. Accessors for the --response-before option.
 *
 * \test
 *    -  unit_test_options_prompt_before_set()
 *    -  unit_test_options_prompt_before()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_27 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 27,
      "unit_test_options_t", "unit_test_options_prompt_before()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t not_ok = ! unit_test_options_prompt_before_set(nullptr, 0);
         if (not_ok)
            not_ok = unit_test_options_prompt_before(nullptr) == 0;

         unit_test_status_pass(&status, not_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Zero value"))
      {
         ok = unit_test_options_prompt_before_set(&x_options_x, 0);
         if (ok)
            ok = unit_test_options_prompt_before(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "'c' value"))
      {
         /*
          * Note how the upper-case value is mapped to lower-case when
          * stored, for easier usage in the unit-test library.
          */

         ok = unit_test_options_prompt_before_set(&x_options_x, 'c');
         if (ok)
            ok = unit_test_options_prompt_before(&x_options_x) == 'c';

         if (ok)
            ok = unit_test_options_prompt_before_set(&x_options_x, 'C');

         if (ok)
            ok = unit_test_options_prompt_before(&x_options_x) == 'c';

         if (ok)
            ok = x_options_x.m_Response_Before = 'c';

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "'s' value"))
      {
         ok = unit_test_options_prompt_before_set(&x_options_x, 's');
         if (ok)
            ok = unit_test_options_prompt_before(&x_options_x) == 's';

         if (ok)
            ok = unit_test_options_prompt_before_set(&x_options_x, 'S');

         if (ok)
            ok = unit_test_options_prompt_before(&x_options_x) == 's';

         if (ok)
            ok = x_options_x.m_Response_Before = 's';

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "'a' value"))
      {
         ok = unit_test_options_prompt_before_set(&x_options_x, 'a');
         if (ok)
            ok = unit_test_options_prompt_before(&x_options_x) == 'a';

         if (ok)
            ok = unit_test_options_prompt_before_set(&x_options_x, 'A');

         if (ok)
            ok = unit_test_options_prompt_before(&x_options_x) == 'a';

         if (ok)
            ok = x_options_x.m_Response_Before = 'a';

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "'q' value"))
      {
         ok = unit_test_options_prompt_before_set(&x_options_x, 'q');
         if (ok)
            ok = unit_test_options_prompt_before(&x_options_x) == 'q';

         if (ok)
            ok = unit_test_options_prompt_before_set(&x_options_x, 'Q');

         if (ok)
            ok = unit_test_options_prompt_before(&x_options_x) == 'q';

         if (ok)
            ok = x_options_x.m_Response_Before = 'q';

         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Unsupported value"))
      {
         if (ok)
         {
            cbool_t not_ok = ! unit_test_options_prompt_before_set
            (
               &x_options_x, '1'    /* '1' is not a valid response character  */
            );

            /*
             * The value from the previous sub-test should still be in
             * force.
             */

            if (not_ok)
               not_ok = unit_test_options_prompt_before(&x_options_x) == 'q';

            if (not_ok)
               not_ok = x_options_x.m_Response_Before = 'q';

            unit_test_status_pass(&status, not_ok);
         }
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the
 *    "prompt-after/response-after" functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   28. Accessors for the --response-after option.
 *
 * \test
 *    -  unit_test_options_prompt_after_set()
 *    -  unit_test_options_prompt_after()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_28 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 28,
      "unit_test_options_t", "unit_test_options_prompt_after()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t not_ok = ! unit_test_options_prompt_after_set(nullptr, 0);
         if (not_ok)
            not_ok = unit_test_options_prompt_after(nullptr) == 0;

         unit_test_status_pass(&status, not_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Zero value"))
      {
         ok = unit_test_options_prompt_after_set(&x_options_x, 0);
         if (ok)
            ok = unit_test_options_prompt_after(&x_options_x) == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "'p' value"))
      {
         /*
          * Note how the upper-case value is mapped to lower-case when
          * stored, for easier usage in the unit-test library.
          */

         ok = unit_test_options_prompt_after_set(&x_options_x, 'p');
         if (ok)
            ok = unit_test_options_prompt_after(&x_options_x) == 'p';

         if (ok)
            ok = unit_test_options_prompt_after_set(&x_options_x, 'P');

         if (ok)
            ok = unit_test_options_prompt_after(&x_options_x) == 'p';

         if (ok)
            ok = x_options_x.m_Response_After = 'p';

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "'f' value"))
      {
         ok = unit_test_options_prompt_after_set(&x_options_x, 'f');
         if (ok)
            ok = unit_test_options_prompt_after(&x_options_x) == 'f';

         if (ok)
            ok = unit_test_options_prompt_after_set(&x_options_x, 'F');

         if (ok)
            ok = unit_test_options_prompt_after(&x_options_x) == 'f';

         if (ok)
            ok = x_options_x.m_Response_After = 'f';

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "'a' value"))
      {
         ok = unit_test_options_prompt_after_set(&x_options_x, 'a');
         if (ok)
            ok = unit_test_options_prompt_after(&x_options_x) == 'a';

         if (ok)
            ok = unit_test_options_prompt_after_set(&x_options_x, 'A');

         if (ok)
            ok = unit_test_options_prompt_after(&x_options_x) == 'a';

         if (ok)
            ok = x_options_x.m_Response_After = 'a';

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "'q' value"))
      {
         ok = unit_test_options_prompt_after_set(&x_options_x, 'q');
         if (ok)
            ok = unit_test_options_prompt_after(&x_options_x) == 'q';

         if (ok)
            ok = unit_test_options_prompt_after_set(&x_options_x, 'Q');

         if (ok)
            ok = unit_test_options_prompt_after(&x_options_x) == 'q';

         if (ok)
            ok = x_options_x.m_Response_After = 'q';

         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Unsupported value"))
      {
         if (ok)
         {
            cbool_t not_ok = ! unit_test_options_prompt_after_set
            (
               &x_options_x, '1'    /* '1' is not a valid response character  */
            );

            /*
             * The value from the previous sub-test should still be in
             * force.
             */

            if (not_ok)
               not_ok = unit_test_options_prompt_after(&x_options_x) == 'q';

            if (not_ok)
               not_ok = x_options_x.m_Response_After = 'q';

            unit_test_status_pass(&status, not_ok);
         }
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "is-simulated"
 *    functionality.
 *
 * \group
 *    3. unit_test_options_t functions.
 *
 * \case
 *   29. Accessors for the --simulated option.
 *
 * \test
 *    -  unit_test_options_is_simulated_set()
 *    -  unit_test_options_is_simulated()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_03_29 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 29,
      "unit_test_options_t", "unit_test_options_is_simulated...()"
   );
   if (ok)
   {
      unit_test_options_t x_options_x;
      ok = unit_test_options_init(&x_options_x);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_options_x, true);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this', set"))
      {
         cbool_t null_ok = ! unit_test_options_is_simulated_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_is_simulated_set(nullptr, true);

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this', get"))
      {
         cbool_t null_ok =
         (
            unit_test_options_is_simulated(nullptr) == XPCCUT_IS_SIMULATED
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_simulated_set(&x_options_x, false);

         if (ok)
            ok = ! x_options_x.m_Is_Simulated;

         if (ok)
            ok = ! unit_test_options_is_simulated(&x_options_x);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_simulated_set(&x_options_x, true);

         if (ok)
            ok = x_options_x.m_Is_Simulated;

         if (ok)
            ok = unit_test_options_is_simulated(&x_options_x);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the basic "default"
 *    initialization function properly zeros or clears all fields to their
 *    "default" values.
 *
 *    The function tested is like a "C" version of a "C++" default
 *    constructor.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *    1. Initialization and setup functions.
 *
 * \test
 *    -  unit_test_init()
 *    -  unit_test_clear() [indirect test of static function]
 *    -  unit_test_options_init() [indirect test of static function]
 *    -  unit_test_allocate_help() [indirect test of static function]
 *    -  unit_test_init_cases() [indirect test of static function]
 *
 * \todo
 *    It would be nice if we could verify the duration of the previous test
 *    to be correct to within plus-or-minus 10 ms.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 1,
      "unit_test_t", "unit_test_init()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      (void) unit_test_init(&x_test_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_init(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Good 'this'"))
      {
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "m_App_Options"))
      {
         ok = default_options_check
         (
            &x_test_x.m_App_Options,      /* check unit_test_options_init()   */
            false                         /* no show-progress testing         */
         );
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Good help setup"))
      {
         /*
          * This sub-test will check the call to the static function
          * unit_test_allocate_help().
          */

         ok = cut_not_nullptr(x_test_x.m_Additional_Help);
         if (ok)
         {
            ok = strlen(x_test_x.m_Additional_Help) > 0;
            if (ok && unit_test_options_is_verbose(options))
            {
               fprintf
               (
                  stdout, "  %s:\n{\n%s\n}\n",
                  _("The allocated help text is"),
                  x_test_x.m_Additional_Help
               );
            }
            else
               xpccut_errprint(_("unit-test help text is empty"));
         }
         else
            xpccut_errprint(_("unit-test help text not allocated"));

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Good cases setup"))
      {
         /*
          * This sub-test will check the call to the static function
          * unit_test_init_cases() and unit_test_allocate_cases.
          */

         if (ok)
         {
            ok = cut_not_nullptr(x_test_x.m_Test_Cases);
            if (ok)
               ok = x_test_x.m_Total_Errors == 0;

            if (ok)
               ok = x_test_x.m_Current_Test_Number == XPCCUT_NO_CURRENT_TEST;

            if (ok)
               ok = x_test_x.m_Test_Count == 0;

            if (ok)
               ok = x_test_x.m_Allocation_Count == XPCCUT_CASE_ALLOCATION;

            if (ok && unit_test_options_is_verbose(options))
            {
               fprintf
               (
                  stdout, "  %s:\n{\n%s\n}\n",
                  _("The allocated help text is"),
                  x_test_x.m_Additional_Help
               );
            }
            else
               xpccut_errprint(_("unit-test help text is empty"));
         }
         else
            xpccut_errprint(_("unit-test help text not allocated"));

         unit_test_status_pass(&status, ok);
      }

      /*
       * Now destroy all allocated resources in the test object.  Even
       * little ol' unit_test_init() allocates them.  If the --leak-check
       * option is in force, we don't call this "destructor".
       */

      if (! g_do_leak_check)
         unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "principal"
 *    initialization function properly zeros or clears all fields to their
 *    "default" values.
 *
 *    The function tested is like a "C" version of a "C++" principal
 *    constructor.
 *
 *    This is a "part 1" unit-test case for unit_test_initialize().
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *    2. unit_test_initialize(), part 1.
 *
 * \test
 *    -  unit_test_initialize()
 *
 * \todo
 *    It would be nice if we could verify the duration of the previous test
 *    to be correct to within plus-or-minus 10 ms.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_02 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 2,
      "unit_test_t", "unit_test_initialize() part 1"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 1;                                /* guarantee initialized   */
      argv[0] = "<unknown>";                       /* guarantee some value    */

      /*
       * Start out with default values so that we can take the opposite tack
       * with the full initialization function.  We have to access the
       * member field directly.
       */

      ok = unit_test_options_init(&x_test_x.m_App_Options);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_test_x.m_App_Options, true);

      if (ok)
      {
         /*
          * Now modify the test structure with the opposite values (or at
          * least different values) for every accessible field, so that the
          * resulting test is a bizarro-clone of test 03_01.
          */

         argv[0]  = "unit_test_test";
         argv[1]  = "--no-verbose";
         argv[2]  = "--no-show-progress";
         argv[3]  = "--no-show-values";
         argv[4]  = "--no-show-step-numbers";
         argv[5]  = "--no-text-synch";
         argv[6]  = "--no-stop-on-error";
         argv[7]  = "--no-batch-mode";
         argv[8]  = "--no-interactive";
         argv[9]  = "--no-beeps";
         argv[10] = "--no-case-pause";
         argv[11] = "--no-summarize";
         argv[12] = "--group";
         argv[13] = "1";
         argv[14] = "--case";
         argv[15] = "2";
         argv[16] = "--sub-test";
         argv[17] = "3";
         argv[18] = "--sleep-time";
         argv[19] = "4";
         argv[20] = "--no-verbose";
         argv[21] = "--no-verbose";
         argc = 22;
         ok = unit_test_initialize
         (
            &x_test_x, argc, argv,
            "Test 04.02", "version", "additionalhelp"
         );
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_initialize
         (
            nullptr, argc, argv, "x", "y", "z"
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Good 'this'"))
      {
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "internal m_Is_Verbose"))
      {
         ok = ! x_test_x.m_App_Options.m_Is_Verbose;
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "internal m_Show_Values"))
      {
         ok = ! x_test_x.m_App_Options.m_Show_Values;
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "internal m_Text_Synch"))
      {
         ok = ! x_test_x.m_App_Options.m_Text_Synch;
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "internal m_Show_Step_Numbers"))
      {
         ok = ! x_test_x.m_App_Options.m_Show_Step_Numbers;
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "internal m_Show_Progress"))
      {
         ok = ! x_test_x.m_App_Options.m_Show_Progress;
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "internal m_Stop_On_Error"))
      {
         ok = ! x_test_x.m_App_Options.m_Stop_On_Error;
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "internal m_Batch_Mode"))
      {
         ok = ! x_test_x.m_App_Options.m_Batch_Mode;
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "internal m_Interactive"))
      {
         ok = ! x_test_x.m_App_Options.m_Interactive;
         unit_test_status_pass(&status, ok);
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "internal m_Beep_Prompt"))
      {
         ok = ! x_test_x.m_App_Options.m_Beep_Prompt;
         unit_test_status_pass(&status, ok);
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "internal m_Summarize"))
      {
         ok = ! x_test_x.m_App_Options.m_Summarize;
         unit_test_status_pass(&status, ok);
      }

      /* 13 */

      if (unit_test_status_next_subtest(&status, "internal m_Case_Pause"))
      {
         ok = ! x_test_x.m_App_Options.m_Case_Pause;
         unit_test_status_pass(&status, ok);
      }

      /* 14 */

      if (unit_test_status_next_subtest(&status, "internal m_Single_Test_Group"))
      {
         ok = x_test_x.m_App_Options.m_Single_Test_Group ==
            (XPCCUT_NO_SINGLE_GROUP+1);

         unit_test_status_pass(&status, ok);
      }

      /* 15 */

      if (unit_test_status_next_subtest(&status, "internal m_Single_Test_Case"))
      {
         ok = x_test_x.m_App_Options.m_Single_Test_Case ==
            (XPCCUT_NO_SINGLE_CASE+2);

         unit_test_status_pass(&status, ok);
      }

      /* 16 */

      if (unit_test_status_next_subtest(&status, "internal m_Single_Sub_Test"))
      {
         ok = x_test_x.m_App_Options.m_Single_Sub_Test ==
            (XPCCUT_NO_SINGLE_SUB_TEST+3);

         unit_test_status_pass(&status, ok);
      }

      /* 17 */

      if (unit_test_status_next_subtest(&status, "internal m_Test_Sleep_Time"))
      {
         ok = x_test_x.m_App_Options.m_Test_Sleep_Time ==
            (XPCCUT_TEST_SLEEP_TIME+4);

         unit_test_status_pass(&status, ok);
      }

      /* 18 */

      if (unit_test_status_next_subtest(&status, "m_Test_Application_Name"))
      {
         /* "Test 04.02", "version", "additionalhelp" */

         ok = strcmp(x_test_x.m_Test_Application_Name, "Test 04.02") == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 19 */

      if (unit_test_status_next_subtest(&status, "m_Test_Application_Version"))
      {
         ok = strcmp(x_test_x.m_Test_Application_Version, "version") == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 20 */

      if (unit_test_status_next_subtest(&status, "m_Additional_Help"))
      {
         ok = strcmp(x_test_x.m_Additional_Help, "additionalhelp") == 0;
         unit_test_status_pass(&status, ok);
      }

      /**
       * Testing of null and blank app-names, version strings, and help
       * strings is done in the next two unit-test cases.
       */

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the basic
 *    initialization function properly zeros or clears all fields to their
 *    "default" values.
 *
 *    The function tested is like a "C" version of a "C++" principal
 *    constructor.
 *
 *    This is a "part 2" unit-test case for unit_test_initialize().
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *    3. unit_test_initialize(), part 2.
 *
 * \test
 *    -  unit_test_initialize()
 *
 * \todo
 *    It would be nice if we could verify the duration of the previous test
 *    to be correct to within plus-or-minus 10 ms.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_03 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 3,
      "unit_test_t", "unit_test_initialize() part 2"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 1;                                /* guarantee initialized   */
      argv[0] = "<unknown>";                       /* guarantee some value    */

      /*
       * Start out with default values so that we can take the opposite tack
       * with the full initialization function.  We have to access the
       * member field directly.
       */

      ok = unit_test_options_init(&x_test_x.m_App_Options);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_test_x.m_App_Options, true);

      if (ok)
      {
         /*
          * Now modify the test structure with the opposite values (or at
          * least different values) for every accessible field, so that the
          * resulting test is a bizarro-clone of test 03_01.
          *
          * Also note that some options turn off the global output option or
          * alter other options.  We disable them by tacking on "---ignore".
          *
          * The --summarize option does this, and the output looks weird.
          */

         argv[0]  = "unit_test_test";
         argv[1]  = "--verbose";
         argv[2]  = "--show-progress";
         argv[3]  = "--show-values";
         argv[4]  = "--show-step-numbers";
         argv[5]  = "--text-synch";
         argv[6]  = "--stop-on-error";
         argv[7]  = "--batch-mode---ignore";
         argv[8]  = "--interactive---ignore";
         argv[9]  = "--beeps";
         argv[10] = "--case-pause";
         argv[11] = "--summarize---ignore";
         argv[12] = "--group";
         argv[13] = "1";
         argv[14] = "--case";
         argv[15] = "2";
         argv[16] = "--sub-test";
         argv[17] = "3";
         argv[18] = "--sleep-time";
         argv[19] = "4";
         argc = 20;

         /*
          * This function call will fail due to the empty help text, but we
          * can learn more by continuing nonetheless.
          *
          * ca 06/23/2008
          * Actually, it will not fail now, since empties produce good
          * defaults for the caller now.  Hence, the not (!) operator is
          * gone.
          */

         ok = unit_test_initialize(&x_test_x, argc, argv, "", "", "");
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_initialize
         (
            nullptr, argc, argv, "x", "y", "z"
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Good 'this'"))
      {
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "internal m_Is_Verbose"))
      {
         ok = x_test_x.m_App_Options.m_Is_Verbose;
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "internal m_Show_Values"))
      {
         ok = x_test_x.m_App_Options.m_Show_Values;
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "internal m_Text_Synch"))
      {
         ok = x_test_x.m_App_Options.m_Text_Synch;
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "internal m_Show_Step_Numbers"))
      {
         ok = x_test_x.m_App_Options.m_Show_Step_Numbers;
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "internal m_Show_Progress"))
      {
         ok = x_test_x.m_App_Options.m_Show_Progress;
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "internal m_Stop_On_Error"))
      {
         ok = x_test_x.m_App_Options.m_Stop_On_Error;
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "internal m_Batch_Mode"))
      {
         ok = ! x_test_x.m_App_Options.m_Batch_Mode;
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "internal m_Interactive"))
      {
         ok = ! x_test_x.m_App_Options.m_Interactive;
         unit_test_status_pass(&status, ok);
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "internal m_Beep_Prompt"))
      {
         ok = x_test_x.m_App_Options.m_Beep_Prompt;
         unit_test_status_pass(&status, ok);
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "internal m_Summarize"))
      {
         ok = ! x_test_x.m_App_Options.m_Summarize;
         unit_test_status_pass(&status, ok);
      }

      /* 13 */

      if (unit_test_status_next_subtest(&status, "internal m_Case_Pause"))
      {
         ok = x_test_x.m_App_Options.m_Case_Pause;
         unit_test_status_pass(&status, ok);
      }

      /* 14 */

      if (unit_test_status_next_subtest(&status, "internal m_Single_Test_Group"))
      {
         ok = x_test_x.m_App_Options.m_Single_Test_Group ==
            (XPCCUT_NO_SINGLE_GROUP+1);

         unit_test_status_pass(&status, ok);
      }

      /* 15 */

      if (unit_test_status_next_subtest(&status, "internal m_Single_Test_Case"))
      {
         ok = x_test_x.m_App_Options.m_Single_Test_Case ==
            (XPCCUT_NO_SINGLE_CASE+2);

         unit_test_status_pass(&status, ok);
      }

      /* 16 */

      if (unit_test_status_next_subtest(&status, "internal m_Single_Sub_Test"))
      {
         ok = x_test_x.m_App_Options.m_Single_Sub_Test ==
            (XPCCUT_NO_SINGLE_SUB_TEST+3);

         unit_test_status_pass(&status, ok);
      }

      /* 17 */

      if (unit_test_status_next_subtest(&status, "internal m_Test_Sleep_Time"))
      {
         ok = x_test_x.m_App_Options.m_Test_Sleep_Time ==
            (XPCCUT_TEST_SLEEP_TIME+4);

         unit_test_status_pass(&status, ok);
      }

      /* 18 */

      if (unit_test_status_next_subtest(&status, "m_Test_Application_Name"))
      {
         /* "", "", "" */

         ok = strcmp(x_test_x.m_Test_Application_Name, "") == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 19 */

      if (unit_test_status_next_subtest(&status, "m_Test_Application_Version"))
      {
         ok = strcmp(x_test_x.m_Test_Application_Version, "") == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 20 */

      if (unit_test_status_next_subtest(&status, "m_Additional_Help"))
      {
         /*
          * The text here is assigned internally from a static character
          * string in the static function unit_test_allocate_help().
          * However, we cannot then free this pointer in the call to
          * unit_test_destroy() below.  So all we can do here is check for
          * the null pointer.
          *
          * ok = strcmp
          * (
          *    x_test_x.m_Additional_Help, _("? unit_test_allocate_help() null")
          * ) == 0;
          *
          * ca 06/23/2008
          * Actually, it will not be null now, since empties produce good
          * defaults for the caller.
          * gone.
          *
          *    ok = cut_is_nullptr(x_test_x.m_Additional_Help);
          */

         ok = ! cut_is_nullptr(x_test_x.m_Additional_Help);
         unit_test_status_pass(&status, ok);
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the basic
 *    initialization function properly zeros or clears all fields to their
 *    "default" values.
 *
 *    The function tested is like a "C" version of a "C++" principal
 *    constructor.
 *
 *    This is a "part 3" unit-test case for unit_test_initialize().
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *    4. unit_test_initialize(), part 3.
 *
 * \test
 *    -  unit_test_initialize()
 *
 * \todo
 *    It would be nice if we could verify the duration of the previous test
 *    to be correct to within plus-or-minus 10 ms.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_04 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 4,
      "unit_test_t", "unit_test_initialize() part 3"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 1;                                /* guarantee initialized   */
      argv[0] = "<unknown>";                       /* guarantee some value    */

      /*
       * Start out with default values so that we can take the opposite tack
       * with the full initialization function.  We have to access the
       * member field directly.
       */

      ok = unit_test_options_init(&x_test_x.m_App_Options);
      if (ok)
         ok = unit_test_options_is_simulated_set(&x_test_x.m_App_Options, true);

      if (ok)
      {
         /*
          * Now modify the test structure with the opposite values (or at
          * least different values) for every accessible field, so that the
          * resulting test is a bizarro-clone of test 03_01.
          *
          * Also note that some options turn off the global output option or
          * alter other options.  We disable them by tacking on "---ignore".
          *
          * The --summarize option does this, and the output looks weird.
          */

         argv[0]  = "unit_test_test";
         argv[1]  = "--verbose";
         argc = 2;
         ok = unit_test_initialize
         (
            &x_test_x, argc, argv, nullptr, nullptr, nullptr
         );
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_initialize
         (
            nullptr, argc, argv, "x", "y", "z"
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Good 'this'"))
      {
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "internal m_Is_Verbose"))
      {
         ok = x_test_x.m_App_Options.m_Is_Verbose;
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "m_Test_Application_Name"))
      {
         /* nullptr, nullptr, nullptr */

         ok = strcmp(x_test_x.m_Test_Application_Name, _("Unit Test")) == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "m_Test_Application_Version"))
      {
         ok = strcmp(x_test_x.m_Test_Application_Version, "1.1") == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "m_Additional_Help"))
      {
         ok = strcmp(x_test_x.m_Additional_Help, _("No help")) == 0;
         unit_test_status_pass(&status, ok);
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "test-loading"
 *    functionality works.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *    5. Initialization and setup functions.
 *
 * \test
 *    -  unit_test_load()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_05 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 5, "unit_test_t", "unit_test_load()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_load(nullptr, unit_unit_test_01_01);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null test-pointer"))
      {
         cbool_t null_ok = ! unit_test_load(&x_test_x, nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Good 'this'"))
      {
         if (ok)
            ok = unit_test_load(&x_test_x, unit_unit_test_01_01);

         if (ok)
            ok = x_test_x.m_Test_Count == 1;

         if (ok)
            ok = x_test_x.m_Allocation_Count == XPCCUT_CASE_ALLOCATION;

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Second test"))
      {
         if (ok)
            ok = unit_test_load(&x_test_x, unit_unit_test_01_01);

         if (ok)
            ok = x_test_x.m_Test_Count == 2;

         if (ok)
            ok = x_test_x.m_Allocation_Count == XPCCUT_CASE_ALLOCATION;

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Until next allocation"))
      {
         if (ok)
         {
            int ti;
            for (ti = x_test_x.m_Test_Count; ti < XPCCUT_CASE_ALLOCATION; ti++)
            {
               if (ok)
                  ok = unit_test_load(&x_test_x, unit_unit_test_01_01);

               if (ok)
                  ok = x_test_x.m_Test_Count == (ti + 1);

               if (ok)
                  ok = x_test_x.m_Allocation_Count == XPCCUT_CASE_ALLOCATION;

               if (! ok)
               {
                  if (! xpccut_is_silent())
                  {
                     fprintf
                     (
                        stdout, "%s %d %s %d\n",
                        _("load failed at count"), x_test_x.m_Test_Count,
                        _("and allocation"), x_test_x.m_Allocation_Count
                     );
                  }
                  break;
               }
            }
            unit_test_status_pass(&status, ok);
         }
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "After next allocation"))
      {
         if (ok)
         {
            int ti;
            for (ti = x_test_x.m_Test_Count; ti < 2*XPCCUT_CASE_ALLOCATION; ti++)
            {
               if (ok)
                  ok = unit_test_load(&x_test_x, unit_unit_test_01_01);

               if (ok)
                  ok = x_test_x.m_Test_Count == (ti + 1);

               if (ok)
                  ok = x_test_x.m_Allocation_Count == 2*XPCCUT_CASE_ALLOCATION;

               if (! ok)
               {
                  if (! xpccut_is_silent())
                  {
                     fprintf
                     (
                        stdout, "%s %d %s %d\n",
                        _("load failed at count"), x_test_x.m_Test_Count,
                        _("and allocation"), x_test_x.m_Allocation_Count
                     );
                  }
                  break;
               }
            }
            unit_test_status_pass(&status, ok);
         }
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the
 *    "test-load-counting" functionality is working, and also can be used by
 *    C++ code.
 *
 *    This functionality is /not/ tested in test 04.05 above, because the
 *    unit_test_load() function does its own counting.  The
 *    unit_test_cpp_load_count() is mean for usage in a C++ unit-test
 *    wrapper.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *    6. unit_test_cpp_load_count()
 *
 * \test
 *    -  unit_test_cpp_load_count()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_06 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 6, "unit_test_t", "unit_test_cpp_load_count()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_cpp_load_count(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Zero verifaction"))
      {
         if (ok)
            ok = x_test_x.m_Test_Count == 0;
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "One verifaction"))
      {
         if (ok)
            ok = unit_test_cpp_load_count(&x_test_x);

         if (ok)
            ok = x_test_x.m_Test_Count == 1;
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "1000 verifaction"))
      {
         if (ok)
         {
            int ti;
            for (ti = x_test_x.m_Test_Count; ti < 1000; ti++)
            {
               if (ok)
                  ok = unit_test_cpp_load_count(&x_test_x);
               else
               {
                  if (! xpccut_is_silent())
                  {
                     fprintf
                     (
                        stdout, "%s %d\n",
                        _("load failed at count"), x_test_x.m_Test_Count
                     );
                  }
                  break;
               }
            }
            if (ok)
               ok = x_test_x.m_Test_Count == 1000;
         }
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "test-dispose"
 *    functionality.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *    7. unit_test_dispose()
 *
 * \test
 *    -  unit_test_dispose()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_07 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 7, "unit_test_t", "unit_test_dispose()"
   );
   if (ok)
   {
      unit_test_status_t x_status_x;
      ok = unit_test_status_init(&x_status_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_dispose(nullptr);
         if (ok)
            ok = x_status_x.m_Test_Disposition == XPCCUT_DISPOSITION_ABORTED;

         unit_test_status_pass(&status, ok && null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "init verifaction"))
      {
         if (ok)
            ok = x_status_x.m_Test_Disposition == XPCCUT_DISPOSITION_ABORTED;

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "CONTINUE"))
      {
         cbool_t quit;
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_CONTINUE;
         quit = unit_test_dispose(&x_status_x);
         ok = ! quit && unit_test_status_passed(&x_status_x);
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "DNT"))
      {
         cbool_t quit;
         x_status_x.m_Test_Result = true;
         x_status_x.m_Subtest_Error_Count = 0;
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_DNT;
         quit = unit_test_dispose(&x_status_x);
         ok = ! quit && unit_test_status_passed(&x_status_x);
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "FAILED"))
      {
         cbool_t quit;
         x_status_x.m_Test_Result = true;
         x_status_x.m_Subtest_Error_Count = 0;
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_FAILED;
         quit = unit_test_dispose(&x_status_x);
         show_deliberate_failure(options);
         ok = ! quit && ! unit_test_status_passed(&x_status_x);
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "QUITTED"))
      {
         cbool_t quit;
         x_status_x.m_Test_Result = true;
         x_status_x.m_Subtest_Error_Count = 0;
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_QUITTED;
         quit = unit_test_dispose(&x_status_x);
         ok = quit && unit_test_status_passed(&x_status_x);
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "ABORTED"))
      {
         cbool_t quit;
         x_status_x.m_Test_Result = true;
         x_status_x.m_Subtest_Error_Count = 0;
         x_status_x.m_Test_Disposition = XPCCUT_DISPOSITION_ABORTED;
         quit = unit_test_dispose(&x_status_x);
         show_deliberate_failure(options);
         ok = quit && ! unit_test_status_passed(&x_status_x);
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "test-load-count"
 *    function works.
 *
 *    This test uses the same method as 04_06, but gets the count
 *    through the unit_test_count() accessor instead of directly.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *    8. unit_test_cpp_load_count()
 *
 * \test
 *    -  unit_test_cpp_load_count()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_08 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 8, "unit_test_t", "unit_test_count()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_count(nullptr) == 0;
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Zero verifaction"))
      {
         if (ok)
            ok = unit_test_count(&x_test_x) == 0;
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "One verifaction"))
      {
         if (ok)
            ok = unit_test_cpp_load_count(&x_test_x);

         if (ok)
            ok = unit_test_count(&x_test_x) == 1;
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "1000 verifaction"))
      {
         if (ok)
         {
            int ti;
            for (ti = x_test_x.m_Test_Count; ti < 1000; ti++)
            {
               if (ok)
                  ok = unit_test_cpp_load_count(&x_test_x);
               else
               {
                  if (! xpccut_is_silent())
                  {
                     fprintf
                     (
                        stdout, "%s %d\n",
                        _("load failed at count"), unit_test_count(&x_test_x)
                     );
                  }
                  break;
               }
            }
            if (ok)
               ok = unit_test_count(&x_test_x) == 1000;
         }
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "test-number"
 *    functionality works.
 *
 *    This test is very rudimentary, and simply tests this function as a
 *    field accessor for unit_test_t, and little more.  The
 *    m_Current_Test_Number field is manipulated directly.
 *
 *    For more extensive testing, see the unit-test for the
 *    unit_test_next_test() function:  unit_unit_test_04_10().
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *    9. unit_test_number()
 *
 * \test
 *    -  unit_test_number()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_09 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 9, "unit_test_t", "unit_test_number()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_number(nullptr) ==
            XPCCUT_INVALID_PARAMETER;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_number(&x_test_x) == XPCCUT_NO_CURRENT_TEST;
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Zero verifaction"))
      {
         x_test_x.m_Current_Test_Number = 0;
         if (ok)
            ok = unit_test_number(&x_test_x) == 0;
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "One verifaction"))
      {
         x_test_x.m_Current_Test_Number = 1;
         if (ok)
            ok = unit_test_number(&x_test_x) == 1;
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "1000 verifaction"))
      {
         if (ok)
         {
            int ti;
            for (ti = x_test_x.m_Test_Count; ti < 1000; ti++)
            {
               if (ok)
                  ok = unit_test_number(&x_test_x);
               else
               {
                  if (! xpccut_is_silent())
                  {
                     fprintf
                     (
                        stdout, "%s %d\n",
                        _("load failed at count"), x_test_x.m_Test_Count
                     );
                  }
                  break;
               }
            }
            if (ok)
               ok = x_test_x.m_Test_Count == 1000;
         }
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "next test-number"
 *    functionality works.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   10. Next test-number functionality.
 *
 * \test
 *    -  unit_test_next_test()
 *    -  unit_test_number() [an additional test]
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_10 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 10, "unit_test_t", "unit_test_next_test()"
   );
   if (ok)
   {
      /*
       * Observe how we have to sneak in and turn off the show-progress
       * option of the x_test_x object, otherwise the default value of this
       * option will yield a lot of unhelpful output.
       */

      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);
      if (ok)
      {
         ok = unit_test_options_show_progress_set
         (
            &x_test_x.m_App_Options, false
         );
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_next_test(nullptr) ==
            XPCCUT_NO_CURRENT_TEST;

         /*
          * Now verify that the m_Current_Test_Number field is not modified
          * in this situation.  How could it be?!
          */

         if (null_ok)
            null_ok = x_test_x.m_Current_Test_Number == XPCCUT_NO_CURRENT_TEST;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.  Already done in another test, but
          * if it gets broken, it is convenient to see it here, too, and we
          * don't want to keep testing stuff that is already broken based on
          * a bad result here.
          */

         if (ok)
            ok = unit_test_number(&x_test_x) == XPCCUT_NO_CURRENT_TEST;
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "No tests loaded yet"))
      {
         /*
          * At this point, no unit-test functions have been loaded into the
          * x_test_x object, so it has an m_Test_Count field of 0.  So the
          * unit_test_next_test() function should always fail.  However,
          * internally the m_Current_Test_Number field is always
          * incremented, so we do some white-box testing of it.  (This may
          * cause us pain later!)
          */

         if (ok)
         {
            int test = unit_test_next_test(&x_test_x);
            ok = test == XPCCUT_NO_CURRENT_TEST;
            if (ok)
               ok = x_test_x.m_Current_Test_Number == 0;
         }
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Still no tests loaded"))
      {
         if (ok)
         {
            int test = unit_test_next_test(&x_test_x);
            ok = test == XPCCUT_NO_CURRENT_TEST;
            if (ok)
               ok = x_test_x.m_Current_Test_Number == 1;
         }
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Load 3 tests"))
      {
         if (ok)
         {
            ok = unit_test_init(&x_test_x);                 /* start again */
            if (ok)
            {
               ok = unit_test_options_show_progress_set
               (
                  &x_test_x.m_App_Options, false
               );
            }
            if (ok)
               ok = unit_test_load(&x_test_x, unit_unit_test_01_01);

            if (ok)
               ok = unit_test_load(&x_test_x, unit_unit_test_01_01);

            if (ok)
               ok = unit_test_load(&x_test_x, unit_unit_test_01_01);

            if (ok)
            {
               int test = unit_test_next_test(&x_test_x);
               ok = test == 0;
            }
            if (ok)
            {
               int test = unit_test_next_test(&x_test_x);
               ok = test == 1;
            }
            if (ok)
            {
               int test = unit_test_next_test(&x_test_x);
               ok = test == 2;
            }
            if (ok)
            {
               /*
                * Here, will will exceed the three loaded tests.
                */

               int test = unit_test_next_test(&x_test_x);
               ok = test == XPCCUT_NO_CURRENT_TEST;
            }
         }
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "1000 fake tests loop"))
      {
         if (ok)
         {
            int ti;
            int test;
            ok = unit_test_init(&x_test_x);                 /* start again */
            if (ok)
            {
               ok = unit_test_options_show_progress_set
               (
                  &x_test_x.m_App_Options, false
               );
            }
            if (ok)
            {
               x_test_x.m_Test_Count = 1000;                /* 1000 fakes  */
               for (ti = 0; ti < 1000; ti++)
               {
                  test = unit_test_next_test(&x_test_x);
                  ok = test == ti;
                  if (! ok)
                  {
                     if (! xpccut_is_silent())
                     {
                        fprintf
                        (
                           stdout, "%s %d\n", _("test failed at count"), ti
                        );
                     }
                     break;
                  }
               }
            }
         }
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "1000 fake while loop"))
      {
         if (ok)
         {
            int ti = 0;
            int test;
            ok = unit_test_init(&x_test_x);                 /* start again */
            if (ok)
            {
               ok = unit_test_options_show_progress_set
               (
                  &x_test_x.m_App_Options, false
               );
            }
            if (ok)
            {
               x_test_x.m_Test_Count = 1000;                /* 1000 fakes  */
               while ((test = unit_test_next_test(&x_test_x)) <= 0)
               {
                  ti++;
               }
               ok = ti == 1000;
               if (! ok)
               {
                  if (! xpccut_is_silent())
                     fprintf(stdout, "  %s == %d\n", _("time counter"), ti);
               }
            }
         }
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "sub-test count"
 *    functionality works.
 *
 *    This test is very rudimentary, and simply tests this function as a
 *    field accessor for unit_test_t, and little more.  The
 *    m_Subtest_Count field is manipulated directly.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   11. unit_test_subtest_count()
 *
 * \test
 *    -  unit_test_subtest_count()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_11 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 11, "unit_test_t", "unit_test_subtest_count()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_subtest_count(nullptr) == 0;
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_subtest_count(&x_test_x) == 0;
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Zero verifaction"))
      {
         x_test_x.m_Subtest_Count = 0;
         if (ok)
            ok = unit_test_subtest_count(&x_test_x) == 0;
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "One verifaction"))
      {
         x_test_x.m_Subtest_Count = 1;
         if (ok)
            ok = unit_test_subtest_count(&x_test_x) == 1;
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "failures count"
 *    functionality works.
 *
 *    This test is very rudimentary, and simply tests this function as a
 *    field accessor for unit_test_t, and little more.  The
 *    m_Total_Errors field is manipulated directly.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   12. unit_test_failures()
 *
 * \test
 *    -  unit_test_failures()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_12 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 12, "unit_test_t", "unit_test_failures()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_failures(nullptr) ==
            XPCCUT_INVALID_PARAMETER;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_failures(&x_test_x) == 0;
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Zero verifaction"))
      {
         x_test_x.m_Total_Errors = 0;
         if (ok)
            ok = unit_test_failures(&x_test_x) == 0;
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "One verifaction"))
      {
         x_test_x.m_Total_Errors = 1;
         if (ok)
            ok = unit_test_failures(&x_test_x) == 1;
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "first-failed-test"
 *    functionality works.
 *
 *    This test is very rudimentary, and simply tests this function as a
 *    field accessor for unit_test_t, and little more.  The
 *    m_First_Failed_Test field is manipulated directly.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   13. unit_test_first_failed_test()
 *
 * \test
 *    -  unit_test_first_failed_test()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_13 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 13, "unit_test_t", "unit_test_first_failed_test()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_first_failed_test(nullptr) ==
            XPCCUT_INVALID_PARAMETER;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_first_failed_test(&x_test_x) == 0;
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Zero verifaction"))
      {
         x_test_x.m_First_Failed_Test = 0;
         if (ok)
            ok = unit_test_first_failed_test(&x_test_x) == 0;
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "One verifaction"))
      {
         x_test_x.m_First_Failed_Test = 1;
         if (ok)
            ok = unit_test_first_failed_test(&x_test_x) == 1;
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the
 *    "first-failed-group" functionality works.
 *
 *    This test is very rudimentary, and simply tests this function as a
 *    field accessor for unit_test_t, and little more.  The
 *    m_First_Failed_Group field is manipulated directly.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   14. unit_test_first_failed_group()
 *
 * \test
 *    -  unit_test_first_failed_group()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_14 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 14, "unit_test_t", "unit_test_first_failed_group()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_first_failed_group(nullptr) ==
            XPCCUT_INVALID_PARAMETER;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_first_failed_group(&x_test_x) == 0;
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Zero verifaction"))
      {
         x_test_x.m_First_Failed_Group = 0;
         if (ok)
            ok = unit_test_first_failed_group(&x_test_x) == 0;
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "One verifaction"))
      {
         x_test_x.m_First_Failed_Group = 1;
         if (ok)
            ok = unit_test_first_failed_group(&x_test_x) == 1;
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "first-failed-case"
 *    functionality works.
 *
 *    This test is very rudimentary, and simply tests this function as a
 *    field accessor for unit_test_t, and little more.  The
 *    m_First_Failed_Case field is manipulated directly.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   15. unit_test_first_failed_case()
 *
 * \test
 *    -  unit_test_first_failed_case()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_15 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 15, "unit_test_t", "unit_test_first_failed_case()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_first_failed_case(nullptr) ==
            XPCCUT_INVALID_PARAMETER;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_first_failed_case(&x_test_x) == 0;
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Zero verifaction"))
      {
         x_test_x.m_First_Failed_Case = 0;
         if (ok)
            ok = unit_test_first_failed_case(&x_test_x) == 0;
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "One verifaction"))
      {
         x_test_x.m_First_Failed_Case = 1;
         if (ok)
            ok = unit_test_first_failed_case(&x_test_x) == 1;
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the
 *    "first-failed-sub-test" functionality works.
 *
 *    This test is very rudimentary, and simply tests this function as a
 *    field accessor for unit_test_t, and little more.  The
 *    m_First_Failed_Subtest field is manipulated directly.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   16. unit_test_first_failed_subtest()
 *
 * \test
 *    -  unit_test_first_failed_subtest()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_16 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 16,
      "unit_test_t", "unit_test_first_failed_subtest()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_first_failed_subtest(nullptr) ==
            XPCCUT_INVALID_PARAMETER;

         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_first_failed_subtest(&x_test_x) == 0;
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Zero verifaction"))
      {
         x_test_x.m_First_Failed_Subtest = 0;
         if (ok)
            ok = unit_test_first_failed_subtest(&x_test_x) == 0;
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "One verifaction"))
      {
         x_test_x.m_First_Failed_Subtest = 1;
         if (ok)
            ok = unit_test_first_failed_subtest(&x_test_x) == 1;
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "run-init"
 *    functionality works.
 *
 *    This test is important, as the unit_test_run_init() function is needed
 *    in any C++ unit-test wrapper library that one writes.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   17. unit_test_run_init()
 *
 * \test
 *    -  unit_test_run_init()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_17 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 17, "unit_test_t", "unit_test_run_init()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*
       * Reduce the amount of output.
       */

      ok = unit_test_options_show_progress_set(&x_test_x.m_App_Options, false);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = unit_test_run_init(nullptr) == 0;
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Initialization verifaction"))
      {
         if (ok)
         {
            ok = x_test_x.m_Start_Time_us.tv_sec == 0;
            if (ok)
               ok = x_test_x.m_Start_Time_us.tv_sec == 0;
         }
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Initialization verifaction"))
      {
         x_test_x.m_Test_Count = 10;
         if (ok)
            ok = unit_test_run_init(&x_test_x) == 10;

         if (ok)
            ok = x_test_x.m_Current_Test_Number  == XPCCUT_NO_CURRENT_TEST;

         if (ok)
            ok = x_test_x.m_First_Failed_Test    ==  0;

         if (ok)
            ok = x_test_x.m_First_Failed_Group   ==  0;

         if (ok)
            ok = x_test_x.m_First_Failed_Case    ==  0;

         if (ok)
            ok = x_test_x.m_First_Failed_Subtest ==  0;

         if (ok)
            ok = x_test_x.m_Total_Errors         ==  0;

         /*
          * How to test?:  the value of the m_Start_Time_us field.
          */

         if (ok)
         {
            ok = x_test_x.m_Start_Time_us.tv_sec > 0;
            if (ok)
               ok = x_test_x.m_Start_Time_us.tv_sec >= 0;
         }
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "dispose-of-test"
 *    functionality works.
 *
 *    This test could potentially be quite complex.  I may get tired before
 *    finishing it!
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   18. unit_test_dispose_of_test()
 *
 * \test
 *    -  unit_test_dispose_of_test()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_18 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 18, "unit_test_t", "unit_test_dispose_of_test()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      unit_test_options_t x_options_x;
      unit_test_status_t x_status_x;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 4;

      /*
       * Now modify the test structure with the opposite values (or at
       * least different values) for every accessible field, so that the
       * resulting test is a bizarro-clone of test 03_01.
       *
       *    ok = unit_test_init(&x_test_x);
       */

      argv[0] = "unit_test_test";
      argv[1] = "--no-show-progress";
      argv[2] = "--sleep-time";
      argv[3] = "10";
      ok = unit_test_initialize
      (
         &x_test_x, argc, argv, "Test 04.18.1", "version", "additionalhelp"
      );
      if (ok)
      {
         x_options_x = x_test_x.m_App_Options;     /* copy fake test options  */
         ok = unit_test_status_initialize
         (
            &x_status_x, &x_options_x, 91, 1, _("Test 04.18"), _("Disposal")
         );
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         /**
          * Recall that unit_test_dispose_of_test() returns 'true' only if
          * the caller should cause the unit-test application to quit.
          */

         cbool_t null_ok = ! unit_test_dispose_of_test(nullptr, nullptr, nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Partial init verification"))
      {
         if (ok)
         {
            ok = x_test_x.m_Start_Time_us.tv_sec == 0;
            if (ok)
               ok = x_test_x.m_Start_Time_us.tv_sec == 0;

            if (ok)
               ok = unit_test_options_test_sleep_time(&x_test_x.m_App_Options)
                     == 10;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Initialization verifaction"))
      {
         /*
          * In this sub-test, since we're passing in a null status pointer,
          * there's no way to fail the test [which can only be done inside
          * the unit_dispose_of_test() function via a call to
          * unit_test_dispose(), which requires a good status pointer.]
          *
          * Therefore, no failure values can occur in this sub-test.
          */

         x_test_x.m_Test_Count = 10;
         if (ok)
            ok = ! unit_test_dispose_of_test(&x_test_x, nullptr, nullptr);

         if (ok)
            ok = x_test_x.m_Current_Test_Number == XPCCUT_NO_CURRENT_TEST;

         if (ok)
            ok = x_test_x.m_Total_Errors == 0;              /* and not 1      */

         if (ok)
            ok = x_test_x.m_First_Failed_Test == 0;         /* and not -1     */

         if (ok)
            ok = x_test_x.m_First_Failed_Group == 0;        /* and not -1     */

         if (ok)
            ok = x_test_x.m_First_Failed_Case == 0;         /* and not -1     */

         if (ok)
            ok = x_test_x.m_First_Failed_Subtest == 0;      /* and not -1     */

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Reinitialize"))
      {
         if (ok)
         {
            argv[0] = "unit_test_test";
            argv[1] = "--no-show-progress";
            argc = 2;
            ok = unit_test_initialize
            (
               &x_test_x, argc, argv, "Test 04.18", "version", "additionalhelp"
            );
         }
         if (ok)
            ok = x_test_x.m_Current_Test_Number == XPCCUT_NO_CURRENT_TEST;

         if (ok)
            ok = x_test_x.m_Total_Errors == 0;

         if (ok)
            ok = x_test_x.m_First_Failed_Test == 0;

         if (ok)
            ok = x_test_x.m_First_Failed_Group == 0;

         if (ok)
            ok = x_test_x.m_First_Failed_Case == 0;

         if (ok)
            ok = x_test_x.m_First_Failed_Subtest == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Stop-on-error test, passed"))
      {
         argv[0] = "unit_test_test";
         argv[1] = "--no-show-progress";
         argv[2] = "--stop-on-error";
         argc = 3;
         ok = unit_test_initialize
         (
            &x_test_x, argc, argv, "Test 04.18.5", "version", "additionalhelp"
         );
         if (ok)
         {
            x_options_x = x_test_x.m_App_Options;        /* fake options      */
            ok = unit_test_status_initialize
            (
               &x_status_x, &x_options_x, 92, 2,
               _("Test 04.18"), _("Disposal")
            );
            if (ok)
            {
               cbool_t runresult;
               ok = ! unit_test_dispose_of_test          /* false == no quit  */
               (
                  &x_test_x, &x_status_x, &runresult
               );
               if (ok)
                  ok = runresult;                        /* made it pass      */
            }
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Stop-on-error test, no quit"))
      {
         argv[0] = "unit_test_test";
         argv[1] = "--no-show-progress";
         argv[2] = "--stop-on-error";
         argc = 3;
         ok = unit_test_initialize
         (
            &x_test_x, argc, argv, "Test 04.18.5", "version", "additionalhelp"
         );
         if (ok)
         {
            x_options_x = x_test_x.m_App_Options;     /* fake test options    */
            ok = unit_test_status_initialize
            (
               &x_status_x, &x_options_x, 92, 2,
               _("Test 04.18"), _("Disposal")
            );
            if (ok)
               ok = unit_test_status_fail(&x_status_x);

            if (ok)
            {
               cbool_t runresult;
               ok = unit_test_dispose_of_test               /* true == quit   */
               (
                  &x_test_x, &x_status_x, &runresult
               );
               if (ok)
                  ok = ! runresult;                         /* made it fail   */
            }

            /*
             * Since we're setting up an artificial situation where we
             * haven't called certain setup functions, a few of the
             * results are weird, but accepted.
             */

            if (ok)
               ok = x_test_x.m_Current_Test_Number == XPCCUT_NO_CURRENT_TEST;

            if (ok)
               ok = x_test_x.m_Total_Errors == 1;

            if (ok)
               ok = x_test_x.m_First_Failed_Test == XPCCUT_NO_CURRENT_TEST;

            if (ok)
               ok = x_test_x.m_First_Failed_Group == 92;

            if (ok)
               ok = x_test_x.m_First_Failed_Case == 2;

            if (ok)
               ok = x_test_x.m_First_Failed_Subtest == 0;   /* weird but true */
         }
         unit_test_status_pass(&status, ok);
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a fake test to use in unit_unit_test_04_19().
 *
 *    This function does not call unit_test_status_next_subtest().
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
fake_unit_test_04_19 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 19,
      _("Unit-test fake run"), "unit_test_run()"
   );
   if (ok)
      unit_test_status_pass(&status, true);                    /* fake a pass */

   return status;
}

/**
 *    Provides a fake test to use in unit_unit_test_04_19() to test the
 *    --force-failure option.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
fake_unit_test_04_19_force_fail (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 19, _("Unit-test force-fail run"), "unit_test_run()"
   );
   if (ok)
   {
      cbool_t result = ! unit_test_options_force_failure(options);
      unit_test_status_pass(&status, result);                  /* fake a fail */
   }
   return status;
}

/**
 *    Provides a fake test to use in unit_unit_test_04_19().
 *
 *    This function does call unit_test_status_next_subtest().
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
fake_subtest_unit_test_04_19 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 19, _("Unit-test fake run"), "unit_test_run()"
   );
   if (ok)
   {
      if (unit_test_status_next_subtest(&status, "Do-nothing test 1"))
         unit_test_status_pass(&status, true);
   }
   if (ok)
   {
      if (unit_test_status_next_subtest(&status, "Do-nothing test 2"))
         unit_test_status_pass(&status, true);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "run"
 *    functionality works.
 *
 *    This test could be complex.  Since this function obviously already
 *    passes the smoke test (we know this because the unit-test application
 *    succeeds, and we've seen it succeed under the debugger), here we
 *    mainly test for the obvious error conditions that can occur in
 *    unit_test_run().
 *
 * Group 4:
 *
 *    unit_test_t functions.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   19. Running and sub-test functionality.
 *
 * \test
 *    -  unit_test_run()
 *    -  unit_test_initialize()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_19 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 19, "unit_test_t", "unit_test_run()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 2;
      argv[0] = "unit_test_test";
      argv[1] = "--no-show-progress";
      ok = unit_test_initialize
      (
         &x_test_x, argc, argv, "Test 04.19.1", "version", "additionalhelp"
      );

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         cbool_t null_ok = ! unit_test_run(nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Empty test list"))
      {
         /*
          * This test insures (indirectly) that unit_test_run() calls
          * unit_test_run_init() to set up the test.
          */

         if (ok)
            ok = ! unit_test_run(&x_test_x);

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "No sub-test test part 1"))
      {
         /*
          * This test checks that the caller used the
          * unit_test_status_next_subtest() function.  Right now, this is a
          * bit of a weird test.  If the caller specified
          * --require-sub-tests on the command-line of this unit-test
          *  application, then we call the fake test function that does
          *  provide a unit test, and vice versa.
          */

         if (unit_test_options_force_failure(options))
         {
            if (ok)
               ok = unit_test_load(&x_test_x, fake_subtest_unit_test_04_19);
         }
         else
         {
            if (ok)
               ok = unit_test_load(&x_test_x, fake_unit_test_04_19);
         }

         if (ok)
            ok = unit_test_run(&x_test_x);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "No sub-test test part 2"))
      {
         /*
          * Tests that not having sub-test properly fails if they are
          * required.  It temporarily modifies this option to force it on.
          * This loaded test should cause unit_test_run() to fail.
          */

         cbool_t force_failure = unit_test_options_force_failure
         (
            &x_test_x.m_App_Options
         );
         if (ok)
            ok = unit_test_load(&x_test_x, fake_unit_test_04_19);

         (void) unit_test_options_force_failure_set
         (
            &x_test_x.m_App_Options, true                /* force option on   */
         );

         /*
          * Even though --force-failure is now set, the fake unit-test
          * function that is loaded does not support it.  Therefore, the
          * following call succeeds (returns 'true').
          */

         if (ok)
            ok = unit_test_run(&x_test_x);

         /*
          * Now use a test that supports the --force-failure option.
          */

         if (ok)
            ok = unit_test_load(&x_test_x, fake_unit_test_04_19_force_fail);

         if (ok)
            ok = ! unit_test_run(&x_test_x);

         (void) unit_test_options_force_failure_set
         (
            &x_test_x.m_App_Options, force_failure
         );
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Stop-on-error test, passed"))
      {
         unit_test_options_t x_options_x;
         unit_test_status_t x_status_x;
         argv[0] = "unit_test_test";
         argv[1] = "--no-show-progress";
         argv[2] = "--stop-on-error";
         argc = 3;
         ok = unit_test_initialize
         (
            &x_test_x, argc, argv, "Test 04.18.5", "version", "additionalhelp"
         );
         if (ok)
         {
            x_options_x = x_test_x.m_App_Options;        /* fake options      */
            ok = unit_test_status_initialize
            (
               &x_status_x, &x_options_x, 92, 2,
               _("Test 04.18"), _("Disposal")
            );
            if (ok)
            {
               /*
                * TODO:
                * Now we need to set up the status structure to indicate
                * that unit_test_run() should quit (and quit with a "passed"
                * status).
                *
                *    1. Need to LOAD a test.
                *    2. Need to set up the status structure properly.
                */

               if (ok)
                  ok = unit_test_load(&x_test_x, fake_subtest_unit_test_04_19);

               if (ok)
                  ok = unit_test_run(&x_test_x);
            }
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Stop-on-error test, no quit"))
      {
         unit_test_options_t x_options_x;
         unit_test_status_t x_status_x;
         argv[0] = "unit_test_test";
         argv[1] = "--no-show-progress";
         argv[2] = "--stop-on-error";
         argc = 3;
         ok = unit_test_initialize
         (
            &x_test_x, argc, argv, "Test 04.18.5", "version", "additionalhelp"
         );
         if (ok)
         {
            x_options_x = x_test_x.m_App_Options;     /* fake test options    */
            ok = unit_test_status_initialize
            (
               &x_status_x, &x_options_x, 92, 2,
               _("Test 04.18"), _("Disposal")
            );
            if (ok)
               ok = unit_test_status_fail(&x_status_x);

            if (ok)
            {
               /*
                * Now we need to set up the status structure to indicate
                * that unit_test_run() should quit (and quit with a "passed"
                * status).
                */

               if (ok)
                  ok = ! unit_test_run(&x_test_x);
            }
         }
         unit_test_status_pass(&status, ok);
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "before-run"
 *    functionality works.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   20. unit_test_run_a_test_before()
 *
 * \test
 *    -  unit_test_run_a_test_before()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_20 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 20,
      "unit_test_t", "unit_test_run_a_test_before()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 2;
      argv[0] = "unit_test_test";
      argv[1] = "--no-show-progress";
      ok = unit_test_initialize
      (
         &x_test_x, argc, argv, "Test 04.20.1", "version", "additionalhelp"
      );

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this' 1"))
      {
         cbool_t null_ok = ! unit_test_run_a_test_before
         (
            nullptr, (intptr_t) nullptr
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this' 2"))
      {
         cbool_t null_ok = ! unit_test_run_a_test_before
         (
            nullptr, (intptr_t) fake_unit_test_04_19
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Null test-pointer"))
      {
         cbool_t null_ok = ! unit_test_run_a_test_before
         (
            &x_test_x, (intptr_t) nullptr
         );
         unit_test_status_pass(&status, null_ok);
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "after-run"
 *    functionality works.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   21. unit_test_run_a_test_after()
 *
 * \test
 *    -  unit_test_run_a_test_after()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_21 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 21,
      "unit_test_t", "unit_test_run_a_test_after()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      unit_test_status_t x_status_x;
      unit_test_options_t x_options_x;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 2;
      argv[0] = "unit_test_test";
      argv[1] = "--no-show-progress";
      ok = unit_test_initialize
      (
         &x_test_x, argc, argv, "Test 04.20.1", "version", "additionalhelp"
      );
      if (ok)
         x_options_x = x_test_x.m_App_Options;

      if (ok)
      {
         ok = unit_test_status_initialize
         (
            &x_status_x, &x_options_x, 90, 3, _("Test 04.21"), _("Disposal")
         );
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this' 1"))
      {
         cbool_t null_ok = ! unit_test_run_a_test_after(nullptr, nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this' 2"))
      {
         cbool_t null_ok = ! unit_test_run_a_test_after(nullptr, &x_status_x);
         unit_test_status_pass(&status, null_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Null status-pointer"))
      {
         cbool_t null_ok = ! unit_test_run_a_test_after(&x_test_x, nullptr);
         unit_test_status_pass(&status, null_ok);
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "run-a-test"
 *    functionality works.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   22. unit_test_run_a_test()
 *
 * \test
 *    -  unit_test_run_a_test()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_22 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 22,
      "unit_test_t", "unit_test_run_a_test()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 2;
      argv[0] = "unit_test_test";
      argv[1] = "--no-show-progress";
      ok = unit_test_initialize
      (
         &x_test_x, argc, argv, "Test 04.22.1", "version", "additionalhelp"
      );

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this' 1"))
      {
         /**
          * Sub-test 1:
          *
          * In the unit_test_run_a_test() function, unit_test_status_init()
          * is always called before checking the arguments.  This is
          * necessary even in the event of failure.  So this sub-test checks
          * both the return result and the internal values of the resulting
          * unit_test_status_t structure.
          */

         unit_test_status_t result = unit_test_run_a_test(nullptr, nullptr);
         cbool_t ok = ! unit_test_status_passed(&result);
         if (ok)
         {
            show_deliberate_failure(options);
            ok = result.m_Test_Options == nullptr;
         }
         if (ok)
            ok = strlen(result.m_Group_Name) == 0;

         if (ok)
            ok = strlen(result.m_Case_Description) == 0;

         if (ok)
            ok = strlen(result.m_Subtest_Name) == 0;

         if (ok)
            ok = result.m_Test_Group == 0;

         if (ok)
            ok = result.m_Test_Case == 0;

         if (ok)
            ok = result.m_Test_Result == false;       /* it failed, remember? */

         if (ok)
            ok = result.m_Subtest_Error_Count == 1;   /* Yup.  It failed.     */

         if (ok)
            ok = result.m_Failed_Subtest == 0;

         if (ok)
            ok = result.m_Test_Disposition == XPCCUT_DISPOSITION_ABORTED;

         if (ok)
            ok = result.m_Start_Time_us.tv_sec == 0;

         if (ok)
            ok = result.m_Start_Time_us.tv_usec == 0;

         if (ok)
            ok = result.m_End_Time_us.tv_sec == 0;

         if (ok)
            ok = result.m_End_Time_us.tv_sec == 0;

         if (ok)
            ok = result.m_Test_Duration_ms == 0.0;

         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this' 2"))
      {
         unit_test_status_t result = unit_test_run_a_test
         (
            nullptr, fake_unit_test_04_19
         );
         cbool_t ok = ! unit_test_status_passed(&result);
         if (ok)
         {
            show_deliberate_failure(options);
            ok = result.m_Test_Options == nullptr;
         }
         if (ok)
            ok = strlen(result.m_Group_Name) == 0;

         if (ok)
            ok = strlen(result.m_Case_Description) == 0;

         if (ok)
            ok = strlen(result.m_Subtest_Name) == 0;

         if (ok)
            ok = result.m_Test_Group == 0;

         if (ok)
            ok = result.m_Test_Case == 0;

         if (ok)
            ok = result.m_Test_Result == false;       /* it failed, remember? */

         if (ok)
            ok = result.m_Subtest_Error_Count == 1;   /* Yup.  It failed.     */

         if (ok)
            ok = result.m_Failed_Subtest == 0;

         if (ok)
            ok = result.m_Test_Disposition == XPCCUT_DISPOSITION_ABORTED;

         if (ok)
            ok = result.m_Start_Time_us.tv_sec == 0;

         if (ok)
            ok = result.m_Start_Time_us.tv_usec == 0;

         if (ok)
            ok = result.m_End_Time_us.tv_sec == 0;

         if (ok)
            ok = result.m_End_Time_us.tv_sec == 0;

         if (ok)
            ok = result.m_Test_Duration_ms == 0.0;
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Null test-pointer"))
      {
         /**
          * \todo
          *    Try casting to function type unit_test_func_t.
          *
          * @gcc warns:
          *
          *    unit_test_run_a_test(&x_test_x, (intptr_t) nullptr);
          */

         unit_test_status_t s =
            unit_test_run_a_test(&x_test_x, (const unit_test_func_t) nullptr);


         ok = ! unit_test_status_passed(&s);
         show_deliberate_failure(options);
         unit_test_status_pass(&status, ok);
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "check-subtest"
 *    functionality works.
 *
 * \group
 *    4. unit_test_t functions.
 *
 * \case
 *   23. unit_test_check_subtests()
 *
 * \test
 *    -  unit_test_check_subtests()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_04_23 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 4, 23,
      "unit_test_t", "unit_test_check_subtests()"
   );
   if (ok)
   {
      unit_test_t x_test_x;
      unit_test_status_t x_status_x;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 2;
      argv[0] = "unit_test_test";
      argv[1] = "--no-show-progress";
      ok = unit_test_initialize
      (
         &x_test_x, argc, argv, "Test 04.23.1", "version", "additionalhelp"
      );
      if (ok)
         ok = unit_test_status_init(&x_status_x);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this' 1"))
      {
         int result = unit_test_check_subtests(nullptr, nullptr);
         ok = result == XPCCUT_INVALID_PARAMETER;
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Null 'this' 2"))
      {
         int result = unit_test_check_subtests(nullptr, &x_status_x);
         ok = result == XPCCUT_INVALID_PARAMETER;
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Null status-pointer"))
      {
         int result = unit_test_check_subtests(&x_test_x, nullptr);
         ok = result == XPCCUT_INVALID_PARAMETER;
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Null status, need subtest"))
      {
         int result = unit_test_check_subtests(&x_test_x, nullptr);
         argv[0] = "unit_test_test";
         argv[1] = "--no-show-progress";
         argv[2] = "--require-sub-tests";
         argc = 3;
         ok = unit_test_initialize
         (
            &x_test_x, argc, argv, "Test 04.23.1", "version", "additionalhelp"
         );
         if (ok)
            ok = result == XPCCUT_INVALID_PARAMETER;  /* checked before init  */

         result = unit_test_check_subtests(&x_test_x, nullptr);
         if (ok)
            ok = result == XPCCUT_INVALID_PARAMETER;  /* checked after init   */

         unit_test_status_pass(&status, ok);
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    This test demonstrates the simplistic setting up of language support.
 *
 * \group
 *    5. Additional functions.
 *
 * \case
 *    1. Verbosity settings and text-domain support.
 *
 * \test
 *    -  xpccut_text_domain()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_05_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 5, 1, _("xpccut_text_domain()"), _("Smoke Test")
   );
   if (ok)
   {
      /*  1 */

      cbool_t original_silence_setting = xpccut_is_silent();
      if (original_silence_setting)
      {
         if (unit_test_options_is_verbose(options))
            xpccut_allow_printing();
      }
      if (unit_test_status_next_subtest(&status, "1"))
      {
#ifndef WIN32
         ok = xpccut_text_domain();
         unit_test_status_pass(&status, ok);
#else
         /*
          * xpccut_errprint_func
          * (
          *    _(
          *       "comment out this test if gettext() for Win32 is not "
          *       "important to you"
          *     )
          * );
          */
#endif
         unit_test_status_pass(&status, ok);
      }

      /*
       * Now restore the original setting of the silence flag, so that
       * the output of the unit-test will have the appearance expected
       * by the dude who ran this test.
       */

      if (original_silence_setting)
         xpccut_silence_printing();
      else
         xpccut_allow_printing();
   }
   return status;
}

/**
 *    This test demonstrates the usage of the unit-test prompt facility.  If
 *    the user answers all the prompts as directed, the test will pass.  This
 *    means that the user cannot abort this test (grin).
 *
 * \group
 *    5. Additional functions.
 *
 * \case
 *    2. Support for interactive prompting in tests.
 *
 * \test
 *    This unit-test function perforce tests more than one unit from the
 *    unit-test library (libxpccut):
 *    -  unit_test_status_initialize()
 *    -  unit_test_status_pass()
 *    -  unit_test_status_fail()
 *    -  unit_test_options_show_values(options) [by user visualization only]
 *    -  unit_test_status_is_verbose() [by user visualization only]
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

/* static */ unit_test_status_t
unit_unit_test_05_02 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 5, 2, _("XPCCUT Test"), _("Prompts")
   );
   if (! unit_test_status_can_proceed(&status))    /* is test allowed to run? */
      unit_test_status_pass(&status, true);        /* no, force it to pass    */
   else if (ok)
   {
      if (unit_test_status_next_subtest(&status, _("'C' test")))
      {
         cbool_t ok = unit_test_status_prompt
         (
            &status,
            _("This sequence of tests runs through the various responses\n"
            "possible when using the prompting facility of the code-and-\n"
            "unit test library.  For this first prompt, please simply hit\n"
            "'p' or 'c', and then the Enter key.")
         );
         if (ok)
         {
            unit_test_status_pass
            (
               &status,
               unit_test_status_disposition(&status) ==
                  XPCCUT_DISPOSITION_CONTINUE
            );
         }
         else
            unit_test_status_pass(&status, true);
      }
      if
      (
         unit_test_status_next_subtest(&status, _("'S' test"))
      )
      {
         (void) unit_test_status_prompt
         (
            &status, _("Press 's' or 'S' followed by the Enter key")
         );

         /*
          * From here on we'll use accessor functions for comparision.  In
          * batch mode (--batch), this function cannot pass, because the
          * answer returned in batch mode is that given by the 'c' key
          * (continue testing).  That is, isSkipped() returns false.  So if
          * we're in batch mode, we'll return true.  The same applies to
          * interactive mode.  Rather than call two functions, we will just
          * call the isOkay() function.
          */

         unit_test_status_pass
         (
            &status,
            unit_test_status_is_skipped(&status) ||
            unit_test_status_is_okay(&status)
         );
      }

      /*
       * We used to do checks like this:
       *
       *    if (unit_test_status_passed(&status) &&
       *       unit_test_status_next_subtest(&status, _("'F' test")))
       *
       * However, the status-passed call is done in the next-subtest call,
       * and also keeps track of the stop-on-error option.  So the test
       * becomes much easier to fit on a single line.
       */

      /*
       * There is no 'f' option for prompting, so this test keeps
       * reprompting over and over until the user realizes (perhaps)
       * that 'f' is not supported.
       *
       *    if (unit_test_status_next_subtest(&status, _("'F' test")))
       *    {
       *       (void) unit_test_status_prompt
       *       (
       *          &status, _("Press 'f' or 'F' followed by the Enter key")
       *       );
       *       (void) unit_test_status_pass
       *       (
       *          &status,
       *          unit_test_status_is_failed(&status) ||
       *          unit_test_status_is_okay(&status)
       *       );
       *    }
       *
       */

      if (unit_test_status_next_subtest(&status, _("'Q' test")))
      {
         (void) unit_test_status_prompt
         (
            &status, _("Press 'q' or 'Q' followed by the Enter key")
         );
         (void) unit_test_status_pass
         (
            &status,
            unit_test_status_is_quitted(&status) ||
            unit_test_status_is_okay(&status)
         );
      }
      if (unit_test_status_next_subtest(&status, _("'A' test")))
      {
         (void) unit_test_status_prompt
         (
            &status, _("Press 'a' or 'A' followed by the Enter key")
         );
         (void) unit_test_status_pass
         (
            &status,
            unit_test_status_is_aborted(&status) ||
            unit_test_status_is_okay(&status)
         );
      }
      if (! unit_test_status_passed(&status))
         xpccut_errprint(_("Hey, why did you disobey me?!"));
   }
   unit_test_status_reset(&status);    /* prevent premature test termination  */
   return status;
}

/**
 *    This test demonstrates the usage of the unit-test prompt facility.  If
 *    the user answers all the prompts as directed, the test will pass.  This
 *    means that the user cannot abort this test (grin).
 *
 * \group
 *    5. Additional functions.
 *
 * \case
 *    3. More prompt support -- quitting.
 *
 * \test
 *    This unit-test function perforce tests more than one unit from the
 *    unit-test library (libxpccut):
 *
 *    -  unit_test_status_initialize()
 *    -  unit_test_status_pass()
 *    -  unit_test_status_fail()
 *    -  unit_test_status_is_verbose() [by user visualization only]
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

/* static */ unit_test_status_t
unit_unit_test_05_03 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 5, 3, _("XPCCUT Test"), _("Disposal")
   );
   if (! unit_test_status_can_proceed(&status))    /* is test allowed to run? */
      unit_test_status_pass(&status, true);        /* no, force it to pass    */
   else if (ok)
   {
      if (unit_test_status_next_subtest(&status, _("'Q' test")))
      {
         (void) unit_test_status_prompt
         (
            &status,
            _("Press 'q' or 'a' to halt with success or failure, at your option")
         );
      }
   }
   return status;
}

/**
 *    This test demonstrates the usage of the unit-test prompt facility.  If
 *    the user answers all the prompts as directed, the test will pass.  This
 *    means that the user cannot abort this test (grin).
 *
 * \group
 *    5. Additional functions.
 *
 * \case
 *    4. More prompt support -- continuing.
 *
 * \test
 *    This unit-test function perforce tests more than one unit from the
 *    unit-test library (libxpccut):
 *
 *    -  unit_test_status_initialize()
 *    -  unit_test_status_pass()
 *    -  unit_test_status_fail()
 *    -  unit_test_status_is_verbose() [by user visualization only]
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

/* static */ unit_test_status_t
unit_unit_test_05_04 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 5, 4, _("XPCCUT Test"), _("Continuation")
   );
   if (! unit_test_status_can_proceed(&status))    /* is test allowed to run? */
      unit_test_status_pass(&status, true);        /* no, force it to pass    */
   else if (ok)
   {
      unit_test_status_pass(&status, true);
      if (unit_test_options_is_verbose(options))
      {
         fprintf
         (
            stdout, "  %s\n",
            _("You will see this message only if you didn't answer 'q' before")
         );
      }
      if (unit_test_status_next_subtest(&status, _("Continuation test")))
         (void) unit_test_status_prompt(&status, _("Answer as you wish"));
   }
   return status;
}

/**
 *    Recapitulates unit_unit_test_01_01().
 *
 *    Why?  To make it easy to check up on some aspects of the --summarize
 *    option.
 *
 * \group
 *    6. Test recapitulation.
 *
 * \case
 *    1. 01.01 smoke-test recapitulation.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_06_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 6, 1, "XPCCUT", _("01.01 Smoke Test Recap")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         if (unit_test_options_show_values(options))
            fprintf(stdout, "  %s\n", _("No values to show in this test"));

         /*  1 */

         if (unit_test_status_next_subtest(&status, "unit_test_status_pass()"))
         {
            unit_test_status_pass(&status, true);
            ok =
            (
               (status.m_Subtest_Error_Count == 0) &&
               (status.m_Failed_Subtest == 0)
            );
            if (! ok)
            {
               (void) unit_test_status_fail(&status);    /* create a failure  */
               if (unit_test_options_is_verbose(options))
               {
                  fprintf
                  (
                     stderr, "%s %s\n",
                     "unit_test_status_pass()", _("internal failure")
                  );
               }
            }
            unit_test_status_pass(&status, ok);
         }

         /*  2 */

         if
         (
            ok &&
            unit_test_status_next_subtest(&status, "unit_test_status_fail()")
         )
         {
            ok = unit_test_status_fail_deliberately(&status);
            if (ok)
            {
               ok =
               (
                  (status.m_Subtest_Error_Count == 1) &&
                  (status.m_Failed_Subtest == 2)
               );
               unit_test_status_pass(&status, ok); /* convert it to a "pass"  */
               if (ok)
               {
                  status.m_Subtest_Error_Count--;  /* direct access (bad) !!! */
                  status.m_Failed_Subtest = 0;     /* direct access (bad) !!! */
               }
            }
         }
         /*
          * This visual duration check is useless.
          *
          * if (ok)
          *    xpccut_ms_sleep(500);               // a visual duration check
          *
          */
      }
   }
   return status;
}

/**
 *    Provides tests of some of the macros, so we can verify they are being
 *    defined properly.
 *
 * \group
 *    7. Macro tests
 *
 * \case
 *    1. cut_not_nullptr() and cut_is_nullptr() macros.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_07_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 7, 1, "XPCCUT", _("cut_xxx_nullptr() tests")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         const char * good = "allo";
         const char * bad = nullptr;

         /*  1 */

         if (unit_test_status_next_subtest(&status, "cut_not_nullptr()"))
         {
#ifdef XPC_NO_NULLPTR
            ok = cut_not_nullptr(good) == true;
            if (ok)
               ok = cut_not_nullptr(bad) == true;
#else
            ok = cut_not_nullptr(good) == true;
            if (ok)
               ok = cut_not_nullptr(bad) == false;
#endif
            unit_test_status_pass(&status, ok);
         }

         /*  2 */

         if (unit_test_status_next_subtest(&status, "cut_is_nullptr()"))
         {
#ifdef XPC_NO_NULLPTR
            ok = cut_is_nullptr(good) == false;
            if (ok)
               ok = cut_is_nullptr(bad) == false;
#else
            ok = cut_is_nullptr(good) == false;
            if (ok)
               ok = cut_is_nullptr(bad) == true;
#endif
            unit_test_status_pass(&status, ok);
         }
      }
   }
   return status;
}

/**
 *    Provides tests of some of the macros, so we can verify they are being
 *    defined properly.
 *
 * \group
 *    7. Macro tests
 *
 * \case
 *    2. cut_not_thisptr() and cut_is_thisptr() macros.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_07_02 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 7, 2, "XPCCUT", _("cut_xxx_thisptr() tests")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
#ifndef XPC_NO_THISPTR
         const char * good = "allo";
         const char * bad = nullptr;
#endif

         /*  1 */

         if (unit_test_status_next_subtest(&status, "cut_not_thisptr()"))
         {
#ifdef XPC_NO_THISPTR

            /*
             * Note that this code will never be run -- it is prevented in
             * main(), since the trying to do stuff with a null pointer is
             * inevitably a segfault.
             */

            ok = cut_not_thisptr(nullptr) == false;
            if (ok)
               ok = cut_not_thisptr(nullptr) == false;
#else
            ok = cut_not_thisptr(good) == false;
            if (ok)
               ok = cut_not_thisptr(bad) == true;
#endif
            unit_test_status_pass(&status, ok);
         }

         /*  2 */

         if (unit_test_status_next_subtest(&status, "cut_is_thisptr()"))
         {
#ifdef XPC_NO_THISPTR
            ok = cut_is_thisptr(nullptr) == true;
            if (ok)
               ok = cut_is_thisptr(nullptr) == true;
#else
            ok = cut_is_thisptr(good) == true;
            if (ok)
               ok = cut_is_thisptr(bad) == false;
#endif
            unit_test_status_pass(&status, ok);
         }
      }
   }
   return status;
}

/**
 *    Provides tests of some of the macros, so we can verify they are being
 *    defined properly.
 *
 * \group
 *    7. Macro tests
 *
 * \case
 *    3. xpccut_thisptr() macro.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_07_03 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 7, 3, "XPCCUT", _("xpccut_thisptr() tests")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
#ifndef XPC_NO_THISPTR
         const char * good = "allo";
         const char * bad = nullptr;
#endif

         /*  1 */

         if (unit_test_status_next_subtest(&status, "xpccut_thisptr()"))
         {

#ifdef XPC_NO_THISPTR

            /*
             * Note that this code will never be run -- it is prevented in
             * main(), since the trying to do stuff with a null pointer is
             * inevitably a segfault.
             */

            ok = xpccut_thisptr(nullptr) == true;
            if (ok)
               ok = xpccut_thisptr(nullptr) == true;
#else
            ok = xpccut_thisptr(good) == true;
            if (ok)
               ok = xpccut_thisptr(bad) == false;
#endif
            unit_test_status_pass(&status, ok);
         }
      }
   }
   return status;
}

/**
 *    Provides a unit test for xpccut_nullptr() in the portable_subset.c
 *    module.
 *
 *    Why?  We don't really want deep testing on this purely internal
 *    module, but we need a good example of the usage of coverage testing.
 *
 *    Hence, this test group is provided.  It can be disabled via the
 *    USE_XPCCUT_NULLPTR_TEST in order to test the usefulness of coverage
 *    testing.
 *
 * \group
 *    8. Special null-pointer test.
 *
 * \case
 *    1. xxxx
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_08_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 8, 1, "xpccut_nullptr", _("01.01 Coverage Test Recap")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {

#ifdef USE_XPCCUT_NULLPTR_TEST

         if (unit_test_options_show_values(options))
            fprintf(stdout, "  %s\n", _("No values to show in this test"));

         /*  1 */

         if (unit_test_status_next_subtest(&status, "xpccut_nullptr()"))
         {
            ok = ! xpccut_nullptr(nullptr, nullptr, nullptr);
            unit_test_status_pass(&status, ok);
         }

         /*  2 */

         if (ok && unit_test_status_next_subtest(&status, "xpccut_nullptr()"))
         {
            // more to come
         }
#else
         unit_test_status_pass(&status, true);     /* no, force it to pass    */

#endif      // USE_XPCCUT_NULLPTR_TEST

      }
   }
   return status;
}

/**
 *    Provides the number of data points we have for checking
 *    xpccut_random().
 */

#define SEED_1_RESULTS_SIZE         60

/**
 *    Provides regression data to test the random-number generator.
 *
 *    The values generated with a seed of 1 should match this list.  There
 *    are 60 values in this list.
 */

static unsigned int gs_selinger_results[SEED_1_RESULTS_SIZE] =
{
   1804289383,  846930886, 1681692777, 1714636915, 1957747793,
    424238335,  719885386, 1649760492,  596516649, 1189641421,
   1025202362, 1350490027,  783368690, 1102520059, 2044897763,
   1967513926, 1365180540, 1540383426,  304089172, 1303455736,
     35005211,  521595368,  294702567, 1726956429,  336465782,
    861021530,  278722862,  233665123, 2145174067,  468703135,
   1101513929, 1801979802, 1315634022,  635723058, 1369133069,
   1125898167, 1059961393, 2089018456,  628175011, 1656478042,
   1131176229, 1653377373,  859484421, 1914544919,  608413784,
    756898537, 1734575198, 1973594324,  149798315, 2038664370,
   1129566413,  184803526,  412776091, 1424268980, 1911759956,
    749241873,  137806862,   42999170,  982906996,  135497281
};

/**
 *    Provides the maximum number of xpccut_random() calls for subtest 3 of
 *    test 09.01.
 *
 *    We used UINT_MAX, but this makes the unit test take a few minutes.
 *    So, having already verified that we never get a value equal to the
 *    first random number we pulled within UINT_MAX tries, we content
 *    ourselves with skipping past this test, essentially.
 */

#define MAX_COUNT_09_01_03    100      //  UINT_MAX

/**
 *    Provides a unit test for the random number functions.
 *
 * \group
 *    9. Random numbers
 *
 * \case
 *    1. Basics
 *
 * \test
 *    -  xpccut_srandom()
 *    -  xpccut_random()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_09_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 9, 1, "Random numbers", _("Basics")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         /*  1 */

         if (unit_test_status_next_subtest(&status, "xpccut_srandom()"))
         {
            unsigned int s = xpccut_srandom(1);    /* set the default seed    */
            ok = s == 1;
            unit_test_status_pass(&status, ok);
         }

         /*  2 */

         if (unit_test_status_next_subtest(&status, "xpccut_random()"))
         {
            int i;
            unsigned int our_results[SEED_1_RESULTS_SIZE];
            if (unit_test_options_show_values(options))
            {
               fprintf
               (
                  stdout,
                  "\n"
                  "The following results should match the table found at\n"
                  "http://www.mscs.dal.ca/~selinger/random/\n\n"
               );
            }
            for (i = 0; i < SEED_1_RESULTS_SIZE; i++)
            {
               unsigned int o = xpccut_random();
               ok = o == gs_selinger_results[i];
               if (ok)
               {
                  our_results[i] = o;
               }
               else
               {
                  xpccut_errprint(_("bad random value"));
                  break;
               }
            }
            if (ok)
            {
               if (unit_test_options_show_values(options))
               {
                  int i;
                  int j;
                  for (i = 0; i < 12; i++)
                  {
                     int k = i;
                     for (j = 0; j < SEED_1_RESULTS_SIZE/12; j++)
                     {
                        int o = our_results[k];
                        fprintf(stdout, "%2d: %10d  ", k, o);
                        k += 12;
                     }
                     fprintf(stdout, "\n");
                  }
                  fprintf(stdout, "\n");
               }
            }
            unit_test_status_pass(&status, ok);
         }

         /*  3 */

         if (unit_test_status_next_subtest(&status, "Repeats"))
         {
            unsigned int s = xpccut_srandom(1234);
            ok = s == 1234;
            if (ok)
            {
               unsigned int i;
               unsigned int q;
               unsigned int r = xpccut_random();
               for (i = 0; i < MAX_COUNT_09_01_03; i++)
               {
                  q = xpccut_random();
                  if (q == r)
                  {
                     ok = false;
                     if (! xpccut_is_silent())
                     {
                        fprintf
                        (
                           stdout,
                           "Duplicate random number after %u iterations.\n", i
                        );
                     }
                     break;
                  }
               }
            }
            unit_test_status_pass(&status, ok);
         }
      }
   }
   return status;
}

/**
 *    Provides a unit test for the random number functions.
 *
 * \group
 *    9. Random numbers
 *
 * \case
 *    2. Comparison to GNU random()
 *
 * \test
 *    -  xpccut_srandom()
 *    -  xpccut_random()
 *    -  xpccut_rand()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_09_02 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 9, 2, "Random numbers", _("Comparison to GNU random()")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         /*  1 */

         if (unit_test_status_next_subtest(&status, "Seeding"))
         {
            unsigned int s = xpccut_srandom(1);    /* set the default seed    */
            ok = s == 1;

#ifndef WIN32
            if (ok)
               srandom(1);                         /* seed the GNU version    */
#endif

            unit_test_status_pass(&status, ok);
         }

         /*  2 */

         if (unit_test_status_next_subtest(&status, "xpccut_random()"))
         {
            int i;
            for (i = 0; i < 10000000; i++)         /* 10 million tries        */
            {
#ifndef WIN32
               unsigned int g = random();
#endif
               unsigned int o = xpccut_random();
               if (i < SEED_1_RESULTS_SIZE)
                  ok = o == gs_selinger_results[i];

#ifndef WIN32
               if (ok)
                  ok = o == g;
#endif

               if (! ok)
               {
                  xpccut_errprint(_("random() results don't match"));
                  break;
               }
            }
            unit_test_status_pass(&status, ok);
         }

         /*  3 */

         if (unit_test_status_next_subtest(&status, "xpccut_rand()"))
         {
            int i;
            for (i = 0; i < 10000000; i++)         /* 10 million tries        */
            {
               unsigned int o = xpccut_rand(1000000);
               ok = /* (o >= 0) && */ (o < 1000000);
               if (! ok)
               {
                  xpccut_errprint(_("xpccut_rand() gave unexpected result"));
                  break;
               }
            }
            unit_test_status_pass(&status, ok);
         }
      }
   }
   return status;
}

/**
 *    Provides a unit test for the memcmp() function.
 *
 * \group
 *    9. Random numbers (sort of)
 *
 * \case
 *    3. memcmp()
 *
 * \test
 *    -  memcmp()
 *
 *    The reason for this test is to clarify what memcmp() returns and to
 *    make sure it returns the same set of values on any platform.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_09_03 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 9, 3, "Random numbers", _("Comparison to GNU random()")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         /*  1 */

         if (unit_test_status_next_subtest(&status, "Equal"))
         {
            int s = memcmp("012345", "012345", 6);
            ok = s == 0;
            unit_test_status_pass(&status, ok);
         }

         /*  2 */

         if (unit_test_status_next_subtest(&status, "Less than (1)"))
         {
            int s = memcmp("012345", "A12345", 6);
            ok = s == -1;
            unit_test_status_pass(&status, ok);
         }

         /*  3 */

         if (unit_test_status_next_subtest(&status, "Less than (2)"))
         {
            int s = memcmp("012345", "0B2345", 6);
            ok = s == -1;
            unit_test_status_pass(&status, ok);
         }

         /*  4 */

         if (unit_test_status_next_subtest(&status, "Greater than (1)"))
         {
            int s = memcmp("A12345", "012345", 6);
            ok = s == 1;
            unit_test_status_pass(&status, ok);
         }

         /*  5 */

         if (unit_test_status_next_subtest(&status, "Greater than (2)"))
         {
            int s = memcmp("0B2345", "012345", 6);
            ok = s == 1;
            unit_test_status_pass(&status, ok);
         }

      }
   }
   return status;
}

/**
 * \param s1
 *    Provides the first string to match.
 *
 * \param s2
 *    Provides the second string to match.
 *
 * \param bytecount
 *    The number of bytes to compare.
 */

static cbool_t
xpccut_match
(
   const char * s1,
   const char * s2,
   int bytecount
)
{
   int mismatch = memcmp(s1, s2, bytecount);
   cbool_t ok = mismatch == 0;
   if (! ok)
   {
      if (! xpccut_is_silent())
      {
         if (mismatch < 0)
            mismatch = -mismatch;

         fprintf
         (
            stdout,
            "? Mismatch at character %d ('0x%02x' versus '0x%02x')\n",
            mismatch, s1[mismatch], s2[mismatch]
         );
      }
   }
   return ok;
}

/**
 *    Provides a unit test for the fuzz-related functions.
 *
 * \group
 *   10. Fuzz-testing
 *
 * \case
 *    1. Character-set building
 *
 * \test
 *    -  xpccut_fuzz(), with XPCCUT_FF_DUMP_CHARSET set.  Nothing is done
 *       except to build the character-set that would be used to construct
 *       the fuzz string, and return this character set for verification.
 *    -  xpccut_build_character_set() is the hidden (static) function that
 *       builds the character set.
 *    -  xpccut_exclude_characters() pares out any "excluded characters"
 *       that were specified.
 *    -  xpccut_dump_string() provides a visual test of the output if the
 *       user specifies the --show-values option on the command-line.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

#define FAKE_SIZE  1                   // avoids an error

static unit_test_status_t
unit_unit_test_10_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 10, 1, "Fuzz functions", _("Character-set building")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         /*
          * This comparision string is useful in checking some of the
          * results.  The bytes in it range from 0 to 256.  To use it, pick
          * the right offset and size to match the string you want to
          * compare.
          */

         char comparison_string[260];
         int i;
         for (i = 0; i < 256; i++)
            comparison_string[i] = (char) i;

         /*  1 */

         if (unit_test_status_next_subtest(&status, "xpccut_dump_string()"))
         {
            if (unit_test_options_show_values(options))
            {
               /*
                * This test is just toying around.
                *
                * const char * teststring =
                *    "(void) unit_test_load(&testbattery, unit_unit_test_02_27);"
                *    "g\n\a\t\b\v\"\\g"
                *    "g\x01\x02\x03\x04\x05\x06\x07\x08\x09g"
                *    "\0"
                *    ;
                *
                * We extend the string by one character to be able to output
                * the final null character.
                *
                * xpccut_dump_string(teststring, strlen(teststring)+1);
                */

               char teststring[260];
               int i;
               for (i = 0; i < 260; i++)
                  teststring[i] = (char) i;

               xpccut_dump_string(teststring, 260);

               /*
                * This test is a bit bogus, but it is just a sanity check.
                */

               ok = xpccut_match(&teststring[0], &comparison_string[0], 256);
               if (! ok)
               {
                  fprintf(stdout, "=========== Comparison String ===========\n");
                  xpccut_dump_string(comparison_string, 260);
               }
               unit_test_status_pass(&status, ok);
            }
         }

         /*  2 */

         if (unit_test_status_next_subtest(&status, "Default character set"))
         {
            char character_set[256];
            unsigned int seed = xpccut_fuzz
            (
               &character_set[0], sizeof(character_set), FAKE_SIZE,
               XPCCUT_SEED_SKIP,
               XPCCUT_FF_DUMP_CHARSET,
               nullptr, nullptr, nullptr, nullptr
            );
            if (unit_test_options_show_values(options))
            {
               xpccut_dump_string(character_set, sizeof(character_set));
            }
            ok = seed != XPCCUT_SEED_ERROR;
            if (ok)
               ok = xpccut_match(&character_set[0], &comparison_string[1], 255);

            unit_test_status_pass(&status, ok);
         }

         /*  3 */

         if (unit_test_status_next_subtest(&status, "Letters only"))
         {
            char character_set[256];
            int size_to_use = 54;         // don't need sizeof(character_set)
            unsigned int seed = xpccut_fuzz
            (
               &character_set[0], size_to_use,
               FAKE_SIZE, XPCCUT_SEED_SKIP,
               XPCCUT_FF_LETTERS_ONLY | XPCCUT_FF_DUMP_CHARSET,
               nullptr, nullptr, nullptr, nullptr
            );
            if (unit_test_options_show_values(options))
            {
               xpccut_dump_string(character_set, size_to_use);
            }
            ok = seed != XPCCUT_SEED_ERROR;
            if (ok)
               ok = xpccut_match(&character_set[0], &comparison_string[65], 26);

            if (ok)
               ok = xpccut_match(&character_set[26], &comparison_string[97], 26);

            unit_test_status_pass(&status, ok);
         }

         /*  4 */

         if (unit_test_status_next_subtest(&status, "Numbers only"))
         {
            char character_set[256];
            int size_to_use = 18;         // don't need sizeof(character_set)
            const char * number_stuff = "0123456789+-.";
            unsigned int seed = xpccut_fuzz
            (
               &character_set[0], size_to_use,
               FAKE_SIZE, XPCCUT_SEED_SKIP,
               XPCCUT_FF_NUMBERS_ONLY | XPCCUT_FF_DUMP_CHARSET,
               nullptr, nullptr, nullptr, nullptr
            );
            if (unit_test_options_show_values(options))
            {
               xpccut_dump_string(character_set, size_to_use);
            }
            ok = seed != XPCCUT_SEED_ERROR;
            if (ok)
               ok = xpccut_match(&character_set[0], &number_stuff[0], 13);

            unit_test_status_pass(&status, ok);
         }

         /*  5 */

         if (unit_test_status_next_subtest(&status, "Letters and numbers"))
         {
            char character_set[256];
            int size_to_use = 66;         // don't need sizeof(character_set)
            const char * char_stuff =
               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
               "abcdefghijklmnopqrstuvwxyz"
               "0123456789+-."
               ;
            int length = strlen(char_stuff);
            unsigned int seed = xpccut_fuzz
            (
               &character_set[0], size_to_use,
               FAKE_SIZE, XPCCUT_SEED_SKIP,
               XPCCUT_FF_NUMBERS_ONLY |
                   XPCCUT_FF_LETTERS_ONLY | XPCCUT_FF_DUMP_CHARSET,
               nullptr, nullptr, nullptr, nullptr
            );
            if (unit_test_options_show_values(options))
            {
               xpccut_dump_string(character_set, size_to_use);
            }
            ok = seed != XPCCUT_SEED_ERROR;
            if (ok)
               ok = xpccut_match(&character_set[0], &char_stuff[0], length);

            unit_test_status_pass(&status, ok);
         }

         /*  6 */

         if (unit_test_status_next_subtest(&status, "Uppcase letters/numbers"))
         {
            char character_set[256];
            int size_to_use = 42;         // don't need sizeof(character_set)
            const char * char_stuff =
               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
               "0123456789+-."
               ;
            int length = strlen(char_stuff);
            unsigned int seed = xpccut_fuzz
            (
               &character_set[0], size_to_use,
               FAKE_SIZE, XPCCUT_SEED_SKIP,
               XPCCUT_FF_NUMBERS_ONLY |
                   XPCCUT_FF_LETTERS_ONLY | XPCCUT_FF_DUMP_CHARSET,
               nullptr,
               "abcdefghijklmnopqrstuvwxyz",
               nullptr, nullptr
            );
            if (unit_test_options_show_values(options))
            {
               xpccut_dump_string(character_set, size_to_use);
            }
            ok = seed != XPCCUT_SEED_ERROR;
            if (ok)
               ok = xpccut_match(&character_set[0], &char_stuff[0], length);

            unit_test_status_pass(&status, ok);
         }

         /*  7 */

         if (unit_test_status_next_subtest(&status, "Lowercase letters special"))
         {
            char character_set[256];
            int size_to_use = 30;         // don't need sizeof(character_set)
            const char * char_stuff = "abcdefghijklmnopqrstuvwxyz";
            int length = strlen(char_stuff);
            unsigned int seed = xpccut_fuzz
            (
               &character_set[0], size_to_use,
               FAKE_SIZE, XPCCUT_SEED_SKIP,
               XPCCUT_FF_DUMP_CHARSET,
               char_stuff,
               nullptr, nullptr, nullptr
            );
            if (unit_test_options_show_values(options))
            {
               xpccut_dump_string(character_set, size_to_use);
            }
            ok = seed != XPCCUT_SEED_ERROR;
            if (ok)
               ok = xpccut_match(&character_set[0], &char_stuff[0], length);

            unit_test_status_pass(&status, ok);
         }

         /*  8 */

         if (unit_test_status_next_subtest(&status, "Lowercase consonants"))
         {
            char character_set[256];
            int size_to_use = 30;         // don't need sizeof(character_set)
            const char * char_stuff = "abcdefghijklmnopqrstuvwxyz";
            const char * consonants = "bcdfghjklmnpqrstvwxyz";
            int length = strlen(consonants);
            unsigned int seed = xpccut_fuzz
            (
               &character_set[0], size_to_use,
               FAKE_SIZE, XPCCUT_SEED_SKIP,
               XPCCUT_FF_DUMP_CHARSET,
               char_stuff,
               "aeiou",
               nullptr, nullptr
            );
            if (unit_test_options_show_values(options))
            {
               xpccut_dump_string(character_set, size_to_use);
            }
            ok = seed != XPCCUT_SEED_ERROR;
            if (ok)
               ok = xpccut_match(&character_set[0], &consonants[0], length);

            unit_test_status_pass(&status, ok);
         }
      }
   }
   return status;
}

/**
 *    Generates a single random character.
 *    The main purpose of this function is to test the randomness of the
 *    generated characters, mostly to make sure our algorithms are not
 *    flawed.
 *
 * \param destination
 *    Provides the destination buffer.
 *
 * \param dlength
 *    Provides the length of the destination buffer.
 *
 * \return
 *    Returns 'true' if the function succeeded.
 */

static cbool_t
xpccut_fuzz_single_character
(
   char * destination,
   int dlength
)
{
   int number_of_bytes = 1;                           /* very short integers  */
   int seed = xpccut_fuzz
   (
      destination, dlength,
      number_of_bytes,
      XPCCUT_SEED_SKIP,
      XPCCUT_FF_DEFAULT,
      nullptr,                                        /* allowed characters   */
      nullptr,                                        /* excluded characters  */
      nullptr,                                        /* prologue characters  */
      nullptr                                         /* epilogue characters  */
   );
   return seed != XPCCUT_SEED_ERROR;
}

/**
 *    Generates a two-digit random number string.
 *
 * \param destination
 *    Provides the destination buffer.
 *
 * \param dlength
 *    Provides the length of the destination buffer.
 *
 * \param extra_flags
 *    Additional flags over and above the built-in XPCCUT_FF_NUMBERS_ONLY
 *    flag.
 *
 * \return
 *    Returns 'true' if the function succeeded.
 */

static cbool_t
xpccut_fuzz_two_byte_integers
(
   char * destination,
   int dlength,
   xpccut_fuzz_flags_t extra_flags
)
{
   int number_of_bytes = 2;                           /* very short integers  */
   xpccut_fuzz_flags_t flags = XPCCUT_FF_NUMBERS_ONLY | extra_flags;
   int seed = xpccut_fuzz
   (
      destination, dlength,
      number_of_bytes,
      XPCCUT_SEED_SKIP,
      flags,
      nullptr,                                        /* allowed characters   */
      "+-.",                                          /* excluded characters  */
      nullptr,                                        /* prologue characters  */
      nullptr                                         /* epilogue characters  */
   );
   return seed != XPCCUT_SEED_ERROR;
}

/**
 *    Generates a random string of random size from 0 to 10 characters.
 *    The main purpose of this function is to test the randomness of the
 *    generated characters, mostly to make sure our algorithms are not
 *    flawed.
 *
 * \param destination
 *    Provides the destination buffer.
 *
 * \param dlength
 *    Provides the length of the destination buffer.
 *
 * \return
 *    Returns 'true' if the function succeeded.
 */

static cbool_t
xpccut_fuzz_random_size_string
(
   char * destination,
   int dlength
)
{
   int number_of_bytes = 10;                          /* very short integers  */
   int seed = xpccut_fuzz
   (
      destination, dlength,
      number_of_bytes,
      XPCCUT_SEED_SKIP,
      XPCCUT_FF_RANDOM_SIZE,
      nullptr,                                        /* allowed characters   */
      nullptr,                                        /* excluded characters  */
      nullptr,                                        /* prologue characters  */
      nullptr                                         /* epilogue characters  */
   );
   return seed != XPCCUT_SEED_ERROR;
}

/**
 *    Provides a unit test for the fuzz-related functions.
 *
 * \group
 *   10. Fuzz-testing
 *
 * \case
 *    2. Small numeric strings.
 *
 * \test
 *    -  xpccut_fuzz()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_10_02 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 10, 2, "Fuzz functions", _("Small numeric strings")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         /*  1 */

         if (unit_test_status_next_subtest(&status, "2-byte integers"))
         {
            int i, j, k;
            char destination[4];
            int histogram[100];
            if (unit_test_options_show_values(options))
            {
               // nada
            }

            for (i = 0; i < 100; i++)
               histogram[i] = 0;

            (void) xpccut_set_seed(99);

            // for (i = 0; i < 100000; i++)

            for (i = 0; i < 1000; i++)
            {
               int value;
               ok = xpccut_fuzz_two_byte_integers
               (
                  destination, sizeof(destination),
                  XPCCUT_FF_DEFAULT
               );
               if (ok)
               {
                  value = atoi(destination);
                  if (unit_test_options_show_values(options))
                     fprintf(stdout, "%2d\n", value);

                  if (value >= 0 && value < 100)
                     histogram[value]++;
                  else
                  {
                     xpccut_errprint(_("bad numerical value encountered"));
                     break;
                  }
               }
               else
                  break;
            }
            if (unit_test_options_show_values(options))
            {
               fprintf
               (
                  stdout,
         "        0     1     2     3     4     5     6     7     8     9\n"
         "  ==============================================================\n"
               );
               for (i = 0, k = 0; i < 10; i++)
               {
                  fprintf(stdout, "%2d| ", i);
                  for (j = 0; j < 10; j++)
                  {
                     fprintf(stdout, " %4d ", histogram[k++]);
                  }
                  fprintf(stdout, "\n");
               }
            }
            unit_test_status_pass(&status, ok);
         }
      }
   }
   return status;
}

/**
 *    Provides a unit test for the fuzz-related functions.
 *
 * \group
 *   10. Fuzz-testing
 *
 * \case
 *    3. Single-character randomness test
 *
 * \test
 *    -  xpccut_fuzz()
 *
 *    This test is a bit different.  Here is its features:
 *
 *       -  Generates only single-character strings
 *       -  Outputs the characters to standard output in a format suitable
 *          for usage with the dieharder application.
 *
 \verbatim
      $ ./unit_test_test --group 9 --case 3 --silent --dump-text > 10_03.input
      $  dieharder -a -g 202 -f 10_03.input
 \endverbatim
 *
 *    With the use of the crappy rand() function, the output looks random,
 *    but FAILs all of the dieharder tests (which all told take a lot of
 *    minutes).  Not yet sure if we really need to use an improved random
 *    number generator or not.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_10_03 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 10, 3, "Fuzz functions",
      _("Single-character randomness test")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         /*  1 */

         if (unit_test_status_next_subtest(&status, "Single character"))
         {
            int i, j, k;
            char dest[4];
            int histogram[256];
            int count = 1000 * 256;
            int seed = 22;
            if (unit_test_options_show_values(options))
            {
               // nada
            }

            for (i = 0; i < 256; i++)
               histogram[i] = 0;

            xpccut_set_seed(seed);
            if (g_do_dump_text)
            {
               fprintf
               (
                  stdout,
      "#==================================================================\n"
      "# generator xpccut_fuzz seed = %d\n"
      "#==================================================================\n"
      "type: d\n"
      "count: %d\n"
      "numbit: 8\n"
                  ,
                  seed, count
               );
            }
            for (i = 0; i < count; i++)
            {
               unsigned value;
               ok = xpccut_fuzz_single_character(dest, sizeof(dest));
               if (ok)
               {
                  value = (unsigned) dest[0];
                  value &= 0xff;
                  histogram[value]++;
                  if (value >= 256)
                  {
                     xpccut_errprint(_("bad character value encountered"));
                     break;
                  }
                  if (g_do_dump_text)
                     fprintf(stdout, "%d\n", value);
               }
               else
               {
                  xpccut_errprint_func(_("fuzz error"));
                  break;
               }
            }

            if (unit_test_options_show_values(options))
            {
               fprintf
               (
                  stdout,
         "        0     1     2     3     4     5     6     7     8     9\n"
         "  ==============================================================\n"
               );
               for (i = 0, k = 0; i < (256/10+1); i++)
               {
                  fprintf(stdout, "%2d| ", i);
                  for (j = 0; j < 10; j++)
                  {
                     fprintf(stdout, " %4d ", histogram[k++]);
                     if (k >= 256)
                        break;
                  }
                  fprintf(stdout, "\n");
                  if (k >= 256)
                     break;
               }
            }
            unit_test_status_pass(&status, ok);
         }
      }
   }
   return status;
}

/**
 *    Provides a unit test for the fuzz-related functions.
 *
 * \group
 *   10. Fuzz-testing
 *
 * \case
 *    4. Random-size strings
 *
 * \test
 *    -  xpccut_fuzz()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_10_04 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 10, 4, "Fuzz functions", _("Random-size strings")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         /*  1 */

         if (unit_test_status_next_subtest(&status, "Random character count"))
         {
            char dest[64];
            int count = 20;
            int seed = 22;
            int i;
            if (unit_test_options_show_values(options))
            {
               // nada
            }
            xpccut_set_seed(seed);
            for (i = 0; i < count; i++)
            {
               ok = xpccut_fuzz_random_size_string(dest, sizeof(dest));
               if (ok)
               {
                  if (unit_test_options_show_values(options))
                  {
                     fprintf(stdout, "  '%s'\n", dest);    // TOO SIMPLE
                  }
               }
               else
               {
                  xpccut_errprint_func(_("fuzz error"));
                  break;
               }
            }
            unit_test_status_pass(&status, ok);
         }
      }
   }
   return status;
}

/**
 *    Provides a unit test for the fuzz-related functions.
 *
 * \group
 *   10. Fuzz-testing
 *
 * \case
 *    4. xpccut_garbled_string()
 *
 * \test
 *    -  xpccut_garbled_string()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static unit_test_status_t
unit_unit_test_10_05 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 10, 5, "Fuzz functions", _("xpccut_garbled_string()")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         /*  1 */

         if (unit_test_status_next_subtest(&status, "Parameter checks"))
         {
            int r;
            char source[4] =
            {
               "hi!"
            };
            if (unit_test_options_show_values(options))
            {
               // nada
            }
            r = xpccut_garbled_string(nullptr, 1);
            ok = r < 0;
            if (ok)
               r = xpccut_garbled_string(source, 0);

            ok = r < 0;
            if (ok)
               r = xpccut_garbled_string(source, -1);

            unit_test_status_pass(&status, ok);
         }

         /*  2 */

         if (unit_test_status_next_subtest(&status, "hi!"))
         {
            unsigned int seed = 10210;
            char source[4] =
            {
               "hi!"
            };
            int trial_count = 10;
            int i;
            if (unit_test_options_show_values(options))
            {
               // nada
            }
            xpccut_set_seed(seed);
            for (i = 0; i < trial_count; i++)
            {
               int r = xpccut_garbled_string(source, strlen(source));
               ok = r >= 0;
               if (ok)
               {
                  if (unit_test_options_show_values(options))
                  {
                     fprintf
                     (
                        stdout, "  '%s', %d characters changed\n",
                        source, r
                     );
                  }
               }
               else
               {
                  xpccut_errprint_func(_("fuzz error"));
                  break;
               }
            }
            unit_test_status_pass(&status, ok);
         }

         /*  3 */

         if (unit_test_status_next_subtest(&status, "Embedded hi!"))
         {
            unsigned int seed = 10210;
            char source[32] =
            {
               "I say hi! You say ho!"
            };
            int trial_count = 10;
            int i;
            int character_change_count[10] =
            {
               1, 2, 1, 1, 2, 1, 0, 0, 1, 0
            };
            if (unit_test_options_show_values(options))
            {
               // nada
            }

            /*
             * Reset the seed.  We have some control over the random
             * sequence -- we can make it the same by setting the seed the
             * same each time.
             *
             * We can also make sure the changes are the expected ones, as
             * an added test of the randomization.
             */

            xpccut_set_seed(seed);
            for (i = 0; i < trial_count; i++)
            {
               int r = xpccut_garbled_string(&source[6], strlen("hi!"));
               ok = r >= 0;
               if (ok)
               {
                  ok = r == character_change_count[i];
                  if (unit_test_options_show_values(options))
                  {
                     fprintf
                     (
                        stdout, "  '%s', %d characters changed\n",
                        source, r
                     );
                  }
                  if (! ok)
                  {
                     xpccut_errprint_func(_("unexpected character change count"));
                     break;
                  }
               }
               else
               {
                  xpccut_errprint_func(_("fuzz error"));
                  break;
               }
            }
            unit_test_status_pass(&status, ok);
         }
      }
   }
   return status;
}

/**
 *    This is the main routine for the unit_unit_test application.
 *
 * \gnu
 *    The unit_test_initialize() function called here should set up the
 *    internationalization support for the application.
 *
 * \note
 *    Parse for any additional (non-unittest) arguments.  Don't try to find
 *    erroneous options in this loop.  If a valid option is found, then turn
 *    off the complaint flag to avoid error messages.  Note that a better
 *    way to do this work is to derive a class from unit-test and extend it
 *    to support the additional variables.  (In this case, it is likely that
 *    one would also want to extend the TestOptions class.
 *
 * \return
 *    Returns POSIX_SUCCESS (0) if the function succeeds.  Other values,
 *    including possible error-codes, are returned otherwise.
 */

#define XPCCUT_APP_NAME          "XPCCUT library"
#define XPCCUT_TEST_NAME         "unit_unit_test"
#define XPCCUT_TEST_VERSION       1.1.2
#define DEFAULT_BASE             "../test"
#define DEFAULT_AUTHOR           "Chris Ahlstrom"

int
main
(
   int argc,               /**< Number of command-line arguments.             */
   char * argv []          /**< The actual array of command-line arguments.   */
)
{

   unit_test_t testbattery;                           /* uses default values */

#ifndef XPC_NO_THISPTR

   static const char * const additional_help =

"\n"
XPCCUT_TEST_NAME "-specific options:\n"
"\n"
"--base dir          Specify root path [" DEFAULT_BASE "].\n"
"--author name       Specify an egotistical option [" DEFAULT_AUTHOR "].\n"
"--no-load           Avoid loading the tests, as a test of handling this\n"
"                    potential bug.\n"
"--leak-check        Disable some of the 'destructor' calls.  This item can\n"
"                    be used when running valgrind (as an example).\n"
"                    NOT YET IMPLEMENTED IN THIS UNIT TEST APP.\n"
"--dump-text         Dump results of test 09.03 to standard output.\n"
"--no-dump-text      Do not dump results of tests to standard output.\n"

   ;
   cbool_t ok = unit_test_initialize
   (
      &testbattery, argc, argv,
      XPCCUT_TEST_NAME,
      XPCCUT_VERSION_STRING(XPCCUT_TEST_VERSION),
      additional_help
   );

#else

   cbool_t ok = false;
   xpccut_infoprint
   (
_("Configured with XPC_NO_THISPTR defined, test will segfault, aborting!!!")
   );

#endif

   if (ok)
   {
      int argcount = argc;
      char ** arglist = argv;
      char gsBasename[80];
      char gsAuthor[80];
      cbool_t load_the_tests = true;
      cbool_t load_interactive = false;               /* for now, anywya      */
      strncpy(gsBasename, DEFAULT_BASE, sizeof(gsBasename));
      gsBasename[sizeof(gsBasename)-1] = 0;
      strncpy(gsAuthor, DEFAULT_AUTHOR, sizeof(gsAuthor));
      gsAuthor[sizeof(gsAuthor)-1] = 0;
      if (argcount > 1)
      {
         int currentarg = 1;
         while (currentarg < argcount)
         {
            const char * arg = arglist[currentarg];
            if (strcmp(arg, "--base") == 0)
            {
               currentarg++;
               if ((currentarg < argcount) && (arglist[currentarg] != 0))
                  strcpy(gsBasename, arglist[currentarg]);
            }
            else if (strcmp(arg, "--author") == 0)
            {
               currentarg++;
               if ((currentarg < argcount) && (arglist[currentarg] != 0))
                  strcpy(gsAuthor, arglist[currentarg]);
            }
            else if (strcmp(arg, "--no-load") == 0)
            {
               load_the_tests = false;
               xpccut_infoprint(_("will not load any tests"));
            }
            else if (strcmp(arg, "--interactive") == 0)
            {
               load_interactive = true;
               xpccut_infoprint(_("will allow interactive tests to load"));
            }
            else if (strcmp(arg, "--leak-check") == 0)
            {
               g_do_leak_check = true;
               xpccut_infoprint(_("memory leakage enabled"));
            }
            else if (strcmp(arg, "--dump-text") == 0)
            {
               g_do_dump_text = true;
               xpccut_infoprint(_("dumping of output text enabled"));
            }
            else if (strcmp(arg, "--no-dump-text") == 0)
            {
               g_do_dump_text = false;
               xpccut_infoprint(_("dumping of output text enabled"));
            }
            currentarg++;
         }
      }

      /*
       * Get remaining (normal/standard) options, including the errorlogging
       * options, and run the test.  We no longer make the loading of the
       * options a separate task; let the initialization function do all the
       * work and simplify the unit-test infrastructure usage even more.
       *
       * cbool_t ok = testbattery.loadOptions
       * (
       *    argc, argv,
       *    XPCCUT_TEST_NAME,
       *    XPCCUT_VERSION_STRING(XPCCUT_TEST_VERSION),
       *    additional_help.c_str()
       * );
       */

      if (ok)                           /* \note fails if --help specified */
      {
         if (load_the_tests)
         {
            /*
             * xpccut_infoprint(_("loading the tests"));
             */

            ok = unit_test_load(&testbattery, unit_unit_test_01_01);
            if (ok)
            {
               (void) unit_test_load(&testbattery, unit_unit_test_02_01);
               (void) unit_test_load(&testbattery, unit_unit_test_02_02);
               (void) unit_test_load(&testbattery, unit_unit_test_02_03);
               (void) unit_test_load(&testbattery, unit_unit_test_02_04);
               (void) unit_test_load(&testbattery, unit_unit_test_02_05);
               (void) unit_test_load(&testbattery, unit_unit_test_02_06);
               (void) unit_test_load(&testbattery, unit_unit_test_02_07);
               (void) unit_test_load(&testbattery, unit_unit_test_02_08);
               (void) unit_test_load(&testbattery, unit_unit_test_02_09);
               (void) unit_test_load(&testbattery, unit_unit_test_02_10);
               (void) unit_test_load(&testbattery, unit_unit_test_02_11);
               (void) unit_test_load(&testbattery, unit_unit_test_02_12);
               (void) unit_test_load(&testbattery, unit_unit_test_02_13);
               (void) unit_test_load(&testbattery, unit_unit_test_02_14);
               (void) unit_test_load(&testbattery, unit_unit_test_02_15);
               (void) unit_test_load(&testbattery, unit_unit_test_02_16);
               (void) unit_test_load(&testbattery, unit_unit_test_02_17);
               (void) unit_test_load(&testbattery, unit_unit_test_02_18);
               (void) unit_test_load(&testbattery, unit_unit_test_02_19);
               (void) unit_test_load(&testbattery, unit_unit_test_02_20);
               (void) unit_test_load(&testbattery, unit_unit_test_02_21);
               (void) unit_test_load(&testbattery, unit_unit_test_02_22);
               (void) unit_test_load(&testbattery, unit_unit_test_02_23);
               (void) unit_test_load(&testbattery, unit_unit_test_02_24);
               (void) unit_test_load(&testbattery, unit_unit_test_02_25);
               (void) unit_test_load(&testbattery, unit_unit_test_02_26);
               (void) unit_test_load(&testbattery, unit_unit_test_02_27);
               (void) unit_test_load(&testbattery, unit_unit_test_02_28);
               (void) unit_test_load(&testbattery, unit_unit_test_02_29);
               (void) unit_test_load(&testbattery, unit_unit_test_02_30);

               /*
                * No reason to ever expect these loads to fail at run-time,
                * but let's use the return value as an artificial way to
                * lump all tests in one group together.
                */

               ok = unit_test_load(&testbattery, unit_unit_test_02_31);
            }
            if (ok)
            {
               (void) unit_test_load(&testbattery, unit_unit_test_03_01);
               (void) unit_test_load(&testbattery, unit_unit_test_03_02);
               (void) unit_test_load(&testbattery, unit_unit_test_03_03);
               (void) unit_test_load(&testbattery, unit_unit_test_03_04);
               (void) unit_test_load(&testbattery, unit_unit_test_03_05);
               (void) unit_test_load(&testbattery, unit_unit_test_03_06);
               (void) unit_test_load(&testbattery, unit_unit_test_03_07);
               (void) unit_test_load(&testbattery, unit_unit_test_03_08);
               (void) unit_test_load(&testbattery, unit_unit_test_03_09);
               (void) unit_test_load(&testbattery, unit_unit_test_03_10);
               (void) unit_test_load(&testbattery, unit_unit_test_03_11);
               (void) unit_test_load(&testbattery, unit_unit_test_03_12);
               (void) unit_test_load(&testbattery, unit_unit_test_03_13);
               (void) unit_test_load(&testbattery, unit_unit_test_03_14);
               (void) unit_test_load(&testbattery, unit_unit_test_03_15);
               (void) unit_test_load(&testbattery, unit_unit_test_03_16);
               (void) unit_test_load(&testbattery, unit_unit_test_03_17);
               (void) unit_test_load(&testbattery, unit_unit_test_03_18);
               (void) unit_test_load(&testbattery, unit_unit_test_03_19);
               (void) unit_test_load(&testbattery, unit_unit_test_03_20);
               (void) unit_test_load(&testbattery, unit_unit_test_03_21);
               (void) unit_test_load(&testbattery, unit_unit_test_03_22);
               (void) unit_test_load(&testbattery, unit_unit_test_03_23);
               (void) unit_test_load(&testbattery, unit_unit_test_03_24);
               (void) unit_test_load(&testbattery, unit_unit_test_03_25);
               (void) unit_test_load(&testbattery, unit_unit_test_03_26);
               (void) unit_test_load(&testbattery, unit_unit_test_03_27);
               (void) unit_test_load(&testbattery, unit_unit_test_03_28);
               ok = unit_test_load(&testbattery, unit_unit_test_03_29);
            }
            if (ok)
            {
               (void) unit_test_load(&testbattery, unit_unit_test_04_01);
               (void) unit_test_load(&testbattery, unit_unit_test_04_02);
               (void) unit_test_load(&testbattery, unit_unit_test_04_03);
               (void) unit_test_load(&testbattery, unit_unit_test_04_04);
               (void) unit_test_load(&testbattery, unit_unit_test_04_05);
               (void) unit_test_load(&testbattery, unit_unit_test_04_06);
               (void) unit_test_load(&testbattery, unit_unit_test_04_07);
               (void) unit_test_load(&testbattery, unit_unit_test_04_08);
               (void) unit_test_load(&testbattery, unit_unit_test_04_09);
               (void) unit_test_load(&testbattery, unit_unit_test_04_10);
               (void) unit_test_load(&testbattery, unit_unit_test_04_11);
               (void) unit_test_load(&testbattery, unit_unit_test_04_12);
               (void) unit_test_load(&testbattery, unit_unit_test_04_13);
               (void) unit_test_load(&testbattery, unit_unit_test_04_14);
               (void) unit_test_load(&testbattery, unit_unit_test_04_15);
               (void) unit_test_load(&testbattery, unit_unit_test_04_16);
               (void) unit_test_load(&testbattery, unit_unit_test_04_17);
               (void) unit_test_load(&testbattery, unit_unit_test_04_18);
               (void) unit_test_load(&testbattery, unit_unit_test_04_19);
               (void) unit_test_load(&testbattery, unit_unit_test_04_20);
               (void) unit_test_load(&testbattery, unit_unit_test_04_21);
               (void) unit_test_load(&testbattery, unit_unit_test_04_22);
               ok = unit_test_load(&testbattery, unit_unit_test_04_23);
            }
            if (ok)
            {
               ok = unit_test_load(&testbattery, unit_unit_test_05_01);
            }
            if (ok)
            {
               /**
                * Right now, the interactive tests are not loaded be default.
                * This is because they are painful to test, and we're not so
                * concerned with them right now.
                *
                * If you really want to run them, add the --interactive option
                * to the command-line.
                */

               if (load_interactive)
               {
                  /*
                   * Note:  these tests are boring!
                   */

                  (void) unit_test_load(&testbattery, unit_unit_test_05_02);
                  (void) unit_test_load(&testbattery, unit_unit_test_05_03);
                  ok = unit_test_load(&testbattery, unit_unit_test_05_04);
               }
            }
            if (ok)
            {
               ok = unit_test_load(&testbattery, unit_unit_test_06_01);
            }
            if (ok)
            {
               (void) unit_test_load(&testbattery, unit_unit_test_07_01);
               (void) unit_test_load(&testbattery, unit_unit_test_07_02);
               ok = unit_test_load(&testbattery, unit_unit_test_07_03);
            }
            if (ok)
            {
               ok = unit_test_load(&testbattery, unit_unit_test_08_01);
            }
            if (ok)
            {
               (void) unit_test_load(&testbattery, unit_unit_test_09_01);
               (void) unit_test_load(&testbattery, unit_unit_test_09_02);
               ok = unit_test_load(&testbattery, unit_unit_test_09_03);
            }
            if (ok)
            {
               (void) unit_test_load(&testbattery, unit_unit_test_10_01);
               (void) unit_test_load(&testbattery, unit_unit_test_10_02);
               (void) unit_test_load(&testbattery, unit_unit_test_10_03);
               (void) unit_test_load(&testbattery, unit_unit_test_10_04);
               ok = unit_test_load(&testbattery, unit_unit_test_10_05);
            }
         }
         if (ok)
         {
            ok = unit_test_run(&testbattery);
            if (! ok)
            {
               /*
                * This is very specific to this particular unit-test
                * application, though you can do the same thing in your
                * application if there are some loosy-goosy parameters that
                * make the test a little variable in how it runs.
                */

               if (g_duration_out_of_range != 0)
               {

            fprintf
            (
                  stdout,
               "%s: %d\n%s.\n"
               "===============================================================\n"
               ,
               _("A duration out-of-range failure in sub-test"),
               g_duration_out_of_range,
               _("Re-run the test, or extend the limits if it fails too often")
            );

               }
            }
         }
         else
            xpccut_errprint(_("test function load failed"));
      }
   }

#ifndef XPC_NO_THISPTR
   unit_test_destroy(&testbattery);
#endif

   return ok ? EXIT_SUCCESS : EXIT_FAILURE ;
}

/*
 * unit_test_test.c
 *
 * vim: ts=3 sw=3 et ft=c
 */
