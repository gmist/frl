require 'mxx_ru/cpp'

MxxRu::Cpp::exe_target("frl.test.opc_write_event_subscribe.debug.rb")\
{
	required_prj( "frl.lib.debug.rb" )
	target("test_opc_write_event_subscribe_d")
	include_path("../../../test/opc_write_event_subscribe")
	runtime_mode( MxxRu::Cpp::RUNTIME_DEBUG )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/test/opc_write_event_subscribe",\
	"../../../output/test/opc_write_event_subscribe/obj/#{mxx_runtime_mode}/1/2/3" ) )
	cpp_sources Dir.glob( "../../../test/opc_write_event_subscribe/**/*.cpp" )
}
