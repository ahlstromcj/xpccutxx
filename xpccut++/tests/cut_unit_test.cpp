/**
 * \file          cut_unit_test.cpp
 * \library       libxpccut++
 * \author        Chris Ahlstrom
 * \date          2008-05-30
 * \updates       2015-10-06
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a few unit tests of the cut library module.
 *    This module is a wrapper for the structures in the xpccut library, which
 *    is written in C.  This unit-test application does not need to
 *    recapitulate the multitude of tests in xpccut/tests/unit_test_test.c.
 *
 *    For example, if a function is an inline accessor, with no validation
 *    code built into it, all the test-case needs to do is make sure that
 *    the accessor returns the proper values.
 *
 *    However, it is much easier to track down problems if the unit-test
 *    groups, cases, and sub-tests of the C++ unit-test application all
 *    exactly parallel those of the C unit-test application.
 *
 *    Also, this parallellism makes it less easy to forget to do a C++
 *    unit-test.
 *
 *    So this unit-test module is a lot bigger than one would expect.
 *
 * \note
 *    It all of these unit-tests, it is important to understand that a
 *    status coming up invalid [i.e. status.valid() == false] is \e not a
 *    test failure.
 *
 *    If status.valid() returns 'false', and you want to be sure that it is
 *    not an error, you will (currently) have to check
 *    xpc::cut_status::m_Is_Valid and xpc::cut_status::m_Can_Run separately.
 *    No separate public accessors are (yet) provided.
 */

#include <stdexcept>                   /* std::logic_error                    */
#include <iostream>                    /* std::cout and std::cerr             */
#include <xpc/cut.hpp>                 /* xpc::cut unit-test class            */

/**
 *    Provides a "null" parameter for some of the tests.
 *
 *    We have to use this parameter in place of nullptr in some parts of
 *    the test code.
 *
 * \gcc
 *    The GNU C++ std::string constructor throws an std::logic_error when
 *    a null pointer is passed to it.  We can catch this exception.
 *
 * \win32
 *    The Visual Studio 2010 std::string constructor makes an assertion when
 *    a null pointer is passed to it.  We can't do anything about that,
 *    because the program aborts.
 */

#if defined _MSC_VER && defined DEBUG
#define NULL_PTR_PARAM        ""
#define RUN_TIME_ASSERTIONS   1              // true
#else
#define NULL_PTR_PARAM        nullptr
#define RUN_TIME_ASSERTIONS   0              // false
#endif

/**
 *    Provides a way to disable some "destructor" calls, in order to test
 *    leakage mechanisms.
 *
 *    This variable is normally false.  It can be set to true using the
 *    --leak-check option (which should not be confused with valgrind's
 *    option of the same name).
 *
 *    This is an element specific to this unit-test application.
 */

static bool gs_do_leak_check = false;

/**
 *    Provides a way to set the option structure's test number to a
 *    recognizably bogus value, for less confusing output.
 *
 *    When output, this value is incremented internally, and will be
 *    displayed as "1000".
 *
 *    This is an element specific to this unit-test application.
 */

static int g_BOGUS_TEST_NUMBER = 999;

/**
 *    Provides a way to dig deeper into the underlying C implementation.
 */

static bool g_use_direct_field_access = false;

/**
 *    Provides an internal helper function to show an exclamation message.
 *    The message is shown if the --verbose option is used, and the --silent
 *    option is not used.
 *
 *    This is a function specific to this unit-test application.
 */

static void
exclaim
(
   const xpc::cut_options & options,   /**< The options object to examine.    */
   const std::string & message         /**< The translated message string.    */
)
{
   if (options.is_verbose() && ! xpccut_is_silent())
      std::cout << "! " << message << std::endl;
}

/**
 *    Provides an internal helper function to show an informational message.
 *    The message is shown if the --verbose option is used, and the --silent
 *    option is not used.
 *
 *    This is a function specific to this unit-test application.
 */

static void
inform
(
   const xpc::cut_options & options,   /**< The options object to examine.    */
   const std::string & message         /**< The translated message string.    */
)
{
   if (options.is_verbose() && ! xpccut_is_silent())
      std::cout << "* " << message << std::endl;
}

/**
 *    Provides an internal helper function to show an informational message.
 *    The message is shown no matter what, unless the --silent option is
 *    used.
 *
 *    This is a function specific to this unit-test application.
 */

static void
show
(
   const xpc::cut_options & options,   /**< The options object to examine.    */
   const std::string & message         /**< The translated message string.    */
)
{
   if (options.show_values() && ! xpccut_is_silent())
      std::cout << "  " << message << std::endl;
}

/**
 *    Provides an internal helper function to show a message indicating that
 *    the failure was deliberate.
 *
 *    This output helps reassure the user that all is really well.  It
 *    appears only if the --show-progress option is in effect.
 *
 *    This is a function specific to this unit-test application.
 */

static void
show_deliberate_failure
(
   const xpc::cut_options & options    /**< The options object to check.      */
)
{
   exclaim(options, _("! This FAILURE is deliberate."));
}

namespace xpc
{

/**
 *    Provides one possible implementation of a white-box testing class for
 *    the CUT++ classes.
 *
 *    There are a number of different ways this test class can access the
 *    internal values used by the xpc::cut, xpc::cut_options, and
 *    xpc::cut_status classes.  Here are some examples for one of the
 *    unit_test_status_t fields.
 *
 *    -# Reading:
 *       -# Direct private const structure-accessor with direct field access:
 *          \n <tt>s.status().m_Test_Group == value;</tt> \n
 *       -# Direct const structure-accessor with C-function access:
 *          \n <tt>unit_test_status_test_group(&s.status()) == value;</tt> \n
 *    -# Writing:
 *       -# Direct structure access with direct field access:
 *          \n <tt>s.m_Status.m_Test_Group = value;</tt> \n
 *       -# Direct structure-accessor with C-function access:
 *          \n <tt>unit_test_status_test_group_set(&s.m_Status, value);</tt> \n
 *
 *    For reading, we can use the const function accessor.  For writing, it
 *    is easier to access the structure directly, rather casting away
 *    const-ness from the const function accessor.
 *
 *    Our first thought was to use the C-functions to do all of the
 *    accesses.  This adds an extra degree of separation between the C++
 *    code and the C structures.
 *
 *    However, if we directly access the C structures from within the C++
 *    code, we can expose changes to the C library internals.  (Maybe this
 *    is a good think for thorough testing?)
 */

class cut_white_box
{

public:

   /**
    *    \defaultctor
    */

   cut_white_box ()
   {
      /* Not one dang thing to do here. */
   }

   /*
    * unit_test_status_t access functions
    */

   /**
    *    White-box access to the unit_test_status_t.m_Test_Options field.
    *
    *    Note the direct access to the structures.  You don't do this in
    *    your non-test applications, do you?  Hmm?  Hmm?  Well, we got
    *    scared of buffer overflows, and have thought better about exposing
    *    a unit_test_status_t structure for modification through a
    *    non-constont status() function, and so we have made status() a \e
    *    const function.  So now you can't do it, unless you make a stupid
    *    move by implementing and instantiating your own cut_white_box
    *    class.
    *
    *    Anyway, there is no unit_test_status_t C setter function for the
    *    unit_test_status_t.m_Test_Options field, so we have no choice but
    *    to access it directly.
    */

   bool Status_Check_Test_Options_Pointer
   (
      const cut_status & s,      /**< The status object to check.             */
      void * options_pointer     /**< Value to check options pointer against. */
   )
   {
      bool ok = s.status().m_Test_Options == options_pointer;
      if (ok)
      {
         /*
          * If cut_status::options() were defined, we'd try this test, but this
          * function must not be implemented because we need it to return a
          * reference, but the m_Test_Options pointer can be null, and we don't
          * want to have an exception thrown.
          *
          * try
          * {
          *    xpc::cut_options & op = s.options();
          *    ok = false;          // should not reach here
          * }
          * catch (...)
          * {
          *    ok = true;          // should reach here
          * }
          */
      }
      return ok;
   }

   /**
    *    White-box access to the unit_test_status_t.m_Group_Name field.
    *
    *    The part implemented is only the white-box portion.  The
    *    corresponding black-box text [of the cut_status::group_name()
    *    function] is not included in this white-box test class.
    *
    *    This same comment applies to many of the other unit tests.
    */

   bool Status_Check_Group_Name
   (
      const cut_status & s,      /**< The status object to check.             */
      const char * name_pointer  /**< Value to check group-name against.      */
   )
   {
      std::string actual(s.status().m_Group_Name);
      std::string expected;
      if (cut_not_nullptr(name_pointer))
         expected = name_pointer;

      return expected == actual;
   }

   /**
    *    White-box write access to the unit_test_status_t.m_Group_Name
    *    field.
    *
    *    There is no C setter function for this field, so we must access the
    *    structure and field directly.
    *
    *    Note that this function does not test the parameter or the length
    *    of the string.
    */

   void Status_Set_Group_Name
   (
      cut_status & s,            /**< The status object to be set.            */
      const char * grpname       /**< The group name to wire into the status. */
   )
   {
      strcpy(&s.m_Status.m_Group_Name[0], grpname);
   }

   /**
    *    White-box access to the unit_test_status_t.m_Case_Description field.
    */

   bool Status_Check_Case_Description
   (
      const cut_status & s,      /**< The status object to check.             */
      const char * name_pointer  /**< Value to check case-name against.       */
   )
   {
      std::string actual;
      if (g_use_direct_field_access)
         actual = s.status().m_Case_Description;
      else
         actual = unit_test_status_case_name(&s.status());

      std::string expected;
      if (cut_not_nullptr(name_pointer))
         expected = name_pointer;

      bool result = expected == actual;
      if (! result)
      {
         xpccut_infoprint_ex(expected.c_str(), "expected");
         xpccut_infoprint_ex(actual.c_str(), "actual");
      }
      return result;
   }

   /**
    *    White-box write access to the unit_test_status_t.m_Case_Description
    *    field.  Don't you do this junk!
    */

   void Status_Set_Case_Description
   (
      cut_status & s,            /**< The status object to be set.            */
      const char * casename      /**< The case name to wire into the status.  */
   )
   {
      size_t casesize = sizeof(s.m_Status.m_Case_Description);
      if (g_use_direct_field_access)
      {
         strncpy(&s.m_Status.m_Case_Description[0], casename, casesize);
         s.m_Status.m_Case_Description[casesize-1] = 0;
      }
      else
      {
         /*
          * There is no setter for this value, so we cast away const-ness
          * from the return value of the C getter function.  This is
          * cheating.
          */

         char * cn = (char *) unit_test_status_case_name(&s.m_Status);
         strncpy(cn, casename, casesize);
      }
   }

   /**
    *    White-box access to the unit_test_status_t.m_Subtest_Name field.
    */

   bool Status_Check_Subtest_Name
   (
      const cut_status & s,      /**< The status object to check.             */
      const char * name_pointer  /**< Value to check case-name against.       */
   )
   {
      std::string actual;
      if (g_use_direct_field_access)
         actual = s.status().m_Subtest_Name;
      else
         actual = unit_test_status_subtest_name(&s.status());

      std::string expected;
      if (cut_not_nullptr(name_pointer))
         expected = name_pointer;

      return expected == actual;
   }

   /**
    *    White-box write access to the unit_test_status_t.m_Subtest_Name
    *    field.
    */

   void Status_Set_Subtest_Name
   (
      cut_status & s,            /**< The status object to be set.            */
      const char * testname      /**< Sub-test name to wire into the status.  */
   )
   {
      if (g_use_direct_field_access)
         strcpy(&s.m_Status.m_Subtest_Name[0], testname);
      else
      {
         /*
          * There is no setter for this value, so we cast away const-ness from
          * the return value of the C getter function.
          */

         strcpy((char *) unit_test_status_subtest_name(&s.m_Status), testname);
      }
   }

   /**
    *    White-box read access to the unit_test_status_t.m_Test_Group field.
    */

   bool Status_Check_Test_Group
   (
      const cut_status & s,      /**< The status object to check.             */
      int value                  /**< Value to check test-group against.      */
   )
   {
      if (g_use_direct_field_access)
         return s.status().m_Test_Group == value;
      else
         return unit_test_status_group(&s.status()) == value;

   }

   /**
    *    White-box read access to the unit_test_status_t.m_Test_Group field.
    *
    *    The only way to access this field is directly; there is no C setter
    *    function for it.
    */

   void Status_Set_Test_Group
   (
      const cut_status & s,      /**< The status object to check.             */
      int value                  /**< Value to check test-group against.      */
   )
   {
      s.m_Status.m_Test_Group = value;
   }

   /**
    *    White-box read access to the unit_test_status_t.m_Test_Case field.
    */

   bool Status_Check_Test_Case
   (
      const cut_status & s,      /**< The status object to check.             */
      int value                  /**< Value to check test-case against.       */
   )
   {
      if (g_use_direct_field_access)
         return s.status().m_Test_Case == value;
      else
         return unit_test_status_case(&s.status()) == value;
   }

   /**
    *    White-box write access to the unit_test_status_t.m_Test_Group field.
    *
    *    The only way to access this field is directly; there is no C setter
    *    function for it.
    */

   void Status_Set_Test_Case
   (
      const cut_status & s,      /**< The status object to check.             */
      int value                  /**< Value to check test-case against.       */
   )
   {
      s.m_Status.m_Test_Case = value;
   }

   /**
    *    White-box read access to the unit_test_status_t.m_Subtest field.
    */

   bool Status_Check_Subtest
   (
      const cut_status & s,      /**< The status object to check.             */
      int value                  /**< Value to check sub-test against.        */
   )
   {
      if (g_use_direct_field_access)
         return s.status().m_Subtest == value;
      else
         return unit_test_status_subtest(&s.status()) == value;
   }

   /**
    *    White-box write access to the unit_test_status_t.m_Subtest field.
    *
    *    There is no C setter for this field, so only direct-access is
    *    provided.
    */

   void Status_Set_Subtest
   (
      const cut_status & s,      /**< The status object to check.             */
      int value                  /**< Value to check sub-test against.        */
   )
   {
      s.m_Status.m_Subtest = value;
   }

   /**
    *    White-box read access to the unit_test_status_t.m_Test_Result
    *    field.
    *
    *    There is no function for accessing m_Test_Result directly.  As
    *    explained at cut_status::passed(), this field is simply the result
    *    of the last sub-test to be performed.  The m_Subtest_Error_Count
    *    sub-test (the next one) is what we really want.
    */

   bool Status_Check_Test_Result
   (
      const cut_status & s,      /**< The status object to check.             */
      bool value                 /**< Value to check test-result against.     */
   )
   {
      return bool(s.status().m_Test_Result) == value;
   }

   /**
    *    White-box read access to the
    *    unit_test_status_t.m_Subtest_Error_Count field.
    */

   bool Status_Check_Subtest_Error_Count
   (
      const cut_status & s,      /**< The status object to check.             */
      int value                  /**< Value to check error-count against.     */
   )
   {
      if (g_use_direct_field_access)
         return s.status().m_Subtest_Error_Count == value;
      else
         return unit_test_status_error_count(&s.status()) == value;
   }

   /**
    *    White-box write access to the
    *    unit_test_status_t.m_Subtest_Error_Count field.
    *
    *    There is no actual setter provided for this value; it is
    *    incremented internally when an error occurs.
    */

   void Status_Set_Subtest_Error_Count
   (
      cut_status & s,            /**< The status object to set.               */
      int value                  /**< Value to set error-count to.            */
   )
   {
      s.m_Status.m_Subtest_Error_Count = value;
   }

   /**
    *    White-box read access to the unit_test_status_t.m_Failed_Subtest
    *    field.
    */

   bool Status_Check_Failed_Subtest
   (
      const cut_status & s,      /**< The status object to check.             */
      int value                  /**< Value to check failed-subtest against.  */
   )
   {
      if (g_use_direct_field_access)
         return s.status().m_Failed_Subtest == value;
      else
         return unit_test_status_failed_subtest(&s.status()) == value;
   }

   /**
    *    White-box write access to the unit_test_status_t.m_Failed_Subtest
    *    field.
    *
    *    There is a "self-test" setter for this value, so we might as well
    *    test it here.
    */

   void Status_Set_Failed_Subtest
   (
      cut_status & s,            /**< The status object to check.             */
      int value                  /**< Value to set failed-subtest field.      */
   )
   {
      if (g_use_direct_field_access)
         s.m_Status.m_Failed_Subtest = value;
      else
         status_self_test_failed_subtest_set(&s.m_Status, value);
   }

   /**
    *    White-box read access to the unit_test_status_t.m_Test_Disposition
    *    field.
    */

   bool Status_Check_Test_Disposition
   (
      const cut_status & s,      /**< The status object to check.             */
      unit_test_disposition_t d  /**< Value to check "disposition" against.   */
   )
   {
      if (g_use_direct_field_access)
         return s.status().m_Test_Disposition == d;
      else
         return unit_test_status_disposition(&s.status()) == d;
   }

   /**
    *    White-box write access to the unit_test_status_t.m_Test_Disposition
    *    field.
    *
    *    Note that the xpc::cut_status::status() function cannot be used to
    *    access xpc::cut_status::m_Status for writing.  But the friendship
    *    of cut_white_box with xpc::cut_status let's us use it directly.
    *    There is no C setter function, however.
    */

   void Status_Set_Test_Disposition
   (
      cut_status & s,            /**< The status object to be set.            */
      unit_test_disposition_t d  /**< The disposition value to be set.        */
   )
   {
      s.m_Status.m_Test_Disposition = d;
   }

   /**
    *    White-box read access to the unit_test_status_t.m_Start_Time field.
    *
    *    There is no C getter for this field.
    */

   bool Status_Check_Start_Time_Zero
   (
      const cut_status & s       /**< The status object to check.             */
   )
   {
      bool ok = s.status().m_Start_Time_us.tv_sec == 0;
      if (ok)
         ok = s.status().m_Start_Time_us.tv_usec == 0;

      return ok;
   }

   /**
    *    White-box read access to the unit_test_status_t.m_Start_Time field.
    *
    *    There is no C getter for this field.
    */

   bool Status_Check_Start_Time
   (
      const cut_status & s       /**< The status object to check.             */
   )
   {
      bool ok = s.status().m_Start_Time_us.tv_sec > 0;
      if (ok)
         ok = s.status().m_Start_Time_us.tv_usec >= 0;

      return ok;
   }

   /**
    *    White-box access to the unit_test_status_t.m_End_Time field.
    *
    *    There is no C getter for this field.
    */

   bool Status_Check_End_Time_Zero
   (
      const cut_status & s       /**< The status object to check.             */
   )
   {
      bool ok = s.status().m_End_Time_us.tv_sec == 0;
      if (ok)
         ok = s.status().m_End_Time_us.tv_usec == 0;

      return ok;
   }

   /**
    *    White-box access to the unit_test_status_t.m_End_Time field.
    *
    *    There is no C getter for this field.
    */

   bool Status_Check_End_Time
   (
      const cut_status & s       /**< The status object to check.             */
   )
   {
      bool ok = s.status().m_End_Time_us.tv_sec > 0;
      if (ok)
         ok = s.status().m_End_Time_us.tv_usec >= 0;

      return ok;
   }

   /**
    *    Show the error-status of the unit_test_status_t time fields.
    *
    *    We tried to convert this output call into usage of std::cout,
    *    std::endl, and operator <<.  However, it is much more difficult to
    *    get right, and much more difficult to understand what it is trying
    *    to show.  So, forget about it!  Stay calm!  Use the C calls!
    */

   void Status_Check_Show_Time_Error
   (
      const cut_status & s       /**< The status object to show time values.  */
   )
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
         int(s.status().m_Start_Time_us.tv_sec),
         int(s.status().m_Start_Time_us.tv_usec),
         int(s.status().m_End_Time_us.tv_sec),
         int(s.status().m_End_Time_us.tv_usec)
      );
   }

   /**
    *    White-box read access to the unit_test_status_t.m_Test_Duration
    *    field.
    */

   bool Status_Check_Test_Duration
   (
      const cut_status & s,      /**< The status object to check.             */
      double t                   /**< Value to check the duration against.    */
   )
   {
      bool ok;
      if (g_use_direct_field_access)
         ok = s.status().m_Test_Duration_ms == t;
      else
         ok = unit_test_status_duration_ms(&s.status()) == t;

      if (! ok)
      {
         std::cout
            << "? m_Test_Duration = "
            << s.status().m_Test_Duration_ms
            << std::endl
            ;
      }
      return ok;
   }

   /**
    *    White-box write access to the unit_test_status_t.m_Test_Duration
    *    field.
    *
    *    There is no C setter for this field.
    */

   void Status_Set_Test_Duration
   (
      const cut_status & s,      /**< The status object to set.               */
      double t                   /**< Value to set the duration to.           */
   )
   {
      s.m_Status.m_Test_Duration_ms = t;
   }

   /*
    * unit_test_options_t access functions
    */

   /**
    *    White-box access to the private xpc::cut_options::options()
    *    function.
    *
    *    The access is via pointer.
    */

   const unit_test_options_t * Options_Const_Options_Pointer
   (
      const cut_options & s      /**< The options reference used for access.  */
   )
   {
      if (g_use_direct_field_access)
         return &s.m_Options;          /* not const, but converted to const   */
      else
         return &s.options();
   }

};

}           /* namespace xpc  */

/**
 *    Provides a basic "smoke test" for the xpc::cut-related classes.
 *    A smoke test is a test of the basic functionality of the object or
 *    function.  It is an easy test that makes sure the code has basic
 *    integrity.
 *
 *    This test is about the simplest unit test function that can be
 *    written.
 *
 * \group
 *    1. Smoke tests.
 *
 * \case
 *    1. Basic smoke test.
 *
 * \note
 *    It all of these unit-tests, it is important to understand that a
 *    status coming up invalid [i.e. status.valid() == false] is \e not a
 *    test failure -- it only indicates that the status object is invalid \e
 *    or that the test is not allowed to run.
 *
 * \tests
 *    -  xpc::cut() [indirectly, by the tests running at all]
 *    -  xpc::cut_status()
 *    -  xpc::cut_options()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_01_01 (const xpc::cut_options & options)
{
   xpc::cut_status status(options, 1, 1, "CUT++", _("Basic Smoke Test"));
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         show(options, _("No values to show in this test"));
         if (status.next_subtest("cut_status::pass()"))
            status.pass();
      }
   }
   return status;
}

/**
 *    Provides a more advanced "smoke test" for the xpc::cut class.
 *    This smoke test parallels unit_unit_test_01_01() in the xpccut C
 *    library.
 *
 * \group
 *    1. Smoke tests.
 *
 * \case
 *    2. Advance smoke test.
 *
 * \tests
 *    -  xpc::cut_status()
 *    -  xpc::cut_options()
 */

static xpc::cut_status
cut_unit_test_01_02 (const xpc::cut_options & options)
{
   xpc::cut_status status(options, 1, 2, "CUT++", _("Advanced Smoke Test"));
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         show(options, _("No values to show in this test"));

         /*  1 */

         if (status.next_subtest("cut_status::pass()"))
         {
            status.pass();
            ok = status.error_count() == 0 && status.failed_subtest() == 0;
            if (ok)
            {
               inform(options, _("We made it past the first sub-test"));
            }
            else
            {
               std::string s("cut_status::pass(): ");
               s += _("internal failure");
               (void) status.fail();
               inform(options, s);
            }
            status.pass(ok);
         }

         /*  2 */

         if (ok && status.next_subtest("cut_status::fail()"))
         {
            /**
             * \tricky
             *    The following two calls create a failure and then converts
             *    it to a pass.  Note the usage of a couple of "self-test"
             *    functions.  These functions are meant \e only for usage in
             *    the CUT++ unit-test application.  Do \e not use them in
             *    your own unit-test applications.
             */

            ok = status.fail_deliberately();       /* force a failure         */
            if (ok)
            {
               ok = status.error_count() == 1 && status.failed_subtest() == 2;
               status.pass(ok);                    /* convert it to a "pass"  */
               if (ok)
               {
                  status.self_test_error_count_decrement(); /* internal only! */
                  status.self_test_failed_subtest_set(0);   /* internal only! */
               }
            }
            status.pass(ok);
         }

         /*
          * This duration check just flies by, and is now useless.
          *
          * if (ok)
          *    xpccut_ms_sleep(100);               // a visual duration check
          */
      }
   }
   return status;
}

/**
 *    Provides a way to verify the default options.
 *    This is a function specific to this unit-test application.
 *
 * \return
 *    Return's true if all the default are matched.  There is no check of
 *    the op parameter in this internal helper function.
 */

static bool
default_options_check
(
   const xpc::cut_options & op,  /**< The options structure to be checked.    */
   bool test_show_progress       /**< If true, test this special case.        */
)
{
   bool result;
   const char * itemname = "m_Is_Verbose";
   result = op.is_verbose() == XPCCUT_IS_VERBOSE;
   if (result)
   {
      itemname = "show_values()";
      result = op.show_values() == XPCCUT_SHOW_VALUES;
   }
   if (result)
   {
      itemname = "use_text_synch()";
      result = op.use_text_synch() == XPCCUT_TEXT_SYNCH;
   }
   if (result)
   {
      itemname = "show_step_numbers()";
      result = op.show_step_numbers() == XPCCUT_SHOW_STEP_NUMBERS;
   }
   if (result)
   {
      /*
       * We sometimes pass along the --no-show-progress option from the
       * normal options structure to the test options structure.  (See the
       * references to "x_options" in the code below).  Therefore, we
       * don't want to regression test for this time.  We'll add special
       * cases in the tests to make sure we don't miss it.
       */

      if (test_show_progress)
      {
         itemname = "show_progress()";
         result = op.show_progress() == XPCCUT_SHOW_PROGRESS;
      }
   }
   if (result)
   {
      itemname = "stop_on_error()";
      result = op.stop_on_error() == XPCCUT_STOP_ON_ERROR;
   }
   if (result)
   {
      itemname = "batch_mode()";
      result = op.batch_mode() == XPCCUT_BATCH_MODE;
   }
   if (result)
   {
      itemname = "is_interactive()";
      result = op.is_interactive() == XPCCUT_INTERACTIVE;
   }
   if (result)
   {
      itemname = "do_beep()";
      result = op.do_beep() == XPCCUT_BEEP;
   }
   if (result)
   {
      itemname = "is_summary()";
      result = op.is_summary() == XPCCUT_SUMMARIZE;
   }
   if (result)
   {
      itemname = "need_subtests()";
      result = op.need_subtests() == XPCCUT_NEED_SUBTESTS;
   }
   if (result)
   {
      itemname = "force_failure()";
      result = op.force_failure() == XPCCUT_FORCE_FAILURE;
   }
   if (result)
   {
      itemname = "is_pause()";
      result = op.is_pause() == XPCCUT_CASE_PAUSE;
   }
   if (result)
   {
      itemname = "single_group()";
      result = op.single_group() == XPCCUT_NO_SINGLE_GROUP;
   }
   if (result)
   {
      itemname = "single_case()";
      result = op.single_case() == XPCCUT_NO_SINGLE_CASE;
   }
   if (result)
   {
      itemname = "single_subtest()";
      result = op.single_subtest() == XPCCUT_NO_SINGLE_SUB_TEST;
   }
   if (result)
   {
      itemname = "test_sleep_time()";
      result = op.test_sleep_time() == XPCCUT_TEST_SLEEP_TIME;
   }
   if (result)
   {
      itemname = "current_test()";
      result = op.current_test() == -1;
   }
   if (! result)
      xpccut_errprint_ex(_("bad default option value"), itemname);

   return result;
}

/**
 *    Provides a unit/regression test to verify that the basic
 *    initialization function properly zeros or clears all fields to their
 *    "default" values.
 *
 *    Note how this unit-test exactly parallels the unit_unit_test_02_01()
 *    function for the xpccut library's unit-test application, even
 *    including stuff that isn't really possible to have go wrong in C++.
 *
 *    Also note that it uses basically the same direct access to the fields
 *    of the C structures.
 *
 *    It also adds tests of the accessors to supplement the direct-access
 *    tests.  These are additional implementations of some of the sub-tests
 *    using member functions instead.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *    1. Initialization and accessors.
 *
 * \test
 *    -  xpc::cut_status() [default constructor]
 *    -  xpc::cut_status::group_name()
 *    -  xpc::cut_status::case_name()
 *    -  xpc::cut_status::subtest_name()
 *    -  xpc::cut_status::group()
 *    -  xpc::cut_status::kase()
 *    -  xpc::cut_status::subtest()
 *    -  xpc::cut_status::error_count()
 *    -  xpc::cut_status::failed_subtest()
 *    -  xpc::cut_status::disposition()
 *    -  xpc::cut_status::duration_ms()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_01 (const xpc::cut_options & options)
{
   xpc::cut_status status           /* not part of the test, see below!       */
   (
      options, 2, 1, "xpc::cut_status", "xpc::cut_status()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_status x_status;     /* this is the subject of this test       */

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Good 'this'"))
      {
         inform(options, _("A good 'this' is always a given in C++ code"));
         status.pass();
      }

      /*  3 */

      if (status.next_subtest("m_Test_Options"))
      {
         /*
          * Here, we'd expect the internal options pointer to be null.  But
          * we can't check for nulls, because of XPC_NO_THISPTR.  So the
          * default constructor for xpc::cut_status wires in an internal
          * structure.  So this will return false:
          *
          *    ok = cwb.Status_Check_Test_Options_Pointer(x_status, nullptr);
          *
          * Maybe we should hide the default constructor!
          */

         xpc::cut_white_box cwb;
         ok = ! cwb.Status_Check_Test_Options_Pointer(x_status, nullptr);
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("m_Group_Name, group_name()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Group_Name(x_status, "");
         if (ok)
            ok = x_status.group_name() == "";

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("m_Case_Description, case_name()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Case_Description(x_status, "");
         if (ok)
            ok = x_status.case_name() == "";

         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("m_Subtest_Name, subtest_name()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Subtest_Name(x_status, "");
         if (ok)
            ok = x_status.subtest_name() == "";

         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("m_Test_Group, group()"))
      {
         /*
          * The default constructor for xpc::cut_status must create a valid
          * object, including its pointer to its options.  To do so, the
          * group and case must be valid, or else
          * unit_test_status_initialize() will not assign the pointer and
          * some other items.
          */

         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Group(x_status, 1);  /* not 0 */
         if (ok)
            ok = x_status.group() == 1;                  /* not 0 */

         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("m_Test_Case, kase()"))
      {
         /*
          * The default constructor for xpc::cut_status must create a valid
          * object, including its pointer to its options.  To do so, the
          * group and case must be valid, or else
          * unit_test_status_initialize() will not assign the pointer and
          * some other items.
          */

         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Case(x_status, 1);   /* not 0 */
         if (ok)
            ok = x_status.kase() == 1;                   /* not 0 */

         status.pass(ok);
      }

      /*  9 */

      if (status.next_subtest("m_Subtest, subtest()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Subtest(x_status, 0);
         if (ok)
            ok = x_status.subtest() == 0;

         status.pass(ok);
      }

      /* 10 */

      if (status.next_subtest("m_Test_Result"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Result(x_status, true);
         status.pass(ok);
      }

      /* 11 */

      if (status.next_subtest("m_Subtest_Error_Count, passed(), error_count()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Subtest_Error_Count(x_status, 0);
         if (ok)
            ok = x_status.passed();

         if (ok)
            ok = x_status.error_count() == 0;

         status.pass(ok);
      }

      /* 12 */

      if (status.next_subtest("m_Failed_Subtest, failed_subtest()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Failed_Subtest(x_status, 0);
         if (ok)
            ok = x_status.failed_subtest() == 0;

         status.pass(ok);
      }

      /* 13 */

      if (status.next_subtest("m_Test_Disposition, disposition()"))
      {
         /*
          * Since we now call unit_test_status_initialize(), instead of
          * unit_test_status_init(), in the xpc::cut_status default
          * constructor, the disposition is not XPCCUT_DISPOSITION_ABORTED,
          * but XPCCUT_DISPOSITION_CONTINUE.
          */

         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Disposition
         (
            x_status, XPCCUT_DISPOSITION_CONTINUE
         );
         if (ok)
            ok = x_status.disposition() == XPCCUT_DISPOSITION_CONTINUE;

         status.pass(ok);
      }

      /* 14 */

      if (status.next_subtest("m_Start_Time_us"))
      {
         /*
          * There is no C++ accessor for this field.
          *
          * Since we now call unit_test_status_initialize(), instead of
          * unit_test_status_init(), in the xpc::cut_status default
          * constructor, the start-time now gets set.  So we no longer check
          * for a 0 start time.
          */

         xpc::cut_white_box cwb;
         ok = ! cwb.Status_Check_Start_Time_Zero(x_status);
         status.pass(ok);
      }

      /* 15 */

      if (status.next_subtest("m_End_Time_us"))
      {
         /*
          * There is no C++ accessor for this field.
          */

         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_End_Time_Zero(x_status);
         status.pass(ok);
      }

      /* 16 */

      if (status.next_subtest("m_Test_Duration_ms, duration_ms()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Duration(x_status, 0.0);
         if (ok)
            ok = x_status.duration_ms() == 0.0;

         status.pass(ok);
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
 *    2. xpc::cut_status functions.
 *
 * \case
 *    2. More initialization and accessors.
 *
 * \test
 *    -  xpc::cut_status() [principal constructor]
 *    -  xpc::cut_status::show_progress()
 *    -  xpc::cut_status::current_test()
 *    -  xpc::cut_status::group_name()
 *    -  xpc::cut_status::case_name()
 *    -  xpc::cut_status::subtest_name()
 *    -  xpc::cut_status::group()
 *    -  xpc::cut_status::kase()
 *    -  xpc::cut_status::subtest()
 *    -  xpc::cut_status::error_count()
 *    -  xpc::cut_status::failed_subtest()
 *    -  xpc::cut_status::disposition()
 *    -  xpc::cut_status::duration_ms()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_02 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 2, "xpc::cut_status", "cut_status::initialize()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*
       * Note that the 'simulated' option is used in order better
       * distinguish test output from the real output.
       */

      xpc::cut_options x_options(XPCCUT_OPTIONS_SIMULATED);
      xpc::cut_status x_status
      (
         x_options, 33, 44, _("Group 33"), _("Case 44")
      );

      /*  1 */

      if (status.next_subtest("Normal setup"))
      {
         ok = x_status.valid() && x_options.valid();
         if (ok)
         {
            /*
             * Pass along this option to guarantee hiding output even from
             * the secondary test options function calls.
             */

            x_options.show_progress(options.show_progress());
         }
         else
         {
            xpccut_errprint(_("internal failure, could not set up options"));
            status.pass(false);
         }
      }

      /*  2 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  3 */

      if (status.next_subtest("Null options-pointer"))
      {

#if ! defined(__GNUG__) && ! defined(WIN32)

         /*
          * g++ complains about the reinterpret_cast:
          *
          *    error: invalid cast of an rvalue expression of type 'int' to
          *    type 'const xpc::cut_options&'
          */

         try
         {
            xpc::cut_status y_status_y
            (
               reinterpret_cast<const xpc::cut_options &>(nullptr),
               , 99, 99, nullptr, "yyy"
            );
            status.pass(! y_status_y.valid());
         }
         catch (std::logic_error & exc )
         {
            std::string s(_("std::logic_error caught: "));
            s += exc.what();
            inform(options, s);
            status.pass();
         }
#else
         inform(options, _("A null reference parameter is impossible in C++"));
         status.pass();
#endif

      }

      /*  4 */

      if (status.next_subtest("Zero group-number"))
      {
         xpc::cut_status y_status_y
         (
            options, 0, 99, "xxx", "yyy"
         );
         status.pass(! y_status_y.valid());
      }

      /*  5 */

      if (status.next_subtest("Zero case-number"))
      {
         xpc::cut_status y_status_y
         (
            options, 99, 0, "xxx", "yyy"
         );
         status.pass(! y_status_y.valid());
      }

      /*  6 */

      if (status.next_subtest("Null group-name"))
      {
         /**
          * \gcc
          *    The null pointer cannot be used to construct an std::string.
          *    It causes the following error:
          *
          *       terminate called after throwing an instance of  \n
          *          'std::logic_error'   \n
          *       what():  basic_string::_S_construct NULL not valid  \n
          *       Aborted  \n
          *
          * \win32
          *    With Visual Studio (in Debug mode, not yet sure about Release
          *    mode, it causes an assertion.  Yikes.
          */

         try
         {
            xpc::cut_status y_status_y
            (
               options, 99, 99, NULL_PTR_PARAM, "yyy"
            );
#ifdef _MSC_VER
            status.pass(y_status_y.valid());          // cannot test this one
#else
            status.pass(! y_status_y.valid());
#endif
         }
         catch (std::logic_error & exc)
         {
            std::string s(_("std::logic_error caught: "));
            s += exc.what();
            inform(options, s);
            status.pass();
         }
         catch (...)
         {
            inform(options, _("exception probably due to null pointer"));
            status.pass();
         }
      }

      /*  7 */

      if (status.next_subtest("Null case-name"))
      {
         try
         {
            xpc::cut_status y_status_y
            (
               options, 99, 99, "xxx", NULL_PTR_PARAM
            );
#ifdef _MSC_VER
            status.pass(y_status_y.valid());
#else
            status.pass(! y_status_y.valid());
#endif
         }
         catch (std::logic_error & exc)
         {
            /*
             * Well, we've used this construct twice now, so next time we'll
             * make a function out of it!
             */

            std::string s(_("std::logic_error caught: "));
            s += exc.what();
            inform(options, s);
            status.pass();
         }
         catch (...)
         {
            inform(options, _("exception probably due to null pointer"));
            status.pass();
         }
      }

      /*  8 */

      if (status.next_subtest("Good 'this'"))
      {
         xpc::cut_status y_status_y;
         status.pass(y_status_y.valid());
      }

      /*  9 */

      if (status.next_subtest("m_Test_Options"))
      {
         xpc::cut_white_box cwb;
         ok = x_status.valid();
         if (ok)
         {
            ok = cwb.Status_Check_Test_Options_Pointer
            (
               x_status, (void *) cwb.Options_Const_Options_Pointer(x_options)
            );
         }
         if (ok)
         {
            /*
             * This subtest is actually kind of redundent relative to the
             * test of xpc::cut_options().  Not totally redundant, though.
             */

            ok = x_options.valid();
            if (ok)
               ok = default_options_check(x_options, false);

            /*
             * Do this after so the check above can succeed.
             */

            if (ok)
            {
               x_options.current_test(g_BOGUS_TEST_NUMBER);
               ok = x_options.current_test() == g_BOGUS_TEST_NUMBER;
            }
         }
         else
            xpccut_errprint(_("internal failure, options address bad"));

         status.pass(ok);
      }

      /* 10 */

      if (status.next_subtest("m_Group_Name, group_name()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Group_Name(x_status, _("Group 33"));
         if (ok)
            ok = x_status.group_name() == _("Group 33");

         status.pass(ok);
      }

      /* 11 */

      if (status.next_subtest("m_Case_Description, case_name()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Case_Description(x_status, _("Case 44"));
         if (ok)
            ok = x_status.case_name() == _("Case 44");

         status.pass(ok);
      }

      /* 12 */

      if (status.next_subtest("m_Subtest_Name, subtest_name()"))
      {
         /*
          * Currently, the subtest-name is left at it default value from
          * cut_status::init(), an empty string.
          */

         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Subtest_Name(x_status, "");
         if (ok)
            ok = x_status.subtest_name() == "";

         status.pass(ok);
      }

      /* 13 */

      if (status.next_subtest("m_Test_Group, group()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Group(x_status, 33);
         if (ok)
            ok = x_status.group() == 33;

         status.pass(ok);
      }

      /* 14 */

      if (status.next_subtest("m_Test_Case, kase()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Case(x_status, 44);
         if (ok)
            ok = x_status.kase() == 44;

         status.pass(ok);
      }

      /* 15 */

      if (status.next_subtest("m_Subtest, subtest()"))
      {
         /*
          * Currently, the subtest-number is left at it default value from
          * cut_status::init(), zero.
          */

         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Subtest(x_status, 0);
         if (ok)
            ok = x_status.subtest() == 0;

         status.pass(ok);
      }

      /* 16 */

      if (status.next_subtest("m_Test_Result"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Result(x_status, true);

         /*
          * There is no C++ accessor function for accessing m_Test_Result.
          * (There's no C accessor function for this purpose, either.)
          */

         status.pass(ok);
      }

      /* 17 */

      if (status.next_subtest("m_Subtest_Error_Count, error_count()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Subtest_Error_Count(x_status, 0);
         if (ok)
            ok = x_status.error_count() == 0;

         status.pass(ok);
      }

      /* 18 */

      if (status.next_subtest("m_Failed_Subtest, failed_subtest()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Failed_Subtest(x_status, 0);
         if (ok)
            ok = x_status.failed_subtest() == 0;

         status.pass(ok);
      }

      /* 19 */

      if (status.next_subtest("m_Test_Disposition, disposition"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Disposition
         (
            x_status, XPCCUT_DISPOSITION_CONTINUE
         );
         if (ok)
            ok = x_status.disposition() == XPCCUT_DISPOSITION_CONTINUE;

         if (ok)
            ok = x_status.is_continue();

         status.pass(ok);
      }

      /* 20 */

      if (status.next_subtest("m_Start_Time_us"))
      {
         /*
          * There is no C++ accessor for this field.  But we can access it
          * using the friend class, xpc::cut_white_box.
          */

         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Start_Time(x_status);
         status.pass(ok);
      }

      /* 21 */

      if (status.next_subtest("m_End_Time_us"))
      {
         /*
          * There is no C++ accessor for this field.  But we can access it
          * using the friend class, xpc::cut_white_box.
          */

         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_End_Time_Zero(x_status);
         status.pass(ok);
      }

      /* 22 */

      if (status.next_subtest("m_Test_Duration_ms, duration_ms()"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Duration(x_status, 0.0);
         if (ok)
            ok = x_status.duration_ms() == 0.0;

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "reset"
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *    3. Reset functionality.
 *
 * \test
 *    -  xpc::cut_status.disposition()
 *    -  xpc::cut_status.reset()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_03 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 3,
      "xpc::cut_status", "cut_status::reset()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_status x_status;

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Good initialization"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Disposition
         (
            x_status, XPCCUT_DISPOSITION_CONTINUE     /* not ABORTED now      */
         );
         if (ok)
            ok = x_status.disposition() == XPCCUT_DISPOSITION_CONTINUE;

         if (ok)
            ok = ! x_status.is_aborted();

         if (ok)
            ok = x_status.is_continue();

         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Good reset of disposition"))
      {
         xpc::cut_white_box cwb;
         ok = x_status.reset();
         if (ok)
         {
            ok = cwb.Status_Check_Test_Disposition
            (
               x_status, XPCCUT_DISPOSITION_CONTINUE
            );
         }
         if (ok)
            ok = x_status.disposition() == XPCCUT_DISPOSITION_CONTINUE;

         if (ok)
            ok = x_status.is_continue();

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "pass/fail`"
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *    4. Initialization and accessors.
 *
 * \test
 *    -  xpc::cut_status.pass()
 *    -  xpc::cut_status.fail()
 *    -  xpc::cut_status.fail_deliberately()
 *    -  xpc::cut_status::group_name()
 *    -  xpc::cut_status::case_name()
 *    -  xpc::cut_status::subtest_name()
 *    -  xpc::cut_status::group()
 *    -  xpc::cut_status::kase()
 *    -  xpc::cut_status::subtest()
 *    -  xpc::cut_status::error_count()
 *    -  xpc::cut_status::failed_subtest()
 *    -  xpc::cut_status.init()
 *    -  xpc::cut_options.init()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_04 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 4, "xpc::cut_status", "cut_status::pass/fail()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*
       * Note that the 'simulated' option is used in order better
       * distinguish test output from the real output.
       */

      xpc::cut_options x_options(XPCCUT_OPTIONS_SIMULATED);
      xpc::cut_status x_status;
      ok = x_status.valid() && x_options.valid();
      if (ok)
      {
         /*
          * Pass along this option to guarantee hiding output even from
          * the secondary test options function calls.
          */

         x_options.show_progress(options.show_progress());
      }

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Good initialization"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Result(x_status, true);
         if (ok)
            ok = cwb.Status_Check_Subtest_Error_Count(x_status, 0);

         if (ok)
            ok = x_status.error_count() == 0;

         if (ok)
            ok = cwb.Status_Check_Failed_Subtest(x_status, 0);

         if (ok)
            ok = x_status.failed_subtest() == 0;

         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Good failure with pass()"))
      {
         /*
          * Wire in a sub-test number by accessing the structure directly (a
          * big no-no, except in our unit test.  Also, since we're now
          * making a legitimate call to fail_deliberately() for our fake
          * status class, x_status, we have to hard-wired in the fake
          * options class, too.  Otherwise, a meaningless (in the context of
          * this unit-test) and confusing error message occurs.
          *
          * Note that there is no setter in xpc::cut_status for the
          * m_Subtest field.  (There is only a getter.)
          *
          * Now forbidden.
          *
          *    x_status.status().m_Subtest = 99;
          *
          * Hook into the options structure after-the-fact.  To be
          * forbidden.
          *
          *    x_status.status().m_Test_Options = &(x_options.options());
          */

         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Result(x_status, true);
         if (ok)
            ok = x_status.fail_deliberately();

         if (ok)
            ok = cwb.Status_Check_Test_Result(x_status, false);

         if (ok)
            ok = cwb.Status_Check_Subtest_Error_Count(x_status, 1);

         if (ok)
            ok = x_status.error_count() == 1;

         /*
          * Sub-test value no longer set to 99 above.
          */

         if (ok)
            ok = cwb.Status_Check_Failed_Subtest(x_status, 0);

         if (ok)
            ok = x_status.failed_subtest() == 0;

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Good success"))
      {
         /*
          * Wire in a sub-test number by accessing the structure directly (a
          * big no-no, except in our unit test.  Also, this sub-test depends
          * on settings made in the previous sub-test.
          *
          * Now forbidden.
          *
          *    x_status.status().m_Subtest = 100;
          */

         xpc::cut_white_box cwb;
         ok = x_status.pass();
         if (ok)
            ok = cwb.Status_Check_Test_Result(x_status, true);

         if (ok)
            ok = cwb.Status_Check_Subtest_Error_Count(x_status, 1);

         if (ok)
            ok = x_status.error_count() == 1;

         /*
          * Sub-test value no longer set to 99 above.
          */

         if (ok)
            ok = cwb.Status_Check_Failed_Subtest(x_status, 0);

         if (ok)
            ok = x_status.failed_subtest() == 0;

         status.pass(ok);
      }

      /*
       * The same tests, through the unit_test_status_fail() interface.
       * Reset the structures again.
       *
       * However, we've made this function private, as it zeroes an options
       * pointer in the class, causing it to become useless.
       *
       * (void) x_status.init();                 // set up for sane usage
       */

      (void) x_options.init();                   /* ditto, for options      */
      x_options.is_simulated(true);
      if (ok)
      {
         /*
          * Pass along this option to guarantee hiding output even from
          * the secondary test options function calls.
          */

         x_options.show_progress(options.show_progress());
      }

      /*  5 */

      if (status.next_subtest("Null 'this', fail()"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  6 */

      if (status.next_subtest("Good initialization"))
      {
         xpc::cut_white_box cwb;
         ok = cwb.Status_Check_Test_Result(x_status, true);

         /*
          * Since we no longer call x_status.init(), the error count remains
          * at what it was.  However, there is no failed subtest.  Need to
          * look into that.
          */

         if (ok)
            ok = cwb.Status_Check_Subtest_Error_Count(x_status, 1);

         if (ok)
            ok = x_status.error_count() == 1;

         if (ok)
            ok = cwb.Status_Check_Failed_Subtest(x_status, 0);

         if (ok)
            ok = x_status.failed_subtest() == 0;

         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("Good failure with fail()"))
      {
         /*
          * Same comments as for subtest 3 above.
          *
          * However, assigned to the status() reference is now forbidden.
          *
          *    x_status.status().m_Subtest = 98;
          *    x_status.status().m_Test_Options = &(x_options.options());
          */

         xpc::cut_white_box cwb;
         ok = x_status.fail_deliberately();           /* another error */
         if (ok)
            ok = cwb.Status_Check_Test_Result(x_status, false);

         if (ok)
            ok = cwb.Status_Check_Subtest_Error_Count(x_status, 2);

         if (ok)
            ok = x_status.error_count() == 2;

         /*
          * Sub-test value no longer set to 98 above.
          */

         if (ok)
            ok = cwb.Status_Check_Failed_Subtest(x_status, 0);

         if (ok)
            ok = x_status.failed_subtest() == 0;

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Shows the nominal and actual durations in a stock format.
 *    This is an internal helper function.
 */

static void
s_show_durations
(
   int nominal_duration,      /**< The expected duration, in integer format.  */
   double actual_duration     /**< The actual duration, in floating-point.    */
)
{
   fprintf
   (
      stdout, "  %s: %s = %d ms, %s = %f ms\n",
      _("Duration"), _("nominal"), nominal_duration,
      _("actual"), float(actual_duration)
   );
}

/**
 *    Provides a unit/regression test to somewhat verify the timing
 *    functionality.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *    5. Timer functions.
 *
 * \test
 *    -  xpc::cut_status::start_timer()
 *    -  xpc::cut_status::time_delta()
 *    -  xpc::cut_status::duration_ms()
 *    -  xpccut_ms_sleep()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_05 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 5, "xpc::cut_status", "xpc::cut_status.time/time_delta()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_status x_status;

      /*  1 */

      if (status.next_subtest("Null 'this', start_timer()"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Basic time-getting"))
      {
         xpc::cut_white_box cwb;
         ok = x_status.start_timer();
         if (ok)
            ok = cwb.Status_Check_End_Time_Zero(x_status);

         if (ok)
            ok = cwb.Status_Check_Start_Time(x_status);

         if (! ok && options.is_verbose())
            cwb.Status_Check_Show_Time_Error(x_status);

         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Null 'this', time_delta() 1"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  4 */

      if (status.next_subtest("Null 'this', time_delta() 2"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  5 */

      if (status.next_subtest("Time-difference, 50 ms"))
      {
         xpc::cut_white_box cwb;
         ok = x_status.start_timer();
         if (ok)
            ok = cwb.Status_Check_End_Time_Zero(x_status);

         if (ok)
            ok = cwb.Status_Check_Start_Time(x_status);

         if (ok)
         {
            double d;
            xpccut_ms_sleep(50);
            d = x_status.time_delta();   /* default is false, no reset */
            if (ok)
               ok = cwb.Status_Check_Test_Duration(x_status, d);

            if (ok)
               ok = x_status.duration_ms() == d;

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
               ok = (d > 40.0) && (d < 70.0);
#endif
            }
            else
               xpccut_errprint(_("Subtest 4, m_Test_Duration_ms not set"));

            if (! ok)
               xpccut_errprint(_("Subtest 4, duration out of range"));

            if (options.is_verbose())
               s_show_durations(50, d);
         }
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("Time-difference, 25 ms"))
      {
         ok = x_status.start_timer();
         if (ok)
         {
            xpc::cut_white_box cwb;
            double d;
            xpccut_ms_sleep(25);
            ok = cwb.Status_Check_Start_Time(x_status);
            if (ok)
            {
               d = x_status.time_delta(false);   /* false == no reset */
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
                  xpccut_errprint(_("Subtest 5, duration out of range"));

               if (options.is_verbose())
                  s_show_durations(25, d);
            }
         }
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("Time-difference, 10 ms"))
      {
         ok = x_status.start_timer();
         if (ok)
         {
            xpc::cut_white_box cwb;
            double d;
            xpccut_ms_sleep(10);
            ok = cwb.Status_Check_Start_Time(x_status);
            if (ok)
            {
               d = x_status.time_delta(false);  /* no reset */
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
                  xpccut_errprint(_("Subtest 6, duration out of range"));

               if (options.is_verbose())
                  s_show_durations(10, d);
            }
         }
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("Time-difference, 5 ms"))
      {
         ok = x_status.start_timer();
         if (ok)
         {
            xpc::cut_white_box cwb;
            double d;
            xpccut_ms_sleep(5);
            ok = cwb.Status_Check_Start_Time(x_status);
            if (ok)
            {
               d = x_status.time_delta(false);  /* no reset */
#ifdef WIN32
                  /*
                   * At least under Win 2000 in a VM, there seems to be more
                   * variance in the timing.
                   */

               ok = (d > 1.0) && (d < 25.0);
#else
               ok = (d > 1.0) && (d < 15.0);                /* up from 10.0   */
#endif
               if (! ok)
                  xpccut_errprint(_("Subtest 7, duration out of range"));

               if (options.is_verbose())
                  s_show_durations(5, d);
            }
         }
         status.pass(ok);
      }

      /*  9 */

      if (status.next_subtest("Time-difference, 1 ms"))
      {
         ok = x_status.start_timer();
         if (ok)
         {
            xpc::cut_white_box cwb;
            double d;
            xpccut_ms_sleep(1);
            ok = cwb.Status_Check_Start_Time(x_status);
            if (ok)
            {
               d = x_status.time_delta(false);  /* no reset */
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
                  xpccut_errprint(_("Subtest 8, duration out of range"));

               if (options.is_verbose())
                  s_show_durations(1, d);
            }
         }
         status.pass(ok);
      }

      /* 10 */

      if (status.next_subtest("Time-difference with reset"))
      {
         ok = x_status.start_timer();
         if (ok)
         {
            xpc::cut_white_box cwb;
            double d;
            xpccut_ms_sleep(5);
            d = x_status.time_delta(true);          /* true == with reset   */
            ok = cwb.Status_Check_Start_Time(x_status);
            if (! ok)
               xpccut_errprint(_("Subtest 10, m_Start_Time_us.tv_sec == 0"));

            if (ok)
            {
               ok = cwb.Status_Check_Test_Duration(x_status, d);
               if (! ok)
                  xpccut_errprint(_("Subtest 10, m_Start_Time_us.tv_usec < 0"));
            }
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the show-title
 *    functionality works.
 *
 *    There's not much to this test.  It can check only the 'this'
 *    handling and show some output when --verbose is used.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *    6. show_title().
 *
 * \test
 *    -  xpc::cut_status.show_title()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_06 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 6, "xpc::cut_status", "xpc::cut_status.show_title()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         ok = status.show_title();
         status.pass(ok);
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
 *    2. xpc::cut_status functions.
 *
 * \case
 *    7. can_proceed()
 *
 * \followup
 *    Since we've changed xpc::cut_status::status() to a \e const function,
 *    this test no longer does much -- we need to figure out a way to to
 *    affect the disposition using only internal functions and then test the
 *    results through the accessor.
 *
 * \test
 *    -  xpc::cut_status::can_proceed()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_07 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 7,
      "xpc::cut_status", "unit_test_status_can_proceed()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_status x_status;

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("CONTINUE"))
      {
         /**
          * All tests that assign to the status() reference are now
          * forbidden.  We no longer want to allow direct access to the
          * unit_test_status_t reference returned by the status() function.
          *
          * Furthermore, access to setting the disposition is completely
          * wrapped in C functions -- the C++ wrapper library does not
          * directly modify the disposition.  So these tests that were made
          * in the C unit-test are not needed in the C++ unit-test.
          *
          *    ok = unit_test_status_can_proceed(&(x_status.status()));
          */

         xpc::cut_white_box cwb;
         cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_CONTINUE);
         ok = cwb.Status_Check_Test_Disposition
         (
            x_status, XPCCUT_DISPOSITION_CONTINUE
         );
         if (ok)
            ok = x_status.can_proceed();           /* if not ABORTED or DNT   */

         if (ok)
            ok = x_status.is_continue();

         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("DNT (Did Not Test)"))
      {
         xpc::cut_white_box cwb;
         cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_DNT);
         ok = cwb.Status_Check_Test_Disposition
         (
            x_status, XPCCUT_DISPOSITION_DNT
         );
         if (ok)
            ok = ! x_status.can_proceed();         /* if ABORTED or DNT       */

         if (ok)
            ok = x_status.ignore();

         if (ok)
            ok = x_status.is_skipped();

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("FAILED"))
      {
         xpc::cut_white_box cwb;
         cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_FAILED);
         ok = cwb.Status_Check_Test_Disposition
         (
            x_status, XPCCUT_DISPOSITION_FAILED
         );
         if (ok)
            ok = x_status.can_proceed();           /* if not ABORTED or DNT   */

         if (ok)
            ok = ! x_status.ignore();

         if (ok)
            ok = x_status.is_failed();

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("QUITTED"))
      {
         xpc::cut_white_box cwb;
         cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_QUITTED);
         ok = cwb.Status_Check_Test_Disposition
         (
            x_status, XPCCUT_DISPOSITION_QUITTED
         );
         if (ok)
            ok = x_status.can_proceed();           /* if not ABORTED or DNT   */

         if (ok)
            ok = x_status.ignore();

         if (ok)
            ok = x_status.is_quitted();

         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("ABORTED"))
      {
         xpc::cut_white_box cwb;
         cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_ABORTED);
         ok = cwb.Status_Check_Test_Disposition
         (
            x_status, XPCCUT_DISPOSITION_ABORTED
         );
         if (ok)
            ok = ! x_status.can_proceed();         /* if ABORTED or DNT       */

         if (ok)
            ok = x_status.ignore();

         if (ok)
            ok = x_status.is_aborted();

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "ignore"
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *    8. ignore().
 *
 * \test
 *    -  xpc::cut_status::ignore()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_08 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 8, "xpc::cut_status", "xpc::cut_status.ignore()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_status x_status;

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("CONTINUE"))
      {
         bool not_ok;
         xpc::cut_white_box cwb;
         cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_CONTINUE);
         not_ok = ! x_status.ignore();
         if (not_ok)
            ok = cwb.Status_Check_Test_Result(x_status, true);
         else
            ok = false;

         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("DNT (Did Not Test)"))
      {
         xpc::cut_white_box cwb;
         cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_DNT);
         if (ok)
            ok = cwb.Status_Check_Test_Result(x_status, true);

         if (ok)
            ok = x_status.is_skipped();

         if (ok)
            ok = x_status.ignore();

         if (ok)
            ok = ! x_status.can_proceed();         /* if ABORTED or DNT       */

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("FAILED"))
      {
         xpc::cut_white_box cwb;
         cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_FAILED);
         if (ok)
            ok = cwb.Status_Check_Test_Result(x_status, true);

         if (ok)
            ok = x_status.is_failed();

         if (ok)
            ok = ! x_status.ignore();

         if (ok)
            ok = x_status.can_proceed();           /* if ABORTED or DNT       */

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("QUITTED"))
      {
         xpc::cut_white_box cwb;
         cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_QUITTED);

         /*
          * Note that this call checks the disposition, and sets the
          * test-result status to 'true'.  In other words, the user wants to
          * quit, but pass the current test.
          */

         ok = x_status.ignore();
         if (ok)
            ok = cwb.Status_Check_Test_Result(x_status, true);

         if (ok)
            ok = x_status.is_quitted();

         if (ok)
            ok = x_status.can_proceed();           /* if ABORTED or DNT       */

         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("ABORTED"))
      {
         xpc::cut_white_box cwb;
         cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_ABORTED);

         /*
          * Note that this call checks the disposition, and sets the
          * test-result status to 'true'.  In other words, the user wants to
          * quit, but pass the current test.
          */

         ok = x_status.ignore();
         if (ok)
            ok = cwb.Status_Check_Test_Result(x_status, false);

         if (ok)
            ok = x_status.is_aborted();

         if (ok)
            ok = ! x_status.can_proceed();         /* if ABORTED or DNT       */

         status.pass(ok);
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
 *    2. xpc::cut_status functions.
 *
 * \case
 *    9. next_subtest().
 *
 * \test
 *    -  xpc::cut_status::next_subtest()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_09 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 9, "xpc::cut_status", "cut_status::next_subtest()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*
       * Note that the 'simulated' option is used in order better
       * distinguish test output from the real output.
       */

      xpc::cut_options x_options(XPCCUT_OPTIONS_SIMULATED);
      xpc::cut_status x_status
      (
         x_options, 97, 97,
         _("Internal Status"), "cut_status::next_subtest()"
      );
      ok = x_options.valid() && x_status.valid();
      if (ok)
         x_options.current_test(g_BOGUS_TEST_NUMBER);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Basic initialization"))
      {
         xpc::cut_white_box cwb;
         if (ok)                                   /* the result from above   */
            ok = x_status.next_subtest("mxlptlx");

         if (ok)
         {
            ok = cwb.Status_Check_Test_Options_Pointer
            (
               x_status, (void *) cwb.Options_Const_Options_Pointer(x_options)
            );
            if (ok)
               ok = cwb.Status_Check_Subtest(x_status, 1);

            if (ok)
               ok = x_status.subtest() == 1;

            if (ok)
               ok = cwb.Status_Check_Subtest_Name(x_status, "mxlptlx");

            if (ok)
               ok = x_status.subtest_name() == "mxlptlx";
         }
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Basic incrementing"))
      {
         xpc::cut_white_box cwb;
         ok = x_status.next_subtest("?");
         if (ok)
         {
            ok = cwb.Status_Check_Test_Options_Pointer
            (
               x_status, (void *) cwb.Options_Const_Options_Pointer(x_options)
            );
            if (ok)
               ok = cwb.Status_Check_Subtest(x_status, 2);

            if (ok)
               ok = x_status.subtest() == 2;

            if (ok)
               ok = cwb.Status_Check_Subtest_Name(x_status, "?");

            if (ok)
               ok = x_status.subtest_name() == "?";
         }
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Null tagname"))
      {
         try
         {
            /*
             * This call /will/ throw an exception, unlike in the
             * corresponding C unit-test in the xpccut unit-test application
             * (unit_test_test[.exe]).  Because of that exception being
             * thrown, the internal m_Subtest field never gets incremented.
             *
             * As built by Visual Studio 2010, an assertion occurs, and the
             * test application simply aborts.  :-(
             */

            xpc::cut_white_box cwb;
            ok = x_status.next_subtest(NULL_PTR_PARAM);
            if (ok)
            {
               ok = cwb.Status_Check_Test_Options_Pointer
               (
                  x_status, (void *) cwb.Options_Const_Options_Pointer(x_options)
               );
               if (ok)
                  ok = cwb.Status_Check_Subtest(x_status, 3);

               if (ok)
                  ok = x_status.subtest() == 3;

               if (ok)
                  ok = cwb.Status_Check_Subtest_Name(x_status, _("unnamed"));

               if (ok)
                  ok = x_status.subtest_name() == _("unnamed");
            }
         }
         catch (std::logic_error & exc )
         {
            /*
             * So, continuing from the comment above, we will normally get
             * to here in the unit-test.  Once we've shown the information,
             * we call a good sub_test() in order to make sure the actual
             * internal value of the m_Subtest field is correct for the next
             * sub-test, sub-test 5.
             */

            std::string s(_("std::logic_error caught: "));
            s += exc.what();
            inform(options, s);
            ok = x_status.next_subtest(_("forcing a sub-test increment"));
         }
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("--summarize behavior"))
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
         bool original_summary_setting = x_options.is_summary();

         /**
          * Turn on the --summarize option.  This option causes
          * cut_status::next_subtest() to return 'false', so that in no
          * case will a sub-test actually run.
          */

         xpc::cut_white_box cwb;
         bool not_ok = false;
         x_options.is_summary(true);
         if (ok)
            not_ok = ! x_status.next_subtest(_("summarize"));

         if (not_ok)
         {
            ok = cwb.Status_Check_Test_Options_Pointer
            (
               x_status, (void *) cwb.Options_Const_Options_Pointer(x_options)
            );
            if (ok)
               ok = cwb.Status_Check_Subtest(x_status, 4);

            if (ok)
               ok = x_status.subtest() == 4;

            if (ok)
               ok = cwb.Status_Check_Subtest_Name(x_status, _("summarize"));

            if (ok)
               ok = x_status.subtest_name() == _("summarize");
         }
         else
            xpccut_errprint(_("next_subtest() returned true in subtest 5"));

         status.pass(ok);

         /*
          * Restore the original setting of the silence flag, so that
          * the output of the unit-test will have the appearance expected
          * by the dude who ran this test.  Also restore the summarize
          * setting.
          */

         x_options.is_summary(original_summary_setting);
         if (original_silence_setting)
            xpccut_silence_printing();
         else
            xpccut_allow_printing();
      }

      /*  6 */

      if (status.next_subtest("--sub-test skip behavior"))
      {
         /*
          * Save the original sub-test value for restoration at the end of
          * this sub-test.
          */

         int original_sub_test = x_options.single_subtest();

         /*
          * Turn on the --sub-test option.
          */

         x_options.single_subtest(5);
         if (ok)
            ok = x_status.next_subtest("skip sub-test");

         if (ok)
         {
            xpc::cut_white_box cwb;
            ok = cwb.Status_Check_Test_Options_Pointer
            (
               x_status, (void *) cwb.Options_Const_Options_Pointer(x_options)
            );
            if (ok)
               ok = cwb.Status_Check_Subtest(x_status, 5);

            if (ok)
               ok = x_status.subtest() == 5;

            if (ok)
               ok = cwb.Status_Check_Subtest_Name(x_status, "skip sub-test");

            if (ok)
               ok = x_status.subtest_name() == "skip sub-test";
         }
         status.pass(ok);

         /*
          * Restore the --sub-test option.
          */

         (void) x_options.single_subtest(original_sub_test);
      }
   }
   return status;
}

/**
 *    This test demonstrates the usage of the beep facility.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   10. Prompt and response functions.
 *
 * \note
 *    This test eventually calls unit_test_status_prompt() or
 *    unit_test_status_response().  Those functions do a lot of work for us,
 *    including checking the is_failed() and is_aborted() functions.
 *
 * <b>Debugging scenario:</b> \n
 *
\verbatim
      ./cut_unit_test --interactive --group 2 --case 10
            --response-before c --response-after f
\endverbatim
 *
 * \test
 *    -  xpc::cut_status::prompt()
 *    -  xpc::cut_status::response()
 *    -  xpc::cut_status::beep() [this is a static member function]
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_10 (const xpc::cut_options & options)
{
   /**
    *    "TRACE" is a special casename, a back-door to turn on showing the
    *    values in the xpc::cut_status's unit_test_status_t object.
    */

   xpc::cut_status status
   (
      options, 2, 10, "xpc::cut_status", "TRACE"   /* "cut_status::beep()" */
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*
       * TRACE
       */

      status.trace(_("Post-constructor in cut_unit_test_02_10"));
      if (options.show_progress())
         fprintf(stdout, "  %s\n", _("This test plays a beep if interactive."));

      /*  1 */

      if (status.next_subtest("beep()"))
      {
         bool do_it = status.prompt(_("Testing the beep functionality..."));
         if (do_it)
         {
            xpc::cut_status::beep();
            (void) status.response(_("Pass this test if you heard the beep"));
         }
      }

      /*  2 */

      if (status.next_subtest("trace()"))
      {
         status.trace(_("Post-response in cut_unit_test_02_10"));
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "group-name"
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   11. group_name()
 *
 * \test
 *    -  xpc::cut_status.group_name()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 * \todo
 *    Consider using default argument values for cut_status() so that the
 *    group and case numbers can be auto-incremented if not provided, and
 *    the group-name and case-name can be recycled.
 */

static xpc::cut_status
cut_unit_test_02_11 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 11, "xpc::cut_status", "cut_status::group_name()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         if (ok)
            ok = status.group_name() == "xpc::cut_status";

         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Reassignment test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            /*
             * Now forbidden, so moved into the cut_white_box class.
             *
             * const char * group;
             * (void) strcpy(x_status.status().m_Group_Name, "privato");
             * group = unit_test_status_group_name(&(x_status.status()));
             */

            xpc::cut_white_box cwb;
            cwb.Status_Set_Group_Name(x_status, "privato");
            ok = cwb.Status_Check_Group_Name(x_status, "privato");
            if (ok)
               ok = x_status.group_name() == "privato";
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "case-name"
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   12. case_name()
 *
 * \test
 *    -  xpc::cut_status.case_name()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_12 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 12, "xpc::cut_status", "cut_status::case_name()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         std::string kase = status.case_name();
         ok = kase == "cut_status::case_name()";
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Reassignment test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            /*
             * This code gets in by the back-door.  Don't you dare do this!
             *
             * In fact, we now forbid it by making status() a \e const
             * function.
             *
             * (void) strcpy
             * (
             *    x_status.status().m_Case_Description, "locacion privato"
             * );
             * std::string kase = x_status.case_name();
             * ok = kase == "locacion privato";
             */

            xpc::cut_white_box cwb;
            cwb.Status_Set_Case_Description(x_status, "privato");
            ok = cwb.Status_Check_Case_Description(x_status, "privato");
            if (ok)
               ok = x_status.case_name() == "privato";
         }
         status.pass(ok);
      }

      /*  4 */

#if 0
      This test is useless, really.

      if (status.next_subtest("Crash test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            /*
             * This code gets in by the back-door.  Don't you dare do this!
             * Here, we're going to deliberately crash it by providing more
             * than 128 bytes.
             *
             * The size of each line is 50 characters, and there are 10
             * lines.
             */

            xpc::cut_white_box cwb;
            const char * crasher =
               "01234567890123456789012345678901234567890123456789"
#if ! RUN_TIME_ASSERTIONS
               "01234567890123456789012345678901234567890123456789"
               "01234567890123456789012345678901234567890123456789"
               "01234567890123456789012345678901234567890123456789"
               "01234567890123456789012345678901234567890123456789"
               "01234567890123456789012345678901234567890123456789"
               "01234567890123456789012345678901234567890123456789"
               "01234567890123456789012345678901234567890123456789"
               "01234567890123456789012345678901234567890123456789"
               "01234567890123456789012345678901234567890123456789"
#endif
               ;

            cwb.Status_Set_Case_Description(x_status, crasher);
            std::string kase = x_status.case_name();

            /*
             * @change ca 2012-06-09
             *    This test now returns 'true'!
             */

            ok = cwb.Status_Check_Case_Description(x_status, crasher);
            if (ok)
               ok = (kase == crasher);
         }
         status.pass(ok);
      }
#endif
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "sub-test-name"
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   13. subtest_name()
 *
 * \test
 *    -  xpc::cut_status::subtest_name()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_13 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 13, "xpc::cut_status", "cut_status::subtest_name()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         std::string subtest = status.subtest_name();
         ok = subtest == "Normal test";
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Reassignment test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Subtest_Name(x_status, "locacion privato");
            ok = cwb.Status_Check_Subtest_Name(x_status, "locacion privato");
            if (ok)
               ok = x_status.subtest_name() == "locacion privato";
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "group-number"
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   14. group()
 *
 * \test
 *    -  xpc::cut_status::group()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_14 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 14, "xpc::cut_status", "cut_status::group()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         int group = status.group();
         ok = group == 2;
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Reassignment test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Group(x_status, 96);
            ok = cwb.Status_Check_Test_Group(x_status, 96);
            if (ok)
               ok = x_status.group() == 96;
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "case-number"
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   15. kase()
 *
 * \test
 *    -  xpc::cut_status::kase()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_15 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 15, "xpc::cut_status", "cut_status::kase()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         int kase = status.kase();
         ok = kase == 15;
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Reassignment test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Case(x_status, 96);
            ok = cwb.Status_Check_Test_Case(x_status, 96);
            if (ok)
               ok = x_status.kase() == 96;
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "sub-test-number"
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   16. subtest()
 *
 * \test
 *    -  xpc::cut_status::subtest()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_16 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 16, "xpc::cut_status", "cut_status::subtest()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         int subtest = status.subtest();
         ok = subtest == 2;
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Reassignment test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Subtest(x_status, 94);
            ok = cwb.Status_Check_Subtest(x_status, 94);
            if (ok)
               ok = x_status.subtest() == 94;
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the
 *    "failed-sub-test-number" functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   17. failed_subtest()
 *
 * \test
 *    -  xpc::cut_status::failed_subtest()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_17 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 17, "xpc::cut_status", "cut_status::failed_subtest()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         int subtest = status.failed_subtest();
         ok = subtest == 0;
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Reassignment test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Failed_Subtest(x_status, 93);
            ok = cwb.Status_Check_Failed_Subtest(x_status, 93);
            if (ok)
               ok = x_status.failed_subtest() == 93;
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the
 *    "error-count-for-sub-tests" functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   18. error_count()
 *
 * \test
 *    -  xpc::cut_status::error_count()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_18 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 18, "xpc::cut_status", "cut_status::error_count()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         int errcount = status.error_count();
         ok = errcount == 0;
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Reassignment test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Subtest_Error_Count(x_status, 92);
            ok = cwb.Status_Check_Subtest_Error_Count(x_status, 92);
            if (ok)
               ok = x_status.error_count() == 92;
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "passed"
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   19. passed()
 *
 * \test
 *    -  xpc::cut_status::passed()
 *    -  xpc::cut_status::error_count()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_19 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 19, "xpc::cut_status", "cut_status::passed()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         ok = status.passed();                        /* incestuous, no?      */
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Zeroing test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Subtest_Error_Count(x_status, 0);
            ok = cwb.Status_Check_Subtest_Error_Count(x_status, 0);
            if (ok)
               ok = x_status.error_count() == 0;

            if (ok)
               ok = x_status.passed();
         }
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Reassignment test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Subtest_Error_Count(x_status, 1);
            ok = ! x_status.passed();
         }
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Negation test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Subtest_Error_Count(x_status, -1);
            ok = ! x_status.passed();
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the "failed"
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   20. failed()
 *
 * \test
 *    -  xpc::cut_status::failed()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_20 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 20, "xpc::cut_status", "cut_status::failed()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         ok = ! status.failed();                         /* incestuous, no?   */
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Zeroing test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Subtest_Error_Count(x_status, 0);
            ok = cwb.Status_Check_Subtest_Error_Count(x_status, 0);
            if (ok)
               ok = x_status.error_count() == 0;

            if (ok)
               ok = ! x_status.failed();
         }
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Reassignment test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Subtest_Error_Count(x_status, 1);
            ok = cwb.Status_Check_Subtest_Error_Count(x_status, 1);
            if (ok)
               ok = x_status.error_count() == 1;

            if (ok)
               ok = x_status.failed();
         }
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Negation test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Subtest_Error_Count(x_status, -1);
            ok = cwb.Status_Check_Subtest_Error_Count(x_status, -1);
            if (ok)
               ok = x_status.error_count() == -1;

            if (ok)
               ok = x_status.failed();
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   21. disposition()
 *
 * \test
 *    -  xpc::cut_status::disposition()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_21 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 21, "xpc::cut_status", "cut_status::disposition()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         int d = status.disposition();
         ok = d == XPCCUT_DISPOSITION_CONTINUE;
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Reassignment test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_DNT);
            int d = x_status.disposition();
            ok = d == XPCCUT_DISPOSITION_DNT;
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor for "continue" functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   22. is_continue()
 *
 * \test
 *    -  xpc::cut_status::is_continue()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_22 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 22, "xpc::cut_status", "cut_status::is_continue()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         ok = status.is_continue();
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("False test DNT"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_DNT);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_DNT;
            if (ok)
               ok = ! x_status.is_continue();
         }
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("False test FAILED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_FAILED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_FAILED;
            if (ok)
               ok = ! x_status.is_continue();
         }
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("False test QUITTED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_QUITTED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_QUITTED;
            if (ok)
               ok = ! x_status.is_continue();
         }
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("False test ABORTED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_ABORTED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_ABORTED;
            if (ok)
               ok = ! x_status.is_continue();
         }
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("False test integer"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition
            (
               x_status, unit_test_disposition_t(92)
            );
            ok = x_status.disposition() == unit_test_disposition_t(92);
            if (ok)
               ok = ! x_status.is_continue();
         }
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("True test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition
            (
               x_status, XPCCUT_DISPOSITION_CONTINUE
            );
            ok = x_status.disposition() == XPCCUT_DISPOSITION_CONTINUE;
            if (ok)
               ok = x_status.is_continue();
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor for "skipped" functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   23. is_skipped()
 *
 * \test
 *    -  xpc::cut_status::is_skipped()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_23 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 23, "xpc::cut_status", "cut_status::is_skipped()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         ok = ! status.is_skipped();
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("True test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_DNT);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_DNT;
            if (ok)
               ok = x_status.is_skipped();
         }
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("False test FAILED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_FAILED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_FAILED;
            if (ok)
               ok = ! x_status.is_skipped();
         }
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("False test QUITTED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_QUITTED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_QUITTED;
            if (ok)
               ok = ! x_status.is_skipped();
         }
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("False test ABORTED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_ABORTED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_ABORTED;
            if (ok)
               ok = ! x_status.is_skipped();
         }
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("False test integer"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition
            (
               x_status, unit_test_disposition_t(92)
            );
            ok = x_status.disposition() == unit_test_disposition_t(92);
            if (ok)
               ok = ! x_status.is_skipped();
         }
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("False test CONTINUE"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition
            (
               x_status, XPCCUT_DISPOSITION_CONTINUE
            );
            ok = x_status.disposition() == XPCCUT_DISPOSITION_CONTINUE;
            if (ok)
               ok = ! x_status.is_skipped();
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor for "failed" functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   24. is_failed()
 *
 * \test
 *    -  xpc::cut_status::is_failed()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_24 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 24, "xpc::cut_status", "cut_status::is_failed()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         ok = ! status.is_failed();
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("False test DNT"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_DNT);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_DNT;
            if (ok)
               ok = ! x_status.is_failed();
         }
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("True test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_FAILED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_FAILED;
            if (ok)
               ok = x_status.is_failed();
         }
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("False test QUITTED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_QUITTED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_QUITTED;
            if (ok)
               ok = ! x_status.is_failed();
         }
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("False test ABORTED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_ABORTED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_ABORTED;
            if (ok)
               ok = ! x_status.is_failed();
         }
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("False test integer"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition
            (
               x_status, unit_test_disposition_t(92)
            );
            ok = x_status.disposition() == unit_test_disposition_t(92);
            if (ok)
               ok = ! x_status.is_failed();
         }
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("False test CONTINUE"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition
            (
               x_status, XPCCUT_DISPOSITION_CONTINUE
            );
            ok = x_status.disposition() == XPCCUT_DISPOSITION_CONTINUE;
            if (ok)
               ok = ! x_status.is_failed();
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor for "quitted" functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   25. is_quitted()
 *
 * \test
 *    -  xpc::cut_status::is_quitted()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_25 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 25, "xpc::cut_status", "cut_status::is_quitted()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         ok = ! status.is_quitted();
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("False test DNT"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_DNT);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_DNT;
            if (ok)
               ok = ! x_status.is_quitted();
         }
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("False test FAILED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_FAILED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_FAILED;
            if (ok)
               ok = ! x_status.is_quitted();
         }
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("True test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_QUITTED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_QUITTED;
            if (ok)
               ok = x_status.is_quitted();
         }
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("False test ABORTED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_ABORTED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_ABORTED;
            if (ok)
               ok = ! x_status.is_quitted();
         }
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("False test integer"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition
            (
               x_status, unit_test_disposition_t(92)
            );
            ok = x_status.disposition() == unit_test_disposition_t(92);
            if (ok)
               ok = ! x_status.is_quitted();
         }
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("False test CONTINUE"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition
            (
               x_status, XPCCUT_DISPOSITION_CONTINUE
            );
            ok = x_status.disposition() == XPCCUT_DISPOSITION_CONTINUE;
            if (ok)
               ok = ! x_status.is_quitted();
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor for "aborted" functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   26. is_aborted()
 *
 * \test
 *    -  xpc::cut_status::is_aborted()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_26 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 26, "xpc::cut_status", "cut_status::is_aborted()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         ok = ! status.is_aborted();
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("False test DNT"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_DNT);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_DNT;
            if (ok)
               ok = ! x_status.is_aborted();
         }
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("False test FAILED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_FAILED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_FAILED;
            if (ok)
               ok = ! x_status.is_aborted();
         }
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("False test QUITTED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_QUITTED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_QUITTED;
            if (ok)
               ok = ! x_status.is_aborted();
         }
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("True test"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_ABORTED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_ABORTED;
            if (ok)
               ok = x_status.is_aborted();
         }
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("False test integer"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition
            (
               x_status, unit_test_disposition_t(92)
            );
            ok = x_status.disposition() == unit_test_disposition_t(92);
            if (ok)
               ok = ! x_status.is_aborted();
         }
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("False test CONTINUE"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition
            (
               x_status, XPCCUT_DISPOSITION_CONTINUE
            );
            ok = x_status.disposition() == XPCCUT_DISPOSITION_CONTINUE;
            if (ok)
               ok = ! x_status.is_aborted();
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the disposition
 *    accessor for "okay" functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   27. is_okay()
 *
 * \test
 *    -  xpc::cut_status::is_okay()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_27 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 27, "xpc::cut_status", "cut_status::is_okay()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         ok = status.is_okay();
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("True test DNT"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_DNT);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_DNT;
            if (ok)
               ok = x_status.is_okay();
         }
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("False test FAILED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_FAILED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_FAILED;
            if (ok)
               ok = ! x_status.is_okay();
         }
         status.pass(ok);
      }

      /*  5 */

      /**
       * \todo
       *    Still need to determine if QUITTED is an appropriate "okay"
       *    status.
       */

      if (status.next_subtest("False test QUITTED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_QUITTED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_QUITTED;
            if (ok)
               ok = ! x_status.is_okay();
         }
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("False test ABORTED"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition(x_status, XPCCUT_DISPOSITION_ABORTED);
            ok = x_status.disposition() == XPCCUT_DISPOSITION_ABORTED;
            if (ok)
               ok = ! x_status.is_okay();
         }
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("False test integer"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition
            (
               x_status, unit_test_disposition_t(92)
            );
            ok = x_status.disposition() == unit_test_disposition_t(92);
            if (ok)
               ok = ! x_status.is_okay();
         }
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("True test CONTINUE"))
      {
         xpc::cut_status x_status;
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Disposition
            (
               x_status, XPCCUT_DISPOSITION_CONTINUE
            );
            ok = x_status.disposition() == XPCCUT_DISPOSITION_CONTINUE;
            if (ok)
               ok = x_status.is_okay();
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the duration accessor
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   28. Duration-measurement.
 *
 * \test
 *    -  xpc::cut_status::duration_ms()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_28 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 28, "xpc::cut_status", "cut_status::duration_ms()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Normal test"))
      {
         double d = status.duration_ms();
         status.pass((d == 0.0));
      }

      /*  3 */

      if (status.next_subtest("Reassignment test"))
      {
         double d;
         xpc::cut_status x_status;
         d = x_status.duration_ms();
         ok = (d == 0.0);
         if (ok)
         {
            xpc::cut_white_box cwb;
            cwb.Status_Set_Test_Duration(x_status, 90.01);
            d = x_status.duration_ms();
            ok = (d == 90.01);
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the integer-checking
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   29. Integer-checking function.
 *
 * \test
 *    -  xpc::cut_status::int_check()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_29 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 29, "xpc::cut_status", "cut_status::int_check()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /**
       *    The first sub-test sets a test group/case of 99/99.  It copies
       *    the provided \e options parameter, and then modifies it slightly
       *    so that the output is less confusing to a user.  The output
       *    value is incremented internally, so "TEST 1000" is shown.
       */

      xpc::cut_options x_options = options;      /* copy existing options   */
      x_options.is_simulated(true);
      x_options.current_test(g_BOGUS_TEST_NUMBER);
      ok = x_options.current_test() == g_BOGUS_TEST_NUMBER;
      if (ok)
      {
         /*
          * ok = unit_test_status_initialize    // C code
          * (
          *    &x_status, &x_options, 99, 99,
          *    "x_status", _("internal test")
          * );
          */

         x_options.is_simulated(true);
         xpc::cut_status x_status
         (
            x_options, 99, 99, "x_status", _("internal test")
         );
      }

      /*  1 */

      if (status.next_subtest("Null 'this' part 1"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  2 */

      if (status.next_subtest("Null 'this' part 2"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass();
      }

      /*  3 */

      if (status.next_subtest("Normal test of integer match"))
      {
         xpc::cut_status x_status
         (
            x_options, 99, 99, "x_status", _("integer test")
         );
         if (ok)
            ok = x_status.int_check(0, 0);

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Normal test of integer mis-match"))
      {
         xpc::cut_status x_status
         (
            x_options, 99, 99, "x_status", _("integer test")
         );
         if (ok)
         {
            ok = ! x_status.int_check(0, 1);
            show_deliberate_failure(options);
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the integer-checking
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *   30. String-checking.
 *
 * \test
 *    -  xpc::cut_status::string_check()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_30 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 30, "xpc::cut_status", "cut_status::string_check()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      const std::string s1 = "123";
      const std::string s2 = "321";
      xpc::cut_options x_options = options;        /* copy existing options   */
      xpc::cut_status x_status
      (
         x_options, 99, 99,
         "x_status", _("internal test")
      );
      x_options.is_simulated(true);
      x_options.current_test(g_BOGUS_TEST_NUMBER);
      ok = x_options.current_test() == g_BOGUS_TEST_NUMBER;
      if (ok)
      {
         ok = x_status.valid();
         x_options.is_simulated(true);
      }

      /*  1 */

      if (status.next_subtest("Null 'this' part 1"))
      {
         inform(options, _("Null 'this' not possible in C++, check equality"));
         ok = x_status.string_check(s1, s1);
         status.pass(ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this' part 2"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! x_status.string_check(s1, s2);
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Null 'this' part 3"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! x_status.string_check("", s2);
         status.pass(null_ok);
      }

      /*  4 */

      if (status.next_subtest("Null 'this' part 4"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! x_status.string_check(s1, "");
         status.pass(null_ok);
      }

      /*  5 */

      if (status.next_subtest("Null 'this' part 5"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         ok = x_status.string_check("", "");
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("Normal test of match"))
      {
         if (ok)
            ok = x_status.string_check(s1, s1);

         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("Normal test of mis-match"))
      {
         if (ok)
         {
            ok = ! x_status.string_check(s1, s2);
            show_deliberate_failure(options);
         }
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("Normal null (empty) match"))
      {
         if (ok)
            ok = x_status.string_check("", "");

         status.pass(ok);
      }

      /*  9 */

      if (status.next_subtest("Normal null mis-match, part 1"))
      {
         if (ok)
         {
            ok = ! x_status.string_check("", s2);
            show_deliberate_failure(options);
         }
         status.pass(ok);
      }

      /* 10 */

      if (status.next_subtest("Normal null mis-match, part 2"))
      {
         if (ok)
         {
            ok = ! x_status.string_check(s1, "");
            show_deliberate_failure(options);
         }
         status.pass(ok);
      }

      /* 11 */

      if (status.next_subtest("Normal empty-string match"))
      {
         if (ok)
            ok = x_status.string_check("", "");

         status.pass(ok);
      }

      /* 12 */

      if (status.next_subtest("Null+empty-string mismatch 1"))
      {
         if (ok)
         {
            ok = ! x_status.string_check("", " ");
            show_deliberate_failure(options);
         }
         status.pass(ok);
      }

      /* 13 */

      if (status.next_subtest("Null+empty-string mismatch 2"))
      {
         if (ok)
         {
            ok = ! x_status.string_check(" ", "");
            show_deliberate_failure(options);
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the boolean-checking
 *    functionality works.
 *
 * \group
 *    2. xpc::cut_status functions.
 *
 * \case
 *    31. bool_check() 2
 *
 * \test
 *    -  xpc::cut_status::bool_check()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_02_31 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 31, "xpc::cut_status", "cut_status::bool_check() 2"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_status x_status(options, 99, 99, "x_status", _("internal test"));

      /*
       * Note that the 'simulated' option is used in order better
       * distinguish test output from the real output.
       */

      xpc::cut_options x_options(XPCCUT_OPTIONS_SIMULATED);
      ok = x_options.valid() && x_status.valid();
      if (ok)
         x_options.current_test(g_BOGUS_TEST_NUMBER);

      /*  1 */

      if (status.next_subtest("Null 'this' part 1"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         ok = x_status.bool_check(false, false);
         status.pass(ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this' part 2"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! x_status.bool_check(false, true);
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Null 'this' part 3"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! x_status.bool_check(true, false);
         status.pass(null_ok);
      }

      /*  4 */

      if (status.next_subtest("Normal test of match"))
      {
         if (ok)
            ok = x_status.bool_check(false, false);

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Normal test of mis-match"))
      {
         if (ok)
         {
            ok = ! x_status.bool_check(false, true);
            show_deliberate_failure(options);
         }
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("Normal test of mis-match"))
      {
         if (ok)
         {
            ok = ! x_status.bool_check(true, false);
            show_deliberate_failure(options);
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the basic
 *    initialization function properly zeros or clears all fields to their
 *    "default" values.
 *
 *    These default values are defined in the unit_test_options.h module in
 *    the XPC CUT library (libxpccut).
 *
 * \group
 *    3. xpc::cut_options functions.
 *
 * \case
 *    1. init().
 *
 * \test
 *    -  xpc::cut_options::init()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_01 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 1,
      "xpc::cut_options", "cut_options::init()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /**
       * Note that the 'simulated' option is used in order better
       * distinguish test output from the real output.  Also note that this
       * test would be more robust if we set all the values to the opposite
       * of their default values before calling the xpc::cut_options::init()
       * function.
       */

      xpc::cut_options x_options(XPCCUT_OPTIONS_SIMULATED);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         // x_options.is_simulated(true);
         status.pass(true);
      }

      /*  2 */

      if (status.next_subtest("Good 'this'"))
      {
         ok = x_options.init();
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("is_verbose()"))
      {
         /**
          * We're not going to bother with any apparatus for checking
          * the C internals for the cut_options class.  We did that for
          * cut_status class, and we're not sure it really adds value.
          * Maybe it does, a little, but enough is enough, time to move on!
          */

         ok = x_options.is_verbose() == XPCCUT_IS_VERBOSE;
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("show_values()"))
      {
         ok = x_options.show_values() == XPCCUT_SHOW_VALUES;
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("use_text_synch()"))
      {
         ok = x_options.use_text_synch() == XPCCUT_TEXT_SYNCH;
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("show_step_numbers()"))
      {
         ok = x_options.show_step_numbers() == XPCCUT_SHOW_STEP_NUMBERS;
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("show_progress()"))
      {
         ok = x_options.show_progress() == XPCCUT_SHOW_PROGRESS;
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("stop_on_error()"))
      {
         ok = x_options.stop_on_error() == XPCCUT_STOP_ON_ERROR;
         status.pass(ok);
      }

      /*  9 */

      if (status.next_subtest("batch_mode()"))
      {
         ok = x_options.batch_mode() == XPCCUT_BATCH_MODE;
         status.pass(ok);
      }

      /* 10 */

      if (status.next_subtest("is_interactive()"))
      {
         ok = x_options.is_interactive() == XPCCUT_INTERACTIVE;
         status.pass(ok);
      }

      /* 11 */

      if (status.next_subtest("do_beep()"))
      {
         ok = x_options.do_beep() == XPCCUT_BEEP;
         status.pass(ok);
      }

      /* 12 */

      if (status.next_subtest("is_summary()"))
      {
         ok = x_options.is_summary() == XPCCUT_SUMMARIZE;
         status.pass(ok);
      }

      /* 13 */

      if (status.next_subtest("is_pause()"))
      {
         ok = x_options.is_pause() == XPCCUT_CASE_PAUSE;
         status.pass(ok);
      }

      /* 14 */

      if (status.next_subtest("single_group()"))
      {
         ok = x_options.single_group() == XPCCUT_NO_SINGLE_GROUP;
         status.pass(ok);
      }

      /* 15 */

      if (status.next_subtest("single_case()"))
      {
         ok = x_options.single_case() == XPCCUT_NO_SINGLE_CASE;
         status.pass(ok);
      }

      /* 16 */

      if (status.next_subtest("single_subtest()"))
      {
         ok = x_options.single_subtest() == XPCCUT_NO_SINGLE_SUB_TEST;
         status.pass(ok);
      }

      /* 17 */

      if (status.next_subtest("test_sleep_time()"))
      {
         ok = x_options.test_sleep_time() == XPCCUT_TEST_SLEEP_TIME;
         status.pass(ok);
      }

      /* 18 */

      if (status.next_subtest("need_subtests()"))
      {
         ok = x_options.need_subtests() == XPCCUT_NEED_SUBTESTS;
         status.pass(ok);
      }

      /* 19 */

      if (status.next_subtest("force_failure()"))
      {
         ok = x_options.force_failure() == XPCCUT_FORCE_FAILURE;
         status.pass(ok);
      }

      /* 20 */

      if (status.next_subtest("current_test()"))
      {
         xpc::cut_status x_status;
         ok = x_options.force_failure() == XPCCUT_FORCE_FAILURE;
         if (ok)
            ok = x_status.int_check(x_options.current_test(), -1);

         status.pass(ok);
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
 *    3. xpc::cut_options functions.
 *
 * \case
 *    2. initialize().  Does not exist; this is a place-filler test to
 *       preserve the parallel to the C version of the unit-test.
 *
 * \test
 *    -  xpc::cut_options::initialize()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_02 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 2,
      "xpc::cut_options", "cut_options::initialize()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*
       * Start out with default values so that we can take the opposite tack
       * with the full initialization function.
       */

      xpc::cut_options x_options;
      ok = x_options.valid();
      if (ok)
      {
         /*
          * Now modify the options structure with the opposite values (or at
          * least different values) for every accessible field, so that the
          * resulting test is a bizarro-clone of test 03_01.
          *
          * Note that there is no wrapper for * unit_test_options_initialize(),
          * so we just call each accessor in succession.  In addition, bu
          * modify all the unparameterized values as well.
          */

            x_options.is_verbose(! XPCCUT_IS_VERBOSE);
            x_options.show_values(! XPCCUT_SHOW_VALUES);
            x_options.use_text_synch(! XPCCUT_TEXT_SYNCH);
            x_options.show_step_numbers(! XPCCUT_SHOW_STEP_NUMBERS);

         /*
          * For now, avoid calling this function here:
          *
          *   x_options.show_progress(! XPCCUT_SHOW_PROGRESS);
          *
          * This has side-effects on multiple structure members, as
          * discussed in unit_test_options_show_progress_set().
          *
          * \todo
          *   We need a separate test to test the tricky side-effects
          *   of unit_test_options_show_progress_set().
          */

            x_options.stop_on_error(! XPCCUT_STOP_ON_ERROR);

         /*
          * For now, avoid calling this function here:
          *
          *   x_options.batch_mode(! XPCCUT_BATCH_MODE);
          *
          * This has side-effects on multiple structure members, as
          * discussed in unit_test_options_batch_mode_set().
          *
          * \todo
          *   We need a separate test to test the tricky side-effects
          *   of unit_test_options_batch_mode_set().
          */

            x_options.is_interactive(! XPCCUT_INTERACTIVE);
            x_options.do_beep(! XPCCUT_BEEP);

          /*
           * For now, avoid calling this function here:
           *
           *   x_options.is_summary(! XPCCUT_SUMMARIZE);
           *
           * This has side-effects on multiple structure members, as
           * discussed in unit_test_options_is_summary_set().
           *
           * \todo
           *   We need a separate test to test the tricky side-effects
           *   of unit_test_options_is_summary_set().
           */

            x_options.is_pause(! XPCCUT_CASE_PAUSE);
            x_options.single_group(XPCCUT_NO_SINGLE_GROUP+1);
            x_options.single_case(XPCCUT_NO_SINGLE_CASE+2);
            x_options.single_subtest(XPCCUT_NO_SINGLE_SUB_TEST+3);
            x_options.test_sleep_time(XPCCUT_TEST_SLEEP_TIME+4);

         /*
          * These options are included in the parameter list of the C
          * function unit_test_options_initialize() anyway.  They
          * are set in that function nonetheless, and the settings here
          * are the \e opposite of the settings made in that function.
          */

            x_options.need_subtests(! XPCCUT_NEED_SUBTESTS);
            x_options.force_failure(! XPCCUT_FORCE_FAILURE);
            x_options.is_simulated(! false);

         /*
          * Same here, except these are string-overloads of the integer
          * versions used just above, and we cannot assign nulls like
          * unit_test_options_initialize() does.  Instead, we make up some
          * bogus strings.
          *
          * Actually, these will screw up the integer versions set above, so
          * we will not call them.
          *
          * x_options.single_group("Pooh");
          * x_options.single_case("Pooh");
          * x_options.single_subtest("Pooh");
          */

            x_options.current_test(99);

         /*
          * Same, but I discovered these functions were missing from the C++
          * wrapper!
          */

            x_options.prompt_before('q');
            x_options.prompt_after('q');
      }

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass(true);
      }

      /*  2 */

      if (status.next_subtest("Good 'this'"))
      {
         status.pass(true);
      }

      /*  3 */

      if (status.next_subtest("is_verbose()"))
      {
         /*
          * Rather than use the recent x_status.bool_check()
          * function, like we did in 03_01 above, we'll leave these tests as
          * they are.  This shows another way to do the checks, for more
          * general comparisons or tests.  We don't need no stinkin'
          * xpc::cut_status member functions to test xpc::cut_options member
          * functions!
          */

         ok = x_options.is_verbose() != XPCCUT_IS_VERBOSE;
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("show_values()"))
      {
         ok = x_options.show_values() != XPCCUT_SHOW_VALUES;
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("use_text_synch()"))
      {
         ok = x_options.use_text_synch() != XPCCUT_TEXT_SYNCH;
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("show_step_numbers()"))
      {
         ok = x_options.show_step_numbers() != XPCCUT_SHOW_STEP_NUMBERS;
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("show_progress()"))
      {
         /*
          * Note that we decided to not modify this value above (too many
          * side-effects for this test-case), so here it should have its
          * default value.
          */

         ok = x_options.show_progress() == XPCCUT_SHOW_PROGRESS;
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("stop_on_error()"))
      {
         ok = x_options.stop_on_error() != XPCCUT_STOP_ON_ERROR;
         status.pass(ok);
      }

      /*  9 */

      if (status.next_subtest("batch_mode()"))
      {
         /*
          * Note that we decided to not modify this value above (too many
          * side-effects for this test-case), so here it should have its
          * default value.
          */

         ok = x_options.batch_mode() == XPCCUT_BATCH_MODE;
         status.pass(ok);
      }

      /* 10 */

      if (status.next_subtest("is_interactive()"))
      {
         ok = x_options.is_interactive() != XPCCUT_INTERACTIVE;
         status.pass(ok);
      }

      /* 11 */

      if (status.next_subtest("do_beep()"))
      {
         ok = x_options.do_beep() != XPCCUT_BEEP;
         status.pass(ok);
      }

      /* 12 */

      if (status.next_subtest("is_summary()"))
      {
         /*
          * Note that we decided to not modify this value above (too many
          * side-effects for this test-case), so here it should have its
          * default value.
          */

         ok = x_options.is_summary() == XPCCUT_SUMMARIZE;
         status.pass(ok);
      }

      /* 13 */

      if (status.next_subtest("is_pause()"))
      {
         ok = x_options.is_pause() != XPCCUT_CASE_PAUSE;
         status.pass(ok);
      }

      /* 14 */

      if (status.next_subtest("single_group() [integer version]"))
      {
         ok = x_options.single_group() == XPCCUT_NO_SINGLE_GROUP+1;
         status.pass(ok);
      }

      /* 15 */

      if (status.next_subtest("single_case() [integer version]"))
      {
         ok = x_options.single_case() == XPCCUT_NO_SINGLE_CASE+2;
         status.pass(ok);
      }

      /* 16 */

      if (status.next_subtest("single_subtest [integer version]"))
      {
         ok = x_options.single_subtest() == XPCCUT_NO_SINGLE_SUB_TEST+3;
         status.pass(ok);
      }

      /* 17 */

      if (status.next_subtest("test_sleep_time()"))
      {
         ok = x_options.test_sleep_time() == XPCCUT_TEST_SLEEP_TIME+4;
         status.pass(ok);
      }

      /* 18 */

      if (status.next_subtest("need_subtests()"))
      {
         ok = x_options.need_subtests() != XPCCUT_NEED_SUBTESTS;
         status.pass(ok);
      }

      /* 19 */

      if (status.next_subtest("current_test()"))
      {
         ok = x_options.current_test() != XPCCUT_NO_CURRENT_TEST;
         if (ok)
            ok = x_options.current_test() == 99;

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify that the parse functionality
 *    handles turning off the options.
 *
 * \group
 *    3. xpc::cut_options functions.
 *
 * \case
 *    3. xpc::cut_options principal constructor.  There is no parse() member
 *       wrapper function provided.
 *
 * \test
 *    -  xpc::cut_options::cut_options() principal constructor.
 *
 * \note
 *    One thing we discovered (probably due to the latest version of gcc) is
 *    that using \e argv as an array of non-constant character pointers was
 *    considered deprecated.  So we declare a non-standard \e argv.
 *
\verbatim
      const char * argv [FULL_ARG_COUNT+1];
\endverbatim
 *
 *    This, of course, causes some changes to the same parameter in the C++
 *    functions using \e argv.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

#define FULL_ARG_COUNT           24

static xpc::cut_status
cut_unit_test_03_03 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 3,
      "xpc::cut_options", "cut_options::parse() part 1"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      const char * argv[FULL_ARG_COUNT + 1];

      /*
       * Start out with default values so that we can take the opposite tack
       * with the full initialization function.
       *
       * However, for xcp::cut_options, parsing is only available when the
       * object is constructed.  Things are simpler this way.
       *
       *    ok = x_options.init();
       */

      /**
       * Creates the cut_options object with the opposite values (or at
       * least different values) for every accessible field, so that the
       * resulting test is a bizarro-clone of test 03_01.
       *
       * \todo
       *    There are probable some relatively new option switches that need
       *    to be tested here.
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
      int argc = 22;                            /* 1 more than last index  */
      xpc::cut_options x_options
      (
         argc, argv,
         "Test 03.03", "version", "additionalhelp"
      );

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         status.pass(true);
      }

      /*  2 */

      if (status.next_subtest("Good 'this'"))
      {
         status.pass(true);
      }

      /*  3 */

      if (status.next_subtest("is_verbose()"))
      {
         ok = ! x_options.is_verbose();
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("show_values()"))
      {
         ok = ! x_options.show_values();
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("use_text_synch()"))
      {
         ok = ! x_options.use_text_synch();
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("show_step_numbers()"))
      {
         ok = ! x_options.show_step_numbers();
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("show_progress()"))
      {
         ok = ! x_options.show_progress();
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("stop_on_error()"))
      {
         ok = ! x_options.stop_on_error();
         status.pass(ok);
      }

      /*  9 */

      if (status.next_subtest("batch_mode()"))
      {
         ok = ! x_options.batch_mode();
         status.pass(ok);
      }

      /* 10 */

      if (status.next_subtest("is_interactive()"))
      {
         ok = ! x_options.is_interactive();
         status.pass(ok);
      }

      /* 11 */

      if (status.next_subtest("do_beep()"))
      {
         ok = ! x_options.do_beep();
         status.pass(ok);
      }

      /* 12 */

      if (status.next_subtest("is_summary()"))
      {
         ok = ! x_options.is_summary();
         status.pass(ok);
      }

      /* 13 */

      if (status.next_subtest("is_pause()"))
      {
         ok = ! x_options.is_pause();
         status.pass(ok);
      }

      /* 14 */

      if (status.next_subtest("single_group()"))
      {
         ok = x_options.single_group() == (XPCCUT_NO_SINGLE_GROUP+1);
         status.pass(ok);
      }

      /* 15 */

      if (status.next_subtest("single_case()"))
      {
         ok = x_options.single_case() == (XPCCUT_NO_SINGLE_CASE+2);
         status.pass(ok);
      }

      /* 16 */

      if (status.next_subtest("single_subtest()"))
      {
         ok = x_options.single_subtest() == (XPCCUT_NO_SINGLE_SUB_TEST+3);
         status.pass(ok);
      }

      /* 17 */

      if (status.next_subtest("test_sleep_time()"))
      {
         ok = x_options.test_sleep_time() == (XPCCUT_TEST_SLEEP_TIME+4);
         status.pass(ok);
      }

      /* 18 */

      if (status.next_subtest("need_subtests()"))
      {
         ok = ! x_options.need_subtests();
         status.pass(ok);
      }

      /* 19 */

      if (status.next_subtest("current_test()"))
      {
         ok = x_options.current_test() == -1;
         status.pass(ok);
      }

#ifdef THIS_IS_POSSIBLE

      /* 20 */

      if (status.next_subtest("Help text"))
      {
         /*
          *    "testname", "version", "additionalhelp"
          *
          * We want to test the three additional parameters.  However,
          * there's no easy way to see that the test-name got packed into
          * the help text and know that we are correct.  And we can't even
          * verify that the testname is \e somewhere in the help text, since
          * the output buffer is freed as soon as it is printed.
          *
          *    ok = x_options.help() == "testname"
          */

         status.pass(true);
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
 *    3. xpc::cut_options functions.
 *
 * \case
 *    4. More tests of parsing in the principal constructor.
 *
 * \test
 *    -  xpc::cut_options::cut_options principal constructor.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_04 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 4,
      "xpc::cut_options", "cut_options::parse() part 2"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      const char * argv[FULL_ARG_COUNT + 1];
      int argc = 1;                                /* guarantee initialized   */
      argv[0] = "<unknown>";                       /* guarantee some value    */

      /*
       * Start out with default values so that we can take the opposite tack
       * with the full initialization function.
       *
       * Not applicable.  See what happens when cut from C code and paste
       * into C++ code?
       *
       *    ok = x_options.init();
       *
       */

      /**
       * Also create the xpc::cut_options object with the opposite values
       * (or at least different values) for every accessible field, so
       * that the resulting test is a bizarro-clone of test 03_01.
       *
       * Also note that some options turn off the global output option or
       * alter other options.  We disable them by tacking on "---ignore".
       *
       * The --summarize option does this, and the output looks weird.
       *
       * \todo
       *    There are probable some relatively new option switches that
       *    need to be tested here.
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
      xpc::cut_options x_options
      (
         argc, argv, "Test 03.04", "version", "additionalhelp"
      );

      /*  1 */

      if (status.next_subtest("Null arguments"))
      {
         argv[0] = "N/A";
         argc = 1;

         /*
          * Null 'this' pointer not possible to test in C++ code.  But we
          * can test the handling of bad arguments.
          */

         xpc::cut_options x_options
         (
            0, argv, "Test 03.04", "version", "additionalhelp"
         );
         bool null_ok = ! x_options.valid();
         if (null_ok)
         {
            xpc::cut_options xx_options
            (
               argc, nullptr, "Test 03.04", "version", "additionalhelp"
            );
            null_ok = ! xx_options.valid();
            if (! null_ok)
            {
               xpccut_errprint(_("did not detect a null 'argv' value"));
            }
         }
         else
         {
            xpccut_errprint(_("did not detect a zero 'argc' value"));
         }
         status.pass(null_ok);
      }

      /*  2 */

      if (ok)
      {
         ok = x_options.valid();
         status.pass(ok);
      }
      if (status.next_subtest("Good 'this'"))
      {
         status.pass(true);
      }

      /*  3 */

      if (status.next_subtest("is_verbose()"))
      {
         ok = x_options.is_verbose();
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("show_values()"))
      {
         ok = x_options.show_values();
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("use_text_synch()"))
      {
         ok = x_options.use_text_synch();
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("show_step_numbers()"))
      {
         ok = x_options.show_step_numbers();
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("show_progress()"))
      {
         ok = x_options.show_progress();
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("stop_on_error()"))
      {
         ok = x_options.stop_on_error();
         status.pass(ok);
      }

      /*  9 */

      if (status.next_subtest("batch_mode()"))
      {
         ok = ! x_options.batch_mode();
         status.pass(ok);
      }

      /* 10 */

      if (status.next_subtest("is_interactive()"))
      {
         ok = ! x_options.is_interactive();
         status.pass(ok);
      }

      /* 11 */

      if (status.next_subtest("do_beep()"))
      {
         ok = x_options.do_beep();
         status.pass(ok);
      }

      /* 12 */

      if (status.next_subtest("is_summary()"))
      {
         ok = ! x_options.is_summary();
         status.pass(ok);
      }

      /* 13 */

      if (status.next_subtest("is_pause()"))
      {
         ok = x_options.is_pause();
         status.pass(ok);
      }

      /* 14 */

      if (status.next_subtest("single_group()"))
      {
         ok = x_options.single_group() == (XPCCUT_NO_SINGLE_GROUP+1);
         status.pass(ok);
      }

      /* 15 */

      if (status.next_subtest("single_case()"))
      {
         ok = x_options.single_case() == (XPCCUT_NO_SINGLE_CASE+2);
         status.pass(ok);
      }

      /* 16 */

      if (status.next_subtest("single_subtest()"))
      {
         ok = x_options.single_subtest() == (XPCCUT_NO_SINGLE_SUB_TEST+3);
         status.pass(ok);
      }

      /* 17 */

      if (status.next_subtest("test_sleep_time()"))
      {
         ok = x_options.test_sleep_time() == (XPCCUT_TEST_SLEEP_TIME+4);
         status.pass(ok);
      }

      /* 18 */

      if (status.next_subtest("need_subtests()"))
      {
         ok = ! x_options.need_subtests();
         status.pass(ok);
      }

      /* 18 */

      if (status.next_subtest("current_test()"))
      {
         ok = x_options.current_test() == -1;
         status.pass(ok);
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
 *    3. xpc::cut_options functions.
 *
 * \case
 *    5. More parsing tests (part 3).
 *
 * \test
 *    -  xpc::cut_options::cut_options() principal constructor
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

static xpc::cut_status
cut_unit_test_03_05 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 5,
      "xpc::cut_options", "cut_options::parse() part 3"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      bool original_silence_setting;
      const char * argv[FULL_ARG_COUNT + 1];
      int argc = 1;                                /* guarantee initialized   */
      argv[0] = "<unknown>";                       /* guarantee some value    */

      /**
       * Obtain the silence-setting that was specified on the command
       * line, so that it can be restored at the end of some of the
       * sub-tests.  If the user didn't specify --summarize, this value
       * should be false by default.
       */

      original_silence_setting = bool(xpccut_is_silent());

      /*  1 */

      /*
       * Start out with default values so that we can take the opposite tack
       * with the full initialization function.
       */

      ok = x_options.init();
      if (status.next_subtest("--batch-mode test"))
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

            x_options.is_interactive(true);
            x_options.show_step_numbers(true);
            x_options.show_values(true);
            x_options.is_verbose(true);
            x_options.is_summary(false);
            x_options.need_subtests(false);

            /*
             * Now run a simulated command-line.
             *
             * However, at this time, there is no wrapper for
             * unit_test_options_parse(), so this test is commented out.
             *
             * argv[0]  = "unit_test_test";
             * argv[1]  = "--batch-mode";
             * argv[2]  = "--interactive---ignore";
             * argv[3] = "--summarize---ignore";
             * argc = 4;
             * ok = x_options.parse
             * (
             *    &x_options, argc, argv,
             *    "03.05", "version", "additionalhelp"
             * );
             *
             */
         }

         /*
          * This stilted style makes it easy to catch an error under a
          * debugger.
          */

         if (ok)
            ok = x_options.is_interactive();

         if (ok)
            ok = x_options.show_step_numbers();

         if (ok)
            ok = x_options.show_values();

         if (ok)
            ok = x_options.is_verbose();

         if (ok)
            ok = ! x_options.is_summary();

         if (ok)
            ok = ! x_options.need_subtests();

         status.pass(ok);
      }

      /*  2 */

      if (ok)
         ok = x_options.init();

      if (status.next_subtest("--interactive re-test (disabled)"))
      {
         /*
          * Again, we cannot parse a command-line into xpc::cut_options
          * after the construction has been done, so this test is disabled.
          *
          * if (ok)
          * {
          *    x_options.m_Interactive = false;
          *    argv[0]  = "unit_test_test";
          *    argv[1]  = "--batch-mode--ignore";     // affects other options
          *    argv[2]  = "--interactive";
          *    argv[3]  = "--summarize---ignore";     // affects other options
          *    argc = 4;
          *    ok = unit_test_options_parse
          *    (
          *       &x_options, argc, argv,
          *       "03.05", "version", "additionalhelp"
          *    );
          * }
          * if (ok)
          *    ok = x_options.m_Interactive;
          *
          * if (ok)
          *    ok = ! x_options.m_Show_Step_Numbers;
          *
          * if (ok)
          *    ok = ! x_options.m_Show_Values;
          *
          * if (ok)
          *    ok = ! x_options.m_Is_Verbose;
          *
          * if (ok)
          *    ok = ! x_options.m_Summarize;
          *
          * if (ok)
          *    ok = ! x_options.m_Need_Subtests;
          *
          * status.pass(ok);
          *
          */
      }

      /*  3 */

      if(ok)
         ok = x_options.init();

      if (status.next_subtest("--summarize test"))
      {
         if (ok)
         {
            x_options.is_interactive(true);
            x_options.is_pause(true);

            /*
             * is_summary() now turns off interactive and case-pause modes.
             */

            x_options.is_summary(true);

            /*
             * argv[0]  = "unit_test_test";
             * argv[1]  = "--batch-mode---ignore";
             * argv[2]  = "--interactive---ignore";
             * argv[3]  = "--summarize";           // affects other options
             * argc = 4;
             * ok = unit_test_options_parse
             * (
             *    &x_options, argc, argv,
             *    "03.05", "version", "additionalhelp"
             * );
             */
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
            ok = ! x_options.is_interactive();

         if (ok)
            ok = ! x_options.show_step_numbers();

         if (ok)
            ok = ! x_options.show_values();

         if (ok)
            ok = ! x_options.is_verbose();

         if (ok)
            ok = ! x_options.is_pause();

         if (ok)
            ok = x_options.is_summary();

         if (ok)
            ok = ! x_options.need_subtests();

         status.pass(ok);
      }

      /*  3 */

      /*
       * if (ok)
       *    ok = x_options.init();
       */

      if (status.next_subtest("--version test"))
      {
         bool null_ok;
         xpccut_silence_printing();
         argv[0]  = "unit_test_test";
         argv[1]  = "--version";
         argc = 2;
         xpc::cut_options xx_options
         (
            argc, argv, "03.05", nullptr, "additionalhelp"
         );
         null_ok = ! xx_options.valid();

         /*
          * if (null_ok)
          * {
          *    null_ok = ! unit_test_options_parse
          *    (
          *       &x_options, argc, argv,
          *       "03.05", "version", "additionalhelp"
          *    );
          * }
          */

         /*
          * Restore the original setting of the silence flag.
          */

         if (original_silence_setting)
            xpccut_silence_printing();
         else
            xpccut_allow_printing();

         status.pass(null_ok);
      }

      /*  4 */

      /*
       * if (ok)
       *    ok = x_options.init();
       */

      if (status.next_subtest("--help test, null help-text"))
      {
         bool null_ok;
         xpccut_silence_printing();
         argv[0]  = "unit_test_test";
         argv[1]  = "--help";
         argc = 2;
         xpc::cut_options xx_options
         (
            argc, argv, "03.05", "version", nullptr
         );
         null_ok = ! xx_options.valid();
         status.pass(null_ok);

         /*
          * Restore the original setting of the silence flag.
          */

         if (original_silence_setting)
            xpccut_silence_printing();
         else
            xpccut_allow_printing();
      }

      /*  5 */

      if (status.next_subtest("--help test, null test-name-text"))
      {
         bool null_ok;
         xpccut_silence_printing();
         xpc::cut_options xx_options
         (
            argc, argv, nullptr, "version", "additionalhelp"
         );
         null_ok = ! xx_options.valid();
         status.pass(null_ok);

         /*
          * Restore the original setting of the silence flag.
          */

         if (original_silence_setting)
            xpccut_silence_printing();
         else
            xpccut_allow_printing();
      }

      /*  6 */

      if (status.next_subtest("--help test, null test-name-text"))
      {
         bool null_ok;
         xpccut_silence_printing();
         xpc::cut_options xx_options
         (
            argc, argv,
            "03.05", "version", "additionalhelp"
         );
         null_ok = ! xx_options.valid();
         status.pass(null_ok);

         /*
          * Restore the original setting of the silence flag.
          */

         if (original_silence_setting)
            xpccut_silence_printing();
         else
            xpccut_allow_printing();
      }
   }
   return status;
}

#if 0

/**
 *    Provides a unit/regression test to verify the "is-verbose"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::is_verbose_set()
 *    -  xpc::cut_options::is_verbose()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_06 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 6,
      "xpc::cut_options", "cut_options::is_verbose...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_is_verbose_set(nullptr, false);
         if (null_ok)
            null_ok = ! unit_test_options_is_verbose_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            unit_test_options_is_verbose(nullptr) == XPCCUT_IS_VERBOSE
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_verbose_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Is_Verbose;

         if (ok)
            ok = ! x_options.is_verbose();

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_verbose_set(&x_options, true);

         if (ok)
            ok = x_options.m_Is_Verbose;

         if (ok)
            ok = x_options.is_verbose();

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "show-values"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::show_values_set()
 *    -  xpc::cut_options::show_values()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_07 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 7,
      "xpc::cut_options", "cut_options::is_show_values...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_show_values_set(nullptr, false);
         if (null_ok)
            null_ok = ! unit_test_options_show_values_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            unit_test_options_show_values(nullptr) == XPCCUT_SHOW_VALUES
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_show_values_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Show_Values;

         if (ok)
            ok = ! x_options.show_values();

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_show_values_set(&x_options, true);

         if (ok)
            ok = x_options.m_Show_Values;

         if (ok)
            ok = x_options.show_values();

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "text-synch"
 *    functionality.
 *
 * \note
 *    Although the xpc::cut_options.m_Text_Synch variable is supported,
 *    the actual usage of a synchronization object has not yet been added.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::use_text_synch_set()
 *    -  xpc::cut_options::use_text_synch()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_08 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 8,
      "xpc::cut_options", "cut_options::use_text_synch...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_use_text_synch_set(nullptr, false);
         if (null_ok)
            null_ok = ! unit_test_options_use_text_synch_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
         inform(options, _("A null 'this' is not possible in C++ code"));
      {
         bool null_ok =
         (
            unit_test_options_use_text_synch(nullptr) == XPCCUT_TEXT_SYNCH
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_use_text_synch_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Text_Synch;

         if (ok)
            ok = ! x_options.use_text_synch();

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_use_text_synch_set(&x_options, true);

         if (ok)
            ok = x_options.m_Text_Synch;

         if (ok)
            ok = x_options.use_text_synch();

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "step-numbers"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::show_step_numbers_set()
 *    -  xpc::cut_options::show_step_numbers()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 */

static xpc::cut_status
cut_unit_test_03_09 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 9,
      "xpc::cut_options", "cut_options::show_step_numbers...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_show_step_numbers_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_show_step_numbers_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         /*
          * Let's do this a different way, just to show another way to do
          * it.
 *
          * bool null_ok =
          * (
          *    unit_test_options_show_step_numbers(nullptr) ==
          *       XPCCUT_SHOW_STEP_NUMBERS
          * );
          * status.pass(null_ok);
          */

         (void) x_status.bool_check
         (
            &status, unit_test_options_show_step_numbers(nullptr),
            XPCCUT_SHOW_STEP_NUMBERS
         );
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_show_step_numbers_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Show_Step_Numbers;

         if (ok)
            ok = ! x_options.show_step_numbers();

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_show_step_numbers_set(&x_options, true);

         if (ok)
            ok = x_options.m_Show_Step_Numbers;

         if (ok)
            ok = x_options.show_step_numbers();

         status.pass(ok);
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
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::show_progress_set()
 *    -  xpc::cut_options::show_progress()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_10 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 10,
      "xpc::cut_options", "cut_options::show_progress...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_show_progress_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_show_progress_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok;
         bool match = XPCCUT_SHOW_PROGRESS;

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
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_show_progress_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Show_Progress;

         if (ok)
            ok = ! x_options.show_progress();

         /*
          * These settings are side-effects.
          */

         if (ok)
            ok = ! x_options.m_Show_Step_Numbers;

         if (ok)
            ok = ! x_options.m_Show_Values;

         if (ok)
            ok = ! x_options.m_Is_Verbose;

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_show_progress_set(&x_options, true);

         if (ok)
            ok = x_options.m_Show_Progress;

         /*
          * These settings are side-effects, but the values are not changed
          * when setting the flag to true.
          */

         if (ok)
            ok = ! x_options.m_Show_Step_Numbers;

         if (ok)
            ok = ! x_options.m_Show_Values;

         if (ok)
            ok = ! x_options.m_Is_Verbose;

         if (ok)
         {
            /*
             * This may be unwise, but the show-progress function also
             * checks the --silent option internally.  At any rate, we have
             * to account for that.
             *
             * \change ca 04/23/2008
             *    No, this is no longer true.  We have to ignore this
             *    sub-sub-test.
             *
             * if (xpccut_is_silent())
             *    ok = ! x_options.show_progress();
             * else
             *    ok = x_options.show_progress();
             */
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "stop-on-error"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::stop_on_error_set()
 *    -  xpc::cut_options::stop_on_error()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_11 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 11,
      "xpc::cut_options", "cut_options::stop_on_error...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_stop_on_error_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_stop_on_error_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            unit_test_options_stop_on_error(nullptr) == XPCCUT_STOP_ON_ERROR
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_stop_on_error_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Stop_On_Error;

         if (ok)
            ok = ! x_options.stop_on_error();

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_stop_on_error_set(&x_options, true);

         if (ok)
            ok = x_options.m_Stop_On_Error;

         if (ok)
            ok = x_options.stop_on_error();

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "batch-mode"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::batch_mode_set()
 *    -  xpc::cut_options::batch_mode()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_12 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 12,
      "xpc::cut_options", "cut_options::batch_mode...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_batch_mode_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_batch_mode_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            unit_test_options_batch_mode(nullptr) == XPCCUT_BATCH_MODE
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_batch_mode_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Batch_Mode;

         if (ok)
            ok = ! x_options.batch_mode();

         /*
          * These settings are side-effects.
          */

         if (ok)
            ok = ! x_options.m_Show_Step_Numbers;

         if (ok)
            ok = ! x_options.m_Show_Values;

         if (ok)
            ok = ! x_options.m_Is_Verbose;

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_batch_mode_set(&x_options, true);

         if (ok)
            ok = x_options.m_Batch_Mode;

         if (ok)
            ok = x_options.batch_mode();

         /*
          * These settings are side-effects, but the values are not changed
          * when setting the flag to true.
          */

         if (ok)
            ok = ! x_options.m_Show_Step_Numbers;

         if (ok)
            ok = ! x_options.m_Show_Values;

         if (ok)
            ok = ! x_options.m_Is_Verbose;

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "interactive"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::is_interactive_set()
 *    -  xpc::cut_options::is_interactive()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_13 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 13,
      "xpc::cut_options", "cut_options::is_interactive...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_is_interactive_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_is_interactive_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            unit_test_options_is_interactive(nullptr) == XPCCUT_INTERACTIVE
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_interactive_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Interactive;

         if (ok)
            ok = ! x_options.is_interactive();

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_interactive_set(&x_options, true);

         if (ok)
            ok = x_options.m_Interactive;

         if (ok)
            ok = x_options.is_interactive();

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "beep"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::do_beep_set()
 *    -  xpc::cut_options::do_beep()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_14 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 14,
      "xpc::cut_options", "cut_options::do_beep...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_do_beep_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_do_beep_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            unit_test_options_do_beep(nullptr) == XPCCUT_BEEP
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_do_beep_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Beep_Prompt;

         if (ok)
            ok = ! x_options.do_beep();

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_do_beep_set(&x_options, true);

         if (ok)
            ok = x_options.m_Beep_Prompt;

         if (ok)
            ok = x_options.do_beep();

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "summarize"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::is_summary_set()
 *    -  xpc::cut_options::is_summary()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_15 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 15,
      "xpc::cut_options", "cut_options::is_summary...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_is_summary_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_is_summary_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            unit_test_options_is_summary(nullptr) == XPCCUT_SUMMARIZE
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_summary_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Summarize;

         if (ok)
            ok = ! x_options.is_summary();

         /*
          * The following settings are side-effects.
          */

         if (ok)
            ok = ! x_options.m_Interactive;

         if (ok)
            ok = ! x_options.m_Case_Pause;

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_summary_set(&x_options, true);

         if (ok)
            ok = x_options.m_Summarize;

         if (ok)
            ok = x_options.is_summary();

         /*
          * The following settings are side-effects that do not get undone
          * by setting the flag to true.
          */

         if (ok)
            ok = ! x_options.m_Interactive;

         if (ok)
            ok = ! x_options.m_Case_Pause;

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "pause"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::is_pause_set()
 *    -  xpc::cut_options::is_pause()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_16 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 16,
      "xpc::cut_options", "cut_options::is_pause...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_is_pause_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_is_pause_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            unit_test_options_is_pause(nullptr) == XPCCUT_CASE_PAUSE
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_pause_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Case_Pause;

         if (ok)
            ok = ! x_options.is_pause();

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_is_pause_set(&x_options, true);

         if (ok)
            ok = x_options.m_Case_Pause;

         if (ok)
            ok = x_options.is_pause();

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "test group"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_optionsest_group_set()
 *    -  xpc::cut_optionsest_group()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_17 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 17,
      "xpc::cut_options", _("xpc::cut_optionsest_group...()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! xpc::cut_optionsest_group_set(nullptr, 0);
         if (null_ok)
            null_ok = ! xpc::cut_optionsest_group_set(nullptr, 1);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         /*
          * Let's do this another way, just to show it.
          *
          * bool null_ok =
          * (
          *    xpc::cut_optionsest_group(nullptr) == XPCCUT_NO_SINGLE_GROUP
          * );
          * status.pass(null_ok);
          */

         (void) x_status.int_check
         (
            &status, xpc::cut_optionsest_group(nullptr),
            XPCCUT_NO_SINGLE_GROUP
         );
      }

      /*  3 */

      if (status.next_subtest("Setting negative, set/get"))
      {
         /*
          * Set explicitly to make sure the correct value replaces this one.
          */

         x_options.m_Single_Test_Group = 5;
         ok = ! xpc::cut_optionsest_group_set(&x_options, -1);

         /*
          * Let's do this another way, just to see how it looks.
          *
          * if (ok)
          *    ok = x_options.m_Single_Test_Group == 0;
          *
          * if (ok)
          *    ok = xpc::cut_optionsest_group(&x_options) == 0;
          */

         if (ok)
         {
            ok = x_status.int_check
            (
               &status, x_options.m_Single_Test_Group, 0
            );
         }
         if (ok)
         {
            ok = x_status.int_check
            (
               &status, xpc::cut_optionsest_group(&x_options), 0
            );
         }

         /*
          * Not needed, since we're using x_status.int_check(),
          * which calls this function under the covers:
          *
          * status.pass(ok);
          */
      }

      /*  4 */

      if (status.next_subtest("Setting to 0, set/get"))
      {
         x_options.m_Single_Test_Group = 5;
         if (ok)
            ok = xpc::cut_optionsest_group_set(&x_options, 0);

         if (ok)
            ok = x_options.m_Single_Test_Group == 0;

         if (ok)
            ok = xpc::cut_optionsest_group(&x_options) == 0;

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Setting to 1, set/get"))
      {
         x_options.m_Single_Test_Group = 5;
         if (ok)
            ok = xpc::cut_optionsest_group_set(&x_options, 1);

         if (ok)
            ok = x_options.m_Single_Test_Group == 1;

         if (ok)
            ok = xpc::cut_optionsest_group(&x_options) == 1;

         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("Setting to 101, set/get"))
      {
         x_options.m_Single_Test_Group = 5;
         if (ok)
            ok = ! xpc::cut_optionsest_group_set(&x_options, 101);

         if (ok)
            ok = x_options.m_Single_Test_Group == 0;

         if (ok)
            ok = xpc::cut_optionsest_group(&x_options) == 0;

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "test case"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_optionsest_case_set()
 *    -  xpc::cut_optionsest_case()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_18 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 18,
      "xpc::cut_options", _("xpc::cut_optionsest_case...()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! xpc::cut_optionsest_case_set(nullptr, 0);
         if (null_ok)
            null_ok = ! xpc::cut_optionsest_case_set(nullptr, 1);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            xpc::cut_optionsest_case(nullptr) == XPCCUT_NO_SINGLE_CASE
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting negative, set/get"))
      {
         /*
          * Set explicitly to make sure the correct value replaces this one.
          */

         x_options.m_Single_Test_Case = 5;
         ok = ! xpc::cut_optionsest_case_set(&x_options, -1);
         if (ok)
            ok = x_options.m_Single_Test_Case == 0;

         if (ok)
            ok = xpc::cut_optionsest_case(&x_options) == 0;

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting to 0, set/get"))
      {
         x_options.m_Single_Test_Case = 5;
         if (ok)
            ok = xpc::cut_optionsest_case_set(&x_options, 0);

         if (ok)
            ok = x_options.m_Single_Test_Case == 0;

         if (ok)
            ok = xpc::cut_optionsest_case(&x_options) == 0;

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Setting to 1, set/get"))
      {
         x_options.m_Single_Test_Case = 5;
         if (ok)
            ok = xpc::cut_optionsest_case_set(&x_options, 1);

         /*
          * This is the first way to do this check:
          */

         if (ok)
            ok = x_options.m_Single_Test_Case == 1;

         /*
          * This is another way to do this check:
          */

         if (ok)
         {
            ok = x_status.int_check
            (
               &status, x_options.m_Single_Test_Case, 1
            );
         }

         /*
          * This is yet another way to do this check.
          */

         if (ok)
            ok = xpc::cut_optionsest_case(&x_options) == 1;

         /*
          * This is yet another way to do this check.  Four different ways
          * to do the same check!
          */

         if (ok)
         {
            ok = x_status.int_check
            (
               &status, xpc::cut_optionsest_case(&x_options), 1
            );
         }

         /*
          * If all the checks above used x_status.int_check(), this
          * line of code would be redundant.
          */

         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("Setting to 101, set/get"))
      {
         x_options.m_Single_Test_Case = 5;
         if (ok)
            ok = ! xpc::cut_optionsest_case_set(&x_options, 101);

         if (ok)
            ok = x_options.m_Single_Test_Case == 0;

         if (ok)
            ok = xpc::cut_optionsest_case(&x_options) == 0;

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "subtest"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::single_subtest_set()
 *    -  xpc::cut_options::single_subtest()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_19 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 19,
      "xpc::cut_options", "cut_options::single_subtest...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_single_subtest_set(nullptr, 0);
         if (null_ok)
            null_ok = ! unit_test_options_single_subtest_set(nullptr, 1);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            unit_test_options_single_subtest(nullptr) == XPCCUT_NO_SINGLE_SUB_TEST
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting negative, set/get"))
      {
         /*
          * Set explicitly to make sure the correct value replaces this one.
          */

         x_options.m_Single_Sub_Test = 5;
         ok = ! unit_test_options_single_subtest_set(&x_options, -1);
         if (ok)
            ok = x_options.m_Single_Sub_Test == 0;

         if (ok)
            ok = x_options.single_subtest() == 0;

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting to 0, set/get"))
      {
         x_options.m_Single_Sub_Test = 5;
         if (ok)
            ok = unit_test_options_single_subtest_set(&x_options, 0);

         if (ok)
            ok = x_options.m_Single_Sub_Test == 0;

         if (ok)
            ok = x_options.single_subtest() == 0;

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Setting to 1, set/get"))
      {
         x_options.m_Single_Sub_Test = 5;
         if (ok)
            ok = unit_test_options_single_subtest_set(&x_options, 1);

         if (ok)
            ok = x_options.m_Single_Sub_Test == 1;

         if (ok)
            ok = x_options.single_subtest() == 1;

         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("Setting to 1001, set/get"))
      {
         x_options.m_Single_Sub_Test = 5;
         if (ok)
            ok = ! unit_test_options_single_subtest_set(&x_options, 1001);

         if (ok)
            ok = x_options.m_Single_Sub_Test == 0;

         if (ok)
            ok = x_options.single_subtest() == 0;

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "test sleep-time"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_optionsest_sleep_time_set()
 *    -  xpc::cut_optionsest_sleep_time()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_20 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 20,
      "xpc::cut_options", _("xpc::cut_optionsest_sleep_time...()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! xpc::cut_optionsest_sleep_time_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! xpc::cut_optionsest_sleep_time_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            xpc::cut_optionsest_sleep_time(nullptr) == XPCCUT_TEST_SLEEP_TIME
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting negative, set/get"))
      {
         /*
          * Set explicitly to make sure the correct value replaces this one.
          */

         x_options.m_Test_Sleep_Time = 5;
         ok = ! xpc::cut_optionsest_sleep_time_set(&x_options, -1);
         if (ok)
            ok = x_options.m_Test_Sleep_Time == 0;

         if (ok)
            ok = xpc::cut_optionsest_sleep_time(&x_options) == 0;

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting to 0, set/get"))
      {
         x_options.m_Test_Sleep_Time = 5;
         if (ok)
            ok = xpc::cut_optionsest_sleep_time_set(&x_options, 0);

         if (ok)
            ok = x_options.m_Test_Sleep_Time == 0;

         if (ok)
            ok = xpc::cut_optionsest_sleep_time(&x_options) == 0;

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Setting to 1, set/get"))
      {
         x_options.m_Test_Sleep_Time = 5;
         if (ok)
            ok = xpc::cut_optionsest_sleep_time_set(&x_options, 1);

         if (ok)
            ok = x_options.m_Test_Sleep_Time == 1;

         if (ok)
            ok = xpc::cut_optionsest_sleep_time(&x_options) == 1;

         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("Setting to 3600001, set/get"))
      {
         x_options.m_Test_Sleep_Time = 5;
         if (ok)
            ok = ! xpc::cut_optionsest_sleep_time_set(&x_options, 3600001);

         if (ok)
            ok = x_options.m_Test_Sleep_Time == 0;

         if (ok)
            ok = xpc::cut_optionsest_sleep_time(&x_options) == 0;

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "named test group"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::named_group_set()
 *    -  xpc::cut_options::named_group()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_21 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 21,
      "xpc::cut_options", "cut_options::named_group...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_named_group_set(nullptr, nullptr);
         if (null_ok)
            null_ok = ! unit_test_options_named_group_set(nullptr, "!");

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = cut_is_nullptr(unit_test_options_named_group(nullptr));
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting to null, set/get"))
      {
         /*
          * Set explicitly to make sure the value is not modified.  We don't
          * bother making sure a 3-character string can be copied here.
          */

         strcpy(x_options.m_Single_Test_Group_Name, "!!!");
         ok = ! unit_test_options_named_group_set(&x_options, nullptr);
         if (ok)
            ok = strcmp(x_options.m_Single_Test_Group_Name, "!!!") == 0;

         if (ok)
            ok = strcmp(x_options.named_group(), "!!!") == 0;

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting to empty, set/get"))
      {
         strcpy(x_options.m_Single_Test_Group_Name, "!!!");
         if (ok)
            ok = unit_test_options_named_group_set(&x_options, "");

         if (ok)
            ok = strlen(x_options.m_Single_Test_Group_Name) == 0;

         if (ok)
            ok = cut_is_nullptr(x_options.named_group());

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Valid setting, set/get"))
      {
         strcpy(x_options.m_Single_Test_Group_Name, "!!!!!");
         if (ok)
            ok = unit_test_options_named_group_set(&x_options, "0xDEADBEEF");

         if (ok)
            ok = strcmp(x_options.m_Single_Test_Group_Name, "0xDEADBEEF") == 0;

         if (ok)
         {
            ok = 0 == strcmp
            (
               x_options.named_group(), "0xDEADBEEF"
            );
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "named test case"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::named_case_set()
 *    -  xpc::cut_options::named_case()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_22 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 22,
      "xpc::cut_options", "cut_options::named_case...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_named_case_set(nullptr, nullptr);
         if (null_ok)
            null_ok = ! unit_test_options_named_case_set(nullptr, "!");

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = cut_is_nullptr(unit_test_options_named_case(nullptr));
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting to null, set/get"))
      {
         /*
          * Set explicitly to make sure the value is not modified.  We don't
          * bother making sure a 3-character string can be copied here.
          */

         strcpy(x_options.m_Single_Test_Case_Name, "!!!");
         ok = ! unit_test_options_named_case_set(&x_options, nullptr);
         if (ok)
            ok = strcmp(x_options.m_Single_Test_Case_Name, "!!!") == 0;

         if (ok)
            ok = strcmp(x_options.named_case(), "!!!") == 0;

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting to empty, set/get"))
      {
         strcpy(x_options.m_Single_Test_Case_Name, "!!!");
         if (ok)
            ok = unit_test_options_named_case_set(&x_options, "");

         if (ok)
            ok = strlen(x_options.m_Single_Test_Case_Name) == 0;

         if (ok)
            ok = cut_is_nullptr(x_options.named_case());

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Valid setting, set/get"))
      {
         strcpy(x_options.m_Single_Test_Case_Name, "!!!!!");
         if (ok)
            ok = unit_test_options_named_case_set(&x_options, "0xDEADBEEF");

         if (ok)
            ok = strcmp(x_options.m_Single_Test_Case_Name, "0xDEADBEEF") == 0;

         if (ok)
         {
            ok = 0 == strcmp
            (
               x_options.named_case(), "0xDEADBEEF"
            );
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "named test sub-test"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::named_subtest_set()
 *    -  xpc::cut_options::named_subtest()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_23 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 23,
      "xpc::cut_options", "cut_options::named_subtest...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_named_subtest_set
         (
            nullptr, nullptr
         );
         if (null_ok)
            null_ok = ! unit_test_options_named_subtest_set(nullptr, "!");

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = cut_is_nullptr(unit_test_options_named_subtest(nullptr));
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting to null, set/get"))
      {
         /*
          * Set explicitly to make sure the value is not modified.  We don't
          * bother making sure a 3-character string can be copied here.
          */

         strcpy(x_options.m_Single_Sub_Test_Name, "!!!");
         ok = ! unit_test_options_named_subtest_set(&x_options, nullptr);
         if (ok)
            ok = strcmp(x_options.m_Single_Sub_Test_Name, "!!!") == 0;

         if (ok)
            ok = strcmp(x_options.named_subtest(), "!!!")
                     == 0;

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting to empty, set/get"))
      {
         strcpy(x_options.m_Single_Sub_Test_Name, "!!!");
         if (ok)
            ok = unit_test_options_named_subtest_set(&x_options, "");

         if (ok)
            ok = strlen(x_options.m_Single_Sub_Test_Name) == 0;

         if (ok)
            ok = cut_is_nullptr(x_options.named_subtest());

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Valid setting, set/get"))
      {
         strcpy(x_options.m_Single_Sub_Test_Name, "!!!!!");
         if (ok)
            ok = unit_test_options_named_subtest_set(&x_options, "0xDEADBEEF");

         if (ok)
            ok = strcmp(x_options.m_Single_Sub_Test_Name, "0xDEADBEEF") == 0;

         if (ok)
         {
            ok = 0 == strcmp
            (
               x_options.named_subtest(), "0xDEADBEEF"
            );
         }
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "require-sub-tests"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::need_subtests_set()
 *    -  xpc::cut_options::need_subtests()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_24 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 24,
      "xpc::cut_options", "cut_options::need_subtests...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_need_subtests_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_need_subtests_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            unit_test_options_need_subtests(nullptr) == XPCCUT_NEED_SUBTESTS
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_need_subtests_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Need_Subtests;

         if (ok)
            ok = ! x_options.need_subtests();

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_need_subtests_set(&x_options, true);

         if (ok)
            ok = x_options.m_Need_Subtests;

         if (ok)
            ok = x_options.need_subtests();

         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the "force-failure"
 *    functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::force_failure_set()
 *    -  xpc::cut_options::force_failure()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_25 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 25,
      "xpc::cut_options", "cut_options::force_failure...()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this', set"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_options_force_failure_set
         (
            nullptr, false
         );
         if (null_ok)
            null_ok = ! unit_test_options_force_failure_set(nullptr, true);

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this', get"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok =
         (
            unit_test_options_force_failure(nullptr) == XPCCUT_FORCE_FAILURE
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Setting false, set/get"))
      {
         if (ok)
            ok = unit_test_options_force_failure_set(&x_options, false);

         if (ok)
            ok = ! x_options.m_Force_Failure;

         if (ok)
            ok = ! x_options.force_failure();

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Setting true, set/get"))
      {
         if (ok)
            ok = unit_test_options_force_failure_set(&x_options, true);

         if (ok)
            ok = x_options.m_Force_Failure;

         if (ok)
            ok = x_options.force_failure();

         status.pass(ok);
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
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::current_test()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_26 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 26, "xpc::cut_options", "cut_options::current_test()"
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = unit_test_options_current_test(nullptr) == -1;
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("get"))
      {
         ok =  x_options.current_test() == -1;
         status.pass(ok);
      }
   }
   return status;
}

/**
 *    Provides a unit/regression test to verify the
 *    "prompt-before/response-before" functionality.
 *
 * \group
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::prompt_before_set()
 *    -  xpc::cut_options::prompt_before()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_27 (const xpc::cut_options & options)
{
   xpc::cut_status status;
   (
      options, 3, 27, "xpc::cut_options", "unit_test_options_prompt_before()"
   );
   if (ok)
   {
      xpc::cut_options x_options;

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool not_ok = ! unit_test_options_prompt_before_set(nullptr, 0);
         if (not_ok)
            not_ok = unit_test_options_prompt_before(nullptr) == 0;

         unit_test_status_pass(&status, not_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Zero value"))
      {
         ok = unit_test_options_prompt_before_set(&x_options, 0);
         if (ok)
            ok = x_options.prompt_before() == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "'c' value"))
      {
         /*
          * Note how the upper-case value is mapped to lower-case when
          * stored, for easier usage in the unit-test library.
          */

         ok = unit_test_options_prompt_before_set(&x_options, 'c');
         if (ok)
            ok = x_options.prompt_before() == 'c';

         if (ok)
            ok = unit_test_options_prompt_before_set(&x_options, 'C');

         if (ok)
            ok = x_options.prompt_before() == 'c';

         if (ok)
            ok = x_options.m_Response_Before = 'c';

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "'s' value"))
      {
         ok = unit_test_options_prompt_before_set(&x_options, 's');
         if (ok)
            ok = x_options.prompt_before() == 's';

         if (ok)
            ok = unit_test_options_prompt_before_set(&x_options, 'S');

         if (ok)
            ok = x_options.prompt_before() == 's';

         if (ok)
            ok = x_options.m_Response_Before = 's';

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "'a' value"))
      {
         ok = unit_test_options_prompt_before_set(&x_options, 'a');
         if (ok)
            ok = x_options.prompt_before() == 'a';

         if (ok)
            ok = unit_test_options_prompt_before_set(&x_options, 'A');

         if (ok)
            ok = x_options.prompt_before() == 'a';

         if (ok)
            ok = x_options.m_Response_Before = 'a';

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "'q' value"))
      {
         ok = unit_test_options_prompt_before_set(&x_options, 'q');
         if (ok)
            ok = x_options.prompt_before() == 'q';

         if (ok)
            ok = unit_test_options_prompt_before_set(&x_options, 'Q');

         if (ok)
            ok = x_options.prompt_before() == 'q';

         if (ok)
            ok = x_options.m_Response_Before = 'q';

         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Unsupported value"))
      {
         if (ok)
         {
            bool not_ok = ! unit_test_options_prompt_before_set
            (
               &x_options, '1'    /* '1' is not a valid response character  */
            );

            /*
             * The value from the previous sub-test should still be in
             * force.
             */

            if (not_ok)
               not_ok = x_options.prompt_before() == 'q';

            if (not_ok)
               not_ok = x_options.m_Response_Before = 'q';

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
 *
 *    xpc::cut_options functions.
 *
 * \test
 *    -  xpc::cut_options::prompt_after_set()
 *    -  xpc::cut_options::prompt_after()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_03_28 (const xpc::cut_options & options)
{
   xpc::cut_status status;
   (
      options, 3, 28, "xpc::cut_options", "unit_test_options_prompt_after()"
   );
   if (ok)
   {
      xpc::cut_options x_options;
      ok = x_options.init();

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool not_ok = ! unit_test_options_prompt_after_set(nullptr, 0);
         if (not_ok)
            not_ok = unit_test_options_prompt_after(nullptr) == 0;

         unit_test_status_pass(&status, not_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Zero value"))
      {
         ok = unit_test_options_prompt_after_set(&x_options, 0);
         if (ok)
            ok = x_options.prompt_after() == 0;

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "'p' value"))
      {
         /*
          * Note how the upper-case value is mapped to lower-case when
          * stored, for easier usage in the unit-test library.
          */

         ok = unit_test_options_prompt_after_set(&x_options, 'p');
         if (ok)
            ok = x_options.prompt_after() == 'p';

         if (ok)
            ok = unit_test_options_prompt_after_set(&x_options, 'P');

         if (ok)
            ok = x_options.prompt_after() == 'p';

         if (ok)
            ok = x_options.m_Response_After = 'p';

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "'f' value"))
      {
         ok = unit_test_options_prompt_after_set(&x_options, 'f');
         if (ok)
            ok = x_options.prompt_after() == 'f';

         if (ok)
            ok = unit_test_options_prompt_after_set(&x_options, 'F');

         if (ok)
            ok = x_options.prompt_after() == 'f';

         if (ok)
            ok = x_options.m_Response_After = 'f';

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "'a' value"))
      {
         ok = unit_test_options_prompt_after_set(&x_options, 'a');
         if (ok)
            ok = x_options.prompt_after() == 'a';

         if (ok)
            ok = unit_test_options_prompt_after_set(&x_options, 'A');

         if (ok)
            ok = x_options.prompt_after() == 'a';

         if (ok)
            ok = x_options.m_Response_After = 'a';

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "'q' value"))
      {
         ok = unit_test_options_prompt_after_set(&x_options, 'q');
         if (ok)
            ok = x_options.prompt_after() == 'q';

         if (ok)
            ok = unit_test_options_prompt_after_set(&x_options, 'Q');

         if (ok)
            ok = x_options.prompt_after() == 'q';

         if (ok)
            ok = x_options.m_Response_After = 'q';

         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Unsupported value"))
      {
         if (ok)
         {
            bool not_ok = ! unit_test_options_prompt_after_set
            (
               &x_options, '1'    /* '1' is not a valid response character  */
            );

            /*
             * The value from the previous sub-test should still be in
             * force.
             */

            if (not_ok)
               not_ok = x_options.prompt_after() == 'q';

            if (not_ok)
               not_ok = x_options.m_Response_After = 'q';

            unit_test_status_pass(&status, not_ok);
         }
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
 *
 *    unit_test_t functions.
 *
 * \test
 *    -  unit_test_init()
 *    -  unit_test_clear() [indirect test of static function]
 *    -  xpc::cut_options::init() [indirect test of static function]
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

static xpc::cut_status
cut_unit_test_04_01 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 1,
      _("unit_test_t"), _("unit_test_init()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      (void) unit_test_init(&x_test_x);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_init(nullptr);
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Good 'this'"))
      {
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("m_App_Options"))
      {
         ok = default_options_check
         (
            &x_test_x.m_App_Options,      /* check unit_test_options_init()   */
            false                         /* no show-progress testing         */
         );
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Good help setup"))
      {
         /*
          * This sub-test will check the call to the static function
          * unit_test_allocate_help().
          */

         ok = cut_not_nullptr(x_test_x.m_Additional_Help);
         if (ok)
         {
            ok = strlen(x_test_x.m_Additional_Help) > 0;
            if (ok && options.is_verbose())
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

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Good cases setup"))
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
               ok = x_test_x.m_Current_Test_Number == -1;

            if (ok)
               ok = x_test_x.m_Test_Count == 0;

            if (ok)
               ok = x_test_x.m_Allocation_Count == XPCCUT_CASE_ALLOCATION;

            if (ok && options.is_verbose())
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

         status.pass(ok);
      }

      /*
       * Now destroy all allocated resources in the test object.  Even
       * little ol' unit_test_init() allocates them.  If the --leak-check
       * option is in force, we don't call this "destructor".
       */

      if (! gs_do_leak_check)
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_02 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 2,
      _("unit_test_t"), _("unit_test_initialize() part 1")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
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

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_initialize
         (
            nullptr, argc, argv, "x", "y", "z"
         );
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Good 'this'"))
      {
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("internal m_Is_Verbose"))
      {
         ok = ! x_test_x.m_App_Options.m_Is_Verbose;
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("internal m_Show_Values"))
      {
         ok = ! x_test_x.m_App_Options.m_Show_Values;
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("internal m_Text_Synch"))
      {
         ok = ! x_test_x.m_App_Options.m_Text_Synch;
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("internal m_Show_Step_Numbers"))
      {
         ok = ! x_test_x.m_App_Options.m_Show_Step_Numbers;
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("internal m_Show_Progress"))
      {
         ok = ! x_test_x.m_App_Options.m_Show_Progress;
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("internal m_Stop_On_Error"))
      {
         ok = ! x_test_x.m_App_Options.m_Stop_On_Error;
         status.pass(ok);
      }

      /*  9 */

      if (status.next_subtest("internal m_Batch_Mode"))
      {
         ok = ! x_test_x.m_App_Options.m_Batch_Mode;
         status.pass(ok);
      }

      /* 10 */

      if (status.next_subtest("internal m_Interactive"))
      {
         ok = ! x_test_x.m_App_Options.m_Interactive;
         status.pass(ok);
      }

      /* 11 */

      if (status.next_subtest("internal m_Beep_Prompt"))
      {
         ok = ! x_test_x.m_App_Options.m_Beep_Prompt;
         status.pass(ok);
      }

      /* 12 */

      if (status.next_subtest("internal m_Summarize"))
      {
         ok = ! x_test_x.m_App_Options.m_Summarize;
         status.pass(ok);
      }

      /* 13 */

      if (status.next_subtest("internal m_Case_Pause"))
      {
         ok = ! x_test_x.m_App_Options.m_Case_Pause;
         status.pass(ok);
      }

      /* 14 */

      if (status.next_subtest("internal m_Single_Test_Group"))
      {
         ok = x_test_x.m_App_Options.m_Single_Test_Group ==
            (XPCCUT_NO_SINGLE_GROUP+1);

         status.pass(ok);
      }

      /* 15 */

      if (status.next_subtest("internal m_Single_Test_Case"))
      {
         ok = x_test_x.m_App_Options.m_Single_Test_Case ==
            (XPCCUT_NO_SINGLE_CASE+2);

         status.pass(ok);
      }

      /* 16 */

      if (status.next_subtest("internal m_Single_Sub_Test"))
      {
         ok = x_test_x.m_App_Options.m_Single_Sub_Test ==
            (XPCCUT_NO_SINGLE_SUB_TEST+3);

         status.pass(ok);
      }

      /* 17 */

      if (status.next_subtest("internal m_Test_Sleep_Time"))
      {
         ok = x_test_x.m_App_Options.m_Test_Sleep_Time ==
            (XPCCUT_TEST_SLEEP_TIME+4);

         status.pass(ok);
      }

      /* 18 */

      if (status.next_subtest("m_Test_Application_Name"))
      {
         /* "Test 04.02", "version", "additionalhelp" */

         ok = strcmp(x_test_x.m_Test_Application_Name, "Test 04.02") == 0;
         status.pass(ok);
      }

      /* 19 */

      if (status.next_subtest("m_Test_Application_Version"))
      {
         ok = strcmp(x_test_x.m_Test_Application_Version, "version") == 0;
         status.pass(ok);
      }

      /* 20 */

      if (status.next_subtest("m_Additional_Help"))
      {
         ok = strcmp(x_test_x.m_Additional_Help, "additionalhelp") == 0;
         status.pass(ok);
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_03 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 3,
      _("unit_test_t"), _("unit_test_initialize() part 2")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
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

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_initialize
         (
            nullptr, argc, argv, "x", "y", "z"
         );
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Good 'this'"))
      {
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("internal m_Is_Verbose"))
      {
         ok = x_test_x.m_App_Options.m_Is_Verbose;
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("internal m_Show_Values"))
      {
         ok = x_test_x.m_App_Options.m_Show_Values;
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("internal m_Text_Synch"))
      {
         ok = x_test_x.m_App_Options.m_Text_Synch;
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("internal m_Show_Step_Numbers"))
      {
         ok = x_test_x.m_App_Options.m_Show_Step_Numbers;
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("internal m_Show_Progress"))
      {
         ok = x_test_x.m_App_Options.m_Show_Progress;
         status.pass(ok);
      }

      /*  8 */

      if (status.next_subtest("internal m_Stop_On_Error"))
      {
         ok = x_test_x.m_App_Options.m_Stop_On_Error;
         status.pass(ok);
      }

      /*  9 */

      if (status.next_subtest("internal m_Batch_Mode"))
      {
         ok = ! x_test_x.m_App_Options.m_Batch_Mode;
         status.pass(ok);
      }

      /* 10 */

      if (status.next_subtest("internal m_Interactive"))
      {
         ok = ! x_test_x.m_App_Options.m_Interactive;
         status.pass(ok);
      }

      /* 11 */

      if (status.next_subtest("internal m_Beep_Prompt"))
      {
         ok = x_test_x.m_App_Options.m_Beep_Prompt;
         status.pass(ok);
      }

      /* 12 */

      if (status.next_subtest("internal m_Summarize"))
      {
         ok = ! x_test_x.m_App_Options.m_Summarize;
         status.pass(ok);
      }

      /* 13 */

      if (status.next_subtest("internal m_Case_Pause"))
      {
         ok = x_test_x.m_App_Options.m_Case_Pause;
         status.pass(ok);
      }

      /* 14 */

      if (status.next_subtest("internal m_Single_Test_Group"))
      {
         ok = x_test_x.m_App_Options.m_Single_Test_Group ==
            (XPCCUT_NO_SINGLE_GROUP+1);

         status.pass(ok);
      }

      /* 15 */

      if (status.next_subtest("internal m_Single_Test_Case"))
      {
         ok = x_test_x.m_App_Options.m_Single_Test_Case ==
            (XPCCUT_NO_SINGLE_CASE+2);

         status.pass(ok);
      }

      /* 16 */

      if (status.next_subtest("internal m_Single_Sub_Test"))
      {
         ok = x_test_x.m_App_Options.m_Single_Sub_Test ==
            (XPCCUT_NO_SINGLE_SUB_TEST+3);

         status.pass(ok);
      }

      /* 17 */

      if (status.next_subtest("internal m_Test_Sleep_Time"))
      {
         ok = x_test_x.m_App_Options.m_Test_Sleep_Time ==
            (XPCCUT_TEST_SLEEP_TIME+4);

         status.pass(ok);
      }

      /* 18 */

      if (status.next_subtest("m_Test_Application_Name"))
      {
         /* "", "", "" */

         ok = strcmp(x_test_x.m_Test_Application_Name, "") == 0;
         status.pass(ok);
      }

      /* 19 */

      if (status.next_subtest("m_Test_Application_Version"))
      {
         ok = strcmp(x_test_x.m_Test_Application_Version, "") == 0;
         status.pass(ok);
      }

      /* 20 */

      if (status.next_subtest("m_Additional_Help"))
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
         status.pass(ok);
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_04 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 4,
      _("unit_test_t"), _("unit_test_initialize() part 3")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
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

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_initialize
         (
            nullptr, argc, argv, "x", "y", "z"
         );
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Good 'this'"))
      {
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("internal m_Is_Verbose"))
      {
         ok = x_test_x.m_App_Options.m_Is_Verbose;
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("m_Test_Application_Name"))
      {
         /* nullptr, nullptr, nullptr */

         ok = strcmp(x_test_x.m_Test_Application_Name, _("Unit Test")) == 0;
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("m_Test_Application_Version"))
      {
         ok = strcmp(x_test_x.m_Test_Application_Version, "1.0") == 0;
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("m_Additional_Help"))
      {
         ok = strcmp(x_test_x.m_Additional_Help, _("No help")) == 0;
         status.pass(ok);
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_05 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 5,
      _("unit_test_t"), _("unit_test_load()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_load(nullptr, cut_unit_test_01_01);
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null test-pointer"))
      {
         bool null_ok = ! unit_test_load(&x_test_x, nullptr);
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Good 'this'"))
      {
         if (ok)
            ok = unit_test_load(&x_test_x, cut_unit_test_01_01);

         if (ok)
            ok = x_test_x.m_Test_Count == 1;

         if (ok)
            ok = x_test_x.m_Allocation_Count == XPCCUT_CASE_ALLOCATION;

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Second test"))
      {
         if (ok)
            ok = unit_test_load(&x_test_x, cut_unit_test_01_01);

         if (ok)
            ok = x_test_x.m_Test_Count == 2;

         if (ok)
            ok = x_test_x.m_Allocation_Count == XPCCUT_CASE_ALLOCATION;

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Until next allocation"))
      {
         if (ok)
         {
            int ti;
            for (ti = x_test_x.m_Test_Count; ti < XPCCUT_CASE_ALLOCATION; ti++)
            {
               if (ok)
                  ok = unit_test_load(&x_test_x, cut_unit_test_01_01);

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
            status.pass(ok);
         }
      }

      /*  6 */

      if (status.next_subtest("After next allocation"))
      {
         if (ok)
         {
            int ti;
            for (ti = x_test_x.m_Test_Count; ti < 2*XPCCUT_CASE_ALLOCATION; ti++)
            {
               if (ok)
                  ok = unit_test_load(&x_test_x, cut_unit_test_01_01);

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
            status.pass(ok);
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_06 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 6,
      _("unit_test_t"), _("unit_test_cpp_load_count()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_cpp_load_count(nullptr);
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Zero verifaction"))
      {
         if (ok)
            ok = x_test_x.m_Test_Count == 0;
      }

      /*  3 */

      if (status.next_subtest("One verifaction"))
      {
         if (ok)
            ok = unit_test_cpp_load_count(&x_test_x);

         if (ok)
            ok = x_test_x.m_Test_Count == 1;
      }

      /*  4 */

      if (status.next_subtest("1000 verifaction"))
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_07 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 7,
      _("unit_test_t"), _("unit_test_dispose()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      xpc::cut_status x_status;

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = unit_test_dispose(nullptr);
         if (ok)
            ok = x_status.status().m_Test_Disposition == XPCCUT_DISPOSITION_ABORTED;

         status.pass(ok && null_ok);
      }

      /*  2 */

      if (status.next_subtest("init verifaction"))
      {
         if (ok)
            ok = x_status.status().m_Test_Disposition == XPCCUT_DISPOSITION_ABORTED;

         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("CONTINUE"))
      {
         bool quit;
         x_status.status().m_Test_Disposition = XPCCUT_DISPOSITION_CONTINUE;
         quit = x_status.dispose();
         ok = ! quit && x_status.passed();
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("DNT"))
      {
         bool quit;
         x_status.status().m_Test_Result = true;
         x_status.status().m_Subtest_Error_Count = 0;
         x_status.status().m_Test_Disposition = XPCCUT_DISPOSITION_DNT;
         quit = x_status.dispose();
         ok = ! quit && x_status.passed();
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("FAILED"))
      {
         bool quit;
         x_status.status().m_Test_Result = true;
         x_status.status().m_Subtest_Error_Count = 0;
         x_status.status().m_Test_Disposition = XPCCUT_DISPOSITION_FAILED;
         quit = x_status.dispose();
         show_deliberate_failure(options);
         ok = ! quit && ! x_status.passed();
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("QUITTED"))
      {
         bool quit;
         x_status.status().m_Test_Result = true;
         x_status.status().m_Subtest_Error_Count = 0;
         x_status.status().m_Test_Disposition = XPCCUT_DISPOSITION_QUITTED;
         quit = x_status.dispose();
         ok = quit && x_status.passed();
         status.pass(ok);
      }

      /*  7 */

      if (status.next_subtest("ABORTED"))
      {
         bool quit;
         x_status.status().m_Test_Result = true;
         x_status.status().m_Subtest_Error_Count = 0;
         x_status.status().m_Test_Disposition = XPCCUT_DISPOSITION_ABORTED;
         quit = x_status.dispose();
         show_deliberate_failure(options);
         ok = quit && ! x_status.passed();
         status.pass(ok);
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_08 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 8,
      _("unit_test_t"), _("unit_test_count()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;

      // ok = unit_test_init(&x_test_x);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = unit_test_count(nullptr) == 0;
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Zero verifaction"))
      {
         if (ok)
            ok = unit_test_count(&x_test_x) == 0;
      }

      /*  3 */

      if (status.next_subtest("One verifaction"))
      {
         if (ok)
            ok = unit_test_cpp_load_count(&x_test_x);

         if (ok)
            ok = unit_test_count(&x_test_x) == 1;
      }

      /*  4 */

      if (status.next_subtest("1000 verifaction"))
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
 *    unit_test_next_test() function:  cut_unit_test_04_10().
 *
 * \group
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_09 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 9,
      _("unit_test_t"), _("unit_test_number()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = unit_test_number(nullptr) == -1;
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_number(&x_test_x) == -1;
      }

      /*  3 */

      if (status.next_subtest("Zero verifaction"))
      {
         x_test_x.m_Current_Test_Number = 0;
         if (ok)
            ok = unit_test_number(&x_test_x) == 0;
      }

      /*  4 */

      if (status.next_subtest("One verifaction"))
      {
         x_test_x.m_Current_Test_Number = 1;
         if (ok)
            ok = unit_test_number(&x_test_x) == 1;
      }

      /*  5 */

      if (status.next_subtest("1000 verifaction"))
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_10 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 10,
      _("unit_test_t"), _("unit_test_next_test()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
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

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = unit_test_next_test(nullptr) == -1;

         /*
          * Now verify that the m_Current_Test_Number field is not modified
          * in this situation.  How could it be?!
          */

         if (null_ok)
            null_ok = x_test_x.m_Current_Test_Number == -1;

         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.  Already done in another test, but
          * if it gets broken, it is convenient to see it here, too, and we
          * don't want to keep testing stuff that is already broken based on
          * a bad result here.
          */

         if (ok)
            ok = unit_test_number(&x_test_x) == -1;
      }

      /*  3 */

      if (status.next_subtest("No tests loaded yet"))
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
            ok = test == -1;
            if (ok)
               ok = x_test_x.m_Current_Test_Number == 0;
         }
      }

      /*  4 */

      if (status.next_subtest("Still no tests loaded"))
      {
         if (ok)
         {
            int test = unit_test_next_test(&x_test_x);
            ok = test == -1;
            if (ok)
               ok = x_test_x.m_Current_Test_Number == 1;
         }
      }

      /*  5 */

      if (status.next_subtest("Load 3 tests"))
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
               ok = unit_test_load(&x_test_x, cut_unit_test_01_01);

            if (ok)
               ok = unit_test_load(&x_test_x, cut_unit_test_01_01);

            if (ok)
               ok = unit_test_load(&x_test_x, cut_unit_test_01_01);

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
               ok = test == -1;
            }
         }
      }

      /*  6 */

      if (status.next_subtest("1000 fake tests loop"))
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

      if (status.next_subtest("1000 fake while loop"))
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_11 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 11,
      _("unit_test_t"), _("unit_test_subtest_count()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = unit_test_subtest_count(nullptr) == 0;
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_subtest_count(&x_test_x) == 0;
      }

      /*  3 */

      if (status.next_subtest("Zero verifaction"))
      {
         x_test_x.m_Subtest_Count = 0;
         if (ok)
            ok = unit_test_subtest_count(&x_test_x) == 0;
      }

      /*  4 */

      if (status.next_subtest("One verifaction"))
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_12 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 12,
      _("unit_test_t"), _("unit_test_failures()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = unit_test_failures(nullptr) == -1;
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_failures(&x_test_x) == 0;
      }

      /*  3 */

      if (status.next_subtest("Zero verifaction"))
      {
         x_test_x.m_Total_Errors = 0;
         if (ok)
            ok = unit_test_failures(&x_test_x) == 0;
      }

      /*  4 */

      if (status.next_subtest("One verifaction"))
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_13 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 13,
      _("unit_test_t"), _("unit_test_first_failed_test()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = unit_test_first_failed_test(nullptr) == -1;
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_first_failed_test(&x_test_x) == 0;
      }

      /*  3 */

      if (status.next_subtest("Zero verifaction"))
      {
         x_test_x.m_First_Failed_Test = 0;
         if (ok)
            ok = unit_test_first_failed_test(&x_test_x) == 0;
      }

      /*  4 */

      if (status.next_subtest("One verifaction"))
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_14 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 14,
      _("unit_test_t"), _("unit_test_first_failed_group()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = unit_test_first_failed_group(nullptr) == -1;
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_first_failed_group(&x_test_x) == 0;
      }

      /*  3 */

      if (status.next_subtest("Zero verifaction"))
      {
         x_test_x.m_First_Failed_Group = 0;
         if (ok)
            ok = unit_test_first_failed_group(&x_test_x) == 0;
      }

      /*  4 */

      if (status.next_subtest("One verifaction"))
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_15 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 15,
      _("unit_test_t"), _("unit_test_first_failed_case()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = unit_test_first_failed_case(nullptr) == -1;
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_first_failed_case(&x_test_x) == 0;
      }

      /*  3 */

      if (status.next_subtest("Zero verifaction"))
      {
         x_test_x.m_First_Failed_Case = 0;
         if (ok)
            ok = unit_test_first_failed_case(&x_test_x) == 0;
      }

      /*  4 */

      if (status.next_subtest("One verifaction"))
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_16 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 16,
      _("unit_test_t"), _("unit_test_first_failed_subtest()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = unit_test_first_failed_subtest(nullptr) == -1;
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Initialization verifaction"))
      {
         /*
          * This sub-test provides additional verification that the field is
          * properly initialized to -1.
          */

         if (ok)
            ok = unit_test_first_failed_subtest(&x_test_x) == 0;
      }

      /*  3 */

      if (status.next_subtest("Zero verifaction"))
      {
         x_test_x.m_First_Failed_Subtest = 0;
         if (ok)
            ok = unit_test_first_failed_subtest(&x_test_x) == 0;
      }

      /*  4 */

      if (status.next_subtest("One verifaction"))
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_17 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 17,
      _("unit_test_t"), _("unit_test_run_init()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      ok = unit_test_init(&x_test_x);

      /*
       * Reduce the amount of output.
       */

      ok = unit_test_options_show_progress_set(&x_test_x.m_App_Options, false);

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = unit_test_run_init(nullptr) == 0;
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Initialization verifaction"))
      {
         if (ok)
         {
            ok = x_test_x.m_Start_Time_us.tv_sec == 0;
            if (ok)
               ok = x_test_x.m_Start_Time_us.tv_sec == 0;
         }
      }

      /*  3 */

      if (status.next_subtest("Initialization verifaction"))
      {
         x_test_x.m_Test_Count = 10;
         if (ok)
            ok = unit_test_run_init(&x_test_x) == 10;

         if (ok)
            ok = x_test_x.m_Current_Test_Number  == -1;

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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_18 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 18,
      _("unit_test_t"), _("unit_test_dispose_of_test()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      xpc::cut_options x_options;
      xpc::cut_status x_status;
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
         x_options = x_test_x.m_App_Options;     /* copy fake test options  */
         ok = unit_test_status_initialize
         (
            &x_status, &x_options, 91, 1, _("Test 04.18"), _("Disposal")
         );
      }

      /*  1 */

      if (status.next_subtest("Null 'this'"))
      {
         /**
          * Recall that unit_test_dispose_of_test() returns 'true' only if
          * the caller should cause the unit-test application to quit.
          */

         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_dispose_of_test(nullptr, nullptr, nullptr);
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Partial init verification"))
      {
         if (ok)
         {
            ok = x_test_x.m_Start_Time_us.tv_sec == 0;
            if (ok)
               ok = x_test_x.m_Start_Time_us.tv_sec == 0;

            if (ok)
               ok = xpc::cut_optionsest_sleep_time(&x_test_x.m_App_Options)
                     == 10;
         }
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Initialization verifaction"))
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
            ok = x_test_x.m_Current_Test_Number == -1;

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

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Reinitialize"))
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
            ok = x_test_x.m_Current_Test_Number == -1;

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

         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Stop-on-error test, passed"))
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
            x_options = x_test_x.m_App_Options;        /* fake options      */
            ok = unit_test_status_initialize
            (
               &x_status, &x_options, 92, 2,
               _("Test 04.18"), _("Disposal")
            );
            if (ok)
            {
               bool runresult;
               ok = ! unit_test_dispose_of_test          /* false == no quit  */
               (
                  &x_test_x, &x_status, &runresult
               );
               if (ok)
                  ok = runresult;                        /* made it pass      */
            }
         }
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("Stop-on-error test, no quit"))
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
            x_options = x_test_x.m_App_Options;     /* fake test options    */
            ok = unit_test_status_initialize
            (
               &x_status, &x_options, 92, 2,
               _("Test 04.18"), _("Disposal")
            );
            if (ok)
               ok = x_status.fail();

            if (ok)
            {
               bool runresult;
               ok = unit_test_dispose_of_test               /* true == quit   */
               (
                  &x_test_x, &x_status, &runresult
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
               ok = x_test_x.m_Current_Test_Number == -1;   /* weird but true */

            if (ok)
               ok = x_test_x.m_Total_Errors == 1;

            if (ok)
               ok = x_test_x.m_First_Failed_Test == -1;     /* weird but true */

            if (ok)
               ok = x_test_x.m_First_Failed_Group == 92;

            if (ok)
               ok = x_test_x.m_First_Failed_Case == 2;

            if (ok)
               ok = x_test_x.m_First_Failed_Subtest == 0;   /* weird by true  */
         }
         status.pass(ok);
      }

      /*
       * Now destroy all allocated resources in the test object.
       */

      unit_test_destroy(&x_test_x);
   }
   return status;
}

/**
 *    Provides a fake test to use in cut_unit_test_04_19().
 *    This function does not call unit_test_status_next_subtest().
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
fake_unit_test_04_19 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 19,
      _("Unit-test fake run"), _("unit_test_run()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
      status.pass(true);                    /* fake a pass */

   return status;
}

/**
 *    Provides a fake test to use in cut_unit_test_04_19() to test the
 *    --force-failure option.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
fake_unit_test_04_19_force_fail (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 19,
      _("Unit-test force-fail run"), _("unit_test_run()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      bool result = ! unit_test_options_force_failure(options);
      status.pass(result);                  /* fake a fail */
   }
   return status;
}

/**
 *    Provides a fake test to use in cut_unit_test_04_19().
 *    This function does call unit_test_status_next_subtest().
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
fake_subtest_unit_test_04_19 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 19,
      _("Unit-test fake run"), _("unit_test_run()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (status.next_subtest("Do-nothing test 1"))
         status.pass(true);
   }
   if (ok)
   {
      if (status.next_subtest("Do-nothing test 2"))
         status.pass(true);
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
 * \group
 *
 *    unit_test_t functions.
 *
 * \test
 *    -  unit_test_run()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_04_19 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 19,
      _("unit_test_t"), _("unit_test_run()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
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

      if (status.next_subtest("Null 'this'"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_run(nullptr);
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Empty test list"))
      {
         /*
          * This test insures (indirectly) that unit_test_run() calls
          * unit_test_run_init() to set up the test.
          */

         if (ok)
            ok = ! unit_test_run(&x_test_x);

         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("No sub-test test part 1"))
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

         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("No sub-test test part 2"))
      {
         /*
          * Tests that not having sub-test properly fails if they are
          * required.  It temporarily modifies this option to force it on.
          * This loaded test should cause unit_test_run() to fail.
          */

         bool force_failure = unit_test_options_force_failure
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
         status.pass(ok);
      }

      /*  5 */

      if (status.next_subtest("Stop-on-error test, passed"))
      {
         xpc::cut_options x_options;
         xpc::cut_status x_status;
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
            x_options = x_test_x.m_App_Options;        /* fake options      */
            ok = unit_test_status_initialize
            (
               &x_status, &x_options, 92, 2,
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
         status.pass(ok);
      }

      /*  6 */

      if (status.next_subtest("Stop-on-error test, no quit"))
      {
         xpc::cut_options x_options;
         xpc::cut_status x_status;
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
            x_options = x_test_x.m_App_Options;     /* fake test options    */
            ok = unit_test_status_initialize
            (
               &x_status, &x_options, 92, 2,
               _("Test 04.18"), _("Disposal")
            );
            if (ok)
               ok = x_status.fail();

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
         status.pass(ok);
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_20 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 20,
      _("unit_test_t"), _("unit_test_run_a_test_before()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
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

      if (status.next_subtest("Null 'this' 1"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_run_a_test_before
         (
            nullptr, (intptr_t) nullptr
         );
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this' 2"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_run_a_test_before
         (
            nullptr, (intptr_t) fake_unit_test_04_19
         );
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Null test-pointer"))
      {
         bool null_ok = ! unit_test_run_a_test_before
         (
            &x_test_x, (intptr_t) nullptr
         );
         status.pass(null_ok);
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_21 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 21,
      _("unit_test_t"), _("unit_test_run_a_test_after()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      xpc::cut_status x_status;
      xpc::cut_options x_options;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 2;
      argv[0] = "unit_test_test";
      argv[1] = "--no-show-progress";
      ok = unit_test_initialize
      (
         &x_test_x, argc, argv, "Test 04.20.1", "version", "additionalhelp"
      );
      if (ok)
         x_options = x_test_x.m_App_Options;

      if (ok)
      {
         ok = unit_test_status_initialize
         (
            &x_status, &x_options, 90, 3, _("Test 04.21"), _("Disposal")
         );
      }

      /*  1 */

      if (status.next_subtest("Null 'this' 1"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_run_a_test_after(nullptr, nullptr);
         status.pass(null_ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this' 2"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_run_a_test_after(nullptr, &x_status);
         status.pass(null_ok);
      }

      /*  3 */

      if (status.next_subtest("Null status-pointer"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         bool null_ok = ! unit_test_run_a_test_after(&x_test_x, nullptr);
         status.pass(null_ok);
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_22 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 22,
      _("unit_test_t"), _("unit_test_run_a_test()")
   );
   bool ok = status.valid();
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

      if (status.next_subtest("Null 'this' 1"))
      {
         /*  1 */

         inform(options, _("A null 'this' is not possible in C++ code"));
         xpc::cut_status result = unit_test_run_a_test(nullptr, nullptr);
         bool ok = ! unit_test_status_passed(&result);
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

         status.pass(ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this' 2"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         xpc::cut_status result = unit_test_run_a_test
         (
            nullptr, fake_unit_test_04_19
         );
         bool ok = ! unit_test_status_passed(&result);
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

      if (status.next_subtest("Null test-pointer"))
      {
         xpc::cut_status s =
            unit_test_run_a_test(&x_test_x, (intptr_t) nullptr);

         ok = ! unit_test_status_passed(&s);
         show_deliberate_failure(options);
         status.pass(ok);
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
 *
 *    unit_test_t functions.
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

static xpc::cut_status
cut_unit_test_04_23 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 23,
      _("unit_test_t"), _("unit_test_check_subtests()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      unit_test_t x_test_x;
      xpc::cut_status x_status;
      char * argv[FULL_ARG_COUNT + 1];
      int argc = 2;
      argv[0] = "unit_test_test";
      argv[1] = "--no-show-progress";
      ok = unit_test_initialize
      (
         &x_test_x, argc, argv, "Test 04.23.1", "version", "additionalhelp"
      );
      // if (ok)
      //    ok = x_status.init();

      /*  1 */

      if (status.next_subtest("Null 'this' 1"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         int result = unit_test_check_subtests(nullptr, nullptr);
         ok = result == -1;
         status.pass(ok);
      }

      /*  2 */

      if (status.next_subtest("Null 'this' 2"))
      {
         inform(options, _("A null 'this' is not possible in C++ code"));
         int result = unit_test_check_subtests(nullptr, &x_status);
         ok = result == -1;
         status.pass(ok);
      }

      /*  3 */

      if (status.next_subtest("Null status-pointer"))
      {
         int result = unit_test_check_subtests(&x_test_x, nullptr);
         ok = result == 0;
         status.pass(ok);
      }

      /*  4 */

      if (status.next_subtest("Null status, need subtest"))
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
            ok = result == 0;                      /* checked /before/ init   */

         result = unit_test_check_subtests(&x_test_x, nullptr);
         if (ok)
            ok = result == -1;                     /* checked /after/ init    */

         status.pass(ok);
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
 * \test
 *    -  xpccut_text_domain()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_05_01 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 5, 1, _("xpccut_text_domain()"), _("Smoke Test")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      /*  1 */

      bool original_silence_setting = xpccut_is_silent();
      if (original_silence_setting)
      {
         if (options.is_verbose())
            xpccut_allow_printing();
      }
      if (status.next_subtest("1"))
      {
         ok = xpccut_text_domain();
         status.pass(ok);
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
 * \test
 *    This unit-test function perforce tests more than one unit from the
 *    unit-test library (libxpccut):
 *
 *    -  xpc::cut_status::initialize()
 *    -  xpc::cut_status::pass()
 *    -  xpc::cut_status::fail()
 *    -  xpc::cut_options::show_values(options) [by user visualization only]
 *    -  xpc::cut_status::is_verbose() [by user visualization only]
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

/* static */ xpc::cut_status
cut_unit_test_05_02 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 5, 2, _("xpccut Test"), _("Prompts")
   );
   if (! status.can_proceed())    /* is test allowed to run? */
      status.pass(true);        /* no, force it to pass    */
   else if (ok)
   {
      if (status.next_subtest(_("'C' test")))
      {
         bool ok = unit_test_status_prompt
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
               status.disposition() ==
                  XPCCUT_DISPOSITION_CONTINUE
            );
         }
         else
            status.pass(true);
      }
      if
      (
         status.next_subtest(_("'S' test"))
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
            status.is_skipped() ||
            status.is_okay()
         );
      }

      /*
       * We used to do checks like this:
       *
       *    if (status.passed() &&
       *       status.next_subtest(_("'F' test")))
       *
       * However, the status-passed call is done in the next-subtest call,
       * and also keeps track of the stop-on-error option.  So the test
       * becomes much easier to fit on a single line.
       */

      if (status.next_subtest(_("'F' test")))
      {
         (void) unit_test_status_prompt
         (
            &status, _("Press 'f' or 'F' followed by the Enter key")
         );
         (void) unit_test_status_pass
         (
            &status,
            status.is_failed() ||
            status.is_okay()
         );
      }
      if (status.next_subtest(_("'Q' test")))
      {
         (void) unit_test_status_prompt
         (
            &status, _("Press 'q' or 'Q' followed by the Enter key")
         );
         (void) unit_test_status_pass
         (
            &status,
            status.is_quitted() ||
            status.is_okay()
         );
      }
      if (status.next_subtest(_("'A' test")))
      {
         (void) unit_test_status_prompt
         (
            &status, _("Press 'a' or 'A' followed by the Enter key")
         );
         (void) unit_test_status_pass
         (
            &status,
            status.is_aborted() ||
            status.is_okay()
         );
      }
      if (! status.passed())
         xpccut_errprint(_("Hey, why did you disobey me?!"));
   }
   status.reset();    /* prevent premature test termination  */
   return status;
}

/**
 *    This test demonstrates the usage of the unit-test prompt facility.  If
 *    the user answers all the prompts as directed, the test will pass.  This
 *    means that the user cannot abort this test (grin).
 *
 * \test
 *    This unit-test function perforce tests more than one unit from the
 *    unit-test library (libxpccut):
 *
 *    -  xpc::cut_status::initialize()
 *    -  xpc::cut_status::pass()
 *    -  xpc::cut_status::fail()
 *    -  xpc::cut_status::is_verbose() [by user visualization only]
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

/* static */ xpc::cut_status
cut_unit_test_05_03 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 5, 3, _("xpccut Test"), _("Disposal")
   );
   if (! status.can_proceed())    /* is test allowed to run? */
      status.pass(true);        /* no, force it to pass    */
   else if (ok)
   {
      if (status.next_subtest(_("'Q' test")))
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
 * \test
 *    This unit-test function perforce tests more than one unit from the
 *    unit-test library (libxpccut):
 *
 *    -  xpc::cut_status::initialize()
 *    -  xpc::cut_status::pass()
 *    -  xpc::cut_status::fail()
 *    -  xpc::cut_status::is_verbose() [by user visualization only]
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

/* static */ xpc::cut_status
cut_unit_test_05_04 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 5, 4, _("xpccut Test"), _("Continuation")
   );
   if (! status.can_proceed())    /* is test allowed to run? */
      status.pass(true);        /* no, force it to pass    */
   else if (ok)
   {
      status.pass(true);
      if (options.is_verbose())
      {
         fprintf
         (
            stdout, "  %s\n",
            _("You will see this message only if you didn't answer 'q' before")
         );
      }
      if (status.next_subtest(_("Continuation test")))
         (void) unit_test_status_prompt(&status, _("Answer as you wish"));
   }
   return status;
}

/**
 *    Recapitulates cut_unit_test_01_01().
 *    Why?  To make it easy to check up on some aspects of the --summarize
 *    option.
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

static xpc::cut_status
cut_unit_test_06_01 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 6, 1, _("xpccut"), _("01.01 Smoke Test Recap")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed()) /* is test allowed to run? */
      {
         status.pass(true);     /* no, force it to pass    */
      }
      else
      {
         if (unit_test_options_show_values(options))
            fprintf(stdout, "  %s\n", _("No values to show in this test"));

         /*  1 */

         if (status.next_subtest("unit_test_status_pass()"))
         {
            status.pass(true);
            ok =
            (
               (status.m_Subtest_Error_Count == 0) &&
               (status.m_Failed_Subtest == 0)
            );
            if (! ok)
            {
               (void) status.fail();    /* create a failure  */
               if (options.is_verbose())
               {
                  fprintf
                  (
                     stderr, "%s %s\n",
                     "unit_test_status_pass()", _("internal failure")
                  );
               }
            }
            status.pass(ok);
         }

         /*  2 */

         if
         (
            ok &&
            status.next_subtest("unit_test_status_fail()")
         )
         {
            ok = status.fail_deliberately();
            if (ok)
            {
               ok =
               (
                  (status.m_Subtest_Error_Count == 1) &&
                  (status.m_Failed_Subtest == 2)
               );
               status.pass(ok); /* convert it to a "pass"  */
               if (ok)
               {
                  status.m_Subtest_Error_Count--;  /* direct access (bad) !!! */
                  status.m_Failed_Subtest = 0;     /* direct access (bad) !!! */
               }
            }
         }
         /*
          * A useless visual check.
          *
          * if (ok)
          *    xpccut_ms_sleep(500);               // a visual duration check
          */
      }
   }
   return status;
}

#endif      // #if 0

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

static xpc::cut_status
cut_unit_test_07_01 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 7, 1, _("xpccut"), _("cut_xxx_nullptr() tests")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed()) /* is test allowed to run? */
      {
         status.pass(true);     /* no, force it to pass    */
      }
      else
      {
         const char * good = "allo";
         const char * bad = nullptr;

         /*  1 */

         if (status.next_subtest("cut_not_nullptr()"))
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
            status.pass(ok);
         }

         /*  2 */

         if (status.next_subtest("cut_not_nullptr()"))
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
            status.pass(ok);
         }
      }
   }
   return status;
}


/**
 *    This is the main routine for the cut_unit_test application.
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
 *
 */

#define CUTPP_APP_NAME           "CUT++ library"
#define CUTPP_TEST_NAME          "cut_unit_test"
#define CUTPP_TEST_VERSION       1.1.2
#define DEFAULT_BASE             "../test"
#define DEFAULT_AUTHOR           "Chris Ahlstrom"

int
main
(
   int argc,               /**< Number of command-line arguments.             */
   char * argv []          /**< The actual array of command-line arguments.   */
)
{
   static const char * const additional_help =

"\n"
CUTPP_TEST_NAME "-specific options:\n"
"\n"
" --base dir         Specify root path [" DEFAULT_BASE "].\n"
" --author name      Specify an egotistical option [" DEFAULT_AUTHOR "].\n"
" --no-load          Avoid loading the tests, as a test of handling this\n"
"                    potential bug.\n"
" --direct-access    Test the fields by accessing them directly, instead of\n"
"                    using the C accessor functions.  This option is useful\n"
"                    to see if the internal C implementation has changed.\n"
"                    The default is to use the C accessors (setter and\n"
"                    getter functions) where they are available.\n"
" --no-direct-access This is the default option.\n"
" --leak-check       Turn on some leak-checking code.\n"

   ;

   xpc::cut testbattery
   (
      argc, argv,
      std::string(CUTPP_TEST_NAME),
      std::string(XPCCUT_VERSION_STRING(CUTPP_TEST_VERSION)),
      std::string(additional_help)
   );
   bool ok = testbattery.valid();
   if (ok)
   {
      int argcount = argc;
      char ** arglist = argv;
      char gsBasename[80];
      char gsAuthor[80];
      cbool_t load_the_tests = true;
      strcpy(gsBasename, DEFAULT_BASE);
      strcpy(gsAuthor, DEFAULT_AUTHOR);
      if (argcount > 1)
      {
         int currentarg = 1;
         while (currentarg < argcount)
         {
            std::string arg = arglist[currentarg];
            if (arg == "--base")
            {
               currentarg++;
               if ((currentarg < argcount) && (arglist[currentarg] != 0))
                  strcpy(gsBasename, arglist[currentarg]);
            }
            else if (arg == "--author")
            {
               currentarg++;
               if ((currentarg < argcount) && (arglist[currentarg] != 0))
                  strcpy(gsAuthor, arglist[currentarg]);
            }
            else if (arg == "--no-load")
            {
               load_the_tests = false;
               xpccut_infoprint(_("will not load any tests"));
            }
            else if (arg == "--direct-access")
            {
               g_use_direct_field_access = true;
            }
            else if (arg == "--no-direct-access")
            {
               g_use_direct_field_access = false;
            }
            else if (arg == "--leak-check")
            {
               gs_do_leak_check = true;
            }
            else if (arg == "--no-leak-check")
            {
               gs_do_leak_check = false;
            }
            currentarg++;
         }
      }
      if (load_the_tests)
      {
         ok = testbattery.load(cut_unit_test_01_01);
         if (ok)
            (void) testbattery.load(cut_unit_test_01_02);

         if (ok)
         {
            (void) testbattery.load(cut_unit_test_02_01);
            (void) testbattery.load(cut_unit_test_02_02);
            (void) testbattery.load(cut_unit_test_02_03);
            (void) testbattery.load(cut_unit_test_02_04);
            (void) testbattery.load(cut_unit_test_02_05);
            (void) testbattery.load(cut_unit_test_02_06);
            (void) testbattery.load(cut_unit_test_02_07);
            (void) testbattery.load(cut_unit_test_02_08);
            (void) testbattery.load(cut_unit_test_02_09);
            (void) testbattery.load(cut_unit_test_02_10);
            (void) testbattery.load(cut_unit_test_02_11);
            (void) testbattery.load(cut_unit_test_02_12);
            (void) testbattery.load(cut_unit_test_02_13);
            (void) testbattery.load(cut_unit_test_02_14);
            (void) testbattery.load(cut_unit_test_02_15);
            (void) testbattery.load(cut_unit_test_02_16);
            (void) testbattery.load(cut_unit_test_02_17);
            (void) testbattery.load(cut_unit_test_02_18);
            (void) testbattery.load(cut_unit_test_02_19);
            (void) testbattery.load(cut_unit_test_02_20);
            (void) testbattery.load(cut_unit_test_02_21);
            (void) testbattery.load(cut_unit_test_02_22);
            (void) testbattery.load(cut_unit_test_02_23);
            (void) testbattery.load(cut_unit_test_02_24);
            (void) testbattery.load(cut_unit_test_02_25);
            (void) testbattery.load(cut_unit_test_02_26);
            (void) testbattery.load(cut_unit_test_02_27);
            (void) testbattery.load(cut_unit_test_02_28);
            (void) testbattery.load(cut_unit_test_02_29);
            (void) testbattery.load(cut_unit_test_02_30);

            /*
             * No reason to ever expect these loads to fail at run-time,
             * but let's use the return value as an artificial way to
             * lump all tests in one group together.
             */

            ok = testbattery.load(cut_unit_test_02_31);
         }
         if (ok)
         {
            (void) testbattery.load(cut_unit_test_03_01);
            (void) testbattery.load(cut_unit_test_03_02);
            (void) testbattery.load(cut_unit_test_03_03);
            (void) testbattery.load(cut_unit_test_03_04);
            (void) testbattery.load(cut_unit_test_03_05);
#if 0
            (void) testbattery.load(cut_unit_test_03_06);
            (void) testbattery.load(cut_unit_test_03_07);
            (void) testbattery.load(cut_unit_test_03_08);
            (void) testbattery.load(cut_unit_test_03_09);
            (void) testbattery.load(cut_unit_test_03_10);
            (void) testbattery.load(cut_unit_test_03_11);
            (void) testbattery.load(cut_unit_test_03_12);
            (void) testbattery.load(cut_unit_test_03_13);
            (void) testbattery.load(cut_unit_test_03_14);
            (void) testbattery.load(cut_unit_test_03_15);
            (void) testbattery.load(cut_unit_test_03_16);
            (void) testbattery.load(cut_unit_test_03_17);
            (void) testbattery.load(cut_unit_test_03_18);
            (void) testbattery.load(cut_unit_test_03_19);
            (void) testbattery.load(cut_unit_test_03_20);
            (void) testbattery.load(cut_unit_test_03_21);
            (void) testbattery.load(cut_unit_test_03_22);
            (void) testbattery.load(cut_unit_test_03_23);
            (void) testbattery.load(cut_unit_test_03_24);
            (void) testbattery.load(cut_unit_test_03_25);
            ok = testbattery.load(cut_unit_test_03_26);
#endif
         }
#if 0
         if (ok)
         {
            (void) testbattery.load(cut_unit_test_04_01);
            (void) testbattery.load(cut_unit_test_04_02);
            (void) testbattery.load(cut_unit_test_04_03);
            (void) testbattery.load(cut_unit_test_04_04);
            (void) testbattery.load(cut_unit_test_04_05);
            (void) testbattery.load(cut_unit_test_04_06);
            (void) testbattery.load(cut_unit_test_04_07);
            (void) testbattery.load(cut_unit_test_04_08);
            (void) testbattery.load(cut_unit_test_04_09);
            (void) testbattery.load(cut_unit_test_04_10);
            (void) testbattery.load(cut_unit_test_04_11);
            (void) testbattery.load(cut_unit_test_04_12);
            (void) testbattery.load(cut_unit_test_04_13);
            (void) testbattery.load(cut_unit_test_04_14);
            (void) testbattery.load(cut_unit_test_04_15);
            (void) testbattery.load(cut_unit_test_04_16);
            (void) testbattery.load(cut_unit_test_04_17);
            (void) testbattery.load(cut_unit_test_04_18);
            (void) testbattery.load(cut_unit_test_04_19);
            (void) testbattery.load(cut_unit_test_04_20);
            (void) testbattery.load(cut_unit_test_04_21);
            (void) testbattery.load(cut_unit_test_04_22);
            ok = testbattery.load(cut_unit_test_04_23);
         }
         if (ok)
         {
            ok = testbattery.load(cut_unit_test_05_01);
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
                * We're still not prepared to clean these tests up.
                *
               (void) testbattery.load(cut_unit_test_05_02);
               (void) testbattery.load(cut_unit_test_05_03);
               (void) testbattery.load(cut_unit_test_05_04);
                */
            }
         }
         if (ok)
         {
            ok = testbattery.load(cut_unit_test_06_01);
         }
#endif
         if (ok)
         {
            ok = testbattery.load(cut_unit_test_07_01);
         }
      }
      if (ok)
         ok = testbattery.run();
      else
         xpccut_errprint(_("load of test functions failed"));
   }
   return ok ? EXIT_SUCCESS : EXIT_FAILURE ;
}

/*
 * cut_unit_test.cpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
