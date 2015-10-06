#ifndef XPCCUT_UNIT_TEST_OPTIONS_H
#define XPCCUT_UNIT_TEST_OPTIONS_H

/**
 * \file          unit_test_options.h
 * \library       xpccut
 * \author        Chris Ahlstrom
 * \date          2008-03-07
 * \updates       2015-10-05
 * \version       $Revision$
 * \license       $XPCCUT_SUITE_GPL_LICENSE$
 *
 *    Provides a C structure to encapsulate the supported options for a
 *    unit-test framework.  The unit_test_options_t structure rovides part of
 *    a fairly capable unit-test framework.  See the C module fnd the
 *    unit_test_options_t description or full details.
 */

#include <xpc/portable_subset.h>    /* nullptr and other options              */

/**
 *    Maximum number of unit-test groups, the highest group that can be
 *    selected.  The actual value is determined by the --group command-line
 *    option.
 *
 */

#define XPCCUT_TESTGROUP_MAX           100

/**
 *    Maximum number of unit-test cases, the highest case that can be
 *    selected.  The actual value is determined by the --case command-line
 *    option.
 */

#define XPCCUT_TESTCASE_MAX            100

/**
 *    Maximum number of unit-test subtests, the highest subtest that can be
 *    selected.  The actual value is determined by the --sub-test command-line
 *    option.
 */

#define XPCCUT_SUBTEST_MAX             1000

/**
 *    Maximum amount of time that a unit-test application can sleep between
 *    tests.  The actual value is determined by the --sleep-time command-line
 *    option.  It cannot be longer than one hour.
 */

#define XPCCUT_SLEEPTIME_MAX           3600000

/**
 *    Default value setting for the m_Is_Verbose ("--verbose") field.  The
 *    default value is as if the "--no-verbose" option had been specified.
 */

#define XPCCUT_IS_VERBOSE              false

/**
 *    Default value setting for the m_Show_Values ("--show-values") field.
 *    The default value is as if the "--no-show-values" option had been
 *    specified.
 */

#define XPCCUT_SHOW_VALUES             false

/**
 *    Default value setting.
 */

#define XPCCUT_TEXT_SYNCH              false

/**
 *    Default value setting of the m_Show_Step_Numbers ("--show-step-numbers")
 *    field.  The default value is as if the "--no-step-numbers" option had
 *    been specified.
 */

#define XPCCUT_SHOW_STEP_NUMBERS       false

/**
 *    Default value setting for the m_Show_Progress ("--show-progress")
 *    field.
 */

#define XPCCUT_SHOW_PROGRESS           true

/**
 *    Default value setting.
 */

#define XPCCUT_STOP_ON_ERROR           false

/**
 *    Default value setting.
 */

#define XPCCUT_BATCH_MODE              false

/**
 *    Default value setting for the m_Interactive ("--interactive")
 *    field.
 */

#define XPCCUT_INTERACTIVE             false

/**
 *    Default value setting.
 */

#define XPCCUT_BEEP                    true

/**
 *    Default value setting.
 */

#define XPCCUT_SUMMARIZE               false

/**
 *    Default value setting.
 */

#define XPCCUT_NEED_SUBTESTS           false

/**
 *    Default value setting.
 */

#define XPCCUT_FORCE_FAILURE           false

/**
 *    Default value setting.
 */

#define XPCCUT_CASE_PAUSE              false

/**
 *    Default value setting.
 */

#define XPCCUT_NO_SINGLE_GROUP         0

/**
 *    Default value setting.
 */

#define XPCCUT_NO_SINGLE_CASE          0

/**
 *    Default value setting.
 */

#define XPCCUT_NO_SINGLE_SUB_TEST      0

/**
 *    Default value setting.
 */

#define XPCCUT_TEST_SLEEP_TIME         0

/**
 *    Default value setting.
 */

#define XPCCUT_IS_SIMULATED            false

/**
 *    This value indicates that there is no test group in force.
 *    Why is it not a status value?  Why is it an options value?
 */

#define XPCCUT_INVALID_PARAMETER       (-1)

/**
 *    This value indicates that testing has not yet begun.
 *    It is a synonym for XPCCUT_INVALID_PARAMETER, so that a bad parameter
 *    pointer or a bad internal setting of a result can be tested for in the
 *    same manner.
 */

#define XPCCUT_NO_CURRENT_TEST         XPCCUT_INVALID_PARAMETER

/**
 *    This value indicates that there is no test group in force.
 *    It is a synonym for XPCCUT_INVALID_PARAMETER, so that a bad parameter
 *    pointer or a bad internal setting of a result can be tested for in the
 *    same manner.
 */

#define XPCCUT_NO_CURRENT_GROUP         XPCCUT_INVALID_PARAMETER

/**
 *    This value indicates that there is no test group in force.
 *    It is a synonym for XPCCUT_INVALID_PARAMETER, so that a bad parameter
 *    pointer or a bad internal setting of a result can be tested for in the
 *    same manner.
 */

#define XPCCUT_NO_CURRENT_CASE         XPCCUT_INVALID_PARAMETER

/**
 *    Provides a number of options that allow the unit-test to serve many
 *    purposes.
 *
 *    The unit-test can serve as a fast test application that simply outputs
 *    a single result (pass or fail), or provides output that shows the
 *    values created and used during the test, step numbers, execution
 *    summaries, prompts, and many more options.
 */

typedef struct
{
   /**
    *    Provides a way to turn on more output from the unit test.
    *    In most cases, this extra output is commentary that the unit-test
    *    writer wants to be able to provide when the user wants a lot more
    *    information about the test.
    *
    *    The caller should enclose any supplemental output in a test of the
    *    result of unit_test_options_is_verbose().
    *
    *    In some cases, the xpccut library itself supports some verbose
    *    output.
    *
    *    This value is set by the --verbose option, and unset by the
    *    --no-verbose option.  The --no-show-progess and --silent options
    *    also unset this value.
    *
    *    The default value of this option is given by the XPCCUT_IS_VERBOSE
    *    macro.
    *
    * \accessor
    *    -  unit_test_options_is_verbose_set()
    *    -  unit_test_options_is_verbose()
    */

   cbool_t m_Is_Verbose;

   /**
    *    Provides a flag for showing test values.
    *    The developer can use this flag for showing test input values,
    *    output values, or whatever.  This flag provides a separate setting
    *    of verbosity for the unit-test application.  The writer of the
    *    unit-test application can enable commentary and the showing of
    *    values separately.
    *
    *    This value is set by the --show-values option, and unset by the
    *    --no-show-values option.  The --no-show-progess option also unsets
    *    this value.
    *
    *    The default value of this option is given by the XPCCUT_SHOW_VALUES
    *    macro.
    *
    * \accessor
    *    -  unit_test_options_show_values_set()
    *    -  unit_test_options_show_values()
    */

   cbool_t m_Show_Values;

   /**
    *    Provides a flag for synchronizing text output in a multi-threaded
    *    application.
    *
    *    This value is set by the --text-synch option, and unset by the
    *    --no-text-synch option.  The default value of this option is given by
    *    the XPCCUT_TEXT_SYNCH macro.
    *
    * \accessor
    *    -  unit_test_options_text_synch_set()
    *    -  unit_test_options_text_synch()
    *
    * \todo
    *    As the text-synch option requires the usage of a mutex or critical
    *    section, this option will be implemented later.
    */

   cbool_t m_Text_Synch;

   /**
    *    Provides a flag for showing the step numbers.
    *
    *    This extra bit of verbosity is useful for seeing all of the places
    *    where a test fails.
    *
    *    This value is set by the --show-step-numbers option, and unset by the
    *    --no-show-step-numbers option.  The default value of this option is
    *    given by the XPCCUT_SHOW_STEP_NUMBERS macro.
    *
    * \accessor
    *    -  unit_test_options_show_step_numbers_set()
    *    -  unit_test_options_show_step_numbers()
    */

   cbool_t m_Show_Step_Numbers;

   /**
    *    Provides a flag for partly quieting the application.
    *
    *    If this flag is set, other flags that turn on output should be
    *    unset.
    *
    *    However, these settings are not enough to completely quiet the
    *    application, since error messages will still appear.  To completely
    *    remove all output, the --silence option must be used.  When the
    *    global option --silence is set, then the only output from the unit
    *    test application will be a single application return value.  The
    *    return value will be POSIX_SUCCESS (0) or, in case a test failed,
    *    1.
    *
    *    This value is set by the --show-progress option, and unset by the
    *    --no-show-progress option.  The default value of this option is given
    *    by the XPCCUT_SHOW_PROGRESS macro.
    *
    * \accessor
    *    -  unit_test_options_show_progress_set()
    *    -  unit_test_options_show_progress()
    */

   cbool_t m_Show_Progress;

   /**
    *    Provides a flag for stopping the test immediately after the first
    *    error.
    *
    *    This flag is most useful when first getting a new library cleaned
    *    of bugs.
    *
    *    This value is set by the --stop-on-error option, and unset by the
    *    --no-stop-on-error option.  The default value of this option is given
    *    by the XPCCUT_STOP_ON_ERROR macro.
    *
    * \accessor
    *    -  unit_test_options_stop_on_error_set()
    *    -  unit_test_options_stop_on_error()
    */

   cbool_t m_Stop_On_Error;

   /**
    *    Provides a flag for "batch mode".
    *
    *    This value is set by the --batch-mode option, and unset by the
    *    --no-batch-mode option.  The default value of this option is given by
    *    the XPCCUT_BATCH_MODE macro.
    *
    * \accessor
    *    -  unit_test_options_batch_mode_set()
    *    -  unit_test_options_batch_mode()
    *
    * \todo
    *    We still need to define and explain what "batch mode" could be; at
    *    present, it means the same as --no-show-progress and
    *    --no-interactive, but it could additionally mean to log progress to
    *    an output file in the future.
    *
    *    One thing batch-mode does is automate the responses to tests that
    *    are interactive.  Hence, it turns on the --interactive option, and
    *    then sets --response-before to 'c' (for 'continue') and
    *    --response-after to 'p' (for 'pass').
    */

   cbool_t m_Batch_Mode;

   /**
    *    Provides a flag for allowing the user to interact with the unit test.
    *    This option can be useful for trying different values, or for running
    *    applications that are necessarily interactive.
    *
    *    This value is set by the --interactive option, and unset by the
    *    --no-interactive option.  The default value of this option is given
    *    by the XPCCUT_INTERACTIVE macro.
    *
    * \accessor
    *    -  unit_test_options_is_interactive_set()
    *    -  unit_test_options_is_interactive()
    */

   cbool_t m_Interactive;

   /**
    *    Provides a flag for playing a beep when running interactively.
    *
    *    This value is set by the --beeps option, and unset by the --no-beeps
    *    option.  The default value of this option is given by the XPCCUT_BEEP
    *    macro.
    *
    * \accessor
    *    -  unit_test_options_do_beep_set()
    *    -  unit_test_options_do_beep()
    */

   cbool_t m_Beep_Prompt;

   /**
    *    Provides a flag for generating summary output about the unit-test.
    *    This option is useful for showing a list of all the unit-test groups,
    *    cases, and sub-tests, and their names.
    *
    *    This value is set by the --summarize option, and unset by the
    *    --no-summarize option.  The default value of this option is given by
    *    the XPCCUT_SUMMARIZE macro.
    *
    * \accessor
    *    -  unit_test_options_is_summary_set()
    *    -  unit_test_options_is_summary()
    */

   cbool_t m_Summarize;

   /**
    *    Provides a flag for requiring the usage of sub-tests.  This option is
    *    useful for forcing the programmer to be diligent in using
    *    unit_test_status_next_subtest(), but currently this flag is false by
    *    default.
    *
    *    This value is set by the --require-sub-tests option, and unset by the
    *    --no-require-sub-tests option.  The default value of this option is
    *    given by the XPCCUT_NEED_SUBTESTS macro.
    *
    * \accessor
    *    -  unit_test_options_need_subtests_set()
    *    -  unit_test_options_need_subtests()
    */

   cbool_t m_Need_Subtests;

   /**
    *    Provides a flag for forcing the failure of a unit test.
    *    This option is useful for special test cases where it is desired
    *    to test a failure mode explicity.
    *
    *    Note that most developers are not likely to make sure that all of
    *    their tests support this option.  This option is most useful for
    *    testing the unit-test framework itself.  However, it is worthwhile
    *    to expose this option to everyone.
    *
    *    This value is set by the --force-failure option, and unset by the
    *    --no-force-failure option.
    *    The default value of this option is given by the
    *    XPCCUT_FORCE_FAILURE macro.
    *
    * \accessor
    *    -  unit_test_options_force_failure_set()
    *    -  unit_test_options_force_failure()
    */

   cbool_t m_Force_Failure;

   /**
    *    Provides a flag for pausing between each case.  Thus, this option
    *    requires that a user be present to interact with the unit-test
    *    application.
    *
    *    This value is set by the --case-pause option, and unset by the
    *    --no-case-pause option.  The default value of this option is given by
    *    the XPCCUT_CASE_PAUSE macro.
    *
    * \accessor
    *    -  unit_test_options_is_pause_set()
    *    -  unit_test_options_is_pause()
    */

   cbool_t m_Case_Pause;

   /**
    *    Provides a flag for running only a single test group.  A "test group"
    *    is a small collection of related tests.  For example, a test group
    *    may be a collection of tests of all of the functions in a given class
    *    in a library.
    *
    *    This value is set by the --group option.  To unset this option, use
    *    an option argument value of zero ("0").  The default value of this
    *    option is given by the XPCCUT_NO_SINGLE_GROUP macro.
    *
    * \accessor
    *    -  unit_test_options_test_group_set()
    *    -  unit_test_options_test_group()
    */

   int m_Single_Test_Group;

   /**
    *    Provides a flag for running only a single test group, by name.
    *
    *    This value is set by the --group option, if the value does not start
    *    with an integer.  To unset this value, pass an empty string as its
    *    value: "".  The default value of this option is given by the empty
    *    string ("").
    *
    * \accessor
    *    -  unit_test_options_named_group_set()
    *    -  unit_test_options_named_group()
    */

   char m_Single_Test_Group_Name[XPCCUT_STRLEN];

   /**
    *    Provides a flag for running only a single test case.  A "test case"
    *    is a single unit-test.  A "test case" is equivalent to a unit-test.
    *    For example, a test case may cover all of the sub-tests used for
    *    testing a single function in a single class.
    *
    *    This value is set by the --case option.  To unset this option, use an
    *    option argument value of zero ("0").  The default value of this
    *    option is given by the XPCCUT_NO_SINGLE_CASE macro.
    *
    * \accessor
    *    -  unit_test_options_test_case_set()
    *    -  unit_test_options_test_case()
    */

   int m_Single_Test_Case;

   /**
    *    Provides a flag for running only a single test case, by name.
    *
    *    This value is set by the --case option, if the value does not start
    *    with an integer.  To unset this value, pass an empty string as its
    *    value: "".  The default value of this option is given by the empty
    *    string ("").
    *
    * \accessor
    *    -  unit_test_options_named_case_set()
    *    -  unit_test_options_named_case()
    */

   char m_Single_Test_Case_Name[XPCCUT_STRLEN];

   /**
    *    Provides a value for running only a single subtest.  A sub-test is a
    *    small test, that usually provides a small variation on parameter
    *    input.  Usually, a test case provides a number of sub-tests that may,
    *    for example, test all of the corner conditions for a function.
    *
    *    This value is set by the --sub-test option.  To unset this option,
    *    use an option argument value of zero ("0").  The default value of
    *    this option is given by the XPCCUT_NO_SINGLE_SUB_TEST macro.
    *
    * \accessor
    *    -  unit_test_options_single_subtest_set()
    *    -  unit_test_options_single_subtest()
    */

   int m_Single_Sub_Test;

   /**
    *    Provides a value for running only a single subtest, by name.
    *
    *    This value is set by the --sub-test option, if the value does not
    *    start with an integer.  To unset this value, pass an empty string as
    *    its value: "".  The default value of this option is given by the
    *    empty string ("");
    *
    * \accessor
    *    -  unit_test_options_named_subtest_set()
    *    -  unit_test_options_named_subtest()
    */

   char m_Single_Sub_Test_Name[XPCCUT_STRLEN];

   /**
    *    Provides a value for sleeping between tests.  Sometimes, the
    *    resources created for a unit test may take a little time to be
    *    released by the operating system, and the next test may have to wait
    *    for the previous test's resources to be put to bed, lest the current
    *    test fail even if it is correct.
    *
    *    This value is in milliseonds.  This value is set by the --sleep-time
    *    option.  It is unset by providing a time-value of zero ("0").  The
    *    default value of this option is given by the XPCCUT_TEST_SLEEP_TIME
    *    macro.
    *
    * \accessor
    *    -  unit_test_options_test_sleep_time_set()
    *    -  unit_test_options_test_sleep_time()
    */

   int m_Test_Sleep_Time;

   /**
    *    Holds the ordinal number of the current test.
    *
    *    This number starts at 1, representing the first test.  It is a copy
    *    of the field of the same name in unit_test_t, and is meant only for
    *    allowing unit_test_status_initialize() to be able to properly
    *    annunciate the test number in a convenient way.
    *
    * \getter
    *    -  unit_test_options_current_test()
    */

   int m_Current_Test_Number;

   /**
    *    Provides a stock (automated) answer for the static function
    *    unit_test_status_prompt_before().
    *
    *    This value, if non-zero, causes that function to not prompt the
    *    user, but instead use the given character as the response.  Note
    *    that the selected response is given for all the calls in the
    *    unit-test application.
    *
    *    The allowed values are:
    *
    *       -  0.    This is the default, and turns off automatic responses.
    *       -  'c'.  Make a response of 'continue'.
    *       -  's'.  Make a response of 'skip'.
    *       -  'a'.  Make a response of 'abort'.
    *       -  'q'.  Make a response of 'quit'.
    *
    * \accessor
    *    -  unit_test_options_prompt_before_set()
    *    -  unit_test_options_prompt_before()
    */

   char m_Response_Before;

   /**
    *    Provides a stock (automated) answer for the static function
    *    unit_test_status_prompt_after().
    *
    *    This value, if non-zero, causes that function to not prompt the
    *    user, but instead use the given character as the response.  Note
    *    that the selected response is given for all the calls in the
    *    unit-test application.
    *
    *    The allowed values are:
    *
    *       -  0.    This is the default, and turns off automatic responses.
    *       -  'p'.  Make a response of 'pass'.
    *       -  'f'.  Make a response of 'fail'.
    *       -  'a'.  Make a response of 'abort'.
    *       -  'q'.  Make a response of 'quit'.
    *
    * \accessor
    *    -  unit_test_options_prompt_after_set()
    *    -  unit_test_options_prompt_after()
    */

   char m_Response_After;

   /**
    *    Provides a special value to make simulated tests easier to
    *    recognize.
    *
    *    When testing the unit-test code itself, some output is seen that
    *    could be confusing.  This flag allows tests to flag their output
    *    with the word "simulated" to make it obvious that the test is not
    *    broken.
    *
    * \note
    *    Only tests of the unit-test library will actually need to set this
    *    value, though it may prove useful in other situations.
    *
    * \accessor
    *    -  unit_test_options_is_simulated_set()
    *    -  unit_test_options_is_simulated()
    */

   cbool_t m_Is_Simulated;

} unit_test_options_t;

/*
 *    These functions are documented in the C module.
 */

EXTERN_C_DEC

extern cbool_t unit_test_options_init (unit_test_options_t * options);

/*
 * Now hidden to discourage callers (other than the unit-test application)
 * from using it.
 *
 *    extern cbool_t unit_test_options_initialize
 *    (
 *       unit_test_options_t * options,
 *       cbool_t verbose,
 *       cbool_t showvalues,
 *       cbool_t usetextsynch,
 *       cbool_t showstepnumbers,
 *       cbool_t showprogress,
 *       cbool_t stoponerror,
 *       cbool_t batchmode,
 *       cbool_t interactive,
 *       cbool_t beeps,
 *       cbool_t summarize,
 *       cbool_t casepause,
 *       int singlegroup,
 *       int singlecase,
 *       int singlesubtest,
 *       int testsleeptime
 *    );
 */

extern cbool_t unit_test_options_parse
(
   unit_test_options_t * options,
   int argc,
   char * argv [],
   const char * testname,
   const char * version,
   const char * additionalhelp
);
extern void unit_test_options_help
(
   const char * appname,
   const char * version,
   const char * additionalhelp
);
extern void unit_test_options_version (const char * version);
extern cbool_t unit_test_options_is_verbose_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_is_verbose
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_show_values_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_show_values
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_use_text_synch_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_use_text_synch
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_show_step_numbers_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_show_step_numbers
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_show_progress_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_show_progress
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_stop_on_error_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_stop_on_error
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_batch_mode_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_batch_mode
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_is_interactive_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_is_interactive
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_do_beep_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_do_beep
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_is_summary_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_is_summary
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_need_subtests_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_need_subtests
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_force_failure_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_force_failure
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_is_pause_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_is_pause
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_test_group_set
(
   unit_test_options_t * options,
   int v
);
extern int unit_test_options_test_group
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_named_group_set
(
   unit_test_options_t * options,
   const char * v
);
extern const char * unit_test_options_named_group
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_test_case_set
(
   unit_test_options_t * options,
   int v
);
extern int unit_test_options_test_case
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_named_case_set
(
   unit_test_options_t * options,
   const char * v
);
extern const char * unit_test_options_named_case
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_single_subtest_set
(
   unit_test_options_t * options,
   int v
);
extern int unit_test_options_single_subtest
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_named_subtest_set
(
   unit_test_options_t * options,
   const char * v
);
extern const char * unit_test_options_named_subtest
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_test_sleep_time_set
(
   unit_test_options_t * options,
   int v
);
extern int unit_test_options_test_sleep_time
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_current_test_set
(
   unit_test_options_t * options,
   int v
);
extern int unit_test_options_current_test
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_prompt_before_set
(
   unit_test_options_t * options,
   char v
);
extern char unit_test_options_prompt_before
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_prompt_after_set
(
   unit_test_options_t * options,
   char v
);
extern char unit_test_options_prompt_after
(
   const unit_test_options_t * options
);
extern cbool_t unit_test_options_is_simulated_set
(
   unit_test_options_t * options,
   cbool_t f
);
extern cbool_t unit_test_options_is_simulated
(
   const unit_test_options_t * options
);

EXTERN_C_END

#endif        /* XPCCUT_UNIT_TEST_OPTIONS_H */

/*
 * unit_test_options.h
 *
 * vim: ts=3 sw=3 et ft=c
 */
