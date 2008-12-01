require 'mxx_ru/cpp'
require '../template/frl.test.template'

MxxRu::Cpp::exe_target("frl.test.logging.release.rb")\
{
	required_prj( "frl.lib.release.rb" )
	test_setup()

	target("test_logging")
	include_path("../../../test/logging")
	runtime_mode( MxxRu::Cpp::RUNTIME_RELEASE )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/test/logging",\
	"../../../output/test/logging/obj/#{mxx_runtime_mode}/1/2/3" ) )
	cpp_sources Dir.glob( "../../../test/logging/**/*.cpp" )
}
