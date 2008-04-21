require 'mxx_ru/cpp'
require 'ftools' 

MxxRu::Cpp::exe_target("frl.project.psoi2.debug.rb")\
{
	required_prj( "frl.lib.debug.rb" )
	target("opc_psoi2_d")
	include_path("../../../projects/opc/psoi2/include")
	runtime_mode( MxxRu::Cpp::RUNTIME_DEBUG )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/projects/opc/psoi2",\
	"../../../output/projects/opc/psoi2/obj/#{mxx_runtime_mode}/1/2/3" ) )
	cpp_sources Dir.glob( "../../../projects/opc/psoi2/src/**/*.cpp" )
}
File.copy '../../../projects/opc/psoi2/config.xml', '../../../output/projects/opc/psoi2/config.xml'