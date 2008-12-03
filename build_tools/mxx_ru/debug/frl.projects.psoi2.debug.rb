require 'mxx_ru/cpp'
require 'ftools'
require '../template/frl.projects.psoi2.template'

MxxRu::Cpp::exe_target("frl.project.psoi2.debug.rb")\
{
	required_prj( "frl.lib.debug.rb" )
	target("opc_psoi2_d")
	runtime_mode( MxxRu::Cpp::RUNTIME_DEBUG )
	psoi2_init()
}
File.copy '../../../projects/opc/psoi2/config.xml', '../../../output/projects/opc/psoi2/config.xml'
