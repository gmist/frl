require 'mxx_ru/cpp'

def psoi2_init()

	screen_mode( MxxRu::Cpp::SCREEN_WINDOW )
	include_path("../../../projects/opc/psoi2/include")
	include_path("../../../projects/opc/psoi2/resource") 
	sources_root("../../../projects/opc/psoi2")
	cpp_sources Dir.glob( "../../../projects/opc/psoi2/src/**/*.cpp" )

	rtl_mode( MxxRu::Cpp::RTL_STATIC )
	threading_mode( MxxRu::Cpp::THREADING_MULTI )

	obj_placement( MxxRu::Cpp::CustomSubdirObjPlacement.new( "../../../output/projects/opc/psoi2",\
	"../../../output/projects/opc/psoi2/obj/#{mxx_runtime_mode}/1/2/3" ) )

	mswin_rc_file("resource/resource.rc",
						[ "../../../projects/opc/psoi2/resource/bitmaps/tree_img_list.bmp",
						"../../../projects/opc/psoi2/resource/bitmaps/logo_text.bmp",
						"../../../projects/opc/psoi2/resource/bitmaps/splash.bmp",
						"../../../projects/opc/psoi2/resource/icons/server_icon.ico" ] )
	lib( "comctl32" )
	lib( "Gdi32" ) 
end