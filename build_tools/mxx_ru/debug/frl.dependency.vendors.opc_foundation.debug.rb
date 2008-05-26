require 'mxx_ru/cpp'

MxxRu::Cpp::lib_target("frl.dependency.vendors.opc_foundation.debug.rb")\
{
	if "vc" == toolset.name
	cpp_compiler_option( "-W4" )
	else
	cpp_compiler_option( "-Wall" )
	end

	target("dependency_vendors_opc_foundation_d")
	include_path("../../../dependency/vendors/opc_foundation")
	runtime_mode( Mxx_ru::Cpp::RUNTIME_DEBUG )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/dependency/vendors/opc_foundation",\
	"../../../output/dependency/vendors/opc_foundation/obj/#{mxx_runtime_mode}/1/2/3" ) )
	c_sources Dir.glob( "../../../dependency/vendors/opc_foundation/**/*.c" )
}
