# Basic script to convert an ABNF definition into outline C++ dsl-pa code.
# It won't do a perfect job, but will hopefully do some of the initial
# grunt work.
#
# It's strongly recommended that you rename the generated files before working
# on them to avoid accidentally overwriting them.
#
# Usage:
#     abnf2dsl-pa.py <src.abnf> <base-C++-file-name>

import sys
import re

def main() :
    if len( sys.argv ) < 3 :
        help()
        return

    if len( sys.argv ) > 2 :
        rules = read_abnf( sys.argv[1] )
        write_h( rules, sys.argv[2] + ".h" )
        write_cpp( rules, sys.argv[2] + ".cpp" )

def help() :
    print( "Usage:" )
    print( "    abnf2dsl-pa.py <src.abnf> <base-C++-file-name>" )

def read_abnf( file_name ) :
    "Read ABNF into an array of strings"
    rules = []
    with open( file_name, 'r' ) as fin:
        for line in fin :
            res_blank1 = re.search( '^\s*;', line )
            res_blank2 = re.search( '^\s*$', line )
            res_rule = re.search( '^[a-zA-Z]', line )
            res_extra = re.search( '^\s+[^;]', line )

            if res_blank1 or res_blank2 :
                pass
            elif res_rule :
                rules.append( line )
            elif res_extra :
                rules[-1] += line
    return rules

def write_h( rules, file_name ) :
    with open( file_name, 'w' ) as fout:
        print( "#include \"dsl-pa/dsl-pa.h\"", file=fout )
        print( "class my_class : public cl::dsl_pa", file=fout )
        print( "{", file=fout )
        for rule in rules :
            name = rule_name_in_cpp( rule )
            print( "    bool " + name + "();", file=fout )
        print( "};", file=fout )

def write_cpp( rules, file_name ) :
    rule_name_map = make_rule_name_map( rules )
    with open( file_name, 'w' ) as fout:
        for rule in rules :
            name = rule_name_in_cpp( rule )
            print( "bool my_class::" + name + "()", file=fout )
            print( "{", file=fout )
            cpp_rule = rule_to_cpp( rule, rule_name_map )
            print( "    // " + cpp_rule, file=fout )
            print( "    return false;", file=fout )
            print( "}", file=fout )

def rule_name_in_cpp( rule ) :
    name = rule_name( rule )
    name = name.replace( '-', '_' )
    return name

def make_rule_name_map( rules ) :
    rule_name_map = []
    for rule in rules :
        original = rule_name( rule )
        modified = original.replace( '-', '_' )
        # if original != modified :
        rule_name_map.append( (original, modified) )
    rule_name_map = sorted( rule_name_map, key=lambda field: len(field[0]), reverse=True );
    return rule_name_map

def rule_name( rule ) :
    res = re.search( '^([\w\-_]+)', rule )
    if res :
        name = res.group(1)
        return name
    return ""

def rule_to_cpp( rule, rule_name_map ) :
    cpp = rule
    for mapping in rule_name_map :
        cpp = re.sub( r'\b' + mapping[0] + r'\b', mapping[1] + "()", cpp )
    cpp = re.sub( r'([\)"\]])\s+/\s+([\w*("])', r'\1 || \2', cpp )    # Insert || operators
    cpp = re.sub( r'([\)"\]])\s+([\w*(])',      r'\1 && \2', cpp )    # Insert && operators
    cpp = re.sub( r'([\r\n]+)(\s+\S)', r'\1    // \2', cpp )          # Add comments to follow-on lines
    cpp = re.sub( r'\s*=\s*', r' = ', cpp )                           # Compact speces around = sign
    return cpp

main()
