/**
 * \file          cut_status.cpp
 * \library       libxpccut++
 * \author        Chris Ahlstrom
 * \date          2008-03-27
 * \updates       2015-10-05
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides the declarations for the xpc::cut_status class.
 *    Also see the cut_status.cpp module and the cut_status.hpp module
 *    for more information.
 */

#include <xpc/cut_status.hpp>          /* xpc::cut_status                     */
#include <xpc/cut_options.hpp>         /* xpc::cut_options                    */

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* fprintf()                           */
#endif

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

static unit_test_options_t gs_Dummy_Options;

/**
 *    Creates a bare-bones unit-test status object
 *    By default, m_Can_Run is set to 'true'.  The m_Status member structure
 *    is set up via a call to unit_test_status_init(), and the result of
 *    that call sets m_Is_Valid.
 *
 * \note
 *    -  This function currently calls trace() as an aid to debugging.  In
 *       the future, we might remove that call.
 *    -  This object has no group, case, or sub-test name.  Other values are
 *       zeroed, too, so this object generally requires a few setter calls
 *       in order to be used.  Prefer the principal constructor.
 *
 * \defaultctor
 *    Provides the default class constructor.  The default constructor
 *    allows this class to (among other things) appear in an array.  See
 *    \ref xpc_nice_classes for more information.
 *
 * \posix
 *    Supported under POSIX.  All functions in this class are supported.
 *
 * \win32
 *    Supported under Windows.  All functions in this class are supported.
 *
 * \unittests
 *    -  cut_unit_test_02_01()
 */

cut_status::cut_status ()
 :
   m_Status       (),
   m_Can_Run      (true),
   m_Is_Valid     (false) // xpccut_boolcast(unit_test_status_init(&m_Status)))
{
   trace("xpc::cut_status()");
   m_Is_Valid = xpccut_boolcast
   (
      unit_test_status_initialize
      (
         &m_Status, &gs_Dummy_Options,
         1,                            /* dummy, but valid, testgroup         */
         1,                            /* dummy, but valid, testcase          */
         "",                           /* dummy groupname                     */
         ""                            /* dummy casename                      */
      )
   );
}

/**
 *    Provides the most-often used constructor.
 *    This constructor mirrors the C function unit_test_status_initialize()
 *    very closely.
 *
 *    Note that we cannot base the setting of the validity flag, m_Is_Valid,
 *    directly on the return value of unit_test_status_initialize(), because
 *    that function will return 'false' if the test group or test case is to
 *    be skipped.
 *
 *    Thus, to determine validity, we (unfortunately) have to repeat some of
 *    the same tests make in unit_test_status_initialize().
 *
 * \param options
 *    The C++ test-options structure.  It's cut_options::options() member is
 *    pass to the unit_test_status_initialize() function.
 *
 * \param testgroup
 *    Optional single-test group.  Passed to the
 *    unit_test_status_initialize() function.
 *
 * \param testcase
 *    Optional single-test case.  Passed to the
 *    unit_test_status_initialize() function.
 *
 * \param groupname
 *    Optional group description.  Passed to the
 *    unit_test_status_initialize() function using the c_str() accessor.
 *
 * \param casename
 *    Optional case description.  Passed to the
 *    unit_test_status_initialize() function using the c_str() accessor.
 *
 * \unittests
 *    -  cut_unit_test_01_01() etc.
 *    -  This function is tested in every unit-test function that follows
 *       our canonical format, in which a cut_status object is created based
 *       on the current option, is altered a bit by the unit-test, and then
 *       returned to the caller.
 */

cut_status::cut_status
(
   const cut_options & options,
   int testgroup,
   int testcase,
   const std::string & groupname,
   const std::string & casename
) :
   m_Status       (),
   m_Can_Run      (false),
   m_Is_Valid     (false)
{
   m_Is_Valid = (testgroup > 0) && (testcase > 0);
   if (m_Is_Valid)
   {
      bool ok = xpccut_boolcast
      (
         unit_test_status_initialize
         (
            &m_Status, &options.options(), testgroup, testcase,
            groupname.c_str(), casename.c_str()
         )
      );
      trace("xpc::cut_status(const cut_options & ...)");
      if (ok)
      {
         m_Can_Run = true;
      }
      else if (! xpccut_is_silent())
      {
         fprintf
         (
            stdout, "  %s: %s %d, %s %d\n",
            _("unit-test skipped"), _("group"), testgroup,
            _("case"), testcase
         );
      }
   }
   else
      xpccut_errprint_func(_("invalid test group or test case"));
}

/**
 *    Provides a standard copy constructor.
 *
 * \copyctor
 *    This function makes a copy of the given object.
 *    The copy constructor can allows this class to be deep-copied, and this
 *    would be the most common case.
 *
 * \param source
 *    The object from which the copy is to be made.
 *
 * \unittests
 *    -  The copy structure is indirectly tested in every unit-test
 *       function, as it is the return value of each unit-test function.
 */

cut_status::cut_status (const cut_status & source)
 :
   m_Status       (source.m_Status),
   m_Can_Run      (source.m_Can_Run),
   m_Is_Valid     (source.m_Is_Valid)
{
   trace("xpc::cut_status(const cut_status &)");
}

/**
 *    Provides a standard assignment operator.
 *    This function can allow this class to be deep-assigned, and that is
 *    the most common situation.
 *
 *    Since the destination object already exists, any resources in that
 *    object that were created [e.g. by calling a create() function] must
 *    first be deleted [e.g. by calling a destroy() function].
 *    Then, each member is obtained from the source object.
 *    Lastly, any dynamic resources are created, based on the
 *    already-assigned members.
 *
 * \param source
 *    The object from which the assignment is to be made.
 *
 * \return
 *    A reference to the destination object is returned.  This allows the
 *    "equals" operator to be called in a string of assignments.
 *
 * \unittests
 *    -  TBD.
 *
 */

cut_status &
cut_status::operator = (const cut_status & source)
{
   if (this != &source)
   {
      m_Status    = source.m_Status;
      m_Can_Run   = source.m_Can_Run;
      m_Is_Valid  = source.m_Is_Valid;
   }
   trace("cut_status::operator =()");
   return *this;
}

/**
 *    Provides the class destructor.
 *    See \ref xpc_nice_classes for more information.
 *
 * \destructor
 *    This function destroys the resources created by the constructor.
 *
 * \dtor
 *    This function destroys the resources created by the constructor.
 *
 * \unittests
 *    -  TBD.
 */

cut_status::~cut_status ()
{
   m_Is_Valid = false;                 /* prevent last-instant usage          */
}

}              /* namespace xpc */

/*
 * cut_status.cpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
