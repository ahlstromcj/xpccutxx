/**
 * \file          cut.cpp
 * \library       libxpccut++
 * \author        Chris Ahlstrom
 * \date          2008-03-28
 * \updates       2015-10-05
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides the definitions and functions for the xpc::cut class.
 *    Also included are a handful of stand-alone functions.
 *    Also see the cut.cpp module and the cut.hpp module
 *    for more information.
 */

#include <iostream>                    /* std::cout and std::cerr             */
#include <xpc/cut.hpp>                 /* the xpc::cut class                  */

/**
 * \doxygen
 *    If Doxygen is running, define WIN32, to read in the documentation for
 *    any Windows-specific code.  Also define POSIX, for a similar reason.
 *    See doc/dox/doxygen.cfg for more information.
 */

#ifdef DOXYGEN
#define WIN32
#define POSIX
#endif

namespace xpc
{

/**
 *    Constructs an uninitialized unit-test object.
 *    Internally, this function calls the C function unit_test_init() from
 *    the xpccut library to set up the m_UnitTest member object.
 *
 *    Provides the default class constructor.  The default constructor
 *    allows this class to (among other things) appear in an array.  See
 *    \ref xpc_nice_classes for more information.
 *
 * \defaultctor
 *    This function creates a default object.
 *
 * \posix
 *    Supported under POSIX.  Every function in this library will compile
 *    under Linux, and probably under other UNIXen (not yet tried).
 *
 * \win32
 *    Supported under Windows.  Every function in this library will compile
 *    under current versions of Visual Studio, although this has been tested
 *    only up to <i>Visual Studio .NET 2002</i>.
 *
 * \unittests
 *    TBD.
 */

cut::cut ()
 :
   m_UnitTest           (),
   m_UnitTest_List      (),
   m_UnitTest_Options   (),
   m_Is_Valid           (xpccut_boolcast(unit_test_init(&m_UnitTest))),
   m_Run_C_Unit_Tests   (false)
{
   /* That's all that is needed! */
}

/**
 *    Sets up the unit-test framework with all the information needed to run
 *    it.
 *
 * \ctor
 *    This function is the \e principal constructor.
 *    See \ref xpc_nice_classes for more information.
 *
 *    Internally, it calls initialize() to do the work, passing along all of
 *    its arguments to that function.
 *
 * \param argc
 *    The standard main() argument count.
 *
 * \param argv
 *    The main() function's argument list.
 *
 * \param appname
 *    Brief name of the application.  This name is almost equivalent to the
 *    name of the executable.
 *
 * \param appversion
 *    The version information of the application.
 *
 * \param addedhelp
 *    Application-specific help text.  The \a addedhelp parameter is useful
 *    for providing help for additional options specific to the test
 *    application.  The unit-test library already has help for its own
 *    options.
 *
 * \unittests
 *    TBD.
 */

cut::cut
(
   int argc,
   char * argv [],
   const std::string & appname,
   const std::string & appversion,
   const std::string & addedhelp
) :
   m_UnitTest           (),
   m_UnitTest_List      (),
   m_UnitTest_Options   (),
   m_Is_Valid           (false),
   m_Run_C_Unit_Tests   (false)
{
   initialize(argc, argv, appname, appversion, addedhelp);
}

/**
 *    Provides a rigorous copy constructor.
 *
 * \copyctor
 *    This function makes a copy of the given object.  The copy constructor
 *    can allows this class to be deep-copied, and this would be the most
 *    common case.
 *
 *    See \ref xpc_nice_classes for more information.
 *
 *    Internally, it calls initialize() to do the work, passing along empty
 *    command-line arguments and the name, version, and help strings that
 *    are part of the m_UnitTest data structure.
 *
 * \param source
 *    The object from which the copy is to be made.
 *
 * \unittests
 *    TBD.
 */

cut::cut (const cut & source)
 :
   m_UnitTest           (source.m_UnitTest),
   m_UnitTest_List      (source.m_UnitTest_List),
   m_UnitTest_Options   (source.m_UnitTest_Options),
   m_Is_Valid           (source.m_Is_Valid),
   m_Run_C_Unit_Tests   (source.m_Run_C_Unit_Tests)
{
   /**
    * \todo
    *    Step through this code under the debugger to see if the
    *    following call is redundant.
    */

   initialize
   (
      0, nullptr,                                              /* argc, argv  */
      source.m_UnitTest.m_Test_Application_Name,
      source.m_UnitTest.m_Test_Application_Version,
      source.m_UnitTest.m_Additional_Help
   );
}

/**
 *    Provides a principal assignment operator.
 *    This function is a canonical principal assignment operator, as
 *    described by Scott Meyers in his books on C++.  It performs the
 *    following operations:
 *
 *       -# Verify that the object is not assigning to itself before
 *          attempting the assignment.
 *       -# Delete the resources of this object, which is the destination
 *          object.  This is done by calling the unit_test_destroy() C
 *          function from the XPC CUT library (libxpccut).
 *       -# Construct any new resources that need creating.  Here, this is
 *          accomplished by calling the initialize() member function.
 *       -# Return the <tt>*this</tt> object, to support multiple
 *          assignments in one line of code.
 *
 *    See \ref xpc_nice_classes for more information.
 *
 * \param source
 *    The object from which the assignment is to be made.
 *
 * \return
 *    A reference to the destination object is returned.  This allows the
 *    "equals" operator to be called in a string of assignments.
 *
 * \unittests
 *    TBD.
 */

cut &
cut::operator = (const cut & source)
{
   if (this != &source)
   {
      destroy();                                            /* m_Is_Valid set */
      m_UnitTest           = source.m_UnitTest;
      m_UnitTest_List      = source.m_UnitTest_List;
      m_UnitTest_Options   = source.m_UnitTest_Options;
      m_Run_C_Unit_Tests   = source.m_Run_C_Unit_Tests;
      initialize                                            /* m_Is_Valid set */
      (
         0, nullptr,                                        /* argc, argv     */
         source.m_UnitTest.m_Test_Application_Name,
         source.m_UnitTest.m_Test_Application_Version,
         source.m_UnitTest.m_Additional_Help
      );
   }
   return *this;
}

/**
 *    Provides the class destructor.
 *    See \ref xpc_nice_classes for more information.
 *
 * \dtor
 *    This destructor function destroys the resources created by the
 *    constructor.  This is done by calling destroy().
 *
 * \unittests
 *    TBD.
 */

cut::~cut ()
{
   destroy();
}

/**
 *    Provides common code for the class destructor and the principal
 *    assignment operator.
 *
 *    This function destroys the resources created by the
 *    constructor.  This is done by calling the C function
 *    unit_test_destroy() with m_UnitTest as an argument.
 *
 * \note
 *    We unset the m_Is_Valid flag right away to disable the class usage by
 *    careful threads that check their object before using it.
 *
 * \unittests
 *    TBD.
 */

void
cut::destroy ()
{
   m_Is_Valid = false;
   (void) unit_test_destroy(&m_UnitTest);
}

/**
 *    Initializes the internals of this class with command-line options and
 *    application-specific tags for name, version, and help.
 *
 *    Internally, it calls unit_test_initialize() to set up the m_UnitTest
 *    member object.  If this setup succeeds, then the C options structure
 *    of that member is copied into the m_UnitTest_Options member.
 *
 * \param argc
 *    The standard main() argument count.
 *
 * \param argv
 *    The main() function's argument list.
 *
 * \param appname
 *    Brief name of the application.  This name is almost equivalent to the
 *    name of the executable.
 *
 * \param appversion
 *    The version information of the application.
 *
 * \param apphelp
 *    Application-specific help text.  The \a addedhelp parameter is useful
 *    for providing help for additional options specific to the test
 *    application.
 *
 * \warning
 *    If the \a argc and \a argv parameters indicate a null argument, fake
 *    values are passed to unit_test_initialize().  This works because the
 *    results of parsing the command-line have already been incorporated
 *    into the current object when the principal constructor was called.
 */

void
cut::initialize
(
   int argc,
   char * argv [],
   const std::string & appname,
   const std::string & appversion,
   const std::string & addedhelp
)
{
   /**
    *    This function has to handle null command-line arguments by faking
    *    them.  This is a bit of a design flaw that we put up with in order
    *    to be able to use the copy constructor and principle assignment
    *    operator in a sensible manner.
    */

   static const char * const dummy[2] = { "dummy", nullptr };
   if ((argc == 0) || cut_is_nullptr(argv))
   {
      argc = 1;
      argv = const_cast<char **>(dummy);
      xpccut_infoprint_ex(_("faking command-line arguments"), __func__);
   }
   m_Is_Valid = xpccut_boolcast
   (
      unit_test_initialize          /* parsing and setup for a unit_test_t    */
      (
         &m_UnitTest, argc, argv,
         appname.c_str(), appversion.c_str(), addedhelp.c_str()
      )
   );
   if (m_Is_Valid)                  /* copy the parsed unit_test_options_t    */
   {
      m_UnitTest_Options.copy_options(m_UnitTest.m_App_Options);
   }
}

/**
 *    Loads the address of a unit-test function into the test container.
 *    Although registering a unit-test function is extra work, it allows an
 *    incomplete test function to be in-progress for coding, while allowing
 *    the completed test functions to be run.
 *
 *    It also makes it easier for the caller to add command-line options
 *    that skip vast swatches of unit-tests, by simply skipping the loading
 *    of those functions.
 *
 * \param test
 *    A unit-test C++ function pointer, with a test_function function
 *    signature.
 *
 * \return
 *    Returns 'true' if the test-function pointer is valid.  If the called
 *    has previously called c_load() to load C unit test functions, then
 *    'false' is returned, as we cannot cope with testing both kinds of
 *    functions in the same object.  <i> Testing of both types can be done
 *    by creating two cut objects and using them. </i>
 *
 * \unittests
 *    TBD.
 */

bool
cut::load
(
   test_function test
)
{
   bool result = cut_not_nullptr(test);
   if (result)
   {
      result = ! m_Run_C_Unit_Tests;
      if (! result)
         xpccut_errprint_func(_("cannot mix C and C++ unit-tests"));
   }
   else
      xpccut_errprint_func(_("null pointer"));

   if (result)
   {
      if (m_Is_Valid)
      {
         size_t size = m_UnitTest_List.size();
         m_UnitTest_List.push_back(test);
         result = m_UnitTest_List.size() == (size+1);
         if (result)
            result = xpccut_boolcast(unit_test_cpp_load_count(&m_UnitTest));
         else
            xpccut_errprint_func(_("failed"));
      }
      else
         xpccut_errprint_func(_("the unit-test object is invalid"));
   }
   return result;
}

/**
 *    Load a C function (instead of a C++ function) for testing.
 *
 * \warning
 *    Right now this is just an idea to try; I have no idea how well it can
 *    work.
 *
 * \param test
 *    A unit-test function written in C, with a unit_test_func_t function
 *    signature.
 *
 * \return
 *    Returns 'true' if the function pointer was valid and the call to
 *    unit_test_load() succeeded.
 *
 * \sideeffect
 *    If the load succeeds, the m_Run_C_Unit_Tests is set to 'true'.
 *
 * \unittests
 *    TBD.
 */

bool
cut::c_load
(
   unit_test_func_t test   /**< A unit-test function written in C.            */
)
{
   bool result = cut_not_nullptr(test);
   if (result)
   {
      if (m_Is_Valid)
      {
         result = xpccut_boolcast(unit_test_load(&m_UnitTest, test));
         if (result)
            m_Run_C_Unit_Tests = true;
      }
      else
         xpccut_errprint_func(_("the unit-test object is invalid"));
   }
   else
      xpccut_errprint_func(_("null pointer"));

   return result;
}

/**
 *    Runs a single C++ unit-test function.
 *    After the unit-test returns, the C function
 *    unit_test_run_a_test_after() is run to finish timing the test and
 *    printing out the results, if applicable.
 *
 * \param test
 *    A unit-test C++ function pointer, with a test_function function
 *    signature.
 *
 * \return
 *    Returns the unit-test status object (a cut_status instantiation) that
 *    resulted from running the unit-test.  Note that some of this status
 *    may be obtained from the unit-test function itself.
 *
 * \unittests
 *    TBD.
 */

cut_status
cut::run_a_test
(
   test_function test
)
{
   cut_status result;
   intptr_t int_test = reinterpret_cast<intptr_t>(test);
   if (m_Is_Valid)
   {
      cbool_t ok = unit_test_run_a_test_before(&m_UnitTest, int_test);
      if (ok)
      {
         /**
          * Unfortunately, the functions in the C unit-test library that
          * affect the current options act on the options stored in the
          * unit_test_t structures.
          *
          * In the current structuring of the C++ unit-test library, any
          * changes made have to be copied to the C++ version of the options
          * structure.  And the option structure is what keeps track of the
          * test number -- unfortunately.  So we have to provide a
          * cut_options::copy_options() function and use it.
          */

         m_UnitTest_Options.copy_options(m_UnitTest.m_App_Options);
         result = (*test)(m_UnitTest_Options);
         unit_test_run_a_test_after(&m_UnitTest, &result.m_Status);
      }
   }
   else
      xpccut_errprint_func(_("the unit-test object is invalid"));

   return result;
}

/**
 *    Runs all of the unit-tests that have been loaded by the load()
 *    function.
 *
 *    Although this function runs a lot like the C version, and uses a lot
 *    of the functionality of the C module unit_test.c, there are quite a
 *    few differences in how the C++ version works, internally.
 *
 *    Compare this function to the C function unit_test_run().
 *    What \e are the differences?
 *
 *       -  The C structures are replaced with C++ classes:
\verbatim
               unit_test_t             xpc::cut
               unit_test_options_t     xpc::cut_options
               unit_test_status_t      xpc::cut_status
               unit_test_func_t        xpc::test_function
               unit_test_list_t        xpc::cut_test_list
\endverbatim
 *       -  The status structure is a member of cut, instead of being
 *          provided by the caller.
 *
 * \warning
 *    This function has to be kept in synchrony with changes to the C
 *    function unit_test_run() in the xpccut library.
 *
 * \return
 *    Returns 'true' if all setup and unit-tests ran successfully.
 *
 * \unittests
 *    TBD.
 */

bool
cut::run ()
{
   bool result = true;                       /* set to true for our algorithm */
   if (m_Run_C_Unit_Tests)                   /* run a bunch of plain C tests? */
   {
      result = xpccut_boolcast(unit_test_run(&m_UnitTest));          /* yes   */
   }
   else
   {
      unit_test_t * tests = &m_UnitTest;
      int length = unit_test_run_init(tests);
      if (length == 0)
         result = false;
      else
      {
         cbool_t cresult = ctrue;            /* this is a C "boolean"         */
         int testnumber = 0;
         while ((testnumber = unit_test_next_test(tests)) >= 0)
         {
            cut_status testresult = run_a_test(m_UnitTest_List[testnumber]);
            if (unit_test_check_subtests(&m_UnitTest, &testresult.status()) < 0)
               break;

            if (unit_test_dispose_of_test(tests, &testresult.m_Status, &cresult))
               break;
         }

         /*
          * The \e cresult value is meaningless, since it ends up being the
          * result of the last unit-test.  We have to base the overall
          * result of the unit-test application on the number of failures
          * being greater than 0.
          */

         result = unit_test_failures(&m_UnitTest) == 0;    /* no failures?   */
      }
      unit_test_post_loop(tests, result);
   }
   return result;
}

/**
 *    Provides a static helper function to show an exclamation message.
 *
 *    The message is shown if the --verbose option is used, and the --silent
 *    option is not used.
 *
 *    This is a function specific to this unit-test application.
 */

void
cut::exclaim
(
   const xpc::cut_options & options,   /**< The options object to examine.    */
   const std::string & message         /**< The translated message string.    */
)
{
   if (options.is_verbose() && ! xpccut_is_silent())
      std::cout << "! " << message << std::endl;
}

/**
 *    Provides a static helper function to show an informational message.
 *    The message is shown if the --verbose option is used, and the --silent
 *    option is not used.
 *
 *    This is a function specific to this unit-test application.
 */

void
cut::inform
(
   const xpc::cut_options & options,   /**< The options object to examine.    */
   const std::string & message         /**< The translated message string.    */
)
{
   if (options.is_verbose() && ! xpccut_is_silent())
      std::cout << "* " << message << std::endl;
}

/**
 *    Provides a static helper function to show an informational message.
 *
 *    The message is shown no matter what, unless the --silent option is
 *    used.
 */

void
cut::show
(
   const xpc::cut_options & options,   /**< The options object to examine.    */
   const std::string & message         /**< The translated message string.    */
)
{
   if (options.show_values() && ! xpccut_is_silent())
      std::cout << "= " << message << std::endl;
}

/**
 *    Provides a static helper function to show a message indicating that
 *    the failure was deliberate.
 *
 *    This output helps reassure the user that all is really well.  It
 *    appears only if the --show-progress option is in effect.
 */

void
cut::show_deliberate_failure
(
   const xpc::cut_options & options    /**< The options object to check.      */
)
{
   exclaim(options, _("! This FAILURE is deliberate."));
}

}              /* namespace xpc */

/*
 * cut.cpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
