require 'mxx_ru/cpp'

MxxRu::Cpp::exe_target("frl.test.poor_xml.release.rb")\
{
	required_prj( "frl.lib.release.rb" )
	target("test_poor_xml")
	include_path("../../../test/poor_xml")
	runtime_mode( MxxRu::Cpp::RUNTIME_RELEASE )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/test/poor_xml",\
	"../../../output/test/poor_xml/obj/#{mxx_runtime_mode}/1/2/3" ) )
	cpp_sources Dir.glob( "../../../test/poor_xml/**/*.cpp" )
}
File.copy '../../../test/poor_xml/xml_data/empty_file.xml','../../../output/test/poor_xml/empty_file.xml'
File.copy '../../../test/poor_xml/xml_data/largeFile.xml','../../../output/test/poor_xml/largeFile.xml'
File.copy '../../../test/poor_xml/xml_data/test_poor_xml.xml','../../../output/test/poor_xml/test_poor_xml.xml'