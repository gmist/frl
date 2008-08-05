require 'mxx_ru/cpp'

MxxRu::Cpp::lib_target("frl.lib.debug.rb")\
{
	runtime_mode( Mxx_ru::Cpp::RUNTIME_DEBUG )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )

	if "vc" == toolset.name
	global_cpp_compiler_option( "-W4" )
	global_cpp_compiler_option( "/MP" )
	global_cpp_compiler_option( "/D UNICODE" )
	else
	global_cpp_compiler_option( "-Wall" )
	end

	required_prj( "frl.dependency.vendors.opc_foundation.debug.rb" )
	target("frl_d")

	global_include_path("../../../include")

	# set path to boost library (please modify to you local copy ot boost)
	global_include_path( "#{ENV['BOOST_HOME']}" )
	lib_path( "#{ENV['BOOST_HOME']}" + "/stage/lib" )

	# if you using version MinGW != 3.4.5, you maybe need correct libs name
	if "vc" != toolset.name
	lib("libboost_thread-mgw34-mt-sd")
	lib("libboost_filesystem-mgw34-mt-sd")
	lib("libboost_system-mgw34-mt-sd")
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
