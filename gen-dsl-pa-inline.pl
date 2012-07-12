#!/usr/bin/perl

open( FIN, "dsl-pa-dsl-pa.txt" );
open( FOUT, ">dsl-pa-inline.txt" );

while( <FIN> ) {
    # size_t /*num chars read*/ get( std::string * p_output, const alphabet & r_alphabet );
    s|/\*.*\*/| |;
	/\s*(\w+)\s+(\w+)\(([^\)]*)\)/;
	my $return = $1;
	my $method = $2;
	my $param_spec = $3;
	my $param_struct = mk_params_struct_name( $param_spec );
	print "DEBUG: $param_spec: $param_struct\n";
	my $param_calling = mk_params_calling( $param_spec );
	print "DEBUG2: $param_spec: $param_calling\n";
	my $param_invoke = mk_params_invoke( $param_calling );
	print "DEBUG3: $param_spec: $param_invoke\n";
	
	if( $return eq 'bool' ) {
		gen_bool_return( $return, $method, $param_spec, $param_struct, $param_calling, $param_invoke );
	}
	else {
		gen_sized_return( $return, $method, $param_spec, $param_struct, $param_calling, $param_invoke );
	}
	
	if( ! defined( $param_structs{$param_struct} ) ) {
		gen_param_struct( $param_spec, $param_struct, $param_calling );
		$param_structs{$param_struct} = 1;
	}
	
	if( ! defined( $action_structs{$method} ) ) {
		gen_action_struct( $method );
		$action_structs{$method} = 1;
	}
}

print FOUT $actions;
print FOUT $structs;
print FOUT $functions;
close FIN;
close FOUT;

sub gen_bool_return()
{
	my ($return, $method, $param_spec, $param_struct, $param_calling, $param_invoke) = @_;

	$functions .= << "(END)";

private:
    bool action( const action_$method &, const $param_struct & params )
    {
        return pa.$method($param_invoke);
    }
public:
    dsl_pa_inline & $method($param_spec)
    {
        return invoke( action_$method(), $param_struct($param_calling) );
    }
(END)
}

sub gen_sized_return()
{
	my ($return, $method, $param_spec, $param_struct, $param_calling, $param_invoke) = @_;

	$functions .= << "(END)";

private:
    size_t action( const action_$method &, const $param_struct & params )
    {
        return pa.$method($param_invoke);
    }
public:
    dsl_pa_inline & $method($param_spec)
    {
        return invoke_sized( action_$method(), $param_struct($param_calling) );
    }
(END)
}

sub gen_action_struct()
{
	my ($method) = @_;

	$actions .= << "(END)";
    struct action_$method {};
(END)
}

sub gen_param_struct()
{
	my( $param_spec, $param_struct, $param_calling ) = @_;
	my $members = $param_spec;
	$members =~ s/,/;/g;
	$members =~ s/;\s*/;\n\t\t/g;
	$members =~ s/^\s*//;
	$members =~ s/\s*$//;
	my $con_args = $param_spec;
	my $con_init = '';
	foreach $arg (split /\s*\,\s*/, $param_calling) {
		$con_init .= ", " if $con_init ne '';
		$con_init .= "$arg( ${arg}_in )";
		$con_args =~ s/$arg/${arg}_in/;
	}

	$structs .= << "(END)";

	struct $param_struct
	{
		$members;
		$param_struct($con_args) : $con_init {}
	};
(END)
}


sub mk_params_struct_name()
{
	my $params = shift;
	my $param_struct = $params;
	$param_struct =~ s/const\s*//g;
	$param_struct =~ s/std:://g;
	$param_struct =~ s/char\s*\*/char_p/g;
	$param_struct =~ s/[&*]//g;
	$param_struct =~ s/\w+,//g;
	$param_struct =~ s/\w+\s*$//;

	$param_struct =~ s/^\s*//;
	$param_struct =~ s/\s*$//;
	$param_struct =~ s/\s+/_/g;
	
	return 'params_none' if $param_struct =~ /^\s*$/;
	
	return "params_$param_struct";
}

sub mk_params_calling()
{
	my $params = shift;
	my $param_calling;
	foreach my $param (split /,/, $params ) {
		$param =~ /(\w+)\s*$/;
		$param_calling .= ", " if $param_calling ne '';
		$param_calling .= $1;
	}

	return $param_calling;
}

sub mk_params_invoke()
{
	my $param_calling = shift;
	$param_calling =~ s/(\w+)/params.$1/g;
	return $param_calling;
}
