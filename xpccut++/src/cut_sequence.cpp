/**
 * \file          cut_sequence.cpp
 * \library       libxpccut++
 * \author        Chris Ahlstrom
 * \date          2010-01-04
 * \updates       2015-10-05
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides the implementations for methods of the xpc::Sequencing class.
 *    Also see the cut_sequence.h module and the xpc::Sequencing class.
 */

#include <xpc/portable_subset.h>       /* C::xpccut_infoprintf()              */
#include <xpc/cut_sequence.hpp>        /* class xpc::Sequencing               */
#include <stdio.h>                     /* sprintf() [gasp!]                   */

namespace xpc
{

int Sequencing::gm_Sequence_Value = 0;

/**
 * \param tag
 *    Provides the tag to identify the part of the sequence information
 *    being logged.
 *
 * \param doshow
 *    Provides an indicator if the output is to be seen, or not.
 */

Sequencing::Sequencing (const char * tag, bool doshow)
 :
   m_Tag_Name     (cut_not_nullptr(tag) ? tag : "???"),
   m_Do_Show      (cut_not_nullptr(tag) ? doshow : true)
{
   if (m_Do_Show)
   {
      char temp[64];
      sprintf(temp, "%4d:  IN: %s", gm_Sequence_Value, m_Tag_Name.c_str());
      xpccut_infoprint(temp);
      gm_Sequence_Value++;
   }
}

/**
 * \unittests
 *    TBD
 */

Sequencing::~Sequencing ()
{
   if (m_Do_Show)
   {
      char temp[64];
      sprintf(temp, "%4d: OUT: %s", gm_Sequence_Value, m_Tag_Name.c_str());
      xpccut_infoprint(temp);
      gm_Sequence_Value++;
   }
}

}           // namespace xpc

/*
 * cut_sequence.cpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
