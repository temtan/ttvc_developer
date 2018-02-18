
require './ttrl/releaser.rb'

base = File.dirname( File.expand_path( __FILE__ ) )
project_name = "ttvc_developer"


## -- exe -----
STDERR.puts( "== exe ====================" )

releaser = TTRL::Releaser.new( project_name, base )
releaser.strftime_template = '%y%m%d'

[
"Release/ttvc_developer.exe",
"readme.txt",
].each {|file| releaser.add_file( file ) }

[
].each {|dir| releaser.add_dir( dir ) }

releaser.add_pre_copy {|file_utils, to_base|
  [
  ].each {|one| file_utils.mkdir( "#{to_base}/#{one}" ) }

  file_utils.mkdir( "#{to_base}/manual" )
  Dir.glob( "./man/html/*" ) {|file|
    file_utils.cp_r( file, "#{to_base}/manual" )
  }
}



releaser.release_dir = "../backups/#{project_name}/web"
releaser.make_zip
