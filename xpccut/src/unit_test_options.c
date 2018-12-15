/**
 * \file          unit_test_options.c
 * \library       xpccut
 * \author        Chris Ahlstrom
 * \date          2010-03-07
 * \updates       2018-11-12
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This module provides unit test functionality for all internal unit
 *    tests.
 *
 *    The classes in this module support a pretty easy-to-use and orderly
 *    sequence of unit tests, with a lot of useful run-time options.
 *
 * <b> Goals: </b> \n
 *    -# Build a test plan for unit testing (white-box testing), including
 *       decision coverage and condition coverage as appropriate.
 *    -# Build a test driver using C/C++ code and a scripting language as
 *       applicable.
 *    -# Build test cases and a unit-test application to run them.
 *    -# Build benchmarks, if time permits.
 *    -# Provide semi-automatic documentation of traceability between the
 *       tests and the functions that are tested.
 *    -# Incorporated support for test-coverage profiling using GNU gcov.
 *
 * <b> Test cases: </b> \n
 *    A test case is a function that exercises another function or a small set
 *    of functions.  A test case function returns true if the test is passed,
 *    and false if the test is failed.
 *
 *    Please see the marked "Test case:" sections below for complete details
 *    of each test.  Also note that the method is akin to that in the Halladay
 *    and Weibel reference shown below.
 *
 * <b> Test groups: </b> \n
 *    A test group is a set of test cases that test related items.  For
 *    example, a test group might be a group of all the test case functions
 *    that test the member functions of a "string" class.
 *
 * \note
 *    Functions that are not used in production code will not have a unit test
 *    in the present module.  As new functions are added, however, be sure to
 *    also add a unit test to the appropriate unit test class.
 *
 *    This small object holds the command-line options that are to apply to
 *    each and every test.
 *
 *    Here are the options, their default values, and their meanings:
 *
 * <b> Integer options: </b> \n
 *
\verbatim
     OPTION NAME           DEFAULT  MIN   MAX FIELD(S) SET

   --sleep-time            0        0  3600000  m_Test_Sleep_Time
   --group                 0        0      100  m_Single_Test_Group
   --group                 empty                m_Single_Test_Group_Name
   --case                  0        0      100  m_Single_Test_Case
   --case                  empty                m_Single_Test_Case_Name
   --sub-test              0        0     1000  m_Single_Sub_Test
   --sub-test              empty                m_Single_Sub_Test_Name
\endverbatim
 *
 * <b> Boolean options: </b>
 *
\verbatim
     OPTION NAME           DEFAULT  FIELD(S) SET

   --show-progress         true     m_Show_Progress = true
   --no-show-progress       ~       m_Show_Progress = false
                                    m_Show_Step_Numbers = false
                                    m_Show_Values = false
                                    m_Is_Verbose = false
   --show-values           false    m_Show_Values = true
   --no-show-values         ~       m_Show_Values = false
   --show-step-numbers     false    m_Show_Step_Numbers = true
   --no-show-step-numbers   ~       m_Show_Step_Numbers = false
   --verbose               false    m_Is_Verbose = true
   --no-verbose            false    m_Is_Verbose = false
   --summary               false    m_Summarize = true
                                    m_Interactive = false
                                    m_Case_Pause = false
   --no-summary             ~       m_Summarize = false
   --case-pause            false    m_Case_Pause = true
   --no-case-pause          ~       m_Case_Pause = false
   --stop-on-error         false    m_Stop_On_Error = true
   --no-stop-on-error       ~       m_Stop_On_Error = false
   --batch-mode            false    m_Batch_Mode = true
                                    m_Show_Progress = false
                                    m_Show_Step_Numbers = false
                                    m_Show_Values = false
                                    m_Is_Verbose = false
                                    m_Interactive = false
   --no-batch-mode          ~       m_Batch_Mode = false
   --text-synch            false    m_Text_Synch = true
   --no-text-synch          ~       m_Text_Synch = false
   --require-sub-tests     false    m_Need_Subtests = true
   --no-require-sub-tests   ~       m_Need_Subtests = false
   --force-failure         false    m_Force_Failure = true
   --no-force-failure       ~       m_Force_Failure = false
   --response-before        0       m_Response_Before = 0
   --response-after         0       m_Response_After = 0
   --simulated             false    m_Is_Simulated = false
\endverbatim
 *
 *    In unit testing, --no-verbose and --verbose are opposites.  If the
 *    verbose mode is set, then any output tested with the
 *    unit_test_options_is_verbose() function is shown.  Being verbose is
 *    purely a decision by the programmer to show extra annotations only
 *    needed by an interested reader of the output.
 *
 *    No function in the unit-test library will test for or use the
 *    verbosity option; at least, that is the current plan.
 *
 *    However, there are other options that control output that the unit-test
 *    library does use:  --show-value, --show-progress, and more.
 *
 * \example
 *    You can show a lot of stuff if you compile for debug and use a lot of
 *    options, as in:
 *
\verbatim
         ./xdr_unit_test --show-steps --show-values --verbose
\endverbatim
 *
 * \references
 *    -# Halladay, S., and Wiebel, M. (1993) "Object-Oriented Software
 *       Engineering:  Examples in C++", R & D Publications, Lawrence, Kansas,
 *       66046.  See Chapter 7 "White-Box Testing".
 *
 */

#include <xpc/unit_test_options.h>     /* unit_test_options_t functions       */
#include <xpc/portable_subset.h>       /* xpccut "error print" functions      */
#include <ctype.h>                     /* defines the isdigit() C macro       */

#if XPC_HAVE_STDDEF_H
#include <stddef.h>                    /* size_t (for Win32)                  */
#endif

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* fprintf() and stdout                */
#endif

/**
 *    Provides a default initializer for a unit_test_options_t structure.
 *    The settings are given by the following variables:
 *
 *       -  XPCCUT_IS_VERBOSE
 *       -  XPCCUT_SHOW_VALUES
 *       -  XPCCUT_TEXT_SYNCH
 *       -  XPCCUT_SHOW_STEP_NUMBERS
 *       -  XPCCUT_SHOW_PROGRESS
 *       -  XPCCUT_STOP_ON_ERROR
 *       -  XPCCUT_BATCH_MODE
 *       -  XPCCUT_INTERACTIVE
 *       -  XPCCUT_BEEP
 *       -  XPCCUT_SUMMARIZE
 *       -  XPCCUT_CASE_PAUSE
 *       -  XPCCUT_NO_SINGLE_GROUP
 *       -  XPCCUT_NO_SINGLE_CASE
 *       -  XPCCUT_NO_SINGLE_SUB_TEST
 *       -  XPCCUT_TEST_SLEEP_TIME
 *
 * \note
 *    It is recommended that calling this function always be the first
 *    operation performed on a unit_test_options_t structure.
 *
 * \return
 *    Returns 'true' if the \a options parameter is valid.  The
 *    xpccut_thisptr() macro is used for testing this parameter.
 *
 * \unittests
 *    -  unit_unit_test_03_01()
 *
 * \todo
 *    -  Update the unit test for the 3 new string fields.
 */

cbool_t
unit_test_options_init
(
   unit_test_options_t * options    /**< The "this" pointer for the function. */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Is_Verbose                  = XPCCUT_IS_VERBOSE;
      options->m_Show_Values                 = XPCCUT_SHOW_VALUES;
      options->m_Text_Synch                  = XPCCUT_TEXT_SYNCH;
      options->m_Show_Step_Numbers           = XPCCUT_SHOW_STEP_NUMBERS;
      options->m_Show_Progress               = XPCCUT_SHOW_PROGRESS;
      options->m_Stop_On_Error               = XPCCUT_STOP_ON_ERROR;
      options->m_Batch_Mode                  = XPCCUT_BATCH_MODE;
      options->m_Interactive                 = XPCCUT_INTERACTIVE;
      options->m_Beep_Prompt                 = XPCCUT_BEEP;
      options->m_Summarize                   = XPCCUT_SUMMARIZE;
      options->m_Need_Subtests               = XPCCUT_NEED_SUBTESTS;
      options->m_Force_Failure               = XPCCUT_FORCE_FAILURE;
      options->m_Case_Pause                  = XPCCUT_CASE_PAUSE;
      options->m_Single_Test_Group           = XPCCUT_NO_SINGLE_GROUP;
      options->m_Single_Test_Group_Name[0]   = 0;
      options->m_Single_Test_Case            = XPCCUT_NO_SINGLE_CASE;
      options->m_Single_Test_Case_Name[0]    = 0;
      options->m_Single_Sub_Test             = XPCCUT_NO_SINGLE_SUB_TEST;
      options->m_Single_Sub_Test_Name[0]     = 0;
      options->m_Test_Sleep_Time             = XPCCUT_TEST_SLEEP_TIME;
      options->m_Current_Test_Number         = XPCCUT_NO_CURRENT_TEST;
      options->m_Response_Before             = 0;
      options->m_Response_After              = 0;
      options->m_Is_Simulated                = false;
   }
   return result;
}

/**
 *    The members are set to the values that are passed in.  If no values
 *    are passed in, then every member is set to its default.
 *
 *    Note that some of the members are currently set by the global options
 *    of the errorlogging facility.  It may be a little bit confusing, as
 *    those options usually apply to stderr, while here they will also be
 *    used for stdout.
 *
 * \warning
 *    -  This function does not support the three new string options for
 *       specifying the single group, case, or sub-test to run.
 *    -  We're not yet sure we want to add more parameters here.  It is
 *       better to use the unit_test_options_parse() function.  Maybe we
 *       should remove this function, or at least deprecate it.
 *    -  If we set the \a showprogress parameter to 'true' here, this has
 *       \e different side-effects than calling the
 *       unit_test_options_show_progress_set() function with a \a flag
 *       parameter of 'false'.  In that case, in addition to setting the
 *       m_Show_Progress flag to 'false', the m_Show_Step_Numbers,
 *       m_Show_Values, and m_Is_Verbose flags are also set to 'false'.
 *       Tricky code.
 *    -  Because of these tricky aspects, we do not recommend using this
 *       function.
 *
 * \deprecated
 *    We've decided to discourage the usage of this function, as it is
 *    problematic:  it does not supply enough parameters to initialize all
 *    the structure fields/members, and it initializes them differently
 *    than calling the setter functions separately.
 *
 * \return
 *    Returns 'true' if the \a options parameter is valid.
 *
 * \unittests
 *    -  unit_unit_test_03_02()
 */

cbool_t
unit_test_options_initialize
(
   unit_test_options_t * options,   /**< The "this" pointer for the function. */
   cbool_t verbose,                 /**< Show commentary information.         */
   cbool_t showvalues,              /**< Show the values of data items.       */
   cbool_t usetextsynch,            /**< Synchronize text output in threads.  */
   cbool_t showstepnumbers,         /**< Label each step as it is performed.  */
   cbool_t showprogress,            /**< Show progress information in test.   */
   cbool_t stoponerror,             /**< Stop immediately after first error.  */
   cbool_t batchmode,               /**< (NOT YET sure what this does.)       */
   cbool_t interactive,             /**< Allow user prompts to occurs.        */
   cbool_t beeps,                   /**< Beep when user input is needed.      */
   cbool_t summarize,               /**< Show the summary at the end of test. */
   cbool_t casepause,               /**< Pause between each test case.        */
   int singlegroup,                 /**< Execute only one test group.         */
   int singlecase,                  /**< Execute only one test case.          */
   int singlesubtest,               /**< Execute only one sub-test.           */
   int testsleeptime                /**< Sleep-time (ms) between the tests.   */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Is_Verbose         = verbose;
      options->m_Show_Values        = showvalues;
      options->m_Text_Synch         = usetextsynch;
      options->m_Show_Step_Numbers  = showstepnumbers;
      options->m_Show_Progress      = showprogress;
      options->m_Stop_On_Error      = stoponerror;
      options->m_Batch_Mode         = batchmode;
      options->m_Interactive        = interactive;
      options->m_Beep_Prompt        = beeps;
      options->m_Summarize          = summarize;
      options->m_Case_Pause         = casepause;
      options->m_Single_Test_Group  = singlegroup;
      options->m_Single_Test_Case   = singlecase;
      options->m_Single_Sub_Test    = singlesubtest;
      options->m_Test_Sleep_Time    = testsleeptime;

      /*
       * These options do not yet have function parameters for setting them.
       * See the warning in the banner section above.
       */

      options->m_Single_Test_Group_Name[0]   = 0;
      options->m_Single_Test_Case_Name[0]    = 0;
      options->m_Single_Sub_Test_Name[0]     = 0;
      options->m_Need_Subtests               = XPCCUT_NEED_SUBTESTS;
      options->m_Force_Failure               = XPCCUT_FORCE_FAILURE;
      options->m_Current_Test_Number         = XPCCUT_NO_CURRENT_TEST;
      options->m_Response_Before             = 0;
      options->m_Response_After              = 0;
      options->m_Is_Simulated                = false;
   }
   return result;
}

/**
 *    Shows a message, for error output.
 *
 * \unittests
 *    -  Not applicable for this very simple internal helper function.
 */

static void
unit_test_options_show_error
(
   const unit_test_options_t * options,   /**< A "this" pointer for options.  */
   const char * message                   /**< The message to be shown.       */
)
{
   if (xpccut_thisptr(options))
   {
      const char * output = message;
      if (cut_is_nullptr(message))
         output = _("null message pointer");
      else if (strlen(message) == 0)
         output = _("empty message");

      if (! xpccut_is_silent())                               /* --silent?   */
      {
         fprintf
         (
            stderr,
            "%s %d, %s %d\n",
            _("group"), unit_test_options_test_group(options),
            _("case"), unit_test_options_test_case(options)
         );
         xpccut_errprint_ex(output, "unit_test_options_show_error()");
      }
   }
}

/**
 *    Shows a message, for informational output.
 *    The message is shown only if the --silent option is disabled.
 *
 * \unittests
 *    -  Not applicable for this very simple internal helper function.
 */

static void
unit_test_options_show_info
(
   const unit_test_options_t * options,   /**< A "this" pointer for options.  */
   const char * message                   /**< The message to be shown.       */
)
{
   if (xpccut_thisptr(options) && cut_not_nullptr(message))
   {
      if (unit_test_options_show_progress(options))
         xpccut_infoprint(message);
   }
}

/**
 *    Shows a message and a value, for informational output.
 *    The message is shown only if the --silent option is disabled.
 *
 * \unittests
 *    -  Not applicable for this very simple internal helper function.
 */

static void
unit_test_options_show_info_value
(
   const unit_test_options_t * options,   /**< A "this" pointer for options.  */
   const char * message,                  /**< The message to be shown.       */
   int value                              /**< The value to be shown.         */
)
{
   if (xpccut_thisptr(options) && unit_test_options_show_progress(options))
   {
      if (! xpccut_is_silent() && cut_not_nullptr(message))
         fprintf(stdout, "  %s %d\n", message, value);
   }
}

/**
 *    This function loads the unit-test options specified on the
 *    command-line of the unit-test application.
 *
 *    It is a bit tricky.  The reason is that some of the options affect
 *    values that can be set by other options.  Also, this means that the
 *    order of the options can have an effect on the final disposition of
 *    the unit_test_options_t structure's field members.
 *
 *    The only current correction made for order is so that combining --help
 *    or --version and --silent always results in no output.
 *
 * \return
 *    Returns 'true' if the \a options, \a argc, and \a argv parameters are
 *    valid.  There must be at least one argument in the argument list.
 *    If there are no errors, but the --version or --help options are
 *    specified, this function returns 'false'.
 *
 * \unittests
 *    There are many many permutations of options that could conceivably be
 *    tested.  The following tests do not pretend to cover anywhere near all
 *    of them.
 *    -  unit_unit_test_03_03()
 *    -  unit_unit_test_03_04()
 *    -  unit_unit_test_03_05()
 */

cbool_t
unit_test_options_parse
(
   unit_test_options_t * options,   /**< The "this" pointer for options.      */
   int argc,                        /**< Standard main() argument count.      */
   char * argv [],                  /**< Standard main() argument list.       */
   const char * testname,           /**< Optional name of the current test.   */
   const char * version,            /**< Optional version of current test.    */
   const char * additionalhelp      /**< Optional added help for test app.    */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      result = (argc > 0) && cut_not_nullptr(argv);
      if (! result)
         xpccut_errprint_3_func(_("empty"), "argc/argv[]");
   }
   if (result && (argc > 1))
   {
      cbool_t do_help = false;
      cbool_t do_version = false;
      cbool_t do_version_short = false;
      int currentarg = 1;

      /*
       * To avoid surprise output, scan for the --silent option before
       * scanning for the other command-line options.
       */

      while (currentarg < argc)
      {
         char * arg = argv[currentarg];
         if (strcmp(arg, "--silent") == 0)
            xpccut_silence_printing();
         else if (strcmp(arg, "--hide-errors") == 0)
            xpccut_silence_printing();

         ++currentarg;
      }

      /*
       * Now scan for the rest of the command-line options.
       */

      currentarg = 1;
      while (currentarg < argc)
      {
         char * arg = argv[currentarg];
         if (strcmp(arg, "--test") == 0 || strcmp(arg, "-t") == 0)
         {
            /*
             * A no-op to help calling applications that do more than testing.
             */

            printf("--test option from the caller\n");
         }
         else if (strcmp(arg, "--verbose") == 0)
         {
            unit_test_options_is_verbose_set(options, true);
         }
         else if (strcmp(arg, "--no-verbose") == 0)
         {
            unit_test_options_is_verbose_set(options, false);
         }
         else if (strcmp(arg, "--show-progress") == 0)
         {
            unit_test_options_show_progress_set(options, true);
         }
         else if (strcmp(arg, "--no-show-progress") == 0)
         {
            unit_test_options_show_progress_set(options, false);
         }
         else if (strcmp(arg, "--no-silent") == 0)
         {
            /*
             * unit_test_options_show_progress_set(options, true);
             */

            xpccut_allow_printing();
         }
         else if (strcmp(arg, "--show-values") == 0)
         {
            unit_test_options_show_values_set(options, true);
         }
         else if (strcmp(arg, "--no-show-values") == 0)
         {
            unit_test_options_show_values_set(options, false);
         }
         else if (strcmp(arg, "--show-step-numbers") == 0)
         {
            unit_test_options_show_step_numbers_set(options, true);
         }
         else if (strcmp(arg, "--no-show-step-numbers") == 0)
         {
            unit_test_options_show_step_numbers_set(options, false);
         }
         else if
         (
            (strcmp(arg, "--stop-on-error") == 0) ||
            (strcmp(arg, "-soe") == 0)
         )
         {
            unit_test_options_stop_on_error_set(options, true);
         }
         else if (strcmp(arg, "--no-stop-on-error") == 0)
         {
            unit_test_options_stop_on_error_set(options, false);
         }
         else if ((strcmp(arg, "--batch-mode") == 0))
         {
            unit_test_options_batch_mode_set(options, true);
         }
         else if ((strcmp(arg, "--no-batch-mode") == 0))
         {
            unit_test_options_batch_mode_set(options, false);
         }
         else if (strcmp(arg, "--interactive") == 0)
         {
            unit_test_options_is_interactive_set(options, true);
         }
         else if ((strcmp(arg, "--no-interactive") == 0))
         {
            unit_test_options_is_interactive_set(options, false);
         }
         else if ((strcmp(arg, "--beeps") == 0))
         {
            unit_test_options_do_beep_set(options, true);
         }
         else if ((strcmp(arg, "--no-beeps") == 0))
         {
            unit_test_options_do_beep_set(options, false);
         }
         else if
         (
            (strcmp(arg, "--summarize") == 0) ||
            (strcmp(arg, "--summary") == 0)
         )
         {
            unit_test_options_is_summary_set(options, true);
         }
         else if
         (
            (strcmp(arg, "--no-summarize") == 0) ||
            (strcmp(arg, "--no-summary") == 0)
         )
         {
            unit_test_options_is_summary_set(options, false);
         }
         else if ((strcmp(arg, "--require-sub-tests") == 0))
         {
            unit_test_options_need_subtests_set(options, true);
         }
         else if ((strcmp(arg, "--no-require-sub-tests") == 0))
         {
            unit_test_options_need_subtests_set(options, false);
         }
         else if ((strcmp(arg, "--force-failure") == 0))
         {
            unit_test_options_force_failure_set(options, true);
            xpccut_silence_printing();
         }
         else if ((strcmp(arg, "--no-force-failure") == 0))
         {
            unit_test_options_force_failure_set(options, false);
         }
         else if (strcmp(arg, "--case-pause") == 0)
         {
            unit_test_options_is_pause_set(options, true);
         }
         else if (strcmp(arg, "--no-case-pause") == 0)
         {
            unit_test_options_is_pause_set(options, false);
         }
         else if ((strcmp(arg, "--sleep-time") == 0))
         {
            int count = 0;
            ++currentarg;
            if ((currentarg < argc) && cut_not_nullptr(argv[currentarg]))
               count = atoi(argv[currentarg]);

            result = unit_test_options_test_sleep_time_set(options, count);
         }
         else if (strcmp(arg, "--group") == 0)
         {
            int count = 0;
            ++currentarg;
            if ((currentarg < argc) && cut_not_nullptr(argv[currentarg]))
            {
               if (isdigit(argv[currentarg][0]))
               {
                  count = atoi(argv[currentarg]);
                  result = unit_test_options_test_group_set(options, count);
               }
               else
               {
                  result = unit_test_options_named_group_set
                  (
                     options, argv[currentarg]
                  );
               }
            }
            else
            {
               result = false;
               xpccut_errprint_ex(_("argument required"), "--group");
            }
         }
         else if (strcmp(arg, "--case") == 0)
         {
            int count = 0;
            ++currentarg;
            if ((currentarg < argc) && cut_not_nullptr(argv[currentarg]))
            {
               if (isdigit(argv[currentarg][0]))
               {
                  count = atoi(argv[currentarg]);
                  result = unit_test_options_test_case_set(options, count);
               }
               else
               {
                  result = unit_test_options_named_case_set
                  (
                     options, argv[currentarg]
                  );
               }
            }
            else
            {
               result = false;
               xpccut_errprint_ex(_("argument required"), "--case");
            }
         }
         else if
         (
            (strcmp(arg, "--sub-test") == 0) || (strcmp(arg, "--subtest") == 0)
         )
         {
            int count = 0;
            ++currentarg;
            if ((currentarg < argc) && cut_not_nullptr(argv[currentarg]))
            {
               if (isdigit(argv[currentarg][0]))
               {
                  count = atoi(argv[currentarg]);
                  result = unit_test_options_single_subtest_set(options, count);
               }
               else
               {
                  result = unit_test_options_named_subtest_set
                  (
                     options, argv[currentarg]
                  );
               }
            }
            else
               result = false;
         }
         else if
         (
            (strcmp(arg, "--response-before") == 0) ||
            (strcmp(arg, "--rb") == 0)
         )
         {
            ++currentarg;
            if ((currentarg < argc) && cut_not_nullptr(argv[currentarg]))
            {
               result = unit_test_options_prompt_before_set
               (
                  options, argv[currentarg][0]
               );
            }
            else
            {
               result = false;
               xpccut_errprint_ex(_("argument required"), "--response-before");
            }
         }
         else if
         (
            (strcmp(arg, "--response-after") == 0) ||
            (strcmp(arg, "--ra") == 0)
         )
         {
            ++currentarg;
            if ((currentarg < argc) && cut_not_nullptr(argv[currentarg]))
            {
               result = unit_test_options_prompt_after_set
               (
                  options, argv[currentarg][0]
               );
            }
            else
            {
               result = false;
               xpccut_errprint_ex(_("argument required"), "--response-after");
            }
         }
         else if ((strcmp(arg, "--simulated") == 0))
         {
            unit_test_options_is_simulated_set(options, true);
         }
         else if
         (
            (strcmp(arg, "--no-simulated") == 0) ||
            (strcmp(arg, "--not-simulated") == 0)
         )
         {
            unit_test_options_is_simulated_set(options, false);
         }
         else if ((strcmp(arg, "--text-synch") == 0))
         {
            unit_test_options_use_text_synch_set(options, true);
         }
         else if ((strcmp(arg, "--no-text-synch") == 0))
         {
            unit_test_options_use_text_synch_set(options, false);
         }
         else if (strcmp(arg, "--version") == 0)
         {
            do_version = true;
         }
         else if (strcmp(arg, "-v") == 0)
         {
            do_version = true;
            do_version_short = true;
         }
         else if (strcmp(arg, "--help") == 0)
         {
            do_help = true;
         }

         /**
          *    Not handled:
\verbatim
               else xpccut_errprint_ex(_("illegal option"), arg);
\endverbatim
          *    Non-unit-test options have to be allowed in this loop, since we
          *    may be passing the same command line to a number of different
          *    parsing functions.  Hence, there is no checking for illegal
          *    options at this time.
          */

         ++currentarg;
      }

      /*
       * Although these options are supposed to result in no output when
       * --silent is in force, we do want to run the routines for testing
       *  purposes, and so we let them, in the end, honor the --silent
       *  option.
       */

      if (do_version)
      {
         result = false;
         if (do_version_short)
            unit_test_options_version(nullptr);
         else
            unit_test_options_version(version);
      }
      if (do_help)
      {
         result = false;
         unit_test_options_help(testname, version, additionalhelp);
      }
   }
   return result;
}

/**
 *    This string is a formatting string which takes four mandatory substrings:
 *
 *       -  Test name
 *       -  Test version
 *       -  Test name
 *       -  Additional help
 *
 * \todo
 *    Handle translation of the help text to other languages, which requires
 *    wrapping the large amount text in a _() macro, and getting a language
 *    expert to make the translation.
 *
 * \gcc
 *    For long string constants, gcc warns that string length 'whatever' is
 *    greater than the length '4095' ISO C99 compilers are required to
 *    support.  Therefore, we break the stock help text into three pieces.
 */

static const char * const unit_test_options_gHelpText =

   "%s %s" "\n\n"
   "Unit-test options:\n"
   "\n"
   "%s [ one or more of the following options]\n"
   "\n"
   "Each option shown here is the non-default version of the option.  All\n"
   "options have an opposite setting that can be made by adding or removing\n"
   "the 'no-' prefix to the beginning of the option.\n"
   "\n"
   " --verbose             Show unit-test-application supplemental messages\n"
   "                       added by the programmer.  Additional messages\n"
   "                       internal to the unit-test infrastructure may also\n"
   "                       be shown.\n"
   " --no-verbose          Don't show any additional messages beyond progress\n"
   "                       messages.  This is the default option.\n"
   " --show-progress       Show output indicating the ongoing progress of the\n"
   "                       unit-tests.  This option is on by default.\n"
   "                       Normally, progress is shown, to reassure the user.\n"
   " --no-show-progress    Show minimal output, hopefully nothing but error\n"
   "                       messages.  With --no-show-progress in force, the\n"
   "                       only way to determine success is to test the return\n"
   "                       values of the test applications.  This option also\n"
   "                       sets --no-show-step-numbers, --no-verbose, and\n"
   "                       --no-show-values.  Thus, it is not quite opposite\n"
   "                       of --show-progress.  Note that error messages\n"
   "                       will still appear.  In tests that generate error\n"
   "                       messages as the normal result of testing, the\n"
   "                       --silent option is required to suppress even these\n"
   "                       messages.\n"
   " --silent              Sets the --no-show-progress option and the options\n"
   "                       that it sets.  It also (or should) turn off any test\n"
   "                       test output, including error messages and result\n"
   "                       messages.  No output whatsoever should be shown.\n"
   "                       However, not all combinations of options are\n"
   "                       completely silent, at this time.\n"
   " --no-silent           Make sure error-messages and progress-showing are\n"
   "                       turned back on.  Thus, this option is not quite the\n"
   "                       the opposite of --silent.\n"
   " --hide-errors         Just turn off the showing of error messages that are\n"
   "                       presented via the xpccut_errprint() functions.  The\n"
   "                       alternative to this option is to redirect stderr to\n"
   "                       /dev/null (\"Linux\" only).  Another way is to\n"
   "                       use '--silent --show-progress' (in that order).\n"
   " --show-values         Show resultant numeric values where the programmer\n"
   "                       has used this option.  This option makes it easier\n"
   "                       to discern the cause of errors.\n"
   " --no-show-values      Do not show resultant numeric values.  This options\n"
   "                       is the default.\n"
   " --show-step-numbers   Show each step number.  Helpful in finding the\n"
   "                       cause of an error.  This option is false by default.\n"
   "                       Turn it on to see if it can help you further pin\n"
   "                       down errors.\n"
   "%s\n"
   "%s\n"
   "%s\n"
   ;

/*
 * ABOVE:  The last three string arguments are:
 *
 *         unit_test_options_gHelpText_2
 *         unit_test_options_gHelpText_3
 *         naddhelp
 */

static const char * const unit_test_options_gHelpText_2 =

   " --no-show-step-numbers Do not show each step number.\n"
   " --text-synch          Synchronize text output in multithreaded.\n"
   "                       applications [NOT YET READY].\n"
   " --no-text-synch       Do not synchronize text output.  This is the\n"
   "                       default options.\n"
   " --stop-on-error       Stop the test after the first error occurs.\n"
   " --no-stop-on-error    Do not stop the test after the first error.  This is\n"
   "                       the default.\n"
   " --batch-mode          Run in batch mode.  This means that user prompts\n"
   "                       are not shown, and 'continue'  and 'pass' responses\n"
   "                       are assumed.\n"
   "                       Also, it sets the --no-show-progress option.\n"
   " --interactive         Perform any interactive tests.  These tests are\n"
   "                       handled by the unit_test_status_prompt() or\n"
   "                       unit_test_status_response() functions.  This mode is\n"
   "                       the default.\n"
   " --no-interactive      Skip any interactive tests. This mode is different\n"
   "                       differs from batch mode, which skips no tests.\n"
   "                       For --no-interactive, tests are\n"
   "                       skipped by assume a 'skip' answer to prompts.  In\n"
   "                       other words, interactive tests are skipped.\n"
   " --case-pause          After each test case, pause, to give the user a\n"
   "                       chance to view the results before moving on.\n"
   " --beeps               If set to beep, interactive prompts alert the user\n"
   "                       with a beep.  Beeping is the default.\n"
   " --no-beeps            Do not beep for alerts and prompts.\n"
   " --sleep-time s        Sleep s milliseconds after each test case [the\n"
   "                       default value of this option is 0, which means no\n"
   "                       sleep].  The maximum value is 3600000 (one hour).\n"
   " --group g             Select one test group to run [from 1 on up].\n"
   " --case c              Select one test case to run [requires the --group \n"
   "                       option to also be specified.]\n"
   ;

static const char * const unit_test_options_gHelpText_3 =

   " --sub-test s          Select one subtest to run [requires --group and\n"
   " --subtest s           --case options to also be specified].  However, note\n"
   "                       that subtests may require the first subtests to be\n"
   "                       run.  Therefore, the first subtest will normally\n"
   "                       always run, even if s != 1.  Note the two forms of\n"
   "                       the options, because it is so easy to forget the\n"
   "                       hyphen, and thus be confused.\n"
   " --summarize           Simply list the tests.  Do not execute them. Also\n"
   " --summary             sets --silent to avoid gratuitous errors from\n"
   "                       tests that test failure scenarios.  (Note:  add the\n"
   "                       --silent option for an even more compact listing.\n"
   " --no-summarize        Turns off summarize mode.  Normally not needed.\n"
   " --no-summary          A synonym for --no-summarize.\n"
   " --require-sub-tests   Force the unit-test application to fail if a\n"
   "                       unit-test does not specify sub-tests via a call to\n"
   "                       unit_test_status_next_subtest().   This option is\n"
   "                       currently off by default; we're not sure how nice it\n"
   "                       is to require the usage of sub-tests, though we do\n"
   "                       recommend their use.\n"
   " --no-require-sub-tests  Don't require sub-tests.  Currently the default.\n"
   " --force-failure       Force all tests to fail.  Each unit-test must\n"
   "                       include explicit support for this option.  Most of\n"
   "                       the time, unit-tests will not suport this option.\n"
   "                       This option is instead intended for special cases\n"
   "                       that might be needed for complete testing.\n"
   " --no-force-failure    Do not force unit-tests to fail.  This is the\n"
   "                       default option.\n"
   " --response-before c   Use character 'c' as an automatic response.\n"
   " --ra                  Synonym for --response-before.\n"
   " --response-after c    Use character 'c' as an automatic response.\n"
   " --rb                  Synonym for --response-after.\n"
   " --simulated           The test is simulated.  This option is used in\n"
   "                       the test of the unit-test library, but the flag\n"
   "                       can be used for other purposes in your own code.\n"
   " --no-simulated        The opposite of --simulated.\n"
   " --version             Show library version information.\n"
   " --help                Show this help text.\n"
   ;

/**
 *    Provides the major help option for the unit_test_options_t structure.
 *    This function spits out some standard help.  Often, the application will
 *    provide some additional text to supplement the standard help.
 *
 *    We found that the unit test would crash when deleting the
 *    character buffer.  It happened because the appname was counted only
 *    once, but was being used twice in the sprintf() statement.  So we now
 *    multiply the length of the appname by 2 below.  We also make the
 *    padding (to allow for the null) an explicit constant.
 *
 *    This output will /e not appear if the user specified any option (such
 *    as "--silent") that turns on the global silence option in the
 *    portable_subset.c module.
 *
 * \unittests
 *    The testing performed for unit_test_options_parse() is sufficient for
 *    this simple output function.
 *    -  unit_unit_test_03_05(), sub-test 4
 */

void
unit_test_options_help
(
   const char * appname,         /**< The name of the unit-test application.  */
   const char * version,         /**< The version information of the app.     */
   const char * additionalhelp   /**< Help peculiar to this application.      */
)
{
   const size_t PADDING = 8;                                   /* null + slop */
   size_t total = strlen(unit_test_options_gHelpText);
   char * nappname = (char *) appname;
   char * nversion = (char *) version;
   char * naddhelp = (char *) additionalhelp;
   char * buffer;
   size_t appnamlength;
   size_t versionlength;
   size_t addonlength;

   if (! xpccut_is_silent())                                   /* --silent?   */
   {
      if (cut_is_nullptr(nappname) || strlen(nappname) == 0)
         nappname = " ";

      if (cut_is_nullptr(nversion) || strlen(nversion) == 0)
         nversion = " ";

      if (cut_is_nullptr(naddhelp) || strlen(naddhelp) == 0)
         naddhelp = " ";

      appnamlength  = strlen(nappname) * 2 + 1;                /* shown twice */
      versionlength = strlen(nversion);
      addonlength   = strlen(naddhelp);
      total += strlen(unit_test_options_gHelpText_2);
      total += strlen(unit_test_options_gHelpText_3);
      total += appnamlength + versionlength + addonlength;
      buffer = malloc(total + PADDING);
      if (cut_not_nullptr(buffer))
      {
         (void) sprintf
         (
            buffer, unit_test_options_gHelpText,
            nappname, nversion, nappname,
            unit_test_options_gHelpText_2,
            unit_test_options_gHelpText_3,
            naddhelp
         );
         fprintf(stdout, "%s", buffer);
         free(buffer);
      }
      else
         xpccut_errprint_3_func(_("failed"), "malloc()");
   }
}

/**
 *    Writes the XPCCUT library version information to standard output.
 *    This output will /e not appear if the user specified any option (such
 *    as "--silent") that turns on the global silence option in the
 *    portable_subset.c module.
 *
 *    If the \a version parameter is a null pointer, then the
 *    XPC_API_VERSION number, which is hard-wired to be a string such as
 *    "1.0" in the \e configure.ac script, is used instead.  It is passed to
 *    gcc on the command line, and looks like a 'double' value to the
 *    compiler.
 *
 * \unittests
 *    The testing performed for unit_test_options_parse() is sufficient for
 *    this simple output function.
 *    -  unit_unit_test_03_05(), sub-test 3
 */

void
unit_test_options_version
(
   const char * version    /**< String specifying the version information.    */
)
{
   if (! xpccut_is_silent())
   {
      if (cut_is_nullptr(version))
      {
         fprintf
         (
            stdout, "XPCCUT %s (%s)\n", XPC_VERSION, XPC_VERSION_DATE_SHORT
         );
      }
      else
      {
         fprintf(stdout, "XPCCUT %s %s\n", _("version"), version);
      }
   }
}

/**
 *    Sets the value of the m_Is_Verbose flag.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_06()
 */

cbool_t
unit_test_options_is_verbose_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Is_Verbose = f;
      if (f)
         unit_test_options_show_info(options, _("verbose enabled"));
   }
   return result;
}

/**
 *    Provides the value of m_Is_Verbose.
 *    If this flag is set, the caller can arrange for more output than
 *    normal, so that the user can better follow the progress of the test
 *    application.
 *
 * \return
 *    Returns the value of the mVerbose flag.  If the parameter is bad, then
 *    the default value, XPCCUT_IS_VERBOSE is returned.
 *
 * \unittests
 *    -  unit_unit_test_03_06()
 */

cbool_t
unit_test_options_is_verbose
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Is_Verbose : XPCCUT_IS_VERBOSE ;
   return result;
}

/**
 *    Sets the value of m_Show_Values.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_07()
 */

cbool_t
unit_test_options_show_values_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Show_Values = f;
      if (f)
         unit_test_options_show_info(options, _("show-values enabled"));
   }
   return result;
}

/**
 *    Provides the value of m_Show_Values.
 *
 * \return
 *    Returns the value of the m_Show_Values flag.
 *
 * \unittests
 *    -  unit_unit_test_03_07()
 */

cbool_t
unit_test_options_show_values
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Show_Values : XPCCUT_SHOW_VALUES ;
   return result;
}

/**
 *    Sets the value of m_Text_Synch.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_08()
 */

cbool_t
unit_test_options_use_text_synch_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Text_Synch = f;
      if (f)
         unit_test_options_show_info(options, _("synchronize enabled (TODO)"));
   }
   return result;
}

/**
 *    Provides the value of m_Text_Synch.
 *
 * \return
 *    Returns the value of the m_Text_Synch flag.
 *
 * \unittests
 *    -  unit_unit_test_03_08()
 */

cbool_t
unit_test_options_use_text_synch
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Text_Synch : XPCCUT_TEXT_SYNCH ;
   return result;
}

/**
 *    Sets the value of m_Show_Step_Numbers.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_09()
 */

cbool_t
unit_test_options_show_step_numbers_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Show_Step_Numbers = f;
      if (f)
         unit_test_options_show_info(options, _("show-step-numbers enabled"));
      else
         unit_test_options_show_info(options, _("show-step-numbers disabled"));
   }
   return result;
}

/**
 *    Provides the value of m_Show_Step_Numbers.
 *
 * \return
 *    Returns the value of the m_Show_Step_Numbers flag if the "this"
 *    pointer is valid.  Otherwise, returns the default value, given by the
 *    macro XPCCUT_SHOW_STEP_NUMBERS.
 *
 * \unittests
 *    -  unit_unit_test_03_09()
 */

cbool_t
unit_test_options_show_step_numbers
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Show_Step_Numbers : XPCCUT_SHOW_STEP_NUMBERS ;
   return result;
}

/**
 *    Sets the value of m_Show_Progress.
 *
 *    This function helps implement the --show-progress and
 *    --no-show-progress command-line options.
 *
 * \tricky
 *    If this variable gets set to 'false', then the following verbosity
 *    variables are also set to 'false':
 *
 *       -  options->m_Show_Step_Numbers
 *       -  options->m_Show_Values
 *       -  options->m_Is_Verbose
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_10()
 */

cbool_t
unit_test_options_show_progress_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Show_Progress = f;
      if (f)
      {
         unit_test_options_show_info(options, _("show-progress enabled"));
      }
      else
      {
         unit_test_options_show_info(options, _("show-progress disabled"));
         options->m_Show_Step_Numbers = false;
         options->m_Show_Values = false;
         options->m_Is_Verbose = false;
      }
   }
   return result;
}

/**
 *    Provides the value of m_Show_Progress.
 *    Also checks the global function xpccut_is_silent().  This check is
 *    made because some test situations will be working with an
 *    unmodified (that is, default) unit_test_options_t structure.
 *
 * \return
 *    Returns the value of the m_Show_Progress flag if the "this" parameter
 *    is valid.  Otherwise, returns the value of the XPCCUT_SHOW_PROGRESS
 *    macro (the default value of this option).
 *
 * \unittests
 *    -  unit_unit_test_03_10()
 */

cbool_t
unit_test_options_show_progress
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      result = options->m_Show_Progress;
      if (result)
         result = ! xpccut_is_silent();
   }
   else
      result = XPCCUT_SHOW_PROGRESS;

   return result;
}

/**
 *    Sets the value of the m_Stop_On_Error field.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_11()
 */

cbool_t
unit_test_options_stop_on_error_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Stop_On_Error = f;
      if (f)
         unit_test_options_show_info(options, _("stop-on-error enabled"));
   }
   return result;
}

/**
 *    Provides the value of the m_Stop_On_Error field.
 *
 * \return
 *    Returns the value of the m_Stop_On_Error flag if the "this" parameter
 *    is valid.  Otherwise, the default value, XPCCUT_STOP_ON_ERROR, is
 *    returned.
 *
 * \unittests
 *    -  unit_unit_test_03_11()
 */

cbool_t
unit_test_options_stop_on_error
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Stop_On_Error : XPCCUT_STOP_ON_ERROR ;
   return result;
}

/**
 *    Sets the value of the m_Batch_Mode field, and also some other fields
 *    that play a part in batch-mode.
 *
 *    If this variable gets set to false, then this function also calls
 *    unit_test_options_show_progress_set() with a 'false' parameter to turn
 *    off all output.
 *
 *    Batch mode is essentially a no-output mode, with any user
 *    interactivity handled automatically to allow interactive tests to
 *    operate automatically with a simulated user response of 'pass'.
 *
 *    Sets the following options:
 *
 *       -  --interactive
 *       -  --response-before to 'c'
 *       -  --response-after to 'p'
 *       -  --no-show-values
 *       -  --no-verbose
 *
 *    However, it cannot set --silent because a unit-test of batch-mode
 *    would then turn silence on, making the output look confusing to the
 *    user.
 *
 * \todo
 *    In the future, batch mode might instead be a signal to dump all output
 *    to a log file.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_12()
 */

cbool_t
unit_test_options_batch_mode_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Batch_Mode = f;
      if (f)
      {
         /*
          * We're in batch mode.  We don't really want to see this.
          * However, we'll still let the user have the option to specify
          * --silent or not.
          *
          * unit_test_options_show_info(options, _("batch-mode enabled"));
          */

         result = unit_test_options_is_interactive_set(options, true);

         if (result)
            result = unit_test_options_prompt_before_set(options, 'c');

         if (result)
            result = unit_test_options_prompt_after_set(options, 'p');

         if (result)
            result = unit_test_options_show_values_set(options, false);

         if (result)
            result = unit_test_options_is_verbose_set(options, false);

         /*
          * Change our minds on these three:
          *
          *    if (result)
          *       result = unit_test_options_show_progress_set(options, false);
          *
          *    if (result)
          *       xpccut_silence_printing();
          *
          *    options->m_Interactive = false;
          */
      }
   }
   return result;
}

/**
 *    Provides the value of m_Batch_Mode.
 *
 * \return
 *    Returns the value of the m_Batch_Mode flag.
 *
 * \unittests
 *    -  unit_unit_test_03_12()
 */

cbool_t
unit_test_options_batch_mode
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Batch_Mode : XPCCUT_BATCH_MODE ;
   return result;
}

/**
 *    Sets the value of the m_Interactive field.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_13()
 */

cbool_t
unit_test_options_is_interactive_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Interactive = f;
      if (! f)
         unit_test_options_show_info(options, _("interactive tests disabled"));
   }
   return result;
}

/**
 *    Provides the value of the m_Interactive field.
 *
 * \return
 *    Returns the value of the m_Interactive flag if the "this" parameter is
 *    valid.  Otherwise, the default value, XPCCUT_INTERACTIVE, is returned.
 *
 * \unittests
 *    -  unit_unit_test_03_13()
 */

cbool_t
unit_test_options_is_interactive
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Interactive : XPCCUT_INTERACTIVE ;
   return result;
}

/**
 *    Sets the value of m_Beep_Prompt.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_14()
 */

cbool_t
unit_test_options_do_beep_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Beep_Prompt = f;
      if (! f)
         unit_test_options_show_info(options, _("beeping disabled"));
   }
   return result;
}

/**
 *    Provides the value of m_Beep_Prompt.
 *
 * \return
 *    Returns the value of the m_Beep_Prompt flag.
 *
 * \unittests
 *    -  unit_unit_test_03_14()
 */

cbool_t
unit_test_options_do_beep
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Beep_Prompt : XPCCUT_BEEP ;
   return result;
}

/**
 *    Sets the value of the m_Summarize field.
 *
 * \sideeffect
 *    If setting the value to 'true', the following fields are set to
 *    'false':
 *
 *       -  m_Interactive
 *       -  m_Case_Pause
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_15()
 */

cbool_t
unit_test_options_is_summary_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Summarize = f;
      if (f)
      {
         unit_test_options_show_info(options, _("summary of tests enabled"));
         options->m_Interactive = false;
         options->m_Case_Pause = false;
      }
   }
   return result;
}

/**
 *    Provides the value of m_Summarize.
 *
 * \return
 *    Returns the value of the m_Summarize flag.
 *
 * \unittests
 *    -  unit_unit_test_03_15()
 */

cbool_t
unit_test_options_is_summary
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Summarize : XPCCUT_SUMMARIZE ;
   return result;
}

/**
 *    Sets the value of the m_Case_Pause field.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_16()
 */

cbool_t
unit_test_options_is_pause_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Case_Pause = f;
      if (f)
         unit_test_options_show_info(options, _("case-pausing enabled"));
   }
   return result;
}

/**
 *    Provides the value of the m_Case_Pause field.
 *
 * \return
 *    Returns the value of the m_Case_Pause flag.  If the "this" parameter is
 *    invalid, then the default value, XPCCUT_SUMMARIZE, is returned.
 *
 * \unittests
 *    -  unit_unit_test_03_16()
 */

cbool_t
unit_test_options_is_pause
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Case_Pause : XPCCUT_CASE_PAUSE ;
   return result;
}

/**
 *    Sets the value of m_Single_Test_Group.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_17()
 */

cbool_t
unit_test_options_test_group_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   int v                            /**< The value to use for the setting.    */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      if (v < 0)
      {
         v = -1;
         xpccut_errprint_func(_("negative value"));
      }
      else if (v > XPCCUT_TESTGROUP_MAX)     /* maximum number of test groups */
      {
         v = -1;
         xpccut_errprint_func(_("value > XPCCUT_TESTGROUP_MAX"));
      }
      if (v == -1)
      {
         /*
          * unit_test_options_show_error
          * (
          *    options, _("Bad/missing test group number")
          * );
          */

         result = false;
         options->m_Single_Test_Group = 0;
      }
      else
      {
         options->m_Single_Test_Group = v;               /* result = true */
         if (v == 0)
         {
            unit_test_options_show_info
            (
               options, _("Running all unit-test groups")
            );
         }
         else
         {
            unit_test_options_show_info_value
            (
               options, _("Running only group #"), v
            );
         }
      }
   }
   return result;
}

/**
 *    Provides the value of m_Single_Test_Group.  Also, as a sanity check, the
 *    value of the item is checked.  This checkcan help detect situations
 *    where the unit_test_status_t structure has not been initialized.  We
 *    assume that the value should be 0 (in case no single group of tests is
 *    selected), greater than 0, or less than a million.  (That last
 *    restriction might be considered an \e insane sanity check!)
 *
 * \return
 *    Returns the value of the m_Single_Test_Group flag.  If a problem
 *    occurs, then XPCCUT_NO_SINGLE_GROUP is returned.
 *
 * \unittests
 *    -  unit_unit_test_03_17()
 */

int
unit_test_options_test_group
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   int result = XPCCUT_NO_SINGLE_GROUP;
   cbool_t ok = xpccut_thisptr(options);
   if (ok)
   {
      ok = options->m_Single_Test_Group >= 0;  /* the first sanity check  */
      if (ok)
         ok = options->m_Single_Test_Group < XPCCUT_TESTGROUP_MAX; /* 1000000 */

      if (ok)
         result = options->m_Single_Test_Group;

      if (! ok)
      {
         xpccut_errprint_func
         (
            _("the crazy group number indicates that it was not initialized")
         );
      }
   }
   return result;
}

/**
 *    Sets the value of m_Single_Test_Group_Name.
 *    If the \a name parameter is a null pointer, this usage is an error.
 *    However, if the \a name parameter is valid, but the \a name is of zero
 *    length, then this is a valid way to disable the "named-group" setting.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *    If 'false' is returned, the group name is \e not modified.
 *
 * \unittests
 *    -  unit_unit_test_03_21()
 */

cbool_t
unit_test_options_named_group_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   const char * name                /**< The name of the test-group to test.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      if (cut_not_nullptr(name))
      {
         if (strlen(name) > 0)
            xpccut_stringcopy(options->m_Single_Test_Group_Name, name);
         else
            options->m_Single_Test_Group_Name[0] = 0;
      }
      else
         result = false;
   }
   return result;
}

/**
 *    Provides the value of m_Single_Test_Group_Name.
 *
 * \return
 *    Returns the value of the m_Single_Test_Group_String.  If a problem
 *    occurs, or the string is empty, then nullptr is returned.  Thus,
 *    nullptr is not necessarily an error.  It simply indicates that the
 *    group name should not be used to filter unit-test groups.
 *
 * \unittests
 *    -  unit_unit_test_03_21()
 */

const char *
unit_test_options_named_group
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   const char * result = nullptr;
   cbool_t ok = xpccut_thisptr(options);
   if (ok)
   {
      ok = strlen(options->m_Single_Test_Group_Name) > 0;
      if (ok)
         result = options->m_Single_Test_Group_Name;
   }
   return result;
}

/**
 *    Sets the value of m_Single_Test_Case.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_18()
 */

cbool_t
unit_test_options_test_case_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   int v                            /**< The value to use for the setting.    */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      if (v < 0)
      {
         v = -1;
         xpccut_errprint_func(_("negative value"));
      }
      else if (v > XPCCUT_TESTCASE_MAX)      /* maximum number of test cases  */
      {
         v = -1;
         xpccut_errprint_func(_("value > XPCCUT_TESTCASE_MAX"));
      }
      if (v == -1)
      {
         /*
          * unit_test_options_show_error
          * (
          *    options, _("Bad/missing test case number")
          * );
          */

         options->m_Single_Test_Case = 0;
         result = false;
      }
      else
      {
         options->m_Single_Test_Case = v;
         result = true;
         if (v == 0)
         {
            unit_test_options_show_info
            (
               options, _("Running all unit-test cases")
            );
         }
         else
         {
            unit_test_options_show_info_value
            (
               options, _("Running only case #"),  v
            );
         }
      }
   }
   return result;
}

/**
 *    Provides the value of m_Single_Test_Case.
 *    A sanity check is made.
 *
 * \return
 *    Returns the value of the m_Single_Test_Case flag.
 *
 * \unittests
 *    -  unit_unit_test_03_18()
 */

int
unit_test_options_test_case
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   int result = XPCCUT_NO_SINGLE_CASE;
   cbool_t ok = xpccut_thisptr(options);
   if (ok)
   {
      ok = options->m_Single_Test_Case >= 0;  /* the first sanity check  */
      if (ok)
         ok = options->m_Single_Test_Case < XPCCUT_TESTCASE_MAX;  /* 1000000  */

      if (ok)
         result = options->m_Single_Test_Case;

      if (! ok)
      {
         xpccut_errprint_func
         (
            _("the crazy case number indicates that it was not initialized")
         );
      }
   }
   return result;
}

/**
 *    Sets the value of m_Single_Test_Case_Name.
 *    If the \a name parameter is a null pointer, this usage is an error.
 *    However, if the \a name parameter is valid, but the \a name is of zero
 *    length, then this is a valid way to disable the "named-case" setting.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *    If 'false' is returned, the group name is \e not modified.
 *
 * \unittests
 *    -  unit_unit_test_03_22()
 */

cbool_t
unit_test_options_named_case_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   const char * name                /**< The name of the test-case to test.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      if (cut_not_nullptr(name))
      {
         if (strlen(name) > 0)
            xpccut_stringcopy(options->m_Single_Test_Case_Name, name);
         else
            options->m_Single_Test_Case_Name[0] = 0;
      }
      else
         result = false;
   }
   return result;
}

/**
 *    Provides the value of m_Single_Test_Case_Name.
 *
 * \return
 *    Returns the value of the m_Single_Test_Case_String.  If a problem
 *    occurs, or the string is empty, then nullptr is returned.  Thus,
 *    nullptr is not necessarily an error.  It simply indicates that the
 *    case name should not be used to filter unit-test cases.
 *
 * \unittests
 *    -  unit_unit_test_03_22()
 */

const char *
unit_test_options_named_case
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   const char * result = nullptr;
   cbool_t ok = xpccut_thisptr(options);
   if (ok)
   {
      ok = strlen(options->m_Single_Test_Case_Name) > 0;
      if (ok)
         result = options->m_Single_Test_Case_Name;
   }
   return result;
}

/**
 *    Sets the value of m_Single_Sub_Test.
 *    A sanity check is made.  A value of 0 is allowed in order to provide
 *    for unsetting the single-sub-test option.  But negative values, or
 *    insanely large values are rejected.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_19()
 */

cbool_t
unit_test_options_single_subtest_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   int subtest                      /**< The number of the sub-test to do.    */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      cbool_t crazy = false;
      int v = subtest;
      if (subtest < 0)                          /* convert invalid to zero    */
         crazy = true;
      else if (subtest > XPCCUT_SUBTEST_MAX)    /* convert insane to zero     */
         crazy = true;

      if (crazy)
      {
         unit_test_options_show_error(options, _("Bad/missing subtest number"));
         result = false;
         options->m_Single_Sub_Test = 0;
      }
      else
      {
         result = true;
         options->m_Single_Sub_Test = v;
         if (v > 0)
         {
            unit_test_options_show_info_value
            (
               options, _("Running only subtest #"), v
            );
         }
         else
         {
            if (unit_test_options_show_progress(options))
               fprintf(stdout, "  %s\n", _("No restrictions in sub-tests"));
         }
      }
   }
   return result;
}

/**
 *    Provides the value of m_Single_Sub_Test.
 *
 * \return
 *    Returns the value of the m_Single_Sub_Test flag.
 *
 * \unittests
 *    -  unit_unit_test_03_19()
 */

int
unit_test_options_single_subtest
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   int result = XPCCUT_NO_SINGLE_SUB_TEST;
   cbool_t ok = xpccut_thisptr(options);
   if (ok)
   {
      ok = options->m_Single_Sub_Test >= 0;           /* first sanity check   */
      if (ok)
         ok = options->m_Single_Sub_Test < XPCCUT_SUBTEST_MAX;    /* 1000000  */

      if (ok)
         result = options->m_Single_Sub_Test;

      if (! ok)
      {
         xpccut_errprint_func
         (
            _("the crazy sub-test number indicates that it was not initialized")
         );
      }
   }
   return result;
}

/**
 *    Sets the value of m_Single_Sub_Test_Name.
 *    If the \a name parameter is a null pointer, this usage is an error.
 *    However, if the \a name parameter is valid, but the \a name is of zero
 *    length, then this is a valid way to disable the "named-sub-test" setting.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *    If 'false' is returned, the group name is \e not modified.
 *
 * \unittests
 *    -  unit_unit_test_03_23()
 */

cbool_t
unit_test_options_named_subtest_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   const char * name                /**< The name of the test-subtest to test.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      if (cut_not_nullptr(name))
      {
         if (strlen(name) > 0)
            xpccut_stringcopy(options->m_Single_Sub_Test_Name, name);
         else
         options->m_Single_Sub_Test_Name[0] = 0;
      }
      else
         result = false;
   }
   return result;
}

/**
 *    Provides the value of m_Single_Sub_Test_Name.
 *
 * \return
 *    Returns the value of the m_Single_Test_Group_String.  If a problem
 *    occurs, or the string is empty, then nullptr is returned.  Thus,
 *    nullptr is not necessarily an error.  It simply indicates that the
 *    subtest name should not be used to filter unit-test subtests.
 *
 * \unittests
 *    -  unit_unit_test_03_23()
 */

const char *
unit_test_options_named_subtest
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   const char * result = nullptr;
   cbool_t ok = xpccut_thisptr(options);
   if (ok)
   {
      ok = strlen(options->m_Single_Sub_Test_Name) > 0;
      if (ok)
         result = options->m_Single_Sub_Test_Name;
   }
   return result;
}

/**
 *    Sets the value of m_Test_Sleep_Time.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_20()
 */

cbool_t
unit_test_options_test_sleep_time_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   int v                            /**< The value to use for the setting.    */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      cbool_t crazy = false;
      if (v < 0)
         crazy = true;
      else if (v > XPCCUT_SLEEPTIME_MAX)              /* one-hour sleep time  */
         crazy = true;

      if (crazy)
      {
         unit_test_options_show_error(options, _("Bad/missing sleep time (ms)"));
         result = false;
         options->m_Test_Sleep_Time = 0;
      }
      else
      {
         result = true;
         options->m_Test_Sleep_Time = v;
         unit_test_options_show_info_value
         (
            options, _("sleep-time (ms) between tests"), v
         );
      }
   }
   return result;
}

/**
 *    Provides the value of the m_Test_Sleep_Time field.
 *    A sanity check is provided.  The maximum sane sleep time is considered
 *    to be one hour (3.6 million milliseconds).
 *
 * \return
 *    Returns the value of the m_Test_Sleep_Time flag if the "this"
 *    parameter is valid.  Otherwise, the default value,
 *    XPCCUT_TEST_SLEEP_TIME, is returned.
 *
 * \unittests
 *    -  unit_unit_test_03_20()
 */

int
unit_test_options_test_sleep_time
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   int result = XPCCUT_TEST_SLEEP_TIME;
   cbool_t ok = xpccut_thisptr(options);
   if (ok)
   {
      ok = options->m_Test_Sleep_Time >= 0;  /* the first sanity check  */
      if (ok)
         ok = options->m_Test_Sleep_Time < XPCCUT_SLEEPTIME_MAX;

      if (ok)
         result = options->m_Test_Sleep_Time;
      else
      {
         xpccut_errprint_func
         (
            _("the crazy sleep time indicates that it was not initialized")
         );
      }
   }
   return result;
}

/**
 *    Sets the value of the m_Need_Subtests field.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_24()
 */

cbool_t
unit_test_options_need_subtests_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Need_Subtests = f;
      if (f)
         unit_test_options_show_info(options, _("requiring sub-tests enabled"));
   }
   return result;
}

/**
 *    Provides the value of m_Need_Subtests.
 *
 * \return
 *    Returns the value of the m_Need_Subtests flag.
 *
 * \unittests
 *    -  unit_unit_test_03_24()
 */

cbool_t
unit_test_options_need_subtests
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Need_Subtests : XPCCUT_NEED_SUBTESTS ;
   return result;
}

/**
 *    Sets the value of the m_Force_Failure field.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_25()
 */

cbool_t
unit_test_options_force_failure_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Force_Failure = f;
      if (f)
         unit_test_options_show_info(options, _("forcing test-failure enabled"));
   }
   return result;
}

/**
 *    Provides the value of m_Force_Failure.
 *
 * \return
 *    Returns the value of the m_Force_Failure flag.
 *
 * \unittests
 *    -  unit_unit_test_03_25()
 */

cbool_t
unit_test_options_force_failure
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Force_Failure : XPCCUT_FORCE_FAILURE ;
   return result;
}

/**
 *    Sets the value of m_Current_Test_Number.
 *
 *    This function is provided mainly to allow for clarity in testing the
 *    unit_test_status_initialize() function.
 *
 * \note
 *    Only in rare cases will a developer want to use this function in a
 *    unit-test that is not testing the xpccut library.
 *
 * \return
 *    Returns 'true' if the setting was able to be made.  Only zero or
 *    positive values are allowed.
 *
 * \unittests
 *    Some of the unit-tests use this function to provide more accurate
 *    progress output.  There is no direct test of this function.
 *    -  unit_unit_test_02_02() [indirect test]
 *    -  unit_unit_test_02_09() [indirect test]
 *    -  unit_unit_test_02_29() [indirect test]
 *    -  unit_unit_test_02_30() [indirect test]
 *    -  unit_unit_test_02_31() [indirect test]
 */

cbool_t
unit_test_options_current_test_set
(
   unit_test_options_t * options,   /**< The this-pointer for the function.   */
   int v                            /**< The value to set the test number.    */
)
{
   cbool_t result = false;
   if (xpccut_thisptr(options))
   {
      if (unit_test_options_show_progress(options))
         fprintf(stdout, "  %s\n", _("Developer is overriding test number"));

      if (v >= 0)
      {
         options->m_Current_Test_Number = v;
         result = true;
      }
      else
         xpccut_errprint_func(_("negative tests numbers are illegal"));
   }
   return result;
}

/**
 *    Provides the value of m_Current_Test_Number.
 *    This value is a copy of unit_test_t.m_Current_Test_Number.  It
 *    is made available to unit_test_status_initialize() via the
 *    unit_test_options_t structure that is passed to it.
 *
 *    The main purpose of this accessor is to allow the code called by
 *    unit_test_status_initialize() to avoid printing the test number for
 *    tests that are skipped.
 *
 * \return
 *    Returns the value of the m_Current_Test_Number value.  If there is a
 *    parameter problem, -1 (XPCCUT_INVALID_PARAMETER) is returned.
 *    The same value (but named XPCCUT_NO_CURRENT_TEST) can be returned if
 *    the test number has not been activated/incremented yet.
 *
 * \unittests
 *    -  unit_unit_test_03_26()
 */

int
unit_test_options_current_test
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   int result = XPCCUT_INVALID_PARAMETER;
   if (xpccut_thisptr(options))
      result = options->m_Current_Test_Number;

   return result;
}

/**
 *    Sets the value of m_Response_Before.
 *    This value is a copy of unit_test_t.m_Response_Before.  It
 *    is made available to unit_test_status_initialize() via the
 *    unit_test_options_t structure that is passed to it.
 *
 *    The main purpose of this accessor is to allow the responses to
 *    interactive prompts to be automated, in a primitive way.  It is
 *    primitive because all responses are given the same value.
 *    However, by running a unit-test application many times with different
 *    values of --group, --case, --response-before, and --response-after,
 *    all responses can be properly covered.
 *
 * \return
 *    Returns 'true' if the setting was able to be made.  Only valid values
 *    are allowed.  See the m_Response_Before value for valid values.
 *
 * \unittests
 *    -  unit_unit_test_03_27()
 */

cbool_t
unit_test_options_prompt_before_set
(
   unit_test_options_t * options,   /**< The this-pointer for the function.   */
   char v                           /**< Value to set the response character. */
)
{
   cbool_t result = false;
   if (xpccut_thisptr(options))
   {
      if (unit_test_options_show_progress(options))
      {
         fprintf
         (
            stdout, "  %s\n",
            _("Developer is automating the response-before")
         );
      }
      if (v >= 0)
      {
         cbool_t ok =
         (
            ((v == 'c') || (v == 'C')) ||
            ((v == 's') || (v == 'S')) ||
            ((v == 'a') || (v == 'A')) ||
            ((v == 'q') || (v == 'Q')) ||
            (v == 0)
         );
         if (ok)
         {
            options->m_Response_Before = (char) tolower(v);
            result = true;
         }
         else
            xpccut_errprint_func(_("illegal response-before character"));
      }
      else
         xpccut_errprint_func(_("negative response-character illegal"));
   }
   return result;
}

/**
 *    Provides the value of m_Response_Before.
 *
 * \return
 *    Returns the value of the m_Response_Before value.  If there is a
 *    parameter problem, then 0, which indicates no automation, is returned.
 *
 * \unittests
 *    -  unit_unit_test_03_27()
 */

char
unit_test_options_prompt_before
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   char result = 0;
   if (xpccut_thisptr(options))
      result = options->m_Response_Before;

   return result;
}

/**
 *    Sets the value of m_Response_After.
 *    This value is a copy of unit_test_t.m_Response_After.  It
 *    is made available to unit_test_status_initialize() via the
 *    unit_test_options_t structure that is passed to it.
 *
 *    The main purpose of this accessor is to allow the responses to
 *    interactive prompts to be automated, in a primitive way.  It is
 *    primitive because all responses are given the same value.
 *    However, by running a unit-test application many times with different
 *    values of --group, --case, --response-before, and --response-after,
 *    all responses can be properly covered.
 *
 * \return
 *    Returns 'true' if the setting was able to be made.  Only valid values
 *    are allowed.  See the m_Response_After value for valid values.
 *
 * \unittests
 *    -  unit_unit_test_03_28()
 */

cbool_t
unit_test_options_prompt_after_set
(
   unit_test_options_t * options,   /**< The this-pointer for the function.   */
   char v                           /**< Value to set the response character. */
)
{
   cbool_t result = false;
   if (xpccut_thisptr(options))
   {
      if (unit_test_options_show_progress(options))
      {
         fprintf
         (
            stdout, "  %s\n",
            _("Developer is automating the response-after")
         );
      }
      if (v >= 0)
      {
         cbool_t ok =
         (
            ((v == 'p') || (v == 'P')) ||
            ((v == 'f') || (v == 'F')) ||
            ((v == 'a') || (v == 'A')) ||
            ((v == 'q') || (v == 'Q')) ||
            (v == 0)
         );
         if (ok)
         {
            options->m_Response_After = (char) tolower(v);
            result = true;
         }
         else
            xpccut_errprint_func(_("illegal response-after character"));
      }
      else
         xpccut_errprint_func(_("negative response-character illegal"));
   }
   return result;
}

/**
 *    Provides the value of m_Response_After.
 *
 * \return
 *    Returns the value of the m_Response_After value.  If there is a
 *    parameter problem, then 0, which indicates no automation, is returned.
 *
 * \unittests
 *    -  unit_unit_test_03_28()
 */

char
unit_test_options_prompt_after
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   char result = 0;
   if (xpccut_thisptr(options))
      result = options->m_Response_After;

   return result;
}

/**
 *    Sets the value of the m_Is_Simulated field.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the setting succeeds.
 *
 * \unittests
 *    -  unit_unit_test_03_25()
 */

cbool_t
unit_test_options_is_simulated_set
(
   unit_test_options_t * options,   /**< A "this" pointer for this function.  */
   cbool_t f                        /**< The value of the flag to be set.     */
)
{
   cbool_t result = xpccut_thisptr(options);
   if (result)
   {
      options->m_Is_Simulated = f;
      if (f)
         unit_test_options_show_info(options, _("forcing test-failure enabled"));
   }
   return result;
}

/**
 *    Provides the value of m_Is_Simulated.
 *
 * \return
 *    Returns the value of the m_Is_Simulated flag.
 *
 * \unittests
 *    -  unit_unit_test_03_25()
 */

cbool_t
unit_test_options_is_simulated
(
   const unit_test_options_t * options /**< A this-pointer for the function.  */
)
{
   cbool_t result = xpccut_thisptr(options);
   result = result ? options->m_Is_Simulated : XPCCUT_IS_SIMULATED ;
   return result;
}

/*
 * unit_test_options.c
 *
 * vim: ts=3 sw=3 et ft=c
 */
