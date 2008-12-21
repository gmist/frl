if ARGV.include?('-d') 
	File.delete("files.lst")
end

if ARGV.include?('-g')

	String out_path = "SetOutPath $INSTDIR\\src"
	String s = out_path
	s += "\n"

	Dir['../../../**/*'].each do |f|
		if f.include?("../../../output")
			next
		end
		
		if File.file?( f )
			s += 'File "'
			s += File.expand_path( f )
			s += '"'
			s+="\n"
		else
			String tmp = f[9, s.length-1]
			s.concat( out_path )
			s += "\\"
			s.concat( tmp )
			s += "\n"
		end
	end


	i = 0
	s.each_byte { |c|
		if c == 47
			s[i] = "\\"
		end
		i += 1
	}

	file = File.new("files.lst", "w+")
	file.puts s

end
