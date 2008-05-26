require 'mxx_ru/cpp'

MxxRu::Cpp::exe_target("frl.test.thread.debug.rb")\
{
	required_prj( "frl.lib.debug.rb" )
	target("test_thread_d")
	include_path("../../../test/thread")
	runtime_mode( MxxRu::Cpp::RUNTIME_DEBUG )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/test/thread",\
	"../../../output/test/thread/obj/#{mxx_runtime_mode}/1/2/3" ) )
	cpp_sources Dir.glob( "../../../test/thread/**/*.cpp" )
}
