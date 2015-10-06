#if ! defined XPC_CUT_STATUS_HPP
#define XPC_CUT_STATUS_HPP

/**
 * \file          cut_status.hpp
 * \library       libxpccut++
 * \author        Chris Ahlstrom
 * \date          2008-03-27
 * \updates       2015-10-05
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides the declarations for the xpc::cut_status class.  This class
 *    provides a container for the current status of a unit-test application.
 *    Please see the xpc::cut_status class description for details.  Also see
 *    the cut_status.cpp module and the cut_status.hpp modules for more
 *    information.
 *
 * \note
 *    This module defines the XPC_NO_THISPTR macro in order to avoid
 *    unnecessary checking of null-pointers for passing the C structures to
 *    member functions.
 *
 */

#define XPC_NO_THISPTR                 /* do not test the C "this" pointers   */

#include <string>                      /* std::string                         */
#include <xpc/unit_test_status.h>      /* unit_test_status_t structure        */

namespace xpc
{

/**
 *    Provides access to private members so that the main unit-test class
 *    can alter the status members.
 */

class cut;                             /* forward reference                   */

/**
 *    Provides a class reference so that cut_options class can be used as a
 *    reference parameter.
 */

class cut_options;                     /* forward reference                   */

/**
 *    Provides access to private members so that white-box tests can be
 *    performed on this unit-test library.
 *
 *    See one example of an implementation and instantiation of this
 *    test-only class in cut_unit_test.cpp in the tests directory.
 */

class cut_white_box;                   /* forward reference                   */

/**
 *    Provides an object to manage a unit_test_status_t structure.
 *
 * \note
 *    This class provides a status() function to return a reference to the
 *    unit_test_status_t m_Status member.  However, it cannot provide an
 *    options() function to return a reference to the unit_test_options_t
 *    field that is part of m_Status.  This field is a pointer, and it can
 *    be nullptr.  We don't want exceptions, if we can help it.  See
 *    cut_unit_test_02_01() for more discussion.
 */

class cut_status
{
   /**
    *    The cut class must be a friend of cut_status.
    *
    *    The reason for this friendship is that it needs to access the
    *    cut_status::m_Status member directly, rather that through the \e
    *    const function cut_status::status(), so that it can modify the test
    *    status as tests pass and fail.
    */

   friend class cut;

   /**
    *    Provides a way to perform white-box testing of the cut_status
    *    class.
    *
    *    White-box testing involves testing the protected and private
    *    members of a class.  It means making them visible to a unit-test.
    *
    *    Direct access to the C structure(s) involved is prohibited by
    *    the "private" qualifier.  Furthermore, these C structures have
    *    already been tested (in the C project xpccut).  Nonetheless, it is
    *    still desirable to peek into the internals and make sure the
    *    implementation is as expected, to allow checking for creeping
    *    inconsistencies between the two libraries.
    *
    *    Note that you can define your very own version of cut_white_box,
    *    should you want to test the CUT++ library yourself.
    */

   friend class cut_white_box;

private:

   /**
    *    Provides a structure to hold that status information for the
    *    unit-test.
    *
    *    This structure is a C structure, with C accessor functions.  The
    *    cut_status class will wrap its accessors with efficient inline
    *    functions.
    *
    *    We make it mutable so that we can provide \e const accessor
    *    functions so that outsiders cannot modify m_Status.  For example,
    *    by using strcpy() to overflow the character arrays.
    */

   mutable unit_test_status_t m_Status;

   /**
    *    Provides a way for the caller to check if the object allows the
    *    unit-test to run.  The user can specify that only certain tests
    *    run, so this flag lets the other tests be skipped.
    */

    bool m_Can_Run;

   /**
    *    Provides a way for the caller to check if the object is valid.
    *    This check is normally needed only after the constructor is called.
    *    We could throw an exception in the event of an error, but we just
    *    don't want to use exceptions in this class.
    */

    bool m_Is_Valid;

public:

   /*
    *    Default constructor.  Documented in the cpp file.
    */

   cut_status ();

   /*
    *    Principle constructor.  Documented in the cpp file.
    */

   cut_status
   (
      const cut_options & options,
      int testgroup,
      int testcase,
      const std::string & groupname,
      const std::string & casename
   );

   /*
    *    Copy constructor.  Documented in the cpp file.
    */

   cut_status (const cut_status & source);

   /**
    *    A principal class constructor.
    *    See \ref nice_classes for more information.
    *    This is a hypothetical example documented here instead of in the
    *    cpp file.
    *
    * \ctor
    *    This constructor allows this class to ...?
    *
    * \param item
    *    This is a dummy item to show you how to document it.
    */

   cut_status (int item);

   /*
    *    Principal assignment operator.  Documented in the cpp file.
    */

   cut_status & operator = (const cut_status & source);

   /**
    *    Provides the class destructor.
    *    See \ref nice_classes for more information.
    */

   ~cut_status ();

   /**
    * \accessor m_Is_Valid
    *
    *    After the construction of a cut_status object, this function should
    *    be called before proceeding with the unit-test.
    *
    * \warning
    *    This function can return 'false' for errors and for a request to
    *    skip a function.  Therefore, don't base test failure on the value
    *    returned by this function.
    *
    *    The error-processing could be enforced by instead throwing an
    *    exception, but we prefer to not force the caller to use the
    *    exception mechanism.
    */

   bool valid () const
   {
      return m_Is_Valid && m_Can_Run;
   }

public:

   /**
    * \accessor unit_test_status_beep()
    *
    *    This function merely emits a beep from the console.  Since it
    *    doesn't need any member access, might as well make it a static
    *    member function.
    */

   static void beep ()
   {
      unit_test_status_beep();
   }

   /**
    * \accessor unit_test_status_reset()
    */

   bool reset ()
   {
      return xpccut_boolcast(unit_test_status_reset(&m_Status));
   }

   /**
    * \accessor unit_test_status_pass()
    */

   bool pass (bool flag = true)
   {
      return xpccut_boolcast(unit_test_status_pass(&m_Status, (cbool_t) flag));
   }

   /**
    * \accessor unit_test_status_fail()
    */

   bool fail ()
   {
      return xpccut_boolcast(unit_test_status_fail(&m_Status));
   }

   /**
    * \accessor unit_test_status_int_check()
    */

   bool int_check (int expected_value, int actual_value)
   {
      return xpccut_boolcast
      (
         unit_test_status_int_check(&m_Status, expected_value, actual_value)
      );
   }

   /**
    * \accessor unit_test_status_bool_check()
    */

   bool bool_check (bool expected_value, bool actual_value)
   {
      return xpccut_boolcast
      (
         unit_test_status_bool_check
         (
            &m_Status, bool(expected_value), bool(actual_value)
         )
      );
   }

   /**
    * \accessor unit_test_status_string_check()
    */

   bool string_check
   (
      const std::string & expected_value,
      const std::string & actual_value
   )
   {
      return xpccut_boolcast
      (
         unit_test_status_string_check
         (
            &m_Status, expected_value.c_str(), actual_value.c_str()
         )
      );
   }

   /**
    * \accessor unit_test_status_fail_deliberately()
    */

   bool fail_deliberately ()
   {
      return xpccut_boolcast(unit_test_status_fail_deliberately(&m_Status));
   }

   /**
    * \accessor unit_test_status_start_timer()
    */

   bool start_timer ()
   {
      return xpccut_boolcast(unit_test_status_start_timer(&m_Status));
   }

   /**
    * \accessor unit_test_status_time_delta()
    */

   double time_delta (bool startreset = false)
   {
      return unit_test_status_time_delta(&m_Status, (cbool_t) startreset);
   }

   /**
    * \accessor unit_test_status_show_title()
    */

   bool show_title ()
   {
      return xpccut_boolcast(unit_test_status_show_title(&m_Status));
   }

   /**
    * \accessor unit_test_status_can_proceed()
    */

   bool can_proceed ()
   {
      return xpccut_boolcast(unit_test_status_can_proceed(&m_Status));
   }

   /**
    * \accessor unit_test_status_ignore()
    */

   bool ignore ()
   {
      return xpccut_boolcast(unit_test_status_ignore(&m_Status));
   }

   /**
    * \accessor unit_test_status_next_subtest()
    *
    * \warning
    *    If a null pointer is passed to this function (even as the value
    *    "0"), no compiler error occurs, but the function throws an
    *    std::logic_error exception at run-time.  There's no nice way to
    *    check for this condition inside this function, so the caller is
    *    responsible for using only valid character-string pointers.
    *
    * \param tagname
    *    A brief string describing the sub-test.
    */

   bool next_subtest (const std::string & tagname)
   {
      return xpccut_boolcast
      (
         unit_test_status_next_subtest(&m_Status, tagname.c_str())
      );
   }

   /**
    * \accessor unit_test_status_prompt()
    *
    * \param tagname
    *    A one-line string describing the situation to the user.
    */

   bool prompt (const std::string & message)
   {
      return xpccut_boolcast
      (
         unit_test_status_prompt(&m_Status, message.c_str())
      );
   }

   /**
    * \accessor unit_test_status_response()
    *
    * \note
    *    There is a side-effect on the m_Status member that is not
    *    accessible by the return value of unit_test_status_response():
    *    m_Test_Result can be temporarily set to false.
    *
    * \param tagname
    *    A one-line string describing the situation to the user.
    */

   bool response (const std::string & message)
   {
      return xpccut_boolcast
      (
         unit_test_status_response(&m_Status, message.c_str())
      );
   }

   /**
    * \accessor unit_test_status_group_name()
    *
    * \note
    *    This function returns a fresh std::string build from the C
    *    character pointer, by necessity.
    */

   std::string group_name () const
   {
      return unit_test_status_group_name(&m_Status);
   }

   /**
    * \accessor unit_test_status_case_name()
    *
    * \note
    *    This function returns a fresh std::string build from the C
    *    character pointer, by necessity.
    */

   std::string case_name () const
   {
      return unit_test_status_case_name(&m_Status);
   }

   /**
    * \accessor unit_test_status_subtest_name()
    *
    * \note
    *    This function returns a fresh std::string build from the C
    *    character pointer, by necessity.
    */

   std::string subtest_name () const
   {
      return unit_test_status_subtest_name(&m_Status);
   }

   /**
    * \accessor unit_test_status_group()
    */

   int group () const
   {
      return unit_test_status_group(&m_Status);
   }

   /**
    * \accessor unit_test_status_case()
    *
    *    Note the change in spelling to avoid a collision with the 'case'
    *    keyword.
    */

   int kase () const
   {
      return unit_test_status_case(&m_Status);
   }

   /**
    * \accessor unit_test_status_subtest()
    */

   int subtest () const
   {
      return unit_test_status_subtest(&m_Status);
   }

   /**
    * \accessor unit_test_status_failed_subtest()
    */

   int failed_subtest () const
   {
      return unit_test_status_failed_subtest(&m_Status);
   }

   /**
    * \accessor unit_test_status_error_count()
    */

   int error_count () const
   {
      return unit_test_status_error_count(&m_Status);
   }

   /**
    * \accessor unit_test_status_passed()
    */

   bool passed () const
   {
      return xpccut_boolcast(unit_test_status_passed(&m_Status));
   }

   /**
    * \accessor unit_test_status_failed()
    */

   bool failed () const
   {
      return xpccut_boolcast(unit_test_status_failed(&m_Status));
   }

   /**
    * \accessor unit_test_status_disposition()
    */

   unit_test_disposition_t disposition () const
   {
      return unit_test_status_disposition(&m_Status);
   }

   /**
    *    Tests if the disposition is not 'aborted' and not 'did not test'.
    *
    * \change ca 2011-10-29 new function.  NEEDS UNIT-TESTING.
    */

   bool initialized () const
   {
      return
      (
         disposition() != XPCCUT_DISPOSITION_ABORTED &&
         disposition() != XPCCUT_DISPOSITION_DNT
      );
   }


   /**
    * \accessor unit_test_status_is_continue()
    */

   bool is_continue () const
   {
      return xpccut_boolcast(unit_test_status_is_continue(&m_Status));
   }

   /**
    * \accessor unit_test_status_is_skipped()
    */

   bool is_skipped () const
   {
      return xpccut_boolcast(unit_test_status_is_skipped(&m_Status));
   }

   /**
    * \accessor unit_test_status_is_failed()
    */

   bool is_failed () const
   {
      return xpccut_boolcast(unit_test_status_is_failed(&m_Status));
   }

   /**
    * \accessor unit_test_status_is_quitted()
    */

   bool is_quitted () const
   {
      return xpccut_boolcast(unit_test_status_is_quitted(&m_Status));
   }

   /**
    * \accessor unit_test_status_is_aborted()
    */

   bool is_aborted () const
   {
      return xpccut_boolcast(unit_test_status_is_aborted(&m_Status));
   }

   /**
    * \accessor unit_test_status_is_okay()
    */

   bool is_okay () const
   {
      return xpccut_boolcast(unit_test_status_is_okay(&m_Status));
   }

   /**
    * \accessor unit_test_status_duration_ms()
    */

   double duration_ms () const
   {
      return unit_test_status_duration_ms(&m_Status);
   }

   /**
    * \accessor unit_test_status_show()
    */

   void show () const
   {
      (void) unit_test_status_show(&m_Status);
   }

   /**
    * \accessor unit_test_status_trace()
    *    This function provides a way to investigate issues with the
    *    assignment operator and copy constructor.
    *
    *    If the cut_status object was created with a case name of "TRACE",
    *    then this accessor calls unit_test_status_trace().  This let's us
    *    provide some context for debugging messages, and makes it easier to
    *    track down the locations of some classes of obscure bugs involving
    *    the copying of structures.
    *
    * \usage
    *    See cut_unit_test_02_10() for a simple usage example.  Call this
    *    function at various points along a given use case for the
    *    cut_status object.
    *
    * \param context
    *    Provides a string that indicates where or how the trace() function
    *    was called.  If null, it is ignored.
    *
    * \unittests
    *    -  cut_unit_test_02_10()
    */

   void trace (const char * context) const
   {
      if (case_name() == "TRACE")
         (void) unit_test_status_trace(&m_Status, context);
   }

public:                    /* self-test functions  */

   /**
    *    Provides a function to decrement the error count.
    *
    * \warning
    *    This function is to be used only in unit-tests of the CUT++
    *    library.
    */

   void self_test_error_count_decrement ()
   {
      (void) status_self_test_error_count_decrement(&m_Status);
   }

   /**
    *    Provides a function to set the "first-failed subtest" value.
    *
    * \warning
    *    This function is to be used only in unit-tests of the CUT++
    *    library.
    */

   void self_test_failed_subtest_set (int value)
   {
      (void) status_self_test_failed_subtest_set(&m_Status, value);
   }

private:

   /**
    * \accessor unit_test_status_init()
    *    This function is actually dangerous to use, since it will
    *    null out the m_Test_Options pointer of the m_Status structure.
    *    And, since we don't check this pointer in the C++ code, the
    *    application will segfault.
    */

   bool init ()
   {
      return xpccut_boolcast(unit_test_status_init(&m_Status));
   }

   /**
    * \getter m_Status
    *    This function is \e const, even though the unit_test_status_t
    *    structure is a scratchpad for the unit-test application, and thus
    *    it is expected that it can be altered.  We get around this
    *    restriction by making cut a friend and accessing the mutable
    *    m_Status member directly.
    */

   const unit_test_status_t & status () const
   {
      return m_Status;
   }

};             /* class cut_status     */

}              /* namespace xpc        */

#endif         /* XPC_CUT_STATUS_HPP   */

/*
 * cut_status.hpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
