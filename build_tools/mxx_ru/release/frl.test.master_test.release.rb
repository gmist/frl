require 'mxx_ru/cpp'
require 'ftools'
require '../template/frl.test.template'

MxxRu::Cpp::exe_target("frl.test.master_test.release.rb")\
{
	required_prj( "frl.lib.release.rb" )
	test_setup()

	target("test_master_test")
	include_path("../../../test/master_test")
	runtime_mode( MxxRu::Cpp::RUNTIME_RELEASE )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/test/master_test",\
	"../../../output/test/master_test/obj/#{mxx_runtime_mode}/1/2/3" ) )
	cpp_sources Dir.glob( "../../../test/master_test/**/*.cpp" )
}
File.copy '../../../test/poor_xml/xml_data/empty_file.xml','../../../output/test/master_test/empty_file.xml'
File.copy '../../../test/poor_xml/xml_data/largeFile.xml','../../../output/test/master_test/largeFile.xml'
File.copy '../../../test/poor_xml/xml_data/test_poor_xml.xml','../../../output/test/master_test/test_poor_xml.xml'
