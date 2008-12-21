!macro CREATE_OUT_DIR OUT_DIR_PATH
	!system "ruby ../template/create_out_dir.rb ${OUT_DIR_PATH}"
!macroend

!macro BUILDING_SVN_OUT_NAME OUT_DIR PREFIX PATH
!system "ruby ../template/get_revision.rb -g ${OUT_DIR} ${PREFIX} ${PATH}"
!include "out_file.name"
!system "ruby ../template/get_revision.rb -d"
!macroend
