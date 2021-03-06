require 'mxx_ru/cpp'
require '../template/frl.test.template'

MxxRu::Cpp::exe_target("frl.test.smart_ptr.debug.rb")\
{
	required_prj( "frl.lib.debug.rb" )
	test_setup()

	target("test_smart_ptr_d")
	include_path("../../../test/smart_ptr")
	runtime_mode( MxxRu::Cpp::RUNTIME_DEBUG )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/test/smart_ptr",\
	"../../../output/test/smart_ptr/obj/#{mxx_runtime_mode}/1/2/3" ) )
	cpp_sources Dir.glob( "../../../test/smart_ptr/**/*.cpp" )
}
