require 'mxx_ru/cpp'

MxxRu::Cpp::exe_target("frl.test.function.debug.rb")\
{
	required_prj( "frl.lib.debug.rb" )
	target("test_function_d")
	include_path("../../../test/function")
	runtime_mode( MxxRu::Cpp::RUNTIME_DEBUG )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/test/function", "../../../output/test/function/obj/#{mxx_runtime_mode}/1/2/3" ) )
	cpp_sources Dir.glob( "../../../test/function/**/*.cpp" )
}
