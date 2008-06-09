require 'mxx_ru/cpp'

MxxRu::Cpp::lib_target("frl.lib.release.rb")\
{
	runtime_mode( Mxx_ru::Cpp::RUNTIME_RELEASE )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )

	if "vc" == toolset.name
	global_cpp_compiler_option( "-W3" )
	global_cpp_compiler_option( "/MP" )
	global_cpp_compiler_option( "/D UNICODE" )
	else
	global_cpp_compiler_option( "-W" )
	end

	required_prj( "frl.dependency.vendors.opc_foundation.release.rb" )
	target("frl")

	global_include_path("../../../include")

	# set path to boost library (please modify to you local copy ot boost)
	global_include_path( "../../../../../../../../src/lib/boost/trunk" )
	lib_path( "../../../../../../../../src/lib/boost/trunk/stage/lib" )

	# if you using version MinGW != 3.4.5, you maybe need correct libs name
	if "vc" != toolset.name
	lib("libboost_thread-mgw34-mt-s.lib")
	else
	# Visual C++ compiler supports auto-linking
	end

	lib( "shell32" )
	lib( "oleaut32" )
	lib( "kernel32" )
	lib( "advapi32" )
	lib( "ole32" )
	lib( "user32" )
	lib( "uuid" )
	
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/frl_lib",\
	"../../../output/frl_lib/obj/#{mxx_runtime_mode}/1/2/3" ) )
	cpp_sources Dir.glob( "../../../src/**/*.cpp" )
}
