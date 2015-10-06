#!/usr/bin/sed -i -f
#
#  Execution:
#     sed -i -f simplify *.cpp *.c *.h

# s/XPCCUT/xpccut/

s/ \* End of /\r * /

# Remove extra carriage returns and trailing spaces and tabs.

s///

# Remove trailing spaces

s/  *$//g

# Delete the first long line of asterisks, and next line
 
/^\/\*\*\*\*\*\*.*\*$/{N;d;}

# Replace a long line of dashes of format " *-----------*//**"
# with just "/**"

s/^ \*-----------.*\/\/\*\*/\/**/

# Replace a long line of dashes of format " *//*-----------*/"
# with just " */"

s/^ \*\/\/\*------------.*\*\// *\//

# Replace a long line of asterisks of format "/**********" with
# just "/*".  But we like just deleting those lines and the next better.
#
# s/\/\*\*\*\*\*\*\*.*\*$/\/*/

# s/ \* End of / * /

/ Local variables:/d

/ End:$/d

s/ \*----------------------.*---------------------$/ */

s/ \*----------------------.*---------------------\*\/$/ *\//

#-------------------------------------------------------------------------------
# Convert "// " at line's start to " * "
#-------------------------------------------------------------------------------

s/^\/\/ / * /

#-------------------------------------------------------------------------------
# Convert "//" at line's start to " *"
#-------------------------------------------------------------------------------

s/^\/\// */

#-------------------------------------------------------------------------------
# Shorten long lines having nothing but spaces, that end in *
#-------------------------------------------------------------------------------

s/      *\*$/ */

#-------------------------------------------------------------------------------
# End of simplify.sed
#-------------------------------------------------------------------------------
# vim: ts=3 sw=3 et ft=sed
#-------------------------------------------------------------------------------
