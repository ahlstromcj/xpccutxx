#ifndef XPCCUT_UNIT_TEST_H
#define XPCCUT_UNIT_TEST_H

/**
 * \file          unit_test.h
 * \library       xpccut
 * \author        Chris Ahlstrom
 * \date          2008-03-07
 * \updates       2015-10-05
 * \version       $Revision$
 * \license       $XPCCUT_SUITE_GPL_LICENSE$
 *
 *    Provides a fairly capable unit-test framework.
 *
 *    See the C module unit_test.c for full details.  This module provides a
 *    number of macros, typedefs, and unit-test functions.
 */

#include <xpc/portable_subset.h>       /* nullptr and other options           */
#include <xpc/unit_test_options.h>     /* unit_test_options_t and functions   */
#include <xpc/unit_test_status.h>      /* unit_test_status_t and functions    */

#if XPC_HAVE_STDDEF_H
#include <stddef.h>                    /* intptr_t (for Win32)                */
#endif

#if XPC_HAVE_IO_H
#include <io.h>                        /* intptr_t (for MingW32)              */
#endif

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* very commonly needed                */
#endif

#if XPC_HAVE_STDLIB_H
#include <stdlib.h>                    /* commonly needed also                */
#endif

/**
 *    A simple macro for a creation-date string.
 */

#define XPCCUT_VERSION_DATE(x)      #x " (created " __DATE__ " " __TIME__ ")"

/**
 *    A macro to make the tests have a more consistent format without a lot
 *    of extra work for the developer.
 *
 * \usage
 *    CUT_VERSION_STRING(1.4) ----> "1.4 date time"
 */

#define XPCCUT_VERSION_STRING(x)    (char *) (XPCCUT_VERSION_DATE(x))

/**
 *    Provides a template for all unit-test functions.
 *
 *    The unit-test function accepts a pointer to a unit_test_options_t
 *    structure.  The options in this structure control the output generated
 *    by the test, and also may determine which parts of the unit-test, if
 *    any, may run.
 *
 *    The unit-test should allocate and initialize a unit_test_status_t
 *    object, preferably by calling the unit_test_status_initialize()
 *    function.
 *
 *    If the unit-test contains sub-tests, each on should be preceded by a
 *    call to unit_test_status_next_subtest(), and proceed only if that
 *    function returns 'true'.
 *
 *    Once the result is obtained, it must be registered by a call to
 *    unit_test_status_pass() with a value of 'true' if the sub-test passed,
 *    and 'false' otherwise.  [Optionally, for the latter setting, a call to
 *    unit_test_fail() can be made instead.]
 *
 *    See the function unit_unit_test_01_01() and other functions in the
 *    module unit_test_test.c in the tests subdirectory for good examples.
 *
 * \return
 *    The status of the test is returned by value.  The caller should use
 *    only the unit_test_status_t functions to affect this structure.
 */

typedef unit_test_status_t (* unit_test_func_t)
(
   const unit_test_options_t * options    /**< Global options for the tests.  */
);

/**
 *    Provides a typedef for an array of unit-test function pointers.
 *    The number of elements in this array is given by
 *    m_Allocation_Count in the unit_test_t structure.
 */

typedef unit_test_func_t * unit_test_list_t;

/**
 *    The maximum length of the test name.
 *    Much more is allocated than is normally needed.
 */

#define XPCCUT_NAMELEN              132

/**
 *    The maximum length of the test version string.
 *    Much more is allocated than is normally needed.
 */

#define XPCCUT_VERSIONLEN           132

/**
 *    The maximum length of the help text provided by the caller.
 *    A caller may provide additional help text to supplement the help
 *    already included in the unit_test_options_t structure.
 *
 *    This value is simply a sanity check.  It is highly unlikely, we hope,
 *    that a caller needs to add this much text.
 */

#define XPCCUT_HELPLEN_MAX        64000

/**
 *    The amount of test-function array space to be allocated or
 *    reallocated.
 *
 *    Every time the current number of loaded unit-test threatens to exceed
 *    the number of elements already allocated to the m_Test_Cases pointer,
 *    this pointer is reallocated with an additional XPCCUT_CASE_ALLOCATION
 *    increment of memory.
 */

#define XPCCUT_CASE_ALLOCATION       64

/**
 *    Provides a scratch pad for the unit-test application.
 */

typedef struct
{
   /**
    *    Provides command-line options for determining how the unit-tests
    *    will run.
    */

   unit_test_options_t m_App_Options;

   /**
    *    The name of the application.
    *    This item should be fairly short, but the array size is generous.
    *
    * \setter
    *    -  unit_test_initialize()
    */

   char m_Test_Application_Name[XPCCUT_NAMELEN];

   /**
    *    The version info for the application.
    *
    * \example
    *    1.4 09-12-2005 13:14:00
    *
    *    This item should be fairly short, but the array size is generous.
    *
    * \setter
    *    -  unit_test_initialize()
    *
    * \getter
    *    -  unit_test_version() [writes it to standard output]
    */

   char m_Test_Application_Version[XPCCUT_VERSIONLEN];

   /**
    *    The addition help text for the application.
    *    We don't want any restrictions on this item, and so it is
    *    dynamically allocated.
    *
    * \setter
    *    -  unit_test_initialize()
    */

   char * m_Additional_Help;

   /**
    *    Provides a count of the total number of tests loaded.
    *    Each test is represented by a function pointer in the unit-test
    *    list.
    *
    * \setter
    *    -  unit_test_load() [by counting the tests that are added]
    *    -  unit_test_cpp_load_load() [for C++ wrappers]
    *
    * \getter
    *    -  unit_test_count()
    */

   int m_Test_Count;

   /**
    *    Provides a count of the total number of sub-tests encountered while
    *    running the unit-tests.
    *
    *    It is not necessary the total number of sub-tests coded in the
    *    unit-test application.  Only if all groups, cases, and sub-tests
    *    were run or analyzed is it the total number.
    *
    * \setter
    *    -  unit_test_check_subtests()
    *
    * \getter
    *    -  unit_test_subtest_count()
    */

   int m_Subtest_Count;

   /**
    *    Holds the ordinal number of the current test.
    *    This number starts at 1, representing the first test.
    *
    * \getter
    *    -  unit_test_number()
    */

   int m_Current_Test_Number;

   /**
    *    Holds the test number of the first test that failed.
    *
    * \getter
    *    -  unit_test_first_failed_test()
    */

   int m_First_Failed_Test;

   /**
    *    Holds the test number of the first test group that failed.
    *
    * \getter
    *    -  unit_test_first_failed_group()
    */

   int m_First_Failed_Group;

   /**
    *    Holds the test number of the first test case that failed.
    *
    * \getter
    *    -  unit_test_first_failed_case()
    */

   int m_First_Failed_Case;

   /**
    *    Holds the test number of the first test subtest that failed.
    *
    * \getter
    *    -  unit_test_first_failed_subtest()
    */

   int m_First_Failed_Subtest;

   /**
    *    Provides the number of tests that failed.
    *
    * \getter
    * -  unit_test_failures()
    */

   int m_Total_Errors;

   /**
    *    Provides a count of the total number of tests that could be loaded.
    *    This item is used for reallocating buffer spaces as new tests are
    *    added.
    *
    * \setter
    *    -  unit_test_dispose_of_test() [incremented if current test failed]
    */

   int m_Allocation_Count;

   /**
    *    Provides the list of unit-test functions.
    *    This list starts out at a fixed size, and is reallocated, if
    *    needed, in units of that size:  XPCCUT_CASE_ALLOCATION.
    */

   unit_test_list_t m_Test_Cases;

   /**
    *    Provides the relative time (in microseconds) at which the test
    *    application started.
    *
    * \setter
    *    -  unit_test_clear()
    *    -  unit_test_run_init()
    *    -  unit_test_post_loop()
    */

   struct timeval m_Start_Time_us;

   /**
    *    Provides the relative time (in microseconds) at which the test
    *    application ended.
    *
    * \setter
    *    -  unit_test_clear()
    *    -  unit_test_run_init()
    *    -  unit_test_post_loop()
    */

   struct timeval m_End_Time_us;

} unit_test_t;

/*
 *    These functions are documented in the C module.
 */

EXTERN_C_DEC

extern cbool_t unit_test_init (unit_test_t * tests);
extern cbool_t unit_test_initialize
(
   unit_test_t * tests,
   int argc,
   char * argv [],
   const char * appname,
   const char * appversion,
   const char * addedhelp
);
extern void unit_test_destroy (unit_test_t * tests);
extern cbool_t unit_test_load (unit_test_t * tests, const unit_test_func_t test);
extern cbool_t unit_test_cpp_load_count (unit_test_t * tests);
extern cbool_t unit_test_dispose (unit_test_status_t * status);
extern int unit_test_count (const unit_test_t * tests);
extern int unit_test_subtest_count (const unit_test_t * tests);
extern int unit_test_number (const unit_test_t * tests);
extern int unit_test_failures (const unit_test_t * tests);
extern int unit_test_first_failed_test (const unit_test_t * tests);
extern int unit_test_first_failed_group (const unit_test_t * tests);
extern int unit_test_first_failed_case (const unit_test_t * tests);
extern int unit_test_first_failed_subtest (const unit_test_t * tests);
extern cbool_t unit_test_run (unit_test_t * tests);
extern unit_test_status_t unit_test_run_a_test
(
   unit_test_t * tests,
   const unit_test_func_t test
);
extern void unit_test_report (const unit_test_t * tests, cbool_t passed);
extern void unit_test_version (const unit_test_t * tests);

/*
 *    These functions are documented in the C module.
 */

extern int unit_test_run_init (unit_test_t * tests);
extern int unit_test_next_test (unit_test_t * tests);
extern cbool_t unit_test_dispose_of_test
(
   unit_test_t * tests,
   unit_test_status_t * status,
   cbool_t * runresult
);
extern void unit_test_post_loop (unit_test_t * tests, cbool_t testresult);
extern cbool_t unit_test_run_a_test_before (unit_test_t * tests, intptr_t test);
extern cbool_t unit_test_run_a_test_after
(
   unit_test_t * tests,
   unit_test_status_t * status
);
extern int unit_test_check_subtests
(
   unit_test_t * tests,
   const unit_test_status_t * status
);

EXTERN_C_END

#endif            /* XPCCUT_UNIT_TEST_H   */

/*
 * unit_test.h
 *
 * vim: ts=3 sw=3 et ft=c
 */
