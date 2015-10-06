/**
 * \file          cut_options.cpp
 * \library       libxpccut++
 * \author        Chris Ahlstrom
 * \date          2008-03-27
 * \updates       2015-10-05
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides the declarations for the xpc::cut_options class.
 *    Also see the cut_options.cpp module and the cut_options.hpp module
 *    for more information.
 */

#include <xpc/cut_options.hpp>      /* xpc::cut_options                       */

/**
 * \doxygen
 *    If Doxygen is running, define WIN32, to read in the documentation for
 *    the Windows-specific code.  See doc/doxygen.cfg for more information.
 */

#ifdef DOXYGEN
#define WIN32
#define POSIX
#endif

namespace xpc
{

/**
 *    Provides the default class constructor.
 *    The default constructor allows this class to (among other things)
 *    appear in an array.
 *
 *    See \ref xpc_nice_classes for more information.
 *
 *    This constructor passes the m_Options structure to the C function
 *    unit_test_options_init().
 *
 * \defaultctor
 *    This function creates a default object.
 *
 * \param simulate
 *    If set to 'true', the tests will show the word "simulated" in the
 *    output, to clarify to the user what is happening.
 *
 * \unittests
 *    TBD.
 */

cut_options::cut_options
(
   bool simulate
) :
   m_Options      (),
   m_Is_Valid     (xpccut_boolcast(unit_test_options_init(&m_Options)))
{
   is_simulated(simulate);
}

/**
 *    Provides the most-often used constructor.
 *
 * \ctor
 *    This constructor accepts command-line parameters and the name,
 *    version, and help text for the class.
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
 * \param additionalhelp
 *    Application-specific help text.  The \a addedhelp parameter is useful
 *    for providing help for additional options specific to the test
 *    application.  The unit-test library already has help for its own
 *    options.
 *
 * \note
 *    There is no constructor that provides a long list of parameters that
 *    correspond to every possible test option.  The most common use case is
 *    getting the new options from the command line, and that is quite
 *    enough to support at this time..
 *
 * \warning
 *    The unit_test_options_parse() function returns 'false' if the
 *    command-line arguments contain a "--version" or "--help" option.  This
 *    is so that callers will not continue to perform any unit-testing,
 *    since only version output or help output is wanted.
 *
 *    It is important to test the constructed object using the
 *    cut_options::valid() function before proceeding to execute the
 *    unit-test.
 *
 * \unittests
 *    TBD.
 */

cut_options::cut_options
(
   int argc,
   const char * argv [],
   const std::string & testname,
   const std::string & version,
   const std::string & additionalhelp
) :
   m_Options      (),
   m_Is_Valid     (xpccut_boolcast(unit_test_options_init(&m_Options)))
{
   if (m_Is_Valid)
   {
      char ** nc_argv = const_cast<char **>(argv);
      m_Is_Valid = xpccut_boolcast
      (
         unit_test_options_parse
         (
            &m_Options, argc, nc_argv,
            testname.c_str(), version.c_str(), additionalhelp.c_str()
         )
      );
   }
}

/**
 *    Provides an alternate constructor more robust against null pointer
 *    parameters.
 *
 *    This version overloads the std::string version in order to allow the
 *    checking of null pointers for the string parameters.  Unfortunately,
 *    the std::string constructor will accept a null pointer, but then the
 *    results of using it are undefined.  [In the GNU gcc compiler, for
 *    example, the code builds and runs, but an std::logic_error exception
 *    is thrown.]
 *
 * \ctor
 *    This constructor accepts command-line parameters and the name,
 *    version, and help text for the class.
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
 * \param additionalhelp
 *    Application-specific help text.  The \a addedhelp parameter is useful
 *    for providing help for additional options specific to the test
 *    application.  The unit-test library already has help for its own
 *    options.
 *
 * \warning
 *    The unit_test_options_parse() function returns 'false' if the
 *    command-line arguments contain a "--version" or "--help" option.  This
 *    is so that callers will not continue to perform any unit-testing,
 *    since only version output or help output is wanted.
 *
 *    It is important to test the constructed object using the
 *    cut_options::valid() function before proceeding to execute the
 *    unit-test.
 *
 * \unittests
 *    TBD.
 */

cut_options::cut_options
(
   int argc,
   const char * argv [],
   const char * testname,
   const char * version,
   const char * additionalhelp
) :
   m_Options      (),
   m_Is_Valid     (xpccut_boolcast(unit_test_options_init(&m_Options)))
{
   if (m_Is_Valid)
   {
      char ** nc_argv = const_cast<char **>(argv);
      if (cut_is_nullptr(testname))
         testname = "";

      if (cut_is_nullptr(version))
         version = "";

      if (cut_is_nullptr(additionalhelp))
         additionalhelp = "";

      m_Is_Valid = xpccut_boolcast
      (
         unit_test_options_parse
         (
            &m_Options, argc, nc_argv, testname, version, additionalhelp
         )
      );
   }
}

/**
 *    Provides the copy constructor for this class.
 *
 * \copyctor
 *    This function makes a copy of the given object.
 *
 *    The copy constructor can allows this class to be deep-copied, and this
 *    would be the most common case.
 *
 *    See \ref xpc_nice_classes for more information.
 *
 * \warning
 *    The fields of the unit_test_options_t structure are all integers,
 *    booleans, or in-line character arrays.  Therefore, simple assignment
 *    is all that is necessary to copy the m_Options member.  However,
 *    if pointers get added to that structure, then we will need to writing
 *    some allocator/copier functions.
 *
 * \param source
 *    The object from which the copy is to be made.
 *
 * \unittests
 *    TBD.
 */

cut_options::cut_options (const cut_options & source)
 :
   m_Options      (source.m_Options),
   m_Is_Valid     (source.m_Is_Valid)
{
   /* No other functionality necessary. */
}

/**
 *    Provides the main assignment operator.
 *    This function can allow this class to be deep-assigned, and that is
 *    the most common situation.
 *
 *    Since the destination object already exists, any resources in that
 *    object that were created [e.g. by calling a create() function] must
 *    first be deleted [e.g. by calling a destroy() function].
 *
 *    Then, each member is obtained from the source object.
 *    Lastly, any dynamic resources are created, based on the
 *    already-assigned members.  However, in this case, there are none.  See
 *    the cut_options copy constructor for more information.
 *
 *    See \ref xpc_nice_classes for more information about assignment
 *    operators.
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

cut_options &
cut_options::operator = (const cut_options & source)
{
   if (this != &source)
   {
      m_Options   = source.m_Options;
      m_Is_Valid  = source.m_Is_Valid;
   }
   return *this;
}

/**
 *    Provides the class destructor.
 *
 * \dtor
 *    This function destroys the resources created by the constructor.
 *    This particular function merely falsifies the m_Is_Valid flag.
 *
 *    See \ref xpc_nice_classes for more information.
 *
 * \unittests
 *    TBD.
 */

cut_options::~cut_options ()
{
   m_Is_Valid = false;                 /* prevent any chance of usage */
}

}              /* namespace xpc */

/*
 * cut_options.cpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
