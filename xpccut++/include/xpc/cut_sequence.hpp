#if ! defined XPC_CUT_SEQUENCE_HPP
#define XPC_CUT_SEQUENCE_HPP

/**
 * \file          cut_sequence.hpp
 * \library       libxpccut++
 * \author        Chris Ahlstrom
 * \date          2010-01-04
 * \updates       2015-10-05
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    The xpc::Sequencing class is meant to provide a way of generating
 *    sequence-diagram information.  Also see the cut_sequence.cpp module and
 *    the xpc::Sequencing class.
 *
 */

#include <xpc/macros_subset.h>         // nullptr
#include <string>                      // std::string

#ifdef DOXYGEN
#define XPC_SEQUENCE_DEBUG             // turn on some documentation
#endif

#ifdef XPC_DEBUG_SEQUENCE
#define XPC_SEQUENCE_DEBUG             // turn on sequence diagramming
#endif

#ifdef XPC_SEQUENCE_DEBUG

/**
 *    This macro provides a way to conditionally declare a variable that
 *    will mark the entering and leaving of a scope, using the name of the
 *    function.
 *
 * \note
 *    When we forget to create an actual variable, what we found is that the
 *    constructor and destructor are called in quick succession, which kind
 *    of screws up our attempt at logging the correct sequence of event.
 */

#define XPC_SEQUENCE                   xpc::Sequencing xpc_seqq(__func__, true)

/**
 *    This macro provides a way to conditionally declare a variable that
 *    will mark the entering and leaving of a scope.
 */

#define XPC_SEQUENCE_MARK(x)           xpc::Sequencing xpc_seqm(x, true)

#else
#define XPC_SEQUENCE
#define XPC_SEQUENCE_MARK(x)
#endif

namespace xpc
{

/**
 *    Provides a kludge that can be used in generating correct sequence
 *    diagrams.
 *
 *    This class works in a clumsy, intrusive manner.  To use it, place the
 *    following call at the very beginning of all functions that you wish to
 *    track:
 *
\verbatim
         XPC_SEQUENCE;
\endverbatim
 *
 */

class Sequencing
{

private:

   /**
    *    Provides the tag name provided to the constructor.
    *
    *    This is normally the name of the function or block to be sequenced.
    */

   std::string m_Tag_Name;

   /**
    *    Provides a flag to indicate if the sequencing should be shown
    */

   bool m_Do_Show;

   /**
    *    Provides a static counter that covers all instances.
    */

   static int gm_Sequence_Value;

private:

   Sequencing (const Sequencing &);
   Sequencing & operator = (const Sequencing &);

public:

   Sequencing
   (
      const char * tag        = nullptr,
      bool doshow             = false
   );

   virtual ~Sequencing ();

   /*
    * \setter gm_Sequence_Value
    *
    *    We can rely on the compiler/linker to have this value
    *    initialized to zero in a timely fashion, so this function really is
    *    not needed.
    *
    *       static void initialize () { gm_Sequence_Value = 0; }
    */

};             // Sequencing

}              // namespace xpc

#endif         // XPC_CUT_SEQUENCE_HPP

/*
 * cut_sequence.hpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
