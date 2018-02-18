
require './ttrl/releaser.rb'

base = File.dirname( File.expand_path( __FILE__ ) )
project_name = "ttvc_developer"

## -- source -----
STDERR.puts( "== source ====================" )

releaser = TTRL::Releaser.new( project_name, base )
releaser.strftime_template = '%y%m%d%H%M'

## 個別ファイル
[
"backup.rb",
"zip_release.rb",
"history.txt",
"readme.txt",
].each {|file| releaser.add_file( file ) }

## 拡張子種別
[
"./*.cpp",
"./*.h",
"./*.rc",
"./*.bmp",
"./*.ico",
"./*.tdp",
"./*.nut",
"./*.dat",
].each {|pattern|
  Dir.glob( pattern ) {|file|
    releaser.add_file( file )
  }
}


## ディレクトリごと
[
"ttrl",
"ttl",
"icons",
"man",
].each {|dir| releaser.add_dir( dir ) }

releaser.add_pre_copy {|file_utils, to_base|
  ## 空ディレクトリの作成
  [
  ].each {|one| file_utils.mkdir( "#{to_base}/#{one}" ) }
}

releaser.release_dir = "../backups/#{project_name}/backup"
releaser.additional_string = "_src"
releaser.make_zip
