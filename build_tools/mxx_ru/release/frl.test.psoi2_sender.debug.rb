require 'mxx_ru/cpp'
require 'ftools'
require '../template/frl.test.template'

MxxRu::Cpp::exe_target("frl.test.psoi2_sender.release.rb")\
{
	required_prj( "frl.lib.release.rb" )
	test_setup()

	target("test_psoi2_sender")
	include_path("../../../test/psoi2_sender")
	runtime_mode( MxxRu::Cpp::RUNTIME_RELEASE )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/test/psoi2_sender",\
	"../../../output/test/psoi2_sender/obj/#{mxx_runtime_mode}/1/2/3" ) )
	cpp_sources Dir.glob( "../../../test/psoi2_sender/**/*.cpp" )
}
File.copy '../../../test/psoi2_sender/config.xml','../../../output/test/psoi2_sender/config.xml'
