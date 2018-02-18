// ttvc_developer.tdp.nut

/*
{
  local tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\ttvc_developer.bmp" );
  tmp.included_in_build = true;
  tmp.depends.append( "ttvc_developer.bmp" );
  tmp.commands.append( "copy /Y $? $@" );
  Developer.current_structure.additional_make_commands.append( tmp );
}
{
  local tmp = MakeCommandEntry( "bmp_clean" );
  tmp.included_in_clean = true;
  tmp.commands.append( "del " + Developer.current_structure.output_directory_name + "\\ttvc_developer.bmp" );
  Developer.current_structure.additional_make_commands.append( tmp );
}
*/
