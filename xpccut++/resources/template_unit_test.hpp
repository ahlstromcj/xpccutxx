#ifndef $MODULEUC$_UNIT_TEST_HPP
#define $MODULEUC$_UNIT_TEST_HPP

/**
 * \file          $MODULE$_unit_test.hpp
 * \library       $PACKAGE$
 * \author        $AUTHOR$
 * \date          $DATE$
 * \updates       $DATE$
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides unit tests for the $MODULE$ module of the
 *    $PACKAGE$ codebase.  See the $MODULE$_unit_test.cpp for details.
 */

#include <xpc/cut.hpp>                 /* xpc::cut unit-test class            */

#include <$MODULE$.hpp>

extern xpc::cut_status $MODULE$_unit_test_$GNUMBER$_$CNUMBER$
(
   const xpc::cut_options & options
);

#endif   // $MODULEUC$_UNIT_TEST_HPP

/*
 * $MODULE$_unit_test.hpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
