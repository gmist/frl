class SvnRevision
	 @@rev = 0
	 String @@format = ""

	def get_rev_from_entries( path )
		i = 0
		aFile = File.open( path + "/" + "entries", "r")
		aFile.each_line {
			|line| 
			i += 1
			if( i == 4 )
				if( @@rev < line.to_i )
					@@rev = line.to_i
				end
			end
		}
	end
	
	def get_svn_format( path )
		aFile = File.open( path + "/" + "format", "r")
		lines = aFile.readlines
		@@format = lines[0].chomp
	end
	
	def find_svn_format( path )
		tmp = path
		tmp += "/"
		Dir.foreach( path ) { 
			|x|
			
			tmp1 = tmp + x
			if x == ".svn"
				get_svn_format( tmp1 )
				return @@format
			end
		}
		return @@format
	end

	def  find_revision9( path )
		tmp = path
		tmp += "/"
		Dir.foreach( path ) { 
			|x|
			
			tmp1 = tmp + x
			if x == ".svn"
				get_rev_from_entries( tmp1 )
			end

			if x != ".svn" && x != "." && x != ".." && ! FileTest.file?( tmp1 )
				find_revision9( tmp1 )
			end
		}
	end

	def get_revision( path )
		if( find_svn_format( path ) == "9" )
			find_revision9( path )
			return @@rev
		else
			return "0"
		end
	end
end

def get_file_name( out_dir, prefix, path )
	tmp = SvnRevision.new
	String revision = tmp.get_revision( File.expand_path( "../../../" ) )
	time = Time.now
	String date = time.strftime( "%y.%m_" )
	date = date[ 1, date.length-1 ]
	String name = "OutFile " + out_dir + "\\" + prefix + "_" + date + "r"+ String( revision ) + ".exe"
end

if( ARGV[0] == "-g" )
	file_name = File.new( "out_file.name", "w+" )
	file_name.puts get_file_name( ARGV[1], ARGV[2], ARGV[3] )
else
	File.delete("out_file.name")
end
