require 'mxx_ru/cpp'
require 'ftools'
require '../template/frl.projects.psoi2.template'

MxxRu::Cpp::exe_target("frl.project.psoi2.release.rb")\
{
	required_prj( "frl.lib.release.rb" )
	target("opc_psoi2")
	runtime_mode( MxxRu::Cpp::RUNTIME_RELEASE )
	psoi2_init()
}
File.copy '../../../projects/opc/psoi2/config.xml', '../../../output/projects/opc/psoi2/config.xml'
