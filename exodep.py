#!/usr/bin/env python3

# The MIT License (MIT)
#
# Copyright (c) 2016 Codalogic Ltd
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Exodep is a simple dependency downloader. See the following for more details:
#
#     https://github.com/codalogic/exodep

import sys
import re
import io
import os
import urllib.request
import tempfile
import shutil
import filecmp
import glob

host_templates = {
        'github': 'https://raw.githubusercontent.com/${owner}/${project}/${strand}/${path}${file}',
        'gitlab': 'https://gitlab.com/${owner}/${project}/raw/${strand}/${path}${file}',
        'bitbucket': 'https://bitbucket.org/${owner}/${project}/raw/${strand}/${path}${file}' }

init_exodep = 'exodep-imports/__init.exodep'
end_exodep = 'exodep-imports/__end.exodep'
onstop_exodep = 'exodep-imports/__onstop.exodep'

default_vars = { 'strand': 'master', 'path': '' }

class StopException( Exception ):
    pass

def main():
    try:
        if len( sys.argv ) >= 2:
            ProcessDeps( sys.argv[1] )
        elif os.path.isfile( 'mydeps.exodep' ):
            ProcessDeps( 'mydeps.exodep' )
        elif os.path.isfile( 'exodep-imports/mydeps.exodep' ):
            ProcessDeps( 'exodep-imports/mydeps.exodep' )
        else:
            process_globbed_exodep_imports()

    except StopException:
        pass

def process_globbed_exodep_imports():
    vars = default_vars
    if os.path.isfile( init_exodep ):
        pd = ProcessDeps( init_exodep, vars )
        vars = pd.get_vars()
    for file in glob.glob( 'exodep-imports/*.exodep' ):
        file = file.replace( '\\', '/' )
        if not is_ignored_glob( file ):
            ProcessDeps( file, vars )
    if os.path.isfile( end_exodep ):
        ProcessDeps( end_exodep, vars )

def is_ignored_glob( file ):
    return file.find( '/__' ) >= 0 or file.find( '/^' ) >= 0;

class ProcessDeps:
    are_any_files_changed = False
    alert_messages = ""
    shown_alert_messages = ""

    processed_configs = {}

    def __init__( self, dependencies_src, vars = default_vars ):
        self.is_last_file_changed = self.are_files_changed = False
        self.line_num = 0
        self.uritemplate = host_templates['github']
        self.set_vars( vars )
        self.versions = {}  # Each entry is <string of space separated strand names> : <string to use as strand in uri template>
        self.sought_condition = True
        if isinstance( dependencies_src, str ):
            if self.is_config_already_processed( dependencies_src ):
                return
            self.file = dependencies_src
            self.process_dependency_file()
        elif isinstance( dependencies_src, io.StringIO ):    # Primarily for testing
            self.file = "<StringIO>"
            self.process_dependency_stream( dependencies_src )
        else:
            self.error( "Unrecognised dependencies_src type format" )

    def get_vars( self ):
        return self.vars

    def set_vars( self, vars ):
        self.vars = vars.copy()
        # Remove non-exportable vars
        self.vars.pop( '__authority', None )

    def is_config_already_processed( self, dependencies_src ):
        abs_dependencies_src = os.path.abspath( dependencies_src )
        if abs_dependencies_src in ProcessDeps.processed_configs:
            return True
        ProcessDeps.processed_configs[abs_dependencies_src] = True
        return False

    def process_dependency_file( self ):
        try:
            with open(self.file) as f:
                self.process_dependency_stream( f )
        except FileNotFoundError:
            self.error( "Unable to open exodep file: " + self.file )

    def process_dependency_stream( self, f ):
        for line in f:
            self.line_num += 1
            self.sought_condition = True
            self.process_line( line )

    def process_line( self, line ):
        line = line.strip()
        line = remove_comments( line )
        if is_blank_line( line ):
            return
        if not (self.consider_include( line ) or
                self.consider_sinclude( line ) or
                self.consider_hosting( line ) or
                self.consider_uri_template( line ) or
                self.consider_versions( line ) or
                self.consider_variable( line ) or
                self.consider_default_variable( line ) or
                self.consider_showvars( line ) or
                self.consider_autovars( line ) or
                self.consider_get( line ) or
                self.consider_bget( line ) or
                self.consider_file_ops( line ) or
                self.consider_exec( line ) or
                self.consider_subst( line ) or
                self.consider_authority( line ) or
                self.consider_on_conditional( line ) or
                self.consider_ondir( line ) or
                self.consider_onfile( line ) or
                self.consider_onlastchanged( line ) or
                self.consider_onchanged( line ) or
                self.consider_onanychanged( line ) or
                self.consider_onalerts( line ) or
                self.consider_os_conditional( line ) or
                self.consider_not( line ) or
                self.consider_echo( line ) or
                self.consider_pause( line ) or
                self.consider_alert( line ) or
                self.consider_showalerts( line ) or
                self.consider_alertstofile( line ) or
                self.consider_stop( line ) ):
            self.report_unrecognised_command( line )

    def consider_include( self, line ):
        m = re.match( '^include\s+(.+)', line )
        if m != None:
            file_name = self.script_relative_path( m.group(1) )
            if not os.path.isfile( file_name ):
                self.error( "'include' file not found: " + file_name )
                return True     # It was an 'include' command, even though it was a bad one
            ProcessDeps( file_name, self.vars )
            return True
        return False

    def consider_sinclude( self, line ):        # This method is used to support testing
        m = re.match( '^sinclude\s+(.+)', line )
        if m != None:
            args = m.group(1)
            ProcessDeps( io.StringIO( args.replace( '\t', '\n' ) ), self.vars )
            return True
        return False

    def script_relative_path( self, src ):
        return os.path.normpath( os.path.join( os.path.dirname( self.file ), src ) ).replace( '\\', '/' )

    def consider_hosting( self, line ):
        m = re.match( '^hosting\s+(.+)', line )
        if m != None:
            host = m.group(1)
            if host in host_templates:
                self.uritemplate = host_templates[host]
            else:
                self.error( "Unrecognised hosting server provider: " + host )
            return True
        return False

    def consider_uri_template( self, line ):
        m = re.match( '^uritemplate\s+(.+)', line )
        if m != None:
            self.uritemplate = m.group(1)
            return True
        return False

    def consider_versions( self, line ):
        m = re.match( '^versions(?:\s+(.*))?', line )    # If no arguments to 'versions' command, it should use 'versions.exodep'
        if m != None:
            file_name = m.group(1) if m.group(1) else 'versions.exodep'
            uri = self.make_master_strand_uri( file_name )
            try:
                if re.match( 'https?://', uri ):
                    with urllib.request.urlopen( uri ) as fin:
                        self.parse_versions_info( fin )
                else:
                    with open( uri, "rt" ) as fin:
                        self.parse_versions_info( fin )
            except:
                self.error( "Unable to retrieve: " + uri )
            return True
        return False

    def parse_versions_info( self, fin ):
        for line in fin:
            if isinstance( line, bytes ):
                line = line.decode( 'utf-8' )
            line = line.rstrip()
            line = remove_comments( line )
            if not is_blank_line( line ):
                m = re.match( '^(\S+)\s+(.*)', line )
                if m != None:
                    self.versions[m.group(2)] = m.group(1)

    def consider_variable( self, line ):
        m = re.match( '^\$(\w+)(?:\s+(.*))?', line )
        if m != None:
            self.vars[m.group(1)] = m.group(2) if m.group(2) else ''
            return True
        return False

    def consider_default_variable( self, line ):
        m = re.match( '^default\s+\$(\w+)(?:\s+(.*))?', line )
        if m != None:
            if m.group(1) not in self.vars:
                self.vars[m.group(1)] = m.group(2) if m.group(2) else ''
            return True
        return False

    def consider_showvars( self, line ):
        m = re.match( '^showvars', line )
        if m != None:
            for var in sorted( self.vars.keys() ):
                raw = self.vars[var]
                expanded = self.expand_variables( raw )
                expansion = '' if expanded == raw else (' -> ' + expanded)
                print( var + ": " + raw + expansion )
            return True
        return False

    def consider_autovars( self, line ):
        m = re.match( '^autovars', line )
        if m != None:
            uri = re.compile( '-' ).sub( 'master', self.uritemplate )
            project = self.vars['project']
            safe_project = project.replace( '-', '_' )
            if project != '':
                self.process_line( 'versions' )
            self.process_line( 'default $ext_home' )

            self.process_line( 'default $ext_inc_home   ${ext_home}include/' )
            self.process_line( 'default $ext_src_home   ${ext_home}src/' )
            self.process_line( 'default $ext_build_home ${ext_home}build/' )
            self.process_line( 'default $ext_lib_home   ${ext_home}lib/' )
            self.process_line( 'default $ext_bin_home   ${ext_home}bin/' )

            self.process_line( 'default $inc_dst   ${ext_inc_home}' +   project + '/' )
            self.process_line( 'default $src_dst   ${ext_src_home}' +   project + '/' )
            self.process_line( 'default $build_dst ${ext_build_home}' + project + '/' )
            self.process_line( 'default $lib_dst   ${ext_lib_home}' +   project + '/' )
            self.process_line( 'default $bin_dst   ${ext_bin_home}' +   project + '/' )

            self.process_line( 'default $' + safe_project + '_inc_dst   ${inc_dst}' )
            self.process_line( 'default $' + safe_project + '_src_dst   ${src_dst}' )
            self.process_line( 'default $' + safe_project + '_build_dst ${build_dst}' )
            self.process_line( 'default $' + safe_project + '_lib_dst   ${lib_dst}' )
            self.process_line( 'default $' + safe_project + '_bin_dst   ${bin_dst}' )
            return True
        return False

    def consider_get( self, line ):
        m = re.match( '^(?:get|copy)\s+(\S+)(?:\s+(\S+))?', line )
        if m != None:
            self.retrieve_text_file( m.group(1), m.group(2) )
            return True
        return False

    def consider_bget( self, line ):
        m = re.match( '^(?:bget|bcopy)\s+(\S+)(?:\s+(\S+))?', line )
        if m != None:
            self.retrieve_binary_file( m.group(1), m.group(2) )
            return True
        return False

    def report_unrecognised_command( self, line ):
        self.error( "Unrecognised command: " + line )

    def retrieve_text_file( self, src, dst ):
        self.retrieve_file( src, dst, TextDownloadHandler() )

    def retrieve_binary_file( self, src, dst ):
        self.retrieve_file( src, dst, BinaryDownloadHandler() )

    def retrieve_file( self, src, dst, handler ):
        self.is_last_file_changed = False
        if dst == None:
            if re.match( 'https?://', src ):
                self.error( "Explicit uri not supported with commands of the form 'get src_and_dst'" )
                return
            dst = src
            if re.search( '\$\{path\}', self.uritemplate ):
                dst = self.vars['path'] + dst
        from_uri = self.make_uri( src )
        to_file = self.make_destination_file_name( src, dst )
        if from_uri == '':
            self.error( "Unable to evaluate source of: " + src )
            return
        if to_file == '':
            self.error( "Unable to evaluate destination of: " + dst )
            return
        if self.is_file_already_downloaded( from_uri, to_file ):
            print( 'Repeat....', to_file )
            return
        if re.match( 'https?://', from_uri ):
            tmp_name = handler.download_to_temp_file( from_uri )
        else:
            tmp_name = self.local_copy_to_temp_file( from_uri )     # Taking a local copy is not optimal, but keeps the subsequent update logic the same
        if not tmp_name:
            self.error( "Unable to retrieve: " + from_uri )
            return
        self.conditionally_update_dst_file( tmp_name, to_file )

    processed_downloads = {}

    def is_file_already_downloaded( self, src, dst ):
        key = src + "\n" + dst
        if os.path.isfile( dst ) and key in ProcessDeps.processed_downloads:    # Allow for file being deleted between downloads for some reason
            return True
        ProcessDeps.processed_downloads[key] = True
        return False

    def conditionally_update_dst_file( self, tmp_name, to_file ):
        if not os.path.isfile( to_file ):
            if os.path.dirname( to_file ):
                os.makedirs( os.path.dirname( to_file ), exist_ok=True )
            shutil.move( tmp_name, to_file )
            self.is_last_file_changed = self.are_files_changed = ProcessDeps.are_any_files_changed = True
            print( 'Created...', to_file )
        elif not filecmp.cmp( tmp_name, to_file ):
            shutil.move( tmp_name, to_file )
            self.is_last_file_changed = self.are_files_changed = ProcessDeps.are_any_files_changed = True
            print( 'Updated...', to_file )
        else:
            os.unlink( tmp_name )
            print( 'Same......', to_file )

    def make_master_strand_uri( self, file_name ):
        # Override ${master} and ${path} variable
        uri = re.compile( '\$\{strand\}' ).sub( 'master', self.uritemplate )
        uri = re.compile( '\$\{path\}' ).sub( '', uri )
        return self.make_uri( file_name, uri )

    def make_uri( self, file_name, uri = None ):
        if re.match( 'https?://', file_name ):
            return self.expand_variables( file_name )
        if uri == None:
            uri = self.uritemplate
        uri = re.compile( '\$\{file\}' ).sub( file_name, uri )
        return self.expand_variables( uri )

    def make_destination_file_name( self, src, dst ):
        # dst in a get command may refer to a folder, in which case the base file name from the src needs to be incorporated
        dst = self.expand_variables( dst )
        if dst.endswith( '/' ) or os.path.isdir( dst ):
            if not dst.endswith( '/' ):
                dst = dst + '/'
            return dst + os.path.basename( src )
        return dst

    def expand_variables( self, uri ):
        while( True ):
            m = re.search( '\$\{(\w+)\}', uri )
            if m == None:
                return uri
            var_name = m.group(1)
            if var_name == 'strand':        # The strand variable has 'magic' properties (it can be mutated by a versions file) so we need to do something special with it
                uri = re.compile( '\$\{strand\}' ).sub( self.select_strand(), uri )
            elif var_name in self.vars:
                uri = re.compile( '\$\{' + var_name + '\}' ).sub( self.vars[var_name], uri )
            else:
                self.error( "Unrecognised substitution variable: " + var_name )
                return ''

    def select_strand( self ):
        if 'strand' not in self.vars:
            self.error( "No suitable 'strand' variable available for substitution" )
            return ''
        strand = self.vars['strand']
        for supported_strands in self.versions:
            if strand in supported_strands.split():
                return self.versions[supported_strands]
        return self.vars['strand']

    def local_copy_to_temp_file( self, file ):
        try:
            fout = None
            with open( file, 'rb' ) as fin:
                fout = tempfile.NamedTemporaryFile( mode='wb', delete=False )
                while True:
                    data = fin.read( 1000 )
                    if not data:
                        break
                    fout.write( data )
            fout.close()
            return fout.name
        except FileNotFoundError:
            return ''

    def consider_subst( self, line ):
        m = re.match( '^subst\s+(\S+)(?:\s+(\S+))?', line )
        if m != None:
            src = m.group(1)
            dst = m.group(2)
            if dst == None:
                dst = src
            try:
                fout = None
                with open( src, 'rt' ) as fin:
                    fout = tempfile.NamedTemporaryFile( mode='wt', delete=False )
                    for line in fin:
                        fout.write( self.subst_expand_variables( line ) )
                fout.close()
                self.conditionally_update_dst_file( fout.name, dst )
            except FileNotFoundError:
                self.error( "Unable to open file for 'subst' command: " + src )
            return True
        return False

    def subst_expand_variables( self, line ):
        while( True ):
            m = re.search( '\$\{exodep:(\w+)\}', line )
            if m == None:
                return line
            var_name = m.group(1)
            if var_name == 'strand':        # The strand variable has 'magic' properties (it can be mutated by a versions file) so we need to do something special with it
                line = re.compile( '\$\{strand\}' ).sub( self.select_strand(), line )
            elif var_name in self.vars:
                line = re.compile( '\$\{exodep:' + var_name + '\}' ).sub( self.vars[var_name], line )
            else:
                self.error( "Unrecognised variable in 'subst' command: " + var_name )
                return line

    def consider_file_ops( self, line ):
        m = re.match( '^(cp|mv)\s+(\S+)\s+(\S+)', line )
        if m != None:
            op = m.group(1)
            src = self.expand_variables( m.group(2) )
            dst = self.make_destination_file_name( src, m.group(3) )
            if op == 'cp':
                try:
                    if self.is_copy_needed( src, dst ):
                        shutil.copy( src, dst )
                        print( 'cp........', dst )
                except:
                    self.error( "Unable to 'cp' file '" + src + "' to '" + dst + "'" )
            elif op == 'mv':
                try:
                    shutil.move( src, dst )
                    print( 'mv........', dst )
                except:
                    self.error( "Unable to 'mv' file '" + src + "' to '" + dst + "'" )
            return True
        m = re.match( '^(mkdir|rmdir|rm)\s+(\S+)', line )
        if m != None:
            op = m.group(1)
            path = self.expand_variables( m.group(2) )
            if op == 'mkdir':
                try:
                    os.makedirs( path, exist_ok=True )
                    print( 'mkdir.....', path )
                except:
                    self.error( "Unable to 'mkdir' for '" + path + "'" )
            elif op == 'rmdir':
                try:
                    shutil.rmtree( path )
                    print( 'rmdir.....', path )
                except:
                    self.error( "Unable to 'rmdir' on '" + path + "'" )
            elif op == 'rm':
                try:
                    os.unlink( path )
                    print( 'rm........', path )
                except:
                    self.error( "Unable to 'rm' file '" + path + "'" )
            return True
        m = re.match( '^touch\s+(\S+)', line )
        if m != None:
            file = self.expand_variables( m.group(1) )
            open( file, 'a' ).close()
            return True
        return False

    def is_copy_needed( self, src, dst ):
        return not os.path.isfile( dst ) or not filecmp.cmp( src, dst )

    def consider_exec( self, line ):
        m = re.match( '^exec\s+(.+)', line )
        if m != None:
            command = m.group(1)
            org_cwd = os.getcwd()
            file_dirname = os.path.dirname( self.file )
            if file_dirname:
                os.chdir( file_dirname )
            os.system( self.expand_variables( command ) )
            os.chdir( org_cwd )
            return True
        return False

    def consider_authority( self, line ):
        m = re.match( '^authority\s+(.+)', line )
        if m != None:
            src = m.group(1)
            from_uri = self.make_uri( src )
            self.vars['__authority'] = from_uri
            # Currently ignored.  Treated as a human readable documentation feature
            # TODO - See https://github.com/codalogic/exodep/issues/26
            return True
        return False

    def consider_on_conditional( self, line ):
        m = re.match( '^on\s+\$(\w+)\s+(.+)', line )
        if m != None:
            var_name = m.group(1)
            command = m.group(2)
            if self.is_sought_condition( \
                    var_name in self.vars and \
                    self.vars[var_name] != '' and \
                    self.vars[var_name] != '0' and \
                    self.vars[var_name].lower() != 'false' ):
                self.process_line( command )
            return True
        return False

    def consider_ondir( self, line ):
        m = re.match( '^ondir\s+(\S+)\s+(.+)', line )
        if m != None:
            dir = m.group(1)
            command = m.group(2)
            if self.is_sought_condition( os.path.isdir( dir ) ):
                self.process_line( command )
            return True
        return False

    def consider_onfile( self, line ):
        m = re.match( '^onfile\s+(\S+)\s+(.+)', line )
        if m != None:
            file = m.group(1)
            command = m.group(2)
            if self.is_sought_condition( os.path.isfile( file ) ):
                self.process_line( command )
            return True
        return False

    def consider_onlastchanged( self, line ):
        m = re.match( '^onlastchanged\s+(.+)', line )
        if m != None:
            command = m.group(1)
            if self.is_sought_condition( self.is_last_file_changed ):
                self.process_line( command )
            return True
        return False

    def consider_onchanged( self, line ):
        m = re.match( '^onchanged\s+(.+)', line )
        if m != None:
            command = m.group(1)
            if self.is_sought_condition( self.are_files_changed ):
                self.process_line( command )
            return True
        return False

    def consider_onanychanged( self, line ):
        m = re.match( '^onanychanged\s+(.+)', line )
        if m != None:
            command = m.group(1)
            if self.is_sought_condition( ProcessDeps.are_any_files_changed ):
                self.process_line( command )
            return True
        return False

    def consider_onalerts( self, line ):
        m = re.match( '^onalerts\s+(.+)', line )
        if m != None:
            command = m.group(1)
            if self.is_sought_condition( ProcessDeps.shown_alert_messages != "" or ProcessDeps.alert_messages != "" ):
                self.process_line( command )
            return True
        return False

    os_names = { 'windows': 'win32', 'linux': 'linux', 'osx': 'darwin' }

    def consider_os_conditional( self, line ):
        m = re.match( '^(windows|linux|osx)\s+(.+)', line )
        if m != None:
            os_key = m.group(1)
            command = m.group(2)
            if self.is_sought_condition( sys.platform.startswith( ProcessDeps.os_names[os_key] ) ):
                self.process_line( command )
            return True
        return False

    def consider_not( self, line ):
        m = re.match( '^not\s+(.+)', line )
        if m != None:
            command = m.group(1)
            self.sought_condition = not self.sought_condition
            self.process_line( command )
            return True
        return False

    def is_sought_condition( self, condition ):
        my_sought_condition = self.sought_condition
        self.sought_condition = True    # Reset sought_condition so it doesn't affect conditions later in the same command line
        if condition:
            return my_sought_condition
        return not my_sought_condition

    def consider_echo( self, line ):
        m = re.match( '^echo(?:\s+(.*))?', line )
        if m != None:
            message = m.group(1)
            if message:
                print( self.expand_variables( message ) )
            else:
                print( "" )
            return True
        return False

    def consider_pause( self, line ):
        m = re.match( '^pause(?:\s+(.*))?', line )
        if m != None:
            message = m.group(1)
            print( "" )
            if message:
                print( self.expand_variables( message ) )
            print( ">>> Press <Return> to continue <<<" )
            input()
            return True
        return False

    def consider_alert( self, line ):
        m = re.match( '^alert\s+(.*)', line )
        if m != None:
            message = self.expand_variables( m.group(1) )
            alert = "ALERT: " + self.file + " (" + str(self.line_num) + "):\n" + "       " + message
            print( alert )
            if ProcessDeps.alert_messages != "":
                ProcessDeps.alert_messages += "\n"
            ProcessDeps.alert_messages += alert
            return True
        return False

    def consider_showalerts( self, line ):
        if line == "showalerts":
            if ProcessDeps.alert_messages != "":
                print( "RECORDED ALERTS:" )
                print( ProcessDeps.alert_messages )
                if ProcessDeps.shown_alert_messages != "":
                    ProcessDeps.shown_alert_messages += "\n"
                ProcessDeps.shown_alert_messages = ProcessDeps.alert_messages
                ProcessDeps.alert_messages = ""
            return True
        return False

    def consider_alertstofile( self, line ):
        m = re.match( '^alertstofile\s+(.*)', line )
        if m != None:
            file = m.group(1)
            if os.path.isfile( file ):
                shutil.move( file, file + ".old" )
            if ProcessDeps.shown_alert_messages != "" or ProcessDeps.alert_messages != "":
                with open( file, 'w') as fout:
                    if ProcessDeps.shown_alert_messages != "":
                        fout.write( ProcessDeps.shown_alert_messages + "\n" )
                    if ProcessDeps.alert_messages != "":
                        fout.write( ProcessDeps.alert_messages + "\n" )
            return True
        return False

    def consider_stop( self, line ):
        m = re.match( '^stop(?:\s+(.*))?', line )
        if m != None:
            message = m.group(1)
            print( "STOPPED: " + self.file + " (" + str(self.line_num) + "):" )
            if message:
                print( "      " + self.expand_variables( message ) )
            if self.file != onstop_exodep and os.path.isfile( onstop_exodep ):
                ProcessDeps( onstop_exodep, self.vars )
            # return True
            raise StopException
        return False

    def error( self, what ):
        print( "Error:", self.file + ", line " + str(self.line_num) + ":" )
        print( "      ", what )

def remove_comments( line ):
    return re.compile( '\s*#.*' ).sub( '', line )

def is_blank_line( line ):
    return re.match( '^\s*$', line )

class TextDownloadHandler:
    def download_to_temp_file( self, uri ):
        try:
            fout = None
            with urllib.request.urlopen( uri ) as fin:
                fout = tempfile.NamedTemporaryFile( mode='wt', delete=False )
                for line in fin:
                    fout.write( self.normalise_line_ending( line.decode('utf-8') ) )
            fout.close()
            return fout.name
        except urllib.error.URLError:
            return ''

    def normalise_line_ending( self, line ):
        org_len = len( line )
        line = line.rstrip( '\r\n' )
        if org_len > len( line ):
            line = line + '\n'
        return line

class BinaryDownloadHandler:
    def download_to_temp_file( self, uri ):
        try:
            fout = None
            with urllib.request.urlopen( uri ) as fin:
                fout = tempfile.NamedTemporaryFile( mode='wb', delete=False )
                while True:
                    data = fin.read( 1000 )
                    if not data:
                        break
                    fout.write( data )
            fout.close()
            return fout.name
        except urllib.error.URLError:
            return ''

if __name__ == "__main__":
    main()
