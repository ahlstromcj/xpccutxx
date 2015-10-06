#if ! defined XPC_CUT_OPTIONS_HPP
#define XPC_CUT_OPTIONS_HPP

/**
 * \file          cut_options.hpp
 * \library       libxpccut++
 * \author        Chris Ahlstrom
 * \date          2008-03-28
 * \updates       2015-10-06
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides the declarations for the xpc::cut_options class.
 *    This class provides a wrapper for the C structure unit_test_options_t
 *    that encapsulates a number of options for running unit-test
 *    applications.
 *
 *    Also see the cut_options.cpp module and the cut_options.hpp module
 *    for more information.
 *
 *    This module turns on the XPC_NO_THISPTR macro to avoid testing the
 *    pointers to the first parameter of the C functions. <i> We need to
 *    make sure this works under the debugger. </i>
 */

#define XPC_NO_THISPTR                 /* do not test the C "this" pointers   */

#include <string>                      /* std::string                         */
#include <xpc/unit_test_options.h>     /* unit_test_options_t structure       */

/**
 *    Provides a macro to make it easy to see when an xpc::cut_options
 *    object is set up for simulating the options.
 *
 *    This macro is useful in calling the "default" xpc::cut_options
 *    constructor.
 *
 *    We need to explain when you'd want to modify this macro's value,
 *    though.
 */

#define XPCCUT_OPTIONS_SIMULATED       true

namespace xpc
{

/**
 *    Provides access to private members for the sole reason of allowing
 *    cut_status to refer to the cut_options::options() function.
 *    This is necessary only for this purpose.
 *
 *    One problem with this friend is that the private data members might
 *    end up being accessed directly by programmers who maintain this class.
 */

class cut_status;                      /* forward reference                   */

/**
 *    Provides access to private members so that white-box tests can be
 *    performed on this unit-test library.
 *
 *    See one example of an implementation and instantiation of this
 *    test-only class in cut_unit_test.cpp in the tests directory.
 */

class cut_white_box;                   /* forward reference                   */

/**
 *    Provides an object to manage a unit_test_options_t structure.
 */

class cut_options
{

   /**
    *    Provides a way to allow cut_status to directly access the options
    *    structure.
    *
    * \warning
    *    Be very careful not to directly access the private data members of
    *    cut_options, in spite of this friendly status.
    */

   friend class cut_status;

   /**
    *    Provides a way to perform white-box testing of the cut class.
    *    White-box testing involves testing the protected and private
    *    members of a class.  It means making them visible to a unit-test.
    *
    *    Note that you can define your very own version of cut_white_box,
    *    should you want to test the CUT++ library yourself in your own way.
    */

   friend class cut_white_box;

private:

   /**
    *    Provides a structure to hold that options information for the
    *    unit-test.
    *
    *    This structure is a C structure, with C accessor functions.  The
    *    cut_options class will wrap its accessors with efficient inline
    *    functions.
    */

   unit_test_options_t m_Options;

   /**
    *    Provides a way for the caller to check if the object is valid.
    *    This check is normally needed only after the constructor is called.
    *
    *    We could throw an exception in the event of an error, but we just
    *    don't want to use exceptions in this small library.
    */

    bool m_Is_Valid;

public:

    static bool silent ()
    {
       return xpccut_is_silent() ? true : false ;
    }

public:

   /*
    *    Default constructor.  Documented in the cpp file.
    */

   cut_options (bool simulate = false);      /* see XPCCUT_OPTIONS_SIMULATED  */

   /*
    *    Principle constructor.  Documented in the cpp file.
    */

   cut_options
   (
      int argc,
      const char * argv [],
      const std::string & testname,
      const std::string & version,
      const std::string & additionalhelp
   );

   /*
    *    Another principle constructor.  Documented in the cpp file.
    */

   cut_options
   (
      int argc,
      const char * argv [],
      const char * testname,
      const char * version,
      const char * additionalhelp
   );

   /*
    *    Copy constructor.  Documented in the cpp file.
    */

   cut_options (const cut_options & source);

   /*
    *    A principal class constructor.  Documented in the cpp file.
    */

   cut_options (int item);

   /*
    *    Principal assignment operator.  Documented in the cpp file.
    */

   cut_options & operator = (const cut_options & source);

   /*
    *    Provides the class destructor.  Documented in the cpp file.
    */

   ~cut_options ();

   /**
    * \getter m_Is_Valid
    */

   bool valid () const
   {
      return m_Is_Valid;
   }

public:

   /**
    *    Initialize the m_Options member.
    *
    *    This function passes m_Options to the C function
    *    unit_test_options_init().
    *
    * \unittests
    *    -  cut_unit_test_03_01()
    *
    * \return
    *    Returns 'true' if the initialization succeeded.
    */

   bool init ()
   {
      return xpccut_boolcast(unit_test_options_init(&m_Options));
   }

   /*
    *    Initialize the command-line options to set the m_Options member.
    *
    *    Removed because we only need parsing at xpc::cut_options
    *    construction time.
    *
    * \return
    *    Returns 'true' if the parsing succeeded.
    *
    *    bool parse
    *    (
    *       int argc,
    *       char * argv [],
    *       const std::string & testname,
    *       const std::string & version,
    *       const std::string & additionalhelp = ""
    *    );
    */

   /**
    *    Copies the unit_test_options_t parameter into the m_Options member.
    *    This function is needed by the principal xpc::cut constructor,
    *    which needs to copy the options from
    *    xpc::cut::m_UnitTest.m_App_Options to
    *    cut::m_UnitTest_Options::m_Options.
    */

   void copy_options (const unit_test_options_t & options)
   {
      m_Options = options;
   }

   /*
    * Not implemented.
    *
    * void help
    * (
    *    const std::string & appname,
    *    const std::string & version,
    *    const std::string & additionalhelp = ""
    * );
    *
    */

   /**
    * \getter unit_test_options_version()
    */

   void version ()
   {
      unit_test_options_version("CUT++ 1.0.0");
   }

   /**
    * \setter unit_test_options_is_verbose_set()
    */

   void is_verbose (bool f)
   {
      (void) unit_test_options_is_verbose_set(&m_Options, (cbool_t) f);
   }

   /**
    * \getter unit_test_options_is_verbose()
    */

   bool is_verbose () const
   {
      return xpccut_boolcast(unit_test_options_is_verbose(&m_Options));
   }

   /**
    * \setter unit_test_options_show_values_set()
    */

   void show_values (bool f)
   {
      (void) unit_test_options_show_values_set(&m_Options, f);
   }

   /**
    * \getter unit_test_options_show_values()
    */

   bool show_values () const
   {
      return xpccut_boolcast(unit_test_options_show_values(&m_Options));
   }

   /**
    * \setter unit_test_options_use_text_synch_set()
    */

   void use_text_synch (bool f)
   {
      (void) unit_test_options_use_text_synch_set(&m_Options, f);
   }

   /**
    * \getter unit_test_options_use_text_synch()
    */

   bool use_text_synch () const
   {
      return xpccut_boolcast(unit_test_options_use_text_synch(&m_Options));
   }

   /**
    * \setter unit_test_options_show_step_numbers_set()
    */

   void show_step_numbers (bool f)
   {
      (void) unit_test_options_show_step_numbers_set(&m_Options, f);
   }

   /**
    * \getter unit_test_options_show_step_numbers()
    */

   bool show_step_numbers () const
   {
      return xpccut_boolcast(unit_test_options_show_step_numbers(&m_Options));
   }

   /**
    * \setter unit_test_options_show_progress_set()
    */

   void show_progress (bool f)
   {
      (void) unit_test_options_show_progress_set(&m_Options, f);
   }

   /**
    * \getter unit_test_options_show_progress()
    */

   bool show_progress () const
   {
      return xpccut_boolcast(unit_test_options_show_progress(&m_Options));
   }

   /**
    * \setter unit_test_options_stop_on_error_set()
    */

   void stop_on_error (bool f)
   {
      (void) unit_test_options_stop_on_error_set(&m_Options, f);
   }

   /**
    * \getter unit_test_options_stop_on_error()
    */

   bool stop_on_error () const
   {
      return xpccut_boolcast(unit_test_options_stop_on_error(&m_Options));
   }

   /**
    * \setter unit_test_options_batch_mode_set()
    */

   void batch_mode (bool f)
   {
      (void) unit_test_options_batch_mode_set(&m_Options, f);
   }

   /**
    * \getter unit_test_options_batch_mode()
    */

   bool batch_mode () const
   {
      return xpccut_boolcast(unit_test_options_batch_mode(&m_Options));
   }

   /**
    * \setter unit_test_options_is_interactive_set()
    */

   void is_interactive (bool f)
   {
      (void) unit_test_options_is_interactive_set(&m_Options, f);
   }

   /**
    * \getter unit_test_options_is_interactive()
    */

   bool is_interactive () const
   {
      return xpccut_boolcast(unit_test_options_is_interactive(&m_Options));
   }

   /**
    * \setter unit_test_options_do_beep_set()
    */

   void do_beep (bool f)
   {
      (void) unit_test_options_do_beep_set(&m_Options, f);
   }

   /**
    * \getter unit_test_options_do_beep()
    */

   bool do_beep () const
   {
      return xpccut_boolcast(unit_test_options_do_beep(&m_Options));
   }

   /**
    * \setter unit_test_options_need_subtests_set()
    */

   void need_subtests (bool f)
   {
      (void) unit_test_options_need_subtests_set(&m_Options, f);
   }

   /**
    * \getter unit_test_options_need_subtests()
    */

   bool need_subtests () const
   {
      return xpccut_boolcast(unit_test_options_need_subtests(&m_Options));
   }

   /**
    * \setter unit_test_options_force_failure_set()
    */

   void force_failure (bool f)
   {
      (void) unit_test_options_force_failure_set(&m_Options, f);
   }

   /**
    * \getter unit_test_options_force_failure()
    */

   bool force_failure () const
   {
      return xpccut_boolcast(unit_test_options_force_failure(&m_Options));
   }

   /**
    * \setter unit_test_options_is_summary_set()
    */

   void is_summary (bool f)
   {
      (void) unit_test_options_is_summary_set(&m_Options, f);
   }

   /**
    * \getter unit_test_options_is_summary()
    */

   bool is_summary () const
   {
      return xpccut_boolcast(unit_test_options_is_summary(&m_Options));
   }

   /**
    * \setter unit_test_options_is_pause_set()
    */

   void is_pause (bool f)
   {
      (void) unit_test_options_is_pause_set(&m_Options, f);
   }

   /**
    * \getter unit_test_options_is_pause()
    */

   bool is_pause () const
   {
      return xpccut_boolcast(unit_test_options_is_pause(&m_Options));
   }

   /**
    * \setter unit_test_options_test_group_set()
    */

   void single_group (int v)
   {
      (void) unit_test_options_test_group_set(&m_Options, v);
   }

   /**
    * \setter unit_test_options_test_named_group_set()
    */

   void single_group (const char * v)
   {
      (void) unit_test_options_named_group_set(&m_Options, v);
   }

   /**
    * \getter unit_test_options_test_group()
    */

   int single_group () const
   {
      return unit_test_options_test_group(&m_Options);
   }

   /**
    * \setter unit_test_options_test_case_set()
    */

   void single_case (int v)
   {
      (void) unit_test_options_test_case_set(&m_Options, v);
   }

   /**
    * \setter unit_test_options_test_named_case_set()
    */

   void single_case (const char * v)
   {
      (void) unit_test_options_named_case_set(&m_Options, v);
   }

   /**
    * \getter unit_test_options_test_case()
    */

   int single_case () const
   {
      return unit_test_options_test_case(&m_Options);
   }

   /**
    * \setter unit_test_options_single_subtest_set()
    */

   void single_subtest (int v)
   {
      (void) unit_test_options_single_subtest_set(&m_Options, v);
   }

   /**
    * \setter unit_test_options_named_subtest_set()
    */

   void single_subtest (const char * v)
   {
      (void) unit_test_options_named_subtest_set(&m_Options, v);
   }

   /**
    * \getter unit_test_options_single_subtest()
    */

   int single_subtest () const
   {
      return unit_test_options_single_subtest(&m_Options);
   }

   /**
    *    Indicates if any single group, case, or subtest has been selected,
    *    which means that the test is only partial.
    *
    *    One use of this check is for not bothering with tests that
    *    depend on previous sequence of tests, such as checking the number
    *    of counted packets.
    *
    * @todo
    *    Unit-test this function.
    */

   bool is_partial_test () const
   {
      return single_group() > 0 || single_case() > 0 || single_subtest() > 0;
   }

   /**
    * \setter unit_test_options_current_test_set()
    */

   void current_test (int v)
   {
      (void) unit_test_options_current_test_set(&m_Options, v);
   }

   /**
    * \getter unit_test_options_current_test()
    */

   int current_test () const
   {
      return unit_test_options_current_test(&m_Options);
   }

   /**
    * \setter unit_test_options_test_sleep_time_set()
    */

   void test_sleep_time (int v)
   {
      (void) unit_test_options_test_sleep_time_set(&m_Options, v);
   }

   /**
    * \getter unit_test_options_test_sleep_time()
    */

   int test_sleep_time () const
   {
      return unit_test_options_test_sleep_time(&m_Options);
   }

   /**
    * \setter unit_test_options_is_simulated_set()
    */

   void is_simulated (bool v)
   {
      (void) unit_test_options_is_simulated_set(&m_Options, v);
   }

   /**
    * \getter unit_test_options_is_simulated()
    */

   bool is_simulated () const
   {
      return xpccut_boolcast(unit_test_options_is_simulated(&m_Options));
   }

   /**
    * \setter unit_test_options_prompt_before_set()
    */

   void prompt_before (char v)
   {
      (void) unit_test_options_prompt_before_set(&m_Options, v);
   }

   /**
    * \getter unit_test_options_prompt_before()
    */

   char prompt_before () const
   {
      return unit_test_options_prompt_before(&m_Options);
   }

   /**
    * \setter unit_test_options_prompt_after_set()
    */

   void prompt_after (char v)
   {
      (void) unit_test_options_prompt_after_set(&m_Options, v);
   }

   /**
    * \getter unit_test_options_prompt_after()
    */

   char prompt_after () const
   {
      return unit_test_options_prompt_after(&m_Options);
   }

private:

   /**
    * \getter m_Options
    */

   const unit_test_options_t & options () const
   {
      return m_Options;
   }

};             /* class cut_options    */

}              /* namespace xpc        */

#endif         /* XPC_CUT_OPTIONS_HPP  */

/*
 * cut_options.cpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
