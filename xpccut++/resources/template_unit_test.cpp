/**
 * \file          $MODULE$_unit_test.cpp
 * \library       $PACKAGE$
 * \author        $AUTHOR$
 * \date          $DATE$
 * \updates       $DATE$
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides unit tests for the $MODULE$ module of the
 *    $PACKAGE$ codebase.
 *
 *    $PACKAGE$ == library, application, codebase, or other body of modules
 *    $MODULE$ == file, class, or XPCCUT "group"
 *    $FUNCTION$ == function or XPCCUT "case"
 *    $NAMESPACE$ is optional.
 *    $GNUMBER$ == group number, $MODULE$$ == group name
 *    $CNUMBER$ == case number, $FUNCTION$ == case name
 */

#include "$MODULE$_unit_test.hpp"

/**
 *    Provides a basic "smoke test" for the $MODULE$ module.  A smoke test
 *    is a test of the basic functionality of the object or function.  It is
 *    an easy test that makes sure the code has basic integrity.  This test is
 *    about the simplest unit test function that can be written.
 *
 * \group
 *    1. $NAMESPACE$::$MODULE$
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
 *    -  seq64::$MODULE$::$FUNCTION$()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 */

xpc::cut_status
$MODULE$_unit_test_$GNUMBER$_$CNUMBER$ (const xpc::cut_options & options)
{
   xpc::cut_status status(options, $GNUMBER$, $CNUMBER$, "$MODULE$", _("Smoke Test"));
   bool ok = status.valid();                       /* invalidity not an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         if (status.next_subtest("$MODULE$::$FUNCTION$()"))
         {
            seq64::click c;
            ok = ! c.is_press();
            status.pass(ok);
         }
      }
   }
   return status;
}

/*
 * $MODULE$_unit_test.cpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
