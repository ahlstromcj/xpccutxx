#ifndef XPCCUT_UNIT_TEST_STATUS_H
#define XPCCUT_UNIT_TEST_STATUS_H

/**
 * \file          unit_test_status.h
 * \library       xpccut
 * \author        Chris Ahlstrom
 * \date          2008-03-07
 * \updates       2015-10-05
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides the status-control for a fairly capable unit-test framework.
 *
 *    See the C module unit_test_status.c and the unit_test_status_t
 *    description for full details.
 */

#include <xpc/macros_subset.h>      /* nullptr and other options              */
#include <xpc/unit_test_options.h>  /* unit_test_options_t functions, etc.    */

/**
 *    Provides the overall disposition of a unit test.
 *
 *    All unit-tests have a result (pass/fail), but this enumeration lets
 *    the unit-test handle more options.
 *
 *    A test can be continued (the normal state of affairs).  It can also be
 *    skipped, failed, deliberately quit (aborted with success) by the user,
 *    or deliberately aborted (aborted with failure).
 *
 *    This item is important for the m_Test_Disposition field, and for the
 *    unit_test_status_prompt() and unit_test_status_disposition()
 *    functions.
 *
 *    Also see the unit_test_dispose(), unit_test_dispose_of_test(), and
 *    unit_test_run() functions.
 */

typedef enum
{
   XPCCUT_DISPOSITION_CONTINUE,  /**< Keep testing (if not stop-on-error).    */
   XPCCUT_DISPOSITION_DNT,       /**< The test was skipped; did not test.     */
   XPCCUT_DISPOSITION_FAILED,    /**< The test failed completely.             */
   XPCCUT_DISPOSITION_QUITTED,   /**< The user stopped tests with success.    */
   XPCCUT_DISPOSITION_ABORTED    /**< User aborted the tests with failure.    */

} unit_test_disposition_t;

/**
 *    Provides a summary of the current status of the current test.
 *
 *    The status consists first of a pointer to a unit_test_options_t
 *    structure.  This item is passed to the unit-test function, and that
 *    function must wire it into the unit_test_status_t structure using the
 *    unit_test_status_initialize() function.
 *
 *    The status also includes:
 *
 *       -  Group name
 *       -  Group number
 *       -  Case description
 *       -  Case number
 *       -  Subtest number for current sub-test
 *       -  The boolean test result
 *       -  The number of sub-tests found to be in error
 *       -  The first sub-test that failed (for easy location of the error)
 *       -  The disposition of the test (where more complex than simply
 *          pass/fail)
 *       -  The start-time (in computer time) of the test
 *       -  The end-time (in computer time) of the test
 *       -  The calculated duration of the test
 *
 *    The unit-test application can also maintain an overall
 *    unit_test_status_t structure.  This is useful in maintaining the total
 *    duration of all of the tests, for example.
 */

typedef struct
{
   /**
    *    Provides the options used in the test.
    *
    *    The particular unit_test_options_t structure is provided by the
    *    unit-test apparatus, and the status structure maintains a pointer
    *    to it.
    *
    * \note
    *    This item has no accessor functions.  Unit-test applications and
    *    other client code should not use this variable directly.  (Of
    *    course, calling on of the getters of the unit_test_options_t
    *    structure is fine.
    *
    * \warning
    *    This pointer needs to be passed along to any code that wraps this
    *    unit_test_status_t object.  This pointer should not be changed for
    *    the duration of any particular unit-test function.
    */

   const unit_test_options_t * m_Test_Options;

   /**
    *    Provides the name of a group of unit tests.
    *
    *    Often, this name is the name of a function to be tested, when the
    *    function is provided with more than one unit test.
    *
    *    Tests are put into groups so that major categories of similar
    *    functionality can be grouped together.  A group can be selected
    *    on the command-line in order to run only a subset of the unit
    *    tests.
    *
    *    A group of unit-tests is implemented by a number of related
    *    unit-test functions.
    *
    *    Although the developer can use any naming convention, our
    *    convention is to include the group and case number in the name of
    *    each unit-test function.  This allows for easy searching. Here's an
    *    example of a unit test of an FFT function, group 4, case 5:
    *
\verbatim
            fft_unit_test_04_05()
\endverbatim
    *
    *    Optionally, the function name may be even more descriptive, for
    *    even easier searching:
    *
\verbatim
            fft_convolver_unit_test_04_05()
\endverbatim
    *
    *    Also see the m_Test_Group member.
    *
    * \getter
    *    -  unit_test_status_group_name()
    */

   char m_Group_Name[XPCCUT_STRLEN];

   /**
    *    Provides the brief description of a single test case.
    *
    *    Each test group contains a number of these test cases.
    *
    *    A test case is implemented by a single unit-test function.  This
    *    function may also implement a number of much smaller sub-tests, as
    *    described for the m_Subtest_Name member.
    *
    *    Although the developer can use any naming convention, our
    *    convention is to include the group and case number in the name of
    *    each unit-test function.
    *
    * \getter
    *    -  unit_test_status_case_name()
    */

   char m_Case_Description[XPCCUT_STRLEN];

   /**
    *    Provides the name of a sub-test.
    *
    *    Each unit-test may have more than one smaller test.  These smaller
    *    tests are usually just minor parameter variations of a test.  Each
    *    sub-test is normally implemented by one if-statement in a unit-test
    *    function.
    *
    *    The name of the sub-test is specified in the
    *    unit_test_status_next_subtest() function, which also returns a
    *    boolean to indicate if the sub-test should be executed.
    *
    *    Also see the m_Test_Case member.
    *
    * \getter
    *    -  unit_test_status_subtest_name()
    */

   char m_Subtest_Name[XPCCUT_STRLEN];

   /**
    *    Provides the numerical identifier of a test group.
    *
    *    This numerical identifier is the ordinal value of the test.  The
    *    values start at 1.  Zero (0) is considered an uninitialized value.
    *
    *    Also see the m_Group_Name member.
    *
    * \getter
    *    -  unit_test_status_group()
    *
    * \note
    *    Although tests are numbered by a group/case pair, they are also
    *    numbered sequentially across group and case.  Both representations
    *    are shown in the test progress output.
    */

   int m_Test_Group;

   /**
    *    Provides the numerical identifier of a test case.
    *
    *    The values start at 1.  Zero (0) is considered an uninitialized
    *    value.
    *
    *    Also see the m_Case_Description member.
    *
    * \getter
    *    -  unit_test_status_case()
    *
    * \note
    *    Although tests are numbered by a group/case pair, they are also
    *    numbered sequentially across group and case.  Both representations
    *    are shown in the test progress output.
    */

   int m_Test_Case;

   /**
    *    Provides the numerical identifier of a sub-test case.
    *
    *    The values start at 1.  Zero (0) is considered an uninitialized
    *    value.
    *
    *    If the unit_test_status_next_subtest() function is used to
    *    introduce each sub-test, then the m_Subtest field is incremented
    *    and can be seen in the test output.  This can help locate a failed
    *    sub-test more quickly.
    *
    *    Also see the m_Subtest_Name member.
    *
    * \getter
    *    -  unit_test_status_subtest()
    */

   int m_Subtest;

   /**
    *    Provides the overall result of the test or sub-test.
    *
    *    If this value is 'true', the unit-test succeeded.  All sub-tests in
    *    the unit-test case succeeded.
    *
    *    Otherwise, at least one sub-test failed.
    *
    * \setter
    *    -  unit_test_status_pass()
    *    -  unit_test_status_fail()
    *    -  unit_test_status_fail_deliberately()
    */

   cbool_t m_Test_Result;

   /**
    *    Provides the total number of errors that occurred in the current
    *    unit-test.
    *
    *    It counts the number of sub-tests that failed.
    *
    * \getter
    *    -  unit_test_status_error_count()
    *
    * \setter
    *    -  unit_test_status_passed() [returns true if this count is zero]
    */

   int m_Subtest_Error_Count;

   /**
    *    Provides the number of the first sub-test that failed.
    *
    *    The theory is that the developer will want to fix the first error
    *    first, and this value makes it easier to find.
    *
    * \getter
    *    -  unit_test_status_failed_subtest()
    */

   int m_Failed_Subtest;

   /**
    *    Provides the overall result of the test or sub-test.
    *
    *    The test may have failed, but testing should continue.  The test
    *    may have been skipped.  It may have failed.  Or, the user may have
    *    decided to quit the test with a success status, or aborted it with
    *    a failure status.
    *
    *    See the unit_test_disposition_t structure for more information.
    *
    * \getter
    *    -  unit_test_status_disposition()
    *    -  unit_test_status_is_continue()   [indirect check]
    *    -  unit_test_status_is_skipped()    [indirect check]
    *    -  unit_test_status_is_failed()     [indirect check]
    *    -  unit_test_status_is_quitted()    [indirect check]
    *    -  unit_test_status_is_aborted()    [indirect check]
    *    -  unit_test_status_is_okay()       [indirect check]
    *
    * \setter
    *    -  unit_test_status_reset()         [set to 'continue' disposition]
    *    -  unit_test_status_prompt()        [set as per user response]
    */

   unit_test_disposition_t m_Test_Disposition;

   /**
    *    Provides the relative time (in microseconds) at which the current
    *    test started.
    *
    *    This time is set at the end of the unit_test_status_initialize()
    *    function, using the XPCCUT_get_microseconds() function.  It is also
    *    set, in the same manner, by the unit_test_status_start_timer()
    *    function.
    *
    * \setter
    *    -  unit_test_status_start_timer()
    */

   struct timeval m_Start_Time_us;

   /**
    *    Provides the relative time (in microseconds) at which the current
    *    test ended.
    *
    *    This time is set to zero by the unit_test_status_start_timer()
    *    function.
    *
    *    It is then retrieved at the end of the test using the
    *    unit_test_status_time_delta() function.
    *
    * \setter
    *    -  unit_test_status_start_timer()
    *    -  unit_test_status_time_delta()
    */

   struct timeval m_End_Time_us;

   /**
    *    Provides duration of the current test, in milliseconds.
    *
    *    It is calculated from the current values of m_Start_Time_us and
    *    m_End_Time_us;
    *
    * \setter
    *    -  unit_test_status_start_timer()
    *    -  unit_test_status_time_delta()
    *
    * \getter
    *    -  unit_test_status_duration_ms()
    */

   double m_Test_Duration_ms;

   /*
    * \todo
    *
    * char * mRequirement;        // for requirements traceability //
    */

} unit_test_status_t;

/*
 *    These functions are documented in the C module.
 */

EXTERN_C_DEC

extern void unit_test_status_beep (void);
extern cbool_t unit_test_status_init (unit_test_status_t * status);
extern cbool_t unit_test_status_initialize
(
   unit_test_status_t * status,
   const unit_test_options_t * options,
   int testgroup,
   int testcase,
   const char * groupname,
   const char * casename
);
extern cbool_t unit_test_status_reset (unit_test_status_t * status);
extern cbool_t unit_test_status_pass
(
   unit_test_status_t * status,
   cbool_t flag
);
extern cbool_t unit_test_status_fail (unit_test_status_t * status);
extern cbool_t unit_test_status_fail_deliberately (unit_test_status_t * status);
extern cbool_t unit_test_status_start_timer (unit_test_status_t * status);
extern double unit_test_status_time_delta
(
   unit_test_status_t * status,
   cbool_t startreset
);
extern cbool_t unit_test_status_show_title (unit_test_status_t * status);
extern cbool_t unit_test_status_can_proceed (unit_test_status_t * status);
extern cbool_t unit_test_status_ignore (unit_test_status_t * status);
extern cbool_t unit_test_status_next_subtest
(
   unit_test_status_t * status,
   const char * tagname
);
extern cbool_t unit_test_status_prompt
(
   unit_test_status_t * status,
   const char * message
);
extern cbool_t unit_test_status_response
(
   unit_test_status_t * status,
   const char * message
);
extern const char * unit_test_status_group_name
(
   const unit_test_status_t * status
);
extern const char * unit_test_status_case_name
(
   const unit_test_status_t * status
);
extern const char * unit_test_status_subtest_name
(
   const unit_test_status_t * status
);
extern int unit_test_status_group (const unit_test_status_t * status);
extern int unit_test_status_case (const unit_test_status_t * status);
extern int unit_test_status_subtest (const unit_test_status_t * status);
extern int unit_test_status_failed_subtest (const unit_test_status_t * status);
extern int unit_test_status_error_count (const unit_test_status_t * status);
extern cbool_t unit_test_status_passed (const unit_test_status_t * status);
extern cbool_t unit_test_status_failed (const unit_test_status_t * status);
extern unit_test_disposition_t unit_test_status_disposition
(
   const unit_test_status_t * status
);
extern cbool_t unit_test_status_is_continue (const unit_test_status_t * status);
extern cbool_t unit_test_status_is_skipped (const unit_test_status_t * status);
extern cbool_t unit_test_status_is_failed (const unit_test_status_t * status);
extern cbool_t unit_test_status_is_quitted (const unit_test_status_t * status);
extern cbool_t unit_test_status_is_aborted (const unit_test_status_t * status);
extern cbool_t unit_test_status_is_okay (const unit_test_status_t * status);
extern double unit_test_status_duration_ms (const unit_test_status_t * status);
extern cbool_t unit_test_status_int_check
(
   unit_test_status_t * status,
   int expected_value,
   int actual_value
);
extern cbool_t unit_test_status_string_check
(
   unit_test_status_t * status,
   const char * expected_value,
   const char * actual_value
);
extern cbool_t unit_test_status_bool_check
(
   unit_test_status_t * status,
   cbool_t expected_value,
   cbool_t actual_value
);
extern cbool_t unit_test_status_show (const unit_test_status_t * status);
extern cbool_t unit_test_status_trace
(
   const unit_test_status_t * status,
   const char * context
);

EXTERN_C_END

/*
 *    These functions are documented in the C module.  They are provided <i>
 *    solely </i> for usage in the xpccut library's own unit-test application.
 *    Please do <i> not </i> use these functions in your own unit-test
 *    applications.
 */

EXTERN_C_DEC

extern cbool_t status_self_test_error_count_decrement
(
   unit_test_status_t * status
);
extern cbool_t status_self_test_failed_subtest_set
(
   unit_test_status_t * status,
   int value
);

EXTERN_C_END

#endif         /* XPCCUT_UNIT_TEST_STATUS_H */

/*
 * unit_test_status.h
 *
 * vim: ts=3 sw=3 et ft=c
 */
