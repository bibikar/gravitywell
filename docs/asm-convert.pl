# ads2gas.pl
# Author: Eric Fung (efung (at) acm.org)
#
# Convert ARM Developer Suite 1.0.1 syntax assembly source to GNU as format
#
# Usage: cat inputfile | perl ads2gas.pl > outputfile
#

while (<STDIN>) {

    # Comment character
    s/;/@/g;

    # Hexadecimal constants prefaced by 0x
    s/#&/#0x/g;

    # Code directive (ARM vs Thumb)
    s/CODE([0-9][0-9])/.code $1/;

    # No AREA required
    s/^\s*AREA.*$/.text/;

    # Make function visible to linker, and make additional symbol with
    # prepended underscore
    s/(.*)EXPORT\s+\|(\w*)\|/.global _$1\n.global $1/;
    
    # No vertical bars required; make additional symbol with prepended
    # underscore  
    s/^\|(\w+)\|/_$1\n$1:/g;
    
    # Labels need trailing colon
    s/^(\w+)/$1:/ if !/EQU/;

#    s/^\w*writecommand^.*/writecommand:/g
    
    # EQU directive
    s/(.*)EQU(.*)/.equ $1, $2/;
    
    # Begin macro definition
    if (/MACRO/) {
       $_ = <STDIN>;
       s/^/.macro/;
       s/\$//g;      # remove formal param reference
       s/;/@/g;      # change comment characters
    }
    
    # For macros, use \ to reference formal params
    s/\$/\\/g;

    # End macro definition
    s/MEND/.endm/;

    # No need to tell it where to stop assembling
    next if /^\s*END\s*$/;

    print;

}
