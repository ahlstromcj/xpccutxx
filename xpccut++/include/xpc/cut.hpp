#if ! defined XPC_CUT_HPP
#define XPC_CUT_HPP

/**
 * \file          cut.hpp
 * \library       xpccut++
 * \author        Chris Ahlstrom
 * \date          2008-03-28
 * \updates       2015-10-06
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides the declarations for the xpc::cut class.
 *    This class provides a managerial class for C++ unit-testing.  It is
 *    primarily a wrapper for the unit_test_t structure defined in the
 *    xpccut library, which is a standard C library.
 *
 *    Also see the cut.cpp module and the cut.hpp module
 *    for more information.
 */

#define XPC_NO_THISPTR                 /* do not check the C "this" pointers  */

#include <vector>                      /* std::vector                         */
#include <xpc/unit_test.h>             /* unit_test_t and functions           */
#include <xpc/cut_options.hpp>         /* xpc::cut_options                    */
#include <xpc/cut_status.hpp>          /* xpc::cut_status                     */

namespace xpc
{

/**
 *    Provides access to private members so that white-box tests can be
 *    performed on this unit-test library.
 *
 *    See one example of an implementation and instantiation of this
 *    test-only class in cut_unit_test.cpp in the tests directory.
 */

class cut_white_box;                   /* forward reference                   */

/**
 *    Provides an object to manage a full unit-test application.
 *    This object serves as an object-orient wrapper for the unit_test_t
 *    structure.
 *
 *    Fixes in the C code will be useful immediately in the C++ code.
 *
 *    A small amount of efficiency can be gained by building the xpccut and
 *    CUT libraries with the --disable-thisptr option specified at
 *    configuration time.
 */

class cut
{

   /**
    *    Provides a way to perform white-box testing of the cut class.
    *    White-box testing involves testing the protected and private
    *    members of a class.  It means making them visible to a unit-test.
    *
    *    Note that you can define your very own version of cut_white_box,
    *    should you want to test the CUT++ library yourself in your own way.
    */

   friend class cut_white_box;

   /**
    *    Provides a C++ version of the C unit_test_func_t function type.
    *    The reasons for replacing this C structure in the C++ version of the
    *    unit-test framework are:
    *
    *       -# Use an object-oriented options class.
    *       -# Hide data as much as possible.
    *
    *    Note that the unit-test functions are expected to be global functions.
    *    They will use the cut_status and cut_options objects, but will
    *    otherwise not be tied to any class.
    */

   typedef cut_status (* test_function)
   (
      const cut_options & options   /**< Global options for the unit tests.   */
   );

   /**
    *    Provides a container for test_function function pointers.
    *    Because this item replaces the unit_test_list_t array of function
    *    pointers, that part of the unit_test_t utility will never be used in
    *    C++ code, even though a small amount of space will be allocated for it.
    */

   typedef std::vector<test_function> cut_test_list;

private:

   /**
    *    Provides access to the C unit_test_t structure.
    *    This structure allows the C++ code to follow the functionality of
    *    the C code in the XPC CUT library closely.
    *
    *    The present class, cut, provides a wrapper around this C structure.
    */

   unit_test_t m_UnitTest;

   /**
    *    Provides a list of C++ global unit-test functions.
    *    This list replaces the array of function pointers, unit_test_list_t
    *    (m_Test_Cases) used in the C version of the library.
    */

   cut_test_list m_UnitTest_List;

   /**
    *    Provides a C++ version of the unit_test_options_t structure.
    *    This object replaces the unit_test_options_t structure
    *    (m_App_Options) used in the C version of the library.
    */

    cut_options m_UnitTest_Options;

   /**
    *    Provides a way for the caller to check if the object is valid.
    *    This check is normally needed only after the constructor is called.
    *    It's main purpose is to make sure that pointers are not null, but
    *    as a one-time thing.
    *
    *    We could throw an exception in the event of an error, but we just
    *    don't want to use exceptions here.
    */

    bool m_Is_Valid;

   /**
    *    Provides a way to load and run C versions of the unit-tests.
    *    To allow C unit-test functions to run instead of C++ unit-test
    *    functions, the c_load() member is called instead of the load()
    *    function.  This flag is then raised, so that, from then on, only C
    *    unit-test functions can be loaded.
    *
    * \warning
    *    This feature is on trial, and will disappear if it turns out to be
    *    garbage.  For example, if you really want to test some C functions,
    *    you can always wrap them in a C++ unit-test function.
    */

    bool m_Run_C_Unit_Tests;

public:

   /*
    *    Default constructor.  Documented in the cpp file.
    */

   cut ();

   /*
    *    Principal constructor.  Documented in the cpp file.
    */

   cut
   (
      int argc,
      char * argv [],
      const std::string & appname      = "Unit Test",
      const std::string & appversion   = "1.0",
      const std::string & addedhelp    = ""
   );

   /*
    *    Copy constructor.  Documented in the cpp file.
    */

   cut (const cut & source);

   /*
    *    Principal assignment operator.  Documented in the cpp file.
    */

   cut & operator = (const cut & source);

   /*
    *    Provides the class destructor.  Documented in the cpp file.
    */

   ~cut ();

   /**
    * @accessor m_Is_Valid
    */

   bool valid () const
   {
      return m_Is_Valid;
   }

   /*
    * Documented in the cpp file.
    */

   bool load (const test_function test);
   bool c_load (unit_test_func_t test);
   bool run ();

   static void exclaim (const cut_options & option, const std::string & msg);
   static void inform (const cut_options & option, const std::string & msg);
   static void show (const cut_options & option, const std::string & msg);
   static void show_deliberate_failure (const cut_options & options);

private:

   cut_status run_a_test (test_function test);
   bool c_run ();
   void initialize
   (
      int argc,
      char * argv [],
      const std::string & appname,
      const std::string & appversion,
      const std::string & addedhelp
   );

   /**
    * \accessor unit_test_dispose()
    */

   bool dispose (cut_status & cs)
   {
      return xpccut_boolcast(unit_test_dispose(&cs.m_Status));
   }

   /**
    * \accessor unit_test_count()
    */

   int count () const
   {
      return unit_test_count(&m_UnitTest);
   }

   /**
    * \accessor unit_test_subtest_count()
    */

   int subtest_count () const
   {
      return unit_test_subtest_count(&m_UnitTest);
   }

   /**
    * \accessor unit_test_number()
    */

   int number () const
   {
      return unit_test_number(&m_UnitTest);
   }

   /**
    * \accessor unit_test_failures()
    */

   int failures () const
   {
      return unit_test_failures(&m_UnitTest);
   }

   /**
    * \accessor unit_test_first_failed_test()
    */

   int first_failed_test () const
   {
      return unit_test_first_failed_test(&m_UnitTest);
   }

   /**
    * \accessor unit_test_first_failed_group()
    */

   int first_failed_group () const
   {
      return unit_test_first_failed_group(&m_UnitTest);
   }

   /**
    * \accessor unit_test_first_failed_case()
    */

   int first_failed_case () const
   {
      return unit_test_first_failed_case(&m_UnitTest);
   }

   /**
    * \accessor unit_test_first_failed_subtest()
    */

   int first_failed_subtest () const
   {
      return unit_test_first_failed_subtest(&m_UnitTest);
   }

   /**
    * \accessor unit_test_report()
    */

   void report (bool passed) const
   {
      unit_test_report(&m_UnitTest, (cbool_t) passed);
   }

   /**
    * \accessor unit_test_version()
    */

   void version () const
   {
      unit_test_version(&m_UnitTest);
   }

private:

   void destroy ();

};             /* class cut         */

}              /* namespace xpc     */

#endif         /* XPC_CUT_HPP       */

/*
 * cut.hpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
