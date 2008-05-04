require 'mxx_ru/cpp'

MxxRu::Cpp::lib_target("frl.lib.release.rb")\
{
	if "vc" == toolset.name
	global_cpp_compiler_option( "-W3" )
	global_cpp_compiler_option( "/MP" )
	global_cpp_compiler_option( "/D UNICODE" )
	else
	global_cpp_compiler_option( "-Wall" )
	end

	required_prj( "frl.dependency.vendors.opc_foundation.release.rb" )
	target("frl")

	global_include_path("../../../include")

	runtime_mode( Mxx_ru::Cpp::RUNTIME_RELEASE )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )

	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/frl_lib",\
	"../../../output/frl_lib/obj/#{mxx_runtime_mode}/1/2/3" ) )
	cpp_sources Dir.glob( "../../../src/**/*.cpp" )

	lib( "shell32" )
	lib( "oleaut32" )
	lib( "kernel32" )
	lib( "advapi32" )
	lib( "ole32" )
	lib( "user32" )
	lib( "uuid" )
}