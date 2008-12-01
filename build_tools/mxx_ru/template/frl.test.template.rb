require 'mxx_ru/cpp'

def test_setup()
	# set path to boost library (please modify to you local copy ot boost)
	global_include_path( "#{ENV['BOOST_HOME']}" )
	lib_path( "#{ENV['BOOST_HOME']}" + "/stage/lib" )

	# if you using version MinGW != 3.4.5, you maybe need correct libs name
	if "vc" != toolset.name
		if Mxx_ru::Cpp::RUNTIME_DEBUG != mxx_runtime_mode
			lib("libboost_unit_test_framework-mgw34-mt-s")
		else
			lib("libboost_unit_test_framework-mgw34-mt-sd")
		end
	else
		# Visual C++ compiler supports auto-linking
	end
end