require 'mxx_ru/cpp'

MxxRu::Cpp::exe_target("frl.test.io_win32_registry.release.rb")\
{
	required_prj( "frl.lib.release.rb" )
	target("test_io_win32_registry")
	include_path("../../../test/io_win32_registry")
	runtime_mode( MxxRu::Cpp::RUNTIME_RELEASE )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/test/io_win32_registry",\
				"../../../output/test/io_win32_registry/obj/#{mxx_runtime_mode}/1/2/3" ) )
	cpp_sources Dir.glob( "../../../test/io_win32_registry/**/*.cpp" )
}