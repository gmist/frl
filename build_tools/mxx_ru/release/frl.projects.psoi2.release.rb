require 'mxx_ru/cpp'
require 'ftools'

MxxRu::Cpp::exe_target("frl.project.psoi2.release.rb")\
{
	required_prj( "frl.lib.release.rb" )
	target("opc_psoi2")
	screen_mode( MxxRu::Cpp::SCREEN_WINDOW )
	include_path("../../../projects/opc/psoi2/include")
	include_path("../../../projects/opc/psoi2/resource")
	runtime_mode( MxxRu::Cpp::RUNTIME_RELEASE )
	rtl_mode( MxxRu::Cpp::RTL_STATIC )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )
	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/projects/opc/psoi2",\
	"../../../output/projects/opc/psoi2/obj/#{mxx_runtime_mode}/1/2/3" ) )
	sources_root("../../../projects/opc/psoi2")
	cpp_sources Dir.glob( "../../../projects/opc/psoi2/src/**/*.cpp" )
	mswin_rc_file("resource/resource.rc",
						[ "../../../projects/opc/psoi2/resource/bitmaps/tree_img_list.bmp",
						"../../../projects/opc/psoi2/resource/icons/102.ico" ] )
	lib( "comctl32" )
	lib( "Gdi32" )
}
File.copy '../../../projects/opc/psoi2/config.xml', '../../../output/projects/opc/psoi2/config.xml'
