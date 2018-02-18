// project.tdp.nut

function puts( str )
{
  print( str );
  print( "\n" );
}


Developer.current_structure.build_functions.first = function ()
{
  print( "first\n" );
}

Developer.current_structure.build_functions.last = function ( exist_error )
{
  print( "last\n" );
}

Developer.current_structure.build_functions.before_compile = function ( target )
{
  print( target + " before_compile\n" );
}

Developer.current_structure.build_functions.after_compile = function ( target, exit_code )
{
  print( target + " after_compile\n" );
}

Developer.current_structure.build_functions.before_build = function ()
{
  print( "before_build\n" );
}

Developer.current_structure.build_functions.after_build = function ( exit_code )
{
  print( "after_build\n" );
}

Developer.current_structure.build_functions.before_clean = function ()
{
  print( "before_clean\n" );
}

Developer.current_structure.build_functions.after_clean = function ( exit_code )
{
  print( "after_clean\n" );
}
